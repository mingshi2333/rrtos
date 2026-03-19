#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_i2c.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_i2c_task_tcb;
static uint8_t g_i2c_task_stack[768];

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void i2c_polling_task(void *arg)
{
    uint32_t tick = 0;
    int rc;

    (void)arg;

    while (1) {
        rc = hal_i2c_probe(0x55u, 100000u);
        os_print("[I2C_APP] tick %u probe-0x55 rc=%d\n", tick++, rc);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[I2C_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    rc = hal_i2c_init(BE_U1000_HEADER_I2C_BASE, BE_U1000_DIAG_I2C_BUS_HZ);
    if (rc != 0) {
        os_print("[I2C_APP] init failed rc=%d\n", rc);
        return;
    }

    os_print("[I2C_APP] i2c_polling ready base=0x%x hz=%u init-rc=%d\n",
             (uint32_t)BE_U1000_HEADER_I2C_BASE,
             (uint32_t)BE_U1000_DIAG_I2C_BUS_HZ,
             rc);
    os_print("[I2C_APP] Initializing kernel...\n");

    os_kernel_init();
    rc = os_task_create(&g_i2c_task_tcb,
                        "i2c_polling",
                        i2c_polling_task,
                        0,
                        10,
                        g_i2c_task_stack,
                        sizeof(g_i2c_task_stack));
    if (rc != OS_EOK) {
        os_print("[I2C_APP] task create failed rc=%d\n", rc);
        return;
    }

    os_print("[I2C_APP] Starting scheduler...\n");
    os_kernel_start();
}
