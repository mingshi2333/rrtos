#include <stdbool.h>
#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_gpio.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_poll_task_tcb;
static uint8_t g_poll_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void gpio_inputpolling_task(void *arg)
{
    bool led_on;
    bool button_high;
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        button_high = hal_gpio_read(BE_U1000_USER_BTN_GPIO_PIN);
        led_on = !button_high;
        (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, led_on);
        os_print("[GPIO_POLL] tick %u button=%u led=%u\n",
                 tick++,
                 button_high ? 1u : 0u,
                 led_on ? 1u : 0u);
        busy_wait_cycles(200000u);
    }
}

void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[GPIO_POLL] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    hal_gpio_init(BE_U1000_USER_LED_GPIO_BASE);
    if (hal_gpio_set_direction(BE_U1000_USER_LED_GPIO_PIN, true) != 0) {
        os_print("[GPIO_POLL] led direction: FAIL\n");
        while (1) {
        }
    }
    if (hal_gpio_set_direction(BE_U1000_USER_BTN_GPIO_PIN, false) != 0) {
        os_print("[GPIO_POLL] button direction: FAIL\n");
        while (1) {
        }
    }

    (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, false);

    os_print("[GPIO_POLL] inputpolling ready led-base=0x%x led-pin=%u button-pin=%u\n",
             (uint32_t)BE_U1000_USER_LED_GPIO_BASE,
             (uint32_t)BE_U1000_USER_LED_GPIO_PIN,
             (uint32_t)BE_U1000_USER_BTN_GPIO_PIN);

    os_print("[GPIO_POLL] Initializing kernel...\n");
    os_kernel_init();

    rc = os_task_create(&g_poll_task_tcb,
                        "gpio_inputpoll",
                        gpio_inputpolling_task,
                        0,
                        10,
                        g_poll_task_stack,
                        sizeof(g_poll_task_stack));
    if (rc != OS_EOK) {
        os_print("[GPIO_POLL] task create failed rc=%d\n", (int)rc);
        return;
    }

    os_print("[GPIO_POLL] Starting scheduler...\n");
    os_kernel_start();
}
