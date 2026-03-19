#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_i2s.h"

typedef struct {
    volatile uint32_t IER;
    volatile uint32_t IRER;
    volatile uint32_t ITER;
    volatile uint32_t RESERVED0[2];
    volatile uint32_t RXFFR;
    volatile uint32_t TXFFR;
    volatile uint32_t RESERVED1;
    volatile uint32_t LTHR0;
    volatile uint32_t RTHR0;
    volatile uint32_t RER0;
    volatile uint32_t TER0;
    volatile uint32_t RCR0;
    volatile uint32_t TCR0;
    volatile uint32_t ISR0;
    volatile uint32_t IMR0;
    volatile uint32_t ROR0;
    volatile uint32_t TOR0;
    volatile uint32_t RFCR0;
    volatile uint32_t TFCR0;
    volatile uint32_t RFF0;
    volatile uint32_t TFF0;
    uint32_t RESERVED2[90];
    volatile uint32_t RXDMA;
    volatile uint32_t RESERVED3;
    volatile uint32_t TXDMA;
    uint32_t RESERVED4[9];
    volatile uint32_t COMP_PARAM_2;
    volatile uint32_t COMP_PARAM_1;
    volatile uint32_t COMP_VERSION;
    volatile uint32_t COMP_TYPE;
    volatile uint32_t DMACR;
} be_u1000_i2s_t;

static volatile be_u1000_i2s_t *g_i2s;

#define I2S_IER_IEN            (1u << 0)
#define I2S_ITER_TXEN          (1u << 0)
#define I2S_TER0_TXCHEN0       (1u << 0)
#define I2S_TCR0_WLEN_Pos      0u
#define I2S_TCR0_WLEN_Msk      (0x7u << I2S_TCR0_WLEN_Pos)
#define I2S_IMR0_TXFEM         (1u << 4)
#define I2S_IMR0_TXFOM         (1u << 5)
#define I2S_TFCR0_TXCHET_Pos   0u
#define I2S_TFCR0_TXCHET_Msk   (0xFu << I2S_TFCR0_TXCHET_Pos)
#define I2S_TFF0_TXCHFR        (1u << 0)
#define I2S_TFF0_LEVEL_Msk     0x1Fu
#define I2S_TOR0_TXCHO0        (1u << 0)
#define I2S_DMACR_TXEN         (1u << 0)

int hal_i2s_init(uintptr_t base, const hal_i2s_config_t *config)
{
    if (base == 0 || config == NULL) {
        return -1;
    }

    g_i2s = (volatile be_u1000_i2s_t *)base;
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->IER = I2S_IER_IEN;
    g_i2s->ITER = 0u;
    g_i2s->TER0 = 0u;
    g_i2s->TCR0 = (((uint32_t)config->resolution << I2S_TCR0_WLEN_Pos) & I2S_TCR0_WLEN_Msk);
    g_i2s->TFCR0 = (((uint32_t)config->fifo_level << I2S_TFCR0_TXCHET_Pos) & I2S_TFCR0_TXCHET_Msk);
    g_i2s->IMR0 |= (I2S_IMR0_TXFEM | I2S_IMR0_TXFOM);
    g_i2s->DMACR = config->dma_transfer ? 1u : 0u;
    g_i2s->TXFFR = 1u;
    return 0;
}

int hal_i2s_enable_tx(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->TER0 |= I2S_TER0_TXCHEN0;
    g_i2s->ITER |= I2S_ITER_TXEN;
    return 0;
}

int hal_i2s_disable_tx(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->ITER &= ~I2S_ITER_TXEN;
    g_i2s->TER0 &= ~I2S_TER0_TXCHEN0;
    return 0;
}

int hal_i2s_tx_ready(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    return (g_i2s->ISR0 & I2S_IMR0_TXFEM) != 0u;
}

int hal_i2s_tx_overrun(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    return (g_i2s->TOR0 & I2S_TOR0_TXCHO0) != 0u;
}

int hal_i2s_clear_tx_overrun(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->TOR0 = I2S_TOR0_TXCHO0;
    return 0;
}

int hal_i2s_flush_tx_fifo(void)
{
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->TFF0 = I2S_TFF0_TXCHFR;
    return 0;
}

int hal_i2s_transmit_word(uint32_t value)
{
    if (g_i2s == NULL) {
        return -1;
    }

    g_i2s->TXDMA = value;
    return 0;
}

int hal_i2s_get_tx_fifo_level(uint32_t *level)
{
    if (g_i2s == NULL || level == NULL) {
        return -1;
    }

    *level = g_i2s->TFF0 & I2S_TFF0_LEVEL_Msk;
    return 0;
}

int hal_i2s_get_tx_state(hal_i2s_tx_state_t *state)
{
    if (g_i2s == NULL || state == NULL) {
        return -1;
    }

    state->fifo_level = g_i2s->TFF0 & I2S_TFF0_LEVEL_Msk;
    state->fifo_threshold = ((g_i2s->TFCR0 & I2S_TFCR0_TXCHET_Msk) >> I2S_TFCR0_TXCHET_Pos) + 1u;
    state->enabled = ((g_i2s->IER & I2S_IER_IEN) != 0u) &&
                     ((g_i2s->ITER & I2S_ITER_TXEN) != 0u) &&
                     ((g_i2s->TER0 & I2S_TER0_TXCHEN0) != 0u);
    state->ready = (g_i2s->ISR0 & I2S_IMR0_TXFEM) != 0u;
    state->overrun = (g_i2s->TOR0 & I2S_TOR0_TXCHO0) != 0u;
    state->dma_enabled = (g_i2s->DMACR & I2S_DMACR_TXEN) != 0u;
    return 0;
}
