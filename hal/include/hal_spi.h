#ifndef HAL_SPI_H
#define HAL_SPI_H

#include <stdint.h>

#define HAL_SPI_MODE0 0u
#define HAL_SPI_MODE1 1u
#define HAL_SPI_MODE2 2u
#define HAL_SPI_MODE3 3u

int hal_spi_init(uintptr_t base, uint32_t baud_div, uint32_t mode);
int hal_spi_transfer(uint8_t tx, uint8_t *rx, uint32_t timeout);
int hal_spi_transfer_buffer(const uint8_t *tx, uint8_t *rx, uint32_t len, uint32_t timeout);

#endif
