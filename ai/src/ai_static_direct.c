#include "ai_static_direct.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

enum {
    AI_STATIC_DIRECT_MAX_BINDINGS = 8,
};

static int checked_region(
    ai_static_direct_context_t *context,
    ai_static_direct_buffer_base_t base,
    uint8_t **out_base,
    uint32_t *out_bytes) {
    if (context == NULL || out_base == NULL || out_bytes == NULL) {
        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;
    }

    switch (base) {
    case AI_STATIC_DIRECT_BUFFER_INPUT:
        *out_base = (uint8_t *)(uintptr_t)context->input;
        *out_bytes = context->input_bytes;
        return AI_STATIC_DIRECT_OK;
    case AI_STATIC_DIRECT_BUFFER_OUTPUT:
        *out_base = context->output;
        *out_bytes = context->output_bytes;
        return AI_STATIC_DIRECT_OK;
    case AI_STATIC_DIRECT_BUFFER_CONST:
        *out_base = (uint8_t *)(uintptr_t)context->const_data;
        *out_bytes = context->const_bytes;
        return AI_STATIC_DIRECT_OK;
    case AI_STATIC_DIRECT_BUFFER_ARENA:
        *out_base = context->arena;
        *out_bytes = context->arena_bytes;
        return AI_STATIC_DIRECT_OK;
    default:
        return AI_STATIC_DIRECT_ERR_INVALID_SLICE;
    }
}

static int checked_slice_ptr(
    ai_static_direct_context_t *context,
    const ai_static_direct_slice_t *slice,
    void **out_ptr,
    size_t *out_length) {
    uint8_t *base = NULL;
    uint32_t region_bytes = 0;

    if (slice == NULL || out_ptr == NULL || out_length == NULL) {
        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;
    }

    int rc = checked_region(context, slice->base, &base, &region_bytes);
    if (rc != AI_STATIC_DIRECT_OK) {
        return rc;
    }
    if (base == NULL || slice->offset > region_bytes ||
        slice->length > region_bytes - slice->offset) {
        return AI_STATIC_DIRECT_ERR_INVALID_SLICE;
    }

    *out_ptr = base + slice->offset;
    *out_length = slice->length;
    return AI_STATIC_DIRECT_OK;
}

static int validate_dispatch_attrs(
    const iree_hal_executable_library_v0_t *library,
    const ai_static_direct_dispatch_t *dispatch) {
    if (dispatch->ordinal >= library->exports.count ||
        library->exports.ptrs == NULL ||
        library->exports.ptrs[dispatch->ordinal] == NULL) {
        return AI_STATIC_DIRECT_ERR_UNSUPPORTED_DISPATCH;
    }

    if (library->exports.attrs != NULL) {
        const iree_hal_executable_dispatch_attrs_v0_t *attrs =
            &library->exports.attrs[dispatch->ordinal];
        if (attrs->constant_count != dispatch->constant_count ||
            attrs->binding_count != dispatch->binding_count ||
            attrs->local_memory_pages != 0u) {
            return AI_STATIC_DIRECT_ERR_UNSUPPORTED_DISPATCH;
        }
    }

    return AI_STATIC_DIRECT_OK;
}

