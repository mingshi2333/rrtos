import subprocess
import tempfile
import textwrap
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]


VM_API_H = r"""
#ifndef IREE_VM_API_H
#define IREE_VM_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int iree_status_t;
typedef size_t iree_host_size_t;
typedef size_t iree_device_size_t;
typedef uintptr_t iree_vm_ref_type_t;

#define IREE_STATUS_OK 0
#define IREE_STATUS_UNKNOWN 2
#define IREE_VM_TYPE_CAPACITY_DEFAULT 0
#define IREE_VM_FUNCTION_LINKAGE_EXPORT 1
#define IREE_VM_CONTEXT_FLAG_NONE 0
#define IREE_VM_INVOCATION_FLAG_NONE 0

typedef struct {
    const char *data;
    size_t size;
} iree_string_view_t;

typedef struct {
    void *data;
    size_t data_length;
} iree_byte_span_t;

typedef iree_byte_span_t iree_const_byte_span_t;

typedef struct {
    void *self;
} iree_allocator_t;

typedef struct iree_vm_instance_t iree_vm_instance_t;
typedef struct iree_vm_module_t iree_vm_module_t;
typedef struct iree_vm_context_t iree_vm_context_t;
typedef struct iree_vm_list_t iree_vm_list_t;

typedef struct {
    int linkage;
    int ordinal;
} iree_vm_function_t;

typedef struct {
    iree_vm_ref_type_t type;
} iree_vm_type_def_t;

typedef struct {
    iree_vm_ref_type_t type;
    void *ptr;
} iree_vm_ref_t;

static inline bool iree_status_is_ok(iree_status_t status) {
    return status == IREE_STATUS_OK;
}

static inline iree_status_t iree_ok_status(void) {
    return IREE_STATUS_OK;
}

static inline iree_status_t iree_make_status(int code, const char *message) {
    (void)message;
    return code;
}

static inline iree_allocator_t iree_allocator_system(void) {
    iree_allocator_t allocator = {0};
    return allocator;
}

static inline iree_string_view_t iree_make_cstring_view(const char *value) {
    iree_string_view_t view = {value, 0};
    while (value && value[view.size]) {
        view.size++;
    }
    return view;
}

static inline iree_const_byte_span_t iree_make_const_byte_span(const void *data,
                                                               size_t length) {
    iree_const_byte_span_t span = {(void *)data, length};
    return span;
}

static inline iree_vm_type_def_t iree_vm_make_ref_type_def(iree_vm_ref_type_t type) {
    iree_vm_type_def_t def = {type};
    return def;
}

static inline iree_vm_ref_t iree_vm_ref_null(void) {
    iree_vm_ref_t ref = {0, 0};
    return ref;
}

int iree_status_format(iree_status_t status, iree_host_size_t buffer_capacity,
                       char *buffer, iree_host_size_t *out_length);
const char *iree_status_code_string(int code);
int iree_status_code(iree_status_t status);

iree_status_t iree_vm_instance_create(int type_capacity, iree_allocator_t allocator,
                                      iree_vm_instance_t **out_instance);
void iree_vm_instance_retain(iree_vm_instance_t *instance);
void iree_vm_instance_release(iree_vm_instance_t *instance);

iree_status_t iree_vm_module_lookup_function_by_name(
    iree_vm_module_t *module, int linkage, iree_string_view_t name,
    iree_vm_function_t *out_function);
void iree_vm_module_retain(iree_vm_module_t *module);
void iree_vm_module_release(iree_vm_module_t *module);

iree_status_t iree_vm_context_create_with_modules(
    iree_vm_instance_t *instance, int flags, iree_host_size_t module_count,
    iree_vm_module_t **modules, iree_allocator_t allocator,
    iree_vm_context_t **out_context);
void iree_vm_context_release(iree_vm_context_t *context);

iree_status_t iree_vm_list_create(iree_vm_type_def_t element_type,
                                  iree_host_size_t capacity,
                                  iree_allocator_t allocator,
                                  iree_vm_list_t **out_list);
void iree_vm_list_release(iree_vm_list_t *list);
iree_status_t iree_vm_list_push_ref_move(iree_vm_list_t *list,
                                         iree_vm_ref_t *value);
iree_status_t iree_vm_list_get_ref_retain(const iree_vm_list_t *list,
                                          iree_host_size_t i,
                                          iree_vm_ref_t *out_value);
void iree_vm_ref_release(iree_vm_ref_t *ref);

iree_status_t iree_vm_invoke(iree_vm_context_t *context, iree_vm_function_t function,
                             int flags, void *policy, iree_vm_list_t *inputs,
                             iree_vm_list_t *outputs, iree_allocator_t allocator);

#endif
"""


