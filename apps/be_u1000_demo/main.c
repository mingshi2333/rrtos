#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "os_kernel.h"
#include "os_config.h"
#include "hal_board.h"
#if OS_CFG_SMP_EN
#include "hal_board_smp.h"
#endif
#include "hal_clint.h"
#include "hal_uart.h"

#if OS_CFG_SMP_EN
#include "riscv_csr.h"
#include "os_smp.h"
#endif

static uint8_t control_task_stack[1024];
static uint8_t worker_task_stack[1024];
static hal_board_execution_profile_t g_execution_profile;
#if OS_CFG_SMP_EN
static uint8_t balance_task_stack[1024];
#endif
static os_tcb_t control_task_tcb;
static os_tcb_t worker_task_tcb;
#if OS_CFG_SMP_EN
static os_tcb_t balance_task_tcb;
static volatile uint32_t g_balance_seen_mask;
static uint32_t g_balance_expected_mask;
#endif

static void platform_self_check(void) {
    uint64_t t0;
    uint64_t t1;
    volatile uint32_t i;

    t0 = hal_clint_mtime_get();
    for (i = 0; i < 50000; i++) {
    }
    t1 = hal_clint_mtime_get();

    if (t1 > t0) {
        os_print("[CHK] CLINT mtime monotonic: OK (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    } else {
        os_print("[CHK] CLINT mtime monotonic: FAIL (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    }

    if (hal_board_run_selftest() != 0) {
        os_print("[CHK] board selftest profile: SKIP\n");
    }
}

static void control_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
#if OS_CFG_SMP_EN
        os_tcb_t *self = os_task_self();
        os_print("[AFFINITY] control on Core%u affinity Core%u tick %u\n",
                 self->cpu_id, self->affinity, count++);
#else
        os_print("[CTRL] single-core tick %u\n", count++);
#endif
        os_task_delay(1000);
    }
}

static void worker_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
#if OS_CFG_SMP_EN
        os_tcb_t *self = os_task_self();
        os_print("[AFFINITY] worker on Core%u affinity Core%u tick %u\n",
                 self->cpu_id, self->affinity, count++);
#else
        os_print("[WORK] single-core tick %u\n", count++);
#endif
        os_task_delay(2000);
    }
}

#if OS_CFG_SMP_EN
static void balance_remote_marker(void *unused)
{
    (void)unused;
    g_balance_seen_mask |= 1u << os_cpu_id();
    os_print("[BALANCE] balance on Core%u affinity any bootstrap seen-mask=0x%x\n",
             os_cpu_id(), g_balance_seen_mask);
}

static void balance_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
        os_tcb_t *self = os_task_self();
        g_balance_seen_mask |= 1u << self->cpu_id;
        if (os_smp_online_count() >= 2 && g_balance_seen_mask != g_balance_expected_mask) {
            os_cpu_t peer;

            if (hal_board_get_balance_peer(self->cpu_id, &peer) &&
                (g_balance_seen_mask & (1u << peer)) == 0u) {
                (void)os_smp_call_on_cpu(peer, balance_remote_marker, NULL, true);
            }
        }
        os_print("[BALANCE] balance on Core%u affinity any tick %u seen-mask=0x%x\n",
                 self->cpu_id, count++, g_balance_seen_mask);
        os_task_delay(500);
    }
}
#endif

void os_kernel_main(void) {
    hal_board_init();
    hal_board_print_banner();
    os_print("[BOOT] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());
    hal_board_get_execution_profile(&g_execution_profile);

#if OS_CFG_IRQ_MODEL_CLIC
    os_print("[BOOT] IRQ model: CLIC\n");
#else
    os_print("[BOOT] IRQ model: PLIC\n");
#endif

    platform_self_check();

    os_print("[BOOT] Initializing kernel...\n");
    os_kernel_init();

#if OS_CFG_SMP_EN
    if (g_execution_profile.available && g_execution_profile.role_plan) {
        os_print("[SMP] role-plan: %s\n", g_execution_profile.role_plan);
    }
    os_smp_init();
    os_smp_start_cpus();
    (void)hal_board_issue_reschedule_probe();
#endif

    os_task_create(&control_task_tcb, "control", control_task_entry, NULL,
                   10, control_task_stack, sizeof(control_task_stack));
    os_task_create(&worker_task_tcb, "worker", worker_task_entry, NULL,
                   11, worker_task_stack, sizeof(worker_task_stack));

#if OS_CFG_SMP_EN
    (void)hal_board_bind_demo_tasks(&control_task_tcb, &worker_task_tcb);
    g_balance_expected_mask = hal_board_balance_expected_mask();
    os_task_create(&balance_task_tcb, "balance", balance_task_entry, NULL,
                   12, balance_task_stack, sizeof(balance_task_stack));
    if (g_execution_profile.available) {
        const char *task_map_description = hal_board_task_map_description(true);

        if (task_map_description != NULL) {
            os_print("[SMP] task-map: %s\n", task_map_description);
        }
    }
    os_smp_release_cpus();
#else
    if (g_execution_profile.available) {
        const char *task_map_description = hal_board_task_map_description(false);

        if (task_map_description != NULL) {
            os_print("[BOOT] task-map: %s\n", task_map_description);
        }
    }
#endif

    os_print("[BOOT] Starting scheduler...\n");
    os_kernel_start();
}
