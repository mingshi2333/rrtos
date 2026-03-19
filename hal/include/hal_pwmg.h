#ifndef HAL_PWMG_H
#define HAL_PWMG_H

#include <stdint.h>

typedef enum {
    HAL_PWMG_CLOCK_DIV_1 = 0,
    HAL_PWMG_CLOCK_DIV_2 = 1,
    HAL_PWMG_CLOCK_DIV_4 = 2,
} hal_pwmg_clock_div_t;

typedef enum {
    HAL_PWMG_OC_FROZEN = 0,
    HAL_PWMG_OC_ACTIVE = 1,
    HAL_PWMG_OC_INACTIVE = 2,
    HAL_PWMG_OC_TOGGLE = 3,
    HAL_PWMG_OC_FORCED_INACTIVE = 4,
    HAL_PWMG_OC_FORCED_ACTIVE = 5,
    HAL_PWMG_OC_PWM1 = 6,
    HAL_PWMG_OC_PWM2 = 7,
} hal_pwmg_oc_mode_t;

typedef struct {
    uint16_t prescaler;
    uint16_t autoreload;
    uint16_t compare;
    hal_pwmg_clock_div_t clock_division;
    hal_pwmg_oc_mode_t oc_mode;
} hal_pwmg_config_t;

int hal_pwmg_init(uintptr_t base, const hal_pwmg_config_t *config);
int hal_pwmg_enable_counter(void);
int hal_pwmg_enable_compare_interrupt(void);
int hal_pwmg_disable_compare_interrupt(void);
int hal_pwmg_get_counter(uint16_t *counter);
int hal_pwmg_get_compare(uint16_t *compare);
int hal_pwmg_compare_interrupt_enabled(void);
int hal_pwmg_compare_match_pending(void);
int hal_pwmg_clear_compare_match(void);

#endif
