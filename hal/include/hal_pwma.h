#ifndef HAL_PWMA_H
#define HAL_PWMA_H

#include <stdint.h>

typedef enum {
    HAL_PWMA_COUNTER_UP = 0,
    HAL_PWMA_COUNTER_DOWN = 1,
    HAL_PWMA_COUNTER_CENTER1 = 2,
    HAL_PWMA_COUNTER_CENTER2 = 3,
    HAL_PWMA_COUNTER_CENTER3 = 4,
} hal_pwma_counter_mode_t;

typedef enum {
    HAL_PWMA_CLOCK_DIV_1 = 0,
    HAL_PWMA_CLOCK_DIV_2 = 1,
    HAL_PWMA_CLOCK_DIV_4 = 2,
} hal_pwma_clock_div_t;

typedef struct {
    uint16_t prescaler;
    uint16_t autoreload;
    hal_pwma_counter_mode_t counter_mode;
    hal_pwma_clock_div_t clock_division;
    uint8_t repetition_counter;
} hal_pwma_config_t;

int hal_pwma_init(uintptr_t base, const hal_pwma_config_t *config);
int hal_pwma_enable_counter(void);
int hal_pwma_enable_update_interrupt(void);
int hal_pwma_disable_update_interrupt(void);
int hal_pwma_set_autoreload(uint16_t value);
int hal_pwma_set_repetition_counter(uint8_t value);
int hal_pwma_get_counter(uint16_t *counter);
int hal_pwma_get_repetition_counter(uint8_t *value);
int hal_pwma_update_interrupt_enabled(void);
int hal_pwma_update_pending(void);
int hal_pwma_clear_update(void);

#endif
