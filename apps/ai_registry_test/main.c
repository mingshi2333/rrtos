#include "ai_model_registry.h"
#include "os_kernel.h"
#include "hal_uart.h"
#include "hal_clint.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static os_tcb_t test_tcb;
static uint64_t test_stack[4096];

static int8_t input_buffer[56 * 56 * 3] __attribute__((aligned(16)));
static int8_t output_buffer[7 * 7 * 18] __attribute__((aligned(16)));

void test_task(void *arg) {
    (void)arg;
    
    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║   AI Runtime Registry - Verification Test  ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("[TEST] Initializing AI Runtime...\n");
    int ret = ai_runtime_init();
    if (ret != 0) {
        printf("[TEST] ERROR: ai_runtime_init() failed: %d\n", ret);
        goto exit;
    }
    printf("[TEST] AI Runtime initialized successfully\n\n");
    
    const char *model_names[8];
    int model_count = ai_model_list(model_names, 8);
    printf("[TEST] Registered models: %d\n", model_count);
    for (int i = 0; i < model_count; i++) {
        printf("  [%d] %s\n", i, model_names[i]);
    }
    printf("\n");
    
    ai_model_handle_t mnist = ai_model_find_by_name("mnist");
    if (mnist) {
        ai_tensor_spec_t input_spec;
        ai_model_get_input_info(mnist, 0, &input_spec);
        printf("  Input: [%d, %d, %d, %d], dtype=%d\n",
               input_spec.dims[0], input_spec.dims[1],
               input_spec.dims[2], input_spec.dims[3],
               input_spec.dtype);
        
        ai_tensor_spec_t output_spec;
        ai_model_get_output_info(mnist, 0, &output_spec);
        printf("  Output: [%d, %d], dtype=%d\n",
               output_spec.dims[0], output_spec.dims[1],
               output_spec.dtype);

        // Run MNIST inference
        printf("[TEST] Running MNIST inference...\n");
        uint8_t mnist_input[28*28];
        memset(mnist_input, 0, sizeof(mnist_input)); // Dummy data
        
        ai_tensor_t input = {
            .data = mnist_input,
            .shape = {1, 28, 28, 1},
            .ndim = 4,
            .dtype = AI_DTYPE_INT8,
            .size = sizeof(mnist_input)
        };
        
        uint8_t mnist_output[10];
        ai_tensor_t output = {
            .data = mnist_output,
            .shape = {1, 10},
            .ndim = 2,
            .dtype = AI_DTYPE_INT8,
            .size = sizeof(mnist_output)
        };
        
        if (ai_infer_sync(mnist, &input, &output, 1000) != 0) {
            printf("[TEST] ERROR: MNIST inference failed\n");
        } else {
            printf("[TEST] MNIST inference success!\n");
        }
    } else {
        printf("[TEST] WARNING: Model 'mnist' not found\n");
    }
    printf("\n");
    
    ai_model_handle_t yolo = ai_model_find_by_name("yoloface");
    if (yolo) {
        printf("[TEST] Found model: yoloface\n");
        
        ai_tensor_spec_t input_spec;
        ai_model_get_input_info(yolo, 0, &input_spec);
        printf("  Input: [%d, %d, %d, %d], dtype=%d\\n",
               input_spec.dims[0], input_spec.dims[1],
               input_spec.dims[2], input_spec.dims[3],
               input_spec.dtype);
        
        printf("[TEST] Preparing dummy input data (56x56x3)...\\n");
        memset(input_buffer, 0, sizeof(input_buffer));
        
        ai_tensor_t input = {
            .data = input_buffer,
            .shape = {1, 56, 56, 3},
            .ndim = 4,
            .dtype = AI_DTYPE_INT8,
            .size = sizeof(input_buffer)
        };
        
        ai_tensor_t output = {
            .data = output_buffer,
            .shape = {1, 7, 7, 18},
            .ndim = 4,
            .dtype = AI_DTYPE_INT8,
            .size = sizeof(output_buffer)
        };
        
        printf("[TEST] Running inference...\n");
        ret = ai_infer_sync(yolo, &input, &output, 5000);
        if (ret != 0) {
            printf("[TEST] ERROR: Inference failed: %d\n", ret);
        } else {
            printf("[TEST] Inference completed successfully\n");
            
            ai_perf_stats_t stats;
            ai_model_get_perf_stats(yolo, &stats);
            printf("[PERF] Latency: %lu us\n", (unsigned long)stats.latency_last_us);
            printf("[PERF] Min: %lu us, Max: %lu us\n",
                   (unsigned long)stats.latency_min_us, (unsigned long)stats.latency_max_us);
            printf("[PERF] Total inferences: %lu\n", (unsigned long)stats.total_inferences);
        }
    } else {
        printf("[TEST] WARNING: Model 'yoloface' not found\n");
    }
    printf("\n");
    
    printf("╔════════════════════════════════════════════╗\n");
    printf("║              TEST COMPLETE                 ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    
exit:
    ai_runtime_deinit();
    printf("[TEST] Done.\n");
    
    while(1) {}
}

void os_kernel_main(void) {
    hal_uart_init(0x10000000, 115200);
    printf("Booting RTOS...\n");
    
    hal_clint_init(0x02000000);
    os_kernel_init();
    
    os_task_create(&test_tcb, "ai_registry_test", test_task, NULL,
                   10, test_stack, sizeof(test_stack));
    
    os_kernel_start();
}
