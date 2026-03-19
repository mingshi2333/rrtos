#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_pwmg.h"

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t RESERVED0[2];
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t RESERVED1;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RESERVED2;
    volatile uint32_t CCR0;
} be_u1000_pwmg_t;

static volatile be_u1000_pwmg_t *g_pwmg;

#define PWMG_CR1_CEN            (1u << 0)
#define PWMG_CR1_CKD_Pos        8u
#define PWMG_CR1_CKD_Msk        (0x3u << PWMG_CR1_CKD_Pos)

#define PWMG_CCMR1_CC0S_OC      (0u << 0)
#define PWMG_CCMR1_OC0PE        (1u << 3)
#define PWMG_CCMR1_OC0M_Pos     4u
#define PWMG_CCMR1_OC0M_Msk     (0x7u << PWMG_CCMR1_OC0M_Pos)

#define PWMG_CCER_CC0E          (1u << 0)
#define PWMG_DIER_CC0IE         (1u << 1)
#define PWMG_SR_CC0IF           (1u << 1)

int hal_pwmg_init(uintptr_t base, const hal_pwmg_config_t *config)
{
    uint32_t cr1;
    uint32_t ccmr1;

    if (base == 0 || config == NULL) {
        return -1;
    }

    g_pwmg = (volatile be_u1000_pwmg_t *)base;
    if (g_pwmg == NULL) {
        return -1;
    }

    cr1 = ((uint32_t)config->clock_division << PWMG_CR1_CKD_Pos) & PWMG_CR1_CKD_Msk;
    ccmr1 = PWMG_CCMR1_CC0S_OC |
            PWMG_CCMR1_OC0PE |
            (((uint32_t)config->oc_mode << PWMG_CCMR1_OC0M_Pos) & PWMG_CCMR1_OC0M_Msk);

    g_pwmg->CR1 = cr1;
    g_pwmg->DIER = 0u;
    g_pwmg->SR = 0u;
    g_pwmg->PSC = config->prescaler;
    g_pwmg->ARR = config->autoreload;
    g_pwmg->CCR0 = config->compare;
    g_pwmg->CCMR1 = ccmr1;
    g_pwmg->CCER = PWMG_CCER_CC0E;
    return 0;
}

int hal_pwmg_enable_counter(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    g_pwmg->CR1 |= PWMG_CR1_CEN;
    return 0;
}

int hal_pwmg_enable_compare_interrupt(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    g_pwmg->DIER |= PWMG_DIER_CC0IE;
    return 0;
}

int hal_pwmg_disable_compare_interrupt(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    g_pwmg->DIER &= ~PWMG_DIER_CC0IE;
    return 0;
}

int hal_pwmg_get_counter(uint16_t *counter)
{
    if (g_pwmg == NULL || counter == NULL) {
        return -1;
    }

    *counter = (uint16_t)(g_pwmg->CNT & 0xFFFFu);
    return 0;
}

int hal_pwmg_get_compare(uint16_t *compare)
{
    if (g_pwmg == NULL || compare == NULL) {
        return -1;
    }

    *compare = (uint16_t)(g_pwmg->CCR0 & 0xFFFFu);
    return 0;
}

int hal_pwmg_compare_interrupt_enabled(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    return (g_pwmg->DIER & PWMG_DIER_CC0IE) != 0u;
}

int hal_pwmg_compare_match_pending(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    return (g_pwmg->SR & PWMG_SR_CC0IF) != 0u;
}

int hal_pwmg_clear_compare_match(void)
{
    if (g_pwmg == NULL) {
        return -1;
    }

    g_pwmg->SR = PWMG_SR_CC0IF;
    return 0;
}
