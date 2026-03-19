#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_pwma.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_pwma_task_tcb;
static uint8_t g_pwma_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void pwma_timebase_task(void *arg)
{
    uint32_t tick = 0;
    uint32_t updates = 0;
    uint16_t counter = 0;
    uint8_t repetition_counter = 0;
    int rc;
    int repetition_rc;
    int update;
    int uie;
    int disable_rc = -1;
    int repetition_set_rc = -1;
    int interrupt_disabled = 0;
    int repetition_reprogrammed = 0;

    (void)arg;

    while (1) {
        rc = hal_pwma_get_counter(&counter);
        repetition_rc = hal_pwma_get_repetition_counter(&repetition_counter);
        update = hal_pwma_update_pending();
        uie = hal_pwma_update_interrupt_enabled();
        if (update > 0) {
            (void)hal_pwma_clear_update();
            ++updates;
            if (interrupt_disabled == 0) {
                disable_rc = hal_pwma_disable_update_interrupt();
                interrupt_disabled = disable_rc == 0;
                uie = hal_pwma_update_interrupt_enabled();
                os_print("[PWMA_APP] update-disable rc=%d after-update=%u\n",
                         disable_rc,
                         updates);
            }
            if (repetition_reprogrammed == 0) {
                repetition_set_rc = hal_pwma_set_repetition_counter(1u);
                repetition_reprogrammed = repetition_set_rc == 0;
                repetition_rc = hal_pwma_get_repetition_counter(&repetition_counter);
                os_print("[PWMA_APP] repetition-set rc=%d after-update=%u rcr=%u\n",
                         repetition_set_rc,
                         updates,
                         (uint32_t)repetition_counter);
            }
        }
        os_print("[PWMA_APP] tick %u rc=%d rcr-rc=%d counter=%u rcr=%u uif=%d uie=%d updates=%u\n",
                 tick++,
                 rc,
                 repetition_rc,
                 (uint32_t)counter,
                 (uint32_t)repetition_counter,
                 update > 0 ? 1 : 0,
                 uie > 0 ? 1 : 0,
                 updates);
        busy_wait_cycles(100000u);
    }
}

void os_kernel_main(void)
{
    hal_pwma_config_t cfg;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[PWMA_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    cfg.prescaler = 9999u;
    cfg.autoreload = 5u;
    cfg.counter_mode = HAL_PWMA_COUNTER_UP;
    cfg.clock_division = HAL_PWMA_CLOCK_DIV_1;
    cfg.repetition_counter = 0u;

    rc = hal_pwma_init(BE_U1000_PWMA2_BASE, &cfg);
    os_print("[PWMA_APP] pwma_timebase ready base=0x%x psc=%u arr=%u init-rc=%d\n",
             (uint32_t)BE_U1000_PWMA2_BASE,
             (uint32_t)cfg.prescaler,
             (uint32_t)cfg.autoreload,
             rc);
    rc = hal_pwma_enable_update_interrupt();
    os_print("[PWMA_APP] irq-enable rc=%d\n", rc);
    rc = hal_pwma_enable_counter();
    os_print("[PWMA_APP] counter-enable rc=%d\n", rc);
    os_print("[PWMA_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_pwma_task_tcb,
                   "pwma_timebase",
                   pwma_timebase_task,
                   0,
                   10,
                   g_pwma_task_stack,
                   sizeof(g_pwma_task_stack));

    os_print("[PWMA_APP] Starting scheduler...\n");
    os_kernel_start();
}
