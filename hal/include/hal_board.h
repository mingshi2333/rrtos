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

void hal_board_init(void);
const char *hal_board_name(void);
void hal_board_print_banner(void);
void hal_board_get_diag_config(hal_board_diag_config_t *config);

#endif
