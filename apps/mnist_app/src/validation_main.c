#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "os_kernel.h"
#include "validation.h"

static os_tcb_t validation_tcb;
static uint64_t validation_stack[8192];

enum {
    AI_TENSOR_SHAPE_CAPACITY = 4,
};

static void ai_tensor_from_spec(ai_tensor_t *tensor,
                                const ai_tensor_spec_t *spec,
                                void *data,
                                size_t size) {
    uint32_t i;

    memset(tensor, 0, sizeof(*tensor));
    tensor->data = data;
    tensor->dtype = spec->dtype;
    tensor->ndim = spec->ndim;
    tensor->size = size;
    for (i = 0; i < spec->ndim; ++i) {
        tensor->shape[i] = spec->dims[i];
    }
}

static void ai_validation_halt(void) {
    while (1) {
        os_task_delay(1000);
    }
}

static void ai_validation_task(void *arg) {
    ai_model_handle_t handle;
    ai_perf_stats_t stats;
    ai_tensor_spec_t input_spec;
    ai_tensor_spec_t output_spec;
    ai_tensor_t input_tensor;
    ai_tensor_t output_tensor;
    ai_st_mnist_28_input_t input;
    ai_st_mnist_28_output_t output;
    mnist_validation_observation_t observation;
    mnist_validation_status_t validation_status;
    const mnist_validation_sample_t *samples;
    size_t sample_count;
    size_t sample_index;
    uint32_t failure_count = 0;
    int ret;

    (void)arg;

    printf("=== AI Validation Task Started: mnist_validation ===\n");
    printf("AI_VALIDATION_SAMPLESET: name=%s count=%u\n",
           MNIST_VALIDATION_SAMPLESET_NAME,
           (unsigned)MNIST_VALIDATION_SAMPLE_COUNT);

    samples = mnist_validation_get_samples(&sample_count);
    if (!samples || sample_count == 0) {
        printf("AI_VALIDATION_FAIL: no validation samples configured\n");
        ai_validation_halt();
    }

    ret = ai_runtime_init();
    if (ret != 0) {
        printf("AI_VALIDATION_FAIL: runtime init failed (%d)\n", ret);
        ai_validation_halt();
    }

    handle = ai_model_find_by_name("st_mnist_28");
    if (!handle) {
        printf("AI_VALIDATION_FAIL: model lookup failed\n");
        ai_validation_halt();
    }

    if (ai_model_get_input_info(handle, 0u, &input_spec) != 0 ||
        ai_model_get_output_info(handle, 0u, &output_spec) != 0) {
        printf("AI_VALIDATION_FAIL: model tensor metadata unavailable\n");
        ai_validation_halt();
    }

    if (input_spec.ndim > AI_TENSOR_SHAPE_CAPACITY ||
        output_spec.ndim > AI_TENSOR_SHAPE_CAPACITY) {
        printf(
            "AI_VALIDATION_FAIL: tensor metadata ndim exceeds local shape capacity (input=%u output=%u max=%u)\n",
            input_spec.ndim,
            output_spec.ndim,
            AI_TENSOR_SHAPE_CAPACITY);
        ai_validation_halt();
    }

    for (sample_index = 0; sample_index < sample_count; ++sample_index) {
        const mnist_validation_sample_t *sample = &samples[sample_index];

        ai_model_reset_perf_stats(handle);
        mnist_validation_fill_input(&input, sample);
        ai_tensor_from_spec(&input_tensor, &input_spec, input.tensor_0,
                            sizeof(input.tensor_0));
        ai_tensor_from_spec(&output_tensor, &output_spec, output.tensor_0,
                            sizeof(output.tensor_0));

        ret = ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 0u);
        if (ret != 0) {
            printf("AI_VALIDATION_FAIL: sample=%s inference failed (%d)\n",
                   sample->id,
                   ret);
            ai_validation_halt();
        }

        if (ai_model_get_perf_stats(handle, &stats) != 0) {
            printf("AI_VALIDATION_FAIL: sample=%s perf stats unavailable\n", sample->id);
            ai_validation_halt();
        }

        validation_status = mnist_validation_check(sample, &output, &stats, &observation);
        printf(
            "AI_VALIDATION_METRICS: sample=%s idx=%u label=%u argmax=%u hash=%u top_score_q=%d latency_us=%u total=%u arena_peak=%u\n",
            sample->id,
            sample->dataset_index,
            sample->label,
            observation.argmax,
            observation.hash,
            observation.top_score_q,
            observation.latency_us,
            observation.total_inferences,
            observation.arena_peak);

        if (validation_status != MNIST_VALIDATION_OK) {
            printf("AI_VALIDATION_FAIL: sample=%s code=%d\n",
                   sample->id,
                   (int)validation_status);
            ++failure_count;
        }
    }

    if (failure_count == 0) {
        printf("AI_VALIDATION_PASS count=%u\n", (unsigned)sample_count);
    } else {
        printf("AI_VALIDATION_FAIL: total_failures=%u\n", failure_count);
    }

    ai_validation_halt();
}

void os_kernel_main(void) {
    hal_board_init();

    printf("Booting AI validation: mnist_validation...\n");

    os_kernel_init();
    os_task_create(&validation_tcb, "mnist_validation_task", ai_validation_task,
                   NULL, 10, validation_stack, sizeof(validation_stack));
    os_kernel_start();
}
