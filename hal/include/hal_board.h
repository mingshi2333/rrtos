#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uintptr_t gpio_base;
    uint32_t gpio_pin;
    uintptr_t spi_base;
    uint32_t spi_baud_div;
    uintptr_t i2c_base;
    uint32_t i2c_bus_hz;
    bool available;
} hal_board_diag_config_t;

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

void hal_board_init(void);
const char *hal_board_name(void);
void hal_board_print_banner(void);
void hal_board_get_diag_config(hal_board_diag_config_t *config);
int hal_board_apply_pinmux_group(hal_board_pinmux_group_t group);

#endif
