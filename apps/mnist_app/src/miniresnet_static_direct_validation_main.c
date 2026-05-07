#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ai_static_direct.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "miniresnet_static_direct_desc.h"
#include "os_kernel.h"
#include "os_mem.h"

extern const iree_hal_executable_library_header_t **
miniresnetv1_s1_64x50_tl_int8_linked_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t *environment);

extern const uint8_t *miniresnet_static_direct_const_data(size_t *out_size);

enum {
    MINIRESNET_STATIC_DIRECT_CLASS_COUNT = 10,
    MINIRESNET_STATIC_DIRECT_FINAL_OUTPUT_BYTES = 40,
    MINIRESNET_STATIC_DIRECT_SCORE_SCALE = 100000,
    MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES = 64,
    MINIRESNET_STATIC_DIRECT_ARENA_GUARD_PATTERN = 0xA5,
    MINIRESNET_STATIC_DIRECT_RUN_COUNT = 3,
};

#define MINIRESNET_STATIC_DIRECT_EXPECTED_HASH UINT32_C(3045847227)
#define MINIRESNET_STATIC_DIRECT_EXPECTED_ARGMAX UINT32_C(3)
#define MINIRESNET_STATIC_DIRECT_EXPECTED_TOP_SCORE_Q INT32_C(99609)

static os_tcb_t validation_tcb;
static uint64_t validation_stack[8192];
static int8_t direct_input[MINIRESNET_STATIC_DIRECT_INPUT_BYTES]
    __attribute__((aligned(16)));
static uint8_t direct_arena_storage[MINIRESNET_STATIC_DIRECT_ARENA_BYTES +
                                    (2u * MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES)]
    __attribute__((aligned(64)));
static float direct_output[MINIRESNET_STATIC_DIRECT_CLASS_COUNT]
    __attribute__((aligned(16)));

static void validation_halt(void) {
    while (1) {
        os_task_delay(1000);
    }
}

static os_size_t stats_delta(os_size_t value, os_size_t baseline) {
    return value >= baseline ? value - baseline : 0u;
}

static uint8_t *direct_arena_data(void) {
    return direct_arena_storage + MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES;
}

static void reset_direct_arena_guards(void) {
    memset(direct_arena_storage,
           MINIRESNET_STATIC_DIRECT_ARENA_GUARD_PATTERN,
           MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES);
    memset(direct_arena_data() + MINIRESNET_STATIC_DIRECT_ARENA_BYTES,
           MINIRESNET_STATIC_DIRECT_ARENA_GUARD_PATTERN,
           MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES);
}

static int guard_range_is_intact(const uint8_t *range, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (range[i] != MINIRESNET_STATIC_DIRECT_ARENA_GUARD_PATTERN) {
            return 0;
        }
    }
    return 1;
}

static int direct_arena_guards_ok(void) {
    return guard_range_is_intact(direct_arena_storage,
                                 MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES) &&
           guard_range_is_intact(direct_arena_data() +
                                     MINIRESNET_STATIC_DIRECT_ARENA_BYTES,
                                 MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES);
}

static void print_heap_peak(const char *phase, const os_heap_stats_t *baseline) {
    os_heap_stats_t stats = os_heap_stats_get();

    printf(
        "MINIRESNET_STATIC_DIRECT_MODEL_PEAK: phase=%s heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u\n",
        phase,
        (unsigned)stats.current_used_bytes,
        (unsigned)stats_delta(stats.peak_used_bytes, baseline->current_used_bytes),
        (unsigned)stats_delta(stats.allocation_count, baseline->allocation_count),
        (unsigned)stats_delta(stats.free_count, baseline->free_count));
}

static void fill_input(void) {
    for (size_t i = 0; i < sizeof(direct_input) / sizeof(direct_input[0]); ++i) {
        direct_input[i] =
            (int8_t)((int32_t)(((i * 37u) + 11u) & 0xFFu) - 128);
    }
}

static uint32_t output_argmax(void) {
    uint32_t best = 0;

    for (uint32_t i = 1; i < MINIRESNET_STATIC_DIRECT_CLASS_COUNT; ++i) {
        if (direct_output[i] > direct_output[best]) {
            best = i;
        }
    }
    return best;
}

static int32_t top_score_q(uint32_t argmax) {
    return (int32_t)lroundf(
        direct_output[argmax] * (float)MINIRESNET_STATIC_DIRECT_SCORE_SCALE);
}

