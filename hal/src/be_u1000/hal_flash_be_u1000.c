#include <stddef.h>
#include <stdint.h>

#include "board_config.h"
#include "hal_flash.h"

static volatile const uint8_t *g_flash_base;
static uint32_t g_flash_size;

#define FLASH_INFO_JEDEC_OFFSET  0x10u
#define FLASH_INFO_PAGE_OFFSET   0x14u
#define FLASH_INFO_SECTOR_OFFSET 0x18u
#define FLASH_INFO_SIZE_OFFSET   0x1Cu
#define FLASH_SIM_SIG0           0x31505351u
#define FLASH_SIM_SIG1           0x5F4C444Du
#define FLASH_SIM_SIG2           0x00010010u
#define FLASH_SIM_SIG3           0xA55A3CC3u

typedef struct {
    volatile uint32_t CTRLR0;
    volatile uint32_t CTRLR1;
    volatile uint32_t QSPIENR;
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
    volatile uint32_t VER_ID;
    volatile uint32_t DR[36];
    volatile uint32_t RX_SAMPLE_DLY;
    volatile uint32_t SPI_CTRLR0;
    volatile uint32_t TXD_DRIVE_EDGE;
} be_u1000_qspi_t;

typedef struct {
    volatile uint32_t CLKSEL;
    volatile uint32_t PLLSET;
    volatile uint32_t CLKCR0;
    volatile uint32_t PCLK0EN;
    volatile uint32_t PCLK1EN;
    volatile uint32_t PCLK2EN;
    volatile uint32_t SYSCR0;
} be_u1000_cru_t;

#define QSPI_CTRLR0_TMOD_RX        (2u << 8)
#define QSPI_CTRLR0_TMOD_EEPROM    (3u << 8)
#define QSPI_CTRLR0_DFS_8BIT       (7u << 16)
#define QSPI_CTRLR0_DFS_32BIT      (31u << 16)
#define QSPI_CTRLR0_SPI_FRF_QUAD   (2u << 21)
#define QSPI_CTRLR0_SSTE           (1u << 24)
#define QSPI_CTRLR0_SECONV         (1u << 25)

#define QSPI_SPI_CTRLR0_TRANS_BOTH (2u << 0)
#define QSPI_SPI_CTRLR0_TRANS_STD  (0u << 0)
#define QSPI_SPI_CTRLR0_ADDR_0     (0u << 2)
#define QSPI_SPI_CTRLR0_ADDR_24    (6u << 2)
#define QSPI_SPI_CTRLR0_INST_0     (0u << 8)
#define QSPI_SPI_CTRLR0_INST_8     (2u << 8)

#define QSPI_QSPIENR_ENABLE        (1u << 0)
#define QSPI_SER_NONE              0u
#define QSPI_SER_0                 (1u << 0)

#define QSPI_SR_BUSY               (1u << 0)
#define QSPI_SR_TFNF               (1u << 1)
#define QSPI_SR_TFE                (1u << 2)
#define QSPI_SR_RFNE               (1u << 3)

#define CRU_PCLK1_QSPI1_CLKEN      (1u << 0)
#define CRU_PCLK1_GPIO1_CLKEN      (1u << 11)
#define CRU_PCLK1_QSPI1_RSTN       (1u << 16)
#define CRU_SYSCR0_XIP1EN          (1u << 8)

#define QSPI_FLASH_CMD_RDID        0x9Fu
#define QSPI_FLASH_JEDEC_LEN       3u
#define QSPI_FLASH_TIMEOUT         100000u
#define QSPI_FLASH_DEFAULT_BAUDR   32u

enum {
    FLASH_DIAG_STATUS_IDLE = 0,
    FLASH_DIAG_STATUS_BAD_ARG = 1,
    FLASH_DIAG_STATUS_TFNF_TIMEOUT = 2,
    FLASH_DIAG_STATUS_SHORT_RX = 3,
    FLASH_DIAG_STATUS_INVALID_ID = 4,
    FLASH_DIAG_STATUS_OK = 5,
};

