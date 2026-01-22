#include "ai_model_registry.h"
#include "iree/hal/api.h"
#include "iree/vm/api.h"
#include "iree/modules/hal/module.h"
#include "iree/hal/drivers/local_sync/sync_device.h"
#include "iree/hal/local/loaders/static_library_loader.h"
#include "os_kernel.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define AI_MAX_MODELS 8

extern const ai_emitc_model_descriptor_t *g_emitc_models[];

typedef struct ai_model_entry_s {
    const ai_emitc_model_descriptor_t *descriptor;
    iree_vm_module_t *vm_module;
    iree_vm_function_t main_func;
    ai_perf_stats_t perf_stats;
    uint32_t flags;
} ai_model_entry_t;

typedef struct {
    ai_model_entry_t models[AI_MAX_MODELS];
    int count;
    
    iree_allocator_t allocator;
    iree_vm_instance_t *vm_instance;
    iree_hal_device_t *hal_device;
    iree_hal_executable_loader_t *loader;
    iree_vm_context_t *vm_context;
    iree_vm_type_def_t buffer_view_type;
} ai_model_registry_t;

static ai_model_registry_t g_registry;

static inline uint64_t ai_get_time_us(void) {
    return 0;
}

int ai_runtime_init(void) {
    memset(&g_registry, 0, sizeof(g_registry));
    g_registry.allocator = iree_allocator_system();
    
    iree_status_t status = iree_vm_instance_create(
        IREE_VM_TYPE_CAPACITY_DEFAULT,
        g_registry.allocator,
        &g_registry.vm_instance
    );
    if (!iree_status_is_ok(status)) {
        return -1;
    }
    
    // Register HAL types
    status = iree_hal_module_register_all_types(g_registry.vm_instance);
    if (!iree_status_is_ok(status)) {
        iree_vm_instance_release(g_registry.vm_instance);
        return -1;
    }
    
    // Count models and collect library_query functions
    int model_count = 0;
    for (int i = 0; g_emitc_models[i] != NULL && i < AI_MAX_MODELS; i++) {
        model_count++;
    }
    
    // Build library query array
    iree_hal_executable_library_query_fn_t libraries[AI_MAX_MODELS];
    for (int i = 0; i < model_count; i++) {
        libraries[i] = (iree_hal_executable_library_query_fn_t)g_emitc_models[i]->library_query_fn;
    }
    
    // Create static library loader
    status = iree_hal_static_library_loader_create(
        model_count, libraries,
        iree_hal_executable_import_provider_null(),
        g_registry.allocator, &g_registry.loader
    );
    if (!iree_status_is_ok(status)) {
        iree_vm_instance_release(g_registry.vm_instance);
        return -1;
    }
    
    // Create HAL device with loader
    iree_hal_sync_device_params_t params;
    iree_hal_sync_device_params_initialize(&params);
    
    iree_hal_allocator_t* device_allocator = NULL;
    status = iree_hal_allocator_create_heap(
        iree_make_cstring_view("hal"), g_registry.allocator, g_registry.allocator,
        &device_allocator);
    
    if (iree_status_is_ok(status)) {
        status = iree_hal_sync_device_create(
            iree_make_cstring_view("local-sync"), &params,
            /*loader_count=*/1, &g_registry.loader,
            device_allocator, g_registry.allocator, &g_registry.hal_device);
    }
    
    if (device_allocator) {
        iree_hal_allocator_release(device_allocator);
    }
    
    if (!iree_status_is_ok(status)) {
        iree_vm_instance_release(g_registry.vm_instance);
        return -1;
    }
    
    iree_vm_module_t *hal_module;
    status = iree_hal_module_create(
        g_registry.vm_instance,
        /*device_count=*/1, &g_registry.hal_device,
        IREE_HAL_MODULE_FLAG_SYNCHRONOUS,
        iree_hal_module_debug_sink_null(),
        g_registry.allocator,
        &hal_module
    );
    if (!iree_status_is_ok(status)) {
        iree_vm_instance_release(g_registry.vm_instance);
        return -1;
    }
    
    g_registry.buffer_view_type = iree_vm_make_ref_type_def(iree_hal_buffer_view_type());
    
    // Load all model modules first
    iree_vm_module_t *all_modules[AI_MAX_MODELS + 1];
    all_modules[0] = hal_module;
    int module_count = 1;
    
    for (int i = 0; g_emitc_models[i] != NULL && i < AI_MAX_MODELS; i++) {
        const ai_emitc_model_descriptor_t *desc = g_emitc_models[i];
        ai_model_entry_t *entry = &g_registry.models[i];
        
        printf("[AI] Loading model: %s\n", desc->name);
        entry->descriptor = desc;
        
        status = desc->module_create_fn(
            g_registry.vm_instance,
            g_registry.allocator,
            &entry->vm_module
        );
        if (!iree_status_is_ok(status)) {
            printf("[AI] ERROR: Failed to create module for %s\n", desc->name);
            continue;
        }
        
        status = iree_vm_module_lookup_function_by_name(
            entry->vm_module,
            IREE_VM_FUNCTION_LINKAGE_EXPORT,
            iree_make_cstring_view("main"),
            &entry->main_func
        );
        if (!iree_status_is_ok(status)) {
            printf("[AI] ERROR: Failed to resolve 'main' function for %s\n", desc->name);
            iree_vm_module_release(entry->vm_module);
            continue;
        }
        
        all_modules[module_count++] = entry->vm_module;
        entry->perf_stats.latency_min_us = UINT64_MAX;
        entry->perf_stats.latency_max_us = 0;
        
        g_registry.count++;
        printf("[AI] Loaded model: %s\n", desc->name);
    }
    
    // Create a single VM context with all modules
    status = iree_vm_context_create_with_modules(
        g_registry.vm_instance,
        IREE_VM_CONTEXT_FLAG_NONE,
        module_count,
        all_modules,
        g_registry.allocator,
        &g_registry.vm_context
    );
    
    iree_vm_module_release(hal_module);
    
    if (!iree_status_is_ok(status)) {
        printf("[AI] ERROR: Failed to create global VM context\n");
        for (int i = 0; i < g_registry.count; i++) {
            if (g_registry.models[i].vm_module) {
                iree_vm_module_release(g_registry.models[i].vm_module);
            }
        }
        iree_vm_instance_release(g_registry.vm_instance);
        return -1;
    }
    
    printf("[AI] Runtime initialized with %d models\n", g_registry.count);
    return 0;
}

