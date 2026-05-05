#include <stdbool.h>
#include <stdint.h>

#include "hal_board_be_u1000_internal.h"
#include "os_config.h"

#if defined(CONFIG_BOARD_BE_U1000)
#include "board_config.h"

typedef struct {
    volatile uint32_t CLKSEL;
    volatile uint32_t PLLSET;
    volatile uint32_t CLKCR0;
    volatile uint32_t PCLK0EN;
    volatile uint32_t PCLK1EN;
    volatile uint32_t PCLK2EN;
    volatile uint32_t SYSCR0;
    volatile uint32_t SYSCR1;
    volatile uint32_t SYSCR2;
    volatile uint32_t PRIOR0;
    volatile uint32_t PRIOR1;
    volatile uint32_t IOPUCR0;
    volatile uint32_t IOPUCR1;
    volatile uint32_t IOPDCR0;
    volatile uint32_t IOPDCR1;
    volatile uint32_t IOAFCR0;
    volatile uint32_t IOAFCR1;
    volatile uint32_t IOAFCR2;
    volatile uint32_t IOAFCR3;
    volatile uint32_t IOAFCR4;
    volatile uint32_t IOAFCR5;
    volatile uint32_t IODSCR0;
    volatile uint32_t IODSCR1;
    volatile uint32_t IODSCR2;
} be_u1000_cru_t;

