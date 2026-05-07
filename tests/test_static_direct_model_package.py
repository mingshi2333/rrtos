import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from scripts import generate_static_direct_model as package_generator


PACKAGE_SCHEDULE = {
    "schema": "rrtos.iree_hal_schedule.v1",
    "source": "toy.11.hal.mlir",
    "fixed_shape": True,
    "is_linear": True,
    "allocations": [
        {"id": "input", "size": 16, "lifetime": "input"},
        {"id": "weights", "size": 8, "lifetime": "constant"},
        {"id": "patch", "size": 256, "lifetime": "transient"},
        {"id": "scratch", "size": 64, "lifetime": "transient"},
    ],
    "dispatches": [
        {
            "ordinal": 0,
            "workgroups": [1, 1, 1],
            "constants": [],
            "bindings": [
                {"ordinal": 0, "base": "input", "offset": 0, "length": 16, "access": "ro"},
                {"ordinal": 1, "base": "patch", "offset": 0, "length": 256, "access": "rw"},
            ],
        },
        {
            "ordinal": 1,
            "workgroups": [1, 1, 1],
            "constants": [128],
            "bindings": [
                {"ordinal": 0, "base": "patch", "offset": 0, "length": 256, "access": "rw"},
                {"ordinal": 1, "base": "scratch", "offset": 0, "length": 64, "access": "rw"},
                {"ordinal": 2, "base": "weights", "offset": 0, "length": 8, "access": "ro"},
            ],
        },
    ],
    "commands": [
        {"type": "dispatch", "dispatch_index": 0},
        {"type": "dispatch", "dispatch_index": 1},
    ],
    "peak_transient_bytes": 320,
}


