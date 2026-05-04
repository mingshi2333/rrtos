#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "board_config.h"
#include "hal_canfd.h"

typedef struct {
    volatile uint32_t ID;
    volatile uint32_t DLC;
    volatile uint32_t DW[16];
} be_u1000_canfd_msg_t;

typedef struct {
    volatile uint32_t AFMR;
    volatile uint32_t AFIR;
} be_u1000_canfd_filter_t;

typedef struct {
    volatile uint32_t ID;
    volatile uint32_t DLC;
} be_u1000_canfd_txe_t;

typedef struct {
    volatile uint32_t SRR;
    volatile uint32_t MSR;
    volatile uint32_t BRPR;
    volatile uint32_t BTR;
    volatile uint32_t ECR;
    volatile uint32_t ESR;
    volatile uint32_t SR;
    volatile uint32_t ISR;
    volatile uint32_t IER;
    volatile uint32_t ICR;
    volatile uint32_t TSR;
    volatile uint32_t reserved0[23];
    volatile uint32_t DP_BRPR;
    volatile uint32_t DP_BTR;
    volatile uint32_t TRR;
    volatile uint32_t IETRS;
    volatile uint32_t TCR;
    volatile uint32_t IETCS;
    volatile uint32_t TXE_FSR;
    volatile uint32_t TXE_WMR;
    volatile uint32_t reserved1[14];
    volatile uint32_t AFR;
    volatile uint32_t reserved2;
    volatile uint32_t FSR;
    volatile uint32_t WMR;
    volatile uint32_t reserved3[4];
    be_u1000_canfd_msg_t TB[32];
    be_u1000_canfd_filter_t AF[32];
    volatile uint32_t reserved4[1344];
    be_u1000_canfd_txe_t TXE_TB[32];
    be_u1000_canfd_msg_t RB_FIFO0[64];
    volatile uint32_t reserved5[896];
    be_u1000_canfd_msg_t RB_FIFO1[64];
} be_u1000_canfd_t;

typedef struct {
    volatile uint32_t CLKSEL;
    volatile uint32_t PLLSET;
    volatile uint32_t CLKCR0;
    volatile uint32_t PCLK0EN;
    volatile uint32_t PCLK1EN;
    volatile uint32_t PCLK2EN;
    volatile uint32_t SYSCR0;
} be_u1000_cru_t;

static volatile be_u1000_canfd_t *g_canfd;
static bool g_internal_loopback;

#define CANFD_TX_NO_BUFFER             0xFFu
#define CANFD_TX_BUFFER_COUNT          32u
#define CANFD_RX_FIFO0_DEPTH           64u
#define CANFD_TX_BUFFER_ALL            0xFFFFFFFFu

static uint8_t g_last_tx_buffer = CANFD_TX_NO_BUFFER;

#define CANFD_SRR_SRST                 (1u << 0)
#define CANFD_SRR_CEN                  (1u << 1)

#define CANFD_MSR_SLEEP                (1u << 0)
#define CANFD_MSR_LBACK                (1u << 1)
#define CANFD_MSR_SNOOP                (1u << 2)
#define CANFD_MSR_BRSD                 (1u << 3)
#define CANFD_MSR_DAR                  (1u << 4)
#define CANFD_MSR_DPEE                 (1u << 5)
#define CANFD_MSR_ABR                  (1u << 7)
#define CANFD_MSR_ITO                  (0xFFu << 8)

#define CANFD_BRPR_PR_POS              0u
#define CANFD_BTR_TS1_POS              0u
#define CANFD_BTR_TS2_POS              8u
#define CANFD_BTR_SJW_POS              16u
#define CANFD_DP_BRPR_PR_POS           0u
#define CANFD_DP_BRPR_TDCOFF_POS       8u
#define CANFD_DP_BRPR_TDC_POS          16u
#define CANFD_DP_BTR_TS1_POS           0u
#define CANFD_DP_BTR_TS2_POS           8u
#define CANFD_DP_BTR_SJW_POS           16u

