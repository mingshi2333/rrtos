#include <stdbool.h>
#include <stdint.h>

#include "hal_canfd.h"

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t NOMINAL_BITRATE;
    volatile uint32_t STATUS;
    volatile uint32_t COMMAND;
    volatile uint32_t IRQ_ENABLE;
    volatile uint32_t IRQ_STATUS;
    volatile uint32_t ERROR_STATUS;
    volatile uint32_t TX_FIFO_LEVEL;
    volatile uint32_t RX_FIFO_LEVEL;
    volatile uint32_t TX_ID;
    volatile uint32_t TX_DLC;
    volatile uint32_t TX_DATA0;
    volatile uint32_t TX_DATA1;
    volatile uint32_t RX_ID;
    volatile uint32_t RX_DLC;
    volatile uint32_t RX_DATA0;
    volatile uint32_t RX_DATA1;
    volatile uint32_t SIGNATURE;
} be_u1000_canfd_t;

static volatile be_u1000_canfd_t *g_canfd;

#define CANFD_CTRL_ENABLE           (1u << 0)
#define CANFD_CTRL_LOOPBACK         (1u << 1)

#define CANFD_COMMAND_TX_KICK       (1u << 0)
#define CANFD_COMMAND_RX_POP        (1u << 1)

#define CANFD_SELFTEST_IRQ_MASK     (HAL_CANFD_IRQ_TX_COMPLETE | HAL_CANFD_IRQ_RX_READY | HAL_CANFD_IRQ_ERROR)

static uint32_t hal_canfd_pack_word(const uint8_t *data, uint8_t start, uint8_t len)
{
    uint32_t word = 0;
    uint8_t i;

    for (i = 0; i < 4u; ++i) {
        uint8_t index = (uint8_t)(start + i);

        if (index >= len) {
            break;
        }

        word |= (uint32_t)data[index] << (i * 8u);
    }

    return word;
}

static void hal_canfd_unpack_word(uint32_t word, uint8_t *data, uint8_t start, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < 4u; ++i) {
        uint8_t index = (uint8_t)(start + i);

        if (index >= len) {
            break;
        }

        data[index] = (uint8_t)((word >> (i * 8u)) & 0xFFu);
    }
}

static int hal_canfd_wait_status(uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    if (g_canfd == 0) {
        return -1;
    }

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd->STATUS & mask) == mask) {
            return 0;
        }
    }

    return -1;
}

static int hal_canfd_wait_clear(uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    if (g_canfd == 0) {
        return -1;
    }

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd->STATUS & mask) == 0u) {
            return 0;
        }
    }

    return -1;
}

int hal_canfd_init(uintptr_t base, const hal_canfd_config_t *config)
{
    uint32_t ctrl = CANFD_CTRL_ENABLE;

    if (base == 0 || config == 0 || config->nominal_bitrate == 0u) {
        return -1;
    }

    g_canfd = (volatile be_u1000_canfd_t *)base;
    if (g_canfd == 0) {
        return -1;
    }

    if (config->internal_loopback) {
        ctrl |= CANFD_CTRL_LOOPBACK;
    }

    g_canfd->CTRL = 0u;
    g_canfd->NOMINAL_BITRATE = config->nominal_bitrate;
    g_canfd->COMMAND = 0u;
    g_canfd->IRQ_ENABLE = 0u;
    g_canfd->IRQ_STATUS = CANFD_SELFTEST_IRQ_MASK;
    g_canfd->ERROR_STATUS = 0u;
    g_canfd->CTRL = ctrl;

    return hal_canfd_wait_status(HAL_CANFD_STATUS_READY | HAL_CANFD_STATUS_TX_FIFO_SPACE,
                                 1000u);
}

int hal_canfd_irq_enable(uint32_t mask)
{
    if (g_canfd == 0) {
        return -1;
    }

    g_canfd->IRQ_ENABLE = mask & CANFD_SELFTEST_IRQ_MASK;
    return 0;
}

int hal_canfd_irq_ack(uint32_t mask)
{
    if (g_canfd == 0) {
        return -1;
    }

    g_canfd->IRQ_STATUS = mask & CANFD_SELFTEST_IRQ_MASK;
    return 0;
}

int hal_canfd_get_state(hal_canfd_state_t *state)
{
    if (g_canfd == 0 || state == 0) {
        return -1;
    }

    state->status = g_canfd->STATUS;
    state->irq_status = g_canfd->IRQ_STATUS;
    state->error_status = g_canfd->ERROR_STATUS;
    state->tx_fifo_level = g_canfd->TX_FIFO_LEVEL;
    state->rx_fifo_level = g_canfd->RX_FIFO_LEVEL;
    return 0;
}

int hal_canfd_tx_enqueue(const hal_canfd_frame_t *tx, uint32_t timeout)
{
    if (g_canfd == 0 || tx == 0 || tx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    if ((g_canfd->CTRL & (CANFD_CTRL_ENABLE | CANFD_CTRL_LOOPBACK)) !=
        (CANFD_CTRL_ENABLE | CANFD_CTRL_LOOPBACK)) {
        return -1;
    }

    if (hal_canfd_wait_status(HAL_CANFD_STATUS_READY | HAL_CANFD_STATUS_TX_FIFO_SPACE,
                              timeout) != 0) {
        return -1;
    }

    g_canfd->TX_ID = tx->id;
    g_canfd->TX_DLC = tx->len;
    g_canfd->TX_DATA0 = hal_canfd_pack_word(tx->data, 0u, tx->len);
    g_canfd->TX_DATA1 = hal_canfd_pack_word(tx->data, 4u, tx->len);
    g_canfd->COMMAND = CANFD_COMMAND_TX_KICK;

    if (hal_canfd_wait_status(HAL_CANFD_STATUS_IRQ_PENDING | HAL_CANFD_STATUS_RX_PENDING,
                              timeout) != 0) {
        return -1;
    }

    return 0;
}

int hal_canfd_rx_dequeue(hal_canfd_frame_t *rx, uint32_t timeout)
{
    if (g_canfd == 0 || rx == 0) {
        return -1;
    }

    if (hal_canfd_wait_status(HAL_CANFD_STATUS_RX_PENDING, timeout) != 0) {
        return -1;
    }

    rx->id = g_canfd->RX_ID;
    rx->len = (uint8_t)(g_canfd->RX_DLC & 0xFFu);
    if (rx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    hal_canfd_unpack_word(g_canfd->RX_DATA0, rx->data, 0u, rx->len);
    hal_canfd_unpack_word(g_canfd->RX_DATA1, rx->data, 4u, rx->len);
    g_canfd->COMMAND = CANFD_COMMAND_RX_POP;

    if (hal_canfd_wait_clear(HAL_CANFD_STATUS_RX_PENDING, timeout) != 0) {
        return -1;
    }

    return 0;
}

int hal_canfd_loopback(const hal_canfd_frame_t *tx, hal_canfd_frame_t *rx, uint32_t timeout)
{
    if (g_canfd == 0 || tx == 0 || rx == 0 || tx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    if (hal_canfd_tx_enqueue(tx, timeout) != 0) {
        return -1;
    }

    return hal_canfd_rx_dequeue(rx, timeout);
}
