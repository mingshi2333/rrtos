/**
 * @file main.c
 * @brief AI Demo Application - STM32CubeAI Style
 * 
 * 这是一个简洁的 AI 推理模板，用户只需：
 * 1. 修改 ai_model.h 中的模型配置
 * 2. 链接对应的模型 .o 文件
 * 3. 在此文件中填充输入数据并调用 ai_run()
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "ai_model.h"
#include "face_56x56_int8.h"
#include "os_kernel.h"
#include "hal_clint.h"

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
#define CLINT_BASE  0x02000000

/*===========================================================================*/
/*                        任务配置                                            */
/*===========================================================================*/

static os_tcb_t ai_tcb;
static uint64_t ai_stack[4096];

/*===========================================================================*/
/*                        测试数据 (用户填充区)                               */
/*===========================================================================*/

/* 静态分配输入缓冲区 */
static ai_input_t input_buffer[AI_INPUT_SIZE] __attribute__((aligned(16)));

/**
 * @brief 初始化测试输入
 * 
 * 用户可以在这里：
 * - 从传感器读取数据
 * - 加载测试图像
 * - 生成测试模式
 */
static void prepare_input(void) {
    memcpy(input_buffer, face_56x56_int8, sizeof(face_56x56_int8));
    
    printf("[APP] Input prepared (%dx%dx%d)\n", 
           AI_INPUT_HEIGHT, AI_INPUT_WIDTH, AI_INPUT_CHANNELS);
}

static int32_t ai_float_to_fixed1e6(float value) {
    if (value >= 0.0f) {
        return (int32_t)(value * 1000000.0f + 0.5f);
    }
    return (int32_t)(value * 1000000.0f - 0.5f);
}

static float ai_sigmoid(float x) {
    return 1.0f / (1.0f + expf(-x));
}