#define CANFD_SR_CONFIG                (1u << 0)
#define CANFD_SR_LBACK                 (1u << 1)
#define CANFD_SR_SLEEP                 (1u << 2)
#define CANFD_SR_NORMAL                (1u << 3)
#define CANFD_SR_ERRWRN                (1u << 6)

#define CANFD_IT_TXOK                  (1u << 1)
#define CANFD_IT_RXOK                  (1u << 4)
#define CANFD_IT_ERROR                 (1u << 8)

#define CANFD_ERROR_ALL                0x00000F1Fu

#define CANFD_FSR_RI_POS               0u
#define CANFD_FSR_RI_MASK              (0x3Fu << CANFD_FSR_RI_POS)
#define CANFD_FSR_IRI                  (1u << 7)
#define CANFD_FSR_FL_POS               8u
#define CANFD_FSR_FL_MASK              (0x7Fu << CANFD_FSR_FL_POS)

#define CANFD_WMR_RXFP_POS             16u
#define CANFD_WMR_RXFP_MASK            (0x1Fu << CANFD_WMR_RXFP_POS)

#define CANFD_TBID_STD_ID_POS          21u
#define CANFD_TBDLC_DLC_POS            28u
#define CANFD_RBID_IDE                 (1u << 19)
#define CANFD_RBID_STD_ID_POS          21u
#define CANFD_RBDLC_DLC_POS            28u

#define CANFD_AFMR_STD_AMID_POS        21u
#define CANFD_AFMR_AMIDE               (1u << 19)
#define CANFD_AFMR_AMSRR               (1u << 20)
#define CANFD_AFIR_STD_AIID_POS        21u

#define CRU_PCLK0_GPIO0_CLKEN          (1u << 11)
#define CRU_PCLK0_CANFD0_CLKEN         (1u << 13)
#define CRU_PCLK0_GPIO0_RSTN           (1u << 27)
#define CRU_PCLK0_CANFD0_RSTN          (1u << 29)
#define CRU_PCLK1_GPIO1_CLKEN          (1u << 11)
#define CRU_PCLK1_CANFD1_CLKEN         (1u << 13)
#define CRU_PCLK1_GPIO1_RSTN           (1u << 27)
#define CRU_PCLK1_CANFD1_RSTN          (1u << 29)

#define CANFD_SELFTEST_IRQ_MASK \
    (HAL_CANFD_IRQ_TX_COMPLETE | HAL_CANFD_IRQ_RX_READY | HAL_CANFD_IRQ_ERROR)

static void hal_canfd_delay(uint32_t cycles)
{
    volatile uint32_t i;

    for (i = 0; i < cycles; ++i) {
        __asm__ volatile("nop");
    }
}

static uint32_t hal_canfd_popcount32(uint32_t value)
{
    uint32_t count = 0;

    while (value != 0u) {
        count += value & 1u;
        value >>= 1u;
    }

    return count;
}

static void hal_canfd_enable_clock(uintptr_t base)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    if (base == (uintptr_t)BE_U1000_CANFD0_BASE) {
        cru->PCLK0EN |= CRU_PCLK0_GPIO0_CLKEN |
                        CRU_PCLK0_CANFD0_CLKEN |
                        CRU_PCLK0_CANFD0_RSTN;
    } else if (base == (uintptr_t)BE_U1000_CANFD1_BASE) {
        cru->PCLK1EN |= CRU_PCLK1_GPIO1_CLKEN |
                        CRU_PCLK1_CANFD1_CLKEN |
                        CRU_PCLK1_CANFD1_RSTN;
    }
}

static int hal_canfd_wait_mode(uint32_t mode_mask, uint32_t timeout)
{
    uint32_t i;

    if (g_canfd == 0) {
        return -1;
    }

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd->SR & mode_mask) != 0u) {
            return 0;
        }
    }

    return -1;
}

static int hal_canfd_wait_tx_done(uint8_t buffer, uint32_t timeout)
{
    uint32_t mask = 1u << buffer;
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd->TRR & mask) == 0u) {
            return 0;
        }
    }

    return -1;
}

