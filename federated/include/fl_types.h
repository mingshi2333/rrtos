#ifndef FL_TYPES_H
#define FL_TYPES_H

#include "os_types.h"

#if OS_CFG_FL_EN

typedef enum {
    FL_ERR_OK           = 0,
    FL_ERR_INVALID      = -1,
    FL_ERR_NOMEM        = -2,
    FL_ERR_NETWORK      = -3,
    FL_ERR_TIMEOUT      = -4,
    FL_ERR_VERSION      = -5,
    FL_ERR_CHECKSUM     = -6,
    FL_ERR_ABORTED      = -7,
} fl_err_t;

typedef enum {
    FL_STATE_IDLE       = 0,
    FL_STATE_TRAINING   = 1,
    FL_STATE_COMPRESSING= 2,
    FL_STATE_SENDING    = 3,
    FL_STATE_RECEIVING  = 4,
    FL_STATE_UPDATING   = 5,
} fl_state_t;

typedef enum {
    FL_COMPRESS_NONE    = 0,
    FL_COMPRESS_TOPK    = 1,
    FL_COMPRESS_QSGD    = 2,
    FL_COMPRESS_TERNGRAD= 3,
} fl_compress_t;

typedef struct fl_config {
    uint32_t        batch_size;
    float           learning_rate;
    uint32_t        local_epochs;
    fl_compress_t   compression;
    float           topk_ratio;
    bool            dp_enabled;
    float           dp_epsilon;
    float           dp_delta;
    float           dp_clip_norm;
    uint32_t        round_timeout_ms;
    const char      *server_addr;
    uint16_t        server_port;
} fl_config_t;

typedef struct fl_gradient {
    float           *values;
    uint32_t        *indices;
    uint32_t        count;
    uint32_t        capacity;
    uint32_t        original_size;
    bool            sparse;
} fl_gradient_t;

typedef struct fl_model_params {
    float           *weights;
    uint32_t        num_weights;
    uint32_t        version;
    uint32_t        checksum;
} fl_model_params_t;

typedef struct fl_round_stats {
    uint32_t        round_id;
    uint32_t        samples_processed;
    float           train_loss;
    float           train_accuracy;
    uint64_t        train_time_ms;
    uint64_t        compress_time_ms;
    uint64_t        comm_time_ms;
    float           compression_ratio;
    uint32_t        bytes_sent;
    uint32_t        bytes_received;
} fl_round_stats_t;

typedef struct fl_client_stats {
    uint32_t        rounds_completed;
    uint32_t        rounds_failed;
    uint64_t        total_train_time_ms;
    uint64_t        total_comm_time_ms;
    uint32_t        total_bytes_sent;
    uint32_t        total_bytes_received;
    float           avg_compression_ratio;
} fl_client_stats_t;

typedef void (*fl_round_cb_t)(fl_round_stats_t *stats, void *arg);
typedef void (*fl_update_cb_t)(fl_model_params_t *new_params, void *arg);

#endif
#endif
