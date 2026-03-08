#include "hal_board.h"

#include "hal_clint.h"
#include "hal_irq.h"
#include "hal_uart.h"
#include "os_config.h"

typedef enum {
    HAL_BOARD_GPIO_USER_LED = 0,
    HAL_BOARD_GPIO_USER_BUTTON,
} hal_board_gpio_role_t;

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
    hal_irq_init_clic(BE_U1000_CLIC_BASE);
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

static void hal_board_fill_diag_config(hal_board_diag_config_t *config) {
    if (!config) {
        return;
    }

    config->available = false;
    config->spi_base = 0;
    config->spi_baud_div = 0;
    config->spi_probe_tx = 0;
    config->i2c_base = 0;
    config->i2c_bus_hz = 0;
    config->spi_label = NULL;
    config->spi_route = NULL;
    config->i2c_label = NULL;
    config->i2c_route = NULL;

#if defined(CONFIG_BOARD_BE_U1000)
    config->spi_base = BE_U1000_DIAG_SPI_BASE;
    config->spi_baud_div = BE_U1000_DIAG_SPI_BAUD_DIV;
    config->spi_probe_tx = 0xA5u;
    config->i2c_base = BE_U1000_DIAG_I2C_BASE;
    config->i2c_bus_hz = BE_U1000_DIAG_I2C_BUS_HZ;
    config->spi_label = "SPI1 controller";
    config->spi_route = "PA8..PA11";
    config->i2c_label = "I2C0 controller";
    config->i2c_route = "PA4/PA5";
    config->available = true;
#endif
}

static void hal_board_fill_gpio_resource(hal_board_gpio_role_t role,
                                         hal_board_gpio_resource_t *resource)
{
    if (!resource) {
        return;
    }

    resource->base = 0;
    resource->pin = 0;
    resource->label = NULL;
    resource->location = NULL;
    resource->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    switch (role) {
    case HAL_BOARD_GPIO_USER_LED:
        resource->base = BE_U1000_USER_LED_GPIO_BASE;
        resource->pin = BE_U1000_USER_LED_GPIO_PIN;
        resource->label = "USER_LED GPIO";
        resource->location = "PC0";
        resource->available = true;
        break;
    case HAL_BOARD_GPIO_USER_BUTTON:
        resource->base = BE_U1000_USER_BTN_GPIO_BASE;
        resource->pin = BE_U1000_USER_BTN_GPIO_PIN;
        resource->label = "USER_BUTTON GPIO";
        resource->location = "PC13";
        resource->available = true;
        break;
    default:
        break;
    }
#else
    (void)role;
#endif
}

static void hal_board_fill_flash_profile(hal_board_flash_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->ctrl_base = 0;
    profile->window_base = 0;
    profile->window_size = 0;
    profile->window_sample_offset = 0;
    profile->window_sample_words = 0;
    profile->expected_signature[0] = 0;
    profile->expected_signature[1] = 0;
    profile->expected_signature[2] = 0;
    profile->expected_signature[3] = 0;
    profile->jedec_id = 0;
    profile->page_size = 0;
    profile->sector_size = 0;
    profile->capacity_bytes = 0;
    profile->label = NULL;
    profile->route = NULL;
    profile->ready_note = NULL;
    profile->window_read_note = NULL;
    profile->window_match_note = NULL;
    profile->window_capture_note = NULL;
    profile->identify_note = NULL;
    profile->identify_match_note = NULL;
    profile->identify_capture_note = NULL;
    profile->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    profile->ctrl_base = BE_U1000_QSPI1_CTRL_BASE;
    profile->window_base = BE_U1000_QSPI1_BASE;
    profile->window_size = BE_U1000_QSPI1_SIZE;
    profile->window_sample_offset = 0u;
    profile->window_sample_words = 4u;
    profile->expected_signature[0] = 0x31505351u;
    profile->expected_signature[1] = 0x5F4C444Du;
    profile->expected_signature[2] = 0x00010010u;
    profile->expected_signature[3] = 0xA55A3CC3u;
    profile->jedec_id = BE_U1000_DIAG_FLASH_JEDEC_ID;
    profile->page_size = BE_U1000_DIAG_FLASH_PAGE_SIZE;
    profile->sector_size = BE_U1000_DIAG_FLASH_SECTOR_SIZE;
    profile->capacity_bytes = BE_U1000_DIAG_FLASH_CAPACITY_BYTES;
    profile->label = "QSPI1";
    profile->route = "PB0..PB5";
    profile->ready_note = "flash window mapped";
    profile->window_read_note = "flash HAL + sim signature";
    profile->window_match_note = "expected boot signature matched";
    profile->window_capture_note = "flash sample captured for evidence";
    profile->identify_note = "sim flash header";
    profile->identify_match_note = "flash geometry matched profile";
    profile->identify_capture_note = "flash identify sample captured for evidence";
    profile->available = true;
#endif
}

