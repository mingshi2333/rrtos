#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uintptr_t spi_base;
    uint32_t spi_baud_div;
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

typedef enum {
    HAL_BOARD_PINMUX_GROUP_CONSOLE_UART0 = 0,
    HAL_BOARD_PINMUX_GROUP_USER_LED,
    HAL_BOARD_PINMUX_GROUP_USER_BUTTON,
    HAL_BOARD_PINMUX_GROUP_HEADER_I2C0,
    HAL_BOARD_PINMUX_GROUP_HEADER_SPI1,
    HAL_BOARD_PINMUX_GROUP_QSPI1,
    HAL_BOARD_PINMUX_GROUP_CANFD0,
    HAL_BOARD_PINMUX_GROUP_CANFD1,
} hal_board_pinmux_group_t;

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
    bool available;
} hal_board_canfd_profile_t;

typedef struct {
    uint32_t schedulable_cores;
    uint32_t reserved_helper_mask;
    const char *role_plan;
    const char *task_map_smp;
    const char *task_map_single;
    bool available;
} hal_board_execution_profile_t;

#define HAL_BOARD_CANFD_CONTROLLER_COUNT 2u

typedef struct {
    hal_board_pinmux_group_t console_pinmux_group;
    hal_board_pinmux_group_t led_pinmux_group;
    hal_board_pinmux_group_t button_pinmux_group;
    hal_board_pinmux_group_t i2c_pinmux_group;
    hal_board_pinmux_group_t spi_pinmux_group;
    hal_board_pinmux_group_t flash_pinmux_group;
    hal_board_gpio_resource_t led_gpio;
    hal_board_gpio_resource_t button_gpio;
    hal_board_diag_config_t diag;
    hal_board_flash_profile_t flash;
    hal_board_canfd_profile_t canfd[HAL_BOARD_CANFD_CONTROLLER_COUNT];
    bool available;
} hal_board_selftest_profile_t;

void hal_board_init(void);
const char *hal_board_name(void);
void hal_board_print_banner(void);
void hal_board_get_execution_profile(hal_board_execution_profile_t *profile);
void hal_board_get_selftest_profile(hal_board_selftest_profile_t *profile);
const char *hal_board_pinmux_group_name(hal_board_pinmux_group_t group);
int hal_board_apply_pinmux_group(hal_board_pinmux_group_t group);

#endif
