#include "ai_model_registry.h"

#ifndef AI_IREE_ENABLE_STATIC_LIBRARY_HAL
#define AI_IREE_ENABLE_STATIC_LIBRARY_HAL 1
#endif

#ifndef AI_IREE_ENABLE_INLINE_HAL
#define AI_IREE_ENABLE_INLINE_HAL 0
#endif

#include "iree/hal/api.h"
#include "iree/vm/api.h"
#if AI_IREE_ENABLE_STATIC_LIBRARY_HAL
#include "iree/hal/drivers/local_sync/sync_device.h"
#include "iree/hal/local/loaders/static_library_loader.h"
#include "iree/modules/hal/module.h"
#endif
#if AI_IREE_ENABLE_INLINE_HAL
#include "iree/modules/hal/inline/module.h"
#endif

#include "hal_clint.h"
#include "os_config.h"
#include "os_kernel.h"
#if defined(__riscv)
#include "riscv_csr.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define AI_MAX_MODELS 8

#if defined(IREE_HAL_WHOLE_BUFFER)
#define AI_IREE_WHOLE_BUFFER IREE_HAL_WHOLE_BUFFER
#else
#define AI_IREE_WHOLE_BUFFER IREE_WHOLE_BUFFER
#endif

extern const ai_emitc_model_descriptor_t *g_emitc_models[];

typedef struct ai_model_entry_s {
    const ai_emitc_model_descriptor_t *descriptor;
    iree_vm_module_t *vm_module;
    iree_vm_function_t main_func;
    ai_iree_hal_backend_t hal_backend;
    ai_perf_stats_t perf_stats;
    uint32_t flags;
} ai_model_entry_t;

typedef struct {
    ai_model_entry_t models[AI_MAX_MODELS];
    int count;
    
    iree_allocator_t allocator;
    iree_vm_instance_t *vm_instance;
    iree_hal_device_t *hal_device;
#if AI_IREE_ENABLE_STATIC_LIBRARY_HAL
    iree_hal_executable_loader_t *loader;
#endif
    iree_hal_allocator_t *inline_allocator;
    iree_vm_context_t *vm_context;
    iree_vm_type_def_t buffer_view_type;
} ai_model_registry_t;

static ai_model_registry_t g_registry;
static ai_hal_allocation_trace_t g_hal_allocation_trace;

static void ai_hal_allocation_trace_insert_largest(
    const ai_hal_allocation_trace_record_t *record) {
    uint32_t insert_at = AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY;

    if (record->allocation_size == 0) {
        return;
    }

    for (uint32_t i = 0; i < AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY; ++i) {
        if (record->allocation_size >
            g_hal_allocation_trace.largest_allocations[i].allocation_size) {
            insert_at = i;
            break;
        }
    }

    if (insert_at == AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY) {
        return;
    }

    for (uint32_t i = AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY - 1u;
         i > insert_at;
         --i) {
        g_hal_allocation_trace.largest_allocations[i] =
            g_hal_allocation_trace.largest_allocations[i - 1u];
    }
    g_hal_allocation_trace.largest_allocations[insert_at] = *record;
}

void ai_hal_allocation_trace_reset(void) {
    memset(&g_hal_allocation_trace, 0, sizeof(g_hal_allocation_trace));
}

ai_hal_allocation_trace_t ai_hal_allocation_trace_get(void) {
    return g_hal_allocation_trace;
}

void rrtos_iree_hal_allocation_trace_record(
    uint64_t allocation_size,
    uint32_t requested_memory_type,
    uint32_t requested_usage,
    uint32_t requested_access,
    uint32_t compat_memory_type,
    uint32_t compat_usage,
    uint32_t compat_access,
    uint64_t min_alignment,
    uintptr_t buffer_ptr) {
    ai_hal_allocation_trace_record_t record;

    g_hal_allocation_trace.sequence++;
    record.allocation_size = allocation_size;
    record.requested_memory_type = requested_memory_type;
    record.requested_usage = requested_usage;
    record.requested_access = requested_access;
    record.compat_memory_type = compat_memory_type;
    record.compat_usage = compat_usage;
    record.compat_access = compat_access;
    record.min_alignment = min_alignment;
    record.buffer_ptr = buffer_ptr;
    record.sequence = g_hal_allocation_trace.sequence;

    ai_hal_allocation_trace_insert_largest(&record);
    if (record.allocation_size >= g_hal_allocation_trace.peak_event.allocation_size) {
        g_hal_allocation_trace.peak_event = record;
    }
}

