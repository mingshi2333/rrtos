#ifndef AI_DESCRIPTOR_RUNTIME_H
#define AI_DESCRIPTOR_RUNTIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AI_DESCRIPTOR_REGION_INPUT = 0,
    AI_DESCRIPTOR_REGION_OUTPUT = 1,
    AI_DESCRIPTOR_REGION_ARENA = 2,
    AI_DESCRIPTOR_REGION_CONST = 3,
} ai_descriptor_region_t;

typedef enum {
    AI_DESCRIPTOR_ELEMENT_F32 = 0,
} ai_descriptor_element_type_t;

typedef enum {
    AI_DESCRIPTOR_OP_FULLY_CONNECTED_F32 = 0,
    AI_DESCRIPTOR_OP_SOFTMAX_F32 = 1,
} ai_descriptor_op_type_t;

typedef enum {
    AI_DESCRIPTOR_ACTIVATION_NONE = 0,
    AI_DESCRIPTOR_ACTIVATION_RELU = 1,
} ai_descriptor_activation_t;

typedef struct {
    ai_descriptor_region_t region;
    ai_descriptor_element_type_t element_type;
    uint32_t offset_bytes;
    uint32_t length_bytes;
} ai_descriptor_tensor_t;

typedef struct {
    uint16_t input_tensor;
    uint16_t weights_tensor;
    uint16_t bias_tensor;
    uint16_t output_tensor;
    uint32_t input_count;
    uint32_t output_count;
    ai_descriptor_activation_t activation;
} ai_descriptor_fc_f32_t;

typedef struct {
    uint16_t input_tensor;
    uint16_t output_tensor;
    uint32_t element_count;
} ai_descriptor_softmax_f32_t;

typedef struct {
    ai_descriptor_op_type_t type;
    union {
        ai_descriptor_fc_f32_t fc_f32;
        ai_descriptor_softmax_f32_t softmax_f32;
    } config;
} ai_descriptor_op_t;

typedef struct {
    const ai_descriptor_tensor_t *tensors;
    uint32_t tensor_count;
    const ai_descriptor_op_t *ops;
    uint32_t op_count;
    uint32_t arena_bytes;
} ai_descriptor_model_t;

typedef struct {
    const uint8_t *const_data;
    uint32_t const_data_bytes;
    const uint8_t *input;
    uint32_t input_bytes;
    uint8_t *output;
    uint32_t output_bytes;
    uint8_t *arena;
    uint32_t arena_bytes;
} ai_descriptor_context_t;

uint32_t ai_descriptor_required_arena_bytes(const ai_descriptor_model_t *model);

int ai_descriptor_invoke(
    const ai_descriptor_model_t *model,
    ai_descriptor_context_t *context);

#ifdef __cplusplus
}
#endif

#endif
