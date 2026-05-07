import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from scripts import generate_static_direct_model as package_generator

from tests.test_static_direct_model_package import PACKAGE_SCHEDULE


PROJECT_ROOT = Path(__file__).resolve().parents[1]


class StaticDirectPackageValidatorTest(unittest.TestCase):
    def test_validator_accepts_package_hashes_and_no_vm_map(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            output_dir = tmp_path / "package"
            map_path = tmp_path / "toy.map"
            map_path.write_text(".text.dispatch\n", encoding="utf-8")
            package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
                deployment=package_generator.DeploymentConfig(map_file=map_path),
            )

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/validate_static_direct_package.py",
                    "--package",
                    str(output_dir / "static_direct.package.json"),
                ],
                cwd=PROJECT_ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("STATIC_DIRECT_PACKAGE_HASH_PASS", result.stdout)
        self.assertIn("NO_IREE_VM_SYMBOLS_PASS", result.stdout)

    def test_validator_rejects_hash_drift(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            output_dir = tmp_path / "package"
            package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
            )
            descriptor_path = output_dir / "toy_model_static_direct_desc.h"
            descriptor_path.write_text(
                descriptor_path.read_text(encoding="ascii") + "\n",
                encoding="ascii",
            )

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/validate_static_direct_package.py",
                    "--package",
                    str(output_dir / "static_direct.package.json"),
                ],
                cwd=PROJECT_ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertIn("STATIC_DIRECT_PACKAGE_HASH_FAIL", result.stdout)

    def test_validator_rejects_vm_symbols_in_map(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            output_dir = tmp_path / "package"
            map_path = tmp_path / "toy.map"
            map_path.write_text(".text.iree_vm_context_run_function\n", encoding="utf-8")
            package_generator.generate_package(
                PACKAGE_SCHEDULE,
                model_name="toy_model",
                entry="predict",
                output_dir=output_dir,
                deployment=package_generator.DeploymentConfig(map_file=map_path),
            )

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/validate_static_direct_package.py",
                    "--package",
                    str(output_dir / "static_direct.package.json"),
                ],
                cwd=PROJECT_ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

        self.assertEqual(result.returncode, 1, result.stdout)
        self.assertIn("NO_IREE_VM_SYMBOLS_FAIL", result.stdout)


if __name__ == "__main__":
    unittest.main()
