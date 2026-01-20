/**
 * @file ai_model.c
 * @brief AI Model Runtime Implementation - IREE Backend
 * 
 * 此文件封装了 IREE 的复杂初始化，提供简洁的 API。
 * 用户无需修改此文件，只需修改 ai_model.h 中的配置。
 */

#include "ai_model.h"
#include <stdio.h>
#include <string.h>

/* IREE Headers */
#include "iree/base/api.h"
#include "iree/vm/api.h"
#include "iree/hal/api.h"
#include "iree/modules/hal/module.h"
#include "iree/modules/hal/types.h"
#include "iree/hal/drivers/local_sync/sync_driver.h"
#include "iree/hal/local/loaders/static_library_loader.h"

/*===========================================================================*/
/*                   外部符号 - 由模型 .o 文件提供                            */
/*===========================================================================*/

/* EmitC 模块创建函数 */
extern iree_status_t module_create(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module);

#ifdef AI_MODE_BYTECODE
#include "iree/vm/bytecode/module.h"
#include "iree/hal/local/loaders/embedded_elf_loader.h"
// Auto-generated header
#include "st_yolo_lc_v1_192_vmfb.h"
#else
/* 静态库查询函数 (名称需与模型匹配) */
extern const iree_hal_executable_library_header_t**
    st_yolo_lc_v1_192_int_beta_opt_linked_library_query(
        iree_hal_executable_library_version_t max_version,
        const iree_hal_executable_environment_v0_t* environment);
#endif

/*===========================================================================*/
/*                        内部状态                                            */
/*===========================================================================*/

typedef struct {
    iree_allocator_t allocator;
    iree_vm_instance_t* instance;
    iree_vm_context_t* context;
    iree_vm_module_t* model_module;
    iree_vm_module_t* hal_module;
    iree_hal_device_t* device;
    iree_hal_driver_t* driver;
    iree_hal_executable_loader_t* loader;
    iree_vm_function_t main_func;
    iree_vm_ref_type_t buffer_view_type;
    ai_output_t output_buffer[AI_OUTPUT_SIZE];
    int initialized;
} ai_context_t;

static ai_context_t g_ctx = {0};

/*===========================================================================*/
/*                        错误处理宏                                          */
/*===========================================================================*/

#define AI_CHECK_OK(expr) \
    do { \
        iree_status_t _s = (expr); \
        if (!iree_status_is_ok(_s)) { \
            iree_status_ignore(_s); \
            return AI_ERROR_INIT; \
        } \
    } while(0)

#define AI_CHECK_OK_RUN(expr) \
    do { \
        iree_status_t _s = (expr); \
        if (!iree_status_is_ok(_s)) { \
            iree_status_ignore(_s); \
            return AI_ERROR_RUN; \
        } \
    } while(0)

/*===========================================================================*/
/*                        API 实现                                            */
/*===========================================================================*/

