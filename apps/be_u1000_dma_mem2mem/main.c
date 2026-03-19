#include <stdint.h>
#include <string.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_dma.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

#define DMA_BUFFER_LENGTH 64u

static os_tcb_t g_dma_task_tcb;
static uint8_t g_dma_task_stack[768];
static uint8_t g_dma_tx[DMA_BUFFER_LENGTH] __attribute__((aligned(4)));
static uint8_t g_dma_rx[DMA_BUFFER_LENGTH] __attribute__((aligned(4)));

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static int buffers_match(void)
{
    uint32_t i;

    for (i = 0; i < DMA_BUFFER_LENGTH; ++i) {
        if (g_dma_tx[i] != g_dma_rx[i]) {
            return 0;
        }
    }

    return 1;
}

static void dma_mem2mem_task(void *arg)
{
    hal_dma_mem2mem_config_t cfg;
    uint32_t tick = 0;
    int rc;

    (void)arg;

    cfg.src = (uintptr_t)g_dma_tx;
    cfg.dst = (uintptr_t)g_dma_rx;
    cfg.block_size = DMA_BUFFER_LENGTH;
    cfg.channel = HAL_DMA_CH0;

    while (1) {
        memset(g_dma_tx, 0, sizeof(g_dma_tx));
        memset(g_dma_rx, 0, sizeof(g_dma_rx));
        memcpy(g_dma_tx, "DMA Memory to Memory Transfer Example", 38u);

        rc = hal_dma_init_mem2mem(BE_U1000_DMA0_BASE, &cfg);
        if (rc == 0) {
            rc = hal_dma_mem2mem_transfer(256u);
        }

        os_print("[DMA_APP] tick %u rc=%d match=%u tx0=0x%x rx0=0x%x\n",
                 tick++,
                 rc,
                 (uint32_t)buffers_match(),
                 (uint32_t)g_dma_tx[0],
                 (uint32_t)g_dma_rx[0]);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    hal_board_init();
    hal_board_print_banner();

    os_print("[DMA_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());
    os_print("[DMA_APP] dma_mem2mem ready base=0x%x channel=%u bytes=%u\n",
             (uint32_t)BE_U1000_DMA0_BASE,
             0u,
             DMA_BUFFER_LENGTH);
    os_print("[DMA_APP] Initializing kernel...\n");

    os_kernel_init();
    os_task_create(&g_dma_task_tcb,
                   "dma_mem2mem",
                   dma_mem2mem_task,
                   0,
                   10,
                   g_dma_task_stack,
                   sizeof(g_dma_task_stack));

    os_print("[DMA_APP] Starting scheduler...\n");
    os_kernel_start();
}
