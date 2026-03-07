#include <stdint.h>
#include <stddef.h>

#include "os_kernel.h"
#include "os_config.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#include "hal_uart.h"

static uint8_t task1_stack[1024];
static uint8_t task2_stack[1024];
static os_tcb_t task1_tcb;
static os_tcb_t task2_tcb;

static void platform_self_check(void) {
    hal_board_diag_config_t diag;
    uint64_t t0;
    uint64_t t1;
    volatile uint32_t i;

    t0 = hal_clint_mtime_get();
    for (i = 0; i < 50000; i++) {
    }
    t1 = hal_clint_mtime_get();

    if (t1 > t0) {
        os_print("[CHK] CLINT mtime monotonic: OK (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    } else {
        os_print("[CHK] CLINT mtime monotonic: FAIL (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    }

    hal_board_get_diag_config(&diag);
    if (!diag.available) {
        return;
    }

    hal_gpio_init(diag.gpio_base);
    if (hal_gpio_set_direction(diag.gpio_pin, true) == 0 &&
        hal_gpio_write(diag.gpio_pin, true) == 0 &&
        hal_gpio_toggle(diag.gpio_pin) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%d)\n", (uint32_t)diag.gpio_base, diag.gpio_pin);
    } else {
        os_print("[CHK] GPIO init: FAIL\n");
    }

    if (hal_spi_init(diag.spi_base, diag.spi_baud_div, HAL_SPI_MODE0) == 0) {
        os_print("[CHK] SPI init: OK (base=0x%x div=%d)\n", (uint32_t)diag.spi_base, diag.spi_baud_div);
    } else {
        os_print("[CHK] SPI init: FAIL\n");
    }

    if (hal_i2c_init(diag.i2c_base, diag.i2c_bus_hz) == 0) {
        os_print("[CHK] I2C init: OK (base=0x%x hz=%d)\n", (uint32_t)diag.i2c_base, diag.i2c_bus_hz);
    } else {
        os_print("[CHK] I2C init: FAIL\n");
    }
}

static void task1_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;
    while (1) {
        os_print("[Task1] tick %u\n", count++);
        os_task_delay(1000);
    }
}

static void task2_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;
    while (1) {
        os_print("[Task2] tick %u\n", count++);
        os_task_delay(2000);
    }
}

void os_kernel_main(void) {
    hal_board_init();
    hal_board_print_banner();
    os_print("[BOOT] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

#if OS_CFG_IRQ_MODEL_CLIC
    os_print("[BOOT] IRQ model: CLIC\n");
#else
    os_print("[BOOT] IRQ model: PLIC\n");
#endif

    platform_self_check();

    os_print("[BOOT] Initializing kernel...\n");
    os_kernel_init();

    os_task_create(&task1_tcb, "task1", task1_entry, NULL,
                   10, task1_stack, sizeof(task1_stack));
    os_task_create(&task2_tcb, "task2", task2_entry, NULL,
                   11, task2_stack, sizeof(task2_stack));

    os_print("[BOOT] Starting scheduler...\n");
    os_kernel_start();
}
