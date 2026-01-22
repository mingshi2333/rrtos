/**
 * @file ai_model.h
 * @brief AI Model Configuration - STM32CubeAI Style
 * 
 * Usage:
 * 1. Modify MODEL_* macros to match your model
 * 2. Link corresponding .o files (emitc or bytecode generated)
 * 3. Call ai_init() / ai_run() / ai_get_output() in main.c
 * 
 * To switch models, just:
 * - Modify the configuration in this header file
 * - Change the linked .o file
 */

#ifndef AI_MODEL_H
#define AI_MODEL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/*                        User Configuration - Modify here                               */
/*===========================================================================*/

/* Model name (for logging) */
#define AI_MODEL_NAME           "yoloface_int8"

/* Input configuration */
#define AI_INPUT_HEIGHT         56
#define AI_INPUT_WIDTH          56
#define AI_INPUT_CHANNELS       3
#define AI_INPUT_SIZE           (AI_INPUT_HEIGHT * AI_INPUT_WIDTH * AI_INPUT_CHANNELS)

/* Output configuration - YOLO detection (7x7x18) */
#define AI_OUTPUT_HEIGHT        7
#define AI_OUTPUT_WIDTH         7
#define AI_OUTPUT_CHANNELS      18
#define AI_OUTPUT_SIZE          (AI_OUTPUT_HEIGHT * AI_OUTPUT_WIDTH * AI_OUTPUT_CHANNELS)

/* Data type */
typedef int8_t  ai_input_t;     /* Input type: int8_t (uint8-128) */
typedef int8_t  ai_output_t;    /* Output type: int8_t */

/* Quantization parameters (input quantization; not needed for float output) */
#define AI_INPUT_SCALE          1.0f
#define AI_INPUT_ZERO_POINT     (-128)
#define AI_OUTPUT_SCALE         0.14218327403068542f
#define AI_OUTPUT_ZERO_POINT    (-15)

/*===========================================================================*/
/*                        API Interface - Do not modify                                 */
/*===========================================================================*/

/* Error codes */
typedef enum {
    AI_OK = 0,
    AI_ERROR_INIT = -1,
    AI_ERROR_RUN = -2,
    AI_ERROR_PARAM = -3,
} ai_status_t;

/* Model handle (internal use) */
typedef struct ai_handle_s* ai_handle_t;

/**
 * @brief Initialize AI runtime
 * @return AI_OK success, others failed
 */
ai_status_t ai_init(void);

/**
 * @brief Release AI runtime
 */
void ai_deinit(void);

/**
 * @brief Run inference
 * @param input Input data pointer (size must be AI_INPUT_SIZE)
 * @return AI_OK success, others failed
 */
ai_status_t ai_run(const ai_input_t* input);

/**
 * @brief Get output data pointer
 * @return Output data pointer (size is AI_OUTPUT_SIZE)
 */
const ai_output_t* ai_get_output(void);

/**
 * @brief Get output buffer size (bytes)
 */
size_t ai_output_buffer_size(void);

/**
 * @brief Get model constant data size (Flash/RO)
 */
size_t ai_model_rodata_size(void);

/**
 * @brief Get predicted class (classification model)
 * @param confidence Optional, returns confidence
 * @return Predicted class index
 */
int ai_get_prediction(ai_output_t* confidence);

/**
 * @brief Print IREE allocator statistics (if enabled)
 */
void ai_print_allocator_stats(void);

/*===========================================================================*/
/*                        Post-processing templates - Optional use                               */
/*===========================================================================*/

/**
 * @brief Softmax post-processing (int8 output dequantization)
 * @param output Output data
 * @param probs Probability output (float[AI_OUTPUT_SIZE])
 */
void ai_postprocess_softmax(const ai_output_t* output, float* probs);

/**
 * @brief ArgMax post-processing
 * @param output Output data
 * @return Maximum index
 */
int ai_postprocess_argmax(const ai_output_t* output);

#ifdef __cplusplus
}
#endif

#endif /* AI_MODEL_H */
