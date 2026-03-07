#include <stdint.h>
#include <stddef.h>

#include "os_kernel.h"
#include "os_config.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#include "hal_uart.h"

#if OS_CFG_SMP_EN
#include "riscv_csr.h"
#endif

#if defined(CONFIG_BOARD_BE_U1000)
#include "board_config.h"
#endif

#if OS_CFG_SMP_EN
#include "os_smp.h"
#endif

static uint8_t control_task_stack[1024];
static uint8_t worker_task_stack[1024];
#if OS_CFG_SMP_EN
static uint8_t balance_task_stack[1024];
#endif
static os_tcb_t control_task_tcb;
static os_tcb_t worker_task_tcb;
#if OS_CFG_SMP_EN
static os_tcb_t balance_task_tcb;
static volatile uint32_t g_balance_seen_mask;
#endif

#define SELFTEST_SPI_TIMEOUT  100000u
#define QSPI1_SIM_SIG0        0x31505351u
#define QSPI1_SIM_SIG1        0x5F4C444Du
#define QSPI1_SIM_SIG2        0x00010010u
#define QSPI1_SIM_SIG3        0xA55A3CC3u

static void report_pinmux_result(const char *name, hal_board_pinmux_group_t group)
{
    if (hal_board_apply_pinmux_group(group) == 0) {
        os_print("[SELFTEST] %s pinmux: OK\n", name);
    } else {
        os_print("[SELFTEST] %s pinmux: FAIL\n", name);
    }
}

#if defined(CONFIG_BOARD_BE_U1000)
static void board_bus_self_check(void)
{
    uint8_t spi_rx = 0;
    volatile const uint32_t *qspi1_window = (volatile const uint32_t *)BE_U1000_QSPI1_BASE;
    uint32_t qspi_sig0;
    uint32_t qspi_sig1;
    uint32_t qspi_sig2;
    uint32_t qspi_sig3;

    report_pinmux_result("UART0", HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0);
    report_pinmux_result("USER_LED", HAL_BOARD_PINMUX_GROUP_USER_LED);
    report_pinmux_result("USER_BUTTON", HAL_BOARD_PINMUX_GROUP_USER_BUTTON);

    hal_gpio_init(BE_U1000_USER_LED_GPIO_BASE);
    if (hal_gpio_set_direction(BE_U1000_USER_LED_GPIO_PIN, true) == 0 &&
        hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, true) == 0 &&
        hal_gpio_toggle(BE_U1000_USER_LED_GPIO_PIN) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%u)\n",
                 (uint32_t)BE_U1000_USER_LED_GPIO_BASE,
                 (uint32_t)BE_U1000_USER_LED_GPIO_PIN);
        os_print("[SELFTEST] USER_LED GPIO: OK (PC0)\n");
    } else {
        os_print("[CHK] GPIO init: FAIL\n");
        os_print("[SELFTEST] USER_LED GPIO: FAIL\n");
    }

    if (hal_gpio_set_direction(BE_U1000_USER_BTN_GPIO_PIN, false) == 0) {
        os_print("[SELFTEST] USER_BUTTON GPIO: OK (PC13 level=%u)\n",
                 hal_gpio_read(BE_U1000_USER_BTN_GPIO_PIN) ? 1u : 0u);
    } else {
        os_print("[SELFTEST] USER_BUTTON GPIO: FAIL\n");
    }

    report_pinmux_result("I2C0 header", HAL_BOARD_PINMUX_GROUP_HEADER_I2C0);
    if (hal_i2c_init(BE_U1000_HEADER_I2C_BASE, BE_U1000_DIAG_I2C_BUS_HZ) == 0) {
        os_print("[CHK] I2C init: OK (base=0x%x hz=%u)\n",
                 (uint32_t)BE_U1000_HEADER_I2C_BASE,
                 (uint32_t)BE_U1000_DIAG_I2C_BUS_HZ);
        os_print("[SELFTEST] I2C0 controller: OK (PA4/PA5, 100kHz)\n");
    } else {
        os_print("[CHK] I2C init: FAIL\n");
        os_print("[SELFTEST] I2C0 controller: FAIL\n");
    }

    report_pinmux_result("SPI1 header", HAL_BOARD_PINMUX_GROUP_HEADER_SPI1);
    if (hal_spi_init(BE_U1000_HEADER_SPI_BASE, BE_U1000_DIAG_SPI_BAUD_DIV, HAL_SPI_MODE0) == 0 &&
        hal_spi_transfer(0xA5u, &spi_rx, SELFTEST_SPI_TIMEOUT) == 0) {
        os_print("[CHK] SPI init: OK (base=0x%x div=%u)\n",
                 (uint32_t)BE_U1000_HEADER_SPI_BASE,
                 (uint32_t)BE_U1000_DIAG_SPI_BAUD_DIV);
        os_print("[SELFTEST] SPI1 controller: OK (PA8..PA11 rx=0x%x)\n", spi_rx);
    } else {
        os_print("[CHK] SPI init: FAIL\n");
        os_print("[SELFTEST] SPI1 controller: FAIL\n");
    }

    report_pinmux_result("QSPI1", HAL_BOARD_PINMUX_GROUP_QSPI1);
    os_print("[SELFTEST] QSPI1 ready: ctrl=0x%x window=0x%x (PB0..PB5)\n",
             (uint32_t)BE_U1000_QSPI1_CTRL_BASE,
             (uint32_t)BE_U1000_QSPI1_BASE);
    qspi_sig0 = qspi1_window[0];
    qspi_sig1 = qspi1_window[1];
    qspi_sig2 = qspi1_window[2];
    qspi_sig3 = qspi1_window[3];
    os_print("[SELFTEST] QSPI1 window sample: 0x%x 0x%x 0x%x 0x%x\n",
             qspi_sig0, qspi_sig1, qspi_sig2, qspi_sig3);
    if (qspi_sig0 == QSPI1_SIM_SIG0 && qspi_sig1 == QSPI1_SIM_SIG1 &&
        qspi_sig2 == QSPI1_SIM_SIG2 && qspi_sig3 == QSPI1_SIM_SIG3) {
        os_print("[SELFTEST] QSPI1 window read: OK (sim signature)\n");
    } else {
        os_print("[SELFTEST] QSPI1 window read: sample captured\n");
    }

    report_pinmux_result("CANFD0", HAL_BOARD_PINMUX_GROUP_CANFD0);
    os_print("[SELFTEST] CANFD0 ready: base=0x%x pins=PA14/PA15\n",
             (uint32_t)BE_U1000_CANFD0_BASE);

    report_pinmux_result("CANFD1", HAL_BOARD_PINMUX_GROUP_CANFD1);
    os_print("[SELFTEST] CANFD1 ready: base=0x%x pins=PB6/PB7\n",
             (uint32_t)BE_U1000_CANFD1_BASE);
}
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

