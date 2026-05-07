#include "descriptor_mnist_model.h"

extern const uint8_t *st_mnist_28_direct_const_data(size_t *out_size);

enum {
    MNIST_DESCRIPTOR_TENSOR_INPUT = 0,
    MNIST_DESCRIPTOR_TENSOR_HIDDEN = 1,
    MNIST_DESCRIPTOR_TENSOR_OUTPUT = 2,
    MNIST_DESCRIPTOR_TENSOR_FC1_WEIGHTS = 3,
    MNIST_DESCRIPTOR_TENSOR_FC1_BIAS = 4,
    MNIST_DESCRIPTOR_TENSOR_FC2_WEIGHTS = 5,
    MNIST_DESCRIPTOR_TENSOR_FC2_BIAS = 6,

    MNIST_DESCRIPTOR_FC2_BIAS_OFFSET = 0,
    MNIST_DESCRIPTOR_FC2_WEIGHT_OFFSET = 64,
    MNIST_DESCRIPTOR_FC1_BIAS_OFFSET = 5184,
    MNIST_DESCRIPTOR_FC1_WEIGHT_OFFSET = 5696,
};

static const ai_descriptor_tensor_t k_mnist_tensors[] = {
    {
        .region = AI_DESCRIPTOR_REGION_INPUT,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = 0,
        .length_bytes = MNIST_DESCRIPTOR_INPUT_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_ARENA,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = 0,
        .length_bytes = MNIST_DESCRIPTOR_HIDDEN_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_OUTPUT,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = 0,
        .length_bytes = MNIST_DESCRIPTOR_OUTPUT_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_CONST,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = MNIST_DESCRIPTOR_FC1_WEIGHT_OFFSET,
        .length_bytes = MNIST_DESCRIPTOR_INPUT_FLOATS *
                        MNIST_DESCRIPTOR_HIDDEN_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_CONST,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = MNIST_DESCRIPTOR_FC1_BIAS_OFFSET,
        .length_bytes = MNIST_DESCRIPTOR_HIDDEN_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_CONST,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = MNIST_DESCRIPTOR_FC2_WEIGHT_OFFSET,
        .length_bytes = MNIST_DESCRIPTOR_HIDDEN_FLOATS *
                        MNIST_DESCRIPTOR_OUTPUT_FLOATS * sizeof(float),
    },
    {
        .region = AI_DESCRIPTOR_REGION_CONST,
        .element_type = AI_DESCRIPTOR_ELEMENT_F32,
        .offset_bytes = MNIST_DESCRIPTOR_FC2_BIAS_OFFSET,
        .length_bytes = MNIST_DESCRIPTOR_OUTPUT_FLOATS * sizeof(float),
    },
};

static const ai_descriptor_op_t k_mnist_ops[] = {
    {
        .type = AI_DESCRIPTOR_OP_FULLY_CONNECTED_F32,
        .config.fc_f32 = {
            .input_tensor = MNIST_DESCRIPTOR_TENSOR_INPUT,
            .weights_tensor = MNIST_DESCRIPTOR_TENSOR_FC1_WEIGHTS,
            .bias_tensor = MNIST_DESCRIPTOR_TENSOR_FC1_BIAS,
            .output_tensor = MNIST_DESCRIPTOR_TENSOR_HIDDEN,
            .input_count = MNIST_DESCRIPTOR_INPUT_FLOATS,
            .output_count = MNIST_DESCRIPTOR_HIDDEN_FLOATS,
            .activation = AI_DESCRIPTOR_ACTIVATION_RELU,
        },
    },
    {
        .type = AI_DESCRIPTOR_OP_FULLY_CONNECTED_F32,
        .config.fc_f32 = {
            .input_tensor = MNIST_DESCRIPTOR_TENSOR_HIDDEN,
            .weights_tensor = MNIST_DESCRIPTOR_TENSOR_FC2_WEIGHTS,
            .bias_tensor = MNIST_DESCRIPTOR_TENSOR_FC2_BIAS,
            .output_tensor = MNIST_DESCRIPTOR_TENSOR_OUTPUT,
            .input_count = MNIST_DESCRIPTOR_HIDDEN_FLOATS,
            .output_count = MNIST_DESCRIPTOR_OUTPUT_FLOATS,
            .activation = AI_DESCRIPTOR_ACTIVATION_NONE,
        },
    },
    {
        .type = AI_DESCRIPTOR_OP_SOFTMAX_F32,
        .config.softmax_f32 = {
            .input_tensor = MNIST_DESCRIPTOR_TENSOR_OUTPUT,
            .output_tensor = MNIST_DESCRIPTOR_TENSOR_OUTPUT,
            .element_count = MNIST_DESCRIPTOR_OUTPUT_FLOATS,
        },
    },
};

static const ai_descriptor_model_t k_mnist_descriptor_model = {
    .tensors = k_mnist_tensors,
    .tensor_count = sizeof(k_mnist_tensors) / sizeof(k_mnist_tensors[0]),
    .ops = k_mnist_ops,
    .op_count = sizeof(k_mnist_ops) / sizeof(k_mnist_ops[0]),
    .arena_bytes = MNIST_DESCRIPTOR_HIDDEN_FLOATS * sizeof(float),
};

const ai_descriptor_model_t *mnist_descriptor_model(void) {
    return &k_mnist_descriptor_model;
}

const uint8_t *mnist_descriptor_const_data(size_t *out_size) {
    return st_mnist_28_direct_const_data(out_size);
}