static uint32_t hash_u32(uint32_t hash, uint32_t value) {
    hash ^= value;
    hash *= 16777619u;
    return hash;
}

static uint32_t output_hash(void) {
    uint32_t hash = 2166136261u;

    for (uint32_t i = 0; i < MINIRESNET_STATIC_DIRECT_CLASS_COUNT; ++i) {
        int32_t quantized = (int32_t)lroundf(
            direct_output[i] * (float)MINIRESNET_STATIC_DIRECT_SCORE_SCALE);
        hash = hash_u32(hash, (uint32_t)quantized);
    }
    return hash;
}

static void print_output_q(uint32_t run) {
    printf("MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=%u values_q=[",
           (unsigned)run);
    for (uint32_t i = 0; i < MINIRESNET_STATIC_DIRECT_CLASS_COUNT; ++i) {
        int32_t quantized = (int32_t)lroundf(
            direct_output[i] * (float)MINIRESNET_STATIC_DIRECT_SCORE_SCALE);
        printf("%s%d", i == 0u ? "" : ",", quantized);
    }
    printf("]\n");
}

static int output_matches_expected(uint32_t argmax, uint32_t hash, int32_t score_q) {
    return argmax == MINIRESNET_STATIC_DIRECT_EXPECTED_ARGMAX &&
           hash == MINIRESNET_STATIC_DIRECT_EXPECTED_HASH &&
           score_q == MINIRESNET_STATIC_DIRECT_EXPECTED_TOP_SCORE_Q;
}

static int copy_final_output(void) {
    const ai_static_direct_descriptor_t *descriptor =
        &miniresnet_static_direct_descriptor;
    uint8_t *arena = direct_arena_data();
    if (descriptor->dispatch_count == 0u) {
        return AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR;
    }

    const ai_static_direct_dispatch_t *last =
        &descriptor->dispatches[descriptor->dispatch_count - 1u];
    if (last->binding_count == 0u) {
        return AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR;
    }

    const ai_static_direct_slice_t *output_slice =
        &last->bindings[last->binding_count - 1u];
    if (output_slice->base != AI_STATIC_DIRECT_BUFFER_ARENA ||
        output_slice->length != sizeof(direct_output) ||
        output_slice->offset > MINIRESNET_STATIC_DIRECT_ARENA_BYTES ||
        output_slice->length >
            MINIRESNET_STATIC_DIRECT_ARENA_BYTES - output_slice->offset) {
        return AI_STATIC_DIRECT_ERR_INVALID_SLICE;
    }

    memcpy(direct_output, arena + output_slice->offset, sizeof(direct_output));
    return AI_STATIC_DIRECT_OK;
}

static int run_direct_miniresnet(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    uint32_t *out_elapsed_ticks,
    int *out_guard_before,
    int *out_guard_after) {
    size_t const_size = 0;
    const uint8_t *model_const = miniresnet_static_direct_const_data(&const_size);
    uint8_t *arena = direct_arena_data();
    *out_guard_before = 0;
    *out_guard_after = 0;
    if (model_const == NULL ||
        const_size != MINIRESNET_STATIC_DIRECT_CONST_BYTES) {
        return -10;
    }

    reset_direct_arena_guards();
    *out_guard_before = direct_arena_guards_ok();
    if (!*out_guard_before) {
        return -20;
    }

    memset(arena, 0, MINIRESNET_STATIC_DIRECT_ARENA_BYTES);
    memset(direct_output, 0, sizeof(direct_output));
    fill_input();

    ai_static_direct_context_t context = {
        .input = (const uint8_t *)direct_input,
        .input_bytes = sizeof(direct_input),
        .output = NULL,
        .output_bytes = 0u,
        .const_data = model_const,
        .const_bytes = (uint32_t)const_size,
        .arena = arena,
        .arena_bytes = MINIRESNET_STATIC_DIRECT_ARENA_BYTES,
    };

    uint64_t t0 = hal_clint_mtime_get();
    int rc = ai_static_direct_invoke(
        &miniresnet_static_direct_descriptor,
        library,
        environment,
        &context);
    uint64_t t1 = hal_clint_mtime_get();
    if (rc != AI_STATIC_DIRECT_OK) {
        return rc;
    }

    rc = copy_final_output();
    if (rc != AI_STATIC_DIRECT_OK) {
        return rc;
    }

    *out_guard_after = direct_arena_guards_ok();
    if (!*out_guard_after) {
        return -21;
    }

    *out_elapsed_ticks = (uint32_t)(t1 - t0);
    return AI_STATIC_DIRECT_OK;
}