static hal_flash_diag_t g_flash_diag;

static int hal_flash_range_valid(uint32_t offset, size_t len)
{
    if (g_flash_base == 0) {
        return -1;
    }

    if (offset > g_flash_size) {
        return -1;
    }

    if (len > (size_t)(g_flash_size - offset)) {
        return -1;
    }

    return 0;
}

static int hal_flash_info_valid(const hal_flash_info_t *info)
{
    return info->jedec_id != 0u &&
           info->page_size != 0u &&
           info->sector_size != 0u &&
           info->capacity_bytes != 0u;
}

static int hal_flash_read_header_info(hal_flash_info_t *info)
{
    uint32_t sig0 = 0;
    uint32_t sig1 = 0;
    uint32_t sig2 = 0;
    uint32_t sig3 = 0;

    if (hal_flash_read_u32(0u, &sig0) != 0 ||
        hal_flash_read_u32(4u, &sig1) != 0 ||
        hal_flash_read_u32(8u, &sig2) != 0 ||
        hal_flash_read_u32(12u, &sig3) != 0 ||
        sig0 != FLASH_SIM_SIG0 ||
        sig1 != FLASH_SIM_SIG1 ||
        sig2 != FLASH_SIM_SIG2 ||
        sig3 != FLASH_SIM_SIG3) {
        return -1;
    }

    if (hal_flash_read_u32(FLASH_INFO_JEDEC_OFFSET, &info->jedec_id) != 0 ||
        hal_flash_read_u32(FLASH_INFO_PAGE_OFFSET, &info->page_size) != 0 ||
        hal_flash_read_u32(FLASH_INFO_SECTOR_OFFSET, &info->sector_size) != 0 ||
        hal_flash_read_u32(FLASH_INFO_SIZE_OFFSET, &info->capacity_bytes) != 0) {
        return -1;
    }

    return hal_flash_info_valid(info) ? 0 : -1;
}

static void hal_flash_capture_qspi1_diag(volatile be_u1000_qspi_t *qspi,
                                         uint32_t status,
                                         const uint8_t id[QSPI_FLASH_JEDEC_LEN],
                                         uint32_t received,
                                         uint32_t xip_was_enabled)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    g_flash_diag.pclk1en = cru->PCLK1EN;
    g_flash_diag.syscr0 = cru->SYSCR0;
    g_flash_diag.qspienr = qspi->QSPIENR;
    g_flash_diag.ser = qspi->SER;
    g_flash_diag.ctrlr0 = qspi->CTRLR0;
    g_flash_diag.ctrlr1 = qspi->CTRLR1;
    g_flash_diag.spi_ctrlr0 = qspi->SPI_CTRLR0;
    g_flash_diag.baudr = qspi->BAUDR;
    g_flash_diag.sr = qspi->SR;
    g_flash_diag.isr = qspi->ISR;
    g_flash_diag.risr = qspi->RISR;
    g_flash_diag.txflr = qspi->TXFLR;
    g_flash_diag.rxflr = qspi->RXFLR;
    g_flash_diag.jedec_raw = ((uint32_t)id[0] << 16) |
                             ((uint32_t)id[1] << 8) |
                             (uint32_t)id[2];
    g_flash_diag.received = received;
    g_flash_diag.status = status;
    g_flash_diag.xip_was_enabled = xip_was_enabled;
}

static int hal_flash_wait_set(volatile uint32_t *reg, uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((*reg & mask) == mask) {
            return 0;
        }
    }

    return -1;
}

static int hal_flash_wait_clear(volatile uint32_t *reg, uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((*reg & mask) == 0u) {
            return 0;
        }
    }

    return -1;
}

static void hal_flash_qspi1_enable_clock(void)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    cru->PCLK1EN |= CRU_PCLK1_QSPI1_CLKEN |
                    CRU_PCLK1_GPIO1_CLKEN |
                    CRU_PCLK1_QSPI1_RSTN;
}

