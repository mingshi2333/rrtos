#include <stddef.h>
#include <stdint.h>

#include "hal_board.h"
#include "hal_clint.h"
#include "hal_uart.h"
#include "iree/hal/local/executable_library.h"
#include "os_config.h"
#include "os_kernel.h"

extern const iree_hal_executable_library_header_t **
be_u1000_hello_world_float_linked_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t *environment);

static os_tcb_t probe_task_tcb;

enum {
    HELLO_WORLD_INPUT_BITS = 0x3f800000u,
    HELLO_WORLD_MIN_OUTPUT_BITS = 0x3f400000u,
    HELLO_WORLD_MAX_OUTPUT_BITS = 0x3f800000u,
    HELLO_WORLD_CONST_BYTES = 1280,
    HELLO_WORLD_TEMP_BYTES = 128,
};

extern const uint8_t *be_u1000_hello_world_float_direct_const_data(
    size_t *out_size);

static uint8_t probe_task_stack_storage[8192]
    __attribute__((aligned(16)));
static uint8_t direct_temp[HELLO_WORLD_TEMP_BYTES]
    __attribute__((aligned(64)));
static float direct_input[1] __attribute__((aligned(16)));
static float direct_output[1] __attribute__((aligned(16)));

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

static void probe_float_from_bits(float *value, uint32_t bits) {
    uint8_t *bytes = (uint8_t *)value;

    bytes[0] = (uint8_t)(bits & 0xffu);
    bytes[1] = (uint8_t)((bits >> 8) & 0xffu);
    bytes[2] = (uint8_t)((bits >> 16) & 0xffu);
    bytes[3] = (uint8_t)((bits >> 24) & 0xffu);
}

