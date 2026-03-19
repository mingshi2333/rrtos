#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_flash.h"
#include "../../hal/include/hal_uart.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_flash_task_tcb;
static uint8_t g_flash_task_stack[768];
static uint32_t g_flash_signature[4];
static hal_flash_info_t g_flash_info;

static void busy_wait_cycles(uint32_t iterations)
{
    volatile uint32_t i;

    for (i = 0; i < iterations; ++i) {
    }
}

static void qspi_flash_task(void *arg)
{
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        os_print("[QSPI_APP] tick %u sig=%x %x %x %x jedec=%x size=%x\n",
                 tick++,
                 g_flash_signature[0],
                 g_flash_signature[1],
                 g_flash_signature[2],
                 g_flash_signature[3],
                 g_flash_info.jedec_id,
                 g_flash_info.capacity_bytes);
        busy_wait_cycles(250000u);
    }
}

void os_kernel_main(void)
{
    uint32_t i;
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[QSPI_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    if (hal_flash_init(BE_U1000_QSPI1_BASE, BE_U1000_QSPI1_SIZE) != 0) {
        os_print("[QSPI_APP] init: FAIL\n");
        while (1) {
        }
    }

    for (i = 0; i < 4u; ++i) {
        if (hal_flash_read_u32(i * sizeof(uint32_t), &g_flash_signature[i]) != 0) {
            os_print("[QSPI_APP] read: FAIL\n");
            while (1) {
            }
        }
    }

    if (hal_flash_get_info(&g_flash_info) != 0) {
        os_print("[QSPI_APP] identify: FAIL\n");
        while (1) {
        }
    }

    os_print("[QSPI_APP] qspi_flash ready window=0x%x sig=%x %x %x %x jedec=%x page=%u sector=%u size=%x\n",
             (uint32_t)BE_U1000_QSPI1_BASE,
             g_flash_signature[0],
             g_flash_signature[1],
             g_flash_signature[2],
             g_flash_signature[3],
             g_flash_info.jedec_id,
             g_flash_info.page_size,
             g_flash_info.sector_size,
             g_flash_info.capacity_bytes);
    os_print("[QSPI_APP] Initializing kernel...\n");

    os_kernel_init();
    rc = os_task_create(&g_flash_task_tcb,
                        "qspi_flash",
                        qspi_flash_task,
                        0,
                        10,
                        g_flash_task_stack,
                        sizeof(g_flash_task_stack));
    if (rc != OS_EOK) {
        os_print("[QSPI_APP] task_create: FAIL rc=%d\n", (int)rc);
        return;
    }

    os_print("[QSPI_APP] Starting scheduler...\n");
    os_kernel_start();
}
