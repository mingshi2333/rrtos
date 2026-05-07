import unittest
from pathlib import Path

from scripts.run_miniresnet_static_direct_validation import (
    validate_miniresnet_static_direct_validation_output,
)


PROJECT_ROOT = Path(__file__).resolve().parents[1]


VALID_LOG = "\n".join(
    [
        "Booting MiniResNet static-direct validation...",
        "MINIRESNET_STATIC_DIRECT_PROBE_PASS exports=23 dispatches=33",
        "MINIRESNET_STATIC_DIRECT_STATIC_RAM: input_bytes=3200 arena_bytes=351104 arena_guard_bytes=128 output_bytes=40 io_scratch_bytes=354472 task_stack_bytes=65536",
        "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=0 before=ok after=ok",
        "MINIRESNET_STATIC_DIRECT_RESULT: run=0 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123456",
        "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=0 values_q=[5,5,5,99609,5,5,5,5,5,5]",
        "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=1 before=ok after=ok",
        "MINIRESNET_STATIC_DIRECT_RESULT: run=1 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123457",
        "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=1 values_q=[5,5,5,99609,5,5,5,5,5,5]",
        "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=2 before=ok after=ok",
        "MINIRESNET_STATIC_DIRECT_RESULT: run=2 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123458",
        "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=2 values_q=[5,5,5,99609,5,5,5,5,5,5]",
        "MINIRESNET_STATIC_DIRECT_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
        "MINIRESNET_STATIC_DIRECT_PASS count=3",
    ]
)


class MiniResNetStaticDirectValidationTest(unittest.TestCase):
    def test_validator_accepts_probe_result_and_pass(self):
        self.assertEqual(validate_miniresnet_static_direct_validation_output(VALID_LOG), [])

    def test_validator_rejects_fail_token(self):
        errors = validate_miniresnet_static_direct_validation_output(
            VALID_LOG + "\nMINIRESNET_STATIC_DIRECT_FAIL: rc=-2\n"
        )

        self.assertIn("MINIRESNET_STATIC_DIRECT_FAIL token present", errors)

    def test_validator_rejects_missing_result(self):
        errors = validate_miniresnet_static_direct_validation_output(
            "\n".join(
                [
                    "MINIRESNET_STATIC_DIRECT_PROBE_PASS exports=23 dispatches=33",
                    "MINIRESNET_STATIC_DIRECT_STATIC_RAM: input_bytes=3200 arena_bytes=351104 arena_guard_bytes=128 output_bytes=40 io_scratch_bytes=354472 task_stack_bytes=65536",
                    "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=0 before=ok after=ok",
                    "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=1 before=ok after=ok",
                    "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=2 before=ok after=ok",
                    "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=0 values_q=[5,5,5,99609,5,5,5,5,5,5]",
                    "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=1 values_q=[5,5,5,99609,5,5,5,5,5,5]",
                    "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=2 values_q=[5,5,5,99609,5,5,5,5,5,5]",
                    "MINIRESNET_STATIC_DIRECT_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
                    "MINIRESNET_STATIC_DIRECT_PASS count=3",
                ]
            )
        )

        self.assertIn("missing MINIRESNET_STATIC_DIRECT_RESULT line", errors)

    def test_validator_rejects_guard_failure(self):
        errors = validate_miniresnet_static_direct_validation_output(
            VALID_LOG.replace(
                "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=1 before=ok after=ok",
                "MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=1 before=ok after=fail",
            )
        )

        self.assertIn("arena guard failed for run=1", errors)

    def test_validator_rejects_wrong_result_oracle(self):
        errors = validate_miniresnet_static_direct_validation_output(
            VALID_LOG.replace(
                "MINIRESNET_STATIC_DIRECT_RESULT: run=2 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123458",
                "MINIRESNET_STATIC_DIRECT_RESULT: run=2 sample=synthetic_pattern argmax=4 hash=3045847227 top_score_q=99609 latency_ticks=123458",
            )
        )

        self.assertTrue(
            any("unexpected MINIRESNET_STATIC_DIRECT_RESULT run=2" in error for error in errors)
        )

    def test_validator_rejects_missing_output_vector(self):
        errors = validate_miniresnet_static_direct_validation_output(
            "\n".join(
                line for line in VALID_LOG.splitlines()
                if not line.startswith("MINIRESNET_STATIC_DIRECT_OUTPUT_Q:")
            )
        )

        self.assertIn("missing MINIRESNET_STATIC_DIRECT_OUTPUT_Q line", errors)

    def test_validator_rejects_short_output_vector(self):
        errors = validate_miniresnet_static_direct_validation_output(
            VALID_LOG.replace(
                "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=1 values_q=[5,5,5,99609,5,5,5,5,5,5]",
                "MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=1 values_q=[5,5,5]",
            )
        )

        self.assertIn("output vector for run=1 must contain 10 values", errors)

    def test_cmake_declares_miniresnet_static_direct_target(self):
        cmake_text = (PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt").read_text()

        self.assertIn("miniresnet_static_direct_validation", cmake_text)
        self.assertIn("miniresnet_static_direct_validation_main.c", cmake_text)
        self.assertIn("miniresnet_static_direct_model_const.c", cmake_text)
        self.assertIn("miniresnet_static_direct_desc.c", cmake_text)
        self.assertIn("miniresnetv1_s1_64x50_tl_int8.o", cmake_text)

    def test_static_direct_runtime_is_declared(self):
        cmake_text = (PROJECT_ROOT / "ai" / "CMakeLists.txt").read_text()
        header = PROJECT_ROOT / "ai" / "include" / "ai_static_direct.h"
        source = PROJECT_ROOT / "ai" / "src" / "ai_static_direct.c"

        self.assertTrue(header.exists())
        self.assertTrue(source.exists())
        self.assertIn("add_library(rv_aios_ai_static_direct STATIC", cmake_text)
        self.assertIn("ai_static_direct.c", cmake_text)

    def test_pixi_exposes_miniresnet_static_direct_qemu_lane(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("build-miniresnet-static-direct-qemu-test", pixi_text)
        self.assertIn("validate-miniresnet-static-direct-qemu-test", pixi_text)
        self.assertIn("run_miniresnet_static_direct_validation.py", pixi_text)


if __name__ == "__main__":
    unittest.main()
