#include "fl_client.h"
#include "fl_compress.h"
#include "os_kernel.h"
#include "os_string.h"
#include "os_math.h"
#include "os_mem.h"
#include "riscv_atomic.h"

#if OS_CFG_FL_EN

typedef struct fl_client_ctx {
    fl_config_t         config;
    fl_state_t          state;
    ai_model_t          *model;
    fl_model_params_t   *params;
    void                *train_data;
    uint32_t            num_samples;
    uint32_t            sample_size;
    uint32_t            label_offset;
    fl_gradient_t       gradients;
    fl_client_stats_t   stats;
    os_mutex_t          lock;
    os_tcb_t            task_tcb;
    uint8_t             task_stack[2048];
    fl_round_cb_t       round_callback;
    void                *callback_arg;
    bool                running;
    bool                initialized;
} fl_client_ctx_t;

static fl_client_ctx_t g_fl_ctx;

static float compute_loss(fl_client_ctx_t *ctx, uint32_t sample_idx) {
    (void)ctx;
    (void)sample_idx;
    return 0.01f;
}

static void compute_gradients(fl_client_ctx_t *ctx, uint32_t sample_idx) {
    (void)sample_idx;
    
    if (!ctx->params || ctx->gradients.count == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < ctx->params->num_weights && i < ctx->gradients.capacity; i++) {
        ctx->gradients.values[i] = 0.001f;
        ctx->gradients.indices[i] = i;
    }
    ctx->gradients.count = ctx->params->num_weights;
}

static void apply_gradients(fl_client_ctx_t *ctx, float learning_rate) {
    if (!ctx->params) return;
    
    for (uint32_t i = 0; i < ctx->gradients.count; i++) {
        uint32_t idx = ctx->gradients.sparse ? ctx->gradients.indices[i] : i;
        if (idx < ctx->params->num_weights) {
            ctx->params->weights[idx] -= learning_rate * ctx->gradients.values[i];
        }
    }
}

static void shuffle_indices(uint32_t *indices, uint32_t count) {
    static uint32_t seed = 42;
    
    for (uint32_t i = count - 1; i > 0; i--) {
        seed = seed * 1103515245 + 12345;
        uint32_t j = (seed >> 16) % (i + 1);
        
        uint32_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }
}

fl_err_t fl_client_init(fl_config_t *config) {
    if (!config) {
        return FL_ERR_INVALID;
    }
    
    memset(&g_fl_ctx, 0, sizeof(g_fl_ctx));
    
    g_fl_ctx.config = *config;
    g_fl_ctx.state = FL_STATE_IDLE;
    g_fl_ctx.running = false;
    
    os_mutex_init(&g_fl_ctx.lock, "fl_client");
    
    uint32_t grad_capacity = 10000;
    fl_err_t err = fl_gradient_alloc(&g_fl_ctx.gradients, grad_capacity);
    if (err != FL_ERR_OK) {
        return err;
    }
    
    memset(&g_fl_ctx.stats, 0, sizeof(g_fl_ctx.stats));
    
    g_fl_ctx.initialized = true;
    
    return FL_ERR_OK;
}

void fl_client_deinit(void) {
    if (!g_fl_ctx.initialized) return;
    
    fl_client_stop();
    fl_gradient_free(&g_fl_ctx.gradients);
    
    g_fl_ctx.initialized = false;
}

fl_err_t fl_client_set_model(ai_model_t *model, fl_model_params_t *params) {
    if (!g_fl_ctx.initialized) {
        return FL_ERR_INVALID;
    }
    
    os_mutex_lock(&g_fl_ctx.lock, OS_WAIT_FOREVER);
    
    g_fl_ctx.model = model;
    g_fl_ctx.params = params;
    
    os_mutex_unlock(&g_fl_ctx.lock);
    
    return FL_ERR_OK;
}

