#ifndef MNIST_VALIDATION_H
#define MNIST_VALIDATION_H

#include <stddef.h>
#include <stdint.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "mnist_validation_samples.h"

typedef enum {
    MNIST_VALIDATION_OK = 0,
    MNIST_VALIDATION_LABEL_MISMATCH = 1,
    MNIST_VALIDATION_PERF_STATS_MISSING = 2,
    MNIST_VALIDATION_INFERENCE_COUNT_MISMATCH = 3,
    MNIST_VALIDATION_LATENCY_MISSING = 4,
} mnist_validation_status_t;

typedef struct {
    uint32_t argmax;
    uint32_t hash;
    int32_t top_score_q;
    uint32_t latency_us;
    uint64_t latency_cycles;
    uint64_t latency_instructions;
    uint32_t total_inferences;
    uint32_t arena_peak;
} mnist_validation_observation_t;

const mnist_validation_sample_t *mnist_validation_get_samples(size_t *count);
void mnist_validation_fill_input(ai_st_mnist_28_input_t *input,
                                 const mnist_validation_sample_t *sample);
uint32_t mnist_validation_output_argmax(const ai_st_mnist_28_output_t *output);
int32_t mnist_validation_top_score_q(const ai_st_mnist_28_output_t *output);
uint32_t mnist_validation_output_hash(const ai_st_mnist_28_output_t *output);
mnist_validation_status_t mnist_validation_check(
    const mnist_validation_sample_t *sample,
    const ai_st_mnist_28_output_t *output,
    const ai_perf_stats_t *stats,
    mnist_validation_observation_t *observation);

#endif
