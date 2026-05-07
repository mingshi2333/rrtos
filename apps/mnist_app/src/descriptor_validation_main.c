#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "descriptor_mnist_model.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "mnist_validation_samples.h"
#include "os_kernel.h"
#include "os_mem.h"

enum {
    MNIST_DESCRIPTOR_EXPECTED_LABEL = 7,
};

static os_tcb_t validation_tcb;
static uint64_t validation_stack[8192];
static float descriptor_input[MNIST_DESCRIPTOR_INPUT_FLOATS]
    __attribute__((aligned(16)));
static uint8_t descriptor_arena[MNIST_DESCRIPTOR_HIDDEN_FLOATS * sizeof(float)]
    __attribute__((aligned(16)));
static float descriptor_output[MNIST_DESCRIPTOR_OUTPUT_FLOATS]
    __attribute__((aligned(16)));

static uint32_t float_to_bits(const float *value) {
    const uint8_t *bytes = (const uint8_t *)value;

    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static void fill_input(const mnist_validation_sample_t *sample) {
    for (size_t i = 0; i < MNIST_DESCRIPTOR_INPUT_FLOATS; ++i) {
        descriptor_input[i] = (float)sample->pixels[i] / 255.0f;
    }
}

static uint32_t argmax_output(void) {
    uint32_t best_index = 0;
    float best_score = descriptor_output[0];

    for (uint32_t i = 1; i < MNIST_DESCRIPTOR_OUTPUT_FLOATS; ++i) {
        if (descriptor_output[i] > best_score) {
            best_score = descriptor_output[i];
            best_index = i;
        }
    }
    return best_index;
}

static int run_descriptor_mnist(
    const mnist_validation_sample_t *sample,
    uint32_t *out_argmax,
    uint32_t *out_top_bits,
    uint32_t *out_elapsed_ticks) {
    const ai_descriptor_model_t *model = mnist_descriptor_model();
    size_t const_size = 0;
    const uint8_t *model_const = mnist_descriptor_const_data(&const_size);
    if (model == NULL || model_const == NULL ||
        const_size != MNIST_DESCRIPTOR_CONST_BYTES) {
        return -10;
    }

    memset(descriptor_arena, 0, sizeof(descriptor_arena));
    memset(descriptor_output, 0, sizeof(descriptor_output));
    fill_input(sample);

    ai_descriptor_context_t context = {
        .const_data = model_const,
        .const_data_bytes = (uint32_t)const_size,
        .input = (const uint8_t *)(const void *)descriptor_input,
        .input_bytes = sizeof(descriptor_input),
        .output = (uint8_t *)(void *)descriptor_output,
        .output_bytes = sizeof(descriptor_output),
        .arena = descriptor_arena,
        .arena_bytes = sizeof(descriptor_arena),
    };

    uint64_t t0 = hal_clint_mtime_get();
    int rc = ai_descriptor_invoke(model, &context);
    uint64_t t1 = hal_clint_mtime_get();
    if (rc != 0) {
        return rc;
    }

    *out_argmax = argmax_output();
    *out_top_bits = float_to_bits(&descriptor_output[*out_argmax]);
    *out_elapsed_ticks = (uint32_t)(t1 - t0);
    return 0;
}

static void validation_halt(void) {
    while (1) {
        os_task_delay(1000);
    }
}

static os_size_t stats_delta(os_size_t value, os_size_t baseline) {
    return value >= baseline ? value - baseline : 0u;
}

static void print_heap_peak(const char *phase, const os_heap_stats_t *baseline) {
    os_heap_stats_t stats = os_heap_stats_get();

    printf(
        "MNIST_DESCRIPTOR_MODEL_PEAK: phase=%s heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u\n",
        phase,
        (unsigned)stats.current_used_bytes,
        (unsigned)stats_delta(stats.peak_used_bytes, baseline->current_used_bytes),
        (unsigned)stats_delta(stats.allocation_count, baseline->allocation_count),
        (unsigned)stats_delta(stats.free_count, baseline->free_count));
}

static void validation_task(void *arg) {
    (void)arg;

    if (MNIST_VALIDATION_SAMPLE_COUNT == 0u) {
        printf("MNIST_DESCRIPTOR_FAIL: no samples\n");
        validation_halt();
    }

    const mnist_validation_sample_t *sample = &k_mnist_validation_samples[0];
    if (sample->label != MNIST_DESCRIPTOR_EXPECTED_LABEL) {
        printf("MNIST_DESCRIPTOR_FAIL: label=%u expected=%u\n",
               sample->label,
               MNIST_DESCRIPTOR_EXPECTED_LABEL);
        validation_halt();
    }

    const ai_descriptor_model_t *model = mnist_descriptor_model();
    if (model == NULL) {
        printf("MNIST_DESCRIPTOR_FAIL: missing model\n");
        validation_halt();
    }

    printf("MNIST_DESCRIPTOR_PROBE_PASS ops=%u tensors=%u\n",
           (unsigned)model->op_count,
           (unsigned)model->tensor_count);
    printf(
        "MNIST_DESCRIPTOR_STATIC_RAM: input_bytes=%u arena_bytes=%u output_bytes=%u io_scratch_bytes=%u task_stack_bytes=%u\n",
        (unsigned)sizeof(descriptor_input),
        (unsigned)sizeof(descriptor_arena),
        (unsigned)sizeof(descriptor_output),
        (unsigned)(sizeof(descriptor_input) + sizeof(descriptor_arena) +
                   sizeof(descriptor_output)),
        (unsigned)sizeof(validation_stack));

    uint32_t argmax = 0;
    uint32_t top_bits = 0;
    uint32_t elapsed_ticks = 0;
    os_heap_stats_t baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    int rc = run_descriptor_mnist(sample, &argmax, &top_bits, &elapsed_ticks);
    print_heap_peak("invoke", &baseline);
    printf(
        "MNIST_DESCRIPTOR_RESULT: sample=%s label=%u argmax=%u top_hex=%x latency_ticks=%u\n",
        sample->id,
        sample->label,
        argmax,
        top_bits,
        elapsed_ticks);

    if (rc == 0 && argmax == sample->label) {
        printf("MNIST_DESCRIPTOR_PASS count=1\n");
    } else {
        printf("MNIST_DESCRIPTOR_FAIL: rc=%d argmax=%u expected=%u\n",
               rc,
               argmax,
               sample->label);
    }
    validation_halt();
}

void os_kernel_main(void) {
    hal_board_init();
    printf("Booting MNIST descriptor validation...\n");

    os_kernel_init();
    os_task_create(&validation_tcb, "mnist_descriptor", validation_task,
                   NULL, 10, validation_stack, sizeof(validation_stack));
    os_kernel_start();
}