HAL_API_H = r"""
#ifndef IREE_HAL_API_H
#define IREE_HAL_API_H

#include "iree/vm/api.h"

typedef struct iree_hal_device_t iree_hal_device_t;
typedef struct iree_hal_allocator_t iree_hal_allocator_t;
typedef struct iree_hal_executable_loader_t iree_hal_executable_loader_t;
typedef struct iree_hal_buffer_view_t iree_hal_buffer_view_t;
typedef struct iree_hal_buffer_t iree_hal_buffer_t;
typedef void (*iree_hal_executable_library_query_fn_t)(void);
typedef int iree_hal_executable_import_provider_t;
typedef int iree_hal_element_type_t;

typedef struct {
    uint32_t usage;
    uint32_t access;
    uint32_t type;
} iree_hal_buffer_params_t;

typedef struct {
    iree_byte_span_t contents;
} iree_hal_buffer_mapping_t;

#define IREE_HAL_ELEMENT_TYPE_NONE 0
#define IREE_HAL_ELEMENT_TYPE_SINT_8 1
#define IREE_HAL_ELEMENT_TYPE_UINT_8 2
#define IREE_HAL_ELEMENT_TYPE_SINT_16 3
#define IREE_HAL_ELEMENT_TYPE_SINT_32 4
#define IREE_HAL_ELEMENT_TYPE_FLOAT_32 5
#define IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR 1
#define IREE_HAL_BUFFER_USAGE_TRANSFER 0x1u
#define IREE_HAL_BUFFER_USAGE_DISPATCH 0x2u
#define IREE_HAL_BUFFER_USAGE_MAPPING 0x4u
#define IREE_HAL_MEMORY_ACCESS_ALL 0xFFFFFFFFu
#define IREE_HAL_MEMORY_ACCESS_READ 0x1u
#define IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL 0x1u
#define IREE_HAL_MEMORY_TYPE_HOST_VISIBLE 0x2u
#define IREE_HAL_MAPPING_MODE_SCOPED 1
#define IREE_WHOLE_BUFFER ((iree_device_size_t)-1)
#define IREE_HAL_MODULE_FLAG_SYNCHRONOUS 1

iree_hal_executable_import_provider_t iree_hal_executable_import_provider_null(void);
void iree_hal_executable_loader_retain(iree_hal_executable_loader_t *loader);
void iree_hal_executable_loader_release(iree_hal_executable_loader_t *loader);
void iree_hal_device_release(iree_hal_device_t *device);
iree_hal_allocator_t *iree_hal_device_allocator(iree_hal_device_t *device);
iree_status_t iree_hal_allocator_create_heap(
    iree_string_view_t identifier, iree_allocator_t host_allocator,
    iree_allocator_t device_allocator, iree_hal_allocator_t **out_allocator);
void iree_hal_allocator_retain(iree_hal_allocator_t *allocator);
void iree_hal_allocator_release(iree_hal_allocator_t *allocator);
iree_status_t iree_hal_buffer_view_allocate_buffer_copy(
    iree_hal_device_t *device, iree_hal_allocator_t *allocator, uint32_t shape_rank,
    const uint32_t *shape, iree_hal_element_type_t element_type, int encoding_type,
    iree_hal_buffer_params_t params, iree_const_byte_span_t source,
    iree_hal_buffer_view_t **out_view);
iree_vm_ref_t iree_hal_buffer_view_move_ref(iree_hal_buffer_view_t *view);
iree_vm_ref_type_t iree_hal_buffer_view_type(void);
iree_hal_buffer_view_t *iree_hal_buffer_view_deref(iree_vm_ref_t ref);
iree_hal_buffer_t *iree_hal_buffer_view_buffer(iree_hal_buffer_view_t *view);
iree_status_t iree_hal_buffer_map_range(
    iree_hal_buffer_t *buffer, int mapping_mode, uint32_t memory_access,
    iree_device_size_t byte_offset, iree_device_size_t byte_length,
    iree_hal_buffer_mapping_t *out_mapping);
void iree_hal_buffer_unmap_range(iree_hal_buffer_mapping_t *mapping);

#endif
"""


