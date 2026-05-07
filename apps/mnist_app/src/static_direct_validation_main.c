#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hal_board.h"
#include "hal_clint.h"
#include "iree/hal/local/executable_library.h"
#include "mnist_validation_samples.h"
#include "os_kernel.h"
#include "os_mem.h"

extern const iree_hal_executable_library_header_t **
st_mnist_28_linked_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t *environment);

extern const uint8_t *st_mnist_28_direct_const_data(size_t *out_size);

enum {
    MNIST_STATIC_DIRECT_INPUT_FLOATS = 784,
    MNIST_STATIC_DIRECT_OUTPUT_FLOATS = 10,
    MNIST_STATIC_DIRECT_CONST_BYTES = 407104,
    MNIST_STATIC_DIRECT_SCRATCH_BYTES = 576,
    MNIST_STATIC_DIRECT_EXPECTED_LABEL = 7,
};

static os_tcb_t validation_tcb;
static uint64_t validation_stack[8192];
static float direct_input[MNIST_STATIC_DIRECT_INPUT_FLOATS] __attribute__((aligned(16)));
static uint8_t direct_scratch[MNIST_STATIC_DIRECT_SCRATCH_BYTES]
    __attribute__((aligned(64)));
static float direct_output[MNIST_STATIC_DIRECT_OUTPUT_FLOATS]
    __attribute__((aligned(16)));

