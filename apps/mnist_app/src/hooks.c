#include <stdio.h>
#include <stdint.h>
#include "ai_models.h"

/**
 * User-defined pre-processing.
 * e.g., image normalization, resizing, or sensor data scaling.
 */
void ai_app_pre_process(void *input_data, size_t size) {
    // printf("[App] Pre-processing input data...\n");
    // Default: identity
}

/**
 * User-defined post-processing.
 * e.g., finding max index (argmax), applying thresholds, or formatting strings.
 */
void ai_app_post_process(void *output_data, size_t size) {
    // printf("[App] Post-processing output data...\n");
    // Example: find argmax for classification
}
