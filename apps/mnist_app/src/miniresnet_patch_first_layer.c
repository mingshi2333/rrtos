#include "miniresnet_patch_first_layer.h"

#include <stdint.h>
#include <string.h>

#include "rrtos_ai_ukernel.h"
#include "miniresnet_patch_first_layer_data.h"

enum {
    MINIRESNET_PATCH_INPUT_H = 64,
    MINIRESNET_PATCH_INPUT_W = 50,
    MINIRESNET_PATCH_INPUT_ZP = 127,
    MINIRESNET_PATCH_OUTPUT_H = 32,
    MINIRESNET_PATCH_OUTPUT_W = 25,
    MINIRESNET_PATCH_OUTPUT_C = 64,
    MINIRESNET_PATCH_OUTPUT_ZP = -128,
    MINIRESNET_PATCH_PADDED_OUTPUT_H = 34,
    MINIRESNET_PATCH_PADDED_OUTPUT_W = 27,
    MINIRESNET_PATCH_PADDED_OUTPUT_BYTES = 34 * 27 * 64,
};

rrtos_ai_ukernel_status_t miniresnet_patch_run_first_layer(
    const int8_t *input,
    uint8_t *arena,
    uint32_t output_offset) {
    int8_t *padded_output =
        (int8_t *)(void *)(arena + output_offset);
    rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t params = {
        .struct_size = sizeof(params),
        .abi_version = RRTOS_AI_UKERNEL_ABI_VERSION,
        .flags = 0,
        .reserved = 0,
        .input_h = MINIRESNET_PATCH_INPUT_H,
        .input_w = MINIRESNET_PATCH_INPUT_W,
        .input_c = 1,
        .output_h = MINIRESNET_PATCH_OUTPUT_H,
        .output_w = MINIRESNET_PATCH_OUTPUT_W,
        .output_c = MINIRESNET_PATCH_OUTPUT_C,
        .kernel_h = 7,
        .kernel_w = 7,
        .stride_h = 2,
        .stride_w = 2,
        .pad_top = 3,
        .pad_left = 3,
        .tile_row_begin = 0,
        .tile_row_count = MINIRESNET_PATCH_OUTPUT_H,
        .padded_output_h = MINIRESNET_PATCH_PADDED_OUTPUT_H,
        .padded_output_w = MINIRESNET_PATCH_PADDED_OUTPUT_W,
        .input_zero_point = MINIRESNET_PATCH_INPUT_ZP,
        .weight_zero_point = 0,
        .output_zero_point = MINIRESNET_PATCH_OUTPUT_ZP,
        .activation_min = -128,
        .activation_max = 127,
    };

    memset(padded_output,
           (uint8_t)(int8_t)MINIRESNET_PATCH_OUTPUT_ZP,
           MINIRESNET_PATCH_PADDED_OUTPUT_BYTES);

    rrtos_ai_ukernel_status_t rc =
        rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(
            input,
            miniresnet_patch_conv1_weights,
            miniresnet_patch_conv1_bias,
            miniresnet_patch_conv1_multiplier_q31,
            miniresnet_patch_conv1_shift,
            padded_output,
            &params);
    if (rc != RRTOS_AI_UKERNEL_OK) {
        return rc;
    }
    return RRTOS_AI_UKERNEL_OK;
}