static void ai_log_status(const char *operation, iree_status_t status) {
    char status_buffer[256];
    iree_host_size_t status_length = 0;

    if (iree_status_format(status, sizeof(status_buffer), status_buffer,
                           &status_length)) {
        printf("[AI] ERROR: %s failed: %s\n", operation, status_buffer);
        return;
    }

    printf("[AI] ERROR: %s failed: %s\n",
           operation,
           iree_status_code_string(iree_status_code(status)));
}

static void ai_registry_release_resources(void) {
    if (g_registry.vm_context) {
        iree_vm_context_release(g_registry.vm_context);
        g_registry.vm_context = NULL;
    }

    for (int i = 0; i < AI_MAX_MODELS; i++) {
        ai_model_entry_t *entry = &g_registry.models[i];
        if (entry->vm_module) {
            iree_vm_module_release(entry->vm_module);
            entry->vm_module = NULL;
        }
    }

    if (g_registry.hal_device) {
        iree_hal_device_release(g_registry.hal_device);
        g_registry.hal_device = NULL;
    }

#if AI_IREE_ENABLE_STATIC_LIBRARY_HAL
    if (g_registry.loader) {
        iree_hal_executable_loader_release(g_registry.loader);
        g_registry.loader = NULL;
    }
#endif

    if (g_registry.inline_allocator) {
        iree_hal_allocator_release(g_registry.inline_allocator);
        g_registry.inline_allocator = NULL;
    }

    if (g_registry.vm_instance) {
        iree_vm_instance_release(g_registry.vm_instance);
        g_registry.vm_instance = NULL;
    }

    g_registry.count = 0;
    memset(&g_registry.buffer_view_type, 0, sizeof(g_registry.buffer_view_type));
}

uint64_t ai_get_time_us(void) {
    uint64_t ticks = hal_clint_mtime_get();
    return (ticks * 1000000ULL) / OS_CFG_TIMER_FREQ_HZ;
}

static uint64_t ai_get_cycle_count(void) {
#if defined(__riscv)
    return os_cycle_get();
#else
    return ai_get_time_us();
#endif
}

static uint64_t ai_get_instruction_count(void) {
#if defined(__riscv)
    return os_instret_get();
#else
    return ai_get_time_us();
#endif
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
        ai_log_status("vm_instance_create", status);
        return -1;
    }
    
    // Register HAL types
    status = iree_hal_module_register_all_types(g_registry.vm_instance);
    if (!iree_status_is_ok(status)) {
        ai_log_status("hal_module_register_all_types", status);
        ai_registry_release_resources();
        return -1;
    }
    
    // Count models and select the HAL module ABI required by each generated
    // module. static_library uses the full HAL module; inline uses the
    // smaller hal_inline VM module emitted by vmvx-inline.
    int static_model_count = 0;
#if AI_IREE_ENABLE_INLINE_HAL
    int inline_model_count = 0;