MODULE_H = r"""
#ifndef IREE_MODULES_HAL_MODULE_H
#define IREE_MODULES_HAL_MODULE_H

#include "iree/hal/api.h"

typedef int iree_hal_module_device_policy_t;
typedef int iree_hal_module_debug_sink_t;

iree_status_t iree_hal_module_register_all_types(iree_vm_instance_t *instance);
iree_hal_module_debug_sink_t iree_hal_module_debug_sink_null(void);
iree_status_t iree_hal_module_create(
    iree_vm_instance_t *instance, iree_host_size_t device_count,
    iree_hal_device_t **devices, int flags,
    iree_hal_module_debug_sink_t debug_sink, iree_allocator_t allocator,
    iree_vm_module_t **out_module);

#endif
"""


SYNC_DEVICE_H = r"""
#ifndef IREE_HAL_DRIVERS_LOCAL_SYNC_SYNC_DEVICE_H
#define IREE_HAL_DRIVERS_LOCAL_SYNC_SYNC_DEVICE_H

#include "iree/hal/api.h"

typedef struct {
    int reserved;
} iree_hal_sync_device_params_t;

void iree_hal_sync_device_params_initialize(iree_hal_sync_device_params_t *params);
iree_status_t iree_hal_sync_device_create(
    iree_string_view_t identifier, const iree_hal_sync_device_params_t *params,
    iree_host_size_t loader_count, iree_hal_executable_loader_t **loaders,
    iree_hal_allocator_t *device_allocator, iree_allocator_t host_allocator,
    iree_hal_device_t **out_device);

#endif
"""


STATIC_LIBRARY_LOADER_H = r"""
#ifndef IREE_HAL_LOCAL_LOADERS_STATIC_LIBRARY_LOADER_H
#define IREE_HAL_LOCAL_LOADERS_STATIC_LIBRARY_LOADER_H

#include "iree/hal/api.h"

iree_status_t iree_hal_static_library_loader_create(
    iree_host_size_t library_count,
    const iree_hal_executable_library_query_fn_t *libraries,
    iree_hal_executable_import_provider_t import_provider,
    iree_allocator_t host_allocator, iree_hal_executable_loader_t **out_loader);

#endif
"""


