#ifndef HAL_CANFD_H
#define HAL_CANFD_H

#include <stdbool.h>
#include <stdint.h>

#define HAL_CANFD_MAX_DATA_LEN 8u

#define HAL_CANFD_STATUS_READY         (1u << 0)
#define HAL_CANFD_STATUS_TX_FIFO_SPACE (1u << 1)
#define HAL_CANFD_STATUS_RX_PENDING    (1u << 2)
#define HAL_CANFD_STATUS_LOOPBACK      (1u << 3)
#define HAL_CANFD_STATUS_IRQ_PENDING   (1u << 4)
#define HAL_CANFD_STATUS_ERROR         (1u << 5)

#define HAL_CANFD_IRQ_TX_COMPLETE      (1u << 0)
#define HAL_CANFD_IRQ_RX_READY         (1u << 1)
#define HAL_CANFD_IRQ_ERROR            (1u << 2)

#define HAL_CANFD_ERROR_NONE           0u
#define HAL_CANFD_ERROR_CONFIG         (1u << 0)
#define HAL_CANFD_ERROR_TIMEOUT        (1u << 1)
#define HAL_CANFD_ERROR_PROTOCOL       (1u << 2)
#define HAL_CANFD_ERROR_RX_OVERFLOW    (1u << 3)

typedef struct {
    uint32_t nominal_bitrate;
    bool internal_loopback;
} hal_canfd_config_t;

typedef struct {
    uint32_t id;
    uint8_t len;
    uint8_t data[HAL_CANFD_MAX_DATA_LEN];
} hal_canfd_frame_t;

typedef struct {
    uint32_t status;
    uint32_t irq_status;
    uint32_t error_status;
    uint32_t tx_fifo_level;
    uint32_t rx_fifo_level;
} hal_canfd_state_t;

int hal_canfd_init(uintptr_t base, const hal_canfd_config_t *config);
int hal_canfd_irq_enable(uint32_t mask);
int hal_canfd_irq_ack(uint32_t mask);
int hal_canfd_get_state(hal_canfd_state_t *state);
int hal_canfd_tx_enqueue(const hal_canfd_frame_t *tx, uint32_t timeout);
int hal_canfd_rx_dequeue(hal_canfd_frame_t *rx, uint32_t timeout);
int hal_canfd_loopback(const hal_canfd_frame_t *tx, hal_canfd_frame_t *rx, uint32_t timeout);

#endif
