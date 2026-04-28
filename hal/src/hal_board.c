#include <stdbool.h>
#include <stdint.h>

#include "hal_board_smp.h"

#ifndef RRTOS_HAL_HAS_BOARD_SELFTEST
#define RRTOS_HAL_HAS_BOARD_SELFTEST 0
#endif

#if defined(CONFIG_BOARD_BE_U1000) && RRTOS_HAL_HAS_BOARD_SELFTEST
#include "hal_canfd.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#endif

#include "hal_clint.h"
#include "hal_irq.h"
#include "hal_uart.h"
#include "os_config.h"
#include "os_kernel.h"

#if OS_CFG_SMP_EN
#include "os_smp.h"
#endif

#if RRTOS_HAL_HAS_BOARD_SELFTEST
typedef enum {
    HAL_BOARD_GPIO_USER_LED = 0,
    HAL_BOARD_GPIO_USER_BUTTON,
} hal_board_gpio_role_t;

typedef struct {
    uintptr_t spi_base;
    uint32_t spi_baud_div;
    uint8_t spi_probe_tx;
    uintptr_t i2c_base;
    uint32_t i2c_bus_hz;
    const char *spi_label;
    const char *spi_route;
    const char *i2c_label;
    const char *i2c_route;
    bool available;
} hal_board_diag_config_t;

typedef struct {
    uintptr_t base;
    uint32_t pin;
    const char *label;
    const char *location;
    bool available;
} hal_board_gpio_resource_t;

typedef struct {
    uintptr_t ctrl_base;
    uintptr_t window_base;
    uint32_t window_size;
    uint32_t window_sample_offset;
    uint32_t window_sample_words;
    uint32_t expected_signature[4];
    uint32_t jedec_id;
    uint32_t page_size;
    uint32_t sector_size;
    uint32_t capacity_bytes;
    const char *label;
    const char *route;
    const char *ready_note;
    const char *window_read_note;
    const char *window_match_note;
    const char *window_capture_note;
    const char *identify_note;
    const char *identify_match_note;
    const char *identify_capture_note;
    bool available;
} hal_board_flash_profile_t;

typedef struct {
    uintptr_t base;
    uint32_t bitrate;
    uint32_t frame_id;
    uint32_t irq_num;
    uint32_t frame_len;
    uint8_t payload_seed;
    hal_board_pinmux_group_t pinmux_group;
    const char *label;
    const char *route;
    const char *loopback_note;
    bool internal_loopback;
    bool available;
} hal_board_canfd_profile_t;

#define HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT 2u

typedef struct {
    hal_board_pinmux_group_t console_pinmux_group;
    hal_board_pinmux_group_t led_pinmux_group;
    hal_board_pinmux_group_t button_pinmux_group;
    hal_board_pinmux_group_t i2c_pinmux_group;
    hal_board_pinmux_group_t spi_pinmux_group;
    hal_board_pinmux_group_t flash_pinmux_group;
    uint32_t canfd_count;
    hal_board_gpio_resource_t led_gpio;
    hal_board_gpio_resource_t button_gpio;
    hal_board_diag_config_t diag;
    hal_board_flash_profile_t flash;
    hal_board_canfd_profile_t canfd[HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT];
    bool available;
} hal_board_selftest_profile_t;

#define SELFTEST_SPI_TIMEOUT  100000u
#define SELFTEST_CANFD_TIMEOUT 100000u
#define CANFD_IRQ_SEEN(index) (1u << (index))

#if defined(CONFIG_BOARD_BE_U1000)
static volatile uint32_t g_canfd_irq_seen_mask;
static volatile uint32_t g_canfd_irq_count[HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT];
static hal_board_canfd_profile_t g_canfd_profiles[HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT];
static uint32_t g_canfd_profile_count;
#endif
#endif

typedef struct {
    os_cpu_t control_cpu;
    os_cpu_t worker_cpu;
    os_cpu_t reschedule_probe_cpu;
    os_cpu_t balance_probe_cpu_a;
    os_cpu_t balance_probe_cpu_b;
    bool available;
} hal_board_demo_topology_t;

