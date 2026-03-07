#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>

void hal_gpio_init(uintptr_t base);
int hal_gpio_set_direction(uint8_t pin, bool output);
int hal_gpio_write(uint8_t pin, bool value);
bool hal_gpio_read(uint8_t pin);
int hal_gpio_toggle(uint8_t pin);

#endif
