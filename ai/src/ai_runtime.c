#include "ai_runtime.h"
#include "ai_backend.h"
#include "os_kernel.h"
#include "riscv_atomic.h"
#include <stdio.h>
#include <string.h>

#if OS_CFG_AI_EN

typedef struct ai_arena {
    uint8_t         *base;
    uint32_t        size;
    uint32_t        used;
    uint32_t        peak;
    os_spinlock_t   lock;
} ai_arena_t;

typedef struct ai_arena_block_header {
    uint32_t size;
} ai_arena_block_header_t;

typedef enum {
    AI_OP_CONV2D = 0,
    AI_OP_DEPTHWISE_CONV2D,
    AI_OP_FULLY_CONNECTED,
    AI_OP_POOLING,
    AI_OP_RELU,
    AI_OP_RELU6,
    AI_OP_SOFTMAX,
    AI_OP_ADD,
    AI_OP_MUL,
    AI_OP_RESHAPE,
    AI_OP_PAD,
    AI_OP_MAX,
} ai_op_type_t;

typedef struct ai_op {
    ai_op_type_t    type;
    uint8_t         input_indices[4];
    uint8_t         output_index;
    uint8_t         num_inputs;
    void            *params;
    uint16_t        params_size;
} ai_op_t;

struct ai_model {
    ai_tensor_t     tensors[AI_MAX_TENSORS];
    ai_op_t         ops[AI_MAX_OPS];
    uint8_t         num_tensors;
    uint8_t         num_ops;
    uint8_t         input_indices[8];
    uint8_t         output_indices[8];
    uint8_t         num_inputs;
    uint8_t         num_outputs;
    ai_status_t     status;
    uint32_t        current_op;
    ai_perf_stats_t perf;
    os_mutex_t      lock;
    const char      *name;
    uint32_t        version;
    bool            quantized;
};

static ai_arena_t g_arena;
static ai_model_t *g_models[OS_CFG_AI_MODEL_MAX];
static uint8_t g_model_count;
static bool g_initialized = false;
static const ai_backend_ops_t *g_backend;

static void *arena_alloc(uint32_t size) {
    size = OS_ALIGN_UP(size, 8);
    
    os_spinlock_lock(&g_arena.lock);
    
    if (g_arena.used + size > g_arena.size) {
        os_spinlock_unlock(&g_arena.lock);
        return NULL;
    }
    
    void *ptr = g_arena.base + g_arena.used;
    g_arena.used += size;
    if (g_arena.used > g_arena.peak) {
        g_arena.peak = g_arena.used;
    }
    
    os_spinlock_unlock(&g_arena.lock);
    
    return ptr;
}

static void arena_reset(void) {
    os_spinlock_lock(&g_arena.lock);
    g_arena.used = 0;
    g_arena.peak = 0;
    os_spinlock_unlock(&g_arena.lock);
}

static void *arena_alloc_with_header(uint32_t size, bool zero) {
    if (size == 0) {
        return NULL;
    }

    uint32_t aligned_size = OS_ALIGN_UP(size, 8);
    uint32_t header_size = OS_ALIGN_UP(sizeof(ai_arena_block_header_t), 8);

    os_spinlock_lock(&g_arena.lock);

    uint32_t offset = OS_ALIGN_UP(g_arena.used, 8);
    if (offset + header_size + aligned_size > g_arena.size) {
        printf("[AI] Arena OOM: req=%u used=%u total=%u\n",
               aligned_size, g_arena.used, g_arena.size);
        os_spinlock_unlock(&g_arena.lock);
        return NULL;
    }

    ai_arena_block_header_t *header = (ai_arena_block_header_t *)(g_arena.base + offset);
    header->size = aligned_size;

    uint8_t *ptr = (uint8_t *)header + header_size;
    g_arena.used = offset + header_size + aligned_size;
    if (g_arena.used > g_arena.peak) {
        g_arena.peak = g_arena.used;
    }

    if (zero) {
        memset(ptr, 0, aligned_size);
    }

    os_spinlock_unlock(&g_arena.lock);

    return ptr;
}

void *ai_runtime_arena_malloc(uint32_t size) {
    return arena_alloc_with_header(size, false);
}

void *ai_runtime_arena_calloc(uint32_t size) {
    return arena_alloc_with_header(size, true);
}