#if OS_CFG_SMP_EN
static void hal_board_fill_demo_topology(hal_board_demo_topology_t *topology)
{
    if (!topology) {
        return;
    }

    topology->control_cpu = 0u;
    topology->worker_cpu = 0u;
    topology->reschedule_probe_cpu = 0u;
    topology->balance_probe_cpu_a = 0u;
    topology->balance_probe_cpu_b = 0u;
    topology->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    topology->worker_cpu = 1u;
    topology->reschedule_probe_cpu = 1u;
    topology->balance_probe_cpu_a = 0u;
    topology->balance_probe_cpu_b = 1u;
    topology->available = true;
#endif
}
#endif

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
    /*
     * Optional I2C/SPI/QSPI groups stay deferred until selftest or another
     * explicit consumer requests them. This keeps early board init limited to
     * the mandatory console and user-visible GPIO path.
     */
}
#endif

#if defined(CONFIG_BOARD_BE_U1000) && RRTOS_HAL_HAS_BOARD_SELFTEST
static const char *selftest_label(const char *label, const char *fallback)
{
    return label ? label : fallback;
}

static void report_pinmux_result(const char *name, hal_board_pinmux_group_t group)
{
    const char *label = name ? name : hal_board_pinmux_group_name(group);

    if (hal_board_apply_pinmux_group(group) == 0) {
        os_print("[SELFTEST] %s pinmux: OK\n", label);
    } else {
        os_print("[SELFTEST] %s pinmux: FAIL\n", label);
    }
}

static void report_flash_sample(const char *label, const uint32_t *words, uint32_t count)
{
    uint32_t i;

    os_print("[SELFTEST] %s window sample:", label);
    for (i = 0; i < count; ++i) {
        os_print(" 0x%x", words[i]);
    }
    os_print("\n");
}

static bool canfd_frames_match(const hal_canfd_frame_t *lhs, const hal_canfd_frame_t *rhs)
{
    uint8_t i;

    if (lhs->id != rhs->id || lhs->len != rhs->len) {
        return false;
    }

    for (i = 0; i < lhs->len; ++i) {
        if (lhs->data[i] != rhs->data[i]) {
            return false;
        }
    }

    return true;
}

static bool wait_for_canfd_irq(uint32_t irq_mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd_irq_seen_mask & irq_mask) != 0u) {
            return true;
        }
    }

    return false;
}

static void canfd_irq_handler(uint32_t irq_num)
{
    uint32_t index;

    for (index = 0; index < g_canfd_profile_count; ++index) {
        const char *name = selftest_label(g_canfd_profiles[index].label, "CANFD");

        if (irq_num != g_canfd_profiles[index].irq_num) {
            continue;
        }

        (void)hal_canfd_irq_enable(0u);
        (void)hal_canfd_irq_ack(HAL_CANFD_IRQ_TX_COMPLETE |
                                HAL_CANFD_IRQ_RX_READY |
                                HAL_CANFD_IRQ_ERROR);
        g_canfd_irq_seen_mask |= CANFD_IRQ_SEEN(index);
        ++g_canfd_irq_count[index];
        os_print("[IRQ] %s external: OK (irq=%u count=%u)\n",
                 name,
                 irq_num,
                 g_canfd_irq_count[index]);
        break;
    }
}

