#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "mnist_validation_samples.h"
#include "os_kernel.h"
#include "os_mem.h"

static os_tcb_t quant_validation_tcb;
static uint64_t quant_validation_stack[8192];

enum {
    AI_TENSOR_SHAPE_CAPACITY = 4,
    ST_MNISTV1_CLASS_COUNT = 36,
    ST_MNISTV1_EXPECTED_SAMPLE_INDEX = 0,
    ST_MNISTV1_EXPECTED_DIGIT = 7,
    ST_MNISTV1_INPUT_ZERO_POINT = 128,
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

static void quant_validation_halt(void) {
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
        "ST_MNISTV1_QUANT_MODEL_PEAK: phase=%s heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u\n",
        phase,
        (unsigned)stats.current_used_bytes,
        (unsigned)stats_delta(stats.peak_used_bytes, baseline->current_used_bytes),
        (unsigned)stats_delta(stats.allocation_count, baseline->allocation_count),
        (unsigned)stats_delta(stats.free_count, baseline->free_count));
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

static void fill_quant_input(ai_st_mnistv1_28_tfs_int8_input_t *input,
                             const mnist_validation_sample_t *sample) {
    size_t i;

    for (i = 0; i < sizeof(input->tensor_0) / sizeof(input->tensor_0[0]); ++i) {
        input->tensor_0[i] =
            (int8_t)((int32_t)sample->pixels[i] - ST_MNISTV1_INPUT_ZERO_POINT);
    }
}

static uint32_t output_argmax(const ai_st_mnistv1_28_tfs_int8_output_t *output) {
    uint32_t best = 0;
    uint32_t i;

    for (i = 1; i < ST_MNISTV1_CLASS_COUNT; ++i) {
        if (output->tensor_0[i] > output->tensor_0[best]) {
            best = i;
        }
    }
    return best;
}

static int32_t top_score_q(const ai_st_mnistv1_28_tfs_int8_output_t *output,
                           uint32_t argmax) {
    return (int32_t)lroundf(output->tensor_0[argmax] * 100000.0f);
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

static void quant_validation_task(void *arg) {
    ai_model_handle_t handle;
    ai_perf_stats_t perf;
    ai_tensor_spec_t input_spec;
    ai_tensor_spec_t output_spec;
    ai_tensor_t input_tensor;
    ai_tensor_t output_tensor;
    ai_st_mnistv1_28_tfs_int8_input_t input;
    ai_st_mnistv1_28_tfs_int8_output_t output;
    const mnist_validation_sample_t *sample;
    os_heap_stats_t baseline;
    uint32_t argmax;
    int32_t score_q;
    char latency_cycles_text[21];
    char latency_instructions_text[21];
    int ret;

    (void)arg;

    printf("=== ST MNISTv1 Quant Validation Started ===\n");
    printf("ST_MNISTV1_QUANT_SAMPLESET: name=%s count=1\n",
           MNIST_VALIDATION_SAMPLESET_NAME);

    if (MNIST_VALIDATION_SAMPLE_COUNT <= ST_MNISTV1_EXPECTED_SAMPLE_INDEX) {
        printf("ST_MNISTV1_QUANT_FAIL: validation sample unavailable\n");
        quant_validation_halt();
    }
    sample = &k_mnist_validation_samples[ST_MNISTV1_EXPECTED_SAMPLE_INDEX];
    if (sample->label != ST_MNISTV1_EXPECTED_DIGIT) {
        printf("ST_MNISTV1_QUANT_FAIL: sample label=%u expected=%u\n",
               sample->label,
               ST_MNISTV1_EXPECTED_DIGIT);
        quant_validation_halt();
    }

    baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    ret = ai_runtime_init();
    if (ret != 0) {
        printf("ST_MNISTV1_QUANT_FAIL: runtime init failed (%d)\n", ret);
        quant_validation_halt();
    }
    print_heap_peak("init", &baseline);

    handle = ai_model_find_by_name("st_mnistv1_28_tfs_int8");
    if (!handle) {
        printf("ST_MNISTV1_QUANT_FAIL: model lookup failed\n");
        quant_validation_halt();
    }

    if (ai_model_get_input_info(handle, 0u, &input_spec) != 0 ||
        ai_model_get_output_info(handle, 0u, &output_spec) != 0) {
        printf("ST_MNISTV1_QUANT_FAIL: tensor metadata unavailable\n");
        quant_validation_halt();
    }

    if (!tensor_spec_matches(&input_spec, AI_DTYPE_INT8, 1u, 28u, 28u, 1u) ||
        output_spec.dtype != AI_DTYPE_FP32 || output_spec.ndim != 2 ||
        output_spec.dims[0] != 1u || output_spec.dims[1] != ST_MNISTV1_CLASS_COUNT) {
        printf("ST_MNISTV1_QUANT_FAIL: unexpected tensor metadata input_dtype=%u output_dtype=%u output_dims=%ux%u\n",
               (unsigned)input_spec.dtype,
               (unsigned)output_spec.dtype,
               (unsigned)output_spec.dims[0],
               (unsigned)output_spec.dims[1]);
        quant_validation_halt();
    }

    ai_model_reset_perf_stats(handle);
    fill_quant_input(&input, sample);
    memset(&output, 0, sizeof(output));
    tensor_from_spec(&input_tensor, &input_spec, input.tensor_0, sizeof(input.tensor_0));
    tensor_from_spec(&output_tensor, &output_spec, output.tensor_0, sizeof(output.tensor_0));

    baseline = os_heap_stats_get();
    os_heap_stats_reset_peak();
    ret = ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 0u);
    if (ret != 0) {
        printf("ST_MNISTV1_QUANT_FAIL: inference failed (%d)\n", ret);
        quant_validation_halt();
    }
    print_heap_peak("invoke", &baseline);

    if (ai_model_get_perf_stats(handle, &perf) != 0) {
        printf("ST_MNISTV1_QUANT_FAIL: perf stats unavailable\n");
        quant_validation_halt();
    }

    argmax = output_argmax(&output);
    score_q = top_score_q(&output, argmax);
    printf(
        "ST_MNISTV1_QUANT_RESULT: sample=%s label=%u argmax=%u top_score_q=%d latency_us=%u latency_cycles=%s latency_instructions=%s total=%u\n",
        sample->id,
        sample->label,
        argmax,
        score_q,
        (unsigned)perf.latency_last_us,
        u64_to_dec(perf.latency_last_cycles,
                   latency_cycles_text,
                   sizeof(latency_cycles_text)),
        u64_to_dec(perf.latency_last_instructions,
                   latency_instructions_text,
                   sizeof(latency_instructions_text)),
        (unsigned)perf.total_inferences);

    if (argmax != ST_MNISTV1_EXPECTED_DIGIT) {
        printf("ST_MNISTV1_QUANT_FAIL: expected_argmax=%u actual_argmax=%u\n",
               ST_MNISTV1_EXPECTED_DIGIT,
               argmax);
        quant_validation_halt();
    }

    printf("ST_MNISTV1_QUANT_PASS count=1\n");
    quant_validation_halt();
}

void os_kernel_main(void) {
    hal_board_init();

    printf("Booting ST MNISTv1 quant validation...\n");

    os_kernel_init();
    os_task_create(&quant_validation_tcb, "mnist_quant_task", quant_validation_task,
                   NULL, 10, quant_validation_stack, sizeof(quant_validation_stack));
    os_kernel_start();
}