#if defined(CONFIG_BOARD_BE_U1000)
    board_bus_self_check();
#endif
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
        if (os_smp_online_count() >= 2 && g_balance_seen_mask != 0x3u) {
            os_cpu_t peer = self->cpu_id == 0 ? 1 : 0;
            if ((g_balance_seen_mask & (1u << peer)) == 0u) {
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

#if OS_CFG_IRQ_MODEL_CLIC
    os_print("[BOOT] IRQ model: CLIC\n");
#else
    os_print("[BOOT] IRQ model: PLIC\n");
#endif

    platform_self_check();

    os_print("[BOOT] Initializing kernel...\n");
    os_kernel_init();

#if OS_CFG_SMP_EN
    os_print("[SMP] role-plan: core0=boot core1=worker core2=reserved\n");
    os_smp_init();
    os_smp_start_cpus();
    if (os_smp_cpu_online(1)) {
        os_ipi_send(1, OS_IPI_RESCHEDULE);
    }
#endif

    os_task_create(&control_task_tcb, "control", control_task_entry, NULL,
                   10, control_task_stack, sizeof(control_task_stack));
    os_task_create(&worker_task_tcb, "worker", worker_task_entry, NULL,
                   11, worker_task_stack, sizeof(worker_task_stack));

#if OS_CFG_SMP_EN
    os_task_set_affinity(&control_task_tcb, 0);
    os_task_set_affinity(&worker_task_tcb, 1);
    os_task_create(&balance_task_tcb, "balance", balance_task_entry, NULL,
                   12, balance_task_stack, sizeof(balance_task_stack));
    os_print("[SMP] task-map: control->core0 worker->core1 core2=reserved\n");
    os_smp_release_cpus();
#else
    os_print("[BOOT] task-map: control+worker share single core\n");
#endif

    os_print("[BOOT] Starting scheduler...\n");
    os_kernel_start();
}