void ai_runtime_deinit(void) {
    if (g_registry.vm_context) {
        iree_vm_context_release(g_registry.vm_context);
        g_registry.vm_context = NULL;
    }

    for (int i = 0; i < g_registry.count; i++) {
        ai_model_entry_t *entry = &g_registry.models[i];
        if (entry->vm_module) {
            iree_vm_module_release(entry->vm_module);
        }
    }
    
    if (g_registry.hal_device) {
        iree_hal_device_release(g_registry.hal_device);
    }
    if (g_registry.vm_instance) {
        iree_vm_instance_release(g_registry.vm_instance);
    }
    
    memset(&g_registry, 0, sizeof(g_registry));
}

ai_model_handle_t ai_model_find_by_name(const char *name) {
    for (int i = 0; i < g_registry.count; i++) {
        if (strcmp(g_registry.models[i].descriptor->name, name) == 0) {
            return (ai_model_handle_t)&g_registry.models[i];
        }
    }
    return NULL;
}

int ai_model_get_input_info(ai_model_handle_t handle, uint32_t index,
                             ai_tensor_spec_t *spec) {
    if (!handle || !spec) return -1;
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    if (index >= entry->descriptor->num_inputs) {
        return -1;
    }
    
    *spec = entry->descriptor->inputs[index];
    return 0;
}

int ai_model_get_output_info(ai_model_handle_t handle, uint32_t index,
                              ai_tensor_spec_t *spec) {
    if (!handle || !spec) return -1;
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    if (index >= entry->descriptor->num_outputs) {
        return -1;
    }
    
    *spec = entry->descriptor->outputs[index];
    return 0;
}

const char* ai_model_get_name(ai_model_handle_t handle) {
    if (!handle) return NULL;
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    return entry->descriptor->name;
}

int ai_model_get_perf_stats(ai_model_handle_t handle, ai_perf_stats_t *stats) {
    if (!handle || !stats) return -1;
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    *stats = entry->perf_stats;
    return 0;
}

void ai_model_reset_perf_stats(ai_model_handle_t handle) {
    if (!handle) return;
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    memset(&entry->perf_stats, 0, sizeof(ai_perf_stats_t));
    entry->perf_stats.latency_min_us = UINT64_MAX;
}

int ai_model_list(const char **names, int max_count) {
    int count = g_registry.count < max_count ? g_registry.count : max_count;
    for (int i = 0; i < count; i++) {
        names[i] = g_registry.models[i].descriptor->name;
    }
    return g_registry.count;
}

