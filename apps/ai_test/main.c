#include "os_kernel.h"
#include "hal_uart.h"
#include "ai_runtime.h"

void ai_test_task(void *param) {
    (void)param;
    
    os_print("[AI Test] Starting AI model test...\n");
    
    ai_err_t err = ai_runtime_init(NULL, 0);
    if (err != AI_ERR_OK) {
        os_print("[AI Test] Failed to initialize AI runtime: %d\n", err);
        return;
    }
    os_print("[AI Test] AI runtime initialized successfully\n");
    
    ai_model_t *model = NULL;
    err = ai_model_load(&model, NULL, 0);
    if (err != AI_ERR_OK) {
        os_print("[AI Test] Failed to load model: %d\n", err);
        ai_runtime_deinit();
        return;
    }
    os_print("[AI Test] Model loaded successfully\n");
    
    float input1[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float input2[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float output[4] = {0};
    
    ai_tensor_t inputs[2] = {
        {
            .data = input1,
            .shape = {4},
            .rank = 1,
            .dtype = AI_DTYPE_FLOAT32
        },
        {
            .data = input2,
            .shape = {4},
            .rank = 1,
            .dtype = AI_DTYPE_FLOAT32
        }
    };
    
    ai_tensor_t outputs[1] = {
        {
            .data = output,
            .shape = {4},
            .rank = 1,
            .dtype = AI_DTYPE_FLOAT32
        }
    };
    
    os_print("[AI Test] Running inference...\n");
    err = ai_model_invoke(model, inputs, 2, outputs, 1);
    if (err != AI_ERR_OK) {
        os_print("[AI Test] Inference failed: %d\n", err);
    } else {
        os_print("[AI Test] Inference succeeded!\n");
        os_print("[AI Test] Results: [%.2f, %.2f, %.2f, %.2f]\n",
                 output[0], output[1], output[2], output[3]);
        os_print("[AI Test] Expected: [6.00, 8.00, 10.00, 12.00]\n");
    }
    
    ai_model_unload(model);
    ai_runtime_deinit();
    
    os_print("[AI Test] Test completed\n");
    
    while(1) {
        os_task_delay(1000);
    }
}

int main(void) {
    hal_uart_init();
    os_print("\n=== RTOS AI Model Test ===\n");
    
    os_init();
    
    os_tcb_t *task = os_task_create(ai_test_task, NULL, 2048, 5, "ai_test");
    if (task == NULL) {
        os_print("Failed to create AI test task\n");
        return -1;
    }
    
    os_start();
    
    return 0;
}
