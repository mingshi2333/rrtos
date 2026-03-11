#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#include <stdbool.h>
#include <stdint.h>

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
    uint32_t schedulable_cores;
    uint32_t reserved_helper_mask;
    const char *role_plan;
    bool available;
} hal_board_execution_profile_t;

void hal_board_init(void);
const char *hal_board_name(void);
void hal_board_print_banner(void);
void hal_board_get_execution_profile(hal_board_execution_profile_t *profile);
const char *hal_board_task_map_description(bool smp_mode);
const char *hal_board_pinmux_group_name(hal_board_pinmux_group_t group);
int hal_board_apply_pinmux_group(hal_board_pinmux_group_t group);
int hal_board_run_selftest(void);

#endif
