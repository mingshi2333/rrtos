#include "hal_board.h"

#include "hal_clint.h"
#include "hal_irq.h"
#include "hal_uart.h"
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

static int be_u1000_apply_pinmux_group(hal_board_pinmux_group_t group)
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

static void be_u1000_board_pinmux_init(void)
{
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0);
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_USER_LED);
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_USER_BUTTON);
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_HEADER_I2C0);
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_HEADER_SPI1);
    (void)be_u1000_apply_pinmux_group(HAL_BOARD_PINMUX_GROUP_QSPI1);
}
#endif

void hal_board_init(void) {
#if defined(CONFIG_BOARD_BE_U1000)
    be_u1000_board_pinmux_init();
#endif

    hal_uart_init(OS_CFG_UART_BASE, OS_CFG_UART_BAUD);
    hal_clint_init(OS_CFG_CLINT_BASE);

#if OS_CFG_IRQ_MODEL_CLIC
    hal_irq_init_clic(0);
#else
    hal_irq_init_plic(OS_CFG_PLIC_BASE);
#endif
}

const char *hal_board_name(void) {
#if defined(CONFIG_BOARD_BE_U1000)
    return "BE-U1000/EVU-BA";
#else
    return "qemu_virt";
#endif
}

void hal_board_print_banner(void) {
#if defined(CONFIG_BOARD_BE_U1000)
    os_print("\n========================================\n");
    os_print("  RV-AIOS v1.0.0 on BE-U1000\n");
    os_print("  Baikal Electronics RISC-V RV32IMAFC\n");
    os_print("========================================\n\n");
#else
    os_print("[BOOT] Board: %s\n", hal_board_name());
#endif
}

void hal_board_get_diag_config(hal_board_diag_config_t *config) {
    if (!config) {
        return;
    }

    config->available = false;
    config->gpio_base = 0;
    config->gpio_pin = 0;
    config->spi_base = 0;
    config->spi_baud_div = 0;
    config->i2c_base = 0;
    config->i2c_bus_hz = 0;

#if defined(CONFIG_BOARD_BE_U1000)
    config->gpio_base = BE_U1000_DIAG_GPIO_BASE;
    config->gpio_pin = BE_U1000_DIAG_GPIO_PIN;
    config->spi_base = BE_U1000_DIAG_SPI_BASE;
    config->spi_baud_div = BE_U1000_DIAG_SPI_BAUD_DIV;
    config->i2c_base = BE_U1000_DIAG_I2C_BASE;
    config->i2c_bus_hz = BE_U1000_DIAG_I2C_BUS_HZ;
    config->available = true;
#endif
}

int hal_board_apply_pinmux_group(hal_board_pinmux_group_t group)
{
#if defined(CONFIG_BOARD_BE_U1000)
    return be_u1000_apply_pinmux_group(group);
#else
    (void)group;
    return -1;
#endif
}
