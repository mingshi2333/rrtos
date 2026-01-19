#ifndef FL_CLIENT_H
#define FL_CLIENT_H

#include "fl_types.h"
#include "ai_runtime.h"

#if OS_CFG_FL_EN

fl_err_t fl_client_init(fl_config_t *config);
void fl_client_deinit(void);

fl_err_t fl_client_set_model(ai_model_t *model, fl_model_params_t *params);
fl_err_t fl_client_set_data(void *train_data, uint32_t num_samples,
                            uint32_t sample_size, uint32_t label_offset);

fl_err_t fl_client_train_round(fl_round_stats_t *stats);
fl_err_t fl_client_start_async(fl_round_cb_t callback, void *arg);
fl_err_t fl_client_stop(void);
fl_state_t fl_client_state(void);

fl_err_t fl_client_get_gradients(fl_gradient_t *gradients);
fl_err_t fl_client_apply_update(fl_model_params_t *update);

fl_err_t fl_client_get_stats(fl_client_stats_t *stats);
void fl_client_reset_stats(void);

#endif
#endif
