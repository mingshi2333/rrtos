#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>

int hal_i2c_init(uintptr_t base, uint32_t bus_hz);
int hal_i2c_write_byte(uint16_t addr, uint8_t data, uint32_t timeout);
int hal_i2c_read_byte(uint16_t addr, uint8_t *data, uint32_t timeout);
int hal_i2c_probe(uint16_t addr, uint32_t timeout);

#endif