HARNESS_C = r"""
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ai/src/ai_model_registry.c"

struct iree_vm_instance_t { int id; };
struct iree_vm_module_t { int id; };
struct iree_vm_context_t { int id; };
struct iree_vm_list_t { int id; };
struct iree_hal_device_t { int id; };
struct iree_hal_allocator_t { int id; };
struct iree_hal_executable_loader_t { int id; };
struct iree_hal_buffer_view_t { int id; };
struct iree_hal_buffer_t { int id; };

static struct iree_vm_instance_t g_instance;
static struct iree_vm_module_t g_hal_module;
static struct iree_vm_module_t g_model_module;
static struct iree_vm_context_t g_context;
static struct iree_vm_list_t g_input_list;
static struct iree_vm_list_t g_output_list;
static struct iree_hal_device_t g_device;
static struct iree_hal_allocator_t g_allocator;
static struct iree_hal_executable_loader_t g_loader;
static struct iree_hal_buffer_view_t g_view;
static struct iree_hal_buffer_t g_buffer;
static uint8_t g_mapped_output[4] = {1, 2, 3, 4};

static int g_instance_refs;
static int g_loader_refs;
static int g_device_refs;
static int g_model_module_refs;
static int g_hal_module_refs;
static int g_context_refs;
static int g_allocator_refs;
static int g_list_releases;
static int g_ref_releases;
static uint64_t g_fake_time;

static int g_fail_hal_module_create;
static int g_fail_push_ref_move;
static int g_fail_get_ref;
static int g_fail_map_range;
static char g_sync_device_identifier[32];

static void reset_fakes(void) {
    memset(&g_registry, 0, sizeof(g_registry));
    g_instance_refs = 0;
    g_loader_refs = 0;
    g_device_refs = 0;
    g_model_module_refs = 0;
    g_hal_module_refs = 0;
    g_context_refs = 0;
    g_allocator_refs = 0;
    g_list_releases = 0;
    g_ref_releases = 0;
    g_fake_time = 1000;
    g_fail_hal_module_create = 0;
    g_fail_push_ref_move = 0;
    g_fail_get_ref = 0;
    g_fail_map_range = 0;
    memset(g_sync_device_identifier, 0, sizeof(g_sync_device_identifier));
}

static int assert_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "ASSERTION FAILED: %s\n", message);
        return 1;
    }
    return 0;
}

uint64_t hal_clint_mtime_get(void) {
    g_fake_time += 10;
    return g_fake_time;
}

int iree_status_format(iree_status_t status, iree_host_size_t buffer_capacity,
                       char *buffer, iree_host_size_t *out_length) {
    int written = snprintf(buffer, buffer_capacity, "status=%d", status);
    if (out_length) {
        *out_length = written > 0 ? (iree_host_size_t)written : 0;
    }
    return 1;
}

const char *iree_status_code_string(int code) {
    (void)code;
    return "fake-error";
}

int iree_status_code(iree_status_t status) {
    return status;
}

iree_status_t iree_vm_instance_create(int type_capacity, iree_allocator_t allocator,
                                      iree_vm_instance_t **out_instance) {
    (void)type_capacity;
    (void)allocator;
    g_instance_refs = 1;
    *out_instance = &g_instance;
    return IREE_STATUS_OK;
}

void iree_vm_instance_retain(iree_vm_instance_t *instance) {
    (void)instance;
    g_instance_refs++;
}

void iree_vm_instance_release(iree_vm_instance_t *instance) {
    (void)instance;
    g_instance_refs--;
}

iree_status_t iree_vm_module_lookup_function_by_name(
    iree_vm_module_t *module, int linkage, iree_string_view_t name,
    iree_vm_function_t *out_function) {
    (void)module;
    (void)linkage;
    (void)name;
    out_function->ordinal = 7;
    return IREE_STATUS_OK;
}

void iree_vm_module_retain(iree_vm_module_t *module) {
    if (module == &g_model_module) {
        g_model_module_refs++;
    } else if (module == &g_hal_module) {
        g_hal_module_refs++;
    }
}

void iree_vm_module_release(iree_vm_module_t *module) {
    if (module == &g_model_module) {
        g_model_module_refs--;
    } else if (module == &g_hal_module) {
        g_hal_module_refs--;
    }
}

iree_status_t iree_vm_context_create_with_modules(
    iree_vm_instance_t *instance, int flags, iree_host_size_t module_count,
    iree_vm_module_t **modules, iree_allocator_t allocator,
    iree_vm_context_t **out_context) {
    (void)flags;
    (void)allocator;
    iree_vm_instance_retain(instance);
    for (iree_host_size_t i = 0; i < module_count; i++) {
        iree_vm_module_retain(modules[i]);
    }
    g_context_refs = 1;
    *out_context = &g_context;
    return IREE_STATUS_OK;
}

void iree_vm_context_release(iree_vm_context_t *context) {
    (void)context;
    g_context_refs--;
    iree_vm_instance_release(&g_instance);
    iree_vm_module_release(&g_hal_module);
    iree_vm_module_release(&g_model_module);
}

iree_status_t iree_vm_list_create(iree_vm_type_def_t element_type,
                                  iree_host_size_t capacity,
                                  iree_allocator_t allocator,
                                  iree_vm_list_t **out_list) {
    (void)element_type;
    (void)capacity;
    (void)allocator;
    *out_list = *out_list == &g_input_list ? &g_output_list : &g_input_list;
    return IREE_STATUS_OK;
}

void iree_vm_list_release(iree_vm_list_t *list) {
    (void)list;
    g_list_releases++;
}

iree_status_t iree_vm_list_push_ref_move(iree_vm_list_t *list,
                                         iree_vm_ref_t *value) {
    (void)list;
    if (g_fail_push_ref_move) {
        return IREE_STATUS_UNKNOWN;
    }
    value->ptr = NULL;
    return IREE_STATUS_OK;
}

iree_status_t iree_vm_list_get_ref_retain(const iree_vm_list_t *list,
                                          iree_host_size_t i,
                                          iree_vm_ref_t *out_value) {
    (void)list;
    (void)i;
    if (g_fail_get_ref) {
        return IREE_STATUS_UNKNOWN;
    }
    out_value->type = 42;
    out_value->ptr = &g_view;
    return IREE_STATUS_OK;
}

void iree_vm_ref_release(iree_vm_ref_t *ref) {
    if (ref && ref->ptr) {
        g_ref_releases++;
    }
    if (ref) {
        ref->ptr = NULL;
    }
}

iree_status_t iree_vm_invoke(iree_vm_context_t *context, iree_vm_function_t function,
                             int flags, void *policy, iree_vm_list_t *inputs,
                             iree_vm_list_t *outputs, iree_allocator_t allocator) {
    (void)context;
    (void)function;
    (void)flags;
    (void)policy;
    (void)inputs;
    (void)outputs;
    (void)allocator;
    return IREE_STATUS_OK;
}

iree_hal_executable_import_provider_t iree_hal_executable_import_provider_null(void) {
    return 0;
}

void iree_hal_executable_loader_retain(iree_hal_executable_loader_t *loader) {
    (void)loader;
    g_loader_refs++;
}

void iree_hal_executable_loader_release(iree_hal_executable_loader_t *loader) {
    (void)loader;
    g_loader_refs--;
}

void iree_hal_device_release(iree_hal_device_t *device) {
    (void)device;
    g_device_refs--;
    iree_hal_executable_loader_release(&g_loader);
    iree_hal_allocator_release(&g_allocator);
}

iree_hal_allocator_t *iree_hal_device_allocator(iree_hal_device_t *device) {
    (void)device;
    return &g_allocator;
}

iree_status_t iree_hal_allocator_create_heap(
    iree_string_view_t identifier, iree_allocator_t host_allocator,
    iree_allocator_t device_allocator, iree_hal_allocator_t **out_allocator) {
    (void)identifier;
    (void)host_allocator;
    (void)device_allocator;
    g_allocator_refs = 1;
    *out_allocator = &g_allocator;
    return IREE_STATUS_OK;
}

void iree_hal_allocator_retain(iree_hal_allocator_t *allocator) {
    (void)allocator;
    g_allocator_refs++;
}

void iree_hal_allocator_release(iree_hal_allocator_t *allocator) {
    (void)allocator;
    g_allocator_refs--;
}

iree_status_t iree_hal_buffer_view_allocate_buffer_copy(
    iree_hal_device_t *device, iree_hal_allocator_t *allocator, uint32_t shape_rank,
    const uint32_t *shape, iree_hal_element_type_t element_type, int encoding_type,
    iree_hal_buffer_params_t params, iree_const_byte_span_t source,
    iree_hal_buffer_view_t **out_view) {
    (void)device;
    (void)allocator;
    (void)shape_rank;
    (void)shape;
    (void)element_type;
    (void)encoding_type;
    (void)params;
    (void)source;
    *out_view = &g_view;
    return IREE_STATUS_OK;
}

iree_vm_ref_t iree_hal_buffer_view_move_ref(iree_hal_buffer_view_t *view) {
    iree_vm_ref_t ref = {42, view};
    return ref;
}

iree_vm_ref_type_t iree_hal_buffer_view_type(void) {
    return 42;
}

iree_hal_buffer_view_t *iree_hal_buffer_view_deref(iree_vm_ref_t ref) {
    return (iree_hal_buffer_view_t *)ref.ptr;
}

iree_hal_buffer_t *iree_hal_buffer_view_buffer(iree_hal_buffer_view_t *view) {
    (void)view;
    return &g_buffer;
}

iree_status_t iree_hal_buffer_map_range(
    iree_hal_buffer_t *buffer, int mapping_mode, uint32_t memory_access,
    iree_device_size_t byte_offset, iree_device_size_t byte_length,
    iree_hal_buffer_mapping_t *out_mapping) {
    (void)buffer;
    (void)mapping_mode;
    (void)memory_access;
    (void)byte_offset;
    (void)byte_length;
    if (g_fail_map_range) {
        return IREE_STATUS_UNKNOWN;
    }
    out_mapping->contents.data = g_mapped_output;
    out_mapping->contents.data_length = sizeof(g_mapped_output);
    return IREE_STATUS_OK;
}

void iree_hal_buffer_unmap_range(iree_hal_buffer_mapping_t *mapping) {
    (void)mapping;
}

iree_status_t iree_hal_module_register_all_types(iree_vm_instance_t *instance) {
    (void)instance;
    return IREE_STATUS_OK;
}

iree_hal_module_debug_sink_t iree_hal_module_debug_sink_null(void) {
    return 0;
}

iree_status_t iree_hal_module_create(
    iree_vm_instance_t *instance, iree_host_size_t device_count,
    iree_hal_device_t **devices, int flags,
    iree_hal_module_debug_sink_t debug_sink, iree_allocator_t allocator,
    iree_vm_module_t **out_module) {
    (void)instance;
    (void)device_count;
    (void)devices;
    (void)flags;
    (void)debug_sink;
    (void)allocator;
    if (g_fail_hal_module_create) {
        return IREE_STATUS_UNKNOWN;
    }
    g_hal_module_refs = 1;
    *out_module = &g_hal_module;
    return IREE_STATUS_OK;
}

void iree_hal_sync_device_params_initialize(iree_hal_sync_device_params_t *params) {
    params->reserved = 0;
}

iree_status_t iree_hal_sync_device_create(
    iree_string_view_t identifier, const iree_hal_sync_device_params_t *params,
    iree_host_size_t loader_count, iree_hal_executable_loader_t **loaders,
    iree_hal_allocator_t *device_allocator, iree_allocator_t host_allocator,
    iree_hal_device_t **out_device) {
    (void)params;
    (void)host_allocator;
    size_t copy_length = identifier.size;
    if (copy_length >= sizeof(g_sync_device_identifier)) {
        copy_length = sizeof(g_sync_device_identifier) - 1;
    }
    memcpy(g_sync_device_identifier, identifier.data, copy_length);
    g_sync_device_identifier[copy_length] = '\0';
    g_device_refs = 1;
    iree_hal_allocator_retain(device_allocator);
    for (iree_host_size_t i = 0; i < loader_count; i++) {
        iree_hal_executable_loader_retain(loaders[i]);
    }
    *out_device = &g_device;
    return IREE_STATUS_OK;
}

iree_status_t iree_hal_static_library_loader_create(
    iree_host_size_t library_count,
    const iree_hal_executable_library_query_fn_t *libraries,
    iree_hal_executable_import_provider_t import_provider,
    iree_allocator_t host_allocator, iree_hal_executable_loader_t **out_loader) {
    (void)library_count;
    (void)libraries;
    (void)import_provider;
    (void)host_allocator;
    g_loader_refs = 1;
    *out_loader = &g_loader;
    return IREE_STATUS_OK;
}

static iree_status_t fake_model_create(iree_vm_instance_t *instance,
                                       iree_allocator_t allocator,
                                       iree_vm_module_t **out_module) {
    (void)instance;
    (void)allocator;
    g_model_module_refs = 1;
    *out_module = &g_model_module;
    return IREE_STATUS_OK;
}

static void fake_library_query(void) {}

static const ai_emitc_model_descriptor_t g_model = {
    .name = "fake_model",
    .version = "1",
    .entry_function = "main",
    .module_create_fn = fake_model_create,
    .library_query_fn = fake_library_query,
    .inputs = {{{1, 4, 1, 1}, 2, AI_DTYPE_UINT8, 1.0f, 0}},
    .outputs = {{{1, 4, 1, 1}, 2, AI_DTYPE_UINT8, 1.0f, 0}},
    .num_inputs = 1,
    .num_outputs = 1,
    .arena_size = 0,
    .peak_memory = 16,
};

const ai_emitc_model_descriptor_t *g_emitc_models[] = {&g_model, NULL};

static int init_successfully(void) {
    int rc = ai_runtime_init();
    if (rc != 0) {
        fprintf(stderr, "ai_runtime_init failed unexpectedly\n");
        return 1;
    }
    return 0;
}

static int test_init_failure_releases_partial_resources(void) {
    reset_fakes();
    g_fail_hal_module_create = 1;
    if (assert_true(ai_runtime_init() == -1, "init failure must propagate")) return 1;
    if (assert_true(g_instance_refs == 0, "vm_instance ref leaked on init failure")) return 1;
    if (assert_true(g_device_refs == 0, "hal_device ref leaked on init failure")) return 1;
    if (assert_true(g_loader_refs == 0, "loader ref leaked on init failure")) return 1;
    if (assert_true(g_allocator_refs == 0, "allocator ref leaked on init failure")) return 1;
    ai_runtime_deinit();
    if (assert_true(g_instance_refs == 0, "deinit after failed init is not idempotent")) return 1;
    return 0;
}

static int test_deinit_releases_loader_and_is_idempotent(void) {
    reset_fakes();
    if (init_successfully()) return 1;
    ai_runtime_deinit();
    if (assert_true(g_context_refs == 0, "context ref leaked after deinit")) return 1;
    if (assert_true(g_model_module_refs == 0, "model module ref leaked after deinit")) return 1;
    if (assert_true(g_hal_module_refs == 0, "hal module ref leaked after deinit")) return 1;
    if (assert_true(g_instance_refs == 0, "vm_instance ref leaked after deinit")) return 1;
    if (assert_true(g_device_refs == 0, "hal_device ref leaked after deinit")) return 1;
    if (assert_true(g_loader_refs == 0, "loader ref leaked after deinit")) return 1;
    if (assert_true(g_allocator_refs == 0, "allocator ref leaked after deinit")) return 1;
    ai_runtime_deinit();
    if (assert_true(g_loader_refs == 0, "second deinit touched released loader")) return 1;
    if (assert_true(g_device_refs == 0, "second deinit touched released device")) return 1;
    return 0;
}

static int test_sync_device_identifier_matches_iree_local_target(void) {
    reset_fakes();
    if (init_successfully()) return 1;
    if (assert_true(strcmp(g_sync_device_identifier, "local") == 0,
                    "sync device identifier must match IREE local target")) {
        return 1;
    }
    ai_runtime_deinit();
    return 0;
}

static int run_inference_with_failure(int *failure_flag) {
    uint8_t input_data[4] = {9, 8, 7, 6};
    uint8_t output_data[4] = {0};
    ai_tensor_t input = {
        .data = input_data,
        .shape = {1, 4, 1, 1},
        .ndim = 2,
        .dtype = AI_DTYPE_UINT8,
        .size = sizeof(input_data),
    };
    ai_tensor_t output = {
        .data = output_data,
        .shape = {1, 4, 1, 1},
        .ndim = 2,
        .dtype = AI_DTYPE_UINT8,
        .size = sizeof(output_data),
    };
    ai_perf_stats_t stats;
    ai_model_handle_t handle;
    int rc;

    reset_fakes();
    if (init_successfully()) return 1;
    handle = ai_model_find_by_name("fake_model");
    if (assert_true(handle != NULL, "model handle missing")) return 1;
    *failure_flag = 1;
    rc = ai_infer_sync(handle, &input, 1, &output, 1, 0);
    if (assert_true(rc == -1, "inference failure must propagate")) return 1;
    if (assert_true(ai_model_get_perf_stats(handle, &stats) == 0, "stats missing")) return 1;
    if (assert_true(stats.total_inferences == 0, "failed inference updated perf stats")) return 1;
    ai_runtime_deinit();
    return 0;
}

static int test_input_push_failure_propagates(void) {
    return run_inference_with_failure(&g_fail_push_ref_move);
}

static int test_output_get_ref_failure_propagates(void) {
    return run_inference_with_failure(&g_fail_get_ref);
}

static int test_output_map_failure_propagates(void) {
    return run_inference_with_failure(&g_fail_map_range);
}

int main(void) {
    int failures = 0;
    failures += test_init_failure_releases_partial_resources();
    failures += test_deinit_releases_loader_and_is_idempotent();
    failures += test_sync_device_identifier_matches_iree_local_target();
    failures += test_input_push_failure_propagates();
    failures += test_output_get_ref_failure_propagates();
    failures += test_output_map_failure_propagates();
    return failures == 0 ? 0 : 1;
}
"""


