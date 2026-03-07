#include "hal_board.h"

#include "hal_clint.h"
#include "hal_irq.h"
#include "hal_uart.h"
#include "os_config.h"

#if defined(CONFIG_BOARD_BE_U1000)
#include "board_config.h"
#endif

void hal_board_init(void) {
    hal_uart_init(OS_CFG_UART_BASE, OS_CFG_UART_BAUD);
    hal_clint_init(OS_CFG_CLINT_BASE);

#if OS_CFG_IRQ_MODEL_CLIC
    hal_irq_init_clic(0);
#else
    hal_irq_init_plic(OS_CFG_PLIC_BASE);
#endif
}

const char *hal_board_name(void) {
#if defined(CONFIG_BOARD_BE_U1000)
    return "BE-U1000";
#else
    return "qemu_virt";
#endif
}

void hal_board_print_banner(void) {
#if defined(CONFIG_BOARD_BE_U1000)
    os_print("\n========================================\n");
    os_print("  RV-AIOS v1.0.0 on BE-U1000\n");
    os_print("  Baikal Electronics RISC-V RV32IMAFC\n");
    os_print("========================================\n\n");
#else
    os_print("[BOOT] Board: %s\n", hal_board_name());
#endif
}

void hal_board_get_diag_config(hal_board_diag_config_t *config) {
    if (!config) {
        return;
    }

    config->available = false;
    config->gpio_base = 0;
    config->gpio_pin = 0;
    config->spi_base = 0;
    config->spi_baud_div = 0;
    config->i2c_base = 0;
    config->i2c_bus_hz = 0;

#if defined(CONFIG_BOARD_BE_U1000)
    config->gpio_base = BE_U1000_DIAG_GPIO_BASE;
    config->gpio_pin = BE_U1000_DIAG_GPIO_PIN;
    config->spi_base = BE_U1000_DIAG_SPI_BASE;
    config->spi_baud_div = BE_U1000_DIAG_SPI_BAUD_DIV;
    config->i2c_base = BE_U1000_DIAG_I2C_BASE;
    config->i2c_bus_hz = BE_U1000_DIAG_I2C_BUS_HZ;
    config->available = true;
#endif
}
