#ifndef AI_MODELS_GEN_H
#define AI_MODELS_GEN_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h> // For abort, abs, etc.
#include <math.h>   // For fabs, etc.

/* --- Wrappers and Descriptors --- */
/* Model: be_u1000_hello_world_float */
typedef struct {
    float tensor_0[1]; // Shape: [1, 1]
} ai_be_u1000_hello_world_float_input_t;

typedef struct {
    float tensor_0[1]; // Shape: [1, 1]
} ai_be_u1000_hello_world_float_output_t;

/**
 * Run inference for be_u1000_hello_world_float
 * Returns 0 on success, non-zero on failure.
 */
int ai_be_u1000_hello_world_float_run(const ai_be_u1000_hello_world_float_input_t *input, ai_be_u1000_hello_world_float_output_t *output);

#endif // AI_MODELS_GEN_H
