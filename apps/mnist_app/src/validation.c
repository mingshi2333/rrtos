#include "validation.h"

#include <math.h>
#include <string.h>

#define MNIST_VALIDATION_SCORE_SCALE 100000.0f
#define MNIST_VALIDATION_FNV_OFFSET 2166136261u
#define MNIST_VALIDATION_FNV_PRIME 16777619u
#define MNIST_VALIDATION_INPUT_COUNT \
    (sizeof(((ai_st_mnist_28_input_t *)0)->tensor_0) / sizeof(((ai_st_mnist_28_input_t *)0)->tensor_0[0]))
#define MNIST_VALIDATION_OUTPUT_COUNT \
    (sizeof(((ai_st_mnist_28_output_t *)0)->tensor_0) / sizeof(((ai_st_mnist_28_output_t *)0)->tensor_0[0]))

const mnist_validation_sample_t *mnist_validation_get_samples(size_t *count) {
    if (count) {
        *count = MNIST_VALIDATION_SAMPLE_COUNT;
    }
    return k_mnist_validation_samples;
}

void mnist_validation_fill_input(ai_st_mnist_28_input_t *input,
                                 const mnist_validation_sample_t *sample) {
    size_t i;

    memset(input, 0, sizeof(*input));
    if (!sample) {
        return;
    }

    for (i = 0; i < MNIST_VALIDATION_INPUT_COUNT && i < sizeof(sample->pixels); ++i) {
        input->tensor_0[i] = (float)sample->pixels[i] / 255.0f;
    }
}

uint32_t mnist_validation_output_argmax(const ai_st_mnist_28_output_t *output) {
    uint32_t best_index = 0;
    uint32_t i;
    float best_score = output->tensor_0[0];

    for (i = 1; i < MNIST_VALIDATION_OUTPUT_COUNT; ++i) {
        if (output->tensor_0[i] > best_score) {
            best_score = output->tensor_0[i];
            best_index = i;
        }
    }

    return best_index;
}

int32_t mnist_validation_top_score_q(const ai_st_mnist_28_output_t *output) {
    return (int32_t)lroundf(
        output->tensor_0[mnist_validation_output_argmax(output)] * MNIST_VALIDATION_SCORE_SCALE);
}

static uint32_t mnist_validation_hash_u32(uint32_t hash, uint32_t value) {
    uint32_t shift;

    for (shift = 0; shift < 32; shift += 8) {
        hash ^= (value >> shift) & 0xFFu;
        hash *= MNIST_VALIDATION_FNV_PRIME;
    }

    return hash;
}

uint32_t mnist_validation_output_hash(const ai_st_mnist_28_output_t *output) {
    uint32_t hash = MNIST_VALIDATION_FNV_OFFSET;
    uint32_t i;

    for (i = 0; i < MNIST_VALIDATION_OUTPUT_COUNT; ++i) {
        int32_t quantized =
            (int32_t)lroundf(output->tensor_0[i] * MNIST_VALIDATION_SCORE_SCALE);
        hash = mnist_validation_hash_u32(hash, (uint32_t)quantized);
    }

    return hash;
}

mnist_validation_status_t mnist_validation_check(
    const mnist_validation_sample_t *sample,
    const ai_st_mnist_28_output_t *output,
    const ai_perf_stats_t *stats,
    mnist_validation_observation_t *observation) {
    if (!sample || !output || !observation) {
        return MNIST_VALIDATION_PERF_STATS_MISSING;
    }

    observation->argmax = mnist_validation_output_argmax(output);
    observation->hash = mnist_validation_output_hash(output);
    observation->top_score_q = mnist_validation_top_score_q(output);
    observation->latency_us = stats ? (uint32_t)stats->latency_last_us : 0u;
    observation->latency_cycles = stats ? stats->latency_last_cycles : 0u;
    observation->latency_instructions = stats ? stats->latency_last_instructions : 0u;
    observation->total_inferences = stats ? (uint32_t)stats->total_inferences : 0u;
    observation->arena_peak = stats ? (uint32_t)stats->arena_peak_bytes : 0u;

    if (observation->argmax != sample->label) {
        return MNIST_VALIDATION_LABEL_MISMATCH;
    }

    if (!stats) {
        return MNIST_VALIDATION_PERF_STATS_MISSING;
    }

    if (stats->total_inferences != 1) {
        return MNIST_VALIDATION_INFERENCE_COUNT_MISMATCH;
    }

    if (stats->latency_last_us == 0 || stats->latency_last_cycles == 0 ||
        stats->latency_last_instructions == 0) {
        return MNIST_VALIDATION_LATENCY_MISSING;
    }

    return MNIST_VALIDATION_OK;
}