static int issue_dispatch(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    const ai_static_direct_dispatch_t *dispatch,
    ai_static_direct_context_t *context) {
    void *binding_ptrs[AI_STATIC_DIRECT_MAX_BINDINGS];
    size_t binding_lengths[AI_STATIC_DIRECT_MAX_BINDINGS];

    if (dispatch->binding_count > AI_STATIC_DIRECT_MAX_BINDINGS) {
        return AI_STATIC_DIRECT_ERR_UNSUPPORTED_DISPATCH;
    }

    int rc = validate_dispatch_attrs(library, dispatch);
    if (rc != AI_STATIC_DIRECT_OK) {
        return rc;
    }

    for (uint32_t i = 0; i < dispatch->binding_count; ++i) {
        rc = checked_slice_ptr(
            context,
            &dispatch->bindings[i],
            &binding_ptrs[i],
            &binding_lengths[i]);
        if (rc != AI_STATIC_DIRECT_OK) {
            return rc;
        }
    }

    iree_hal_executable_dispatch_state_v0_t dispatch_state = {
        .workgroup_size_x = 1,
        .workgroup_size_y = 1,
        .workgroup_size_z = 1,
        .constant_count = dispatch->constant_count,
        .workgroup_count_x = dispatch->workgroups[0],
        .workgroup_count_y = dispatch->workgroups[1],
        .workgroup_count_z = (uint16_t)dispatch->workgroups[2],
        .max_concurrency = 1,
        .binding_count = dispatch->binding_count,
        .constants = dispatch->constants,
        .binding_ptrs = binding_ptrs,
        .binding_lengths = binding_lengths,
    };
    iree_hal_executable_workgroup_state_v0_t workgroup_state = {0};

    for (uint32_t z = 0; z < dispatch->workgroups[2]; ++z) {
        workgroup_state.workgroup_id_z = (uint16_t)z;
        for (uint32_t y = 0; y < dispatch->workgroups[1]; ++y) {
            workgroup_state.workgroup_id_y = y;
            for (uint32_t x = 0; x < dispatch->workgroups[0]; ++x) {
                workgroup_state.workgroup_id_x = x;
                rc = library->exports.ptrs[dispatch->ordinal](
                    environment,
                    &dispatch_state,
                    &workgroup_state);
                if (rc != 0) {
                    return AI_STATIC_DIRECT_ERR_DISPATCH_FAILED;
                }
            }
        }
    }

    return AI_STATIC_DIRECT_OK;
}

static int issue_fill(
    ai_static_direct_context_t *context,
    const ai_static_direct_fill_t *fill) {
    void *ptr = NULL;
    size_t length = 0;

    int rc = checked_slice_ptr(context, &fill->target, &ptr, &length);
    if (rc != AI_STATIC_DIRECT_OK) {
        return rc;
    }

    memset(ptr, (int)(fill->pattern & 0xFFu), length);
    return AI_STATIC_DIRECT_OK;
}

static int issue_command(
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    ai_static_direct_context_t *context,
    const ai_static_direct_command_t *command) {
    if (command == NULL) {
        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;
    }

    switch (command->type) {
    case AI_STATIC_DIRECT_COMMAND_DISPATCH:
        if (command->config.dispatch.dispatch == NULL) {
            return AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR;
        }
        return issue_dispatch(
            library,
            environment,
            command->config.dispatch.dispatch,
            context);
    case AI_STATIC_DIRECT_COMMAND_FILL:
        return issue_fill(context, &command->config.fill);
    default:
        return AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR;
    }
}

uint32_t ai_static_direct_required_arena_bytes(
    const ai_static_direct_descriptor_t *descriptor) {
    return descriptor != NULL ? descriptor->arena_bytes : 0u;
}

int ai_static_direct_invoke(
    const ai_static_direct_descriptor_t *descriptor,
    const iree_hal_executable_library_v0_t *library,
    const iree_hal_executable_environment_v0_t *environment,
    ai_static_direct_context_t *context) {
    if (descriptor == NULL || library == NULL || environment == NULL ||
        context == NULL || descriptor->dispatches == NULL ||
        descriptor->dispatch_count == 0u ||
        descriptor->unsupported_feature_flags != 0u ||
        context->arena_bytes < descriptor->arena_bytes ||
        context->input_bytes < descriptor->input_bytes ||
        context->const_bytes < descriptor->const_bytes ||
        context->output_bytes < descriptor->output_bytes) {
        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;
    }

    if (descriptor->commands != NULL && descriptor->command_count > 0u) {
        for (uint32_t i = 0; i < descriptor->command_count; ++i) {
            int rc = issue_command(
                library,
                environment,
                context,
                &descriptor->commands[i]);
            if (rc != AI_STATIC_DIRECT_OK) {
                return rc;
            }
        }
        return AI_STATIC_DIRECT_OK;
    }

    for (uint32_t i = 0; i < descriptor->dispatch_count; ++i) {
        int rc = issue_dispatch(
            library,
            environment,
            &descriptor->dispatches[i],
            context);
        if (rc != AI_STATIC_DIRECT_OK) {
            return rc;
        }
    }

    return AI_STATIC_DIRECT_OK;
}
