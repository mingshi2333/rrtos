/**
 * @file ai_model_registry.h
 * @brief Canonical AI Model Registry runtime for supported IREE/EmitC models
 * 
 * Supported AI integrations in rrtos SHALL use this header.
 */

#ifndef AI_MODEL_REGISTRY_H
#define AI_MODEL_REGISTRY_H

#include "ai_model_registry_c_api.h"

#include "iree/vm/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/*                        Data type definitions                                        */
/*===========================================================================*/

/**
 * @brief EmitC module creation function type
 */
typedef iree_status_t (*ai_module_create_fn_t)(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module
);

typedef enum {
    AI_IREE_HAL_BACKEND_STATIC_LIBRARY = 0,
    AI_IREE_HAL_BACKEND_INLINE = 1,
} ai_iree_hal_backend_t;

/**
 * @brief EmitC model descriptor (defined at compile time)
 */
typedef struct {
    const char *name;                       ///< Model name
    const char *version;                    ///< Version number
    const char *entry_function;             ///< Exported entry function name
    ai_module_create_fn_t module_create_fn; ///< EmitC module creation function
    const void *library_query_fn;           ///< Static library query function
    ai_iree_hal_backend_t hal_backend;      ///< HAL ABI required by the generated module
    
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
    uint64_t latency_min_us;                ///< Minimum CLINT/mtime latency
    uint64_t latency_max_us;                ///< Maximum CLINT/mtime latency
    uint64_t latency_last_us;               ///< Last CLINT/mtime latency
    uint64_t latency_min_cycles;            ///< Minimum CPU cycle latency
    uint64_t latency_max_cycles;            ///< Maximum CPU cycle latency
    uint64_t latency_last_cycles;           ///< Last CPU cycle latency
    uint64_t latency_min_instructions;      ///< Minimum retired instruction count
    uint64_t latency_max_instructions;      ///< Maximum retired instruction count
    uint64_t latency_last_instructions;     ///< Last retired instruction count
    uint64_t total_inferences;              ///< Total inferences
    size_t arena_peak_bytes;                ///< Arena peak usage
} ai_perf_stats_t;

enum {
    AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY = 8,
};

typedef struct {
    uint64_t allocation_size;
    uint32_t requested_memory_type;
    uint32_t requested_usage;
    uint32_t requested_access;
    uint32_t compat_memory_type;
    uint32_t compat_usage;
    uint32_t compat_access;
    uint64_t min_alignment;
    uintptr_t buffer_ptr;
    uint32_t sequence;
} ai_hal_allocation_trace_record_t;

typedef struct {
    ai_hal_allocation_trace_record_t largest_allocations[AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY];
    ai_hal_allocation_trace_record_t peak_event;
    uint32_t sequence;
} ai_hal_allocation_trace_t;

/*===========================================================================*/
/*                        Model registration API                                        */
/*===========================================================================*/

/**
 * @brief De-initialize AI Runtime
 */
void ai_runtime_deinit(void);

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

void ai_hal_allocation_trace_reset(void);
ai_hal_allocation_trace_t ai_hal_allocation_trace_get(void);

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
 * @brief Compatibility async entrypoint.
 *
 * The current supported implementation completes inference synchronously and
 * invokes the callback before returning. Callers MUST NOT assume queued or
 * scheduler-integrated execution.
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
