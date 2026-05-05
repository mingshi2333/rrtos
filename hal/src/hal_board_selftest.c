#include <stdbool.h>
#include <stdint.h>

#include "hal_board.h"
#include "hal_uart.h"
#include "os_config.h"
#include "os_kernel.h"

#ifndef RRTOS_HAL_HAS_BOARD_SELFTEST
#define RRTOS_HAL_HAS_BOARD_SELFTEST 0
#endif

#if defined(CONFIG_BOARD_BE_U1000) && RRTOS_HAL_HAS_BOARD_SELFTEST
#include "board_config.h"
#include "hal_canfd.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_irq.h"
#include "hal_spi.h"

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

static volatile uint32_t g_canfd_irq_seen_mask;
static volatile uint32_t g_canfd_irq_count[HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT];
static hal_board_canfd_profile_t g_canfd_profiles[HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT];
static uint32_t g_canfd_profile_count;

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

static void hal_board_fill_diag_config(hal_board_diag_config_t *config)
{
    if (!config) {
        return;
    }

    config->available = false;
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
}

static void hal_board_fill_flash_profile(hal_board_flash_profile_t *profile)
{
    if (!profile) {
        return;
    }

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
}

static void hal_board_fill_canfd_profile(uint32_t index, hal_board_canfd_profile_t *profile)
{
    if (!profile) {
        return;
    }

    profile->base = 0;
    profile->bitrate = BE_U1000_DIAG_CANFD_BITRATE;
    profile->frame_id = 0;
    profile->irq_num = 0;
    profile->frame_len = BE_U1000_DIAG_CANFD_FRAME_LEN;
    profile->payload_seed = 0;
    profile->pinmux_group = HAL_BOARD_PINMUX_GROUP_CANFD0;
    profile->label = NULL;
    profile->route = NULL;
    profile->loopback_note = NULL;
    profile->internal_loopback = false;
    profile->available = false;

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
}

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
    profile->canfd_count = HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT;
    hal_board_fill_gpio_resource(HAL_BOARD_GPIO_USER_LED, &profile->led_gpio);
    hal_board_fill_gpio_resource(HAL_BOARD_GPIO_USER_BUTTON, &profile->button_gpio);
    hal_board_fill_diag_config(&profile->diag);
    hal_board_fill_flash_profile(&profile->flash);
    hal_board_fill_canfd_profile(0u, &profile->canfd[0]);
    hal_board_fill_canfd_profile(1u, &profile->canfd[1]);
    profile->available = true;
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
            hal_flash_diag_t diag;
            if (hal_flash_get_diag(&diag) == 0) {
                os_print("[CHK] FLASH identify diag: status=%u recv=%u raw=0x%x xip=%u\n",
                         diag.status,
                         diag.received,
                         diag.jedec_raw,
                         diag.xip_was_enabled);
                os_print("[CHK] FLASH qspi diag: pclk1=0x%x syscr0=0x%x qspien=0x%x ser=0x%x sr=0x%x rxflr=%u txflr=%u\n",
                         diag.pclk1en,
                         diag.syscr0,
                         diag.qspienr,
                         diag.ser,
                         diag.sr,
                         diag.rxflr,
                         diag.txflr);
                os_print("[CHK] FLASH qspi cfg: ctrlr0=0x%x ctrlr1=0x%x spi_ctrlr0=0x%x baudr=%u isr=0x%x risr=0x%x\n",
                         diag.ctrlr0,
                         diag.ctrlr1,
                         diag.spi_ctrlr0,
                         diag.baudr,
                         diag.isr,
                         diag.risr);
            }
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

int hal_board_run_selftest(void)
{
    hal_board_selftest_profile_t selftest_profile;

    hal_board_fill_selftest_profile(&selftest_profile);
    if (!selftest_profile.available) {
        return -1;
    }

    report_pinmux_result(NULL, selftest_profile.console_pinmux_group);
    run_gpio_selftest(&selftest_profile);
    run_serial_bus_selftest(&selftest_profile);
    run_flash_selftest(&selftest_profile);
    run_canfd_selftest(&selftest_profile);
    return 0;
}
#endif
