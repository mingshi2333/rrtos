#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "hal_board.h"
#include "hal_clint.h"
#include "hal_uart.h"
#include "iree/hal/local/executable_library.h"
#include "mnist_validation_samples.h"
#include "os_config.h"
#include "os_kernel.h"

extern const iree_hal_executable_library_header_t **
st_mnist_28_linked_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t *environment);

extern const uint8_t *st_mnist_28_direct_const_data(size_t *out_size);

static os_tcb_t probe_task_tcb;

enum {
    MNIST_DIRECT_INPUT_FLOATS = 784,
    MNIST_DIRECT_OUTPUT_FLOATS = 10,
    MNIST_DIRECT_INPUT_BYTES = MNIST_DIRECT_INPUT_FLOATS * 4,
    MNIST_DIRECT_OUTPUT_BYTES = MNIST_DIRECT_OUTPUT_FLOATS * 4,
    MNIST_DIRECT_CONST_BYTES = 407104,
    MNIST_DIRECT_SCRATCH_BYTES = 576,
    MNIST_DIRECT_EXPECTED_LABEL = 7,
};

static uint8_t probe_task_stack_storage[16384]
    __attribute__((aligned(16)));
static float direct_input[MNIST_DIRECT_INPUT_FLOATS]
    __attribute__((aligned(16)));
static uint8_t direct_scratch[MNIST_DIRECT_SCRATCH_BYTES]
    __attribute__((aligned(64)));
static float direct_output[MNIST_DIRECT_OUTPUT_FLOATS]
    __attribute__((aligned(16)));

static const char *probe_str_or_null(const char *value) {
    return value != NULL ? value : "(null)";
}