#endif
    for (int i = 0; g_emitc_models[i] != NULL && i < AI_MAX_MODELS; i++) {
        const ai_emitc_model_descriptor_t *desc = g_emitc_models[i];
#if AI_IREE_ENABLE_INLINE_HAL
        if (desc->hal_backend == AI_IREE_HAL_BACKEND_INLINE) {
            inline_model_count++;
        } else {
            static_model_count++;
        }
#else
        (void)desc;
        static_model_count++;
#endif
    }

    g_registry.buffer_view_type = iree_vm_make_ref_type_def(iree_hal_buffer_view_type());

    // Load all model modules first
    iree_vm_module_t *all_modules[AI_MAX_MODELS + 2];
    iree_vm_module_t *static_hal_module = NULL;
    iree_vm_module_t *inline_hal_module = NULL;
    int module_count = 0;

    if (static_model_count > 0) {
#if AI_IREE_ENABLE_STATIC_LIBRARY_HAL
        iree_hal_executable_library_query_fn_t libraries[AI_MAX_MODELS];
        int library_count = 0;
        for (int i = 0; g_emitc_models[i] != NULL && i < AI_MAX_MODELS; i++) {
            const ai_emitc_model_descriptor_t *desc = g_emitc_models[i];
#if AI_IREE_ENABLE_INLINE_HAL
            if (desc->hal_backend == AI_IREE_HAL_BACKEND_INLINE) {
                continue;
            }
#endif
            if (!desc->library_query_fn) {
                printf("[AI] ERROR: static-library model missing library_query: %s\n",
                       desc->name);
                ai_registry_release_resources();
                return -1;
            }
            libraries[library_count++] =
                (iree_hal_executable_library_query_fn_t)desc->library_query_fn;
        }

        status = iree_hal_static_library_loader_create(
            library_count, libraries,
            iree_hal_executable_import_provider_null(),
            g_registry.allocator, &g_registry.loader
        );
        if (!iree_status_is_ok(status)) {
            ai_log_status("hal_static_library_loader_create", status);
            ai_registry_release_resources();
            return -1;
        }

        iree_hal_sync_device_params_t params;
        iree_hal_sync_device_params_initialize(&params);

        iree_hal_allocator_t* device_allocator = NULL;
        status = iree_hal_allocator_create_heap(
            iree_make_cstring_view("hal"), g_registry.allocator, g_registry.allocator,
            &device_allocator);

        if (iree_status_is_ok(status)) {
            status = iree_hal_sync_device_create(
                iree_make_cstring_view("local"), &params,
                /*loader_count=*/1, &g_registry.loader,
                device_allocator, g_registry.allocator, &g_registry.hal_device);
        }

        if (device_allocator) {
            iree_hal_allocator_release(device_allocator);
        }

        if (!iree_status_is_ok(status)) {
            ai_log_status("hal_sync_device_create", status);
            ai_registry_release_resources();
            return -1;
        }

        status = iree_hal_module_create(
            g_registry.vm_instance,
            /*device_count=*/1, &g_registry.hal_device,
            IREE_HAL_MODULE_FLAG_SYNCHRONOUS,
            iree_hal_module_debug_sink_null(),
            g_registry.allocator,
            &static_hal_module
        );
        if (!iree_status_is_ok(status)) {
            ai_log_status("hal_module_create", status);
            ai_registry_release_resources();
            return -1;
        }

        all_modules[module_count++] = static_hal_module;
#else
        printf("[AI] ERROR: static-library HAL model present but AI_IREE_ENABLE_STATIC_LIBRARY_HAL=0\n");
        ai_registry_release_resources();
        return -1;
#endif
    }

#if AI_IREE_ENABLE_INLINE_HAL
    if (inline_model_count > 0) {
        status = iree_hal_allocator_create_heap(
            iree_make_cstring_view("hal_inline"), g_registry.allocator,
            g_registry.allocator, &g_registry.inline_allocator);
        if (!iree_status_is_ok(status)) {
            ai_log_status("hal_inline allocator create", status);
            ai_registry_release_resources();
            return -1;
        }

        status = iree_hal_inline_module_create(
            g_registry.vm_instance,
            IREE_HAL_INLINE_MODULE_FLAG_NONE,
            iree_hal_module_debug_sink_null(),
            g_registry.inline_allocator,
            g_registry.allocator,
            &inline_hal_module);
        if (!iree_status_is_ok(status)) {
            ai_log_status("hal_inline_module_create", status);
            ai_registry_release_resources();
            return -1;
        }

        all_modules[module_count++] = inline_hal_module;
    }
