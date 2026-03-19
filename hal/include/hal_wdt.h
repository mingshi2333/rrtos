#ifndef HAL_WDT_H
#define HAL_WDT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_WDT_MODE_RESET = 0,
    HAL_WDT_MODE_INTERRUPT = 1,
} hal_wdt_mode_t;

typedef enum {
    HAL_WDT_RESET_2 = 0,
    HAL_WDT_RESET_4 = 1,
    HAL_WDT_RESET_8 = 2,
    HAL_WDT_RESET_16 = 3,
    HAL_WDT_RESET_32 = 4,
    HAL_WDT_RESET_64 = 5,
    HAL_WDT_RESET_128 = 6,
    HAL_WDT_RESET_256 = 7,
} hal_wdt_reset_len_t;

typedef enum {
    HAL_WDT_TIMEOUT_0 = 0,
    HAL_WDT_TIMEOUT_1 = 1,
    HAL_WDT_TIMEOUT_2 = 2,
    HAL_WDT_TIMEOUT_3 = 3,
    HAL_WDT_TIMEOUT_4 = 4,
    HAL_WDT_TIMEOUT_5 = 5,
    HAL_WDT_TIMEOUT_6 = 6,
    HAL_WDT_TIMEOUT_7 = 7,
    HAL_WDT_TIMEOUT_8 = 8,
    HAL_WDT_TIMEOUT_9 = 9,
    HAL_WDT_TIMEOUT_10 = 10,
    HAL_WDT_TIMEOUT_11 = 11,
    HAL_WDT_TIMEOUT_12 = 12,
    HAL_WDT_TIMEOUT_13 = 13,
    HAL_WDT_TIMEOUT_14 = 14,
    HAL_WDT_TIMEOUT_15 = 15,
} hal_wdt_timeout_t;

typedef struct {
    hal_wdt_mode_t mode;
    hal_wdt_reset_len_t reset_len;
    hal_wdt_timeout_t timeout;
} hal_wdt_config_t;

int hal_wdt_init(uintptr_t base, const hal_wdt_config_t *config);
int hal_wdt_reload(void);
int hal_wdt_get_counter(uint32_t *value);
int hal_wdt_irq_active(bool *active);

#endif
