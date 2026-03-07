#include <stdio.h>
#include <stdint.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "os_kernel.h"
#include "validation.h"

static os_tcb_t validation_tcb;
static uint64_t validation_stack[8192];

static void ai_validation_halt(void) {
    while (1) {
        os_task_delay(1000);
    }
}

static void ai_validation_task(void *arg) {
    ai_model_handle_t handle;
    ai_perf_stats_t stats;
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

    for (sample_index = 0; sample_index < sample_count; ++sample_index) {
        const mnist_validation_sample_t *sample = &samples[sample_index];

        ai_model_reset_perf_stats(handle);
        mnist_validation_fill_input(&input, sample);

        ret = ai_st_mnist_28_run(&input, &output);
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