void *ai_runtime_arena_realloc(void *ptr, uint32_t size) {
    if (!ptr) {
        return ai_runtime_arena_malloc(size);
    }
    if (size == 0) {
        ai_runtime_arena_free(ptr);
        return NULL;
    }

    uint32_t header_size = OS_ALIGN_UP(sizeof(ai_arena_block_header_t), 8);
    ai_arena_block_header_t *header =
        (ai_arena_block_header_t *)((uint8_t *)ptr - header_size);
    uint32_t old_size = header->size;

    uint32_t aligned_size = OS_ALIGN_UP(size, 8);
    if (aligned_size <= old_size) {
        header->size = aligned_size;
        return ptr;
    }

    void *new_ptr = ai_runtime_arena_malloc(size);
    if (!new_ptr) {
        return NULL;
    }

    memcpy(new_ptr, ptr, old_size);
    return new_ptr;
}

void ai_runtime_arena_free(void *ptr) {
    (void)ptr;
    // No-op: arena allocations are reclaimed in arena_reset.
}

static inline int32_t saturate_int8(int32_t val) {
    if (val > 127) return 127;
    if (val < -128) return -128;
    return val;
}

static inline int32_t multiply_by_quantized_mult(int32_t x, int32_t mult, int32_t shift) {
    int64_t result = (int64_t)x * mult;
    
    if (shift > 0) {
        result = result >> shift;
    } else if (shift < 0) {
        result = result << (-shift);
    }
    
    return (int32_t)result;
}

ai_err_t ai_runtime_init(void *arena, uint32_t arena_size) {
    if (!arena || arena_size < 1024) {
        return AI_ERR_INVALID;
    }

    g_arena.base = (uint8_t *)arena;
    g_arena.size = arena_size;
    g_arena.used = 0;
    g_arena.peak = 0;
    os_spinlock_init(&g_arena.lock);

    for (int i = 0; i < OS_CFG_AI_MODEL_MAX; i++) {
        g_models[i] = NULL;
    }
    g_model_count = 0;

    g_backend = NULL;
#if OS_CFG_AI_BACKEND_IREE
    g_backend = ai_backend_iree_ops();
    if (!g_backend || !g_backend->init) {
        return AI_ERR_OP_NOT_FOUND;
    }
    ai_err_t err = g_backend->init(arena, arena_size);
    if (err != AI_ERR_OK) {
        return err;
    }
#endif

    g_initialized = true;

    return AI_ERR_OK;
}

void ai_runtime_deinit(void) {
    for (int i = 0; i < OS_CFG_AI_MODEL_MAX; i++) {
        if (g_models[i]) {
            ai_model_unload(g_models[i]);
        }
    }

    if (g_backend && g_backend->deinit) {
        g_backend->deinit();
    }

    arena_reset();
    g_initialized = false;
}

uint32_t ai_runtime_get_free_memory(void) {
    return g_arena.size - g_arena.used;
}

uint32_t ai_runtime_get_peak_memory(void) {
    return g_arena.peak;
}

void ai_runtime_reset_peak(void) {
    g_arena.peak = g_arena.used;
}

ai_err_t ai_model_load(ai_model_t **model, const void *model_data, uint32_t size) {
    if (g_backend && g_backend->load) {
        return g_backend->load(model, model_data, size);
    }

    if (!g_initialized || !model || !model_data || size < 16) {
        return AI_ERR_INVALID;
    }
    
    if (g_model_count >= OS_CFG_AI_MODEL_MAX) {
        return AI_ERR_NOMEM;
    }
    
    const uint8_t *data = (const uint8_t *)model_data;
    
    if (data[0] != 'T' || data[1] != 'F' || data[2] != 'L' || data[3] != '3') {
        return AI_ERR_FORMAT;
    }
    
    ai_model_t *m = (ai_model_t *)arena_alloc(sizeof(ai_model_t));
    if (!m) {
        return AI_ERR_NOMEM;
    }
    
    m->status = AI_STATUS_LOADING;
    m->num_tensors = 0;
    m->num_ops = 0;
    m->num_inputs = 0;
    m->num_outputs = 0;
    m->current_op = 0;
    m->quantized = true;
    m->name = "model";
    m->version = 1;
    
    m->perf.invoke_count = 0;
    m->perf.total_time_us = 0;
    m->perf.min_time_us = UINT64_MAX;
    m->perf.max_time_us = 0;
    m->perf.preemptions = 0;
    
    os_mutex_init(&m->lock, "ai_model");
    
    m->status = AI_STATUS_READY;
    
    for (int i = 0; i < OS_CFG_AI_MODEL_MAX; i++) {
        if (!g_models[i]) {
            g_models[i] = m;
            g_model_count++;
            break;
        }
    }
    
    *model = m;
    return AI_ERR_OK;
}

