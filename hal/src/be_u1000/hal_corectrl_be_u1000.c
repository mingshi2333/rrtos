#include <stdint.h>

#include "board_config.h"
#include "hal_corectrl.h"

typedef struct {
    volatile uint32_t CLKSEL;
    volatile uint32_t PLLSET;
    volatile uint32_t CLKCR0;
    volatile uint32_t PCLK0EN;
    volatile uint32_t PCLK1EN;
    volatile uint32_t PCLK2EN;
    volatile uint32_t SYSCR0;
    volatile uint32_t SYSCR1;
    volatile uint32_t SYSCR2;
} be_u1000_cru_t;

enum {
    BE_U1000_CRU_SYSCR0_CORE1CLKEN = 1u << 0,
    BE_U1000_CRU_SYSCR0_CORE1RSTN = 1u << 1,
    BE_U1000_CRU_SYSCR0_CORE1FPRSTN = 1u << 2,
    BE_U1000_CRU_SYSCR0_CORE1CXRSTN = 1u << 3,
    BE_U1000_CRU_SYSCR0_CORE2RSTN = 1u << 4,
    BE_U1000_CRU_SYSCR0_CORE2FPRSTN = 1u << 5,
    BE_U1000_CRU_SYSCR0_CORE2CXRSTN = 1u << 6,
};

static volatile be_u1000_cru_t *be_u1000_cru(void)
{
    return (volatile be_u1000_cru_t *)BE_U1000_CRU_BASE;
}

static int be_u1000_corectrl_core_valid(hal_corectrl_core_t core)
{
    return core == HAL_CORECTRL_CORE1 || core == HAL_CORECTRL_CORE2;
}

int hal_corectrl_start(hal_corectrl_core_t core, uintptr_t reset_vector)
{
    volatile be_u1000_cru_t *cru = be_u1000_cru();

    if (!be_u1000_corectrl_core_valid(core)) {
        return -1;
    }

    if (core == HAL_CORECTRL_CORE1) {
        cru->SYSCR1 = (uint32_t)reset_vector;
        cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE1CLKEN;
        cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE1CXRSTN |
                       BE_U1000_CRU_SYSCR0_CORE1FPRSTN |
                       BE_U1000_CRU_SYSCR0_CORE1RSTN;
        return 0;
    }

    cru->SYSCR2 = (uint32_t)reset_vector;
    cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE2CXRSTN |
                   BE_U1000_CRU_SYSCR0_CORE2FPRSTN |
                   BE_U1000_CRU_SYSCR0_CORE2RSTN;
    return 0;
}

int hal_corectrl_force_reset_all(hal_corectrl_core_t core)
{
    volatile be_u1000_cru_t *cru = be_u1000_cru();

    if (!be_u1000_corectrl_core_valid(core)) {
        return -1;
    }

    if (core == HAL_CORECTRL_CORE1) {
        cru->SYSCR0 &= ~(BE_U1000_CRU_SYSCR0_CORE1CXRSTN |
                         BE_U1000_CRU_SYSCR0_CORE1FPRSTN |
                         BE_U1000_CRU_SYSCR0_CORE1RSTN);
        return 0;
    }

    cru->SYSCR0 &= ~(BE_U1000_CRU_SYSCR0_CORE2CXRSTN |
                     BE_U1000_CRU_SYSCR0_CORE2FPRSTN |
                     BE_U1000_CRU_SYSCR0_CORE2RSTN);
    return 0;
}

int hal_corectrl_release_reset_all(hal_corectrl_core_t core)
{
    volatile be_u1000_cru_t *cru = be_u1000_cru();

    if (!be_u1000_corectrl_core_valid(core)) {
        return -1;
    }

    if (core == HAL_CORECTRL_CORE1) {
        cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE1CLKEN;
        cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE1CXRSTN |
                       BE_U1000_CRU_SYSCR0_CORE1FPRSTN |
                       BE_U1000_CRU_SYSCR0_CORE1RSTN;
        return 0;
    }

    cru->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE2CXRSTN |
                   BE_U1000_CRU_SYSCR0_CORE2FPRSTN |
                   BE_U1000_CRU_SYSCR0_CORE2RSTN;
    return 0;
}

bool hal_corectrl_is_in_reset_all(hal_corectrl_core_t core)
{
    uint32_t mask;
    volatile be_u1000_cru_t *cru = be_u1000_cru();

    if (!be_u1000_corectrl_core_valid(core)) {
        return true;
    }

    if (core == HAL_CORECTRL_CORE1) {
        mask = BE_U1000_CRU_SYSCR0_CORE1CXRSTN |
               BE_U1000_CRU_SYSCR0_CORE1FPRSTN |
               BE_U1000_CRU_SYSCR0_CORE1RSTN;
    } else {
        mask = BE_U1000_CRU_SYSCR0_CORE2CXRSTN |
               BE_U1000_CRU_SYSCR0_CORE2FPRSTN |
               BE_U1000_CRU_SYSCR0_CORE2RSTN;
    }

    return (cru->SYSCR0 & mask) == 0u;
}

int hal_corectrl_core2_release_reset_cx(void)
{
    be_u1000_cru()->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE2CXRSTN;
    return 0;
}

int hal_corectrl_core2_release_reset_fp(void)
{
    be_u1000_cru()->SYSCR0 |= BE_U1000_CRU_SYSCR0_CORE2FPRSTN;
    return 0;
}

int hal_corectrl_core2_copy_image(const void *image, uintptr_t system_dst, uint32_t size)
{
    const uint8_t *src = (const uint8_t *)image;
    volatile uint8_t *dst = (volatile uint8_t *)system_dst;
    uint32_t i;

    if (!image || size == 0u) {
        return -1;
    }

    for (i = 0; i < size; ++i) {
        dst[i] = src[i];
    }

    return 0;
}