static uint32_t probe_float_to_bits(const float *value) {
    const uint8_t *bytes = (const uint8_t *)value;

    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static void probe_print_export(const iree_hal_executable_library_v0_t *library,
                               uint32_t ordinal) {
    const char *name = NULL;
    const char *tag = NULL;
    const iree_hal_executable_dispatch_attrs_v0_t *attrs = NULL;

    if (library->exports.names != NULL) {
        name = library->exports.names[ordinal];
    }
    if (library->exports.tags != NULL) {
        tag = library->exports.tags[ordinal];
    }
    if (library->exports.attrs != NULL) {
        attrs = &library->exports.attrs[ordinal];
    }

    os_print("[AI_STATIC_DIRECT_MNIST] export[%u] ptr=0x%x name=%s tag=%s\n",
             ordinal,
             (uint32_t)(uintptr_t)library->exports.ptrs[ordinal],
             probe_str_or_null(name),
             probe_str_or_null(tag));
    if (attrs != NULL) {
        os_print("[AI_STATIC_DIRECT_MNIST] export[%u] attrs constants=%u bindings=%u local_pages=%u\n",
                 ordinal,
                 (uint32_t)attrs->constant_count,
                 (uint32_t)attrs->binding_count,
                 (uint32_t)attrs->local_memory_pages);
    } else {
        os_print("[AI_STATIC_DIRECT_MNIST] export[%u] attrs absent\n", ordinal);
    }
}

static int probe_issue_dispatch(
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

static void probe_fill_mnist_input(void) {
    const mnist_validation_sample_t *sample = &k_mnist_validation_samples[0];

    for (size_t i = 0; i < MNIST_DIRECT_INPUT_FLOATS; ++i) {
        direct_input[i] = (float)sample->pixels[i] / 255.0f;
    }
}

static uint32_t probe_argmax_output(void) {
    uint32_t best_index = 0;
    float best_score = direct_output[0];

    for (uint32_t i = 1; i < MNIST_DIRECT_OUTPUT_FLOATS; ++i) {
        if (direct_output[i] > best_score) {
            best_score = direct_output[i];
            best_index = i;
        }
    }
    return best_index;
}

static int probe_run_direct_mnist(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    uint32_t *out_argmax,
    uint32_t *out_top_bits,
    uint32_t *out_elapsed_ticks) {
    size_t const_size = 0;
    const uint8_t *model_const = st_mnist_28_direct_const_data(&const_size);
    if (model_const == NULL || const_size != MNIST_DIRECT_CONST_BYTES) {
        return -10;
    }
    if (MNIST_VALIDATION_SAMPLE_COUNT == 0u ||
        k_mnist_validation_samples[0].label != MNIST_DIRECT_EXPECTED_LABEL) {
        return -11;
    }

    memset(direct_scratch, 0, sizeof(direct_scratch));
    memset(direct_output, 0, sizeof(direct_output));
    probe_fill_mnist_input();

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
    int rc = probe_issue_dispatch(
        library, environment, 0u, 4u, 1u, 1u,
        NULL, 0u, bindings0, binding_lengths0, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = probe_issue_dispatch(
        library, environment, 1u, 1u, 1u, 1u,
        NULL, 0u, bindings1, binding_lengths1, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = probe_issue_dispatch(
        library, environment, 2u, 1u, 1u, 1u,
        NULL, 0u, bindings2, binding_lengths2, 2u);
    uint64_t t1 = hal_clint_mtime_get();
    if (rc != 0) {
        return rc;
    }

    *out_argmax = probe_argmax_output();
    *out_top_bits = probe_float_to_bits(&direct_output[*out_argmax]);
    *out_elapsed_ticks = (uint32_t)(t1 - t0);
    return 0;
}

static void probe_task_entry(void *arg) {
    (void)arg;

    iree_hal_executable_environment_v0_t environment = {0};
    union {
        const iree_hal_executable_library_header_t **header;
        const iree_hal_executable_library_v0_t *v0;
    } library;

    library.header = st_mnist_28_linked_library_query(
        IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
        &environment);
    if (library.header == NULL || *library.header == NULL) {
        os_print("[AI_STATIC_DIRECT_MNIST] query: FAIL\n");
        while (1) {
            os_task_delay(1000);
        }
    }

    const iree_hal_executable_library_header_t *header = *library.header;
    os_print("[AI_STATIC_DIRECT_MNIST] query: OK\n");
    os_print("[AI_STATIC_DIRECT_MNIST] header version=%u latest=%u name=%s features=0x%x sanitizer=%u\n",
             (uint32_t)header->version,
             (uint32_t)IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
             probe_str_or_null(header->name),
             (uint32_t)header->features,
             (uint32_t)header->sanitizer);
    os_print("[AI_STATIC_DIRECT_MNIST] tables imports=%u exports=%u constants=%u sources=%u\n",
             library.v0->imports.count,
             library.v0->exports.count,
             library.v0->constants.count,
             library.v0->sources.count);

    if (library.v0->exports.ptrs == NULL || library.v0->exports.count != 3u) {
        os_print("[AI_STATIC_DIRECT_MNIST] exports: FAIL count=%u\n",
                 library.v0->exports.count);
    } else {
        for (uint32_t i = 0; i < library.v0->exports.count; ++i) {
            probe_print_export(library.v0, i);
        }
        os_print("[AI_STATIC_DIRECT_MNIST] BE_U1000_STATIC_DIRECT_MNIST_PROBE_PASS\n");

        uint32_t argmax = 0;
        uint32_t top_bits = 0;
        uint32_t elapsed_ticks = 0;
        int infer_rc = probe_run_direct_mnist(
            library.v0,
            &environment,
            &argmax,
            &top_bits,
            &elapsed_ticks);
        os_print("[AI_STATIC_DIRECT_MNIST] direct_infer rc=%d ticks=%u sample=%s expected=%u argmax=%u top_hex=0x%x y0=0x%x y7=0x%x\n",
                 infer_rc,
                 elapsed_ticks,
                 k_mnist_validation_samples[0].id,
                 k_mnist_validation_samples[0].label,
                 argmax,
                 top_bits,
                 probe_float_to_bits(&direct_output[0]),
                 probe_float_to_bits(&direct_output[7]));
        if (infer_rc == 0 &&
            argmax == k_mnist_validation_samples[0].label) {
            os_print("[AI_STATIC_DIRECT_MNIST] BE_U1000_STATIC_DIRECT_MNIST_INFER_PASS\n");
        } else {
            os_print("[AI_STATIC_DIRECT_MNIST] BE_U1000_STATIC_DIRECT_MNIST_INFER_FAIL rc=%d argmax=%u expected=%u\n",
                     infer_rc,
                     argmax,
                     k_mnist_validation_samples[0].label);
        }
    }

    while (1) {
        os_task_delay(1000);
    }
}

void os_kernel_main(void) {
    hal_board_init();
    hal_board_print_banner();
    os_print("[AI_STATIC_DIRECT_MNIST] boot marker=0x%x\n",
             (uint32_t)hal_clint_mtime_get());
    os_print("[AI_STATIC_DIRECT_MNIST] OS_AI_EN=%u direct MNIST static executable query\n",
             (uint32_t)OS_CFG_AI_EN);

    os_kernel_init();
    os_task_create(&probe_task_tcb,
                   "ai_static_mnist",
                   probe_task_entry,
                   NULL,
                   10,
                   probe_task_stack_storage,
                   sizeof(probe_task_stack_storage));
    os_kernel_start();
}