static int hal_canfd_wait_rx_ready(uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd->FSR & CANFD_FSR_FL_MASK) != 0u) {
            return 0;
        }
    }

    return -1;
}

static uint8_t hal_canfd_find_tx_buffer(void)
{
    uint32_t pending = g_canfd->TRR;
    uint8_t i;

    if (pending == CANFD_TX_BUFFER_ALL) {
        return CANFD_TX_NO_BUFFER;
    }

    for (i = 0; i < CANFD_TX_BUFFER_COUNT; ++i) {
        if ((pending & (1u << i)) == 0u) {
            return i;
        }
    }

    return CANFD_TX_NO_BUFFER;
}

static uint32_t hal_canfd_pack_data_word(const uint8_t *data, uint8_t start, uint8_t len)
{
    uint32_t word = 0;
    uint8_t i;

    for (i = 0; i < 4u; ++i) {
        uint8_t index = (uint8_t)(start + i);

        if (index >= len) {
            break;
        }

        word |= (uint32_t)data[index] << (8u * (3u - i));
    }

    return word;
}

static void hal_canfd_unpack_data_word(uint32_t word, uint8_t *data, uint8_t start, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < 4u; ++i) {
        uint8_t index = (uint8_t)(start + i);

        if (index >= len) {
            break;
        }

        data[index] = (uint8_t)((word >> (8u * (3u - i))) & 0xFFu);
    }
}

static uint32_t hal_canfd_irq_to_hw(uint32_t mask)
{
    uint32_t hw = 0;

    if ((mask & HAL_CANFD_IRQ_TX_COMPLETE) != 0u) {
        hw |= CANFD_IT_TXOK;
    }
    if ((mask & HAL_CANFD_IRQ_RX_READY) != 0u) {
        hw |= CANFD_IT_RXOK;
    }
    if ((mask & HAL_CANFD_IRQ_ERROR) != 0u) {
        hw |= CANFD_IT_ERROR;
    }

    return hw;
}

static uint32_t hal_canfd_irq_from_hw(uint32_t hw)
{
    uint32_t mask = 0;

    if ((hw & CANFD_IT_TXOK) != 0u) {
        mask |= HAL_CANFD_IRQ_TX_COMPLETE;
    }
    if ((hw & CANFD_IT_RXOK) != 0u) {
        mask |= HAL_CANFD_IRQ_RX_READY;
    }
    if ((hw & CANFD_IT_ERROR) != 0u) {
        mask |= HAL_CANFD_IRQ_ERROR;
    }

    return mask;
}

static uint32_t hal_canfd_status_from_hw(void)
{
    uint32_t status = 0;
    uint32_t hw_sr = g_canfd->SR;
    uint32_t rx_level = (g_canfd->FSR & CANFD_FSR_FL_MASK) >> CANFD_FSR_FL_POS;

    if ((g_canfd->SRR & CANFD_SRR_CEN) != 0u ||
        (hw_sr & (CANFD_SR_CONFIG | CANFD_SR_NORMAL | CANFD_SR_LBACK)) != 0u) {
        status |= HAL_CANFD_STATUS_READY;
    }
    if (g_canfd->TRR != CANFD_TX_BUFFER_ALL) {
        status |= HAL_CANFD_STATUS_TX_FIFO_SPACE;
    }
    if (rx_level != 0u) {
        status |= HAL_CANFD_STATUS_RX_PENDING;
    }
    if ((hw_sr & CANFD_SR_LBACK) != 0u || (g_canfd->MSR & CANFD_MSR_LBACK) != 0u) {
        status |= HAL_CANFD_STATUS_LOOPBACK;
    }
    if ((g_canfd->ISR & g_canfd->IER) != 0u) {
        status |= HAL_CANFD_STATUS_IRQ_PENDING;
    }
    if ((g_canfd->ESR & CANFD_ERROR_ALL) != 0u || (hw_sr & CANFD_SR_ERRWRN) != 0u) {
        status |= HAL_CANFD_STATUS_ERROR;
    }

    return status;
}

