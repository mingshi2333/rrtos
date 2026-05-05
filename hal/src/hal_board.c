#include <stdbool.h>
#include <stdint.h>

#include "hal_board_smp.h"
#include "hal_board_be_u1000_internal.h"
#include "hal_clint.h"
#include "hal_irq.h"
#include "hal_uart.h"
#include "os_config.h"
#include "os_kernel.h"

#ifndef RRTOS_HAL_HAS_BOARD_SELFTEST
#define RRTOS_HAL_HAS_BOARD_SELFTEST 0
#endif

#if OS_CFG_SMP_EN
#include "os_smp.h"
#endif

typedef struct {
    os_cpu_t control_cpu;
    os_cpu_t worker_cpu;
    os_cpu_t reschedule_probe_cpu;
    os_cpu_t balance_probe_cpu_a;
    os_cpu_t balance_probe_cpu_b;
    bool available;
} hal_board_demo_topology_t;

#if OS_CFG_SMP_EN
static void hal_board_fill_demo_topology(hal_board_demo_topology_t *topology)
{
    if (!topology) {
        return;
    }

    topology->control_cpu = 0u;
    topology->worker_cpu = 0u;
    topology->reschedule_probe_cpu = 0u;
    topology->balance_probe_cpu_a = 0u;
    topology->balance_probe_cpu_b = 0u;
    topology->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    topology->worker_cpu = 1u;
    topology->reschedule_probe_cpu = 1u;
    topology->balance_probe_cpu_a = 0u;
    topology->balance_probe_cpu_b = 1u;
    topology->available = true;
#endif
}
#endif

void hal_board_init(void)
{
#if defined(CONFIG_BOARD_BE_U1000)
    hal_board_be_u1000_pinmux_init();
#endif

    hal_uart_init(OS_CFG_UART_BASE, OS_CFG_UART_BAUD);
    hal_clint_init(OS_CFG_CLINT_BASE);

#if OS_CFG_IRQ_MODEL_CLIC
    hal_irq_init_clic(BE_U1000_CLIC_BASE);
#else
    hal_irq_init_plic(OS_CFG_PLIC_BASE);
#endif
}

const char *hal_board_name(void)
{
#if defined(CONFIG_BOARD_BE_U1000)
    return "BE-U1000/EVU-BA";
#else
    return "qemu_virt";
#endif
}

void hal_board_print_banner(void)
{
#if defined(CONFIG_BOARD_BE_U1000)
    os_print("\n========================================\n");
    os_print("  RV-AIOS v1.0.0 on BE-U1000\n");
    os_print("  Baikal Electronics RISC-V RV32IMAFC\n");
    os_print("========================================\n\n");
#else
    os_print("[BOOT] Board: %s\n", hal_board_name());
#endif
}

void hal_board_get_execution_profile(hal_board_execution_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->schedulable_cores = 0;
    profile->reserved_helper_mask = 0;
    profile->role_plan = NULL;
    profile->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    profile->schedulable_cores = 2u;
    profile->reserved_helper_mask = 1u << 2;
    profile->role_plan = "core0=boot core1=worker core2=reserved";
    profile->available = true;
#endif
}

const char *hal_board_task_map_description(bool smp_mode)
{
#if defined(CONFIG_BOARD_BE_U1000)
    return smp_mode
               ? "control->core0 worker->core1 core2=reserved"
               : "control+worker share single core";
#else
    (void)smp_mode;
    return NULL;
#endif
}

#if OS_CFG_SMP_EN
os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available || !control_task || !worker_task) {
        return OS_EINVAL;
    }

    if (os_task_set_affinity(control_task, topology.control_cpu) != OS_EOK) {
        return OS_ERROR;
    }
    if (os_task_set_affinity(worker_task, topology.worker_cpu) != OS_EOK) {
        return OS_ERROR;
    }
    return OS_EOK;
}

bool hal_board_issue_reschedule_probe(void)
{
#if !OS_CFG_SMP_RUNTIME_IPI_EN
    return false;
#else
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available ||
        topology.reschedule_probe_cpu >= os_cpu_count() ||
        !os_smp_cpu_online(topology.reschedule_probe_cpu)) {
        return false;
    }

    os_ipi_send(topology.reschedule_probe_cpu, OS_IPI_RESCHEDULE);
    return true;
#endif
}

bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available || !peer_cpu) {
        return false;
    }

    if (current_cpu == topology.balance_probe_cpu_a) {
        *peer_cpu = topology.balance_probe_cpu_b;
        return true;
    }
    if (current_cpu == topology.balance_probe_cpu_b) {
        *peer_cpu = topology.balance_probe_cpu_a;
        return true;
    }
    return false;
}

uint32_t hal_board_balance_expected_mask(void)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available) {
        return 0u;
    }

    return (1u << topology.balance_probe_cpu_a) |
           (1u << topology.balance_probe_cpu_b);
}
#endif

const char *hal_board_pinmux_group_name(hal_board_pinmux_group_t group)
{
    switch (group) {
    case HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0:
        return "UART0";
    case HAL_BOARD_PINMUX_GROUP_USER_LED:
        return "USER_LED";
    case HAL_BOARD_PINMUX_GROUP_USER_BUTTON:
        return "USER_BUTTON";
    case HAL_BOARD_PINMUX_GROUP_HEADER_I2C0:
        return "I2C0 header";
    case HAL_BOARD_PINMUX_GROUP_HEADER_SPI1:
        return "SPI1 header";
    case HAL_BOARD_PINMUX_GROUP_QSPI1:
        return "QSPI1";
    case HAL_BOARD_PINMUX_GROUP_CANFD0:
        return "CANFD0";
    case HAL_BOARD_PINMUX_GROUP_CANFD1:
        return "CANFD1";
    default:
        return "unknown";
    }
}

int hal_board_apply_pinmux_group(hal_board_pinmux_group_t group)
{
#if defined(CONFIG_BOARD_BE_U1000)
    return hal_board_be_u1000_apply_pinmux_group(group);
#else
    (void)group;
    return -1;
#endif
}

#if !RRTOS_HAL_HAS_BOARD_SELFTEST
int hal_board_run_selftest(void)
{
    return -1;
}
#endif