class StaticDirectModelPackageTest(unittest.TestCase):
    def test_build_package_generates_descriptor_verify_and_manifest(self):
        with tempfile.TemporaryDirectory() as tmp:
            output_dir = Path(tmp) / "package"
            manifest = package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
            )

            package_json = json.loads(
                (output_dir / "static_direct.package.json").read_text(encoding="utf-8")
            )
            verify_json = json.loads(
                (output_dir / "static_direct.verify.json").read_text(encoding="utf-8")
            )
            descriptor_c = (output_dir / "toy_model_static_direct_desc.c").read_text(
                encoding="ascii"
            )
            descriptor_h = (output_dir / "toy_model_static_direct_desc.h").read_text(
                encoding="ascii"
            )

        self.assertEqual(manifest["schema"], "rrtos.static_direct_package.v1")
        self.assertEqual(package_json["model_name"], "toy_model")
        self.assertEqual(package_json["entry_name"], "predict")
        self.assertEqual(package_json["metrics"]["input_bytes"], 16)
        self.assertEqual(package_json["metrics"]["input_kib"], 0.016)
        self.assertEqual(package_json["metrics"]["const_bytes"], 8)
        self.assertEqual(package_json["metrics"]["const_kib"], 0.008)
        self.assertEqual(package_json["metrics"]["output_bytes"], 0)
        self.assertEqual(package_json["metrics"]["output_kib"], 0.0)
        self.assertEqual(package_json["metrics"]["arena_kib"], 0.312)
        self.assertEqual(package_json["files"]["descriptor_c"], "toy_model_static_direct_desc.c")
        self.assertEqual(package_json["files"]["verify_json"], "static_direct.verify.json")
        self.assertEqual(package_json["files"]["memory_plan_json"], "static_direct.verify.json")
        self.assertIn("descriptor_c", package_json["artifact_sha256"])
        self.assertEqual(len(package_json["artifact_sha256"]["descriptor_c"]), 64)
        self.assertIn("verify_json", package_json["artifact_sha256"])
        self.assertEqual(verify_json["arena_bytes"], 320)
        self.assertEqual(verify_json["placements"]["patch"]["offset"], 0)
        self.assertEqual(verify_json["placements"]["scratch"]["offset"], 256)
        self.assertIn(".offset = 256u,", descriptor_c)
        self.assertIn("const ai_static_direct_descriptor_t toy_model_descriptor", descriptor_c)
        self.assertIn("#define TOY_MODEL_ARENA_BYTES 320u", descriptor_h)

    def test_build_package_records_deployment_gates(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            output_dir = tmp_path / "package"
            map_path = tmp_path / "toy.map"
            map_path.write_text(".text.dispatch\n", encoding="utf-8")

            manifest = package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
                deployment=package_generator.DeploymentConfig(
                    map_file=map_path,
                    oracle_runner=Path("scripts/run_miniresnet_patch_validation.py"),
                    oracle_kernel=Path("build-qemu-test/apps/mnist_app/miniresnet_patch_validation"),
                    oracle_log=Path("logs/miniresnet_patch_validation_qemu_test.log"),
                    oracle_memory="64M",
                ),
            )

        gates = manifest["deployment"]["gates"]
        self.assertTrue(manifest["deployment"]["deploy_ready"])
        self.assertEqual(gates["no_vm_map"]["status"], "configured")
        self.assertEqual(gates["no_vm_map"]["map"], str(map_path))
        self.assertIn("scripts/check_no_iree_vm_symbols.py", gates["no_vm_map"]["command"])
        self.assertEqual(gates["output_oracle"]["status"], "configured")
        self.assertIn("--memory", gates["output_oracle"]["command"])
        self.assertIn("--run-oracle", manifest["deployment"]["validate_command"])

    def test_build_package_can_generate_optional_remap_artifacts(self):
        with tempfile.TemporaryDirectory() as tmp:
            output_dir = Path(tmp) / "package"
            manifest = package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
                remap=package_generator.RemapConfig(
                    name="first_patch",
                    patch_allocation="patch",
                    patch_internal_offset=128,
                    produced_bytes=32,
                    continuation_command_index=1,
                    remap_end_command=2,
                    scratch_allocation="scratch",
                    scratch_end_command=2,
                    macro_prefix="TOY_REMAP",
                    header_guard="TOY_REMAP_H",
                    function_name="toy_invoke_remapped",
                ),
            )

            remap_plan = json.loads(
                (output_dir / "toy_model_first_patch_remap_plan.json").read_text(
                    encoding="utf-8"
                )
            )
            remap_source = (output_dir / "toy_model_first_patch_remap.c").read_text(
                encoding="ascii"
            )
            remap_header = (output_dir / "toy_model_first_patch_remap.h").read_text(
                encoding="ascii"
            )

        self.assertEqual(remap_plan["arena_bytes"], 192)
        self.assertEqual(remap_plan["savings_bytes"], 128)
        self.assertEqual(
            manifest["remaps"][0]["files"]["plan_json"],
            "toy_model_first_patch_remap_plan.json",
        )
        self.assertIn("plan_json", manifest["remaps"][0]["artifact_sha256"])
        self.assertEqual(len(manifest["remaps"][0]["artifact_sha256"]["source_c"]), 64)
        self.assertIn("TOY_REMAP_OLD_BIG_INTERNAL_BASE = 128", remap_source)
        self.assertIn("#define TOY_REMAP_ARENA_BYTES 192u", remap_header)

    def test_cli_generates_package(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            schedule_path = tmp_path / "schedule.json"
            output_dir = tmp_path / "package"
            schedule_path.write_text(
                json.dumps(PACKAGE_SCHEDULE, indent=2) + "\n",
                encoding="utf-8",
            )

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/generate_static_direct_model.py",
                    "--schedule",
                    str(schedule_path),
                    "--model-name",
                    "toy_model",
                    "--entry",
                    "predict",
                    "--output-dir",
                    str(output_dir),
                    "--remap-name",
                    "first_patch",
                    "--patch-allocation",
                    "patch",
                    "--patch-internal-offset",
                    "128",
                    "--produced-bytes",
                    "32",
                    "--continuation-command-index",
                    "1",
                    "--remap-end-command",
                    "2",
                    "--scratch-allocation",
                    "scratch",
                    "--scratch-end-command",
                    "2",
                    "--remap-macro-prefix",
                    "TOY_REMAP",
                    "--remap-header-guard",
                    "TOY_REMAP_H",
                    "--remap-function-name",
                    "toy_invoke_remapped",
                ],
                cwd=Path(__file__).resolve().parents[1],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            package_json = json.loads(
                (output_dir / "static_direct.package.json").read_text(encoding="utf-8")
            )

        self.assertEqual(package_json["schema"], "rrtos.static_direct_package.v1")
        self.assertEqual(package_json["remaps"][0]["arena_bytes"], 192)


if __name__ == "__main__":
    unittest.main()
