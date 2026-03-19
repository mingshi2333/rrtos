#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hal_adc.h"

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t ASER;
    volatile uint32_t ASTR;
    volatile uint32_t ASMPR;
    volatile uint32_t ASQR;
    volatile uint32_t ADR;
} be_u1000_adc_t;

static volatile be_u1000_adc_t *g_adc;

#define ADC_CR1_ADON              (1u << 0)
#define ADC_CR1_SWSTART           (1u << 15)
#define ADC_CR1_CKD_Pos           16u
#define ADC_CR1_CKD_Msk           (0xFu << ADC_CR1_CKD_Pos)

#define ADC_ASER_SELVI_HD_LS      (1u << 0)
#define ADC_ASER_SELDO_HS_LU      (1u << 1)

#define ADC_ASTR_EOC              (1u << 0)
#define ADC_ASTR_STRT             (1u << 1)

#define ADC_ASMPR_SMP0_Msk        0x7u
#define ADC_ASQR_SQ0_Msk          0x7u

#define ADC_ADR_VALUE_Msk         0x0FFFu

static uint32_t hal_adc_clock_div_bits(uint8_t division)
{
    if (division < 1u || division > 16u) {
        return 3u << ADC_CR1_CKD_Pos;
    }

    return ((uint32_t)(division - 1u) & 0xFu) << ADC_CR1_CKD_Pos;
}

int hal_adc_init(uintptr_t base, const hal_adc_config_t *config)
{
    uint32_t cr1;
    uint32_t aser;

    if (base == 0 || config == NULL || config->channel > HAL_ADC_CHANNEL_7) {
        return -1;
    }

    g_adc = (volatile be_u1000_adc_t *)base;
    if (g_adc == NULL) {
        return -1;
    }

    cr1 = ADC_CR1_ADON | hal_adc_clock_div_bits(config->clock_division);
    aser = 0u;
    if (config->differential_input) {
        aser |= ADC_ASER_SELVI_HD_LS;
    }
    if (config->signed_output) {
        aser |= ADC_ASER_SELDO_HS_LU;
    }

    g_adc->CR1 = cr1;
    g_adc->ASER = aser;
    g_adc->ASMPR = ((uint32_t)config->sample_time & ADC_ASMPR_SMP0_Msk);
    g_adc->ASQR = ((uint32_t)config->channel & ADC_ASQR_SQ0_Msk);
    g_adc->ASTR = ADC_ASTR_EOC | ADC_ASTR_STRT;

    return 0;
}

int hal_adc_start_single(void)
{
    if (g_adc == NULL) {
        return -1;
    }

    g_adc->ASTR = ADC_ASTR_EOC | ADC_ASTR_STRT;
    g_adc->CR1 |= ADC_CR1_SWSTART;
    return 0;
}

int hal_adc_poll_eoc(uint32_t timeout)
{
    uint32_t i;

    if (g_adc == NULL) {
        return -1;
    }

    for (i = 0; i < timeout; ++i) {
        if ((g_adc->ASTR & ADC_ASTR_EOC) != 0u) {
            return 0;
        }
    }

    return -1;
}

int hal_adc_read_u12(uint16_t *value)
{
    if (g_adc == NULL || value == NULL) {
        return -1;
    }

    *value = (uint16_t)(g_adc->ADR & ADC_ADR_VALUE_Msk);
    return 0;
}

int hal_adc_convert_single(uint16_t *value, uint32_t timeout)
{
    if (hal_adc_start_single() != 0) {
        return -1;
    }

    if (hal_adc_poll_eoc(timeout) != 0) {
        return -1;
    }

    return hal_adc_read_u12(value);
}
