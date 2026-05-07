#include "rrtos_ai_ukernel.h"

static int8_t saturate_i8(int32_t value, int32_t activation_min,
                          int32_t activation_max) {
    if (activation_min < -128) {
        activation_min = -128;
    }
    if (activation_max > 127) {
        activation_max = 127;
    }
    if (value < activation_min) {
        value = activation_min;
    }
    if (value > activation_max) {
        value = activation_max;
    }
    return (int8_t)value;
}

static int32_t clamp_i64_to_i32(int64_t value) {
    if (value > 2147483647LL) {
        return 2147483647;
    }
    if (value < (-2147483647LL - 1LL)) {
        return (-2147483647 - 1);
    }
    return (int32_t)value;
}

static int32_t rounding_high_mul(int32_t lhs, int32_t rhs) {
    int64_t product = (int64_t)lhs * (int64_t)rhs;
    int64_t nudge = product >= 0 ? (1LL << 30) : (1LL - (1LL << 30));
    int64_t adjusted = product + nudge;
    if (adjusted >= 0) {
        return (int32_t)(adjusted >> 31);
    }
    return (int32_t)(-((-adjusted) >> 31));
}

static int32_t rounding_divide_by_power_of_two(int32_t value, int32_t exponent) {
    if (exponent <= 0) {
        return value;
    }
    if (exponent >= 31) {
        return value >= 0 ? 0 : -1;
    }

    const int32_t mask = (1 << exponent) - 1;
    const int32_t remainder = value & mask;
    const int32_t threshold = (mask >> 1) + (value < 0 ? 1 : 0);
    return (value >> exponent) + (remainder > threshold ? 1 : 0);
}

static int32_t multiply_by_quantized_multiplier(int32_t value, int32_t multiplier,
                                                int32_t shift) {
    const int32_t left_shift = shift > 0 ? shift : 0;
    const int32_t right_shift = shift > 0 ? 0 : -shift;
    int64_t shifted = (int64_t)value * (1LL << left_shift);
    int32_t high_mul = rounding_high_mul(clamp_i64_to_i32(shifted), multiplier);
    return rounding_divide_by_power_of_two(high_mul, right_shift);
}

static int32_t round_float_to_i32(float value) {
    if (value >= 2147483520.0f) {
        return 2147483647;
    }
    if (value <= -2147483648.0f) {
        return (-2147483647 - 1);
    }
    return (int32_t)(value >= 0.0f ? value + 0.5f : value - 0.5f);
}

static int32_t requantize_accumulator(int32_t acc,
                                      int32_t channel,
                                      const float *float_multipliers,
                                      const int32_t *q31_multipliers,
                                      const int32_t *q31_shifts) {
    if (q31_multipliers != 0 && q31_shifts != 0) {
        return multiply_by_quantized_multiplier(
            acc, q31_multipliers[channel], q31_shifts[channel]);
    }
    return round_float_to_i32((float)acc * float_multipliers[channel]);
}

static int range_exceeds_i32(int32_t begin, int32_t count, int32_t limit) {
    if (begin < 0 || count < 0 || limit < 0 || begin > limit) {
        return 1;
    }
    return count > limit - begin;
}

static int padded_dim_excludes_halo(int32_t padded_dim, int32_t output_dim) {
    if (padded_dim < 2 || output_dim <= 0) {
        return 1;
    }
    return padded_dim - 2 < output_dim;
}

static int is_invalid_conv_params(
    const rrtos_ai_ukernel_conv2d_i8_tile_params_t *params) {
    if (params == 0) {
        return 1;
    }
    if (params->input_h <= 0 || params->input_w <= 0 || params->input_c <= 0) {
        return 1;
    }
    if (params->output_h <= 0 || params->output_w <= 0 ||
        params->output_c <= 0) {
        return 1;
    }
    if (params->kernel_h <= 0 || params->kernel_w <= 0 ||
        params->stride_h <= 0 || params->stride_w <= 0) {
        return 1;
    }
    if (range_exceeds_i32(params->tile_row_begin,
                          params->tile_row_count,
                          params->output_h)) {
        return 1;
    }
    if (params->weight_layout != RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_HWIO &&
        params->weight_layout != RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_OHWI) {
        return 1;
    }
    return 0;
}

