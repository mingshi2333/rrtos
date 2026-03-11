#ifndef HAL_BOARD_SMP_H
#define HAL_BOARD_SMP_H

#include <stdbool.h>
#include <stdint.h>

#include "hal_board.h"
#include "os_types.h"

#if OS_CFG_SMP_EN
os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task);
bool hal_board_issue_reschedule_probe(void);
bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu);
uint32_t hal_board_balance_expected_mask(void);
#endif

#endif