void ai_model_unload(ai_model_t *model) {
    if (!model) return;
    
    model->status = AI_STATUS_IDLE;
    
    for (int i = 0; i < OS_CFG_AI_MODEL_MAX; i++) {
        if (g_models[i] == model) {
            g_models[i] = NULL;
            g_model_count--;
            break;
        }
    }
}

ai_err_t ai_model_get_info(ai_model_t *model, ai_model_info_t *info) {
    if (!model || !info) {
        return AI_ERR_INVALID;
    }
    
    info->name = model->name;
    info->version = model->version;
    info->num_inputs = model->num_inputs;
    info->num_outputs = model->num_outputs;
    info->quantized = model->quantized;
    info->arena_size = g_arena.used;
    info->flash_size = 0;
    
    return AI_ERR_OK;
}

ai_tensor_t *ai_model_input(ai_model_t *model, uint32_t index) {
    if (!model || index >= model->num_inputs) {
        return NULL;
    }
    return &model->tensors[model->input_indices[index]];
}

ai_tensor_t *ai_model_output(ai_model_t *model, uint32_t index) {
    if (!model || index >= model->num_outputs) {
        return NULL;
    }
    return &model->tensors[model->output_indices[index]];
}

static ai_err_t execute_op(ai_model_t *model, ai_op_t *op) {
    (void)model;
    
    switch (op->type) {
        case AI_OP_CONV2D:
        case AI_OP_DEPTHWISE_CONV2D:
        case AI_OP_FULLY_CONNECTED:
        case AI_OP_POOLING:
        case AI_OP_RELU:
        case AI_OP_RELU6:
        case AI_OP_SOFTMAX:
        case AI_OP_ADD:
        case AI_OP_MUL:
        case AI_OP_RESHAPE:
        case AI_OP_PAD:
            break;
        default:
            return AI_ERR_OP_NOT_FOUND;
    }
    
    return AI_ERR_OK;
}

ai_err_t ai_model_invoke(ai_model_t *model) {
    if (!model) {
        return AI_ERR_INVALID;
    }

    if (g_backend && g_backend->invoke) {
        return g_backend->invoke(model);
    }
    
    if (model->status != AI_STATUS_READY) {
        return AI_ERR_INVALID;
    }
    
    os_mutex_lock(&model->lock, OS_WAIT_FOREVER);
    
    model->status = AI_STATUS_RUNNING;
    model->current_op = 0;
    
    uint64_t start_tick = os_tick_get();
    
    for (uint32_t i = 0; i < model->num_ops; i++) {
        model->current_op = i;
        
        ai_err_t err = execute_op(model, &model->ops[i]);
        if (err != AI_ERR_OK) {
            model->status = AI_STATUS_ERROR;
            os_mutex_unlock(&model->lock);
            return err;
        }
    }
    
    uint64_t end_tick = os_tick_get();
    uint64_t elapsed_us = (end_tick - start_tick) * 1000000 / OS_CFG_TICK_FREQ_HZ;
    
    model->perf.invoke_count++;
    model->perf.total_time_us += elapsed_us;
    if (elapsed_us < model->perf.min_time_us) {
        model->perf.min_time_us = elapsed_us;
    }
    if (elapsed_us > model->perf.max_time_us) {
        model->perf.max_time_us = elapsed_us;
    }
    
    model->status = AI_STATUS_READY;
    
    os_mutex_unlock(&model->lock);
    
    return AI_ERR_OK;
}

ai_status_t ai_model_status(ai_model_t *model) {
    if (!model) {
        return AI_STATUS_ERROR;
    }
    return model->status;
}

ai_err_t ai_model_get_perf(ai_model_t *model, ai_perf_stats_t *stats) {
    if (!model || !stats) {
        return AI_ERR_INVALID;
    }
    *stats = model->perf;
    return AI_ERR_OK;
}

void ai_quantize_float_to_int8(const float *input, int8_t *output, 
                                uint32_t count, float scale, int32_t zero_point) {
    for (uint32_t i = 0; i < count; i++) {
        int32_t val = (int32_t)(input[i] / scale) + zero_point;
        output[i] = (int8_t)saturate_int8(val);
    }
}

void ai_dequantize_int8_to_float(const int8_t *input, float *output,
                                  uint32_t count, float scale, int32_t zero_point) {
    for (uint32_t i = 0; i < count; i++) {
        output[i] = ((float)input[i] - zero_point) * scale;
    }
}