static uint32_t hal_flash_qspi1_xip_enabled(void)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    return (cru->SYSCR0 & CRU_SYSCR0_XIP1EN) != 0u;
}

static void hal_flash_qspi1_disable_xip(volatile be_u1000_qspi_t *qspi)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    cru->SYSCR0 &= ~CRU_SYSCR0_XIP1EN;
    qspi->SER = QSPI_SER_NONE;
    qspi->QSPIENR = 0u;
    qspi->SPI_CTRLR0 = QSPI_SPI_CTRLR0_ADDR_24;
    qspi->CTRLR0 = QSPI_CTRLR0_DFS_8BIT;
}

static void hal_flash_qspi1_enable_xip(volatile be_u1000_qspi_t *qspi)
{
    volatile be_u1000_cru_t *cru = (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;

    qspi->SER = QSPI_SER_NONE;
    qspi->QSPIENR = 0u;
    qspi->CTRLR0 = QSPI_CTRLR0_TMOD_RX |
                   QSPI_CTRLR0_DFS_32BIT |
                   QSPI_CTRLR0_SPI_FRF_QUAD |
                   QSPI_CTRLR0_SSTE |
                   QSPI_CTRLR0_SECONV;
    qspi->CTRLR1 = 0u;
    qspi->SPI_CTRLR0 = QSPI_SPI_CTRLR0_TRANS_BOTH |
                       QSPI_SPI_CTRLR0_ADDR_24 |
                       QSPI_SPI_CTRLR0_INST_0;
    qspi->QSPIENR = QSPI_QSPIENR_ENABLE;
    qspi->SER = QSPI_SER_0;
    cru->SYSCR0 |= CRU_SYSCR0_XIP1EN;
}

static int hal_flash_qspi1_read_jedec(uint32_t *jedec_id)
{
    volatile be_u1000_qspi_t *qspi = (volatile be_u1000_qspi_t *)BE_U1000_QSPI1_CTRL_BASE;
    uint8_t id[QSPI_FLASH_JEDEC_LEN] = {0};
    uint32_t received = 0;
    uint32_t timeout = QSPI_FLASH_TIMEOUT;
    uint32_t was_xip_enabled;
    uint32_t status = FLASH_DIAG_STATUS_TFNF_TIMEOUT;

    if (jedec_id == 0) {
        g_flash_diag.status = FLASH_DIAG_STATUS_BAD_ARG;
        return -1;
    }

    hal_flash_qspi1_enable_clock();
    was_xip_enabled = hal_flash_qspi1_xip_enabled();
    hal_flash_qspi1_disable_xip(qspi);

    qspi->BAUDR = QSPI_FLASH_DEFAULT_BAUDR;
    qspi->SER = QSPI_SER_NONE;
    qspi->CTRLR0 = QSPI_CTRLR0_TMOD_EEPROM | QSPI_CTRLR0_DFS_8BIT;
    qspi->SPI_CTRLR0 = QSPI_SPI_CTRLR0_TRANS_STD |
                       QSPI_SPI_CTRLR0_ADDR_0 |
                       QSPI_SPI_CTRLR0_INST_8;
    qspi->CTRLR1 = QSPI_FLASH_JEDEC_LEN - 1u;
    qspi->RXFTLR = 0u;
    qspi->TXFTLR = 0u;
    qspi->QSPIENR = QSPI_QSPIENR_ENABLE;

    if (hal_flash_wait_set(&qspi->SR, QSPI_SR_TFNF, QSPI_FLASH_TIMEOUT) == 0) {
        qspi->DR[0] = QSPI_FLASH_CMD_RDID;
        qspi->SER = QSPI_SER_0;

        while (received < QSPI_FLASH_JEDEC_LEN && timeout-- != 0u) {
            if ((qspi->SR & QSPI_SR_RFNE) != 0u) {
                id[received++] = (uint8_t)(qspi->DR[0] & 0xFFu);
            } else if ((qspi->SR & QSPI_SR_BUSY) == 0u &&
                       (qspi->SR & QSPI_SR_TFE) != 0u) {
                break;
            }
        }
        status = (received == QSPI_FLASH_JEDEC_LEN) ?
                 FLASH_DIAG_STATUS_INVALID_ID :
                 FLASH_DIAG_STATUS_SHORT_RX;
    }

    qspi->SER = QSPI_SER_NONE;
    (void)hal_flash_wait_clear(&qspi->SR, QSPI_SR_BUSY, QSPI_FLASH_TIMEOUT);
    qspi->QSPIENR = 0u;

    if (received == QSPI_FLASH_JEDEC_LEN &&
        id[0] != 0u &&
        (id[0] != 0xFFu || id[1] != 0xFFu || id[2] != 0xFFu)) {
        status = FLASH_DIAG_STATUS_OK;
    }
    hal_flash_capture_qspi1_diag(qspi, status, id, received, was_xip_enabled);

    if (was_xip_enabled != 0u) {
        hal_flash_qspi1_enable_xip(qspi);
    }

    if (received != QSPI_FLASH_JEDEC_LEN ||
        id[0] == 0u ||
        (id[0] == 0xFFu && id[1] == 0xFFu && id[2] == 0xFFu)) {
        return -1;
    }

    *jedec_id = ((uint32_t)id[0] << 16) |
                ((uint32_t)id[1] << 8) |
                (uint32_t)id[2];
    return 0;
}

static int hal_flash_get_qspi1_info(hal_flash_info_t *info)
{
    uint8_t capacity_pow;

    if (g_flash_base != (volatile const uint8_t *)BE_U1000_QSPI1_BASE ||
        g_flash_size != BE_U1000_QSPI1_SIZE) {
        return -1;
    }

    if (hal_flash_qspi1_read_jedec(&info->jedec_id) != 0) {
        return -1;
    }

    capacity_pow = (uint8_t)(info->jedec_id & 0xFFu);
    info->page_size = BE_U1000_DIAG_FLASH_PAGE_SIZE;
    info->sector_size = BE_U1000_DIAG_FLASH_SECTOR_SIZE;
    info->capacity_bytes = (capacity_pow < 32u) ? (1u << capacity_pow) : g_flash_size;
    if (info->capacity_bytes == 0u || info->capacity_bytes > g_flash_size) {
        info->capacity_bytes = g_flash_size;
    }

    return hal_flash_info_valid(info) ? 0 : -1;
}

int hal_flash_init(uintptr_t base, uint32_t size)
{
    if (base == 0 || size == 0) {
        return -1;
    }

    g_flash_base = (volatile const uint8_t *)base;
    g_flash_size = size;
    return 0;
}

int hal_flash_read(uint32_t offset, void *buf, size_t len)
{
    volatile const uint8_t *src;
    uint8_t *dst;
    size_t i;

    if (len == 0u) {
        return 0;
    }

    if (buf == 0) {
        return -1;
    }

    if (hal_flash_range_valid(offset, len) != 0) {
        return -1;
    }

    src = g_flash_base + offset;
    dst = (uint8_t *)buf;

    for (i = 0; i < len; ++i) {
        dst[i] = src[i];
    }

    return 0;
}

int hal_flash_read_u32(uint32_t offset, uint32_t *value)
{
    if (value == 0) {
        return -1;
    }

    return hal_flash_read(offset, value, sizeof(*value));
}

int hal_flash_get_info(hal_flash_info_t *info)
{
    if (info == 0) {
        return -1;
    }

    info->jedec_id = 0u;
    info->page_size = 0u;
    info->sector_size = 0u;
    info->capacity_bytes = 0u;

    if (hal_flash_read_header_info(info) == 0) {
        return 0;
    }

    return hal_flash_get_qspi1_info(info);
}

int hal_flash_get_diag(hal_flash_diag_t *diag)
{
    if (diag == 0) {
        return -1;
    }

    *diag = g_flash_diag;
    return 0;
}
