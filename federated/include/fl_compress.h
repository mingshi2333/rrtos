#ifndef FL_COMPRESS_H
#define FL_COMPRESS_H

#include "fl_types.h"

#if OS_CFG_FL_EN

fl_err_t fl_compress_topk(const float *input, uint32_t input_size,
                          fl_gradient_t *output, float ratio);

fl_err_t fl_compress_qsgd(const float *input, uint32_t input_size,
                          int8_t *output, float *scale, uint8_t bits);

fl_err_t fl_compress_terngrad(const float *input, uint32_t input_size,
                              int8_t *output, float *scale);

fl_err_t fl_decompress_topk(const fl_gradient_t *input, float *output,
                            uint32_t output_size);

fl_err_t fl_decompress_qsgd(const int8_t *input, uint32_t input_size,
                            float scale, float *output);

fl_err_t fl_dp_add_noise(float *gradients, uint32_t count,
                         float clip_norm, float noise_mult);

fl_err_t fl_gradient_clip(float *gradients, uint32_t count, float clip_norm);

float fl_compute_norm(const float *values, uint32_t count);

fl_err_t fl_gradient_alloc(fl_gradient_t *grad, uint32_t capacity);
void fl_gradient_free(fl_gradient_t *grad);
void fl_gradient_clear(fl_gradient_t *grad);

#endif
#endif
