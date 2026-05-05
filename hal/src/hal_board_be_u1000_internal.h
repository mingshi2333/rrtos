#ifndef HAL_BOARD_BE_U1000_INTERNAL_H
#define HAL_BOARD_BE_U1000_INTERNAL_H

#include "hal_board.h"

#if defined(CONFIG_BOARD_BE_U1000)
void hal_board_be_u1000_pinmux_init(void);
int hal_board_be_u1000_apply_pinmux_group(hal_board_pinmux_group_t group);
#endif

#endif
