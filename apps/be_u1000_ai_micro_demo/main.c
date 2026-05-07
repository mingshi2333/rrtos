#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "os_kernel.h"
#include "os_mem.h"

static os_tcb_t g_ai_task_tcb;

enum {
    AI_MICRO_TASK_STACK_SIZE = 16 * 1024,
    AI_TENSOR_SHAPE_CAPACITY = 4,
    AI_MICRO_DEFAULT_BUFFER_BYTES = 4096,
    HELLO_WORLD_INPUT_BITS = 0x3f800000u,      /* 1.0f */
    HELLO_WORLD_MIN_OUTPUT_BITS = 0x3f400000u, /* 0.75f */
    HELLO_WORLD_MAX_OUTPUT_BITS = 0x3f800000u, /* 1.0f */
};

#ifndef AI_MICRO_MODEL_NAME
#define AI_MICRO_MODEL_NAME "be_u1000_hello_world_float"
#endif

#ifndef AI_MICRO_INPUT_BUFFER_BYTES
#define AI_MICRO_INPUT_BUFFER_BYTES AI_MICRO_DEFAULT_BUFFER_BYTES
#endif

#ifndef AI_MICRO_OUTPUT_BUFFER_BYTES
#define AI_MICRO_OUTPUT_BUFFER_BYTES AI_MICRO_DEFAULT_BUFFER_BYTES
#endif

static uint8_t g_ai_task_stack[AI_MICRO_TASK_STACK_SIZE]
    __attribute__((section(".shared.ai_stack"), aligned(16)));
static uint8_t g_ai_input_buffer[AI_MICRO_INPUT_BUFFER_BYTES]
    __attribute__((aligned(16)));
static uint8_t g_ai_output_buffer[AI_MICRO_OUTPUT_BUFFER_BYTES]
    __attribute__((aligned(16)));

