#ifndef AI_MODELS_GEN_H
#define AI_MODELS_GEN_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h> // For abort, abs, etc.
#include <math.h>   // For fabs, etc.

/* --- Wrappers and Descriptors --- */
/* Model: miniresnetv1_s1_64x50_tl_int8 */
typedef struct {
    int8_t tensor_0[3200]; // Shape: [1, 64, 50, 1]
} ai_miniresnetv1_s1_64x50_tl_int8_input_t;

typedef struct {
    float tensor_0[10]; // Shape: [1, 10]
} ai_miniresnetv1_s1_64x50_tl_int8_output_t;

/**
 * Run inference for miniresnetv1_s1_64x50_tl_int8
 * Returns 0 on success, non-zero on failure.
 */
int ai_miniresnetv1_s1_64x50_tl_int8_run(const ai_miniresnetv1_s1_64x50_tl_int8_input_t *input, ai_miniresnetv1_s1_64x50_tl_int8_output_t *output);

#endif // AI_MODELS_GEN_H