ai_status_t ai_init(void) {
    if (g_ctx.initialized) {
        return AI_OK;
    }

    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.allocator = iree_allocator_system();

    /* 1. 创建 VM 实例 */
    AI_CHECK_OK(iree_vm_instance_create(
        IREE_VM_TYPE_CAPACITY_DEFAULT, 
        g_ctx.allocator, 
        &g_ctx.instance));
    
    AI_CHECK_OK(iree_hal_module_register_all_types(g_ctx.instance));

#ifdef AI_MODE_BYTECODE
    /* 2. 创建 Embedded ELF 加载器 */
    AI_CHECK_OK(iree_hal_embedded_elf_loader_create(
        /*plugin_manager=*/NULL, g_ctx.allocator, &g_ctx.loader));
#else
    /* 2. 创建静态库加载器 */
    const iree_hal_executable_library_query_fn_t libraries[] = {
        st_yolo_lc_v1_192_int_beta_opt_linked_library_query,
    };
    AI_CHECK_OK(iree_hal_static_library_loader_create(
        IREE_ARRAYSIZE(libraries), libraries,
        iree_hal_executable_import_provider_null(),
        g_ctx.allocator, &g_ctx.loader));
#endif

    /* 3. 创建 HAL 驱动和设备 */
    iree_hal_sync_device_params_t params;
    iree_hal_sync_device_params_initialize(&params);

    iree_hal_allocator_t* device_allocator = NULL;
    AI_CHECK_OK(iree_hal_allocator_create_heap(
        iree_make_cstring_view("heap"),
        g_ctx.allocator, g_ctx.allocator,
        &device_allocator));

    AI_CHECK_OK(iree_hal_sync_driver_create(
        iree_make_cstring_view("local-sync"),
        &params, 1, &g_ctx.loader,
        device_allocator,
        g_ctx.allocator, &g_ctx.driver));

    AI_CHECK_OK(iree_hal_driver_create_default_device(
        g_ctx.driver, g_ctx.allocator, &g_ctx.device));

    /* 4. 创建 HAL 模块 */
    AI_CHECK_OK(iree_hal_module_create(
        g_ctx.instance, 1, &g_ctx.device,
        IREE_HAL_MODULE_FLAG_NONE,
        iree_hal_module_debug_sink_null(),
        g_ctx.allocator, &g_ctx.hal_module));

    /* 5. 加载模型模块 */
#ifdef AI_MODE_BYTECODE
    AI_CHECK_OK(iree_vm_bytecode_module_create(
        g_ctx.instance,
        iree_make_const_byte_span(st_yolo_lc_v1_192_vmfb, st_yolo_lc_v1_192_vmfb_len),
        iree_allocator_null(),
        g_ctx.allocator, &g_ctx.model_module));
#else
    AI_CHECK_OK(module_create(
        g_ctx.instance, g_ctx.allocator, &g_ctx.model_module));
#endif

    /* 6. 创建上下文 */
    iree_vm_module_t* modules[] = {g_ctx.hal_module, g_ctx.model_module};
    AI_CHECK_OK(iree_vm_context_create_with_modules(
        g_ctx.instance, IREE_VM_CONTEXT_FLAG_NONE,
        2, modules,
        g_ctx.allocator, &g_ctx.context));

    /* 7. 查找 main 函数 */
    AI_CHECK_OK(iree_vm_module_lookup_function_by_name(
        g_ctx.model_module, IREE_VM_FUNCTION_LINKAGE_EXPORT,
        iree_make_cstring_view("main"), &g_ctx.main_func));

    /* 8. 获取 buffer_view 类型 */
    g_ctx.buffer_view_type = iree_vm_instance_lookup_type(
        g_ctx.instance, iree_make_cstring_view("hal.buffer_view"));

    g_ctx.initialized = 1;
    printf("[AI] Model '%s' initialized\n", AI_MODEL_NAME);
    return AI_OK;
}

void ai_deinit(void) {
    if (!g_ctx.initialized) return;
    
    if (g_ctx.context) iree_vm_context_release(g_ctx.context);
    if (g_ctx.instance) iree_vm_instance_release(g_ctx.instance);
    if (g_ctx.driver) iree_hal_driver_release(g_ctx.driver);
    if (g_ctx.device) iree_hal_device_release(g_ctx.device);
    if (g_ctx.loader) iree_hal_executable_loader_release(g_ctx.loader);
    
    memset(&g_ctx, 0, sizeof(g_ctx));
    printf("[AI] Deinitialized\n");
}

