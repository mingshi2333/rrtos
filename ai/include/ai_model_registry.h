/**
 * @file ai_model_registry.h
 * @brief AI Model Registry - EmitC Model Management
 * 
 * Provides model registration, lookup, and metadata management functions
 */

#ifndef AI_MODEL_REGISTRY_H
#define AI_MODEL_REGISTRY_H

#include <stdint.h>
#include <stddef.h>
#include "iree/vm/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/*                        Data type definitions                                        */
/*===========================================================================*/

/**
 * @brief Tensor data type
 */
typedef enum {
    AI_DTYPE_INT8 = 0,
    AI_DTYPE_UINT8,
    AI_DTYPE_INT16,
    AI_DTYPE_INT32,
    AI_DTYPE_FP32,
} ai_dtype_t;

/**
 * @brief Tensor specification
 */
typedef struct {
    uint32_t dims[4];           ///< NHWC dimensions
    uint32_t ndim;              ///< Number of dimensions
    ai_dtype_t dtype;           ///< Data type
    float scale;                ///< Quantization scale factor
    int32_t zero_point;         ///< Quantization zero point
} ai_tensor_spec_t;

/**
 * @brief Tensor data structure
 */
typedef struct {
    void *data;                 ///< Data pointer
    uint32_t shape[4];          ///< Shape
    uint32_t ndim;              ///< Number of dimensions
    ai_dtype_t dtype;           ///< Data type
    size_t size;                ///< Data size (bytes)
} ai_tensor_t;

/**
 * @brief Model handle (opaque pointer)
 */
typedef struct ai_model_entry_s* ai_model_handle_t;

/**
 * @brief EmitC module creation function type
 */
typedef iree_status_t (*ai_module_create_fn_t)(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module
);

/**
 * @brief EmitC model descriptor (defined at compile time)
 */
typedef struct {
    const char *name;                       ///< Model name
    const char *version;                    ///< Version number
    ai_module_create_fn_t module_create_fn; ///< EmitC module creation function
    const void *library_query_fn;           ///< Static library query function
    
    ai_tensor_spec_t inputs[4];             ///< Input specifications (up to 4)
    ai_tensor_spec_t outputs[4];            ///< Output specifications (up to 4)
    uint32_t num_inputs;                    ///< Number of inputs
    uint32_t num_outputs;                   ///< Number of outputs
    
    size_t arena_size;                      ///< Arena allocator requirements
    size_t peak_memory;                     ///< Peak memory estimate
} ai_emitc_model_descriptor_t;

/**
 * @brief Performance statistics
 */
typedef struct {
    uint64_t latency_min_us;                ///< Minimum latency
    uint64_t latency_max_us;                ///< Maximum latency
    uint64_t latency_last_us;               ///< Last latency
    uint64_t total_inferences;              ///< Total inferences
    size_t arena_peak_bytes;                ///< Arena peak usage
} ai_perf_stats_t;

/*===========================================================================*/
/*                        Model registration API                                        */
/*===========================================================================*/

/**
 * @brief Initialize AI Runtime and model registry
 * 
 * Load all registered EmitC models
 * 
 * @return 0 success, <0 failure
 */
int ai_runtime_init(void);

/**
 * @brief De-initialize AI Runtime
 */
void ai_runtime_deinit(void);

/**
 * @brief Find model by name
 * 
 * @param name Model name
 * @return Model handle, returns NULL if not found
 */
ai_model_handle_t ai_model_find_by_name(const char *name);

/**
 * @brief Get model input specifications
 * 
 * @param handle Model handle
 * @param index Input index
 * @param spec Output: Tensor specification
 * @return 0 success, <0 failure
 */
int ai_model_get_input_info(ai_model_handle_t handle, uint32_t index,
                             ai_tensor_spec_t *spec);

/**
 * @brief Get model output specifications
 * 
 * @param handle Model handle
 * @param index Output index
 * @param spec Output: Tensor specification
 * @return 0 success, <0 failure
 */
int ai_model_get_output_info(ai_model_handle_t handle, uint32_t index,
                              ai_tensor_spec_t *spec);

/**
 * @brief Get model name
 * 
 * @param handle Model handle
 * @return Model name string
 */
const char* ai_model_get_name(ai_model_handle_t handle);

/**
 * @brief Get performance statistics
 * 
 * @param handle Model handle
 * @param stats Output: Performance statistics
 * @return 0 success, <0 failure
 */
int ai_model_get_perf_stats(ai_model_handle_t handle, ai_perf_stats_t *stats);

/**
 * @brief Reset performance statistics
 * 
 * @param handle Model handle
 */
void ai_model_reset_perf_stats(ai_model_handle_t handle);

/**
 * @brief List all registered models
 * 
 * @param names Output: Model name array (allocated by caller)
 * @param max_count Maximum array capacity
 * @return Actual number of models
 */
int ai_model_list(const char **names, int max_count);

/*===========================================================================*/
/*                        Inference API                                           */
/*===========================================================================*/

/**
 * @brief Synchronous inference (blocking)
 * 
 * @param handle Model handle
 * @param input Input tensors
 * @param output Output tensors (allocated by caller)
 * @param timeout_ms Timeout (ms), 0 = wait indefinitely
 * @return 0 success, <0 failure
 */
int ai_infer_sync(ai_model_handle_t handle,
                  const ai_tensor_t *inputs, uint32_t num_inputs,
                  ai_tensor_t *outputs, uint32_t num_outputs,
                  uint32_t timeout_ms);

/**
 * @brief Inference callback function
 * 
 * @param model Model handle
 * @param output Output tensors
 * @param status Inference status (0 = success)
 * @param user_data User data
 */
typedef void (*ai_inference_callback_t)(
    ai_model_handle_t model,
    const ai_tensor_t *output,
    int status,
    void *user_data
);

/**
 * @brief Asynchronous inference (non-blocking)
 * 
 * @param handle Model handle
 * @param input Input tensors
 * @param callback Completion callback
 * @param user_data User data
 * @return 0 success, <0 failure
 */
int ai_infer_async(ai_model_handle_t handle,
                   const ai_tensor_t *inputs, uint32_t num_inputs,
                   ai_tensor_t *outputs, uint32_t num_outputs,
                   ai_inference_callback_t callback,
                   void *user_data);

/**
 * @brief Get current timestamp in microseconds.
 */
uint64_t ai_get_time_us(void);

#ifdef __cplusplus
}
#endif

#endif /* AI_MODEL_REGISTRY_H */
