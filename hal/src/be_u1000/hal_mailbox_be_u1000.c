#include <stdint.h>

#include "board_config.h"
#include "hal_mailbox.h"

typedef struct {
    volatile uint32_t DATA0;
    volatile uint32_t FULL0;
    volatile uint32_t EMPTY0;
    volatile uint32_t reserved0;
    volatile uint32_t DATA1;
    volatile uint32_t FULL1;
    volatile uint32_t EMPTY1;
} be_u1000_mailbox_t;

enum {
    BE_U1000_MAILBOX_FLAG_READY = 1u,
};

static volatile be_u1000_mailbox_t *be_u1000_mailbox(hal_mailbox_id_t mailbox)
{
    if (mailbox == HAL_MAILBOX0) {
        return (volatile be_u1000_mailbox_t *)BE_U1000_MB0_BASE;
    }
    if (mailbox == HAL_MAILBOX1) {
        return (volatile be_u1000_mailbox_t *)BE_U1000_MB1_BASE;
    }
    return 0;
}

int hal_mailbox_sys_write_data0(hal_mailbox_id_t mailbox, uint32_t value)
{
    volatile be_u1000_mailbox_t *mb = be_u1000_mailbox(mailbox);

    if (mb == 0) {
        return -1;
    }

    mb->DATA0 = value;
    mb->FULL0 = BE_U1000_MAILBOX_FLAG_READY;
    return 0;
}

int hal_mailbox_sys_read_data1(hal_mailbox_id_t mailbox, uint32_t *value)
{
    volatile be_u1000_mailbox_t *mb = be_u1000_mailbox(mailbox);

    if (mb == 0 || value == 0) {
        return -1;
    }

    *value = mb->DATA1;
    return 0;
}

bool hal_mailbox_sys_data0_full(hal_mailbox_id_t mailbox)
{
    volatile be_u1000_mailbox_t *mb = be_u1000_mailbox(mailbox);

    if (mb == 0) {
        return false;
    }

    return (mb->FULL0 & BE_U1000_MAILBOX_FLAG_READY) != 0u;
}

bool hal_mailbox_sys_data1_full(hal_mailbox_id_t mailbox)
{
    volatile be_u1000_mailbox_t *mb = be_u1000_mailbox(mailbox);

    if (mb == 0) {
        return false;
    }

    return (mb->FULL1 & BE_U1000_MAILBOX_FLAG_READY) != 0u;
}
