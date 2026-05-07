#ifndef RRTOS_AI_UKERNEL_H
#define RRTOS_AI_UKERNEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RRTOS_AI_UKERNEL_ABI_VERSION 1u

typedef enum rrtos_ai_ukernel_status {
    RRTOS_AI_UKERNEL_OK = 0,
    RRTOS_AI_UKERNEL_INVALID_ARGUMENT = -1,
} rrtos_ai_ukernel_status_t;

typedef enum rrtos_ai_ukernel_conv2d_weight_layout {
    RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_HWIO = 0,
    RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_OHWI = 1,
} rrtos_ai_ukernel_conv2d_weight_layout_t;

typedef struct rrtos_ai_ukernel_conv2d_i8_tile_params {
    int32_t input_h;
    int32_t input_w;
    int32_t input_c;
    int32_t output_h;
    int32_t output_w;
    int32_t output_c;
    int32_t kernel_h;
    int32_t kernel_w;
    int32_t stride_h;
    int32_t stride_w;
    int32_t pad_top;
    int32_t pad_left;
    int32_t tile_row_begin;
    int32_t tile_row_count;
    int32_t input_zero_point;
    int32_t weight_zero_point;
    rrtos_ai_ukernel_conv2d_weight_layout_t weight_layout;
} rrtos_ai_ukernel_conv2d_i8_tile_params_t;

typedef struct rrtos_ai_ukernel_requant_i32_to_i8_params {
    int32_t output_zero_point;
    int32_t activation_min;
    int32_t activation_max;
} rrtos_ai_ukernel_requant_i32_to_i8_params_t;

typedef struct rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params {
    uint32_t struct_size;
    uint32_t abi_version;
    uint32_t flags;
    uint32_t reserved;
    int32_t input_h;
    int32_t input_w;
    int32_t input_c;
    int32_t output_h;
    int32_t output_w;
    int32_t output_c;
    int32_t kernel_h;
    int32_t kernel_w;
    int32_t stride_h;
    int32_t stride_w;
    int32_t pad_top;
    int32_t pad_left;
    int32_t tile_row_begin;
    int32_t tile_row_count;
    int32_t padded_output_h;
    int32_t padded_output_w;
    int32_t input_zero_point;
    int32_t weight_zero_point;
    int32_t output_zero_point;
    int32_t activation_min;
    int32_t activation_max;
} rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t;

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_tile(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    int32_t *accumulator,
    const rrtos_ai_ukernel_conv2d_i8_tile_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const float *multipliers,
    int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const int32_t *multipliers,
    const int32_t *shifts,
    int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_requant_i32_to_i8_tile(
    const int32_t *input,
    int8_t *output,
    int32_t count,
    int32_t multiplier,
    int32_t shift,
    const rrtos_ai_ukernel_requant_i32_to_i8_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_maxpool_i8_tile(
    const int8_t *input,
    int8_t *output,
    int32_t input_h,
    int32_t input_w,
    int32_t channels,
    int32_t pool_h,
    int32_t pool_w,
    int32_t stride_h,
    int32_t stride_w,
    int32_t tile_row_begin,
    int32_t tile_row_count);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_add_i8_tile(
    const int8_t *lhs,
    const int8_t *rhs,
    int8_t *output,
    int32_t count,
    int32_t activation_min,
    int32_t activation_max);

void rrtos_ai_ukernel_clamp_i8(
    int8_t *data,
    int32_t count,
    int32_t activation_min,
    int32_t activation_max);

#ifdef __cplusplus
}
#endif

#endif /* RRTOS_AI_UKERNEL_H */