static void run_gpio_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_gpio_resource_t *led_gpio = &profile->led_gpio;
    const hal_board_gpio_resource_t *button_gpio = &profile->button_gpio;

    report_pinmux_result(NULL, profile->led_pinmux_group);
    report_pinmux_result(NULL, profile->button_pinmux_group);

    hal_gpio_init(led_gpio->base);
    if (led_gpio->available &&
        hal_gpio_set_direction(led_gpio->pin, true) == 0 &&
        hal_gpio_write(led_gpio->pin, true) == 0 &&
        hal_gpio_toggle(led_gpio->pin) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%u)\n",
                 (uint32_t)led_gpio->base,
                 (uint32_t)led_gpio->pin);
        os_print("[SELFTEST] %s: OK (%s)\n",
                 selftest_label(led_gpio->label, "LED GPIO"),
                 selftest_label(led_gpio->location, "n/a"));
    } else {
        os_print("[CHK] GPIO init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(led_gpio->label, "LED GPIO"));
    }

    hal_gpio_init(button_gpio->base);
    if (button_gpio->available &&
        hal_gpio_set_direction(button_gpio->pin, false) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%u)\n",
                 (uint32_t)button_gpio->base,
                 (uint32_t)button_gpio->pin);
        os_print("[SELFTEST] %s: OK (%s level=%u)\n",
                 selftest_label(button_gpio->label, "button GPIO"),
                 selftest_label(button_gpio->location, "n/a"),
                 hal_gpio_read(button_gpio->pin) ? 1u : 0u);
    } else {
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(button_gpio->label, "button GPIO"));
    }
}