static uint32_t float_to_bits(const float *value) {
    const uint8_t *bytes = (const uint8_t *)value;

    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static void fill_input(const mnist_validation_sample_t *sample) {
    for (size_t i = 0; i < MNIST_STATIC_DIRECT_INPUT_FLOATS; ++i) {
        direct_input[i] = (float)sample->pixels[i] / 255.0f;
    }
}

static uint32_t argmax_output(void) {
    uint32_t best_index = 0;
    float best_score = direct_output[0];

    for (uint32_t i = 1; i < MNIST_STATIC_DIRECT_OUTPUT_FLOATS; ++i) {
        if (direct_output[i] > best_score) {
            best_score = direct_output[i];
            best_index = i;
        }
    }
    return best_index;
}

static int issue_dispatch(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    uint32_t ordinal,
    uint32_t workgroup_count_x,
    uint32_t workgroup_count_y,
    uint32_t workgroup_count_z,
    const uint32_t *constants,
    uint16_t constant_count,
    void *const *binding_ptrs,
    const size_t *binding_lengths,
    uint8_t binding_count) {
    if (ordinal >= library->exports.count ||
        library->exports.ptrs == NULL ||
        library->exports.ptrs[ordinal] == NULL) {
        return -1;
    }
    if (library->exports.attrs != NULL) {
        const iree_hal_executable_dispatch_attrs_v0_t *attrs =
            &library->exports.attrs[ordinal];
        if (attrs->constant_count != constant_count ||
            attrs->binding_count != binding_count ||
            attrs->local_memory_pages != 0u) {
            return -2;
        }
    }

    iree_hal_executable_dispatch_state_v0_t dispatch_state = {
        .workgroup_size_x = 1,
        .workgroup_size_y = 1,
        .workgroup_size_z = 1,
        .constant_count = constant_count,
        .workgroup_count_x = workgroup_count_x,
        .workgroup_count_y = workgroup_count_y,
        .workgroup_count_z = (uint16_t)workgroup_count_z,
        .max_concurrency = 1,
        .binding_count = binding_count,
        .constants = constants,
        .binding_ptrs = binding_ptrs,
        .binding_lengths = binding_lengths,
    };
    iree_hal_executable_workgroup_state_v0_t workgroup_state = {0};

    for (uint32_t z = 0; z < workgroup_count_z; ++z) {
        workgroup_state.workgroup_id_z = (uint16_t)z;
        for (uint32_t y = 0; y < workgroup_count_y; ++y) {
            workgroup_state.workgroup_id_y = y;
            for (uint32_t x = 0; x < workgroup_count_x; ++x) {
                workgroup_state.workgroup_id_x = x;
                int rc = library->exports.ptrs[ordinal](
                    environment,
                    &dispatch_state,
                    &workgroup_state);
                if (rc != 0) {
                    return rc;
                }
            }
        }
    }
    return 0;
}

static int run_direct_mnist(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    const mnist_validation_sample_t *sample,
    uint32_t *out_argmax,
    uint32_t *out_top_bits,
    uint32_t *out_elapsed_ticks) {
    size_t const_size = 0;
    const uint8_t *model_const = st_mnist_28_direct_const_data(&const_size);
    if (model_const == NULL || const_size != MNIST_STATIC_DIRECT_CONST_BYTES) {
        return -10;
    }

    memset(direct_scratch, 0, sizeof(direct_scratch));
    memset(direct_output, 0, sizeof(direct_output));
    fill_input(sample);

    void *const bindings0[3] = {
        direct_input,
        (void *)(uintptr_t)model_const,
        direct_scratch,
    };
    const size_t binding_lengths0[3] = {
        sizeof(direct_input),
        const_size,
        sizeof(direct_scratch),
    };
    void *const bindings1[3] = {
        direct_scratch,
        (void *)(uintptr_t)model_const,
        direct_scratch,
    };
    const size_t binding_lengths1[3] = {
        sizeof(direct_scratch),
        const_size,
        sizeof(direct_scratch),
    };
    void *const bindings2[2] = {
        direct_scratch,
        direct_output,
    };
    const size_t binding_lengths2[2] = {
        sizeof(direct_scratch),
        sizeof(direct_output),
    };

    uint64_t t0 = hal_clint_mtime_get();
    int rc = issue_dispatch(
        library, environment, 0u, 4u, 1u, 1u,
        NULL, 0u, bindings0, binding_lengths0, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = issue_dispatch(
        library, environment, 1u, 1u, 1u, 1u,
        NULL, 0u, bindings1, binding_lengths1, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = issue_dispatch(
        library, environment, 2u, 1u, 1u, 1u,
        NULL, 0u, bindings2, binding_lengths2, 2u);
    uint64_t t1 = hal_clint_mtime_get();
    if (rc != 0) {
        return rc;
    }

    *out_argmax = argmax_output();
    *out_top_bits = float_to_bits(&direct_output[*out_argmax]);
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
        "MNIST_STATIC_DIRECT_MODEL_PEAK: phase=%s heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u\n",
        phase,
        (unsigned)stats.current_used_bytes,
        (unsigned)stats_delta(stats.peak_used_bytes, baseline->current_used_bytes),
        (unsigned)stats_delta(stats.allocation_count, baseline->allocation_count),
        (unsigned)stats_delta(stats.free_count, baseline->free_count));
}

static void validation_task(void *arg) {
    (void)arg;

    if (MNIST_VALIDATION_SAMPLE_COUNT == 0u) {
        printf("MNIST_STATIC_DIRECT_FAIL: no samples\n");
        validation_halt();
    }

    const mnist_validation_sample_t *sample = &k_mnist_validation_samples[0];
    if (sample->label != MNIST_STATIC_DIRECT_EXPECTED_LABEL) {
        printf("MNIST_STATIC_DIRECT_FAIL: label=%u expected=%u\n",
               sample->label,
               MNIST_STATIC_DIRECT_EXPECTED_LABEL);
        validation_halt();
    }

    iree_hal_executable_environment_v0_t environment = {0};
    union {
        const iree_hal_executable_library_header_t **header;
        const iree_hal_executable_library_v0_t *v0;
    } library;

    library.header = st_mnist_28_linked_library_query(
        IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
        &environment);
    if (library.header == NULL || *library.header == NULL) {
        printf("MNIST_STATIC_DIRECT_FAIL: query failed\n");
        validation_halt();
    }
    if (library.v0->exports.ptrs == NULL || library.v0->exports.count != 3u) {
        printf("MNIST_STATIC_DIRECT_FAIL: export_count=%u\n",
               (unsigned)library.v0->exports.count);
        validation_halt();
    }

    printf("MNIST_STATIC_DIRECT_PROBE_PASS exports=%u\n",
           (unsigned)library.v0->exports.count);
    printf(
        "MNIST_STATIC_DIRECT_STATIC_RAM: input_bytes=%u scratch_bytes=%u output_bytes=%u io_scratch_bytes=%u task_stack_bytes=%u\n",
        (unsigned)sizeof(direct_input),
        (unsigned)sizeof(direct_scratch),
        (unsigned)sizeof(direct_output),
        (unsigned)(sizeof(direct_input) + sizeof(direct_scratch) +
                   sizeof(direct_output)),
        (unsigned)sizeof(validation_stack));

    uint32_t argmax = 0;
    uint32_t top_bits = 0;
    uint32_t elapsed_ticks = 0;
    os_heap_stats_t baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    int rc = run_direct_mnist(
        library.v0,
        &environment,
        sample,
        &argmax,
        &top_bits,
        &elapsed_ticks);
    print_heap_peak("invoke", &baseline);
    printf(
        "MNIST_STATIC_DIRECT_RESULT: sample=%s label=%u argmax=%u top_hex=%x latency_ticks=%u\n",
        sample->id,
        sample->label,
        argmax,
        top_bits,
        elapsed_ticks);

    if (rc == 0 && argmax == sample->label) {
        printf("MNIST_STATIC_DIRECT_PASS count=1\n");
    } else {
        printf("MNIST_STATIC_DIRECT_FAIL: rc=%d argmax=%u expected=%u\n",
               rc,
               argmax,
               sample->label);
    }
    validation_halt();
}

void os_kernel_main(void) {
    hal_board_init();
    printf("Booting MNIST static-direct validation...\n");

    os_kernel_init();
    os_task_create(&validation_tcb, "mnist_static_direct", validation_task,
                   NULL, 10, validation_stack, sizeof(validation_stack));
    os_kernel_start();
}