#endif
    
    for (int i = 0; g_emitc_models[i] != NULL && i < AI_MAX_MODELS; i++) {
        const ai_emitc_model_descriptor_t *desc = g_emitc_models[i];
        ai_model_entry_t *entry = &g_registry.models[i];
        
        printf("[AI] Loading model: %s\n", desc->name);
        entry->descriptor = desc;
#if AI_IREE_ENABLE_INLINE_HAL
        entry->hal_backend = desc->hal_backend;
#else
        entry->hal_backend = AI_IREE_HAL_BACKEND_STATIC_LIBRARY;
#endif
        
        status = desc->module_create_fn(
            g_registry.vm_instance,
            g_registry.allocator,
            &entry->vm_module
        );
        if (!iree_status_is_ok(status)) {
            ai_log_status("model module create", status);
            printf("[AI] ERROR: Failed to create module for %s\n", desc->name);
            continue;
        }
        
        const char *entry_function = desc->entry_function ? desc->entry_function : "main";

        status = iree_vm_module_lookup_function_by_name(
            entry->vm_module,
            IREE_VM_FUNCTION_LINKAGE_EXPORT,
            iree_make_cstring_view(entry_function),
            &entry->main_func
        );
        if (!iree_status_is_ok(status)) {
            ai_log_status("model function lookup", status);
            printf("[AI] ERROR: Failed to resolve '%s' function for %s\n",
                   entry_function,
                   desc->name);
            iree_vm_module_release(entry->vm_module);
            entry->vm_module = NULL;
            continue;
        }
        
        all_modules[module_count++] = entry->vm_module;
        entry->perf_stats.latency_min_us = UINT64_MAX;
        entry->perf_stats.latency_max_us = 0;
        entry->perf_stats.latency_min_cycles = UINT64_MAX;
        entry->perf_stats.latency_max_cycles = 0;
        entry->perf_stats.latency_min_instructions = UINT64_MAX;
        entry->perf_stats.latency_max_instructions = 0;
        entry->perf_stats.arena_peak_bytes = desc->peak_memory;
        
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

    if (static_hal_module) {
        iree_vm_module_release(static_hal_module);
    }
    if (inline_hal_module) {
        iree_vm_module_release(inline_hal_module);
    }

    if (!iree_status_is_ok(status)) {
        ai_log_status("vm_context_create_with_modules", status);
        printf("[AI] ERROR: Failed to create global VM context\n");
        ai_registry_release_resources();
        return -1;
    }
    
    printf("[AI] Runtime initialized with %d models\n", g_registry.count);
    return 0;
}

void ai_runtime_deinit(void) {
    ai_registry_release_resources();
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
    entry->perf_stats.latency_min_cycles = UINT64_MAX;
    entry->perf_stats.latency_min_instructions = UINT64_MAX;
}

int ai_model_list(const char **names, int max_count) {
    int count = g_registry.count < max_count ? g_registry.count : max_count;
    for (int i = 0; i < count; i++) {
        names[i] = g_registry.models[i].descriptor->name;
    }
    return g_registry.count;
}

static iree_hal_element_type_t ai_dtype_to_iree(ai_dtype_t dtype) {
    switch (dtype) {
        case AI_DTYPE_INT8:   return IREE_HAL_ELEMENT_TYPE_SINT_8;
        case AI_DTYPE_UINT8:  return IREE_HAL_ELEMENT_TYPE_UINT_8;
        case AI_DTYPE_INT16:  return IREE_HAL_ELEMENT_TYPE_SINT_16;
        case AI_DTYPE_INT32:  return IREE_HAL_ELEMENT_TYPE_SINT_32;
        case AI_DTYPE_FP32:   return IREE_HAL_ELEMENT_TYPE_FLOAT_32;
        default:              return IREE_HAL_ELEMENT_TYPE_NONE;
    }
}

#if AI_IREE_ENABLE_INLINE_HAL
static iree_status_t ai_allocate_inline_input_view(
    const ai_tensor_t *tensor,
    iree_hal_buffer_params_t params,
    iree_hal_buffer_view_t **out_view) {
    *out_view = NULL;
    if (!g_registry.inline_allocator) {
        return iree_make_status(IREE_STATUS_UNKNOWN,
                                "inline HAL allocator is not initialized");
    }

    iree_device_size_t allocation_size = 0;
    IREE_RETURN_IF_ERROR(iree_hal_buffer_compute_view_size(
        tensor->ndim, tensor->shape, ai_dtype_to_iree(tensor->dtype),
        IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR, &allocation_size));

    iree_hal_buffer_t *buffer = NULL;
    IREE_RETURN_IF_ERROR(iree_hal_allocator_allocate_buffer(
        g_registry.inline_allocator, params, allocation_size, &buffer));

    iree_hal_buffer_mapping_t mapping;
    iree_status_t status = iree_hal_buffer_map_range(
        buffer, IREE_HAL_MAPPING_MODE_SCOPED,
        IREE_HAL_MEMORY_ACCESS_DISCARD_WRITE, 0, AI_IREE_WHOLE_BUFFER,
        &mapping);
    if (iree_status_is_ok(status)) {
        size_t copy_size = mapping.contents.data_length < tensor->size ?
                           mapping.contents.data_length : tensor->size;
        memcpy(mapping.contents.data, tensor->data, copy_size);
        status = iree_hal_buffer_unmap_range(&mapping);
    }

    if (iree_status_is_ok(status)) {
        status = iree_hal_buffer_view_create(
            buffer, tensor->ndim, tensor->shape, ai_dtype_to_iree(tensor->dtype),
            IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
            iree_hal_allocator_host_allocator(g_registry.inline_allocator),
            out_view);
    }

    iree_hal_buffer_release(buffer);
    return status;
}
#endif