_Static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_t *)0)->shape) / sizeof(((ai_tensor_t *)0)->shape[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_t.shape capacity");

_Static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_spec_t *)0)->dims) / sizeof(((ai_tensor_spec_t *)0)->dims[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_spec_t.dims capacity");

static void ai_tensor_from_spec(ai_tensor_t *tensor,
                                const ai_tensor_spec_t *spec,
                                void *data,
                                size_t size)
{
    uint32_t ndim = spec->ndim < AI_TENSOR_SHAPE_CAPACITY
                        ? spec->ndim
                        : AI_TENSOR_SHAPE_CAPACITY;

    tensor->data = data;
    tensor->dtype = spec->dtype;
    tensor->ndim = ndim;
    tensor->size = size;
    for (uint32_t i = 0; i < ndim; ++i) {
        tensor->shape[i] = spec->dims[i];
    }
}

static void ai_micro_halt(void)
{
    while (1) {
        os_task_delay(1000);
    }
}

static uint32_t float_to_bits(const float *value)
{
    const uint8_t *bytes = (const uint8_t *)value;

    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static void float_from_bits(float *value, uint32_t bits)
{
    uint8_t *bytes = (uint8_t *)value;

    bytes[0] = (uint8_t)(bits & 0xffu);
    bytes[1] = (uint8_t)((bits >> 8) & 0xffu);
    bytes[2] = (uint8_t)((bits >> 16) & 0xffu);
    bytes[3] = (uint8_t)((bits >> 24) & 0xffu);
}

static size_t ai_dtype_size(ai_dtype_t dtype)
{
    switch (dtype) {
    case AI_DTYPE_INT8:
    case AI_DTYPE_UINT8:
        return 1u;
    case AI_DTYPE_INT16:
        return 2u;
    case AI_DTYPE_INT32:
    case AI_DTYPE_FP32:
        return 4u;
    default:
        return 0u;
    }
}

static size_t ai_tensor_size_from_spec(const ai_tensor_spec_t *spec)
{
    size_t element_count = 1u;
    size_t element_size = ai_dtype_size(spec->dtype);

    if (element_size == 0u || spec->ndim == 0u ||
        spec->ndim > AI_TENSOR_SHAPE_CAPACITY) {
        return 0u;
    }

    for (uint32_t i = 0; i < spec->ndim; ++i) {
        element_count *= spec->dims[i];
    }

    return element_count * element_size;
}

static uint32_t ai_micro_hash_bytes(const void *data, size_t size)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t hash = 2166136261u;

    for (size_t i = 0; i < size; ++i) {
        hash ^= bytes[i];
        hash *= 16777619u;
    }

    return hash;
}

static int ai_micro_is_hello_model(void)
{
    return strcmp(AI_MICRO_MODEL_NAME, "be_u1000_hello_world_float") == 0;
}

static void ai_micro_reset_memory_trace(void)
{
#if OS_CFG_HEAP_EN
    os_heap_stats_reset_peak();
    os_heap_trace_reset();
#endif
    ai_hal_allocation_trace_reset();
}

static void ai_micro_print_memory_trace(void)
{
    ai_hal_allocation_trace_t hal_trace = ai_hal_allocation_trace_get();

#if OS_CFG_HEAP_EN
    os_heap_stats_t heap_stats = os_heap_stats_get();
    os_heap_trace_t heap_trace = os_heap_trace_get();

    os_print("[AI_MICRO] heap current=%u peak=%u allocs=%u frees=%u\n",
             (uint32_t)heap_stats.current_used_bytes,
             (uint32_t)heap_stats.peak_used_bytes,
             (uint32_t)heap_stats.allocation_count,
             (uint32_t)heap_stats.free_count);
    os_print("[AI_MICRO] heap_peak_event size=%u current=%u caller=0x%x\n",
             (uint32_t)heap_trace.peak_event.size,
             (uint32_t)heap_trace.peak_event.current_used_bytes,
             (uint32_t)heap_trace.peak_event.caller);
    os_print("[AI_MICRO] heap_largest_live size=%u current=%u caller=0x%x\n",
             (uint32_t)heap_trace.largest_live_block.size,
             (uint32_t)heap_trace.largest_live_block.current_used_bytes,
             (uint32_t)heap_trace.largest_live_block.caller);
#endif

    os_print("[AI_MICRO] hal_alloc_peak size=%u ptr=0x%x seq=%u\n",
             (uint32_t)hal_trace.peak_event.allocation_size,
             (uint32_t)hal_trace.peak_event.buffer_ptr,
             hal_trace.peak_event.sequence);
    for (uint32_t i = 0; i < AI_HAL_ALLOCATION_TRACE_TOP_CAPACITY; ++i) {
        const ai_hal_allocation_trace_record_t *record =
            &hal_trace.largest_allocations[i];
        if (record->allocation_size == 0u) {
            continue;
        }
        os_print("[AI_MICRO] hal_alloc_top[%u] size=%u ptr=0x%x seq=%u\n",
                 i,
                 (uint32_t)record->allocation_size,
                 (uint32_t)record->buffer_ptr,
                 record->sequence);
    }
}

static uint32_t float_order_key(uint32_t bits)
{
    return (bits & 0x80000000u) ? ~bits : (bits | 0x80000000u);
}

static void ai_micro_task(void *arg)
{
    ai_model_handle_t handle;
    ai_tensor_spec_t input_spec;
    ai_tensor_spec_t output_spec;
    ai_tensor_t input_tensor;
    ai_tensor_t output_tensor;
    int rc;
    size_t input_size;
    size_t output_size;
    uint32_t y_bits;
    uint32_t y_key;
    uint32_t output_hash;

    (void)arg;

    os_print("[AI_MICRO] initializing runtime\n");
    rc = ai_runtime_init();
    if (rc != 0) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL init rc=%d\n", rc);
        ai_micro_halt();
    }

    /* Default call shape: ai_model_find_by_name("be_u1000_hello_world_float"). */
    handle = ai_model_find_by_name(AI_MICRO_MODEL_NAME);
    if (!handle) {
        os_print("[AI_MICRO] BE_U1000_AI_MICRO_FAIL model lookup name=%s\n",
                 AI_MICRO_MODEL_NAME);
        ai_micro_halt();
    }

    if (ai_model_get_input_info(handle, 0u, &input_spec) != 0 ||
        ai_model_get_output_info(handle, 0u, &output_spec) != 0) {
        os_print("[AI_MICRO] BE_U1000_AI_MICRO_FAIL tensor metadata\n");
        ai_micro_halt();
    }

    if (input_spec.ndim > AI_TENSOR_SHAPE_CAPACITY ||
        output_spec.ndim > AI_TENSOR_SHAPE_CAPACITY) {
        os_print("[AI_MICRO] BE_U1000_AI_MICRO_FAIL tensor rank input=%u output=%u\n",
                 input_spec.ndim,
                 output_spec.ndim);
        ai_micro_halt();
    }

    input_size = ai_tensor_size_from_spec(&input_spec);
    output_size = ai_tensor_size_from_spec(&output_spec);
    if (input_size == 0u || output_size == 0u ||
        input_size > sizeof(g_ai_input_buffer) ||
        output_size > sizeof(g_ai_output_buffer)) {
        os_print("[AI_MICRO] BE_U1000_AI_MICRO_FAIL tensor bytes input=%u/%u output=%u/%u\n",
                 (uint32_t)input_size,
                 (uint32_t)sizeof(g_ai_input_buffer),
                 (uint32_t)output_size,
                 (uint32_t)sizeof(g_ai_output_buffer));
        ai_micro_halt();
    }

    memset(g_ai_input_buffer, 0, input_size);
    memset(g_ai_output_buffer, 0, output_size);

    os_print("[AI_MICRO] running %s input_bytes=%u output_bytes=%u\n",
             AI_MICRO_MODEL_NAME,
             (uint32_t)input_size,
             (uint32_t)output_size);
    if (ai_micro_is_hello_model() && input_size >= sizeof(float)) {
        float_from_bits((float *)g_ai_input_buffer, HELLO_WORLD_INPUT_BITS);
    }
    ai_tensor_from_spec(&input_tensor, &input_spec, g_ai_input_buffer, input_size);
    ai_tensor_from_spec(&output_tensor, &output_spec, g_ai_output_buffer, output_size);

    ai_micro_reset_memory_trace();
    rc = ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 1000u);
    ai_micro_print_memory_trace();
    if (rc != 0) {
        os_print("[AI_MICRO] BE_U1000_AI_MICRO_FAIL infer rc=%d\n", rc);
        ai_micro_halt();
    }

    output_hash = ai_micro_hash_bytes(g_ai_output_buffer, output_size);
    os_print("[AI_MICRO] output_hash=0x%x\n", output_hash);

    if (ai_micro_is_hello_model()) {
        y_bits = float_to_bits((const float *)g_ai_output_buffer);
        y_key = float_order_key(y_bits);

        os_print("[AI_MICRO] x_hex=0x%x y_hex=0x%x expected_y_range=[0x%x,0x%x]\n",
                 HELLO_WORLD_INPUT_BITS,
                 y_bits,
                 HELLO_WORLD_MIN_OUTPUT_BITS,
                 HELLO_WORLD_MAX_OUTPUT_BITS);

        if (y_key >= float_order_key(HELLO_WORLD_MIN_OUTPUT_BITS) &&
            y_key <= float_order_key(HELLO_WORLD_MAX_OUTPUT_BITS)) {
            os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_PASS\n");
        } else {
            os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL y_hex=0x%x\n", y_bits);
        }
    }

    os_print("[AI_MICRO] BE_U1000_AI_MICRO_MODEL_PASS name=%s\n",
             AI_MICRO_MODEL_NAME);

    ai_micro_halt();
}

void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[AI_MICRO] boot mtime=0x%x\n", (uint32_t)hal_clint_mtime_get());
    os_print("[AI_MICRO] generic AI micro model probe\n");
    os_print("[AI_MICRO] initializing kernel\n");

    os_kernel_init();
    rc = os_task_create(&g_ai_task_tcb,
                        "ai_micro",
                        ai_micro_task,
                        0,
                        10,
                        g_ai_task_stack,
                        sizeof(g_ai_task_stack));
    if (rc != OS_EOK) {
        os_print("[AI_MICRO] task create failed rc=%d\n", (int)rc);
        while (1) {
        }
    }

    os_print("[AI_MICRO] starting scheduler\n");
    os_kernel_start();
}