static void hal_canfd_config_accept_all_std(void)
{
    g_canfd->AFR &= ~1u;
    g_canfd->AF[0].AFIR = 0u << CANFD_AFIR_STD_AIID_POS;
    g_canfd->AF[0].AFMR = (0u << CANFD_AFMR_STD_AMID_POS) |
                          CANFD_AFMR_AMIDE |
                          CANFD_AFMR_AMSRR;
    g_canfd->WMR = (g_canfd->WMR & ~CANFD_WMR_RXFP_MASK) |
                   (0x1Fu << CANFD_WMR_RXFP_POS);
    g_canfd->AFR |= 1u;
}

int hal_canfd_init(uintptr_t base, const hal_canfd_config_t *config)
{
    uint32_t mode_mask;
    uint32_t nominal_prescaler = 2u;

    if (base == 0 || config == 0 || config->nominal_bitrate == 0u) {
        return -1;
    }

    hal_canfd_enable_clock(base);

    g_canfd = (volatile be_u1000_canfd_t *)base;
    g_internal_loopback = config->internal_loopback;
    g_last_tx_buffer = CANFD_TX_NO_BUFFER;

    g_canfd->SRR |= CANFD_SRR_SRST;
    hal_canfd_delay(128u);
    g_canfd->SRR &= ~CANFD_SRR_CEN;

    g_canfd->IER = 0u;
    g_canfd->ICR = 0xFFFFFFFFu;
    g_canfd->ESR = CANFD_ERROR_ALL;

    if (config->nominal_bitrate <= 500000u) {
        nominal_prescaler = 2u;
    }

    g_canfd->MSR &= ~(CANFD_MSR_SLEEP |
                      CANFD_MSR_SNOOP |
                      CANFD_MSR_LBACK |
                      CANFD_MSR_BRSD |
                      CANFD_MSR_DAR |
                      CANFD_MSR_DPEE |
                      CANFD_MSR_ITO);
    g_canfd->MSR |= CANFD_MSR_ABR;
    if (config->internal_loopback) {
        g_canfd->MSR |= CANFD_MSR_LBACK;
    }

    g_canfd->BRPR = (nominal_prescaler - 1u) << CANFD_BRPR_PR_POS;
    g_canfd->BTR = ((1u - 1u) << CANFD_BTR_SJW_POS) |
                   ((12u - 1u) << CANFD_BTR_TS2_POS) |
                   ((12u - 1u) << CANFD_BTR_TS1_POS);
    g_canfd->DP_BRPR = ((0u << CANFD_DP_BRPR_TDC_POS) |
                        (0u << CANFD_DP_BRPR_TDCOFF_POS) |
                        ((nominal_prescaler - 1u) << CANFD_DP_BRPR_PR_POS));
    g_canfd->DP_BTR = ((1u - 1u) << CANFD_DP_BTR_SJW_POS) |
                      ((12u - 1u) << CANFD_DP_BTR_TS2_POS) |
                      ((12u - 1u) << CANFD_DP_BTR_TS1_POS);

    hal_canfd_config_accept_all_std();

    g_canfd->SRR |= CANFD_SRR_CEN;
    if (!config->internal_loopback) {
        return ((g_canfd->SRR & CANFD_SRR_CEN) != 0u) ? 0 : -1;
    }

    mode_mask = CANFD_SR_LBACK;
    return hal_canfd_wait_mode(mode_mask, 100000u);
}

int hal_canfd_irq_enable(uint32_t mask)
{
    if (g_canfd == 0) {
        return -1;
    }

    g_canfd->IER = hal_canfd_irq_to_hw(mask & CANFD_SELFTEST_IRQ_MASK);
    return 0;
}

int hal_canfd_irq_ack(uint32_t mask)
{
    if (g_canfd == 0) {
        return -1;
    }

    g_canfd->ICR = hal_canfd_irq_to_hw(mask & CANFD_SELFTEST_IRQ_MASK);
    if ((mask & HAL_CANFD_IRQ_ERROR) != 0u) {
        g_canfd->ESR = CANFD_ERROR_ALL;
    }

    return 0;
}

