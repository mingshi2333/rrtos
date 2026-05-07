import unittest
from pathlib import Path

from scripts.run_mnist_static_direct_validation import (
    validate_mnist_static_direct_validation_output,
)
from scripts.run_mnist_descriptor_validation import (
    validate_mnist_descriptor_validation_output,
)


PROJECT_ROOT = Path(__file__).resolve().parents[1]


VALID_LOG = "\n".join(
    [
        "Booting MNIST static-direct validation...",
        "MNIST_STATIC_DIRECT_PROBE_PASS exports=3",
        "MNIST_STATIC_DIRECT_STATIC_RAM: input_bytes=3136 scratch_bytes=576 output_bytes=40 io_scratch_bytes=3752 task_stack_bytes=65536",
        "MNIST_STATIC_DIRECT_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
        "MNIST_STATIC_DIRECT_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_hex=0x3f7f70a4 latency_ticks=1234",
        "MNIST_STATIC_DIRECT_PASS count=1",
    ]
)

VALID_DESCRIPTOR_LOG = "\n".join(
    [
        "Booting MNIST descriptor validation...",
        "MNIST_DESCRIPTOR_PROBE_PASS ops=3 tensors=7",
        "MNIST_DESCRIPTOR_STATIC_RAM: input_bytes=3136 arena_bytes=552 output_bytes=40 io_scratch_bytes=3728 task_stack_bytes=65536",
        "MNIST_DESCRIPTOR_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
        "MNIST_DESCRIPTOR_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_hex=0x3f7f70a4 latency_ticks=1234",
        "MNIST_DESCRIPTOR_PASS count=1",
    ]
)


class MnistStaticDirectValidationTest(unittest.TestCase):
    def test_validator_accepts_probe_result_and_pass(self):
        self.assertEqual(validate_mnist_static_direct_validation_output(VALID_LOG), [])

    def test_validator_rejects_fail_token(self):
        errors = validate_mnist_static_direct_validation_output(
            VALID_LOG + "\nMNIST_STATIC_DIRECT_FAIL: rc=-2\n"
        )

        self.assertIn("MNIST_STATIC_DIRECT_FAIL token present", errors)

    def test_validator_rejects_missing_result(self):
        errors = validate_mnist_static_direct_validation_output(
            "\n".join(
                [
                    "MNIST_STATIC_DIRECT_PROBE_PASS exports=3",
                    "MNIST_STATIC_DIRECT_STATIC_RAM: input_bytes=3136 scratch_bytes=576 output_bytes=40 io_scratch_bytes=3752 task_stack_bytes=65536",
                    "MNIST_STATIC_DIRECT_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
                    "MNIST_STATIC_DIRECT_PASS count=1",
                ]
            )
        )

        self.assertIn("missing MNIST_STATIC_DIRECT_RESULT line", errors)

    def test_validator_rejects_missing_heap_peak(self):
        errors = validate_mnist_static_direct_validation_output(
            "\n".join(
                [
                    "MNIST_STATIC_DIRECT_PROBE_PASS exports=3",
                    "MNIST_STATIC_DIRECT_STATIC_RAM: input_bytes=3136 scratch_bytes=576 output_bytes=40 io_scratch_bytes=3752 task_stack_bytes=65536",
                    "MNIST_STATIC_DIRECT_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_hex=0x3f7f70a4 latency_ticks=1234",
                    "MNIST_STATIC_DIRECT_PASS count=1",
                ]
            )
        )

        self.assertIn("missing MNIST_STATIC_DIRECT_MODEL_PEAK phase=invoke", errors)

    def test_cmake_declares_qemu_static_direct_target_without_iree_runtime(self):
        cmake_text = (PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt").read_text()

        self.assertIn("mnist_static_direct_validation", cmake_text)
        self.assertIn("static_direct_validation_main.c", cmake_text)
        self.assertIn("static_direct_model_const.c", cmake_text)
        self.assertIn("st_mnist_28.o", cmake_text)

    def test_descriptor_validator_accepts_descriptor_result_and_pass(self):
        self.assertEqual(validate_mnist_descriptor_validation_output(VALID_DESCRIPTOR_LOG), [])

    def test_descriptor_validator_rejects_fail_token(self):
        errors = validate_mnist_descriptor_validation_output(
            VALID_DESCRIPTOR_LOG + "\nMNIST_DESCRIPTOR_FAIL: rc=-2\n"
        )

        self.assertIn("MNIST_DESCRIPTOR_FAIL token present", errors)

    def test_descriptor_validator_rejects_missing_result(self):
        errors = validate_mnist_descriptor_validation_output(
            "\n".join(
                [
                    "MNIST_DESCRIPTOR_PROBE_PASS ops=3 tensors=7",
                    "MNIST_DESCRIPTOR_STATIC_RAM: input_bytes=3136 arena_bytes=552 output_bytes=40 io_scratch_bytes=3728 task_stack_bytes=65536",
                    "MNIST_DESCRIPTOR_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
                    "MNIST_DESCRIPTOR_PASS count=1",
                ]
            )
        )

        self.assertIn("missing MNIST_DESCRIPTOR_RESULT line", errors)

    def test_descriptor_runtime_is_declared_without_iree_dependencies(self):
        cmake_text = (PROJECT_ROOT / "ai" / "CMakeLists.txt").read_text()
        header = PROJECT_ROOT / "ai" / "include" / "ai_descriptor_runtime.h"
        source = PROJECT_ROOT / "ai" / "src" / "ai_descriptor_runtime.c"

        self.assertTrue(header.exists())
        self.assertTrue(source.exists())
        self.assertIn("add_library(rv_aios_ai_descriptor STATIC", cmake_text)
        self.assertIn("ai_descriptor_runtime.c", cmake_text)

        header_text = header.read_text()
        source_text = source.read_text()
        self.assertIn("ai_descriptor_region_t", header_text)
        self.assertIn("ai_descriptor_op_t", header_text)
        self.assertIn("ai_descriptor_model_t", header_text)
        self.assertIn("ai_descriptor_invoke", header_text)
        self.assertNotIn("iree/", header_text)
        self.assertNotIn("iree/", source_text)

    def test_cmake_declares_descriptor_target_without_static_dispatch_object(self):
        cmake_text = (PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt").read_text()

        self.assertIn("mnist_descriptor_validation", cmake_text)
        self.assertIn("descriptor_validation_main.c", cmake_text)
        self.assertIn("descriptor_mnist_model.c", cmake_text)
        self.assertIn("rv_aios_ai_descriptor", cmake_text)

        descriptor_block = cmake_text[
            cmake_text.index("add_executable(mnist_descriptor_validation"):
            cmake_text.index("add_executable(mnist_quant_validation")
        ]
        self.assertNotIn("MNIST_STATIC_DIRECT_MODEL_OBJECT", descriptor_block)
        self.assertNotIn("third_party/iree", descriptor_block)
        self.assertNotIn("IREE_PLATFORM_GENERIC", descriptor_block)


if __name__ == "__main__":
    unittest.main()
