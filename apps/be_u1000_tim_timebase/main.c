#include <stdbool.h>
#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_gpio.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_tim_task_tcb;
static uint8_t g_tim_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void tim_timebase_task(void *arg)
{
    bool led_on = false;
    uint64_t last = hal_clint_mtime_get();
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        busy_wait_cycles(250000u);
        led_on = !led_on;
        (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, led_on);

        {
            uint64_t now = hal_clint_mtime_get();
            os_print("[TIM_APP] tick %u delta=%u led=%u\n",
                     tick++,
                     (uint32_t)(now - last),
                     led_on ? 1u : 0u);
            last = now;
        }
    }
}

void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[TIM_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    hal_gpio_init(BE_U1000_USER_LED_GPIO_BASE);
    if (hal_gpio_set_direction(BE_U1000_USER_LED_GPIO_PIN, true) != 0) {
        os_print("[TIM_APP] led direction: FAIL\n");
        while (1) {
        }
    }
    (void)hal_gpio_write(BE_U1000_USER_LED_GPIO_PIN, false);

    os_print("[TIM_APP] tim_timebase ready source=CLINT freq=%u tim0-base=0x%x\n",
             (uint32_t)BE_U1000_TIMER_FREQ,
             (uint32_t)BE_U1000_TIM0_BASE);
    os_print("[TIM_APP] Initializing kernel...\n");

    os_kernel_init();
    rc = os_task_create(&g_tim_task_tcb,
                        "tim_timebase",
                        tim_timebase_task,
                        0,
                        10,
                        g_tim_task_stack,
                        sizeof(g_tim_task_stack));
    if (rc != OS_EOK) {
        os_print("[TIM_APP] task create failed rc=%d\n", (int)rc);
        return;
    }

    os_print("[TIM_APP] Starting scheduler...\n");
    os_kernel_start();
}