int ai_infer_sync(ai_model_handle_t handle,
                  const ai_tensor_t *input,
                  ai_tensor_t *output,
                  uint32_t timeout_ms) {
    if (!handle || !input || !output) {
        fprintf(stderr, "[AI] ERROR: Null parameter (handle=%p, input=%p, output=%p)\n",
                handle, input, output);
        return -1;
    }
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    
    if (!g_registry.vm_context) {
        fprintf(stderr, "[AI] ERROR: Global VM context is null\n");
        return -1;
    }
    
    if (!entry->main_func.ordinal && !entry->main_func.linkage) {
        fprintf(stderr, "[AI] ERROR: Model '%s' has invalid main_func\n", entry->descriptor->name);
        return -1;
    }
    
    if (!input->data) {
        fprintf(stderr, "[AI] ERROR: Input data is null\n");
        return -1;
    }
    
    uint64_t start_us = ai_get_time_us();
    
    iree_hal_buffer_params_t params = {
        .usage = IREE_HAL_BUFFER_USAGE_DISPATCH_STORAGE | IREE_HAL_BUFFER_USAGE_TRANSFER,
        .access = IREE_HAL_MEMORY_ACCESS_ALL,
        .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE
    };
    
    iree_hal_buffer_view_t *input_view;
    iree_status_t status = iree_hal_buffer_view_allocate_buffer_copy(
        g_registry.hal_device,
        iree_hal_device_allocator(g_registry.hal_device),
        input->ndim,
        input->shape,
        IREE_HAL_ELEMENT_TYPE_SINT_8,
        IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        params,
        iree_make_const_byte_span(input->data, input->size),
        &input_view
    );
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Failed to allocate input buffer view, status code=%d\n", 
                (int)iree_status_code(status));
        iree_status_ignore(status);
        return -1;
    }
    
    fprintf(stderr, "[AI] DEBUG: Created input buffer view successfully\n");
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Failed to allocate input buffer view, status code=%d\n", 
                (int)iree_status_code(status));
        iree_status_ignore(status);
        return -1;
    }
    
    iree_vm_list_t *input_list;
    status = iree_vm_list_create(
        g_registry.buffer_view_type,
        1,
        g_registry.allocator,
        &input_list
    );
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Failed to create input_list, status code=%d\n", 
                (int)iree_status_code(status));
        iree_hal_buffer_view_release(input_view);
        iree_status_ignore(status);
        return -1;
    }
    
    iree_vm_ref_t input_ref = {0};
    iree_vm_ref_wrap_retain(input_view, iree_hal_buffer_view_type(), &input_ref);
    status = iree_vm_list_push_ref_move(input_list, &input_ref);
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Failed to push input ref, status code=%d\n", 
                (int)iree_status_code(status));
        iree_vm_list_release(input_list);
        iree_hal_buffer_view_release(input_view);
        iree_status_ignore(status);
        return -1;
    }
    
    iree_vm_list_t *output_list;
    status = iree_vm_list_create(
        g_registry.buffer_view_type,
        1,
        g_registry.allocator,
        &output_list
    );
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Failed to create output_list, status code=%d\n", 
                (int)iree_status_code(status));
        iree_vm_list_release(input_list);
        iree_hal_buffer_view_release(input_view);
        iree_status_ignore(status);
        return -1;
    }
    
    status = iree_vm_invoke(
        g_registry.vm_context,
        entry->main_func,
        IREE_VM_INVOCATION_FLAG_NONE,
        NULL,
        input_list,
        output_list,
        g_registry.allocator
    );
    
    uint64_t elapsed_us = ai_get_time_us() - start_us;
    
    if (!iree_status_is_ok(status)) {
        fprintf(stderr, "[AI] ERROR: Inference failed for model '%s', status code=%d\n", 
                entry->descriptor->name, (int)iree_status_code(status));
        iree_status_ignore(status);
        iree_vm_list_release(input_list);
        iree_vm_list_release(output_list);
        return -1;
    }
    
    iree_hal_buffer_view_t *output_view = NULL;
    iree_vm_ref_t output_ref = iree_vm_ref_null();
    status = iree_vm_list_get_ref_retain(output_list, 0, &output_ref);
    if (iree_status_is_ok(status)) {
        output_view = iree_hal_buffer_view_deref(output_ref);
    }
    
    if (output_view) {
        iree_hal_buffer_t *buffer = iree_hal_buffer_view_buffer(output_view);
        iree_hal_buffer_mapping_t mapping;
        status = iree_hal_buffer_map_range(
            buffer,
            IREE_HAL_MAPPING_MODE_SCOPED,
            IREE_HAL_MEMORY_ACCESS_READ,
            0,
            IREE_WHOLE_BUFFER,
            &mapping
        );
        
        if (iree_status_is_ok(status)) {
            size_t copy_size = mapping.contents.data_length < output->size ?
                               mapping.contents.data_length : output->size;
            memcpy(output->data, mapping.contents.data, copy_size);
            iree_hal_buffer_unmap_range(&mapping);
        }
    }
    
    iree_vm_ref_release(&output_ref);
    iree_vm_list_release(input_list);
    iree_vm_list_release(output_list);
    
    entry->perf_stats.total_inferences++;
    entry->perf_stats.latency_last_us = elapsed_us;
    if (elapsed_us < entry->perf_stats.latency_min_us) {
        entry->perf_stats.latency_min_us = elapsed_us;
    }
    if (elapsed_us > entry->perf_stats.latency_max_us) {
        entry->perf_stats.latency_max_us = elapsed_us;
    }
    
    return 0;
}

int ai_infer_async(ai_model_handle_t handle,
                   const ai_tensor_t *input,
                   ai_inference_callback_t callback,
                   void *user_data) {
    return -1;
}
