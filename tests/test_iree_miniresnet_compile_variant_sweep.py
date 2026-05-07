import tempfile
import unittest
from pathlib import Path

import yaml

from scripts import sweep_iree_miniresnet_compile_variants as sweep


class IreeMiniresnetCompileVariantSweepTest(unittest.TestCase):
    def test_generated_config_carries_variant_flags(self):
        variant = sweep.variant_by_name("aggressive_fusion")
        output_dir = sweep.PROJECT_ROOT / "build" / "tmp" / "generated"

        config = sweep.generated_config(variant, output_dir)

        self.assertEqual(config["defaults"]["target_arch"], "rv32")
        self.assertEqual(config["defaults"]["target_abi"], "ilp32f")
        self.assertFalse(config["defaults"]["enable_llvmcpu_microkernels"])
        self.assertEqual(
            config["defaults"]["enable_data_tiling"],
            variant.enable_data_tiling,
        )
        self.assertEqual(
            config["defaults"]["enable_stream_memory_flags"],
            variant.enable_stream_memory_flags,
        )
        self.assertEqual(
            config["defaults"]["extra_iree_compile_flags"],
            list(variant.extra_flags),
        )
        self.assertEqual(config["toolchain"]["tflite_toolchain_env"], "iree-toolchain310")
        self.assertEqual(config["models"][0]["name"], sweep.MODEL_NAME)
        self.assertEqual(config["models"][0]["file"], str(sweep.MODEL_PATH))

    def test_write_config_persists_variant_yaml(self):
        variant = sweep.variant_by_name("baseline")
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            config_path = sweep.write_config(variant, root / "generated", root)
            config = yaml.safe_load(config_path.read_text(encoding="utf-8"))

        self.assertEqual(config["project_name"], "rrtos_iree_miniresnet_baseline")
        self.assertEqual(config["output_dir"], str(root / "generated"))
        self.assertEqual(config["defaults"]["extra_iree_compile_flags"], [])

    def test_compact_error_prefers_compiler_errors_over_traceback_tail(self):
        output = "\n".join(
            [
                "failed to translate executables",
                "/tmp/model.mlir:29:11: error: 'memref.alloca' op all stack allocations need to be hoisted",
                "Traceback (most recent call last):",
                "subprocess.CalledProcessError: returned non-zero exit status 1",
            ]
        )

        error = sweep.compact_error(output)

        self.assertIn("failed to translate executables", error)
        self.assertIn("'memref.alloca' op", error)
        self.assertNotIn("CalledProcessError", error)

    def test_summarize_dump_finds_stream_and_hal_peak_allocations(self):
        mlir = "\n".join(
            [
                "module {",
                "  util.func public @main(%arg0: !hal.buffer_view) {",
                "    %c64 = arith.constant 64 : index",
                "    %c3920 = arith.constant 3920 : index",
                "    %c204800 = arith.constant 204800 : index",
                "    %c58752 = arith.constant 58752 : index",
                "    %c13312 = arith.constant 13312 : index",
                "    %c14336 = arith.constant 14336 : index",
                "    %c5760 = arith.constant 5760 : index",
                "    %0 = hal.buffer_view.dim<%arg0 : !hal.buffer_view>[0] : index",
                "    %3 = arith.muli %0, %c3920 : index",
                "    %4 = arith.muli %0, %c204800 : index",
                "    %5 = arith.muli %0, %c58752 : index",
                "    %6 = arith.muli %0, %c13312 : index",
                "    %7 = arith.muli %0, %c14336 : index",
                "    %9 = arith.muli %0, %c5760 : index",
                "    %13 = util.align %3, %c64 : index",
                "    %14 = arith.addi %13, %4 : index",
                "    %15 = arith.addi %14, %5 : index",
                "    %16 = arith.addi %15, %6 : index",
                "    %17 = arith.addi %16, %7 : index",
                "    %18 = arith.addi %17, %7 : index",
                "    %19 = arith.addi %18, %9 : index",
                "    %result, %timepoint = stream.resource.alloca uninitialized => !stream.resource<transient>{%19} => !stream.timepoint",
                "    util.return",
                "  }",
                "}",
            ]
        )
        hal_mlir = "\n".join(
            [
                "module {",
                "  util.func public @main(%arg0: !hal.buffer_view) {",
                "    %c315264 = arith.constant 315264 : index",
                "    %transient, %timepoint = hal.device.queue.alloca affinity(%device_0) wait(%c0) => !hal.buffer{%c315264}",
                "    util.return",
                "  }",
                "}",
            ]
        )

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "model.7.stream.mlir").write_text(mlir, encoding="utf-8")
            (root / "model.11.hal.mlir").write_text(hal_mlir, encoding="utf-8")

            summary = sweep.summarize_dump(root)

        self.assertEqual(summary["max_stream_allocation"], 315264)
        self.assertEqual(summary["max_hal_allocation"], 315264)
        self.assertTrue(summary["has_315264"])

    def test_stream_debug_runtime_lane_is_declared(self):
        config = yaml.safe_load(
            (sweep.PROJECT_ROOT / "ai_models_stm32ai_miniresnet_stream_debug.yaml")
            .read_text(encoding="utf-8")
        )
        cmake_text = (
            sweep.PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt"
        ).read_text(encoding="utf-8")

        self.assertEqual(
            config["output_dir"],
            "apps/mnist_app/generated_miniresnet_stream_debug",
        )
        self.assertEqual(
            config["model_library_name"],
            "rv_aios_miniresnet_stream_debug_models",
        )
        self.assertEqual(
            config["defaults"]["extra_iree_compile_flags"],
            ["--iree-stream-partitioning-favor=debug"],
        )
        self.assertIn("generated_miniresnet_stream_debug", cmake_text)
        self.assertIn("miniresnet_stream_debug_validation", cmake_text)
        self.assertIn("rv_aios_miniresnet_stream_debug_models", cmake_text)


if __name__ == "__main__":
    unittest.main()
