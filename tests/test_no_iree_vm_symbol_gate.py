import subprocess
import tempfile
from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
SCRIPT = PROJECT_ROOT / "scripts" / "check_no_iree_vm_symbols.py"


class NoIreeVmSymbolGateTest(unittest.TestCase):
    def run_checker(self, map_text: str) -> subprocess.CompletedProcess:
        with tempfile.TemporaryDirectory() as tmpdir:
            map_path = Path(tmpdir) / "firmware.map"
            map_path.write_text(map_text, encoding="utf-8")
            return subprocess.run(
                ["python3", str(SCRIPT), "--map", str(map_path)],
                cwd=PROJECT_ROOT,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                check=False,
            )

    def test_accepts_map_without_vm_or_hal_loader_symbols(self):
        proc = self.run_checker(
            """
            .text.main_dispatch_0
            be_u1000_hello_world_float_linked_library_query
            iree_hal_executable_library_query
            """
        )

        self.assertEqual(proc.returncode, 0, proc.stdout)
        self.assertIn("NO_IREE_VM_SYMBOLS_PASS", proc.stdout)

    def test_rejects_map_with_iree_vm_symbols(self):
        proc = self.run_checker(
            """
            .text.iree_vm_context_run_function
            .text.iree_hal_static_library_loader_create
            """
        )

        self.assertEqual(proc.returncode, 1, proc.stdout)
        self.assertIn("NO_IREE_VM_SYMBOLS_FAIL", proc.stdout)
        self.assertIn("iree_vm_context_run_function", proc.stdout)


if __name__ == "__main__":
    unittest.main()
