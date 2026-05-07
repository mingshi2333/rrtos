import importlib.util
from pathlib import Path
import unittest


PROJECT_ROOT = Path(__file__).resolve().parents[1]
RUNNER_PATH = PROJECT_ROOT / "scripts" / "be_u1000" / "run_smp_runtime.py"


def load_runner_module():
    spec = importlib.util.spec_from_file_location("run_smp_runtime", RUNNER_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class BeU1000RuntimeRunnerTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.runner = load_runner_module()

    def test_uart_marker_check_passes_when_all_markers_are_present(self):
        log_text = "boot\nBE_U1000_STATIC_DIRECT_PROBE_PASS\nBE_U1000_STATIC_DIRECT_INFER_PASS\n"

        status, first_error = self.runner.check_required_uart_markers(
            log_text,
            [
                "BE_U1000_STATIC_DIRECT_PROBE_PASS",
                "BE_U1000_STATIC_DIRECT_INFER_PASS",
            ],
        )

        self.assertEqual(status, "passed")
        self.assertEqual(first_error, "none")

    def test_uart_marker_check_reports_missing_marker(self):
        status, first_error = self.runner.check_required_uart_markers(
            "boot\nBE_U1000_STATIC_DIRECT_PROBE_PASS\n",
            [
                "BE_U1000_STATIC_DIRECT_PROBE_PASS",
                "BE_U1000_STATIC_DIRECT_INFER_PASS",
            ],
        )

        self.assertEqual(status, "uart-marker-check-failed")
        self.assertIn("BE_U1000_STATIC_DIRECT_INFER_PASS", first_error)

    def test_summarize_status_accepts_marker_check_as_runtime_validation(self):
        self.assertEqual(
            self.runner.summarize_status("passed", "not-run", "passed"),
            ("passed", "none"),
        )
        self.assertEqual(
            self.runner.summarize_status("passed", "not-run", "not-run"),
            ("incomplete", "runtime check not run"),
        )


if __name__ == "__main__":
    unittest.main()
