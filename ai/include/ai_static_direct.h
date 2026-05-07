#ifndef AI_STATIC_DIRECT_H
#define AI_STATIC_DIRECT_H

#include <stdint.h>

#include "iree/hal/local/executable_library.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AI_STATIC_DIRECT_OK = 0,
    AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT = -1,
    AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR = -2,
    AI_STATIC_DIRECT_ERR_INVALID_SLICE = -3,
    AI_STATIC_DIRECT_ERR_UNSUPPORTED_DISPATCH = -4,
    AI_STATIC_DIRECT_ERR_DISPATCH_FAILED = -5,
} ai_static_direct_status_t;

typedef enum {
    AI_STATIC_DIRECT_BUFFER_INPUT = 0,
    AI_STATIC_DIRECT_BUFFER_OUTPUT = 1,
    AI_STATIC_DIRECT_BUFFER_CONST = 2,
    AI_STATIC_DIRECT_BUFFER_ARENA = 3,
} ai_static_direct_buffer_base_t;

typedef struct {
    ai_static_direct_buffer_base_t base;
    uint32_t index;
    uint32_t offset;
    uint32_t length;
    uint32_t access;
} ai_static_direct_slice_t;

typedef struct {
    uint32_t ordinal;
    uint32_t workgroups[3];
    const uint32_t *constants;
    uint32_t constant_count;
    const ai_static_direct_slice_t *bindings;
    uint32_t binding_count;
} ai_static_direct_dispatch_t;

typedef enum {
    AI_STATIC_DIRECT_COMMAND_DISPATCH = 0,
    AI_STATIC_DIRECT_COMMAND_FILL = 1,
} ai_static_direct_command_type_t;

typedef struct {
    ai_static_direct_slice_t target;
    uint32_t pattern;
} ai_static_direct_fill_t;

typedef struct {
    const ai_static_direct_dispatch_t *dispatch;
} ai_static_direct_dispatch_command_t;

typedef struct {
    ai_static_direct_command_type_t type;
    union {
        ai_static_direct_dispatch_command_t dispatch;
        ai_static_direct_fill_t fill;
    } config;
} ai_static_direct_command_t;

typedef struct {
    const char *model_name;
    const char *entry_name;
    uint32_t input_bytes;
    uint32_t output_bytes;
    uint32_t const_bytes;
    uint32_t arena_bytes;
    uint32_t arena_alignment;
    const ai_static_direct_dispatch_t *dispatches;
    uint32_t dispatch_count;
    const ai_static_direct_command_t *commands;
    uint32_t command_count;
    uint32_t unsupported_feature_flags;
    const ai_static_direct_buffer_base_t *buffer_bases;
    uint32_t buffer_base_count;
} ai_static_direct_descriptor_t;

typedef struct {
    const uint8_t *input;
    uint32_t input_bytes;
    uint8_t *output;
    uint32_t output_bytes;
    const uint8_t *const_data;
    uint32_t const_bytes;
    uint8_t *arena;
    uint32_t arena_bytes;
} ai_static_direct_context_t;

uint32_t ai_static_direct_required_arena_bytes(
    const ai_static_direct_descriptor_t *descriptor);

int ai_static_direct_invoke(
    const ai_static_direct_descriptor_t *descriptor,
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    ai_static_direct_context_t *context);

#ifdef __cplusplus
}
#endif

#endif
