#include <stdint.h>

#include "hal_spi.h"

typedef struct {
    volatile uint32_t CTRLR0;
    volatile uint32_t CTRLR1;
    volatile uint32_t SPIENR;
    volatile uint32_t MWCR;
    volatile uint32_t SER;
    volatile uint32_t BAUDR;
    volatile uint32_t TXFTLR;
    volatile uint32_t RXFTLR;
    volatile uint32_t TXFLR;
    volatile uint32_t RXFLR;
    volatile uint32_t SR;
    volatile uint32_t IMR;
    volatile uint32_t ISR;
    volatile uint32_t RISR;
    volatile uint32_t TXOICR;
    volatile uint32_t RXOICR;
    volatile uint32_t RXUICR;
    volatile uint32_t MSTICR;
    volatile uint32_t ICR;
    volatile uint32_t DMACR;
    volatile uint32_t DMATDLR;
    volatile uint32_t DMARDLR;
    volatile uint32_t IDR;
    volatile uint32_t SSI_COMP_VERSION;
    volatile uint32_t DR[36];
} be_u1000_spi_t;

static volatile be_u1000_spi_t *g_spi;

#define SPI_CTRLR0_SCPH            (1u << 6)
#define SPI_CTRLR0_SCPOL           (1u << 7)
#define SPI_CTRLR0_DFS_8BIT        (7u << 16)

#define SPI_SPIENR_ENABLE          (1u << 0)
#define SPI_SER_SLAVE0             (1u << 0)

#define SPI_SR_BUSY                (1u << 0)
#define SPI_SR_TFNF                (1u << 1)
#define SPI_SR_RFNE                (1u << 3)

#ifndef HAL_SPI_MODE0
#define HAL_SPI_MODE0 0u
#define HAL_SPI_MODE1 1u
#define HAL_SPI_MODE2 2u
#define HAL_SPI_MODE3 3u
#endif

static int hal_spi_wait_mask_set(volatile uint32_t *reg, uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; i++) {
        if ((*reg & mask) == mask) {
            return 0;
        }
    }

    return -1;
}

static int hal_spi_wait_mask_clear(volatile uint32_t *reg, uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; i++) {
        if ((*reg & mask) == 0) {
            return 0;
        }
    }

    return -1;
}

int hal_spi_init(uintptr_t base, uint32_t baud_div, uint32_t mode)
{
    uint32_t ctrl0 = SPI_CTRLR0_DFS_8BIT;

    g_spi = (volatile be_u1000_spi_t *)base;
    if (g_spi == 0) {
        return -1;
    }

    if (baud_div < 2u) {
        baud_div = 2u;
    }
    if ((baud_div & 1u) != 0u) {
        baud_div += 1u;
    }

    if (mode == HAL_SPI_MODE1 || mode == HAL_SPI_MODE3) {
        ctrl0 |= SPI_CTRLR0_SCPH;
    }
    if (mode == HAL_SPI_MODE2 || mode == HAL_SPI_MODE3) {
        ctrl0 |= SPI_CTRLR0_SCPOL;
    }

    g_spi->SPIENR = 0;
    g_spi->CTRLR0 = ctrl0;
    g_spi->CTRLR1 = 0;
    g_spi->BAUDR = baud_div;
    g_spi->SER = SPI_SER_SLAVE0;
    g_spi->SPIENR = SPI_SPIENR_ENABLE;

    return 0;
}

int hal_spi_transfer(uint8_t tx, uint8_t *rx, uint32_t timeout)
{
    uint32_t data;

    if (g_spi == 0) {
        return -1;
    }

    if (hal_spi_wait_mask_set(&g_spi->SR, SPI_SR_TFNF, timeout) != 0) {
        return -1;
    }

    g_spi->DR[0] = tx;

    if (hal_spi_wait_mask_set(&g_spi->SR, SPI_SR_RFNE, timeout) != 0) {
        return -1;
    }

    data = g_spi->DR[0] & 0xFFu;
    if (rx != 0) {
        *rx = (uint8_t)data;
    }

    if (hal_spi_wait_mask_clear(&g_spi->SR, SPI_SR_BUSY, timeout) != 0) {
        return -1;
    }

    return 0;
}

int hal_spi_transfer_buffer(const uint8_t *tx, uint8_t *rx, uint32_t len, uint32_t timeout)
{
    uint32_t i;
    uint8_t txb;
    uint8_t rxb;

    for (i = 0; i < len; i++) {
        txb = (tx != 0) ? tx[i] : 0xFFu;
        if (hal_spi_transfer(txb, &rxb, timeout) != 0) {
            return -1;
        }
        if (rx != 0) {
            rx[i] = rxb;
        }
    }

    return 0;
}
