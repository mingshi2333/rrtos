/**
 * @file ai_model.c
 * @brief Optimized AI Model Runtime - Direct HAL Device (No Driver)
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
// Use sync_device directly, skipping the driver layer
#include "iree/hal/drivers/local_sync/sync_device.h" 
#include "iree/hal/local/loaders/static_library_loader.h"

/*===========================================================================*/
/*                   External Symbols (Model)                                 */
/*===========================================================================*/

/* EmitC Module Creator */
extern iree_status_t module_create(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module);

#ifndef AI_MODE_BYTECODE
/* Static Library Query Function */
extern const iree_hal_executable_library_header_t**
    st_yolo_lc_v1_192_int_beta_opt_linked_library_query(
        iree_hal_executable_library_version_t max_version,
        const iree_hal_executable_environment_v0_t* environment);
#endif

/*===========================================================================*/
/*                        Context State                                       */
/*===========================================================================*/

typedef struct {
    iree_allocator_t allocator;
    iree_vm_instance_t* instance;
    iree_vm_context_t* context;
    iree_hal_device_t* device;
    iree_hal_executable_loader_t* loader;
    iree_vm_function_t main_func;
    iree_vm_ref_type_t buffer_view_type;
    ai_output_t output_buffer[AI_OUTPUT_SIZE];
    int initialized;
} ai_context_t;

static ai_context_t g_ctx = {0};

/*===========================================================================*/
/*                        API Implementation                                  */
/*===========================================================================*/

ai_status_t ai_init(void) {
    if (g_ctx.initialized) return AI_OK;
    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.allocator = iree_allocator_system();
    iree_status_t status = iree_ok_status();

    // 1. Create VM Instance
    // Minimal instance, custom allocators can be hooked here if needed
    if (iree_status_is_ok(status)) {
        status = iree_vm_instance_create(IREE_VM_TYPE_CAPACITY_DEFAULT, g_ctx.allocator, &g_ctx.instance);
    }
    
    // Register HAL types (BufferView, etc.)
    if (iree_status_is_ok(status)) {
        status = iree_hal_module_register_all_types(g_ctx.instance);
    }

    // 2. Create Executable Loader (Static Library)
    if (iree_status_is_ok(status)) {
#ifdef AI_MODE_BYTECODE
        // Not implemented in this optimized path
        status = iree_make_status(IREE_STATUS_UNIMPLEMENTED, "Bytecode not supported in optimized build");
#else
        const iree_hal_executable_library_query_fn_t libraries[] = {
            st_yolo_lc_v1_192_int_beta_opt_linked_library_query,
        };
        status = iree_hal_static_library_loader_create(
            IREE_ARRAYSIZE(libraries), libraries,
            iree_hal_executable_import_provider_null(),
            g_ctx.allocator, &g_ctx.loader);
#endif
    }

    // 3. Create HAL Device DIRECTLY (Bypassing Driver)
    // This saves the overhead of the Driver abstraction (~1-2KB Flash + runtime overhead)
    if (iree_status_is_ok(status)) {
        iree_hal_sync_device_params_t params;
        iree_hal_sync_device_params_initialize(&params);
        
        // Device needs its own allocator for buffers
        iree_hal_allocator_t* device_allocator = NULL;
        iree_hal_allocator_create_heap(iree_make_cstring_view("hal"), g_ctx.allocator, g_ctx.allocator, &device_allocator);

        status = iree_hal_sync_device_create(
            iree_make_cstring_view("local-sync"), &params,
            /*loader_count=*/1, &g_ctx.loader,
            device_allocator, g_ctx.allocator, &g_ctx.device);
            
        iree_hal_allocator_release(device_allocator); // Device takes ownership
    }

    // 4. Create HAL Module for VM
    iree_vm_module_t* hal_module = NULL;
    if (iree_status_is_ok(status)) {
        status = iree_hal_module_create(
            g_ctx.instance, 1, &g_ctx.device,
            IREE_HAL_MODULE_FLAG_SYNCHRONOUS, // Optimize: Force synchronous calls
            iree_hal_module_debug_sink_null(),
            g_ctx.allocator, &hal_module);
    }

    // 5. Load Model Module (EmitC)
    iree_vm_module_t* model_module = NULL;
    if (iree_status_is_ok(status)) {
        status = module_create(g_ctx.instance, g_ctx.allocator, &model_module);
    }

    // 6. Create Context with Modules
    if (iree_status_is_ok(status)) {
        iree_vm_module_t* modules[] = {hal_module, model_module};
        status = iree_vm_context_create_with_modules(
            g_ctx.instance, IREE_VM_CONTEXT_FLAG_NONE,
            IREE_ARRAYSIZE(modules), modules,
            g_ctx.allocator, &g_ctx.context);
    }

    // Cleanup temporary module references (Context holds them now)
    if (hal_module) iree_vm_module_release(hal_module);
    if (model_module) iree_vm_module_release(model_module);

    // 7. Lookup Main Function
    if (iree_status_is_ok(status)) {
        status = iree_vm_module_lookup_function_by_name(
            model_module, IREE_VM_FUNCTION_LINKAGE_EXPORT,
            iree_make_cstring_view("main"), &g_ctx.main_func);
    }

    // 8. Cache Buffer View Type
    if (iree_status_is_ok(status)) {
        g_ctx.buffer_view_type = iree_vm_instance_lookup_type(
            g_ctx.instance, iree_make_cstring_view("hal.buffer_view"));
    }

    if (!iree_status_is_ok(status)) {
        iree_status_fprint(stderr, status);
        iree_status_ignore(status);
        return AI_ERROR_INIT;
    }

    g_ctx.initialized = 1;
    printf("[AI] Opt Init OK\n");
    return AI_OK;
}