static int is_invalid_requant_tile_params(
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    if (params == 0) {
        return 1;
    }
    if (params->struct_size != sizeof(*params) ||
        params->abi_version != RRTOS_AI_UKERNEL_ABI_VERSION ||
        params->flags != 0u || params->reserved != 0u) {
        return 1;
    }
    if (params->input_h <= 0 || params->input_w <= 0 || params->input_c <= 0) {
        return 1;
    }
    if (params->output_h <= 0 || params->output_w <= 0 ||
        params->output_c <= 0) {
        return 1;
    }
    if (params->kernel_h <= 0 || params->kernel_w <= 0 ||
        params->stride_h <= 0 || params->stride_w <= 0) {
        return 1;
    }
    if (params->tile_row_count <= 0 ||
        range_exceeds_i32(params->tile_row_begin,
                          params->tile_row_count,
                          params->output_h)) {
        return 1;
    }
    if (padded_dim_excludes_halo(params->padded_output_h, params->output_h) ||
        padded_dim_excludes_halo(params->padded_output_w, params->output_w)) {
        return 1;
    }
    if (params->activation_min > params->activation_max) {
        return 1;
    }
    return 0;
}

static int32_t conv2d_weight_index(
    const rrtos_ai_ukernel_conv2d_i8_tile_params_t *params,
    int32_t kh,
    int32_t kw,
    int32_t ic,
    int32_t oc) {
    if (params->weight_layout == RRTOS_AI_UKERNEL_CONV2D_WEIGHT_LAYOUT_OHWI) {
        return (((oc * params->kernel_h + kh) * params->kernel_w + kw) *
                params->input_c) +
               ic;
    }
    return (((kh * params->kernel_w + kw) * params->input_c + ic) *
            params->output_c) +
           oc;
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_tile(
    const int8_t *input, const int8_t *weights, const int32_t *bias,
    int32_t *accumulator,
    const rrtos_ai_ukernel_conv2d_i8_tile_params_t *params) {
    if (input == 0 || weights == 0 || accumulator == 0 ||
        is_invalid_conv_params(params)) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    for (int32_t tr = 0; tr < params->tile_row_count; ++tr) {
        const int32_t oh = params->tile_row_begin + tr;
        for (int32_t ow = 0; ow < params->output_w; ++ow) {
            for (int32_t oc = 0; oc < params->output_c; ++oc) {
                int32_t acc = bias != 0 ? bias[oc] : 0;

                for (int32_t kh = 0; kh < params->kernel_h; ++kh) {
                    const int32_t ih = oh * params->stride_h + kh - params->pad_top;
                    if (ih < 0 || ih >= params->input_h) {
                        continue;
                    }
                    for (int32_t kw = 0; kw < params->kernel_w; ++kw) {
                        const int32_t iw =
                            ow * params->stride_w + kw - params->pad_left;
                        if (iw < 0 || iw >= params->input_w) {
                            continue;
                        }
                        for (int32_t ic = 0; ic < params->input_c; ++ic) {
                            const int32_t input_index =
                                ((ih * params->input_w + iw) * params->input_c) + ic;
                            const int32_t weight_index =
                                conv2d_weight_index(params, kh, kw, ic, oc);
                            const int32_t input_value =
                                (int32_t)input[input_index] - params->input_zero_point;
                            const int32_t weight_value =
                                (int32_t)weights[weight_index] -
                                params->weight_zero_point;
                            acc += input_value * weight_value;
                        }
                    }
                }

                const int32_t out_index =
                    ((tr * params->output_w + ow) * params->output_c) + oc;
                accumulator[out_index] = acc;
            }
        }
    }

    return RRTOS_AI_UKERNEL_OK;
}

static void conv2d_ohwi_requant_tile_fast_ch1_7x7(
    const int8_t *input, const int8_t *weights, const int32_t *bias,
    const float *float_multipliers, const int32_t *q31_multipliers,
    const int32_t *q31_shifts, int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    enum {
        KERNEL_H = 7,
        KERNEL_W = 7,
        STRIDE_H = 2,
        STRIDE_W = 2,
        PAD_TOP = 3,
        PAD_LEFT = 3,
    };

    const int32_t input_h = params->input_h;
    const int32_t input_w = params->input_w;
    const int32_t output_w = params->output_w;
    const int32_t output_c = params->output_c;
    const int32_t padded_output_w = params->padded_output_w;
    const int32_t input_zero_point = params->input_zero_point;
    const int32_t output_zero_point = params->output_zero_point;
    const int32_t activation_min = params->activation_min;
    const int32_t activation_max = params->activation_max;

    for (int32_t tr = 0; tr < params->tile_row_count; ++tr) {
        const int32_t output_row = params->tile_row_begin + tr;
        const int32_t output_h_offset = output_row + 1;
        for (int32_t ow = 0; ow < output_w; ++ow) {
            for (int32_t oc = 0; oc < output_c; ++oc) {
                int32_t acc = bias[oc];
                const int32_t weight_oc_offset = oc * KERNEL_H * KERNEL_W;

                for (int32_t kh = 0; kh < KERNEL_H; ++kh) {
                    const int32_t ih = output_row * STRIDE_H + kh - PAD_TOP;
                    if (ih < 0 || ih >= input_h) {
                        continue;
                    }

                    const int32_t input_row_offset = ih * input_w;
                    const int32_t weight_kh_offset =
                        weight_oc_offset + kh * KERNEL_W;
                    for (int32_t kw = 0; kw < KERNEL_W; ++kw) {
                        const int32_t iw = ow * STRIDE_W + kw - PAD_LEFT;
                        if (iw < 0 || iw >= input_w) {
                            continue;
                        }

                        const int32_t input_value =
                            (int32_t)input[input_row_offset + iw] -
                            input_zero_point;
                        const int32_t weight_value =
                            (int32_t)weights[weight_kh_offset + kw];
                        acc += input_value * weight_value;
                    }
                }

                const int32_t output_q =
                    requantize_accumulator(acc,
                                           oc,
                                           float_multipliers,
                                           q31_multipliers,
                                           q31_shifts) +
                    output_zero_point;
                const int32_t output_index =
                    ((output_h_offset * padded_output_w + (ow + 1)) *
                        output_c) +
                    oc;
                padded_output[output_index] =
                    saturate_i8(output_q, activation_min, activation_max);
            }
        }
    }
}

static void conv2d_ohwi_requant_tile_generic(
    const int8_t *input, const int8_t *weights, const int32_t *bias,
    const float *float_multipliers, const int32_t *q31_multipliers,
    const int32_t *q31_shifts, int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    for (int32_t tr = 0; tr < params->tile_row_count; ++tr) {
        const int32_t output_row = params->tile_row_begin + tr;
        const int32_t output_h_offset = output_row + 1;
        for (int32_t ow = 0; ow < params->output_w; ++ow) {
            for (int32_t oc = 0; oc < params->output_c; ++oc) {
                int32_t acc = bias[oc];
                for (int32_t kh = 0; kh < params->kernel_h; ++kh) {
                    const int32_t ih =
                        output_row * params->stride_h + kh - params->pad_top;
                    if (ih < 0 || ih >= params->input_h) {
                        continue;
                    }
                    for (int32_t kw = 0; kw < params->kernel_w; ++kw) {
                        const int32_t iw =
                            ow * params->stride_w + kw - params->pad_left;
                        if (iw < 0 || iw >= params->input_w) {
                            continue;
                        }
                        for (int32_t ic = 0; ic < params->input_c; ++ic) {
                            const int32_t input_index =
                                ((ih * params->input_w + iw) *
                                 params->input_c) +
                                ic;
                            const int32_t weight_index =
                                (((oc * params->kernel_h + kh) *
                                      params->kernel_w +
                                  kw) *
                                     params->input_c) +
                                ic;
                            const int32_t input_value =
                                (int32_t)input[input_index] -
                                params->input_zero_point;
                            const int32_t weight_value =
                                (int32_t)weights[weight_index] -
                                params->weight_zero_point;
                            acc += input_value * weight_value;
                        }
                    }
                }

                const int32_t output_q =
                    requantize_accumulator(acc,
                                           oc,
                                           float_multipliers,
                                           q31_multipliers,
                                           q31_shifts) +
                    params->output_zero_point;
                const int32_t output_index =
                    ((output_h_offset * params->padded_output_w + (ow + 1)) *
                        params->output_c) +
                    oc;
                padded_output[output_index] = saturate_i8(
                    output_q, params->activation_min, params->activation_max);
            }
        }
    }
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
    const int8_t *input, const int8_t *weights, const int32_t *bias,
    const float *multipliers, int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    if (input == 0 || weights == 0 || bias == 0 || multipliers == 0 ||
        padded_output == 0 || is_invalid_requant_tile_params(params)) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    if (params->input_c == 1 && params->kernel_h == 7 &&
        params->kernel_w == 7 && params->stride_h == 2 &&
        params->stride_w == 2 && params->pad_top == 3 &&
        params->pad_left == 3 && params->weight_zero_point == 0) {
        conv2d_ohwi_requant_tile_fast_ch1_7x7(
            input,
            weights,
            bias,
            multipliers,
            0,
            0,
            padded_output,
            params);
    } else {
        conv2d_ohwi_requant_tile_generic(input,
                                         weights,
                                         bias,
                                         multipliers,
                                         0,
                                         0,
                                         padded_output,
                                         params);
    }
    return RRTOS_AI_UKERNEL_OK;
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(
    const int8_t *input, const int8_t *weights, const int32_t *bias,
    const int32_t *multipliers, const int32_t *shifts, int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params) {
    if (input == 0 || weights == 0 || bias == 0 || multipliers == 0 ||
        shifts == 0 || padded_output == 0 ||
        is_invalid_requant_tile_params(params)) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    if (params->input_c == 1 && params->kernel_h == 7 &&
        params->kernel_w == 7 && params->stride_h == 2 &&
        params->stride_w == 2 && params->pad_top == 3 &&
        params->pad_left == 3 && params->weight_zero_point == 0) {
        conv2d_ohwi_requant_tile_fast_ch1_7x7(
            input, weights, bias, 0, multipliers, shifts, padded_output, params);
    } else {
        conv2d_ohwi_requant_tile_generic(input,
                                         weights,
                                         bias,
                                         0,
                                         multipliers,
                                         shifts,
                                         padded_output,
                                         params);
    }
    return RRTOS_AI_UKERNEL_OK;
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_requant_i32_to_i8_tile(
    const int32_t *input, int8_t *output, int32_t count, int32_t multiplier,
    int32_t shift,
    const rrtos_ai_ukernel_requant_i32_to_i8_params_t *params) {
    if (input == 0 || output == 0 || params == 0 || count < 0) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    for (int32_t i = 0; i < count; ++i) {
        int32_t value =
            multiply_by_quantized_multiplier(input[i], multiplier, shift);
        value += params->output_zero_point;
        output[i] = saturate_i8(value, params->activation_min,
                                params->activation_max);
    }

    return RRTOS_AI_UKERNEL_OK;
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_maxpool_i8_tile(
    const int8_t *input, int8_t *output, int32_t input_h, int32_t input_w,
    int32_t channels, int32_t pool_h, int32_t pool_w, int32_t stride_h,
    int32_t stride_w, int32_t tile_row_begin, int32_t tile_row_count) {
    if (input == 0 || output == 0 || input_h <= 0 || input_w <= 0 ||
        channels <= 0 || pool_h <= 0 || pool_w <= 0 || stride_h <= 0 ||
        stride_w <= 0 || tile_row_begin < 0 || tile_row_count < 0) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    const int32_t output_h = (input_h - pool_h) / stride_h + 1;
    const int32_t output_w = (input_w - pool_w) / stride_w + 1;
    if (output_h <= 0 || output_w <= 0 ||
        range_exceeds_i32(tile_row_begin, tile_row_count, output_h)) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    for (int32_t tr = 0; tr < tile_row_count; ++tr) {
        const int32_t oh = tile_row_begin + tr;
        for (int32_t ow = 0; ow < output_w; ++ow) {
            for (int32_t c = 0; c < channels; ++c) {
                int32_t max_value = -128;
                for (int32_t kh = 0; kh < pool_h; ++kh) {
                    const int32_t ih = oh * stride_h + kh;
                    for (int32_t kw = 0; kw < pool_w; ++kw) {
                        const int32_t iw = ow * stride_w + kw;
                        const int32_t input_index =
                            ((ih * input_w + iw) * channels) + c;
                        const int32_t value = input[input_index];
                        if (value > max_value) {
                            max_value = value;
                        }
                    }
                }
                const int32_t output_index = ((tr * output_w + ow) * channels) + c;
                output[output_index] = (int8_t)max_value;
            }
        }
    }

    return RRTOS_AI_UKERNEL_OK;
}

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_add_i8_tile(
    const int8_t *lhs, const int8_t *rhs, int8_t *output, int32_t count,
    int32_t activation_min, int32_t activation_max) {
    if (lhs == 0 || rhs == 0 || output == 0 || count < 0) {
        return RRTOS_AI_UKERNEL_INVALID_ARGUMENT;
    }

    for (int32_t i = 0; i < count; ++i) {
        output[i] = saturate_i8((int32_t)lhs[i] + (int32_t)rhs[i],
                                activation_min, activation_max);
    }

    return RRTOS_AI_UKERNEL_OK;
}

void rrtos_ai_ukernel_clamp_i8(int8_t *data, int32_t count,
                               int32_t activation_min,
                               int32_t activation_max) {
    if (data == 0 || count <= 0) {
        return;
    }
    for (int32_t i = 0; i < count; ++i) {
        data[i] = saturate_i8(data[i], activation_min, activation_max);
    }
}
