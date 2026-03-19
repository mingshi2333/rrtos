#ifndef HAL_DMA_H
#define HAL_DMA_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    HAL_DMA_CH0 = 0,
    HAL_DMA_CH1 = 1,
    HAL_DMA_CH2 = 2,
    HAL_DMA_CH3 = 3,
    HAL_DMA_CH4 = 4,
    HAL_DMA_CH5 = 5,
    HAL_DMA_CH6 = 6,
    HAL_DMA_CH7 = 7,
} hal_dma_channel_t;

typedef struct {
    uintptr_t src;
    uintptr_t dst;
    uint16_t block_size;
    hal_dma_channel_t channel;
} hal_dma_mem2mem_config_t;

int hal_dma_init_mem2mem(uintptr_t base, const hal_dma_mem2mem_config_t *config);
int hal_dma_enable_channel(void);
int hal_dma_poll_done(uint32_t timeout);
int hal_dma_mem2mem_transfer(uint32_t timeout);

#endif
