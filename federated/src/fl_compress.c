#include "fl_compress.h"
#include "os_kernel.h"
#include "os_math.h"
#include "os_mem.h"

#if OS_CFG_FL_EN

static uint32_t g_rand_state = 12345;

static float rand_uniform(void) {
    g_rand_state = g_rand_state * 1103515245 + 12345;
    return (float)(g_rand_state & 0x7FFFFFFF) / (float)0x7FFFFFFF;
}

static float rand_gaussian(void) {
    float u1 = rand_uniform();
    float u2 = rand_uniform();
    
    if (u1 < 1e-10f) u1 = 1e-10f;
    
    return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * 3.14159265f * u2);
}

static void quickselect_partition(float *values, uint32_t *indices, 
                                   int32_t left, int32_t right, int32_t k) {
    while (left < right) {
        float pivot = values[right];
        int32_t i = left;
        
        for (int32_t j = left; j < right; j++) {
            if (fabsf(values[j]) > fabsf(pivot)) {
                float tmp_v = values[i];
                values[i] = values[j];
                values[j] = tmp_v;
                
                uint32_t tmp_i = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp_i;
                
                i++;
            }
        }
        
        float tmp_v = values[i];
        values[i] = values[right];
        values[right] = tmp_v;
        
        uint32_t tmp_i = indices[i];
        indices[i] = indices[right];
        indices[right] = tmp_i;
        
        if (i == k) {
            return;
        } else if (i < k) {
            left = i + 1;
        } else {
            right = i - 1;
        }
    }
}

fl_err_t fl_compress_topk(const float *input, uint32_t input_size,
                          fl_gradient_t *output, float ratio) {
    if (!input || !output || ratio <= 0.0f || ratio > 1.0f) {
        return FL_ERR_INVALID;
    }
    
    uint32_t k = (uint32_t)(input_size * ratio);
    if (k == 0) k = 1;
    if (k > output->capacity) k = output->capacity;
    
    float *temp_values = output->values;
    uint32_t *temp_indices = output->indices;
    
    for (uint32_t i = 0; i < input_size && i < output->capacity; i++) {
        temp_values[i] = input[i];
        temp_indices[i] = i;
    }
    
    uint32_t work_size = (input_size < output->capacity) ? input_size : output->capacity;
    
    if (k < work_size) {
        quickselect_partition(temp_values, temp_indices, 0, work_size - 1, k - 1);
    }
    
    output->count = k;
    output->original_size = input_size;
    output->sparse = true;
    
    return FL_ERR_OK;
}

fl_err_t fl_compress_qsgd(const float *input, uint32_t input_size,
                          int8_t *output, float *scale, uint8_t bits) {
    if (!input || !output || !scale || bits == 0 || bits > 8) {
        return FL_ERR_INVALID;
    }
    
    float max_val = 0.0f;
    for (uint32_t i = 0; i < input_size; i++) {
        float abs_val = fabsf(input[i]);
        if (abs_val > max_val) max_val = abs_val;
    }
    
    if (max_val < 1e-10f) {
        max_val = 1e-10f;
    }
    
    int32_t levels = (1 << bits) - 1;
    *scale = max_val / levels;
    
    for (uint32_t i = 0; i < input_size; i++) {
        float normalized = input[i] / max_val;
        
        int32_t quantized = (int32_t)(normalized * levels + 0.5f);
        if (quantized > levels) quantized = levels;
        if (quantized < -levels) quantized = -levels;
        
        output[i] = (int8_t)quantized;
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_compress_terngrad(const float *input, uint32_t input_size,
                              int8_t *output, float *scale) {
    if (!input || !output || !scale) {
        return FL_ERR_INVALID;
    }
    
    float max_val = 0.0f;
    for (uint32_t i = 0; i < input_size; i++) {
        float abs_val = fabsf(input[i]);
        if (abs_val > max_val) max_val = abs_val;
    }
    
    *scale = max_val;
    
    if (max_val < 1e-10f) {
        for (uint32_t i = 0; i < input_size; i++) {
            output[i] = 0;
        }
        return FL_ERR_OK;
    }
    
    for (uint32_t i = 0; i < input_size; i++) {
        float prob = fabsf(input[i]) / max_val;
        float r = rand_uniform();
        
        if (r < prob) {
            output[i] = (input[i] >= 0) ? 1 : -1;
        } else {
            output[i] = 0;
        }
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_decompress_topk(const fl_gradient_t *input, float *output,
                            uint32_t output_size) {
    if (!input || !output) {
        return FL_ERR_INVALID;
    }
    
    for (uint32_t i = 0; i < output_size; i++) {
        output[i] = 0.0f;
    }
    
    for (uint32_t i = 0; i < input->count; i++) {
        uint32_t idx = input->indices[i];
        if (idx < output_size) {
            output[idx] = input->values[i];
        }
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_decompress_qsgd(const int8_t *input, uint32_t input_size,
                            float scale, float *output) {
    if (!input || !output) {
        return FL_ERR_INVALID;
    }
    
    for (uint32_t i = 0; i < input_size; i++) {
        output[i] = (float)input[i] * scale;
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_dp_add_noise(float *gradients, uint32_t count,
                         float clip_norm, float noise_mult) {
    if (!gradients || clip_norm <= 0.0f) {
        return FL_ERR_INVALID;
    }
    
    fl_gradient_clip(gradients, count, clip_norm);
    
    float noise_std = clip_norm * noise_mult;
    
    for (uint32_t i = 0; i < count; i++) {
        gradients[i] += rand_gaussian() * noise_std;
    }
    
    return FL_ERR_OK;
}

fl_err_t fl_gradient_clip(float *gradients, uint32_t count, float clip_norm) {
    if (!gradients || clip_norm <= 0.0f) {
        return FL_ERR_INVALID;
    }
    
    float norm = fl_compute_norm(gradients, count);
    
    if (norm > clip_norm) {
        float scale = clip_norm / norm;
        for (uint32_t i = 0; i < count; i++) {
            gradients[i] *= scale;
        }
    }
    
    return FL_ERR_OK;
}

float fl_compute_norm(const float *values, uint32_t count) {
    float sum_sq = 0.0f;
    for (uint32_t i = 0; i < count; i++) {
        sum_sq += values[i] * values[i];
    }
    return sqrtf(sum_sq);
}

fl_err_t fl_gradient_alloc(fl_gradient_t *grad, uint32_t capacity) {
    if (!grad) {
        return FL_ERR_INVALID;
    }
    
    grad->values = (float *)os_mem_alloc(capacity * sizeof(float));
    if (!grad->values) {
        return FL_ERR_NOMEM;
    }
    
    grad->indices = (uint32_t *)os_mem_alloc(capacity * sizeof(uint32_t));
    if (!grad->indices) {
        os_mem_free(grad->values);
        grad->values = NULL;
        return FL_ERR_NOMEM;
    }
    
    grad->capacity = capacity;
    grad->count = 0;
    grad->original_size = 0;
    grad->sparse = false;
    
    return FL_ERR_OK;
}

void fl_gradient_free(fl_gradient_t *grad) {
    if (grad) {
        if (grad->values) {
            os_mem_free(grad->values);
        }
        if (grad->indices) {
            os_mem_free(grad->indices);
        }
        grad->values = NULL;
        grad->indices = NULL;
        grad->capacity = 0;
        grad->count = 0;
    }
}

void fl_gradient_clear(fl_gradient_t *grad) {
    if (grad) {
        grad->count = 0;
        grad->original_size = 0;
    }
}

#endif
