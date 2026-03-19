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
static os_timer_t validation_timer;
static volatile uint32_t validation_timer_callbacks;
static volatile os_tick_t validation_timer_last_tick;

enum {
    AI_TENSOR_SHAPE_CAPACITY = 4,
};

_Static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_t *)0)->shape) / sizeof(((ai_tensor_t *)0)->shape[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_t.shape capacity");

_Static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_spec_t *)0)->dims) / sizeof(((ai_tensor_spec_t *)0)->dims[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_spec_t.dims capacity");

static void ai_tensor_from_spec(ai_tensor_t *tensor,
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

static void ai_validation_halt(void) {
    while (1) {
        os_task_delay(1000);
    }
}

static void validation_timer_cb(void *arg) {
    uint32_t *counter = (uint32_t *)arg;

    (*counter)++;
    validation_timer_last_tick = os_tick_get();
}

static int wait_for_validation_timer(os_tick_t timeout_ticks) {
    os_tick_t waited = 0;

    while (validation_timer_callbacks == 0 && waited < timeout_ticks) {
        if (os_task_delay(1) != OS_EOK) {
            return -1;
        }
        waited++;
    }

    return validation_timer_callbacks > 0 ? 0 : -1;
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

    validation_timer_callbacks = 0;
    validation_timer_last_tick = 0;
    ret = os_timer_init(&validation_timer, "validation_timer", validation_timer_cb,
                        (void *)&validation_timer_callbacks, 4, 0);
    if (ret != OS_EOK) {
        printf("AI_VALIDATION_FAIL: timer init failed (%d)\n", ret);
        ai_validation_halt();
    }
    ret = os_timer_start(&validation_timer);
    if (ret != OS_EOK) {
        printf("AI_VALIDATION_FAIL: timer start failed (%d)\n", ret);
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
        ai_validation_halt();
    }

    if (wait_for_validation_timer(32) != 0) {
        printf("AI_VALIDATION_FAIL: timer callback missing\n");
        ai_validation_halt();
    }

    printf("OS_TIMER_CALLBACK_PASS count=%u tick=%u\n",
           (unsigned)validation_timer_callbacks,
           (unsigned)validation_timer_last_tick);

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