fl_err_t fl_client_set_data(void *train_data, uint32_t num_samples,
                            uint32_t sample_size, uint32_t label_offset) {
    if (!g_fl_ctx.initialized || !train_data) {
        return FL_ERR_INVALID;
    }
    
    os_mutex_lock(&g_fl_ctx.lock, OS_WAIT_FOREVER);
    
    g_fl_ctx.train_data = train_data;
    g_fl_ctx.num_samples = num_samples;
    g_fl_ctx.sample_size = sample_size;
    g_fl_ctx.label_offset = label_offset;
    
    os_mutex_unlock(&g_fl_ctx.lock);
    
    return FL_ERR_OK;
}

fl_err_t fl_client_train_round(fl_round_stats_t *stats) {
    if (!g_fl_ctx.initialized) {
        return FL_ERR_INVALID;
    }
    
    if (g_fl_ctx.state != FL_STATE_IDLE) {
        return FL_ERR_INVALID;
    }
    
    os_mutex_lock(&g_fl_ctx.lock, OS_WAIT_FOREVER);
    
    g_fl_ctx.state = FL_STATE_TRAINING;
    
    fl_round_stats_t local_stats;
    memset(&local_stats, 0, sizeof(local_stats));
    local_stats.round_id = g_fl_ctx.stats.rounds_completed + 1;
    
    uint64_t start_tick = os_tick_get();
    
    uint32_t batch_size = g_fl_ctx.config.batch_size;
    uint32_t num_batches = g_fl_ctx.num_samples / batch_size;
    float total_loss = 0.0f;
    uint32_t correct = 0;
    
    uint32_t *indices = (uint32_t *)os_mem_alloc(g_fl_ctx.num_samples * sizeof(uint32_t));
    if (indices) {
        for (uint32_t i = 0; i < g_fl_ctx.num_samples; i++) {
            indices[i] = i;
        }
        shuffle_indices(indices, g_fl_ctx.num_samples);
    }
    
    for (uint32_t epoch = 0; epoch < g_fl_ctx.config.local_epochs; epoch++) {
        for (uint32_t batch = 0; batch < num_batches; batch++) {
            fl_gradient_clear(&g_fl_ctx.gradients);
            
            for (uint32_t b = 0; b < batch_size; b++) {
                uint32_t sample_idx = batch * batch_size + b;
                if (indices) sample_idx = indices[sample_idx];
                
                float loss = compute_loss(&g_fl_ctx, sample_idx);
                total_loss += loss;
                
                compute_gradients(&g_fl_ctx, sample_idx);
            }
            
            for (uint32_t i = 0; i < g_fl_ctx.gradients.count; i++) {
                g_fl_ctx.gradients.values[i] /= batch_size;
            }
            
            apply_gradients(&g_fl_ctx, g_fl_ctx.config.learning_rate);
            
            local_stats.samples_processed += batch_size;
        }
    }
    
    if (indices) {
        os_mem_free(indices);
    }
    
    uint64_t train_end = os_tick_get();
    local_stats.train_time_ms = (train_end - start_tick) * 1000 / OS_CFG_TICK_FREQ_HZ;
    local_stats.train_loss = total_loss / local_stats.samples_processed;
    local_stats.train_accuracy = (float)correct / local_stats.samples_processed;
    
    g_fl_ctx.state = FL_STATE_COMPRESSING;
    
    uint64_t compress_start = os_tick_get();
    
    if (g_fl_ctx.config.compression == FL_COMPRESS_TOPK) {
        fl_compress_topk(g_fl_ctx.gradients.values, 
                        g_fl_ctx.gradients.count,
                        &g_fl_ctx.gradients,
                        g_fl_ctx.config.topk_ratio);
        local_stats.compression_ratio = g_fl_ctx.config.topk_ratio;
    }
    
    if (g_fl_ctx.config.dp_enabled) {
        float noise_mult = sqrtf(2.0f * logf(1.25f / g_fl_ctx.config.dp_delta)) / 
                          g_fl_ctx.config.dp_epsilon;
        fl_dp_add_noise(g_fl_ctx.gradients.values, 
                       g_fl_ctx.gradients.count,
                       g_fl_ctx.config.dp_clip_norm,
                       noise_mult);
    }
    
    uint64_t compress_end = os_tick_get();
    local_stats.compress_time_ms = (compress_end - compress_start) * 1000 / OS_CFG_TICK_FREQ_HZ;
    
    g_fl_ctx.stats.rounds_completed++;
    g_fl_ctx.stats.total_train_time_ms += local_stats.train_time_ms;
    g_fl_ctx.stats.avg_compression_ratio = 
        (g_fl_ctx.stats.avg_compression_ratio * (g_fl_ctx.stats.rounds_completed - 1) + 
         local_stats.compression_ratio) / g_fl_ctx.stats.rounds_completed;
    
    g_fl_ctx.state = FL_STATE_IDLE;
    
    os_mutex_unlock(&g_fl_ctx.lock);
    
    if (stats) {
        *stats = local_stats;
    }
    
    return FL_ERR_OK;
}