static uint32_t probe_float_order_key(uint32_t bits) {
    return (bits & 0x80000000u) ? ~bits : (bits | 0x80000000u);
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

    os_print("[AI_STATIC_DIRECT] export[%u] ptr=0x%x name=%s tag=%s\n",
             ordinal,
             (uint32_t)(uintptr_t)library->exports.ptrs[ordinal],
             probe_str_or_null(name),
             probe_str_or_null(tag));
    if (attrs != NULL) {
        os_print("[AI_STATIC_DIRECT] export[%u] attrs constants=%u bindings=%u local_pages=%u\n",
                 ordinal,
                 (uint32_t)attrs->constant_count,
                 (uint32_t)attrs->binding_count,
                 (uint32_t)attrs->local_memory_pages);
    } else {
        os_print("[AI_STATIC_DIRECT] export[%u] attrs absent\n", ordinal);
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

static int probe_run_direct_inference(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    uint32_t *out_y_bits,
    uint32_t *out_elapsed_ticks) {
    size_t const_size = 0;
    const uint8_t *model_const =
        be_u1000_hello_world_float_direct_const_data(&const_size);
    if (model_const == NULL || const_size != HELLO_WORLD_CONST_BYTES) {
        return -10;
    }

    for (size_t i = 0; i < sizeof(direct_temp); ++i) {
        direct_temp[i] = 0;
    }
    probe_float_from_bits(&direct_input[0], HELLO_WORLD_INPUT_BITS);
    probe_float_from_bits(&direct_output[0], 0u);

    const uint32_t constants0[2] = {1u, 0u};
    const uint32_t constants12[4] = {64u, 0u, 1u, 0u};
    void *const bindings0[3] = {
        direct_input,
        (void *)(uintptr_t)model_const,
        direct_temp,
    };
    const size_t binding_lengths0[3] = {
        sizeof(direct_input),
        const_size,
        sizeof(direct_temp),
    };
    void *const bindings1[3] = {
        direct_temp,
        (void *)(uintptr_t)model_const,
        direct_temp,
    };
    const size_t binding_lengths1[3] = {
        sizeof(direct_temp),
        const_size,
        sizeof(direct_temp),
    };
    void *const bindings2[3] = {
        direct_temp,
        (void *)(uintptr_t)model_const,
        direct_output,
    };
    const size_t binding_lengths2[3] = {
        sizeof(direct_temp),
        const_size,
        sizeof(direct_output),
    };

    uint64_t t0 = hal_clint_mtime_get();
    int rc = probe_issue_dispatch(
        library, environment, 0u, 1u, 1u, 1u,
        constants0, 2u, bindings0, binding_lengths0, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = probe_issue_dispatch(
        library, environment, 1u, 1u, 1u, 1u,
        constants12, 4u, bindings1, binding_lengths1, 3u);
    if (rc != 0) {
        return rc;
    }
    rc = probe_issue_dispatch(
        library, environment, 2u, 1u, 1u, 1u,
        constants12, 4u, bindings2, binding_lengths2, 3u);
    uint64_t t1 = hal_clint_mtime_get();
    if (rc != 0) {
        return rc;
    }

    *out_y_bits = probe_float_to_bits(&direct_output[0]);
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

    library.header = be_u1000_hello_world_float_linked_library_query(
        IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
        &environment);
    if (library.header == NULL || *library.header == NULL) {
        os_print("[AI_STATIC_DIRECT] query: FAIL\n");
        while (1) {
            os_task_delay(1000);
        }
    }

    const iree_hal_executable_library_header_t *header = *library.header;
    os_print("[AI_STATIC_DIRECT] query: OK\n");
    os_print("[AI_STATIC_DIRECT] header version=%u latest=%u name=%s features=0x%x sanitizer=%u\n",
             (uint32_t)header->version,
             (uint32_t)IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
             probe_str_or_null(header->name),
             (uint32_t)header->features,
             (uint32_t)header->sanitizer);
    os_print("[AI_STATIC_DIRECT] tables imports=%u exports=%u constants=%u sources=%u\n",
             library.v0->imports.count,
             library.v0->exports.count,
             library.v0->constants.count,
             library.v0->sources.count);

    if (library.v0->exports.ptrs == NULL || library.v0->exports.count == 0) {
        os_print("[AI_STATIC_DIRECT] exports: FAIL\n");
    } else {
        for (uint32_t i = 0; i < library.v0->exports.count; ++i) {
            probe_print_export(library.v0, i);
        }
        os_print("[AI_STATIC_DIRECT] BE_U1000_STATIC_DIRECT_PROBE_PASS\n");

        uint32_t y_bits = 0;
        uint32_t elapsed_ticks = 0;
        int infer_rc = probe_run_direct_inference(
            library.v0,
            &environment,
            &y_bits,
            &elapsed_ticks);
        uint32_t y_key = probe_float_order_key(y_bits);
        os_print("[AI_STATIC_DIRECT] direct_infer rc=%d ticks=%u x_hex=0x%x y_hex=0x%x expected_y_range=[0x%x,0x%x]\n",
                 infer_rc,
                 elapsed_ticks,
                 HELLO_WORLD_INPUT_BITS,
                 y_bits,
                 HELLO_WORLD_MIN_OUTPUT_BITS,
                 HELLO_WORLD_MAX_OUTPUT_BITS);
        if (infer_rc == 0 &&
            y_key >= probe_float_order_key(HELLO_WORLD_MIN_OUTPUT_BITS) &&
            y_key <= probe_float_order_key(HELLO_WORLD_MAX_OUTPUT_BITS)) {
            os_print("[AI_STATIC_DIRECT] BE_U1000_STATIC_DIRECT_INFER_PASS\n");
        } else {
            os_print("[AI_STATIC_DIRECT] BE_U1000_STATIC_DIRECT_INFER_FAIL rc=%d y_hex=0x%x\n",
                     infer_rc,
                     y_bits);
        }
    }

    while (1) {
        os_task_delay(1000);
    }
}

void os_kernel_main(void) {
    hal_board_init();
    hal_board_print_banner();
    os_print("[AI_STATIC_DIRECT] boot marker=0x%x\n",
             (uint32_t)hal_clint_mtime_get());
    os_print("[AI_STATIC_DIRECT] OS_AI_EN=%u direct static executable query\n",
             (uint32_t)OS_CFG_AI_EN);

    os_kernel_init();
    os_task_create(&probe_task_tcb,
                   "ai_static_direct",
                   probe_task_entry,
                   NULL,
                   10,
                   probe_task_stack_storage,
                   sizeof(probe_task_stack_storage));
    os_kernel_start();
}
