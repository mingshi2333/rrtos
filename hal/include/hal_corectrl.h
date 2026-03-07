#ifndef HAL_CORECTRL_H
#define HAL_CORECTRL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_CORECTRL_CORE0 = 0,
    HAL_CORECTRL_CORE1 = 1,
    HAL_CORECTRL_CORE2 = 2,
} hal_corectrl_core_t;

int hal_corectrl_start(hal_corectrl_core_t core, uintptr_t reset_vector);
int hal_corectrl_force_reset_all(hal_corectrl_core_t core);
int hal_corectrl_release_reset_all(hal_corectrl_core_t core);
bool hal_corectrl_is_in_reset_all(hal_corectrl_core_t core);

int hal_corectrl_core2_release_reset_cx(void);
int hal_corectrl_core2_release_reset_fp(void);
int hal_corectrl_core2_copy_image(const void *image, uintptr_t system_dst, uint32_t size);

#endif
