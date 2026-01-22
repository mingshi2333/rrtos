/**
 * @file ai_model_registry.h
 * @brief AI Model Registry - EmitC Model Management
 * 
 * 提供模型注册、查找和元数据管理功能
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
/*                        数据类型定义                                        */
/*===========================================================================*/

/**
 * @brief 张量数据类型
 */
typedef enum {
    AI_DTYPE_INT8 = 0,
    AI_DTYPE_UINT8,
    AI_DTYPE_INT16,
    AI_DTYPE_FP32,
} ai_dtype_t;

/**
 * @brief 张量规格
 */
typedef struct {
    uint32_t dims[4];           ///< NHWC 维度
    uint32_t ndim;              ///< 维度数量
    ai_dtype_t dtype;           ///< 数据类型
    float scale;                ///< 量化缩放因子
    int32_t zero_point;         ///< 量化零点
} ai_tensor_spec_t;

/**
 * @brief 张量数据结构
 */
typedef struct {
    void *data;                 ///< 数据指针
    uint32_t shape[4];          ///< 形状
    uint32_t ndim;              ///< 维度数
    ai_dtype_t dtype;           ///< 数据类型
    size_t size;                ///< 数据大小（字节）
} ai_tensor_t;

/**
 * @brief 模型句柄（不透明指针）
 */
typedef struct ai_model_entry_s* ai_model_handle_t;

/**
 * @brief EmitC 模块创建函数类型
 */
typedef iree_status_t (*ai_module_create_fn_t)(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module
);

/**
 * @brief EmitC 模型描述符（编译时定义）
 */
typedef struct {
    const char *name;                       ///< 模型名称
    const char *version;                    ///< 版本号
    ai_module_create_fn_t module_create_fn; ///< EmitC 模块创建函数
    const void *library_query_fn;           ///< Static library query function
    
    ai_tensor_spec_t inputs[4];             ///< 输入规格（最多4个）
    ai_tensor_spec_t outputs[4];            ///< 输出规格（最多4个）
    uint32_t num_inputs;                    ///< 输入数量
    uint32_t num_outputs;                   ///< 输出数量
    
    size_t arena_size;                      ///< Arena 分配器需求
    size_t peak_memory;                     ///< 峰值内存估计
} ai_emitc_model_descriptor_t;

/**
 * @brief 性能统计
 */
typedef struct {
    uint64_t latency_min_us;                ///< 最小延迟
    uint64_t latency_max_us;                ///< 最大延迟
    uint64_t latency_last_us;               ///< 最近一次延迟
    uint64_t total_inferences;              ///< 总推理次数
    size_t arena_peak_bytes;                ///< Arena 峰值使用
} ai_perf_stats_t;

/*===========================================================================*/
/*                        模型注册 API                                        */
/*===========================================================================*/

/**
 * @brief 初始化 AI Runtime 和模型注册表
 * 
 * 加载所有注册的 EmitC 模型
 * 
 * @return 0 成功, <0 失败
 */
int ai_runtime_init(void);

/**
 * @brief 去初始化 AI Runtime
 */
void ai_runtime_deinit(void);

/**
 * @brief 按名称查找模型
 * 
 * @param name 模型名称
 * @return 模型句柄，未找到返回 NULL
 */
ai_model_handle_t ai_model_find_by_name(const char *name);

/**
 * @brief 获取模型输入规格
 * 
 * @param handle 模型句柄
 * @param index 输入索引
 * @param spec 输出：张量规格
 * @return 0 成功, <0 失败
 */
int ai_model_get_input_info(ai_model_handle_t handle, uint32_t index,
                             ai_tensor_spec_t *spec);

/**
 * @brief 获取模型输出规格
 * 
 * @param handle 模型句柄
 * @param index 输出索引
 * @param spec 输出：张量规格
 * @return 0 成功, <0 失败
 */
int ai_model_get_output_info(ai_model_handle_t handle, uint32_t index,
                              ai_tensor_spec_t *spec);

/**
 * @brief 获取模型名称
 * 
 * @param handle 模型句柄
 * @return 模型名称字符串
 */
const char* ai_model_get_name(ai_model_handle_t handle);

/**
 * @brief 获取性能统计
 * 
 * @param handle 模型句柄
 * @param stats 输出：性能统计
 * @return 0 成功, <0 失败
 */
int ai_model_get_perf_stats(ai_model_handle_t handle, ai_perf_stats_t *stats);

/**
 * @brief 重置性能统计
 * 
 * @param handle 模型句柄
 */
void ai_model_reset_perf_stats(ai_model_handle_t handle);

/**
 * @brief 列出所有已注册的模型
 * 
 * @param names 输出：模型名称数组（调用者分配）
 * @param max_count 数组最大容量
 * @return 实际模型数量
 */
int ai_model_list(const char **names, int max_count);

/*===========================================================================*/
/*                        推理 API                                           */
/*===========================================================================*/

/**
 * @brief 同步推理（阻塞）
 * 
 * @param handle 模型句柄
 * @param input 输入张量
 * @param output 输出张量（调用者分配内存）
 * @param timeout_ms 超时（毫秒），0 = 无限等待
 * @return 0 成功, <0 失败
 */
int ai_infer_sync(ai_model_handle_t handle,
                  const ai_tensor_t *input,
                  ai_tensor_t *output,
                  uint32_t timeout_ms);

/**
 * @brief 推理回调函数
 * 
 * @param model 模型句柄
 * @param output 输出张量
 * @param status 推理状态（0 = 成功）
 * @param user_data 用户数据
 */
typedef void (*ai_inference_callback_t)(
    ai_model_handle_t model,
    const ai_tensor_t *output,
    int status,
    void *user_data
);

/**
 * @brief 异步推理（非阻塞）
 * 
 * @param handle 模型句柄
 * @param input 输入张量
 * @param callback 完成回调
 * @param user_data 用户数据
 * @return 0 成功, <0 失败
 */
int ai_infer_async(ai_model_handle_t handle,
                   const ai_tensor_t *input,
                   ai_inference_callback_t callback,
                   void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* AI_MODEL_REGISTRY_H */