static void hal_board_fill_canfd_profile(uint32_t index, hal_board_canfd_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->base = 0;
    profile->bitrate = 0;
    profile->frame_id = 0;
    profile->irq_num = 0;
    profile->frame_len = 0;
    profile->payload_seed = 0;
    profile->pinmux_group = HAL_BOARD_PINMUX_GROUP_CANFD0;
    profile->label = NULL;
    profile->route = NULL;
    profile->loopback_note = NULL;
    profile->internal_loopback = false;
    profile->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    profile->bitrate = BE_U1000_DIAG_CANFD_BITRATE;
    profile->frame_len = BE_U1000_DIAG_CANFD_FRAME_LEN;

    if (index == 0u) {
        profile->base = BE_U1000_DIAG_CANFD0_BASE;
        profile->frame_id = BE_U1000_DIAG_CANFD0_FRAME_ID;
        profile->irq_num = BE_U1000_IRQ_CANFD0;
        profile->pinmux_group = HAL_BOARD_PINMUX_GROUP_CANFD0;
        profile->payload_seed = 0xA0u;
        profile->label = "CANFD0";
        profile->route = "PA14/PA15";
        profile->loopback_note = "internal loopback";
        profile->internal_loopback = true;
        profile->available = true;
    } else if (index == 1u) {
        profile->base = BE_U1000_DIAG_CANFD1_BASE;
        profile->frame_id = BE_U1000_DIAG_CANFD1_FRAME_ID;
        profile->irq_num = BE_U1000_IRQ_CANFD1;
        profile->pinmux_group = HAL_BOARD_PINMUX_GROUP_CANFD1;
        profile->payload_seed = 0xB0u;
        profile->label = "CANFD1";
        profile->route = "PB6/PB7";
        profile->loopback_note = "internal loopback";
        profile->internal_loopback = true;
        profile->available = true;
    }
#else
    (void)index;
#endif
}

void hal_board_get_execution_profile(hal_board_execution_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->schedulable_cores = 0;
    profile->reserved_helper_mask = 0;
    profile->role_plan = NULL;
    profile->task_map_smp = NULL;
    profile->task_map_single = NULL;
    profile->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    profile->schedulable_cores = 2u;
    profile->reserved_helper_mask = 1u << 2;
    profile->role_plan = "core0=boot core1=worker core2=reserved";
    profile->task_map_smp = "control->core0 worker->core1 core2=reserved";
    profile->task_map_single = "control+worker share single core";
    profile->available = true;
#endif
}

void hal_board_get_selftest_profile(hal_board_selftest_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->console_pinmux_group = HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0;
    profile->led_pinmux_group = HAL_BOARD_PINMUX_GROUP_USER_LED;
    profile->button_pinmux_group = HAL_BOARD_PINMUX_GROUP_USER_BUTTON;
    profile->i2c_pinmux_group = HAL_BOARD_PINMUX_GROUP_HEADER_I2C0;
    profile->spi_pinmux_group = HAL_BOARD_PINMUX_GROUP_HEADER_SPI1;
    profile->flash_pinmux_group = HAL_BOARD_PINMUX_GROUP_QSPI1;
    profile->canfd_count = 0;
    hal_board_fill_gpio_resource(HAL_BOARD_GPIO_USER_LED, &profile->led_gpio);
    hal_board_fill_gpio_resource(HAL_BOARD_GPIO_USER_BUTTON, &profile->button_gpio);
    hal_board_fill_diag_config(&profile->diag);
    hal_board_fill_flash_profile(&profile->flash);
    hal_board_fill_canfd_profile(0u, &profile->canfd[0]);
    hal_board_fill_canfd_profile(1u, &profile->canfd[1]);

#if defined(CONFIG_BOARD_BE_U1000)
    profile->canfd_count = HAL_BOARD_CANFD_CONTROLLER_COUNT;
    profile->available = true;
#else
    profile->available = false;
#endif
}

const char *hal_board_pinmux_group_name(hal_board_pinmux_group_t group)
{
    switch (group) {
    case HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0:
        return "UART0";
    case HAL_BOARD_PINMUX_GROUP_USER_LED:
        return "USER_LED";
    case HAL_BOARD_PINMUX_GROUP_USER_BUTTON:
        return "USER_BUTTON";
    case HAL_BOARD_PINMUX_GROUP_HEADER_I2C0:
        return "I2C0 header";
    case HAL_BOARD_PINMUX_GROUP_HEADER_SPI1:
        return "SPI1 header";
    case HAL_BOARD_PINMUX_GROUP_QSPI1:
        return "QSPI1";
    case HAL_BOARD_PINMUX_GROUP_CANFD0:
        return "CANFD0";
    case HAL_BOARD_PINMUX_GROUP_CANFD1:
        return "CANFD1";
    default:
        return "unknown";
    }
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