ai_status_t ai_run(const ai_input_t* input) {
    if (!g_ctx.initialized || !input) {
        return AI_ERROR_PARAM;
    }

    iree_vm_list_t* inputs = NULL;
    iree_vm_list_t* outputs = NULL;
    iree_hal_buffer_view_t* input_view = NULL;
    ai_status_t ret = AI_OK;

    /* 创建输入 buffer view */
    const iree_hal_dim_t shape[] = {1, AI_INPUT_HEIGHT, AI_INPUT_WIDTH, AI_INPUT_CHANNELS};
    iree_hal_buffer_params_t buffer_params = {
        .usage = IREE_HAL_BUFFER_USAGE_DISPATCH_STORAGE | 
                 IREE_HAL_BUFFER_USAGE_TRANSFER | 
                 IREE_HAL_BUFFER_USAGE_MAPPING,
        .access = IREE_HAL_MEMORY_ACCESS_ALL,
        .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE,
    };

    iree_status_t status = iree_hal_buffer_view_allocate_buffer_copy(
        g_ctx.device,
        iree_hal_device_allocator(g_ctx.device),
        4, shape,
        IREE_HAL_ELEMENT_TYPE_SINT_8,
        IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        buffer_params,
        iree_make_const_byte_span(input, AI_INPUT_SIZE * sizeof(ai_input_t)),
        &input_view);
    
    if (!iree_status_is_ok(status)) {
        iree_status_ignore(status);
        return AI_ERROR_RUN;
    }

    /* 准备输入列表 */
    iree_vm_type_def_t type_def = iree_vm_make_ref_type_def(g_ctx.buffer_view_type);
    status = iree_vm_list_create(type_def, 1, g_ctx.allocator, &inputs);
    if (!iree_status_is_ok(status)) { ret = AI_ERROR_RUN; goto cleanup; }
    
    iree_vm_list_resize(inputs, 1);
    iree_vm_ref_t input_ref = iree_vm_ref_null();
    iree_vm_ref_wrap_retain(input_view, g_ctx.buffer_view_type, &input_ref);
    iree_vm_list_set_ref_retain(inputs, 0, &input_ref);

    /* 准备输出列表 */
    status = iree_vm_list_create(type_def, 1, g_ctx.allocator, &outputs);
    if (!iree_status_is_ok(status)) { ret = AI_ERROR_RUN; goto cleanup; }
    iree_vm_list_resize(outputs, 1);

    /* 运行推理 */
    status = iree_vm_invoke(
        g_ctx.context, g_ctx.main_func,
        IREE_VM_INVOCATION_FLAG_NONE,
        NULL, inputs, outputs, g_ctx.allocator);
    
    if (!iree_status_is_ok(status)) {
        iree_status_ignore(status);
        ret = AI_ERROR_RUN;
        goto cleanup;
    }

    /* 提取输出 */
    iree_vm_ref_t output_ref = iree_vm_ref_null();
    iree_vm_list_get_ref_assign(outputs, 0, &output_ref);
    iree_hal_buffer_view_t* output_view = (iree_hal_buffer_view_t*)output_ref.ptr;

    iree_hal_buffer_mapping_t mapping;
    status = iree_hal_buffer_map_range(
        iree_hal_buffer_view_buffer(output_view),
        IREE_HAL_MAPPING_MODE_SCOPED,
        IREE_HAL_MEMORY_ACCESS_READ,
        0, IREE_WHOLE_BUFFER, &mapping);
    
    if (iree_status_is_ok(status)) {
        memcpy(g_ctx.output_buffer, mapping.contents.data, 
               AI_OUTPUT_SIZE * sizeof(ai_output_t));
        iree_hal_buffer_unmap_range(&mapping);
    } else {
        iree_status_ignore(status);
        ret = AI_ERROR_RUN;
    }

cleanup:
    if (input_view) iree_hal_buffer_view_release(input_view);
    if (inputs) iree_vm_list_release(inputs);
    if (outputs) iree_vm_list_release(outputs);
    return ret;
}

const ai_output_t* ai_get_output(void) {
    return g_ctx.output_buffer;
}

int ai_get_prediction(ai_output_t* confidence) {
    int max_idx = 0;
    ai_output_t max_val = g_ctx.output_buffer[0];
    
    for (int i = 1; i < AI_OUTPUT_SIZE; i++) {
        if (g_ctx.output_buffer[i] > max_val) {
            max_val = g_ctx.output_buffer[i];
            max_idx = i;
        }
    }
    
    if (confidence) *confidence = max_val;
    return max_idx;
}

/*===========================================================================*/
/*                        后处理模板                                          */
/*===========================================================================*/

void ai_postprocess_softmax(const ai_output_t* output, float* probs) {
    /* 简化的 softmax (INT8 输出) */
    float sum = 0.0f;
    for (int i = 0; i < AI_OUTPUT_SIZE; i++) {
        /* 反量化: float_val = (int8_val - zero_point) * scale */
        float val = ((float)output[i] - AI_OUTPUT_ZERO_POINT) * AI_OUTPUT_SCALE;
        probs[i] = val;
        sum += val;
    }
    /* 归一化 */
    if (sum > 0.0f) {
        for (int i = 0; i < AI_OUTPUT_SIZE; i++) {
            probs[i] /= sum;
        }
    }
}

int ai_postprocess_argmax(const ai_output_t* output) {
    int max_idx = 0;
    ai_output_t max_val = output[0];
    for (int i = 1; i < AI_OUTPUT_SIZE; i++) {
        if (output[i] > max_val) {
            max_val = output[i];
            max_idx = i;
        }
    }
    return max_idx;
}
