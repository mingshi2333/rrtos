#include <stddef.h>
#include <stdint.h>

#include "ai_models.h"

extern "C" {
#include "ai_model_registry_c_api.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "hal_uart.h"
#include "os_kernel.h"
}

namespace {

enum : uint32_t {
    AI_TENSOR_SHAPE_CAPACITY = 4u,
    HELLO_WORLD_INPUT_BITS = 0x3f800000u,      // 1.0f
    HELLO_WORLD_MIN_OUTPUT_BITS = 0x3f400000u, // 0.75f
    HELLO_WORLD_MAX_OUTPUT_BITS = 0x3f800000u, // 1.0f
};

enum : size_t {
    AI_MICRO_TASK_STACK_SIZE = 16u * 1024u,
};

static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_t *)0)->shape) / sizeof(((ai_tensor_t *)0)->shape[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_t.shape capacity");

static_assert(
    AI_TENSOR_SHAPE_CAPACITY ==
        (sizeof(((ai_tensor_spec_t *)0)->dims) / sizeof(((ai_tensor_spec_t *)0)->dims[0])),
    "AI_TENSOR_SHAPE_CAPACITY must match ai_tensor_spec_t.dims capacity");

os_tcb_t g_ai_task_tcb;
uint8_t g_ai_task_stack[AI_MICRO_TASK_STACK_SIZE]
    __attribute__((section(".shared.ai_stack"), aligned(16)));

void halt_task()
{
    while (1) {
        os_task_delay(1000);
    }
}

uint32_t float_to_bits(const float &value)
{
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&value);

    return static_cast<uint32_t>(bytes[0]) |
           (static_cast<uint32_t>(bytes[1]) << 8) |
           (static_cast<uint32_t>(bytes[2]) << 16) |
           (static_cast<uint32_t>(bytes[3]) << 24);
}

void float_from_bits(float &value, uint32_t bits)
{
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&value);

    bytes[0] = static_cast<uint8_t>(bits & 0xffu);
    bytes[1] = static_cast<uint8_t>((bits >> 8) & 0xffu);
    bytes[2] = static_cast<uint8_t>((bits >> 16) & 0xffu);
    bytes[3] = static_cast<uint8_t>((bits >> 24) & 0xffu);
}

uint32_t float_order_key(uint32_t bits)
{
    return (bits & 0x80000000u) ? ~bits : (bits | 0x80000000u);
}

struct TensorFactory {
    static ai_tensor_t from_spec(const ai_tensor_spec_t &spec, void *data, size_t size)
    {
        ai_tensor_t tensor = {};
        const uint32_t ndim = spec.ndim < AI_TENSOR_SHAPE_CAPACITY
                                  ? spec.ndim
                                  : AI_TENSOR_SHAPE_CAPACITY;

        tensor.data = data;
        tensor.dtype = spec.dtype;
        tensor.ndim = ndim;
        tensor.size = size;
        for (uint32_t i = 0; i < ndim; ++i) {
            tensor.shape[i] = spec.dims[i];
        }
        return tensor;
    }
};

class HelloWorldRunner {
public:
    bool init()
    {
        os_print("[AI_MICRO_CPP] initializing runtime\n");
        int rc = ai_runtime_init();
        if (rc != 0) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL init rc=%d\n", rc);
            return false;
        }

        handle_ = ai_model_find_by_name("be_u1000_hello_world_float");
        if (!handle_) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL model lookup\n");
            return false;
        }

        if (ai_model_get_input_info(handle_, 0u, &input_spec_) != 0 ||
            ai_model_get_output_info(handle_, 0u, &output_spec_) != 0) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL tensor metadata\n");
            return false;
        }

        if (input_spec_.ndim > AI_TENSOR_SHAPE_CAPACITY ||
            output_spec_.ndim > AI_TENSOR_SHAPE_CAPACITY) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL tensor rank input=%u output=%u\n",
                     input_spec_.ndim,
                     output_spec_.ndim);
            return false;
        }

        return true;
    }

    bool run_once()
    {
        ai_be_u1000_hello_world_float_input_t input = {};
        ai_be_u1000_hello_world_float_output_t output = {};

        os_print("[AI_MICRO_CPP] running be_u1000_hello_world_float\n");
        float_from_bits(input.tensor_0[0], HELLO_WORLD_INPUT_BITS);

        ai_tensor_t input_tensor =
            TensorFactory::from_spec(input_spec_, input.tensor_0, sizeof(input.tensor_0));
        ai_tensor_t output_tensor =
            TensorFactory::from_spec(output_spec_, output.tensor_0, sizeof(output.tensor_0));

        const int rc = ai_infer_sync(handle_, &input_tensor, 1u, &output_tensor, 1u, 1000u);
        if (rc != 0) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL infer rc=%d\n", rc);
            return false;
        }

        const uint32_t y_bits = float_to_bits(output.tensor_0[0]);
        const uint32_t y_key = float_order_key(y_bits);

        os_print("[AI_MICRO_CPP] x_hex=0x%x y_hex=0x%x expected_y_range=[0x%x,0x%x]\n",
                 HELLO_WORLD_INPUT_BITS,
                 y_bits,
                 HELLO_WORLD_MIN_OUTPUT_BITS,
                 HELLO_WORLD_MAX_OUTPUT_BITS);

        if (y_key >= float_order_key(HELLO_WORLD_MIN_OUTPUT_BITS) &&
            y_key <= float_order_key(HELLO_WORLD_MAX_OUTPUT_BITS)) {
            os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_PASS\n");
            return true;
        }

        os_print("[AI_MICRO_CPP] BE_U1000_TFLITE_HELLO_CPP_DEMO_FAIL y_hex=0x%x\n", y_bits);
        return false;
    }

private:
    ai_model_handle_t handle_ = 0;
    ai_tensor_spec_t input_spec_ = {};
    ai_tensor_spec_t output_spec_ = {};
};

void ai_micro_cpp_task(void *arg)
{
    (void)arg;

    HelloWorldRunner runner;
    if (runner.init()) {
        (void)runner.run_once();
    }
    halt_task();
}

} // namespace

extern "C" void os_kernel_main(void)
{
    hal_board_init();
    hal_board_print_banner();

    os_print("[AI_MICRO_CPP] boot mtime=0x%x\n", static_cast<uint32_t>(hal_clint_mtime_get()));
    os_print("[AI_MICRO_CPP] official TFLite hello_world static-library demo via C++ app layer\n");
    os_print("[AI_MICRO_CPP] initializing kernel\n");

    os_kernel_init();
    const os_err_t rc = os_task_create(&g_ai_task_tcb,
                                       "ai_cpp",
                                       ai_micro_cpp_task,
                                       0,
                                       10,
                                       g_ai_task_stack,
                                       sizeof(g_ai_task_stack));
    if (rc != OS_EOK) {
        os_print("[AI_MICRO_CPP] task create failed rc=%d\n", static_cast<int>(rc));
        while (1) {
        }
    }

    os_print("[AI_MICRO_CPP] starting scheduler\n");
    os_kernel_start();
}
