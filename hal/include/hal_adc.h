#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAL_ADC_CHANNEL_0 = 0,
    HAL_ADC_CHANNEL_1 = 1,
    HAL_ADC_CHANNEL_2 = 2,
    HAL_ADC_CHANNEL_3 = 3,
    HAL_ADC_CHANNEL_4 = 4,
    HAL_ADC_CHANNEL_5 = 5,
    HAL_ADC_CHANNEL_6 = 6,
    HAL_ADC_CHANNEL_7 = 7,
} hal_adc_channel_t;

typedef enum {
    HAL_ADC_SAMPLE_0_CYCLES = 0,
    HAL_ADC_SAMPLE_25_CYCLES = 1,
    HAL_ADC_SAMPLE_50_CYCLES = 2,
    HAL_ADC_SAMPLE_75_CYCLES = 3,
    HAL_ADC_SAMPLE_125_CYCLES = 4,
    HAL_ADC_SAMPLE_175_CYCLES = 5,
    HAL_ADC_SAMPLE_225_CYCLES = 6,
    HAL_ADC_SAMPLE_275_CYCLES = 7,
} hal_adc_sample_time_t;

typedef struct {
    hal_adc_channel_t channel;
    hal_adc_sample_time_t sample_time;
    uint8_t clock_division;
    bool signed_output;
    bool differential_input;
} hal_adc_config_t;

int hal_adc_init(uintptr_t base, const hal_adc_config_t *config);
int hal_adc_start_single(void);
int hal_adc_poll_eoc(uint32_t timeout);
int hal_adc_read_u12(uint16_t *value);
int hal_adc_convert_single(uint16_t *value, uint32_t timeout);

#endif