static volatile be_u1000_cru_t *be_u1000_cru(void)
{
    return (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;
}

static void be_u1000_enable_apb0(unsigned bit)
{
    be_u1000_cru()->PCLK0EN |= (1u << bit);
}

static void be_u1000_enable_apb1(unsigned bit)
{
    be_u1000_cru()->PCLK1EN |= (1u << bit);
}

static void be_u1000_enable_apb2(unsigned bit)
{
    be_u1000_cru()->PCLK2EN |= (1u << bit);
}

typedef struct {
    unsigned port;
    unsigned pin;
    unsigned af;
    bool input_enable;
} be_u1000_pinmux_entry_t;

static volatile uint32_t *be_u1000_ioafcr_reg(unsigned port, unsigned pin)
{
    return ((volatile uint32_t *)&be_u1000_cru()->IOAFCR0) + (port * 2u) + (pin / 8u);
}

static volatile uint32_t *be_u1000_iopull_reg(volatile uint32_t *base, unsigned port)
{
    return base + (port / 2u);
}

static volatile uint32_t *be_u1000_iodscr_reg(unsigned port)
{
    return ((volatile uint32_t *)&be_u1000_cru()->IODSCR0) + port;
}

static void be_u1000_set_pin_af(unsigned port, unsigned pin, unsigned af, bool input_enable)
{
    const uint32_t shift = (pin & 7u) * 4u;
    volatile uint32_t *reg = be_u1000_ioafcr_reg(port, pin);
    uint32_t value = *reg;

    value &= ~(0x7u << shift);
    value |= (af & 0x7u) << shift;
    if (input_enable) {
        value |= 1u << (shift + 3u);
    } else {
        value &= ~(1u << (shift + 3u));
    }

    *reg = value;
}

static void be_u1000_set_pin_pull_up(unsigned port, unsigned pin)
{
    const uint32_t bit = 1u << (((port & 1u) * 16u) + pin);
    volatile uint32_t *pull_up = be_u1000_iopull_reg(&be_u1000_cru()->IOPUCR0, port);
    volatile uint32_t *pull_down = be_u1000_iopull_reg(&be_u1000_cru()->IOPDCR0, port);

    *pull_up |= bit;
    *pull_down &= ~bit;
}

static void be_u1000_set_pin_drive_strength(unsigned port, unsigned pin, unsigned strength)
{
    const uint32_t shift = pin * 2u;
    volatile uint32_t *reg = be_u1000_iodscr_reg(port);
    uint32_t value = *reg;

    value &= ~(0x3u << shift);
    value |= (strength & 0x3u) << shift;
    *reg = value;
}

static void be_u1000_apply_qspi1_electrical(void)
{
    unsigned pin;

    for (pin = BE_U1000_QSPI1_CS_PIN; pin <= BE_U1000_QSPI1_IO3_PIN; ++pin) {
        be_u1000_set_pin_drive_strength(BE_U1000_QSPI1_PORT, pin, 3u);
        be_u1000_set_pin_pull_up(BE_U1000_QSPI1_PORT, pin);
    }
}

static void be_u1000_apply_pinmux_entries(
    const be_u1000_pinmux_entry_t *entries,
    unsigned count
)
{
    unsigned i;

    for (i = 0; i < count; ++i) {
        be_u1000_set_pin_af(
            entries[i].port,
            entries[i].pin,
            entries[i].af,
            entries[i].input_enable
        );
    }
}

int hal_board_be_u1000_apply_pinmux_group(hal_board_pinmux_group_t group)
{
    static const be_u1000_pinmux_entry_t uart0_entries[] = {
        {BE_U1000_CONSOLE_UART_PORT, BE_U1000_CONSOLE_UART_TX_PIN, BE_U1000_CONSOLE_UART_AF, false},
        {BE_U1000_CONSOLE_UART_PORT, BE_U1000_CONSOLE_UART_RX_PIN, BE_U1000_CONSOLE_UART_AF, true},
    };
    static const be_u1000_pinmux_entry_t user_led_entries[] = {
        {BE_U1000_USER_LED_PORT, BE_U1000_USER_LED_PIN, BE_U1000_USER_LED_AF, false},
    };
    static const be_u1000_pinmux_entry_t user_button_entries[] = {
        {BE_U1000_USER_BTN_PORT, BE_U1000_USER_BTN_PIN, BE_U1000_USER_BTN_AF, true},
    };
    static const be_u1000_pinmux_entry_t i2c0_entries[] = {
        {BE_U1000_HEADER_I2C_SCL_PORT, BE_U1000_HEADER_I2C_SCL_PIN, BE_U1000_HEADER_I2C_AF, true},
        {BE_U1000_HEADER_I2C_SDA_PORT, BE_U1000_HEADER_I2C_SDA_PIN, BE_U1000_HEADER_I2C_AF, true},
    };
    static const be_u1000_pinmux_entry_t spi1_entries[] = {
        {BE_U1000_HEADER_SPI_CS_PORT, BE_U1000_HEADER_SPI_CS_PIN, BE_U1000_HEADER_SPI_AF, false},
        {BE_U1000_HEADER_SPI_SCK_PORT, BE_U1000_HEADER_SPI_SCK_PIN, BE_U1000_HEADER_SPI_AF, false},
        {BE_U1000_HEADER_SPI_MOSI_PORT, BE_U1000_HEADER_SPI_MOSI_PIN, BE_U1000_HEADER_SPI_AF, false},
        {BE_U1000_HEADER_SPI_MISO_PORT, BE_U1000_HEADER_SPI_MISO_PIN, BE_U1000_HEADER_SPI_AF, true},
    };
    static const be_u1000_pinmux_entry_t qspi1_entries[] = {
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_CS_PIN, BE_U1000_QSPI1_AF, false},
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_SCK_PIN, BE_U1000_QSPI1_AF, false},
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_IO0_PIN, BE_U1000_QSPI1_AF, true},
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_IO1_PIN, BE_U1000_QSPI1_AF, true},
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_IO2_PIN, BE_U1000_QSPI1_AF, true},
        {BE_U1000_QSPI1_PORT, BE_U1000_QSPI1_IO3_PIN, BE_U1000_QSPI1_AF, true},
    };
    static const be_u1000_pinmux_entry_t canfd0_entries[] = {
        {BE_U1000_CANFD0_PORT, BE_U1000_CANFD0_PIN_A, BE_U1000_CANFD0_AF, true},
        {BE_U1000_CANFD0_PORT, BE_U1000_CANFD0_PIN_B, BE_U1000_CANFD0_AF, true},
    };
    static const be_u1000_pinmux_entry_t canfd1_entries[] = {
        {BE_U1000_CANFD1_PORT, BE_U1000_CANFD1_PIN_A, BE_U1000_CANFD1_AF, true},
        {BE_U1000_CANFD1_PORT, BE_U1000_CANFD1_PIN_B, BE_U1000_CANFD1_AF, true},
    };

    switch (group) {
    case HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0:
        be_u1000_enable_apb0(3u);
        be_u1000_enable_apb0(11u);
        be_u1000_apply_pinmux_entries(uart0_entries, 2u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_USER_LED:
        be_u1000_enable_apb2(7u);
        be_u1000_apply_pinmux_entries(user_led_entries, 1u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_USER_BUTTON:
        be_u1000_enable_apb2(7u);
        be_u1000_apply_pinmux_entries(user_button_entries, 1u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_HEADER_I2C0:
        be_u1000_enable_apb0(6u);
        be_u1000_enable_apb0(11u);
        be_u1000_apply_pinmux_entries(i2c0_entries, 2u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_HEADER_SPI1:
        be_u1000_enable_apb0(2u);
        be_u1000_enable_apb0(11u);
        be_u1000_apply_pinmux_entries(spi1_entries, 4u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_QSPI1:
        be_u1000_enable_apb1(0u);
        be_u1000_enable_apb1(11u);
        be_u1000_apply_pinmux_entries(qspi1_entries, 6u);
        be_u1000_apply_qspi1_electrical();
        return 0;
    case HAL_BOARD_PINMUX_GROUP_CANFD0:
        be_u1000_enable_apb0(13u);
        be_u1000_enable_apb0(11u);
        be_u1000_apply_pinmux_entries(canfd0_entries, 2u);
        return 0;
    case HAL_BOARD_PINMUX_GROUP_CANFD1:
        be_u1000_enable_apb1(13u);
        be_u1000_enable_apb1(11u);
        be_u1000_apply_pinmux_entries(canfd1_entries, 2u);
        return 0;
    default:
        return -1;
    }
}

void hal_board_be_u1000_pinmux_init(void)
{
    (void)hal_board_be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0);
    (void)hal_board_be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_USER_LED);
    (void)hal_board_be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_USER_BUTTON);
    /*
     * Optional I2C/SPI/QSPI groups stay deferred until selftest or another
     * explicit consumer requests them. This keeps early board init limited to
     * the mandatory console and user-visible GPIO path.
     */
}
#endif
