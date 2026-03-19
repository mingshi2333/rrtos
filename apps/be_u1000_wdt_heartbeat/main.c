#include <stdbool.h>
#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_uart.h"
#include "../../hal/include/hal_wdt.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_wdt_task_tcb;
static uint8_t g_wdt_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void wdt_heartbeat_task(void *arg)
{
    uint32_t tick = 0;
    uint32_t counter = 0;
    bool irq_active = false;

    (void)arg;

    while (1) {
        (void)hal_wdt_reload();
        (void)hal_wdt_get_counter(&counter);
        (void)hal_wdt_irq_active(&irq_active);
        os_print("[WDT_APP] tick %u counter=%u irq=%u\n",
                 tick++,
                 counter,
                 irq_active ? 1u : 0u);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    hal_wdt_config_t cfg;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[WDT_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    cfg.mode = HAL_WDT_MODE_INTERRUPT;
    cfg.reset_len = HAL_WDT_RESET_32;
    cfg.timeout = HAL_WDT_TIMEOUT_9;

    rc = hal_wdt_init(BE_U1000_WDT0_BASE, &cfg);
    os_print("[WDT_APP] wdt_heartbeat ready base=0x%x mode=%u timeout=%u init-rc=%d\n",
             (uint32_t)BE_U1000_WDT0_BASE,
             (uint32_t)cfg.mode,
             (uint32_t)cfg.timeout,
             rc);
    os_print("[WDT_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_wdt_task_tcb,
                   "wdt_heartbeat",
                   wdt_heartbeat_task,
                   0,
                   10,
                   g_wdt_task_stack,
                   sizeof(g_wdt_task_stack));

    os_print("[WDT_APP] Starting scheduler...\n");
    os_kernel_start();
}