void ai_deinit(void) {
    if (!g_ctx.initialized) return;
    iree_vm_context_release(g_ctx.context);
    iree_vm_instance_release(g_ctx.instance);
    iree_hal_device_release(g_ctx.device);
    iree_hal_executable_loader_release(g_ctx.loader);
    memset(&g_ctx, 0, sizeof(g_ctx));
}

ai_status_t ai_run(const ai_input_t* input) {
    if (!g_ctx.initialized) return AI_ERROR_PARAM;

    iree_vm_list_t* inputs = NULL;
    iree_vm_list_t* outputs = NULL;
    iree_hal_buffer_view_t* input_view = NULL;
    ai_status_t ret = AI_OK;
    iree_status_t status = iree_ok_status();

    // 1. Wrap Input Buffer (Zero Copy)
    const iree_hal_dim_t shape[] = {1, AI_INPUT_HEIGHT, AI_INPUT_WIDTH, AI_INPUT_CHANNELS};
    
    iree_hal_buffer_params_t buffer_params = {
        .usage = IREE_HAL_BUFFER_USAGE_DISPATCH_STORAGE | IREE_HAL_BUFFER_USAGE_TRANSFER,
        .access = IREE_HAL_MEMORY_ACCESS_ALL,
        .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE,
    };

    // External buffer import parameters
    iree_hal_external_buffer_t external_buffer = {
        .type = IREE_HAL_EXTERNAL_BUFFER_TYPE_HOST_ALLOCATION,
        .flags = IREE_HAL_EXTERNAL_BUFFER_FLAG_NONE,
        .size = AI_INPUT_SIZE,
        .handle = {
            .host_allocation = {
                .ptr = (void*)input,
            }
        },
    };

    iree_hal_buffer_t* buffer = NULL;
    status = iree_hal_allocator_import_buffer(
        iree_hal_device_allocator(g_ctx.device),
        buffer_params,
        &external_buffer,
        iree_hal_buffer_release_callback_null(),
        &buffer
    );

    if (iree_status_is_ok(status)) {
        status = iree_hal_buffer_view_create(
            buffer, 4, shape,
            IREE_HAL_ELEMENT_TYPE_SINT_8,
            IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
            g_ctx.allocator,
            &input_view
        );
        iree_hal_buffer_release(buffer); // View holds reference
    }

    if (!iree_status_is_ok(status)) goto error;

    // 2. Prepare VM Lists
    // Optimize: Pre-calculate type defs or reuse if possible (but list creation is cheap)
    iree_vm_type_def_t type_def = iree_vm_make_ref_type_def(g_ctx.buffer_view_type);
    
    status = iree_vm_list_create(type_def, 1, g_ctx.allocator, &inputs);
    if (!iree_status_is_ok(status)) goto error;
    
    iree_vm_ref_t input_ref = iree_vm_ref_null();
    iree_vm_ref_wrap_retain(input_view, g_ctx.buffer_view_type, &input_ref);
    iree_vm_list_push_ref_move(inputs, &input_ref);

    status = iree_vm_list_create(type_def, 1, g_ctx.allocator, &outputs);
    if (!iree_status_is_ok(status)) goto error;

    // 3. Invoke
    status = iree_vm_invoke(
        g_ctx.context, g_ctx.main_func, IREE_VM_INVOCATION_FLAG_NONE,
        NULL, inputs, outputs, g_ctx.allocator);
    
    if (!iree_status_is_ok(status)) goto error;

    // 4. Retrieve Output
    iree_vm_ref_t output_ref = iree_vm_ref_null();
    iree_vm_list_get_ref_assign(outputs, 0, &output_ref);
    iree_hal_buffer_view_t* output_view = (iree_hal_buffer_view_t*)output_ref.ptr;

    // Direct Read - no complex mapping if possible, but map_range is safest
    iree_hal_buffer_mapping_t mapping;
    status = iree_hal_buffer_map_range(
        iree_hal_buffer_view_buffer(output_view),
        IREE_HAL_MAPPING_MODE_SCOPED, IREE_HAL_MEMORY_ACCESS_READ,
        0, IREE_WHOLE_BUFFER, &mapping);
        
    if (iree_status_is_ok(status)) {
        memcpy(g_ctx.output_buffer, mapping.contents.data, AI_OUTPUT_SIZE * sizeof(ai_output_t));
        iree_hal_buffer_unmap_range(&mapping);
    } else {
        goto error;
    }

cleanup:
    if (input_view) iree_hal_buffer_view_release(input_view);
    if (inputs) iree_vm_list_release(inputs);
    if (outputs) iree_vm_list_release(outputs);
    return ret;

error:
    iree_status_ignore(status); // In production, log this
    ret = AI_ERROR_RUN;
    goto cleanup;
}

const ai_output_t* ai_get_output(void) {
    return g_ctx.output_buffer;
}

// Helpers kept for compatibility
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

void ai_postprocess_softmax(const ai_output_t* output, float* probs) {
    // Keep implementation same as before
    float sum = 0.0f;
    for (int i = 0; i < AI_OUTPUT_SIZE; i++) {
        float val = ((float)output[i] - AI_OUTPUT_ZERO_POINT) * AI_OUTPUT_SCALE;
        probs[i] = val;
        sum += val;
    }
    if (sum > 0.0f) {
        for (int i = 0; i < AI_OUTPUT_SIZE; i++) {
            probs[i] /= sum;
        }
    }
}