def write_file(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(textwrap.dedent(text).lstrip(), encoding="utf-8")


class AiModelRegistryRuntimeTest(unittest.TestCase):
    def test_cleanup_and_inference_errors_propagate(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            write_file(tmp_path / "iree/vm/api.h", VM_API_H)
            write_file(tmp_path / "iree/hal/api.h", HAL_API_H)
            write_file(tmp_path / "iree/modules/hal/module.h", MODULE_H)
            write_file(tmp_path / "iree/hal/drivers/local_sync/sync_device.h", SYNC_DEVICE_H)
            write_file(
                tmp_path / "iree/hal/local/loaders/static_library_loader.h",
                STATIC_LIBRARY_LOADER_H,
            )
            write_file(
                tmp_path / "hal_clint.h",
                "uint64_t hal_clint_mtime_get(void);\n",
            )
            write_file(
                tmp_path / "os_config.h",
                "#define OS_CFG_TIMER_FREQ_HZ 1000000UL\n",
            )
            write_file(tmp_path / "os_kernel.h", "\n")
            source = tmp_path / "ai_registry_harness.c"
            binary = tmp_path / "ai_registry_harness"
            write_file(source, HARNESS_C)

            compile_cmd = [
                "cc",
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Werror",
                "-I",
                str(tmp_path),
                "-I",
                str(PROJECT_ROOT),
                "-I",
                str(PROJECT_ROOT / "ai/include"),
                str(source),
                "-o",
                str(binary),
            ]
            subprocess.run(compile_cmd, cwd=PROJECT_ROOT, check=True)
            subprocess.run([str(binary)], cwd=PROJECT_ROOT, check=True)


if __name__ == "__main__":
    unittest.main()