static float ai_dequant_output(ai_output_t value) {
    return ((float)value - AI_OUTPUT_ZERO_POINT) * AI_OUTPUT_SCALE;
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
#define YOLO_ANCHOR_COUNT 3
#define YOLO_ATTRS 6
#define YOLO_CONF_THRESHOLD 0.3f
#define YOLO_NMS_THRESHOLD 0.7f
#define YOLO_MAX_DETECTIONS 20

    const float anchors[YOLO_ANCHOR_COUNT][2] = {
        {9.0f, 14.0f},
        {12.0f, 17.0f},
        {22.0f, 21.0f},
    };
    const float stride = (float)AI_INPUT_WIDTH / (float)AI_OUTPUT_WIDTH;

    typedef struct {
        float x1;
        float y1;
        float x2;
        float y2;
        float score;
    } yolo_det_t;

    yolo_det_t dets[AI_OUTPUT_HEIGHT * AI_OUTPUT_WIDTH * YOLO_ANCHOR_COUNT];
    int det_count = 0;
    int threshold_fp = ai_float_to_fixed1e6(YOLO_CONF_THRESHOLD);

    for (int y = 0; y < AI_OUTPUT_HEIGHT; ++y) {
        for (int x = 0; x < AI_OUTPUT_WIDTH; ++x) {
            int base = (y * AI_OUTPUT_WIDTH + x) * AI_OUTPUT_CHANNELS;
            for (int a = 0; a < YOLO_ANCHOR_COUNT; ++a) {
                int offset = base + a * YOLO_ATTRS;
                float tx = ai_dequant_output(output[offset + 0]);
                float ty = ai_dequant_output(output[offset + 1]);
                float tw = ai_dequant_output(output[offset + 2]);
                float th = ai_dequant_output(output[offset + 3]);
                float obj = ai_sigmoid(ai_dequant_output(output[offset + 4]));
                float cls = ai_sigmoid(ai_dequant_output(output[offset + 5]));
                float score = obj * cls;

                if (score < YOLO_CONF_THRESHOLD ||
                    det_count >= (int)(sizeof(dets) / sizeof(dets[0]))) {
                    continue;
                }

                float cx = (ai_sigmoid(tx) + (float)x) * stride;
                float cy = (ai_sigmoid(ty) + (float)y) * stride;
                float bw = expf(tw) * anchors[a][0];
                float bh = expf(th) * anchors[a][1];

                float x1 = cx - bw * 0.5f;
                float y1 = cy - bh * 0.5f;
                float x2 = cx + bw * 0.5f;
                float y2 = cy + bh * 0.5f;

                if (x1 < 0.0f) x1 = 0.0f;
                if (y1 < 0.0f) y1 = 0.0f;
                if (x2 > (float)AI_INPUT_WIDTH) x2 = (float)AI_INPUT_WIDTH;
                if (y2 > (float)AI_INPUT_HEIGHT) y2 = (float)AI_INPUT_HEIGHT;

                dets[det_count++] = (yolo_det_t){x1, y1, x2, y2, score};
            }
        }
    }

    if (det_count == 0) {
        printf("[APP] No detections (score >= %d x1e6)\n", threshold_fp);
        return;
    }

    // Sort by score (descending)
    for (int i = 0; i < det_count - 1; ++i) {
        for (int j = i + 1; j < det_count; ++j) {
            if (dets[j].score > dets[i].score) {
                yolo_det_t tmp = dets[i];
                dets[i] = dets[j];
                dets[j] = tmp;
            }
        }
    }

    int kept = 0;
    int suppressed[AI_OUTPUT_HEIGHT * AI_OUTPUT_WIDTH * YOLO_ANCHOR_COUNT] = {0};

    printf("[APP] Detections (score >= %d x1e6):\n", threshold_fp);
    for (int i = 0; i < det_count && kept < YOLO_MAX_DETECTIONS; ++i) {
        if (suppressed[i]) {
            continue;
        }

        float area_i = (dets[i].x2 - dets[i].x1) * (dets[i].y2 - dets[i].y1);
        if (area_i < 0.0f) area_i = 0.0f;

        for (int j = i + 1; j < det_count; ++j) {
            if (suppressed[j]) {
                continue;
            }
            float xx1 = dets[i].x1 > dets[j].x1 ? dets[i].x1 : dets[j].x1;
            float yy1 = dets[i].y1 > dets[j].y1 ? dets[i].y1 : dets[j].y1;
            float xx2 = dets[i].x2 < dets[j].x2 ? dets[i].x2 : dets[j].x2;
            float yy2 = dets[i].y2 < dets[j].y2 ? dets[i].y2 : dets[j].y2;

            float iw = xx2 - xx1;
            float ih = yy2 - yy1;
            if (iw <= 0.0f || ih <= 0.0f) {
                continue;
            }

            float inter = iw * ih;
            float area_j = (dets[j].x2 - dets[j].x1) * (dets[j].y2 - dets[j].y1);
            if (area_j < 0.0f) area_j = 0.0f;
            float uni = area_i + area_j - inter;
            if (uni <= 0.0f) {
                continue;
            }
            float iou = inter / uni;
            if (iou > YOLO_NMS_THRESHOLD) {
                suppressed[j] = 1;
            }
        }

        int x1 = (int)(dets[i].x1 + 0.5f);
        int y1 = (int)(dets[i].y1 + 0.5f);
        int x2 = (int)(dets[i].x2 + 0.5f);
        int y2 = (int)(dets[i].y2 + 0.5f);
        int score_fp = (int)ai_float_to_fixed1e6(dets[i].score);
        printf("  box %d: x1=%d y1=%d x2=%d y2=%d score=%d\n",
               kept, x1, y1, x2, y2, score_fp);
        kept++;
    }
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
    printf("[APP] Output: %dx%dx%d (int8)\n",
           AI_OUTPUT_HEIGHT, AI_OUTPUT_WIDTH, AI_OUTPUT_CHANNELS);
    size_t output_bytes = ai_output_buffer_size();
    size_t static_total = sizeof(input_buffer) + output_bytes + sizeof(ai_stack);
    printf("[MEM] Static RAM: input=%u bytes, output=%u bytes, task_stack=%u bytes, total=%u bytes\n",
           (unsigned)sizeof(input_buffer),
           (unsigned)output_bytes,
           (unsigned)sizeof(ai_stack),
           (unsigned)static_total);
    printf("\n");

    /* Step 1: 初始化 AI 运行时 */
    printf("[APP] Initializing AI runtime...\n");
    status = ai_init();
    if (status != AI_OK) {
        printf("[APP] ERROR: AI init failed (%d)\n", status);
        goto exit;
    }
    printf("[APP] AI runtime ready\n\n");
    printf("[MEM] Model rodata (Flash/RO): %u bytes\n",
           (unsigned)ai_model_rodata_size());
    printf("[MEM] IREE allocator statistics (after init):\n");
    ai_print_allocator_stats();
    printf("\n");

    /* Step 2: 准备输入数据 */
    printf("[APP] Preparing input data...\n");
    prepare_input();
    printf("\n");
    printf("[MEM] IREE allocator statistics (before inference):\n");
    ai_print_allocator_stats();
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
    printf("[MEM] IREE allocator statistics (after inference):\n");
    ai_print_allocator_stats();
    printf("\n");

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

    /* 初始化 CLINT (Timer) */
    hal_clint_init(CLINT_BASE);
    
    /* 初始化内核 */
    os_kernel_init();
    
    /* 创建 AI 任务 */
    os_task_create(&ai_tcb, "ai_demo", ai_task, NULL, 
                   10, ai_stack, sizeof(ai_stack));
    
    /* 启动调度器 */
    os_kernel_start();
}
