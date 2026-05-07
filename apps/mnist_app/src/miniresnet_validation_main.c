#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "os_kernel.h"
#include "os_mem.h"

static os_tcb_t miniresnet_validation_tcb;
static uint64_t miniresnet_validation_stack[8192];

enum {
    AI_TENSOR_SHAPE_CAPACITY = 4,
    MINIRESNET_INPUT_H = 64,
    MINIRESNET_INPUT_W = 50,
    MINIRESNET_INPUT_C = 1,
    MINIRESNET_CLASS_COUNT = 10,
    MINIRESNET_SCORE_SCALE = 100000,
};

_Static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_t *)0)->shape) / sizeof(((ai_tensor_t *)0)->shape[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_t.shape capacity");

static const char *u64_to_dec(uint64_t value, char *buffer, size_t buffer_size) {
    char reversed[21];
    size_t length = 0;
    size_t i;

    if (!buffer || buffer_size == 0) {
        return "";
    }

    if (value == 0) {
        if (buffer_size > 1) {
            buffer[0] = '0';
            buffer[1] = '\0';
        } else {
            buffer[0] = '\0';
        }
        return buffer;
    }

    while (value > 0 && length < sizeof(reversed)) {
        reversed[length++] = (char)('0' + (value % 10u));
        value /= 10u;
    }

    if (length >= buffer_size) {
        length = buffer_size - 1u;
    }

    for (i = 0; i < length; ++i) {
        buffer[i] = reversed[length - 1u - i];
    }
    buffer[length] = '\0';
    return buffer;
}

static void miniresnet_halt(void) {
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
        "MINIRESNET_MODEL_PEAK: phase=%s heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u\n",
        phase,
        (unsigned)stats.current_used_bytes,
        (unsigned)stats_delta(stats.peak_used_bytes, baseline->current_used_bytes),
        (unsigned)stats_delta(stats.allocation_count, baseline->allocation_count),
        (unsigned)stats_delta(stats.free_count, baseline->free_count));
}

static void print_heap_trace(const char *phase) {
    os_heap_trace_t trace = os_heap_trace_get();
    const os_heap_trace_record_t *record;
    char ptr_text[21];
    char caller_text[21];
    size_t i;

    record = &trace.peak_event;
    printf(
        "MINIRESNET_HEAP_TRACE_PEAK: phase=%s seq=%u size=%u current_used_bytes=%u ptr=%s caller=%s\n",
        phase,
        (unsigned)record->sequence,
        (unsigned)record->size,
        (unsigned)record->current_used_bytes,
        u64_to_dec((uint64_t)record->ptr, ptr_text, sizeof(ptr_text)),
        u64_to_dec((uint64_t)record->caller, caller_text, sizeof(caller_text)));

    record = &trace.largest_live_block;
    printf(
        "MINIRESNET_HEAP_TRACE_LARGEST_LIVE: phase=%s seq=%u size=%u current_used_bytes=%u ptr=%s caller=%s\n",
        phase,
        (unsigned)record->sequence,
        (unsigned)record->size,
        (unsigned)record->current_used_bytes,
        u64_to_dec((uint64_t)record->ptr, ptr_text, sizeof(ptr_text)),
        u64_to_dec((uint64_t)record->caller, caller_text, sizeof(caller_text)));

    for (i = 0; i < OS_HEAP_TRACE_TOP_CAPACITY; ++i) {
        record = &trace.largest_allocations[i];
        if (record->size == 0) {
            break;
        }
        printf(
            "MINIRESNET_HEAP_TRACE_TOP: phase=%s rank=%u seq=%u size=%u current_used_bytes=%u ptr=%s caller=%s\n",
            phase,
            (unsigned)(i + 1u),
            (unsigned)record->sequence,
            (unsigned)record->size,
            (unsigned)record->current_used_bytes,
            u64_to_dec((uint64_t)record->ptr, ptr_text, sizeof(ptr_text)),
            u64_to_dec((uint64_t)record->caller, caller_text, sizeof(caller_text)));
    }
}

static void print_hal_allocation_trace(const char *phase) {
    ai_hal_allocation_trace_t trace = ai_hal_allocation_trace_get();
    const ai_hal_allocation_trace_record_t *record;
    char size_text[21];
    char ptr_text[21];
    size_t i;

    record = &trace.peak_event;
    printf(
        "MINIRESNET_HAL_TRACE_PEAK: phase=%s seq=%u allocation_size=%s requested_type=%u requested_usage=%u requested_access=%u compat_type=%u compat_usage=%u compat_access=%u alignment=%u buffer=%s\n",
        phase,
        (unsigned)record->sequence,
        u64_to_dec(record->allocation_size, size_text, sizeof(size_text)),
        (unsigned)record->requested_memory_type,
        (unsigned)record->requested_usage,
        (unsigned)record->requested_access,
        (unsigned)record->compat_memory_type,
        (unsigned)record->compat_usage,
        (unsigned)record->compat_access,
        (unsigned)record->min_alignment,
        u64_to_dec((uint64_t)record->buffer_ptr, ptr_text, sizeof(ptr_text)));

    for (i = 0; i < AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY; ++i) {
        record = &trace.largest_allocations[i];
        if (record->allocation_size == 0) {
            break;
        }
        printf(
            "MINIRESNET_HAL_TRACE_TOP: phase=%s rank=%u seq=%u allocation_size=%s requested_type=%u requested_usage=%u requested_access=%u compat_type=%u compat_usage=%u compat_access=%u alignment=%u buffer=%s\n",
            phase,
            (unsigned)(i + 1u),
            (unsigned)record->sequence,
            u64_to_dec(record->allocation_size, size_text, sizeof(size_text)),
            (unsigned)record->requested_memory_type,
            (unsigned)record->requested_usage,
            (unsigned)record->requested_access,
            (unsigned)record->compat_memory_type,
            (unsigned)record->compat_usage,
            (unsigned)record->compat_access,
            (unsigned)record->min_alignment,
            u64_to_dec((uint64_t)record->buffer_ptr, ptr_text, sizeof(ptr_text)));
    }
}

static void tensor_from_spec(ai_tensor_t *tensor,
                             const ai_tensor_spec_t *spec,
                             void *data,
                             size_t size) {
    uint32_t ndim;
    uint32_t i;

    memset(tensor, 0, sizeof(*tensor));
    ndim = spec->ndim < AI_TENSOR_SHAPE_CAPACITY ? spec->ndim : AI_TENSOR_SHAPE_CAPACITY;
    tensor->data = data;
    tensor->dtype = spec->dtype;
    tensor->ndim = ndim;
    tensor->size = size;
    for (i = 0; i < ndim; ++i) {
        tensor->shape[i] = spec->dims[i];
    }
}

static int tensor_spec_matches(const ai_tensor_spec_t *spec,
                               ai_dtype_t dtype,
                               uint32_t dim0,
                               uint32_t dim1,
                               uint32_t dim2,
                               uint32_t dim3) {
    return spec->dtype == dtype && spec->ndim == 4 && spec->dims[0] == dim0 &&
           spec->dims[1] == dim1 && spec->dims[2] == dim2 && spec->dims[3] == dim3;
}

static void fill_input(ai_miniresnetv1_s1_64x50_tl_int8_input_t *input) {
    size_t i;

    for (i = 0; i < sizeof(input->tensor_0) / sizeof(input->tensor_0[0]); ++i) {
        input->tensor_0[i] =
            (int8_t)((int32_t)(((i * 37u) + 11u) & 0xFFu) - 128);
    }
}

static uint32_t output_argmax(const ai_miniresnetv1_s1_64x50_tl_int8_output_t *output) {
    uint32_t best = 0;
    uint32_t i;

    for (i = 1; i < MINIRESNET_CLASS_COUNT; ++i) {
        if (output->tensor_0[i] > output->tensor_0[best]) {
            best = i;
        }
    }
    return best;
}

static int32_t top_score_q(const ai_miniresnetv1_s1_64x50_tl_int8_output_t *output,
                           uint32_t argmax) {
    return (int32_t)lroundf(output->tensor_0[argmax] * MINIRESNET_SCORE_SCALE);
}

static uint32_t hash_u32(uint32_t hash, uint32_t value) {
    hash ^= value;
    hash *= 16777619u;
    return hash;
}

static uint32_t output_hash(const ai_miniresnetv1_s1_64x50_tl_int8_output_t *output) {
    uint32_t hash = 2166136261u;
    uint32_t i;

    for (i = 0; i < MINIRESNET_CLASS_COUNT; ++i) {
        int32_t quantized = (int32_t)lroundf(output->tensor_0[i] * MINIRESNET_SCORE_SCALE);
        hash = hash_u32(hash, (uint32_t)quantized);
    }
    return hash;
}

static void miniresnet_validation_task(void *arg) {
    ai_model_handle_t handle;
    ai_perf_stats_t perf;
    ai_tensor_spec_t input_spec;
    ai_tensor_spec_t output_spec;
    ai_tensor_t input_tensor;
    ai_tensor_t output_tensor;
    ai_miniresnetv1_s1_64x50_tl_int8_input_t input;
    ai_miniresnetv1_s1_64x50_tl_int8_output_t output;
    os_heap_stats_t baseline;
    uint32_t argmax;
    uint32_t hash;
    int32_t score_q;
    char latency_cycles_text[21];
    char latency_instructions_text[21];
    int ret;

    (void)arg;

    printf("=== MiniResNet Validation Started ===\n");
    printf("MINIRESNET_SAMPLESET: name=synthetic_pattern count=1\n");

    baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    ret = ai_runtime_init();
    if (ret != 0) {
        printf("MINIRESNET_FAIL: runtime init failed (%d)\n", ret);
        miniresnet_halt();
    }
    print_heap_peak("init", &baseline);

    handle = ai_model_find_by_name("miniresnetv1_s1_64x50_tl_int8");
    if (!handle) {
        printf("MINIRESNET_FAIL: model lookup failed\n");
        miniresnet_halt();
    }

    if (ai_model_get_input_info(handle, 0u, &input_spec) != 0 ||
        ai_model_get_output_info(handle, 0u, &output_spec) != 0) {
        printf("MINIRESNET_FAIL: tensor metadata unavailable\n");
        miniresnet_halt();
    }

    if (!tensor_spec_matches(&input_spec,
                             AI_DTYPE_INT8,
                             1u,
                             MINIRESNET_INPUT_H,
                             MINIRESNET_INPUT_W,
                             MINIRESNET_INPUT_C) ||
        output_spec.dtype != AI_DTYPE_FP32 || output_spec.ndim != 2 ||
        output_spec.dims[0] != 1u || output_spec.dims[1] != MINIRESNET_CLASS_COUNT) {
        printf("MINIRESNET_FAIL: unexpected tensor metadata input_dtype=%u output_dtype=%u output_dims=%ux%u\n",
               (unsigned)input_spec.dtype,
               (unsigned)output_spec.dtype,
               (unsigned)output_spec.dims[0],
               (unsigned)output_spec.dims[1]);
        miniresnet_halt();
    }

    ai_model_reset_perf_stats(handle);
    fill_input(&input);
    memset(&output, 0, sizeof(output));
    tensor_from_spec(&input_tensor, &input_spec, input.tensor_0, sizeof(input.tensor_0));
    tensor_from_spec(&output_tensor, &output_spec, output.tensor_0, sizeof(output.tensor_0));

    baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    os_heap_trace_reset();
    ai_hal_allocation_trace_reset();
    ret = ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 0u);
    if (ret != 0) {
        printf("MINIRESNET_FAIL: inference failed (%d)\n", ret);
        miniresnet_halt();
    }
    print_heap_peak("invoke", &baseline);
    print_heap_trace("invoke");
    print_hal_allocation_trace("invoke");

    if (ai_model_get_perf_stats(handle, &perf) != 0) {
        printf("MINIRESNET_FAIL: perf stats unavailable\n");
        miniresnet_halt();
    }

    argmax = output_argmax(&output);
    hash = output_hash(&output);
    score_q = top_score_q(&output, argmax);
    printf(
        "MINIRESNET_RESULT: sample=synthetic_pattern argmax=%u hash=%u top_score_q=%d latency_us=%u latency_cycles=%s latency_instructions=%s total=%u\n",
        argmax,
        hash,
        score_q,
        (unsigned)perf.latency_last_us,
        u64_to_dec(perf.latency_last_cycles,
                   latency_cycles_text,
                   sizeof(latency_cycles_text)),
        u64_to_dec(perf.latency_last_instructions,
                   latency_instructions_text,
                   sizeof(latency_instructions_text)),
        (unsigned)perf.total_inferences);

    printf("MINIRESNET_PASS count=1\n");
    miniresnet_halt();
}

void os_kernel_main(void) {
    hal_board_init();

    printf("Booting MiniResNet validation...\n");

    os_kernel_init();
    os_task_create(&miniresnet_validation_tcb,
                   "miniresnet_task",
                   miniresnet_validation_task,
                   NULL,
                   10,
                   miniresnet_validation_stack,
                   sizeof(miniresnet_validation_stack));
    os_kernel_start();
}
