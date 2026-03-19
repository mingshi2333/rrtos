#include <stdbool.h>
#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_gpio.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_led_task_tcb;
static uint8_t g_led_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void gpio_ledblink_task(void *arg)
{
    bool led_on = false;
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        led_on = !led_on;
        (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, led_on);
        os_print("[GPIO_APP] blink tick %u led=%u button=%u\n",
                 tick++,
                 led_on ? 1u : 0u,
                 hal_gpio_read(BE_U1000_USER_BTN_GPIO_PIN) ? 1u : 0u);
        busy_wait_cycles(200000u);
    }
}

void os_kernel_main(void)
{
    hal_board_init();
    hal_board_print_banner();

    os_print("[GPIO_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    hal_gpio_init(BE_U1000_USER_LED_GPIO_BASE);
    if (hal_gpio_set_direction(BE_U1000_USER_LED_GPIO_PIN, true) != 0) {
        os_print("[GPIO_APP] led direction: FAIL\n");
        while (1) {
        }
    }
    if (hal_gpio_set_direction(BE_U1000_USER_BTN_GPIO_PIN, false) != 0) {
        os_print("[GPIO_APP] button direction: FAIL\n");
        while (1) {
        }
    }
    (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, false);

    os_print("[GPIO_APP] ledblink ready base=0x%x pin=%u button=%u\n",
             (uint32_t)BE_U1000_USER_LED_GPIO_BASE,
             (uint32_t)BE_U1000_USER_LED_GPIO_PIN,
             (uint32_t)BE_U1000_USER_BTN_GPIO_PIN);

    os_print("[GPIO_APP] Initializing kernel...\n");
    os_kernel_init();

    os_task_create(&g_led_task_tcb,
                   "gpio_ledblink",
                   gpio_ledblink_task,
                   0,
                   10,
                   g_led_task_stack,
                   sizeof(g_led_task_stack));

    os_print("[GPIO_APP] Starting scheduler...\n");
    os_kernel_start();
}
