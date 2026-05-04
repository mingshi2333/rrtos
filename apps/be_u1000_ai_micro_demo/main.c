#include <stdint.h>
#include <stddef.h>

#include "ai_model_registry.h"
#include "ai_models.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "os_kernel.h"

static os_tcb_t g_ai_task_tcb;

enum {
    AI_MICRO_TASK_STACK_SIZE = 16 * 1024,
    AI_TENSOR_SHAPE_CAPACITY = 4,
    HELLO_WORLD_INPUT_BITS = 0x3f800000u,      /* 1.0f */
    HELLO_WORLD_MIN_OUTPUT_BITS = 0x3f400000u, /* 0.75f */
    HELLO_WORLD_MAX_OUTPUT_BITS = 0x3f800000u, /* 1.0f */
};

static uint8_t g_ai_task_stack[AI_MICRO_TASK_STACK_SIZE]
    __attribute__((section(".shared.ai_stack"), aligned(16)));

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

static uint32_t float_order_key(uint32_t bits)
{
    return (bits & 0x80000000u) ? ~bits : (bits | 0x80000000u);
}

static void ai_micro_task(void *arg)
{
    static ai_be_u1000_hello_world_float_input_t input;
    static ai_be_u1000_hello_world_float_output_t output;
    ai_model_handle_t handle;
    ai_tensor_spec_t input_spec;
    ai_tensor_spec_t output_spec;
    ai_tensor_t input_tensor;
    ai_tensor_t output_tensor;
    int rc;
    uint32_t y_bits;
    uint32_t y_key;

    (void)arg;

    os_print("[AI_MICRO] initializing runtime\n");
    rc = ai_runtime_init();
    if (rc != 0) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL init rc=%d\n", rc);
        ai_micro_halt();
    }

    handle = ai_model_find_by_name("be_u1000_hello_world_float");
    if (!handle) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL model lookup\n");
        ai_micro_halt();
    }

    if (ai_model_get_input_info(handle, 0u, &input_spec) != 0 ||
        ai_model_get_output_info(handle, 0u, &output_spec) != 0) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL tensor metadata\n");
        ai_micro_halt();
    }

    if (input_spec.ndim > AI_TENSOR_SHAPE_CAPACITY ||
        output_spec.ndim > AI_TENSOR_SHAPE_CAPACITY) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL tensor rank input=%u output=%u\n",
                 input_spec.ndim,
                 output_spec.ndim);
        ai_micro_halt();
    }

    os_print("[AI_MICRO] running be_u1000_hello_world_float\n");
    float_from_bits(&input.tensor_0[0], HELLO_WORLD_INPUT_BITS);
    ai_tensor_from_spec(&input_tensor, &input_spec, input.tensor_0, sizeof(input.tensor_0));
    ai_tensor_from_spec(&output_tensor, &output_spec, output.tensor_0, sizeof(output.tensor_0));

    rc = ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 1000u);
    if (rc != 0) {
        os_print("[AI_MICRO] BE_U1000_TFLITE_HELLO_DEMO_FAIL infer rc=%d\n", rc);
        ai_micro_halt();
    }

    y_bits = float_to_bits(&output.tensor_0[0]);
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

    ai_micro_halt();
}

void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[AI_MICRO] boot mtime=0x%x\n", (uint32_t)hal_clint_mtime_get());
    os_print("[AI_MICRO] official TFLite hello_world static-library demo\n");
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
