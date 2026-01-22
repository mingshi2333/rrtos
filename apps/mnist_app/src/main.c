#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ai_models.h"
#include "ai_model_registry.h"
#include "os_kernel.h"
#include "hal_uart.h"
#include "hal_clint.h"

// Hardware addresses (ESP32-C3)
#define UART0_BASE  0x60000000
#define CLINT_BASE  0x60023000

// Task configuration
static os_tcb_t ai_tcb;
static uint64_t ai_stack[8192]; // AI tasks need more stack

// Defined in hooks.c
extern void ai_app_pre_process(void *input_data, size_t size);
extern void ai_app_post_process(void *output_data, size_t size);

void ai_main_task(void *arg) {
    (void)arg;
    printf("=== AI App Task Started: mnist_app ===\n");

    // Initialize AI Runtime
    if (ai_runtime_init() != 0) {
        printf("AI Runtime initialization failed!\n");
        return;
    }

    // Static buffers to save stack space
    static ai_st_mnist_28_input_t input;
    static ai_st_mnist_28_output_t output;

    while(1) {
        // 1. Pre-process
        ai_app_pre_process(&input, sizeof(input));

        // 2. Run Inference
        printf("Running inference for st_mnist_28...\n");
        int ret = ai_st_mnist_28_run(&input, &output);
        if (ret != 0) {
            printf("Inference failed with code: %d\n", ret);
        } else {
            // 3. Post-process
            ai_app_post_process(&output, sizeof(output));
        }

        printf("Inference cycle complete.\n");
        os_task_delay(100); // Delay for 100 ticks
    }
}

void os_kernel_main(void) {
    // 1. Hardware Init
    hal_uart_init(UART0_BASE, 115200);
    hal_clint_init(CLINT_BASE);
    
    printf("Booting AI App: mnist_app...\n");

    // 2. Kernel Init
    os_kernel_init();
    
    // 3. Create AI Task
    os_task_create(&ai_tcb, "mnist_app_task", ai_main_task, NULL, 
                   10, ai_stack, sizeof(ai_stack));
    
    // 4. Start Kernel
    os_kernel_start();
}
