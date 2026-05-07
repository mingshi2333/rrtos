#ifndef DESCRIPTOR_MNIST_MODEL_H
#define DESCRIPTOR_MNIST_MODEL_H

#include <stddef.h>
#include <stdint.h>

#include "ai_descriptor_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MNIST_DESCRIPTOR_INPUT_FLOATS = 784,
    MNIST_DESCRIPTOR_HIDDEN_FLOATS = 128,
    MNIST_DESCRIPTOR_OUTPUT_FLOATS = 10,
    MNIST_DESCRIPTOR_CONST_BYTES = 407104,
};

const ai_descriptor_model_t *mnist_descriptor_model(void);
const uint8_t *mnist_descriptor_const_data(size_t *out_size);

#ifdef __cplusplus
}
#endif

#endif
