#include "ai_descriptor_runtime.h"

#include <math.h>
#include <stddef.h>

enum {
    AI_DESCRIPTOR_OK = 0,
    AI_DESCRIPTOR_ERR_INVALID_ARGUMENT = -1,
    AI_DESCRIPTOR_ERR_INVALID_TENSOR = -2,
    AI_DESCRIPTOR_ERR_UNSUPPORTED_OP = -3,
};

static int checked_span(
    const ai_descriptor_context_t *context,
    const ai_descriptor_tensor_t *tensor,
    uint8_t **out_base,
    uint32_t *out_length) {
    uint8_t *base = NULL;
    uint32_t region_bytes = 0;

    switch (tensor->region) {
    case AI_DESCRIPTOR_REGION_INPUT:
        base = (uint8_t *)(uintptr_t)context->input;
        region_bytes = context->input_bytes;
        break;
    case AI_DESCRIPTOR_REGION_OUTPUT:
        base = context->output;
        region_bytes = context->output_bytes;
        break;
    case AI_DESCRIPTOR_REGION_ARENA:
        base = context->arena;
        region_bytes = context->arena_bytes;
        break;
    case AI_DESCRIPTOR_REGION_CONST:
        base = (uint8_t *)(uintptr_t)context->const_data;
        region_bytes = context->const_data_bytes;
        break;
    default:
        return AI_DESCRIPTOR_ERR_INVALID_TENSOR;
    }

    if (base == NULL || tensor->offset_bytes > region_bytes ||
        tensor->length_bytes > region_bytes - tensor->offset_bytes) {
        return AI_DESCRIPTOR_ERR_INVALID_TENSOR;
    }

    *out_base = base + tensor->offset_bytes;
    *out_length = tensor->length_bytes;
    return AI_DESCRIPTOR_OK;
}

static int tensor_ptr(
    const ai_descriptor_model_t *model,
    ai_descriptor_context_t *context,
    uint16_t tensor_index,
    uint32_t required_bytes,
    uint8_t **out_ptr) {
    if (model == NULL || context == NULL || out_ptr == NULL ||
        tensor_index >= model->tensor_count) {
        return AI_DESCRIPTOR_ERR_INVALID_ARGUMENT;
    }

    const ai_descriptor_tensor_t *tensor = &model->tensors[tensor_index];
    if (tensor->element_type != AI_DESCRIPTOR_ELEMENT_F32 ||
        tensor->length_bytes < required_bytes) {
        return AI_DESCRIPTOR_ERR_INVALID_TENSOR;
    }

    uint8_t *base = NULL;
    uint32_t length = 0;
    int rc = checked_span(context, tensor, &base, &length);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }
    if (length < required_bytes) {
        return AI_DESCRIPTOR_ERR_INVALID_TENSOR;
    }

    *out_ptr = base;
    return AI_DESCRIPTOR_OK;
}

static int invoke_fc_f32(
    const ai_descriptor_model_t *model,
    ai_descriptor_context_t *context,
    const ai_descriptor_fc_f32_t *op) {
    const uint32_t input_bytes = op->input_count * (uint32_t)sizeof(float);
    const uint32_t output_bytes = op->output_count * (uint32_t)sizeof(float);
    const uint32_t weights_bytes =
        op->input_count * op->output_count * (uint32_t)sizeof(float);

    uint8_t *input_bytes_ptr = NULL;
    uint8_t *weights_bytes_ptr = NULL;
    uint8_t *bias_bytes_ptr = NULL;
    uint8_t *output_bytes_ptr = NULL;

    int rc = tensor_ptr(
        model, context, op->input_tensor, input_bytes, &input_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }
    rc = tensor_ptr(
        model, context, op->weights_tensor, weights_bytes, &weights_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }
    rc = tensor_ptr(
        model, context, op->bias_tensor, output_bytes, &bias_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }
    rc = tensor_ptr(
        model, context, op->output_tensor, output_bytes, &output_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }

    const float *input = (const float *)(const void *)input_bytes_ptr;
    const float *weights = (const float *)(const void *)weights_bytes_ptr;
    const float *bias = (const float *)(const void *)bias_bytes_ptr;
    float *output = (float *)(void *)output_bytes_ptr;

    for (uint32_t out = 0; out < op->output_count; ++out) {
        float acc = bias[out];
        for (uint32_t in = 0; in < op->input_count; ++in) {
            acc += input[in] * weights[in * op->output_count + out];
        }
        if (op->activation == AI_DESCRIPTOR_ACTIVATION_RELU && acc < 0.0f) {
            acc = 0.0f;
        }
        output[out] = acc;
    }

    return AI_DESCRIPTOR_OK;
}

static int invoke_softmax_f32(
    const ai_descriptor_model_t *model,
    ai_descriptor_context_t *context,
    const ai_descriptor_softmax_f32_t *op) {
    if (op->element_count == 0u) {
        return AI_DESCRIPTOR_ERR_INVALID_ARGUMENT;
    }

    const uint32_t bytes = op->element_count * (uint32_t)sizeof(float);
    uint8_t *input_bytes_ptr = NULL;
    uint8_t *output_bytes_ptr = NULL;

    int rc = tensor_ptr(
        model, context, op->input_tensor, bytes, &input_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }
    rc = tensor_ptr(
        model, context, op->output_tensor, bytes, &output_bytes_ptr);
    if (rc != AI_DESCRIPTOR_OK) {
        return rc;
    }

    const float *input = (const float *)(const void *)input_bytes_ptr;
    float *output = (float *)(void *)output_bytes_ptr;
    float max_value = input[0];
    for (uint32_t i = 1; i < op->element_count; ++i) {
        if (input[i] > max_value) {
            max_value = input[i];
        }
    }

    float sum = 0.0f;
    for (uint32_t i = 0; i < op->element_count; ++i) {
        const float value = expf(input[i] - max_value);
        output[i] = value;
        sum += value;
    }

    if (sum == 0.0f) {
        return AI_DESCRIPTOR_ERR_INVALID_ARGUMENT;
    }
    const float inv_sum = 1.0f / sum;
    for (uint32_t i = 0; i < op->element_count; ++i) {
        output[i] *= inv_sum;
    }

    return AI_DESCRIPTOR_OK;
}

uint32_t ai_descriptor_required_arena_bytes(const ai_descriptor_model_t *model) {
    return model != NULL ? model->arena_bytes : 0u;
}

int ai_descriptor_invoke(
    const ai_descriptor_model_t *model,
    ai_descriptor_context_t *context) {
    if (model == NULL || context == NULL || model->ops == NULL ||
        model->tensors == NULL || context->arena_bytes < model->arena_bytes) {
        return AI_DESCRIPTOR_ERR_INVALID_ARGUMENT;
    }

    for (uint32_t i = 0; i < model->op_count; ++i) {
        const ai_descriptor_op_t *op = &model->ops[i];
        int rc = AI_DESCRIPTOR_OK;
        switch (op->type) {
        case AI_DESCRIPTOR_OP_FULLY_CONNECTED_F32:
            rc = invoke_fc_f32(model, context, &op->config.fc_f32);
            break;
        case AI_DESCRIPTOR_OP_SOFTMAX_F32:
            rc = invoke_softmax_f32(model, context, &op->config.softmax_f32);
            break;
        default:
            rc = AI_DESCRIPTOR_ERR_UNSUPPORTED_OP;
            break;
        }
        if (rc != AI_DESCRIPTOR_OK) {
            return rc;
        }
    }

    return AI_DESCRIPTOR_OK;
}
