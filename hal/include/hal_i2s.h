#ifndef HAL_I2S_H
#define HAL_I2S_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_I2S_MODE_SLAVE_TX = 0,
    HAL_I2S_MODE_SLAVE_RX = 1,
} hal_i2s_mode_t;

typedef enum {
    HAL_I2S_RES_IGNORE = 0,
    HAL_I2S_RES_12 = 1,
    HAL_I2S_RES_16 = 2,
    HAL_I2S_RES_20 = 3,
    HAL_I2S_RES_24 = 4,
    HAL_I2S_RES_32 = 5,
} hal_i2s_resolution_t;

typedef enum {
    HAL_I2S_FIFO_LVL1 = 0,
    HAL_I2S_FIFO_LVL2 = 1,
    HAL_I2S_FIFO_LVL3 = 2,
    HAL_I2S_FIFO_LVL4 = 3,
    HAL_I2S_FIFO_LVL5 = 4,
    HAL_I2S_FIFO_LVL6 = 5,
    HAL_I2S_FIFO_LVL7 = 6,
    HAL_I2S_FIFO_LVL8 = 7,
    HAL_I2S_FIFO_LVL9 = 8,
    HAL_I2S_FIFO_LVL10 = 9,
    HAL_I2S_FIFO_LVL11 = 10,
    HAL_I2S_FIFO_LVL12 = 11,
    HAL_I2S_FIFO_LVL13 = 12,
    HAL_I2S_FIFO_LVL14 = 13,
    HAL_I2S_FIFO_LVL15 = 14,
    HAL_I2S_FIFO_LVL16 = 15,
} hal_i2s_fifo_level_t;

typedef struct {
    hal_i2s_mode_t mode;
    hal_i2s_resolution_t resolution;
    hal_i2s_fifo_level_t fifo_level;
    bool dma_transfer;
} hal_i2s_config_t;

typedef struct {
    uint32_t fifo_level;
    uint32_t fifo_threshold;
    bool enabled;
    bool ready;
    bool overrun;
    bool dma_enabled;
} hal_i2s_tx_state_t;

int hal_i2s_init(uintptr_t base, const hal_i2s_config_t *config);
int hal_i2s_enable_tx(void);
int hal_i2s_disable_tx(void);
int hal_i2s_tx_ready(void);
int hal_i2s_tx_overrun(void);
int hal_i2s_clear_tx_overrun(void);
int hal_i2s_flush_tx_fifo(void);
int hal_i2s_transmit_word(uint32_t value);
int hal_i2s_get_tx_fifo_level(uint32_t *level);
int hal_i2s_get_tx_state(hal_i2s_tx_state_t *state);

#endif
