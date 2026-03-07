#ifndef HAL_MAILBOX_H
#define HAL_MAILBOX_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_MAILBOX0 = 0,
    HAL_MAILBOX1 = 1,
} hal_mailbox_id_t;

int hal_mailbox_sys_write_data0(hal_mailbox_id_t mailbox, uint32_t value);
int hal_mailbox_sys_read_data1(hal_mailbox_id_t mailbox, uint32_t *value);
bool hal_mailbox_sys_data0_full(hal_mailbox_id_t mailbox);
bool hal_mailbox_sys_data1_full(hal_mailbox_id_t mailbox);

#endif
