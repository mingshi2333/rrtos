/**
 * @file ai_model.h
 * @brief AI Model Configuration - STM32CubeAI Style
 * 
 * 使用方法：
 * 1. 修改 MODEL_* 宏定义匹配你的模型
 * 2. 链接对应的 .o 文件 (emitc 或 bytecode 生成)
 * 3. 在 main.c 中调用 ai_init() / ai_run() / ai_get_output()
 * 
 * 更换模型只需：
 * - 修改此头文件中的配置
 * - 更换链接的 .o 文件
 */

#ifndef AI_MODEL_H
#define AI_MODEL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/*                        用户配置区 - 修改这里                               */
/*===========================================================================*/

/* 模型名称 (用于日志) */
#define AI_MODEL_NAME           "yoloface_int8"

/* 输入配置 */
#define AI_INPUT_HEIGHT         56
#define AI_INPUT_WIDTH          56
#define AI_INPUT_CHANNELS       3
#define AI_INPUT_SIZE           (AI_INPUT_HEIGHT * AI_INPUT_WIDTH * AI_INPUT_CHANNELS)

/* 输出配置 - YOLO检测 (7x7x18) */
#define AI_OUTPUT_HEIGHT        7
#define AI_OUTPUT_WIDTH         7
#define AI_OUTPUT_CHANNELS      18
#define AI_OUTPUT_SIZE          (AI_OUTPUT_HEIGHT * AI_OUTPUT_WIDTH * AI_OUTPUT_CHANNELS)

/* 数据类型 */
typedef int8_t  ai_input_t;     /* 输入类型: int8_t (uint8-128) */
typedef int8_t  ai_output_t;    /* 输出类型: int8_t */

/* 量化参数 (输入量化; float 输出不需要) */
#define AI_INPUT_SCALE          1.0f
#define AI_INPUT_ZERO_POINT     (-128)
#define AI_OUTPUT_SCALE         0.14218327403068542f
#define AI_OUTPUT_ZERO_POINT    (-15)

/*===========================================================================*/
/*                        API 接口 - 不要修改                                 */
/*===========================================================================*/

/* 错误码 */
typedef enum {
    AI_OK = 0,
    AI_ERROR_INIT = -1,
    AI_ERROR_RUN = -2,
    AI_ERROR_PARAM = -3,
} ai_status_t;

/* 模型句柄 (内部使用) */
typedef struct ai_handle_s* ai_handle_t;

/**
 * @brief 初始化 AI 运行时
 * @return AI_OK 成功, 其他失败
 */
ai_status_t ai_init(void);

/**
 * @brief 释放 AI 运行时
 */
void ai_deinit(void);

/**
 * @brief 运行推理
 * @param input 输入数据指针 (大小必须为 AI_INPUT_SIZE)
 * @return AI_OK 成功, 其他失败
 */
ai_status_t ai_run(const ai_input_t* input);

/**
 * @brief 获取输出数据指针
 * @return 输出数据指针 (大小为 AI_OUTPUT_SIZE)
 */
const ai_output_t* ai_get_output(void);

/**
 * @brief 获取输出缓冲区大小 (字节)
 */
size_t ai_output_buffer_size(void);

/**
 * @brief 获取模型常量数据大小 (Flash/RO)
 */
size_t ai_model_rodata_size(void);

/**
 * @brief 获取预测类别 (分类模型)
 * @param confidence 可选, 返回置信度
 * @return 预测类别索引
 */
int ai_get_prediction(ai_output_t* confidence);

/**
 * @brief 打印 IREE allocator 统计信息(若启用)
 */
void ai_print_allocator_stats(void);

/*===========================================================================*/
/*                        后处理模板 - 可选使用                               */
/*===========================================================================*/

/**
 * @brief Softmax 后处理 (int8 输出反量化)
 * @param output 输出数据
 * @param probs 概率输出 (float[AI_OUTPUT_SIZE])
 */
void ai_postprocess_softmax(const ai_output_t* output, float* probs);

/**
 * @brief ArgMax 后处理
 * @param output 输出数据
 * @return 最大值索引
 */
int ai_postprocess_argmax(const ai_output_t* output);

#ifdef __cplusplus
}
#endif

#endif /* AI_MODEL_H */
