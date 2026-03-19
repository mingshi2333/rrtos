#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_adc.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_adc_task_tcb;
static uint8_t g_adc_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void adc_single_task(void *arg)
{
    uint32_t tick = 0;
    uint16_t value = 0;
    int rc;

    (void)arg;

    while (1) {
        rc = hal_adc_convert_single(&value, 256u);
        os_print("[ADC_APP] tick %u rc=%d value=%u\n", tick++, rc, (uint32_t)value);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    hal_adc_config_t adc_cfg;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[ADC_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    adc_cfg.channel = HAL_ADC_CHANNEL_0;
    adc_cfg.sample_time = HAL_ADC_SAMPLE_275_CYCLES;
    adc_cfg.clock_division = 4u;
    adc_cfg.signed_output = false;
    adc_cfg.differential_input = false;

    rc = hal_adc_init(BE_U1000_ADC0_BASE, &adc_cfg);
    os_print("[ADC_APP] adc_single ready base=0x%x ch=%u sample=%u div=%u init-rc=%d\n",
             (uint32_t)BE_U1000_ADC0_BASE,
             (uint32_t)adc_cfg.channel,
             (uint32_t)adc_cfg.sample_time,
             (uint32_t)adc_cfg.clock_division,
             rc);
    os_print("[ADC_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_adc_task_tcb,
                   "adc_single",
                   adc_single_task,
                   0,
                   10,
                   g_adc_task_stack,
                   sizeof(g_adc_task_stack));

    os_print("[ADC_APP] Starting scheduler...\n");
    os_kernel_start();
}
