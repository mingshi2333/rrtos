#include "ai_backend.h"
#include "os_kernel.h"
#include "os_mem.h"

#if OS_CFG_AI_EN

#if OS_CFG_AI_BACKEND_IREE

#include "iree/base/api.h"
#include "iree/vm/api.h"
#include "iree/vm/shims_emitc.h"
#include "iree/hal/drivers/local_sync/sync_driver.h"
#include "iree/modules/hal/module.h"

// Extern from generated C module
iree_status_t module_create(iree_vm_instance_t* instance, iree_allocator_t allocator, iree_vm_module_t** out_module);

typedef struct {
    iree_vm_instance_t* instance;
    iree_vm_context_t* context;
    iree_vm_module_t* main_module;
    iree_allocator_t allocator;
} iree_ctx_t;

static iree_ctx_t g_iree_ctx;

static ai_err_t iree_backend_init(void *arena, uint32_t arena_size) {
    (void)arena;
    (void)arena_size;

    g_iree_ctx.allocator = iree_allocator_system();

    iree_status_t status = iree_vm_instance_create(IREE_VM_TYPE_CAPACITY_DEFAULT,
                                                   g_iree_ctx.allocator,
                                                   &g_iree_ctx.instance);
    if (!iree_status_is_ok(status)) return AI_ERR_INIT_FAILED;

    return AI_ERR_OK;
}

static void iree_backend_deinit(void) {
    if (g_iree_ctx.instance) {
        iree_vm_instance_release(g_iree_ctx.instance);
        g_iree_ctx.instance = NULL;
    }
}

static ai_err_t iree_backend_load(ai_model_t **model, const void *model_data, uint32_t size) {
    (void)model_data;
    (void)size;

    iree_status_t status;
    iree_hal_driver_t* driver = NULL;
    iree_hal_device_t* device = NULL;
    iree_vm_module_t* hal_module = NULL;

    iree_hal_sync_device_params_t params;
    iree_hal_sync_device_params_initialize(&params);

    status = iree_hal_sync_driver_create(
        iree_make_cstring_view("sync"), &params,
        0, NULL, NULL, g_iree_ctx.allocator, &driver);
    if (!iree_status_is_ok(status)) return AI_ERR_INIT_FAILED;

    status = iree_hal_driver_create_default_device(driver, g_iree_ctx.allocator, &device);
    if (!iree_status_is_ok(status)) {
        iree_hal_driver_release(driver);
        return AI_ERR_INIT_FAILED;
    }

    status = iree_hal_module_create(
        g_iree_ctx.instance,
        1, &device,
        IREE_HAL_MODULE_FLAG_NONE,
        iree_hal_module_debug_sink_null(),
        g_iree_ctx.allocator, &hal_module);
    if (!iree_status_is_ok(status)) {
        iree_hal_device_release(device);
        iree_hal_driver_release(driver);
        return AI_ERR_INIT_FAILED;
    }

    status = module_create(g_iree_ctx.instance, g_iree_ctx.allocator, &g_iree_ctx.main_module);
    if (!iree_status_is_ok(status)) {
        iree_vm_module_release(hal_module);
        iree_hal_device_release(device);
        iree_hal_driver_release(driver);
        return AI_ERR_INVALID;
    }

    iree_vm_module_t* modules[] = {hal_module, g_iree_ctx.main_module};
    status = iree_vm_context_create_with_modules(
        g_iree_ctx.instance, IREE_VM_CONTEXT_FLAG_NONE,
        IREE_ARRAYSIZE(modules), modules,
        g_iree_ctx.allocator, &g_iree_ctx.context);

    if (!iree_status_is_ok(status)) {
        iree_vm_module_release(g_iree_ctx.main_module);
        iree_vm_module_release(hal_module);
        iree_hal_device_release(device);
        iree_hal_driver_release(driver);
        return AI_ERR_INIT_FAILED;
    }

    iree_vm_module_release(hal_module);
    iree_hal_device_release(device);
    iree_hal_driver_release(driver);

    *model = (ai_model_t*)g_iree_ctx.context; // Store context as handle
    return AI_ERR_OK;
}

static void iree_backend_unload(ai_model_t *model) {
    (void)model;
    if (g_iree_ctx.context) {
        iree_vm_context_release(g_iree_ctx.context);
        g_iree_ctx.context = NULL;
    }
    if (g_iree_ctx.main_module) {
        iree_vm_module_release(g_iree_ctx.main_module);
        g_iree_ctx.main_module = NULL;
    }
}

static ai_err_t iree_backend_invoke(ai_model_t *model) {
    iree_vm_context_t *ctx = (iree_vm_context_t *)model;
    iree_vm_function_t main_fn;
    
    // Find "main" function
    iree_status_t status = iree_vm_context_resolve_function(
        ctx, iree_make_string_view("module.main", 11), &main_fn);
    if (!iree_status_is_ok(status)) return AI_ERR_OP_NOT_FOUND;

    // Create inputs (example: 4xf32 tensor)
    iree_vm_list_t *inputs = NULL;
    iree_vm_list_t *outputs = NULL;
    
    // Simplified: invoke with empty args for now to test runtime path
    // In real scenario, would create hal_buffer_view here
    
    status = iree_vm_list_create(iree_vm_make_undefined_type_def(), 1, g_iree_ctx.allocator, &inputs);
    if (iree_status_is_ok(status)) {
        status = iree_vm_list_create(iree_vm_make_undefined_type_def(), 1, g_iree_ctx.allocator, &outputs);
    }
    
    if (iree_status_is_ok(status)) {
        status = iree_vm_invoke(ctx, main_fn, IREE_VM_INVOCATION_FLAG_NONE,
                                NULL, inputs, outputs, g_iree_ctx.allocator);
    }

    if (inputs) iree_vm_list_release(inputs);
    if (outputs) iree_vm_list_release(outputs);

    return iree_status_is_ok(status) ? AI_ERR_OK : AI_ERR_INTERNAL;
}

static const ai_backend_ops_t g_iree_ops = {
    .name = "iree",
    .init = iree_backend_init,
    .deinit = iree_backend_deinit,
    .load = iree_backend_load,
    .unload = iree_backend_unload,
    .invoke = iree_backend_invoke,
};

const ai_backend_ops_t *ai_backend_iree_ops(void) {
    return &g_iree_ops;
}

#else

const ai_backend_ops_t *ai_backend_iree_ops(void) {
    return NULL;
}

#endif

const ai_backend_ops_t *ai_backend_native_ops(void) {
    return NULL; // Stub for now
}

#endif
