#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_i2s.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_i2s_task_tcb;
static uint8_t g_i2s_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void i2s_tx_task(void *arg)
{
    uint32_t tick = 0;
    uint32_t sample = 0x00010002u;
    uint32_t word;
    hal_i2s_tx_state_t pre_state;
    hal_i2s_tx_state_t post_state;
    int rc;
    const char *action;

    (void)arg;

    while (1) {
        word = sample + tick;
        action = "steady";
        rc = hal_i2s_get_tx_state(&pre_state);
        (void)hal_i2s_transmit_word(word);
        if ((tick & 0x7u) == 0x7u) {
            (void)hal_i2s_flush_tx_fifo();
            action = "flush";
        }
        if (pre_state.overrun) {
            (void)hal_i2s_clear_tx_overrun();
            action = "clear-overrun";
        }
        if (rc != 0) {
            os_print("[I2S_APP] tick %u state-error rc=%d sample=0x%x\n",
                     tick++,
                     rc,
                     word);
            busy_wait_cycles(250000u);
            continue;
        }

        (void)hal_i2s_get_tx_state(&post_state);

        os_print("[I2S_APP] tick %u en=%u dma=%u ready=%u overrun=%u level=%u threshold=%u action=%s post=%u sample=0x%x\n",
                 tick++,
                 pre_state.enabled ? 1u : 0u,
                 pre_state.dma_enabled ? 1u : 0u,
                 pre_state.ready ? 1u : 0u,
                 pre_state.overrun ? 1u : 0u,
                 pre_state.fifo_level,
                 pre_state.fifo_threshold,
                 action,
                 post_state.fifo_level,
                 word);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    hal_i2s_config_t cfg;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[I2S_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    cfg.mode = HAL_I2S_MODE_SLAVE_TX;
    cfg.resolution = HAL_I2S_RES_16;
    cfg.fifo_level = HAL_I2S_FIFO_LVL3;
    cfg.dma_transfer = false;

    rc = hal_i2s_init(BE_U1000_I2S1_BASE, &cfg);
    os_print("[I2S_APP] i2s_tx ready base=0x%x mode=%u res=%u dma=%u init-rc=%d\n",
             (uint32_t)BE_U1000_I2S1_BASE,
             (uint32_t)cfg.mode,
             (uint32_t)cfg.resolution,
             cfg.dma_transfer ? 1u : 0u,
             rc);
    rc = hal_i2s_enable_tx();
    os_print("[I2S_APP] tx-enable rc=%d\n", rc);
    os_print("[I2S_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_i2s_task_tcb,
                   "i2s_tx",
                   i2s_tx_task,
                   0,
                   10,
                   g_i2s_task_stack,
                   sizeof(g_i2s_task_stack));

    os_print("[I2S_APP] Starting scheduler...\n");
    os_kernel_start();
}
