/**
 * @file main.c
 * @brief AI Demo Application - STM32CubeAI Style
 * 
 * 这是一个简洁的 AI 推理模板，用户只需：
 * 1. 修改 ai_model.h 中的模型配置
 * 2. 链接对应的模型 .o 文件
 * 3. 在此文件中填充输入数据并调用 ai_run()
 */

#include <stdio.h>
#include <string.h>
#include "ai_model.h"
#include "os_kernel.h"

static inline uint64_t get_cycles(void) {
#if __riscv_xlen == 64
    uint64_t cycles;
    asm volatile ("rdcycle %0" : "=r"(cycles));
    return cycles;
#else
    uint32_t lo, hi;
    asm volatile ("rdcycleh %0" : "=r"(hi));
    asm volatile ("rdcycle %0" : "=r"(lo));
    return ((uint64_t)hi << 32) | lo;
#endif
}

/*===========================================================================*/
/*                        硬件配置                                            */
/*===========================================================================*/

#define UART0_BASE  0x10000000

/*===========================================================================*/
/*                        任务配置                                            */
/*===========================================================================*/

static os_tcb_t ai_tcb;
static uint64_t ai_stack[4096];

/*===========================================================================*/
/*                        测试数据 (用户填充区)                               */
/*===========================================================================*/

/* 静态分配输入缓冲区 */
static ai_input_t input_buffer[AI_INPUT_SIZE];

/**
 * @brief 初始化测试输入
 * 
 * 用户可以在这里：
 * - 从传感器读取数据
 * - 加载测试图像
 * - 生成测试模式
 */
static void prepare_input(void) {
    /* 示例: 生成 28x28 的测试图案 */
    memset(input_buffer, 0, sizeof(input_buffer));
    
    for (int y = 0; y < AI_INPUT_HEIGHT; y++) {
        for (int x = 0; x < AI_INPUT_WIDTH; x++) {
            int idx = (y * AI_INPUT_WIDTH + x) * AI_INPUT_CHANNELS;
            /* 简单的渐变图案 */
            input_buffer[idx] = (ai_input_t)((x + y) & 0x7F);
        }
    }
    
    printf("[APP] Input prepared (%dx%dx%d)\n", 
           AI_INPUT_HEIGHT, AI_INPUT_WIDTH, AI_INPUT_CHANNELS);
}

/*===========================================================================*/
/*                        结果处理 (用户填充区)                               */
/*===========================================================================*/

/**
 * @brief 处理推理结果
 * 
 * 用户可以在这里：
 * - 打印预测结果
 * - 控制执行器
 * - 发送结果到上位机
 */
static void process_output(void) {
    const ai_output_t* output = ai_get_output();
    
    /* 打印所有类别的输出 */
    printf("[APP] Output probabilities:\n");
    for (int i = 0; i < AI_OUTPUT_SIZE; i++) {
        printf("  Class %d: %d\n", i, (int)output[i]);
    }
    
    /* 获取预测结果 */
    ai_output_t confidence;
    int prediction = ai_get_prediction(&confidence);
    
    printf("\n[APP] Prediction: Class %d (confidence: %d)\n", 
           prediction, (int)confidence);
}

/*===========================================================================*/
/*                        主任务                                              */
/*===========================================================================*/

void ai_task(void *arg) {
    (void)arg;
    ai_status_t status;

    printf("\n");
    printf("╔════════════════════════════════════════════╗\n");
    printf("║     AI Demo - STM32CubeAI Style Template   ║\n");
    printf("╚════════════════════════════════════════════╝\n");
    printf("\n");
    printf("[APP] Model: %s\n", AI_MODEL_NAME);
    printf("[APP] Input: %dx%dx%d (%d bytes)\n", 
           AI_INPUT_HEIGHT, AI_INPUT_WIDTH, AI_INPUT_CHANNELS, AI_INPUT_SIZE);
    printf("[APP] Output: %d classes\n", AI_OUTPUT_SIZE);
    printf("\n");

    /* Step 1: 初始化 AI 运行时 */
    printf("[APP] Initializing AI runtime...\n");
    status = ai_init();
    if (status != AI_OK) {
        printf("[APP] ERROR: AI init failed (%d)\n", status);
        goto exit;
    }
    printf("[APP] AI runtime ready\n\n");

    /* Step 2: 准备输入数据 */
    printf("[APP] Preparing input data...\n");
    prepare_input();
    printf("\n");

    /* Step 3: 运行推理 */
    printf("[APP] Running inference...\n");
    uint64_t start_cycles = get_cycles();
    status = ai_run(input_buffer);
    uint64_t end_cycles = get_cycles();
    uint64_t inference_cycles = end_cycles - start_cycles;
    if (status != AI_OK) {
        printf("[APP] ERROR: Inference failed (%d)\n", status);
        goto exit;
    }
    printf("[APP] Inference complete\n");
    uint32_t cycles_lo = (uint32_t)(inference_cycles & 0xFFFFFFFF);
    uint32_t cycles_hi = (uint32_t)(inference_cycles >> 32);
    uint32_t ms_100mhz = (uint32_t)(inference_cycles / 100000);
    uint32_t ms_480mhz = (uint32_t)(inference_cycles / 480000);
    printf("[PERF] Cycles High: 0x%x\n", cycles_hi);
    printf("[PERF] Cycles Low:  0x%x\n", cycles_lo);
    printf("[PERF] Time @ 100MHz: %u ms\n", ms_100mhz);
    printf("[PERF] Time @ 480MHz: %u ms\n\n", ms_480mhz);

    /* Step 4: 处理输出 */
    printf("[APP] Processing results...\n");
    process_output();
    printf("\n");

    printf("╔════════════════════════════════════════════╗\n");
    printf("║              SUCCESS!                      ║\n");
    printf("╚════════════════════════════════════════════╝\n");

exit:
    /* 清理 */
    ai_deinit();
    printf("[APP] Done.\n");
    
    while(1) {
        /* 空闲循环 */
    }
}

/*===========================================================================*/
/*                        系统入口                                            */
/*===========================================================================*/

void os_kernel_main(void) {
    /* 初始化 UART */
    hal_uart_init(UART0_BASE, 115200);
    printf("Booting RTOS...\n");
    
    /* 初始化内核 */
    os_kernel_init();
    
    /* 创建 AI 任务 */
    os_task_create(&ai_tcb, "ai_demo", ai_task, NULL, 
                   10, ai_stack, sizeof(ai_stack));
    
    /* 启动调度器 */
    os_kernel_start();
}
