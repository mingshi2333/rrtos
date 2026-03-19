#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_pwmg.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_pwmg_task_tcb;
static uint8_t g_pwmg_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void pwmg_outputcompare_task(void *arg)
{
    uint32_t tick = 0;
    uint32_t matches = 0;
    uint16_t counter = 0;
    uint16_t compare = 0;
    int rc;
    int ccie;
    int compare_rc;
    int disable_rc = -1;
    int match;
    int interrupt_disabled = 0;

    (void)arg;

    while (1) {
        rc = hal_pwmg_get_counter(&counter);
        compare_rc = hal_pwmg_get_compare(&compare);
        ccie = hal_pwmg_compare_interrupt_enabled();
        match = hal_pwmg_compare_match_pending();
        if (match > 0) {
            (void)hal_pwmg_clear_compare_match();
            ++matches;
            if (interrupt_disabled == 0) {
                disable_rc = hal_pwmg_disable_compare_interrupt();
                interrupt_disabled = disable_rc == 0;
                ccie = hal_pwmg_compare_interrupt_enabled();
                os_print("[PWM_APP] match-disable rc=%d after-match=%u\n",
                         disable_rc,
                         matches);
            }
        }
        os_print("[PWM_APP] tick %u rc=%d ccr-rc=%d counter=%u compare=%u match=%d ccie=%d matches=%u\n",
                 tick++,
                 rc,
                 compare_rc,
                 (uint32_t)counter,
                 (uint32_t)compare,
                 match > 0 ? 1 : 0,
                 ccie > 0 ? 1 : 0,
                 matches);
        busy_wait_cycles(100000u);
    }
}

void os_kernel_main(void)
{
    hal_pwmg_config_t cfg;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[PWM_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    cfg.prescaler = 9999u;
    cfg.autoreload = 5u;
    cfg.compare = 2u;
    cfg.clock_division = HAL_PWMG_CLOCK_DIV_1;
    cfg.oc_mode = HAL_PWMG_OC_PWM1;

    rc = hal_pwmg_init(BE_U1000_PWMG0_BASE, &cfg);
    os_print("[PWM_APP] pwmg_outputcompare ready base=0x%x psc=%u arr=%u ccr=%u init-rc=%d\n",
             (uint32_t)BE_U1000_PWMG0_BASE,
             (uint32_t)cfg.prescaler,
             (uint32_t)cfg.autoreload,
             (uint32_t)cfg.compare,
             rc);
    rc = hal_pwmg_enable_compare_interrupt();
    os_print("[PWM_APP] match-enable rc=%d\n", rc);
    rc = hal_pwmg_enable_counter();
    os_print("[PWM_APP] counter-enable rc=%d\n", rc);
    os_print("[PWM_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_pwmg_task_tcb,
                   "pwmg_outputcompare",
                   pwmg_outputcompare_task,
                   0,
                   10,
                   g_pwmg_task_stack,
                   sizeof(g_pwmg_task_stack));

    os_print("[PWM_APP] Starting scheduler...\n");
    os_kernel_start();
}