static void fl_async_task(void *arg) {
    fl_client_ctx_t *ctx = (fl_client_ctx_t *)arg;
    
    while (ctx->running) {
        fl_round_stats_t stats;
        fl_err_t err = fl_client_train_round(&stats);
        
        if (ctx->round_callback) {
            ctx->round_callback(&stats, ctx->callback_arg);
        }
        
        if (err != FL_ERR_OK) {
            ctx->stats.rounds_failed++;
        }
        
        os_task_delay(100);
    }
}

fl_err_t fl_client_start_async(fl_round_cb_t callback, void *arg) {
    if (!g_fl_ctx.initialized || g_fl_ctx.running) {
        return FL_ERR_INVALID;
    }
    
    g_fl_ctx.round_callback = callback;
    g_fl_ctx.callback_arg = arg;
    g_fl_ctx.running = true;
    
    os_err_t err = os_task_create(&g_fl_ctx.task_tcb, "fl_client", 
                                   fl_async_task, &g_fl_ctx,
                                   OS_CFG_AI_TASK_PRIO + 1,
                                   g_fl_ctx.task_stack, 
                                   sizeof(g_fl_ctx.task_stack));
    
    if (err != OS_EOK) {
        g_fl_ctx.running = false;
        return FL_ERR_NOMEM;
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_client_stop(void) {
    if (!g_fl_ctx.running) {
        return FL_ERR_OK;
    }
    
    g_fl_ctx.running = false;
    
    os_task_delay(200);
    
    return FL_ERR_OK;
}

fl_state_t fl_client_state(void) {
    return g_fl_ctx.state;
}

fl_err_t fl_client_get_gradients(fl_gradient_t *gradients) {
    if (!gradients) {
        return FL_ERR_INVALID;
    }
    
    *gradients = g_fl_ctx.gradients;
    return FL_ERR_OK;
}

fl_err_t fl_client_apply_update(fl_model_params_t *update) {
    if (!update || !g_fl_ctx.params) {
        return FL_ERR_INVALID;
    }
    
    os_mutex_lock(&g_fl_ctx.lock, OS_WAIT_FOREVER);
    
    g_fl_ctx.state = FL_STATE_UPDATING;
    
    uint32_t count = (update->num_weights < g_fl_ctx.params->num_weights) ?
                     update->num_weights : g_fl_ctx.params->num_weights;
    
    for (uint32_t i = 0; i < count; i++) {
        g_fl_ctx.params->weights[i] = update->weights[i];
    }
    
    g_fl_ctx.params->version = update->version;
    g_fl_ctx.params->checksum = update->checksum;
    
    g_fl_ctx.state = FL_STATE_IDLE;
    
    os_mutex_unlock(&g_fl_ctx.lock);
    
    return FL_ERR_OK;
}

fl_err_t fl_client_get_stats(fl_client_stats_t *stats) {
    if (!stats) {
        return FL_ERR_INVALID;
    }
    *stats = g_fl_ctx.stats;
    return FL_ERR_OK;
}

void fl_client_reset_stats(void) {
    memset(&g_fl_ctx.stats, 0, sizeof(g_fl_ctx.stats));
}

#endif
