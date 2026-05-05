/**
 * @file ai_model_registry_c_api.h
 * @brief Narrow app-facing AI model registry ABI without IREE header exposure.
 *
 * C++ application layers use this header to call the canonical C AI runtime
 * without pulling in the IREE C++ header surface or any C++ standard library.
 */

#ifndef AI_MODEL_REGISTRY_C_API_H
#define AI_MODEL_REGISTRY_C_API_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/*                        App-facing data definitions                         */
/*===========================================================================*/

/**
 * @brief Tensor data type.
 */
typedef enum {
    AI_DTYPE_INT8 = 0,
    AI_DTYPE_UINT8,
    AI_DTYPE_INT16,
    AI_DTYPE_INT32,
    AI_DTYPE_FP32,
} ai_dtype_t;

/**
 * @brief Tensor specification.
 */
typedef struct {
    uint32_t dims[4];           ///< NHWC dimensions
    uint32_t ndim;              ///< Number of dimensions
    ai_dtype_t dtype;           ///< Data type
    float scale;                ///< Quantization scale factor
    int32_t zero_point;         ///< Quantization zero point
} ai_tensor_spec_t;

/**
 * @brief Tensor data structure.
 */
typedef struct {
    void *data;                 ///< Data pointer
    uint32_t shape[4];          ///< Shape
    uint32_t ndim;              ///< Number of dimensions
    ai_dtype_t dtype;           ///< Data type
    size_t size;                ///< Data size (bytes)
} ai_tensor_t;

/**
 * @brief Model handle (opaque pointer).
 */
typedef struct ai_model_entry_s* ai_model_handle_t;

/*===========================================================================*/
/*                        App-facing registry API                             */
/*===========================================================================*/

/**
 * @brief Initialize AI Runtime and model registry.
 *
 * Load all registered EmitC models.
 *
 * @return 0 success, <0 failure
 */
int ai_runtime_init(void);

/**
 * @brief Find model by name.
 *
 * @param name Model name
 * @return Model handle, returns NULL if not found
 */
ai_model_handle_t ai_model_find_by_name(const char *name);

/**
 * @brief Get model input specifications.
 *
 * @param handle Model handle
 * @param index Input index
 * @param spec Output: Tensor specification
 * @return 0 success, <0 failure
 */
int ai_model_get_input_info(ai_model_handle_t handle, uint32_t index,
                             ai_tensor_spec_t *spec);

/**
 * @brief Get model output specifications.
 *
 * @param handle Model handle
 * @param index Output index
 * @param spec Output: Tensor specification
 * @return 0 success, <0 failure
 */
int ai_model_get_output_info(ai_model_handle_t handle, uint32_t index,
                              ai_tensor_spec_t *spec);

/**
 * @brief Synchronous inference (blocking).
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

#ifdef __cplusplus
}
#endif

#endif /* AI_MODEL_REGISTRY_C_API_H */
