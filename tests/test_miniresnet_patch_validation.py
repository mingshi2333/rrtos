import unittest
from pathlib import Path

from scripts.run_miniresnet_patch_validation import (
    validate_miniresnet_patch_validation_output,
)


PROJECT_ROOT = Path(__file__).resolve().parents[1]


VALID_LOG = "\n".join(
    [
        "Booting MiniResNet patch validation...",
        "MINIRESNET_PATCH_PROBE_PASS exports=23 dispatches=33 continuation_command_index=5",
        "MINIRESNET_PATCH_STATIC_RAM: input_bytes=3200 arena_bytes=142336 arena_guard_bytes=128 output_bytes=40 io_scratch_bytes=145704 task_stack_bytes=65536",
        "MINIRESNET_PATCH_ARENA_GUARD: run=0 before=ok after=ok",
        "MINIRESNET_PATCH_RESULT: run=0 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123456",
        "MINIRESNET_PATCH_OUTPUT_Q: run=0 values_q=[0,391,0,99609,0,0,0,0,0,0]",
        "MINIRESNET_PATCH_ARENA_GUARD: run=1 before=ok after=ok",
        "MINIRESNET_PATCH_RESULT: run=1 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123457",
        "MINIRESNET_PATCH_OUTPUT_Q: run=1 values_q=[0,391,0,99609,0,0,0,0,0,0]",
        "MINIRESNET_PATCH_ARENA_GUARD: run=2 before=ok after=ok",
        "MINIRESNET_PATCH_RESULT: run=2 sample=synthetic_pattern argmax=3 hash=3045847227 top_score_q=99609 latency_ticks=123458",
        "MINIRESNET_PATCH_OUTPUT_Q: run=2 values_q=[0,391,0,99609,0,0,0,0,0,0]",
        "MINIRESNET_PATCH_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0",
        "MINIRESNET_PATCH_PASS count=3",
    ]
)


class MiniResNetPatchValidationTest(unittest.TestCase):
    def test_validator_accepts_reduced_arena_output(self):
        self.assertEqual(validate_miniresnet_patch_validation_output(VALID_LOG), [])

    def test_validator_rejects_old_arena_size(self):
        errors = validate_miniresnet_patch_validation_output(
            VALID_LOG.replace("arena_bytes=142336", "arena_bytes=351104")
        )

        self.assertIn(
            "MINIRESNET_PATCH_STATIC_RAM arena must be 142336, got 351104",
            errors,
        )

    def test_validator_rejects_wrong_static_ram_field(self):
        errors = validate_miniresnet_patch_validation_output(
            VALID_LOG.replace("io_scratch_bytes=145704", "io_scratch_bytes=145705")
        )

        self.assertIn(
            "MINIRESNET_PATCH_STATIC_RAM scratch must be 145704, got 145705",
            errors,
        )

    def test_validator_rejects_wrong_continuation_index(self):
        errors = validate_miniresnet_patch_validation_output(
            VALID_LOG.replace("continuation_command_index=5", "continuation_command_index=4")
        )

        self.assertIn(
            "MINIRESNET_PATCH_PROBE_PASS continuation must be 5, got 4",
            errors,
        )

    def test_validator_rejects_changed_output_vector(self):
        errors = validate_miniresnet_patch_validation_output(
            VALID_LOG.replace(
                "MINIRESNET_PATCH_OUTPUT_Q: run=1 values_q=[0,391,0,99609,0,0,0,0,0,0]",
                "MINIRESNET_PATCH_OUTPUT_Q: run=1 values_q=[0,392,0,99609,0,0,0,0,0,0]",
            )
        )

        self.assertTrue(
            any("unexpected MINIRESNET_PATCH_OUTPUT_Q run=1" in error for error in errors)
        )

    def test_validator_rejects_heap_peak(self):
        errors = validate_miniresnet_patch_validation_output(
            VALID_LOG.replace("heap_peak_bytes=0", "heap_peak_bytes=16")
        )

        self.assertIn("MINIRESNET_PATCH_MODEL_PEAK peak must be 0, got 16", errors)

    def test_cmake_declares_remap_source(self):
        cmake_text = (PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt").read_text()

        self.assertIn("miniresnet_patch_validation", cmake_text)
        self.assertIn("miniresnet_patch_arena_remap.c", cmake_text)
        self.assertIn("miniresnet_patch_first_layer.c", cmake_text)
        self.assertIn("rv_aios_ai_ukernel", cmake_text)

    def test_first_layer_patch_calls_project_ukernel(self):
        header = (
            PROJECT_ROOT
            / "apps"
            / "mnist_app"
            / "src"
            / "miniresnet_patch_first_layer.h"
        ).read_text()
        source = (
            PROJECT_ROOT
            / "apps"
            / "mnist_app"
            / "src"
            / "miniresnet_patch_first_layer.c"
        ).read_text()

        self.assertIn("rrtos_ai_ukernel_status_t miniresnet_patch_run_first_layer", header)
        self.assertIn('#include "rrtos_ai_ukernel.h"', source)
        self.assertIn("rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t", source)
        self.assertIn(
            "rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31",
            source,
        )
        self.assertIn("miniresnet_patch_conv1_multiplier_q31", source)
        self.assertIn("miniresnet_patch_conv1_shift", source)
        self.assertNotIn("miniresnet_patch_conv1_multiplier,", source)
        self.assertNotIn("requant_row", source)
        self.assertNotIn("ch1_ohwi_7x7_s2_pad3", source)
        self.assertNotIn("rrtos_ai_ukernel_conv2d_i8_tile", source)
        self.assertNotIn("MINIRESNET_PATCH_ROW_ACCUMULATOR_COUNT", source)
        self.assertNotIn("for (int32_t oh", source)
        self.assertIn("return RRTOS_AI_UKERNEL_OK;", source)
        self.assertIn("return rc;", source)

    def test_remap_source_handles_work_buffer_sub_slices_by_lifetime(self):
        remap_text = (
            PROJECT_ROOT
            / "apps"
            / "mnist_app"
            / "src"
            / "miniresnet_patch_arena_remap.c"
        ).read_text()

        self.assertIn("MINIRESNET_PATCH_REMAP_WORK_BUFFER_END_COMMAND = 17", remap_text)
        self.assertIn(
            "command_index < MINIRESNET_PATCH_REMAP_WORK_BUFFER_END_COMMAND",
            remap_text,
        )
        self.assertIn(
            "slice->offset < MINIRESNET_PATCH_REMAP_WORK_BUFFER_BYTES",
            remap_text,
        )
        self.assertIn(
            "slice->offset += MINIRESNET_PATCH_REMAP_WORK_BUFFER_NEW_OFFSET",
            remap_text,
        )


if __name__ == "__main__":
    unittest.main()