static iree_status_t ai_allocate_input_view(
    const ai_model_entry_t *entry,
    const ai_tensor_t *tensor,
    iree_hal_buffer_params_t params,
    iree_hal_buffer_view_t **out_view) {
#if AI_IREE_ENABLE_INLINE_HAL
    if (entry->hal_backend == AI_IREE_HAL_BACKEND_INLINE) {
        return ai_allocate_inline_input_view(tensor, params, out_view);
    }
#else
    (void)entry;
#endif

#if AI_IREE_ENABLE_STATIC_LIBRARY_HAL
    if (!g_registry.hal_device) {
        return iree_make_status(IREE_STATUS_UNKNOWN,
                                "static-library HAL device is not initialized");
    }
    return iree_hal_buffer_view_allocate_buffer_copy(
        g_registry.hal_device,
        iree_hal_device_allocator(g_registry.hal_device),
        tensor->ndim, tensor->shape,
        ai_dtype_to_iree(tensor->dtype),
        IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        params,
        iree_make_const_byte_span(tensor->data, tensor->size),
        out_view);
#else
    (void)tensor;
    (void)params;
    (void)out_view;
    return iree_make_status(IREE_STATUS_UNKNOWN,
                            "static-library HAL support is not compiled in");
#endif
}

int ai_infer_sync(ai_model_handle_t handle,
                  const ai_tensor_t *inputs, uint32_t num_inputs,
                  ai_tensor_t *outputs, uint32_t num_outputs,
                  uint32_t timeout_ms) {
    (void)timeout_ms;

    if (!handle || !inputs || !outputs) {
        return -1;
    }
    
    ai_model_entry_t *entry = (ai_model_entry_t*)handle;
    if (!g_registry.vm_context) return -1;
    
    uint64_t start_us = ai_get_time_us();
    uint64_t start_cycles = ai_get_cycle_count();
    uint64_t start_instructions = ai_get_instruction_count();
    int ret = -1;
    
    iree_hal_buffer_params_t params = {
        .usage = IREE_HAL_BUFFER_USAGE_TRANSFER |
                 IREE_HAL_BUFFER_USAGE_DISPATCH |
                 IREE_HAL_BUFFER_USAGE_MAPPING,
        .access = IREE_HAL_MEMORY_ACCESS_ALL,
        .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE
    };
    
    // 1. Prepare Inputs
    iree_vm_list_t *input_list = NULL;
    iree_vm_list_t *output_list = NULL;
    iree_status_t status = iree_vm_list_create(
        g_registry.buffer_view_type, num_inputs, g_registry.allocator, &input_list);
    if (!iree_status_is_ok(status)) {
        ai_log_status("input list create", status);
        return -1;
    }

    for (uint32_t i = 0; i < num_inputs; i++) {
        iree_hal_buffer_view_t *view = NULL;
        status = ai_allocate_input_view(entry, &inputs[i], params, &view);
        if (!iree_status_is_ok(status)) {
            ai_log_status("input buffer view allocate", status);
            goto cleanup;
        }
        iree_vm_ref_t ref = iree_hal_buffer_view_move_ref(view);
        status = iree_vm_list_push_ref_move(input_list, &ref);
        if (!iree_status_is_ok(status)) {
            ai_log_status("input list push", status);
            iree_vm_ref_release(&ref);
            goto cleanup;
        }
    }
    
    // 2. Prepare Outputs
    status = iree_vm_list_create(
        g_registry.buffer_view_type, num_outputs, g_registry.allocator, &output_list);
    if (!iree_status_is_ok(status)) {
        ai_log_status("output list create", status);
        goto cleanup;
    }
    
    // 3. Invoke
    status = iree_vm_invoke(
        g_registry.vm_context, entry->main_func, IREE_VM_INVOCATION_FLAG_NONE,
        NULL, input_list, output_list, g_registry.allocator);
    
    if (!iree_status_is_ok(status)) {
        ai_log_status("vm_invoke", status);
        goto cleanup;
    }
    
    // 4. Copy results back
    for (uint32_t i = 0; i < num_outputs; i++) {
        iree_vm_ref_t ref = iree_vm_ref_null();
        status = iree_vm_list_get_ref_retain(output_list, i, &ref);
        if (!iree_status_is_ok(status)) {
            ai_log_status("output list get", status);
            goto cleanup;
        }
        iree_hal_buffer_view_t *view = iree_hal_buffer_view_deref(ref);
        if (!view) {
            printf("[AI] ERROR: output buffer view dereference failed\n");
            iree_vm_ref_release(&ref);
            goto cleanup;
        }

        iree_hal_buffer_t *buffer = iree_hal_buffer_view_buffer(view);
        iree_hal_buffer_mapping_t mapping;
        status = iree_hal_buffer_map_range(
            buffer, IREE_HAL_MAPPING_MODE_SCOPED, IREE_HAL_MEMORY_ACCESS_READ,
            0, AI_IREE_WHOLE_BUFFER, &mapping);
        if (!iree_status_is_ok(status)) {
            ai_log_status("output buffer map", status);
            iree_vm_ref_release(&ref);
            goto cleanup;
        }

        size_t copy_size = mapping.contents.data_length < outputs[i].size ?
                           mapping.contents.data_length : outputs[i].size;
        memcpy(outputs[i].data, mapping.contents.data, copy_size);
        iree_hal_buffer_unmap_range(&mapping);
        iree_vm_ref_release(&ref);
    }

    uint64_t elapsed_us = ai_get_time_us() - start_us;
    uint64_t elapsed_cycles = ai_get_cycle_count() - start_cycles;
    uint64_t elapsed_instructions = ai_get_instruction_count() - start_instructions;
    
    entry->perf_stats.total_inferences++;
    entry->perf_stats.latency_last_us = elapsed_us;
    if (elapsed_us < entry->perf_stats.latency_min_us) entry->perf_stats.latency_min_us = elapsed_us;
    if (elapsed_us > entry->perf_stats.latency_max_us) entry->perf_stats.latency_max_us = elapsed_us;
    entry->perf_stats.latency_last_cycles = elapsed_cycles;
    if (elapsed_cycles < entry->perf_stats.latency_min_cycles) entry->perf_stats.latency_min_cycles = elapsed_cycles;
    if (elapsed_cycles > entry->perf_stats.latency_max_cycles) entry->perf_stats.latency_max_cycles = elapsed_cycles;
    entry->perf_stats.latency_last_instructions = elapsed_instructions;
    if (elapsed_instructions < entry->perf_stats.latency_min_instructions) entry->perf_stats.latency_min_instructions = elapsed_instructions;
    if (elapsed_instructions > entry->perf_stats.latency_max_instructions) entry->perf_stats.latency_max_instructions = elapsed_instructions;
    ret = 0;

cleanup:
    if (input_list) {
        iree_vm_list_release(input_list);
    }
    if (output_list) {
        iree_vm_list_release(output_list);
    }
    return ret;
}

int ai_infer_async(ai_model_handle_t handle,
                   const ai_tensor_t *inputs, uint32_t num_inputs,
                   ai_tensor_t *outputs, uint32_t num_outputs,
                   ai_inference_callback_t callback,
                   void *user_data) {
    // TODO: Implement async inference with work queue
    // For now, just run sync
    int ret = ai_infer_sync(handle, inputs, num_inputs, outputs, num_outputs, 1000);
    if (callback) {
        callback(handle, outputs, ret, user_data);
    }
    return ret;
}
