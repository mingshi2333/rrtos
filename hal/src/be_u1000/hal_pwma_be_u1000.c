#include <stddef.h>
#include <stdint.h>

#include "../../include/hal_pwma.h"

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR0;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} be_u1000_pwma_t;

static volatile be_u1000_pwma_t *g_pwma;

#define PWMA_CR1_CEN           (1u << 0)
#define PWMA_CR1_DIR_Pos       4u
#define PWMA_CR1_DIR_Msk       (1u << PWMA_CR1_DIR_Pos)
#define PWMA_CR1_CMS_Pos       5u
#define PWMA_CR1_CMS_Msk       (0x3u << PWMA_CR1_CMS_Pos)
#define PWMA_CR1_ARPE          (1u << 7)
#define PWMA_CR1_CKD_Pos       8u
#define PWMA_CR1_CKD_Msk       (0x3u << PWMA_CR1_CKD_Pos)

#define PWMA_EGR_UPDATE        (1u << 0)
#define PWMA_DIER_UIE          (1u << 0)
#define PWMA_SR_UIF            (1u << 0)

int hal_pwma_init(uintptr_t base, const hal_pwma_config_t *config)
{
    uint32_t cr1;

    if (base == 0 || config == NULL) {
        return -1;
    }

    g_pwma = (volatile be_u1000_pwma_t *)base;
    if (g_pwma == NULL) {
        return -1;
    }

    cr1 = PWMA_CR1_ARPE |
          (((uint32_t)config->clock_division << PWMA_CR1_CKD_Pos) & PWMA_CR1_CKD_Msk);

    switch (config->counter_mode) {
    case HAL_PWMA_COUNTER_DOWN:
        cr1 |= (1u << PWMA_CR1_DIR_Pos);
        break;
    case HAL_PWMA_COUNTER_CENTER1:
        cr1 |= (1u << PWMA_CR1_CMS_Pos);
        break;
    case HAL_PWMA_COUNTER_CENTER2:
        cr1 |= (2u << PWMA_CR1_CMS_Pos);
        break;
    case HAL_PWMA_COUNTER_CENTER3:
        cr1 |= (3u << PWMA_CR1_CMS_Pos);
        break;
    default:
        break;
    }

    g_pwma->CR1 = cr1;
    g_pwma->PSC = config->prescaler;
    g_pwma->ARR = config->autoreload;
    g_pwma->RCR = config->repetition_counter;
    g_pwma->EGR = PWMA_EGR_UPDATE;
    return 0;
}

int hal_pwma_enable_counter(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->CR1 |= PWMA_CR1_CEN;
    return 0;
}

int hal_pwma_enable_update_interrupt(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->DIER |= PWMA_DIER_UIE;
    return 0;
}

int hal_pwma_disable_update_interrupt(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->DIER &= ~PWMA_DIER_UIE;
    return 0;
}

int hal_pwma_set_autoreload(uint16_t value)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->ARR = value;
    g_pwma->EGR = PWMA_EGR_UPDATE;
    return 0;
}

int hal_pwma_set_repetition_counter(uint8_t value)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->RCR = value;
    return 0;
}

int hal_pwma_get_counter(uint16_t *counter)
{
    if (g_pwma == NULL || counter == NULL) {
        return -1;
    }

    *counter = (uint16_t)(g_pwma->CNT & 0xFFFFu);
    return 0;
}

int hal_pwma_get_repetition_counter(uint8_t *value)
{
    if (g_pwma == NULL || value == NULL) {
        return -1;
    }

    *value = (uint8_t)(g_pwma->RCR & 0xFFu);
    return 0;
}

int hal_pwma_update_interrupt_enabled(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    return (g_pwma->DIER & PWMA_DIER_UIE) != 0u;
}

int hal_pwma_update_pending(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    return (g_pwma->SR & PWMA_SR_UIF) != 0u;
}

int hal_pwma_clear_update(void)
{
    if (g_pwma == NULL) {
        return -1;
    }

    g_pwma->SR = PWMA_SR_UIF;
    return 0;
}