ai_err_t ai_op_conv2d_int8(const int8_t *input, const int8_t *weights,
                           const int32_t *bias, int8_t *output,
                           int32_t in_h, int32_t in_w, int32_t in_c,
                           int32_t out_c, int32_t k_h, int32_t k_w,
                           int32_t stride, int32_t pad,
                           int32_t input_offset, int32_t weight_offset,
                           int32_t output_offset, int32_t output_mult,
                           int32_t output_shift) {
    if (!input || !weights || !output) {
        return AI_ERR_INVALID;
    }
    
    int32_t out_h = (in_h + 2 * pad - k_h) / stride + 1;
    int32_t out_w = (in_w + 2 * pad - k_w) / stride + 1;
    
    for (int32_t oc = 0; oc < out_c; oc++) {
        for (int32_t oh = 0; oh < out_h; oh++) {
            for (int32_t ow = 0; ow < out_w; ow++) {
                int32_t acc = bias ? bias[oc] : 0;
                
                for (int32_t kh = 0; kh < k_h; kh++) {
                    for (int32_t kw = 0; kw < k_w; kw++) {
                        for (int32_t ic = 0; ic < in_c; ic++) {
                            int32_t ih = oh * stride + kh - pad;
                            int32_t iw = ow * stride + kw - pad;
                            
                            int32_t in_val = 0;
                            if (ih >= 0 && ih < in_h && iw >= 0 && iw < in_w) {
                                in_val = input[(ih * in_w + iw) * in_c + ic] + input_offset;
                            }
                            
                            int32_t w_idx = ((oc * k_h + kh) * k_w + kw) * in_c + ic;
                            int32_t w_val = weights[w_idx] + weight_offset;
                            
                            acc += in_val * w_val;
                        }
                    }
                }
                
                acc = multiply_by_quantized_mult(acc, output_mult, output_shift);
                acc += output_offset;
                
                output[(oh * out_w + ow) * out_c + oc] = (int8_t)saturate_int8(acc);
            }
        }
    }
    
    return AI_ERR_OK;
}

ai_err_t ai_op_fully_connected_int8(const int8_t *input, const int8_t *weights,
                                     const int32_t *bias, int8_t *output,
                                     int32_t in_features, int32_t out_features,
                                     int32_t input_offset, int32_t weight_offset,
                                     int32_t output_offset, int32_t output_mult,
                                     int32_t output_shift) {
    if (!input || !weights || !output) {
        return AI_ERR_INVALID;
    }
    
    for (int32_t o = 0; o < out_features; o++) {
        int32_t acc = bias ? bias[o] : 0;
        
        for (int32_t i = 0; i < in_features; i++) {
            int32_t in_val = input[i] + input_offset;
            int32_t w_val = weights[o * in_features + i] + weight_offset;
            acc += in_val * w_val;
        }
        
        acc = multiply_by_quantized_mult(acc, output_mult, output_shift);
        acc += output_offset;
        
        output[o] = (int8_t)saturate_int8(acc);
    }
    
    return AI_ERR_OK;
}

ai_err_t ai_op_relu_int8(int8_t *data, uint32_t count, int32_t zero_point) {
    if (!data) {
        return AI_ERR_INVALID;
    }
    
    int8_t min_val = (int8_t)zero_point;
    
    for (uint32_t i = 0; i < count; i++) {
        if (data[i] < min_val) {
            data[i] = min_val;
        }
    }
    
    return AI_ERR_OK;
}

ai_err_t ai_op_pooling_int8(const int8_t *input, int8_t *output,
                            int32_t in_h, int32_t in_w, int32_t channels,
                            int32_t pool_h, int32_t pool_w, int32_t stride,
                            bool is_max_pool) {
    if (!input || !output) {
        return AI_ERR_INVALID;
    }
    
    int32_t out_h = (in_h - pool_h) / stride + 1;
    int32_t out_w = (in_w - pool_w) / stride + 1;
    
    for (int32_t c = 0; c < channels; c++) {
        for (int32_t oh = 0; oh < out_h; oh++) {
            for (int32_t ow = 0; ow < out_w; ow++) {
                int32_t result;
                
                if (is_max_pool) {
                    result = -128;
                } else {
                    result = 0;
                }
                
                for (int32_t ph = 0; ph < pool_h; ph++) {
                    for (int32_t pw = 0; pw < pool_w; pw++) {
                        int32_t ih = oh * stride + ph;
                        int32_t iw = ow * stride + pw;
                        int32_t val = input[(ih * in_w + iw) * channels + c];
                        
                        if (is_max_pool) {
                            if (val > result) result = val;
                        } else {
                            result += val;
                        }
                    }
                }
                
                if (!is_max_pool) {
                    result /= (pool_h * pool_w);
                }
                
                output[(oh * out_w + ow) * channels + c] = (int8_t)result;
            }
        }
    }
    
    return AI_ERR_OK;
}

#endif