int hal_canfd_get_state(hal_canfd_state_t *state)
{
    uint32_t rx_level;

    if (g_canfd == 0 || state == 0) {
        return -1;
    }

    rx_level = (g_canfd->FSR & CANFD_FSR_FL_MASK) >> CANFD_FSR_FL_POS;

    state->status = hal_canfd_status_from_hw();
    state->irq_status = hal_canfd_irq_from_hw(g_canfd->ISR & (CANFD_IT_TXOK |
                                                              CANFD_IT_RXOK |
                                                              CANFD_IT_ERROR));
    state->error_status = ((g_canfd->ESR & CANFD_ERROR_ALL) == 0u) ?
                          HAL_CANFD_ERROR_NONE :
                          HAL_CANFD_ERROR_PROTOCOL;
    state->tx_fifo_level = hal_canfd_popcount32(g_canfd->TRR);
    state->rx_fifo_level = rx_level;
    return 0;
}

int hal_canfd_tx_enqueue(const hal_canfd_frame_t *tx, uint32_t timeout)
{
    uint8_t tx_buffer;
    uint8_t word;

    if (g_canfd == 0 || tx == 0 || tx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    if (!g_internal_loopback) {
        return -1;
    }

    tx_buffer = hal_canfd_find_tx_buffer();
    if (tx_buffer == CANFD_TX_NO_BUFFER) {
        return -1;
    }

    g_canfd->TB[tx_buffer].ID = (tx->id & 0x7FFu) << CANFD_TBID_STD_ID_POS;
    g_canfd->TB[tx_buffer].DLC = ((uint32_t)tx->len & 0xFu) << CANFD_TBDLC_DLC_POS;

    for (word = 0; word < 2u; ++word) {
        g_canfd->TB[tx_buffer].DW[word] =
            hal_canfd_pack_data_word(tx->data, (uint8_t)(word * 4u), tx->len);
    }

    g_last_tx_buffer = tx_buffer;
    g_canfd->TRR |= 1u << tx_buffer;

    if (hal_canfd_wait_tx_done(tx_buffer, timeout) != 0) {
        return -1;
    }

    return 0;
}

int hal_canfd_rx_dequeue(hal_canfd_frame_t *rx, uint32_t timeout)
{
    uint32_t read_index;
    uint32_t id_reg;
    uint32_t dlc_reg;
    uint8_t word;

    if (g_canfd == 0 || rx == 0) {
        return -1;
    }

    if (hal_canfd_wait_rx_ready(timeout) != 0) {
        return -1;
    }

    read_index = (g_canfd->FSR & CANFD_FSR_RI_MASK) >> CANFD_FSR_RI_POS;
    if (read_index >= CANFD_RX_FIFO0_DEPTH) {
        return -1;
    }

    id_reg = g_canfd->RB_FIFO0[read_index].ID;
    dlc_reg = g_canfd->RB_FIFO0[read_index].DLC;

    if ((id_reg & CANFD_RBID_IDE) != 0u) {
        return -1;
    }

    rx->id = (id_reg >> CANFD_RBID_STD_ID_POS) & 0x7FFu;
    rx->len = (uint8_t)((dlc_reg >> CANFD_RBDLC_DLC_POS) & 0xFu);
    if (rx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    for (word = 0; word < 2u; ++word) {
        hal_canfd_unpack_data_word(g_canfd->RB_FIFO0[read_index].DW[word],
                                   rx->data,
                                   (uint8_t)(word * 4u),
                                   rx->len);
    }

    g_canfd->FSR |= CANFD_FSR_IRI;
    return 0;
}

int hal_canfd_loopback(const hal_canfd_frame_t *tx, hal_canfd_frame_t *rx, uint32_t timeout)
{
    (void)g_last_tx_buffer;

    if (g_canfd == 0 || tx == 0 || rx == 0 || tx->len > HAL_CANFD_MAX_DATA_LEN) {
        return -1;
    }

    if (hal_canfd_tx_enqueue(tx, timeout) != 0) {
        return -1;
    }

    return hal_canfd_rx_dequeue(rx, timeout);
}
