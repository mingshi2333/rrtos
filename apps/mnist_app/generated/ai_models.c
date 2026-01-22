#include "ai_models.h"
#include "ai_model_registry.h"
#include "iree/vm/api.h"
#include "iree/hal/local/executable_library.h"
#include <string.h>

/* --- Wrappers and Descriptors --- */
#include "st_mnist_28.h"

static iree_status_t st_mnist_28_create_wrapper(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module) {
    return st_mnist_28_create(instance, allocator, out_module);
}

extern const iree_hal_executable_library_header_t** st_mnist_28_linked_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t* environment);

static const ai_emitc_model_descriptor_t st_mnist_28_descriptor = {
    .name = "st_mnist_28",
    .module_create_fn = st_mnist_28_create_wrapper,
    .library_query_fn = (const void*)st_mnist_28_linked_library_query,
    .inputs = {
        { .dims = {1, 28, 28, 1}, .ndim = 4, .dtype = AI_DTYPE_UINT8 },
    },
    .outputs = {
        { .dims = {1, 36}, .ndim = 2, .dtype = AI_DTYPE_FP32 },
    },
    .num_inputs = 1,
    .num_outputs = 1
};

const ai_emitc_model_descriptor_t *g_emitc_models[] = {
    &st_mnist_28_descriptor,
    NULL
};

/* --- Helper Functions --- */

int ai_st_mnist_28_run(const ai_st_mnist_28_input_t *input, ai_st_mnist_28_output_t *output) {
    ai_model_handle_t handle = ai_model_find_by_name("st_mnist_28");
    if (!handle) return -1;

    ai_tensor_t in_tensors[1];
    in_tensors[0].data = (void*)input->tensor_0;
    { uint32_t s[] = {1, 28, 28, 1}; memcpy(in_tensors[0].shape, s, sizeof(s)); }
    in_tensors[0].ndim = 4;
    in_tensors[0].dtype = AI_DTYPE_UINT8;
    in_tensors[0].size = sizeof(input->tensor_0);

    ai_tensor_t out_tensors[1];
    out_tensors[0].data = (void*)output->tensor_0;
    { uint32_t s[] = {1, 36}; memcpy(out_tensors[0].shape, s, sizeof(s)); }
    out_tensors[0].ndim = 2;
    out_tensors[0].dtype = AI_DTYPE_FP32;
    out_tensors[0].size = sizeof(output->tensor_0);

    return ai_infer_sync(handle, in_tensors, 1, out_tensors, 1, 1000);
}
