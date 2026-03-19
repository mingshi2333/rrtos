#include <stdint.h>

#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_uart_task_tcb;
static uint8_t g_uart_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void uart_printf_task(void *arg)
{
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        os_print("[UART_APP] UART printf example tick %u\n", tick++);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    hal_board_init();
    hal_board_print_banner();

    os_print("[UART_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());
    os_print("[UART_APP] uart_printf ready baud=%u\n", 115200u);
    os_print("[UART_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_uart_task_tcb,
                   "uart_printf",
                   uart_printf_task,
                   0,
                   10,
                   g_uart_task_stack,
                   sizeof(g_uart_task_stack));

    os_print("[UART_APP] Starting scheduler...\n");
    os_kernel_start();
}
