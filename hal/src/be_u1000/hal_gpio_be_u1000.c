#include <stdint.h>
#include <stdbool.h>

#include "hal_gpio.h"

typedef struct {
    volatile uint32_t DR;
    volatile uint32_t DDR;
    volatile uint32_t reserved0[10];
    volatile uint32_t INTEN;
    volatile uint32_t INTMASK;
    volatile uint32_t INTTYPE_LEVEL;
    volatile uint32_t INTPOLARITY;
    volatile uint32_t INTSTATUS;
    volatile uint32_t RAWINTSTATUS;
    volatile uint32_t DEBOUNCE;
    volatile uint32_t PORTA_EOI;
    volatile uint32_t EXT_PORTA;
    volatile uint32_t reserved1[3];
    volatile uint32_t LS_SYNC;
    volatile uint32_t ID_CODE;
    volatile uint32_t VER_ID_CODE;
    volatile uint32_t CONFIG_REG2;
    volatile uint32_t CONFIG_REG1;
    volatile uint32_t BOTHEDGE;
} be_u1000_gpio_t;

static volatile be_u1000_gpio_t *g_gpio;

static int hal_gpio_pin_valid(uint8_t pin)
{
    return pin < 16u;
}

static uint32_t hal_gpio_pin_mask(uint8_t pin)
{
    return 1u << pin;
}

void hal_gpio_init(uintptr_t base)
{
    g_gpio = (volatile be_u1000_gpio_t *)base;
}

int hal_gpio_set_direction(uint8_t pin, bool output)
{
    uint32_t mask;

    if (g_gpio == 0 || !hal_gpio_pin_valid(pin)) {
        return -1;
    }

    mask = hal_gpio_pin_mask(pin);
    if (output) {
        g_gpio->DDR |= mask;
    } else {
        g_gpio->DDR &= ~mask;
    }

    return 0;
}

int hal_gpio_write(uint8_t pin, bool value)
{
    uint32_t mask;

    if (g_gpio == 0 || !hal_gpio_pin_valid(pin)) {
        return -1;
    }

    mask = hal_gpio_pin_mask(pin);
    if (value) {
        g_gpio->DR |= mask;
    } else {
        g_gpio->DR &= ~mask;
    }

    return 0;
}

bool hal_gpio_read(uint8_t pin)
{
    if (g_gpio == 0 || !hal_gpio_pin_valid(pin)) {
        return false;
    }

    return (g_gpio->EXT_PORTA & hal_gpio_pin_mask(pin)) != 0;
}

int hal_gpio_toggle(uint8_t pin)
{
    if (g_gpio == 0 || !hal_gpio_pin_valid(pin)) {
        return -1;
    }

    g_gpio->DR ^= hal_gpio_pin_mask(pin);
    return 0;
}