static void run_serial_bus_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_diag_config_t *diag_config = &profile->diag;
    uint8_t spi_rx = 0;

    report_pinmux_result(NULL, profile->i2c_pinmux_group);
    if (diag_config->available &&
        hal_i2c_init(diag_config->i2c_base, diag_config->i2c_bus_hz) == 0) {
        os_print("[CHK] I2C init: OK (base=0x%x hz=%u)\n",
                 (uint32_t)diag_config->i2c_base,
                 (uint32_t)diag_config->i2c_bus_hz);
        os_print("[SELFTEST] %s: OK (%s, %uHz)\n",
                 selftest_label(diag_config->i2c_label, "I2C"),
                 selftest_label(diag_config->i2c_route, "n/a"),
                 (uint32_t)diag_config->i2c_bus_hz);
    } else {
        os_print("[CHK] I2C init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(diag_config->i2c_label, "I2C"));
    }

    report_pinmux_result(NULL, profile->spi_pinmux_group);
    if (diag_config->available &&
        hal_spi_init(diag_config->spi_base, diag_config->spi_baud_div, HAL_SPI_MODE0) == 0 &&
        hal_spi_transfer(diag_config->spi_probe_tx, &spi_rx, SELFTEST_SPI_TIMEOUT) == 0) {
        os_print("[CHK] SPI init: OK (base=0x%x div=%u)\n",
                 (uint32_t)diag_config->spi_base,
                 (uint32_t)diag_config->spi_baud_div);
        os_print("[SELFTEST] %s: OK (%s rx=0x%x)\n",
                 selftest_label(diag_config->spi_label, "SPI"),
                 selftest_label(diag_config->spi_route, "n/a"),
                 spi_rx);
    } else {
        os_print("[CHK] SPI init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(diag_config->spi_label, "SPI"));
    }
}

static void run_flash_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_flash_profile_t *flash_profile = &profile->flash;
    uint32_t qspi_sig[4] = {0};
    uint8_t page_boundary_sample[8] = {0};
    uint32_t sample_offset = flash_profile->window_sample_offset;
    uint32_t sample_words = flash_profile->window_sample_words;
    uint32_t i;
    hal_flash_info_t flash_info = {0};
    bool qspi_sample_valid = false;

    if (sample_words > 4u) {
        sample_words = 4u;
    }

    report_pinmux_result(NULL, profile->flash_pinmux_group);
    os_print("[SELFTEST] %s ready: ctrl=0x%x window=0x%x (%s)\n",
             selftest_label(flash_profile->label, "flash"),
             (uint32_t)flash_profile->ctrl_base,
             (uint32_t)flash_profile->window_base,
             selftest_label(flash_profile->ready_note, selftest_label(flash_profile->route, "n/a")));
    if (flash_profile->available &&
        hal_flash_init(flash_profile->window_base, flash_profile->window_size) == 0) {
        os_print("[CHK] FLASH init: OK (base=0x%x size=0x%x)\n",
                 (uint32_t)flash_profile->window_base,
                 (uint32_t)flash_profile->window_size);
        qspi_sample_valid = sample_words == 4u;
        for (i = 0; i < sample_words; ++i) {
            if (hal_flash_read_u32(sample_offset + (i * sizeof(uint32_t)), &qspi_sig[i]) != 0) {
                qspi_sample_valid = false;
                break;
            }
        }

        if (qspi_sample_valid) {
            os_print("[CHK] FLASH read: OK (offset=0x%x len=%u)\n",
                     sample_offset,
                     sample_words * (uint32_t)sizeof(uint32_t));
            report_flash_sample(selftest_label(flash_profile->label, "flash"),
                                qspi_sig,
                                sample_words);
        } else {
            os_print("[CHK] FLASH read: FAIL\n");
        }

        if (hal_flash_get_info(&flash_info) == 0) {
            os_print("[CHK] FLASH identify: OK (jedec=0x%x page=%u sector=%u size=0x%x)\n",
                     flash_info.jedec_id,
                     flash_info.page_size,
                     flash_info.sector_size,
                     flash_info.capacity_bytes);
            if (flash_info.page_size >= 4u &&
                hal_flash_read(flash_info.page_size - 4u,
                               page_boundary_sample,
                               sizeof(page_boundary_sample)) == 0) {
                os_print("[CHK] FLASH page-boundary: OK (offset=0x%x len=%u)\n",
                         flash_info.page_size - 4u,
                         (uint32_t)sizeof(page_boundary_sample));
            } else {
                os_print("[CHK] FLASH page-boundary: FAIL\n");
            }
            if (hal_flash_read(flash_profile->window_size - 3u,
                               page_boundary_sample,
                               sizeof(page_boundary_sample)) != 0) {
                os_print("[CHK] FLASH range-guard: OK (offset=0x%x len=%u)\n",
                         flash_profile->window_size - 3u,
                         (uint32_t)sizeof(page_boundary_sample));
            } else {
                os_print("[CHK] FLASH range-guard: FAIL\n");
            }
        } else {
            os_print("[CHK] FLASH identify: FAIL\n");
        }
    } else {
        os_print("[CHK] FLASH init: FAIL\n");
    }
    if (qspi_sample_valid &&
        qspi_sig[0] == flash_profile->expected_signature[0] &&
        qspi_sig[1] == flash_profile->expected_signature[1] &&
        qspi_sig[2] == flash_profile->expected_signature[2] &&
        qspi_sig[3] == flash_profile->expected_signature[3]) {
        os_print("[SELFTEST] %s window read: OK (%s)\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->window_match_note, selftest_label(flash_profile->window_read_note, "sample matched")));
    } else {
        os_print("[SELFTEST] %s window read: %s\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->window_capture_note, "sample captured"));
    }
    if (flash_info.jedec_id == flash_profile->jedec_id &&
        flash_info.page_size == flash_profile->page_size &&
        flash_info.sector_size == flash_profile->sector_size &&
        flash_info.capacity_bytes == flash_profile->capacity_bytes) {
        os_print("[SELFTEST] %s identify: OK (%s)\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->identify_match_note, selftest_label(flash_profile->identify_note, "profile matched")));
    } else if (flash_info.jedec_id != 0u) {
        os_print("[SELFTEST] %s identify: %s\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->identify_capture_note, "sample captured"));
    }
}

static void report_canfd_result(const hal_board_canfd_profile_t *profile, uint32_t index)
{
    hal_canfd_config_t config;
    hal_canfd_state_t state;
    hal_canfd_frame_t tx_frame = {0};
    hal_canfd_frame_t rx_frame = {0};
    uint8_t i;
    uint32_t irq_mask = CANFD_IRQ_SEEN(index);
    const char *name = selftest_label(profile ? profile->label : NULL, "CANFD");

    if (!profile || index >= HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT || !profile->available) {
        os_print("[CHK] %s init: FAIL\n", name);
        return;
    }

    report_pinmux_result(name, profile->pinmux_group);

    config.nominal_bitrate = profile->bitrate;
    config.internal_loopback = profile->internal_loopback;
    tx_frame.id = profile->frame_id;
    tx_frame.len = (uint8_t)profile->frame_len;
    for (i = 0; i < tx_frame.len; ++i) {
        tx_frame.data[i] = (uint8_t)(profile->payload_seed + i);
    }

    g_canfd_irq_seen_mask &= ~irq_mask;
    g_canfd_irq_count[index] = 0u;

    hal_irq_register_handler(profile->irq_num, canfd_irq_handler);
    hal_irq_set_priority(profile->irq_num, 1u);
    hal_irq_enable(profile->irq_num);
    os_print("[CHK] %s irq-arm: OK (irq=%u)\n", name, profile->irq_num);

    if (hal_canfd_init(profile->base, &config) == 0) {
        os_print("[CHK] %s init: OK (base=0x%x bitrate=%u loopback=%u)\n",
                 name,
                 (uint32_t)profile->base,
                 profile->bitrate,
                 profile->internal_loopback ? 1u : 0u);
        if (hal_canfd_irq_enable(HAL_CANFD_IRQ_TX_COMPLETE |
                                 HAL_CANFD_IRQ_RX_READY |
                                 HAL_CANFD_IRQ_ERROR) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE) {
            os_print("[CHK] %s state: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
        } else {
            os_print("[CHK] %s state: FAIL\n", name);
            return;
        }

        if (hal_canfd_tx_enqueue(&tx_frame, SELFTEST_CANFD_TIMEOUT) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE) {
            os_print("[CHK] %s path: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
        } else {
            os_print("[CHK] %s path: FAIL\n", name);
            return;
        }

        if (wait_for_canfd_irq(irq_mask, SELFTEST_CANFD_TIMEOUT)) {
            os_print("[CHK] %s irq-fire: OK (irq=%u count=%u)\n",
                     name,
                     profile->irq_num,
                     g_canfd_irq_count[index]);
        } else {
            os_print("[CHK] %s irq-fire: FAIL\n", name);
            return;
        }

        if (hal_canfd_rx_dequeue(&rx_frame, SELFTEST_CANFD_TIMEOUT) == 0 &&
            hal_canfd_irq_ack(HAL_CANFD_IRQ_TX_COMPLETE |
                              HAL_CANFD_IRQ_RX_READY |
                              HAL_CANFD_IRQ_ERROR) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE &&
            canfd_frames_match(&tx_frame, &rx_frame)) {
            os_print("[CHK] %s loopback: OK (id=0x%x len=%u)\n",
                     name,
                     tx_frame.id,
                     (uint32_t)tx_frame.len);
            os_print("[CHK] %s settle: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
            os_print("[SELFTEST] %s loopback: OK (base=0x%x bitrate=%u id=0x%x)\n",
                     name,
                     (uint32_t)profile->base,
                     profile->bitrate,
                     tx_frame.id);
            if (profile->route) {
                os_print("[SELFTEST] %s route: %s\n", name, profile->route);
            }
            if (profile->loopback_note) {
                os_print("[SELFTEST] %s loopback note: %s\n",
                         name,
                         profile->loopback_note);
            }
        } else {
            os_print("[CHK] %s loopback: FAIL\n", name);
        }

        tx_frame.len = HAL_CANFD_MAX_DATA_LEN + 1u;
        if (hal_canfd_tx_enqueue(&tx_frame, SELFTEST_CANFD_TIMEOUT) != 0) {
            os_print("[CHK] %s guard invalid-dlc: OK\n", name);
        } else {
            os_print("[CHK] %s guard invalid-dlc: FAIL\n", name);
        }

        config.internal_loopback = false;
        tx_frame.len = (uint8_t)profile->frame_len;
        if (hal_canfd_init(profile->base, &config) == 0 &&
            hal_canfd_tx_enqueue(&tx_frame, SELFTEST_CANFD_TIMEOUT) != 0) {
            os_print("[CHK] %s guard loopback-required: OK\n", name);
        } else {
            os_print("[CHK] %s guard loopback-required: FAIL\n", name);
        }
    } else {
        os_print("[CHK] %s init: FAIL\n", name);
    }
}

static void run_canfd_selftest(const hal_board_selftest_profile_t *profile)
{
    uint32_t index;
    uint32_t canfd_count = profile->canfd_count;

    if (canfd_count > HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT) {
        canfd_count = HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT;
    }

    g_canfd_profile_count = canfd_count;

    for (index = 0; index < canfd_count; ++index) {
        g_canfd_profiles[index] = profile->canfd[index];
        report_canfd_result(&g_canfd_profiles[index], index);
    }
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

#if RRTOS_HAL_HAS_BOARD_SELFTEST
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
    profile->available = false;

#if defined(CONFIG_BOARD_BE_U1000)
    profile->schedulable_cores = 2u;
    profile->reserved_helper_mask = 1u << 2;
    profile->role_plan = "core0=boot core1=worker core2=reserved";
    profile->available = true;
#endif
}

const char *hal_board_task_map_description(bool smp_mode)
{
#if defined(CONFIG_BOARD_BE_U1000)
    return smp_mode
               ? "control->core0 worker->core1 core2=reserved"
               : "control+worker share single core";
#else
    (void)smp_mode;
    return NULL;
#endif
}

#if OS_CFG_SMP_EN
os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available || !control_task || !worker_task) {
        return OS_EINVAL;
    }

    if (os_task_set_affinity(control_task, topology.control_cpu) != OS_EOK) {
        return OS_ERROR;
    }
    if (os_task_set_affinity(worker_task, topology.worker_cpu) != OS_EOK) {
        return OS_ERROR;
    }
    return OS_EOK;
}

bool hal_board_issue_reschedule_probe(void)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available ||
        topology.reschedule_probe_cpu >= os_cpu_count() ||
        !os_smp_cpu_online(topology.reschedule_probe_cpu)) {
        return false;
    }

    os_ipi_send(topology.reschedule_probe_cpu, OS_IPI_RESCHEDULE);
    return true;
}

bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available || !peer_cpu) {
        return false;
    }

    if (current_cpu == topology.balance_probe_cpu_a) {
        *peer_cpu = topology.balance_probe_cpu_b;
        return true;
    }
    if (current_cpu == topology.balance_probe_cpu_b) {
        *peer_cpu = topology.balance_probe_cpu_a;
        return true;
    }
    return false;
}

uint32_t hal_board_balance_expected_mask(void)
{
    hal_board_demo_topology_t topology;

    hal_board_fill_demo_topology(&topology);
    if (!topology.available) {
        return 0u;
    }

    return (1u << topology.balance_probe_cpu_a) |
           (1u << topology.balance_probe_cpu_b);
}
#endif

static void hal_board_fill_selftest_profile(hal_board_selftest_profile_t *profile)
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
    profile->canfd_count = HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT;
    profile->available = true;
#else
    profile->available = false;
#endif
}
#endif

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

int hal_board_run_selftest(void)
{
#if RRTOS_HAL_HAS_BOARD_SELFTEST
    hal_board_selftest_profile_t selftest_profile;

    hal_board_fill_selftest_profile(&selftest_profile);
    if (!selftest_profile.available) {
        return -1;
    }

#if defined(CONFIG_BOARD_BE_U1000)
    report_pinmux_result(NULL, selftest_profile.console_pinmux_group);
    run_gpio_selftest(&selftest_profile);
    run_serial_bus_selftest(&selftest_profile);
    run_flash_selftest(&selftest_profile);
    run_canfd_selftest(&selftest_profile);
    return 0;
#else
    return -1;
#endif
#else
    return -1;
#endif
}
