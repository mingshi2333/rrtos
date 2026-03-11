#ifndef HAL_BOARD_H
#define HAL_BOARD_H

#include <stdbool.h>
#include <stdint.h>

#include "os_types.h"

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

#if OS_CFG_SMP_EN
os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task);
bool hal_board_issue_reschedule_probe(void);
bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu);
uint32_t hal_board_balance_expected_mask(void);
#endif

#endif
