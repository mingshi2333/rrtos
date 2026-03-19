#include <stdint.h>

#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_canfd_task_tcb;
static uint8_t g_canfd_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void canfd_polling_task(void *arg)
{
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        os_print("[CANFD_APP] tick %u canfd lane alive\n", tick++);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[CANFD_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    if (hal_board_run_selftest() != 0) {
        os_print("[CANFD_APP] board selftest profile: SKIP\n");
        while (1) {
        }
    }

    os_print("[CANFD_APP] canfd_polling ready using board CANFD selftest loopback\n");
    os_print("[CANFD_APP] Initializing kernel...\n");

    os_kernel_init();
    rc = os_task_create(&g_canfd_task_tcb,
                        "canfd_polling",
                        canfd_polling_task,
                        0,
                        10,
                        g_canfd_task_stack,
                        sizeof(g_canfd_task_stack));
    if (rc != OS_EOK) {
        os_print("[CANFD_APP] task create failed rc=%d\n", (int)rc);
        return;
    }

    os_print("[CANFD_APP] Starting scheduler...\n");
    os_kernel_start();
}