static void validation_task(void *arg) {
    (void)arg;

    iree_hal_executable_environment_v0_t environment = {0};
    union {
        const iree_hal_executable_library_header_t **header;
        const iree_hal_executable_library_v0_t *v0;
    } library;

    library.header = miniresnetv1_s1_64x50_tl_int8_linked_library_query(
        IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST,
        &environment);
    if (library.header == NULL || *library.header == NULL) {
        printf("MINIRESNET_STATIC_DIRECT_FAIL: query failed\n");
        validation_halt();
    }
    if (library.v0->exports.ptrs == NULL || library.v0->exports.count < 23u) {
        printf("MINIRESNET_STATIC_DIRECT_FAIL: export_count=%u\n",
               (unsigned)library.v0->exports.count);
        validation_halt();
    }

    printf("MINIRESNET_STATIC_DIRECT_PROBE_PASS exports=%u dispatches=%u\n",
           (unsigned)library.v0->exports.count,
           (unsigned)miniresnet_static_direct_descriptor.dispatch_count);
    printf(
        "MINIRESNET_STATIC_DIRECT_STATIC_RAM: input_bytes=%u arena_bytes=%u arena_guard_bytes=%u output_bytes=%u io_scratch_bytes=%u task_stack_bytes=%u\n",
        (unsigned)sizeof(direct_input),
        (unsigned)MINIRESNET_STATIC_DIRECT_ARENA_BYTES,
        (unsigned)(2u * MINIRESNET_STATIC_DIRECT_ARENA_GUARD_BYTES),
        (unsigned)sizeof(direct_output),
        (unsigned)(sizeof(direct_input) + sizeof(direct_arena_storage) +
                   sizeof(direct_output)),
        (unsigned)sizeof(validation_stack));

    uint32_t pass_count = 0;
    os_heap_stats_t baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();

    for (uint32_t run = 0; run < MINIRESNET_STATIC_DIRECT_RUN_COUNT; ++run) {
        uint32_t elapsed_ticks = 0;
        int guard_before = 0;
        int guard_after = 0;
        int rc = run_direct_miniresnet(library.v0,
                                       &environment,
                                       &elapsed_ticks,
                                       &guard_before,
                                       &guard_after);
        uint32_t argmax = output_argmax();
        uint32_t hash = output_hash();
        int32_t score_q = top_score_q(argmax);

        printf("MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=%u before=%s after=%s\n",
               (unsigned)run,
               guard_before ? "ok" : "fail",
               guard_after ? "ok" : "fail");
        printf(
            "MINIRESNET_STATIC_DIRECT_RESULT: run=%u sample=synthetic_pattern argmax=%u hash=%u top_score_q=%d latency_ticks=%u\n",
            (unsigned)run,
            argmax,
            hash,
            score_q,
            elapsed_ticks);
        print_output_q(run);

        if (rc == AI_STATIC_DIRECT_OK && guard_before && guard_after &&
            output_matches_expected(argmax, hash, score_q)) {
            ++pass_count;
        } else {
            printf("MINIRESNET_STATIC_DIRECT_FAIL: run=%u rc=%d argmax=%u hash=%u top_score_q=%d\n",
                   (unsigned)run,
                   rc,
                   argmax,
                   hash,
                   score_q);
        }
    }

    print_heap_peak("invoke", &baseline);

    if (pass_count == MINIRESNET_STATIC_DIRECT_RUN_COUNT) {
        printf("MINIRESNET_STATIC_DIRECT_PASS count=%u\n", (unsigned)pass_count);
    } else {
        printf("MINIRESNET_STATIC_DIRECT_FAIL: passed=%u expected=%u\n",
               (unsigned)pass_count,
               (unsigned)MINIRESNET_STATIC_DIRECT_RUN_COUNT);
    }
    validation_halt();
}

void os_kernel_main(void) {
    hal_board_init();
    printf("Booting MiniResNet static-direct validation...\n");

    os_kernel_init();
    os_task_create(&validation_tcb,
                   "miniresnet_static",
                   validation_task,
                   NULL,
                   10,
                   validation_stack,
                   sizeof(validation_stack));
    os_kernel_start();
}
