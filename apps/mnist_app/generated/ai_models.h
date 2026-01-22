#ifndef AI_MODELS_GEN_H
#define AI_MODELS_GEN_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h> // For abort, abs, etc.
#include <math.h>   // For fabs, etc.

/* --- Wrappers and Descriptors --- */
/* Model: st_mnist_28 */
typedef struct {
    uint8_t tensor_0[784]; // Shape: [1, 28, 28, 1]
} ai_st_mnist_28_input_t;

typedef struct {
    float tensor_0[36]; // Shape: [1, 36]
} ai_st_mnist_28_output_t;

/**
 * Run inference for st_mnist_28
 * Returns 0 on success, non-zero on failure.
 */
int ai_st_mnist_28_run(const ai_st_mnist_28_input_t *input, ai_st_mnist_28_output_t *output);

#endif // AI_MODELS_GEN_H
