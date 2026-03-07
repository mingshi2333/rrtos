#include <stdint.h>

#include "hal_i2c.h"

typedef struct {
    volatile uint32_t CON;
    volatile uint32_t TAR;
    volatile uint32_t SAR;
    volatile uint32_t HS_MADDR;
    volatile uint32_t DATA_CMD;
    volatile uint32_t SS_SCL_HCNT;
    volatile uint32_t SS_SCL_LCNT;
    volatile uint32_t FS_SCL_HCNT;
    volatile uint32_t FS_SCL_LCNT;
    volatile uint32_t HS_SCL_HCNT;
    volatile uint32_t HS_SCL_LCNT;
    volatile uint32_t INTR_STAT;
    volatile uint32_t INTR_MASK;
    volatile uint32_t RAW_INTR_STAT;
    volatile uint32_t RX_TL;
    volatile uint32_t TX_TL;
    volatile uint32_t CLR_INTR;
    volatile uint32_t CLR_RX_UNDER;
    volatile uint32_t CLR_RX_OVER;
    volatile uint32_t CLR_TX_OVER;
    volatile uint32_t CLR_RD_REQ;
    volatile uint32_t CLR_TX_ABRT;
    volatile uint32_t CLR_RX_DONE;
    volatile uint32_t CLR_ACTIVITY;
    volatile uint32_t CLR_STOP_DET;
    volatile uint32_t CLR_START_DET;
    volatile uint32_t CLR_GEN_CALL;
    volatile uint32_t ENABLE;
    volatile uint32_t STATUS;
    volatile uint32_t TXFLR;
    volatile uint32_t RXFLR;
    volatile uint32_t SDA_HOLD;
    volatile uint32_t TX_ABRT_SOURCE;
    volatile uint32_t SLV_DATA_NACK_ONLY;
    volatile uint32_t DMA_CR;
    volatile uint32_t DMA_TDLR;
    volatile uint32_t DMA_RDLR;
    volatile uint32_t SDA_SETUP;
} be_u1000_i2c_t;

static volatile be_u1000_i2c_t *g_i2c;

#define I2C_CON_MASTER_MODE        (1u << 0)
#define I2C_CON_SPEED_STD          (1u << 1)
#define I2C_CON_SPEED_FAST         (2u << 1)
#define I2C_CON_RESTART_EN         (1u << 5)
#define I2C_CON_SLAVE_DISABLE      (1u << 6)

#define I2C_ENABLE_EN              (1u << 0)

#define I2C_STATUS_ACTIVITY        (1u << 0)
#define I2C_STATUS_TFNF            (1u << 1)
#define I2C_STATUS_RFNE            (1u << 3)

#define I2C_DATA_CMD_READ          (1u << 8)

#define I2C_RAW_INTR_TX_ABRT       (1u << 6)

static int hal_i2c_wait_status_set(uint32_t mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; i++) {
        if ((g_i2c->STATUS & mask) == mask) {
            return 0;
        }
    }

    return -1;
}

static int hal_i2c_wait_idle(uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; i++) {
        if ((g_i2c->STATUS & I2C_STATUS_ACTIVITY) == 0u && g_i2c->TXFLR == 0u) {
            return 0;
        }
    }

    return -1;
}

static int hal_i2c_check_abort(void)
{
    if ((g_i2c->RAW_INTR_STAT & I2C_RAW_INTR_TX_ABRT) != 0u) {
        (void)g_i2c->CLR_TX_ABRT;
        return -1;
    }
    return 0;
}

int hal_i2c_init(uintptr_t base, uint32_t bus_hz)
{
    uint32_t half_period;
    uint32_t con;

    g_i2c = (volatile be_u1000_i2c_t *)base;
    if (g_i2c == 0) {
        return -1;
    }

    if (bus_hz == 0u) {
        bus_hz = 100000u;
    }

    g_i2c->ENABLE = 0u;

    con = I2C_CON_MASTER_MODE | I2C_CON_RESTART_EN | I2C_CON_SLAVE_DISABLE;
    if (bus_hz <= 100000u) {
        con |= I2C_CON_SPEED_STD;
    } else {
        con |= I2C_CON_SPEED_FAST;
    }
    g_i2c->CON = con;

    half_period = 50000000u / (bus_hz * 2u);
    if (half_period < 8u) {
        half_period = 8u;
    }

    if (bus_hz <= 100000u) {
        g_i2c->SS_SCL_HCNT = half_period;
        g_i2c->SS_SCL_LCNT = half_period;
    } else {
        g_i2c->FS_SCL_HCNT = half_period;
        g_i2c->FS_SCL_LCNT = half_period;
    }

    g_i2c->TX_TL = 0u;
    g_i2c->RX_TL = 0u;
    (void)g_i2c->CLR_INTR;
    g_i2c->ENABLE = I2C_ENABLE_EN;

    return 0;
}

int hal_i2c_write_byte(uint16_t addr, uint8_t data, uint32_t timeout)
{
    if (g_i2c == 0) {
        return -1;
    }

    g_i2c->TAR = (uint32_t)(addr & 0x3FFu);

    if (hal_i2c_wait_status_set(I2C_STATUS_TFNF, timeout) != 0) {
        return -1;
    }

    g_i2c->DATA_CMD = (uint32_t)data;

    if (hal_i2c_check_abort() != 0) {
        return -1;
    }

    if (hal_i2c_wait_idle(timeout) != 0) {
        return -1;
    }

    return 0;
}

int hal_i2c_read_byte(uint16_t addr, uint8_t *data, uint32_t timeout)
{
    uint32_t value;

    if (g_i2c == 0 || data == 0) {
        return -1;
    }

    g_i2c->TAR = (uint32_t)(addr & 0x3FFu);

    if (hal_i2c_wait_status_set(I2C_STATUS_TFNF, timeout) != 0) {
        return -1;
    }

    g_i2c->DATA_CMD = I2C_DATA_CMD_READ;

    if (hal_i2c_check_abort() != 0) {
        return -1;
    }

    if (hal_i2c_wait_status_set(I2C_STATUS_RFNE, timeout) != 0) {
        return -1;
    }

    value = g_i2c->DATA_CMD & 0xFFu;
    *data = (uint8_t)value;

    return 0;
}

int hal_i2c_probe(uint16_t addr, uint32_t timeout)
{
    uint8_t data;

    return hal_i2c_read_byte(addr, &data, timeout);
}
