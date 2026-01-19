#ifndef AI_RUNTIME_H
#define AI_RUNTIME_H

#include "os_types.h"

#if OS_CFG_AI_EN

typedef enum {
    AI_DTYPE_FLOAT32 = 0,
    AI_DTYPE_INT8    = 1,
    AI_DTYPE_UINT8   = 2,
    AI_DTYPE_INT16   = 3,
    AI_DTYPE_INT32   = 4,
} ai_dtype_t;

typedef enum {
    AI_STATUS_IDLE      = 0,
    AI_STATUS_LOADING   = 1,
    AI_STATUS_READY     = 2,
    AI_STATUS_RUNNING   = 3,
    AI_STATUS_ERROR     = 4,
} ai_status_t;

typedef enum {
    AI_ERR_OK           = 0,
    AI_ERR_INVALID      = -1,
    AI_ERR_NOMEM        = -2,
    AI_ERR_FORMAT       = -3,
    AI_ERR_VERSION      = -4,
    AI_ERR_OP_NOT_FOUND = -5,
    AI_ERR_INVOKE       = -6,
    AI_ERR_TIMEOUT      = -7,
    AI_ERR_INIT_FAILED  = -8,
    AI_ERR_INTERNAL     = -9,
} ai_err_t;

#define AI_MAX_DIMS         6
#define AI_MAX_TENSORS      64
#define AI_MAX_OPS          128

typedef struct ai_tensor {
    void            *data;
    int32_t         dims[AI_MAX_DIMS];
    uint8_t         num_dims;
    ai_dtype_t      dtype;
    float           scale;
    int32_t         zero_point;
    uint32_t        bytes;
} ai_tensor_t;

typedef struct ai_model_info {
    const char      *name;
    uint32_t        version;
    uint32_t        num_inputs;
    uint32_t        num_outputs;
    uint32_t        arena_size;
    uint32_t        flash_size;
    bool            quantized;
} ai_model_info_t;

typedef struct ai_model ai_model_t;

typedef struct ai_invoke_config {
    uint32_t        timeout_ms;
    uint8_t         priority;
    bool            preemptible;
    void            (*progress_cb)(uint32_t layer, uint32_t total, void *arg);
    void            *cb_arg;
} ai_invoke_config_t;

typedef struct ai_perf_stats {
    uint64_t        invoke_count;
    uint64_t        total_time_us;
    uint64_t        min_time_us;
    uint64_t        max_time_us;
    uint32_t        preemptions;
} ai_perf_stats_t;

ai_err_t ai_runtime_init(void *arena, uint32_t arena_size);
void ai_runtime_deinit(void);
uint32_t ai_runtime_get_free_memory(void);

ai_err_t ai_model_load(ai_model_t **model, const void *model_data, uint32_t size);
ai_err_t ai_model_load_from_flash(ai_model_t **model, const void *flash_addr);
void ai_model_unload(ai_model_t *model);
ai_err_t ai_model_get_info(ai_model_t *model, ai_model_info_t *info);

ai_tensor_t *ai_model_input(ai_model_t *model, uint32_t index);
ai_tensor_t *ai_model_output(ai_model_t *model, uint32_t index);

ai_err_t ai_model_invoke(ai_model_t *model);
ai_err_t ai_model_invoke_async(ai_model_t *model, ai_invoke_config_t *config,
                                void (*callback)(ai_err_t result, void *arg), void *arg);
ai_status_t ai_model_status(ai_model_t *model);
ai_err_t ai_model_abort(ai_model_t *model);

ai_err_t ai_model_get_perf(ai_model_t *model, ai_perf_stats_t *stats);
void ai_model_reset_perf(ai_model_t *model);

void ai_quantize_float_to_int8(const float *input, int8_t *output, 
                                uint32_t count, float scale, int32_t zero_point);
void ai_dequantize_int8_to_float(const int8_t *input, float *output,
                                  uint32_t count, float scale, int32_t zero_point);

ai_err_t ai_op_conv2d_int8(const int8_t *input, const int8_t *weights,
                           const int32_t *bias, int8_t *output,
                           int32_t in_h, int32_t in_w, int32_t in_c,
                           int32_t out_c, int32_t k_h, int32_t k_w,
                           int32_t stride, int32_t pad,
                           int32_t input_offset, int32_t weight_offset,
                           int32_t output_offset, int32_t output_mult,
                           int32_t output_shift);

ai_err_t ai_op_depthwise_conv2d_int8(const int8_t *input, const int8_t *weights,
                                      const int32_t *bias, int8_t *output,
                                      int32_t in_h, int32_t in_w, int32_t channels,
                                      int32_t k_h, int32_t k_w,
                                      int32_t stride, int32_t pad,
                                      int32_t input_offset, int32_t weight_offset,
                                      int32_t output_offset, int32_t output_mult,
                                      int32_t output_shift);

ai_err_t ai_op_fully_connected_int8(const int8_t *input, const int8_t *weights,
                                     const int32_t *bias, int8_t *output,
                                     int32_t in_features, int32_t out_features,
                                     int32_t input_offset, int32_t weight_offset,
                                     int32_t output_offset, int32_t output_mult,
                                     int32_t output_shift);

ai_err_t ai_op_pooling_int8(const int8_t *input, int8_t *output,
                            int32_t in_h, int32_t in_w, int32_t channels,
                            int32_t pool_h, int32_t pool_w, int32_t stride,
                            bool is_max_pool);

ai_err_t ai_op_relu_int8(int8_t *data, uint32_t count, int32_t zero_point);

ai_err_t ai_op_softmax_int8(const int8_t *input, int8_t *output, uint32_t count,
                            int32_t input_mult, int32_t input_shift,
                            int32_t diff_min);

#endif
#endif
