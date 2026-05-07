import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from scripts import verify_static_direct_schedule as verifier


def write_schedule(path: Path, schedule: dict) -> None:
    path.write_text(json.dumps(schedule, indent=2) + "\n", encoding="utf-8")


class StaticDirectScheduleVerifierTest(unittest.TestCase):
    def test_reports_liveness_placements_in_kib(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "unit.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [
                {"id": "arena_a", "size": 128, "lifetime": "transient"},
                {"id": "arena_b", "size": 256, "lifetime": "transient"},
            ],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena_a",
                            "offset": 0,
                            "length": 128,
                            "access": "rw",
                        }
                    ],
                },
                {
                    "ordinal": 1,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena_b",
                            "offset": 4,
                            "length": 128,
                            "access": "rw",
                        }
                    ],
                },
            ],
            "commands": [
                {"type": "dispatch", "dispatch_index": 0},
                {"type": "dispatch", "dispatch_index": 1},
            ],
            "peak_transient_bytes": 256,
        }

        report = verifier.verify_schedule(schedule)

        self.assertEqual(report["status"], "ok")
        self.assertEqual(report["arena_bytes"], 256)
        self.assertEqual(report["arena_kib"], 0.25)
        self.assertEqual(report["input_bytes"], 0)
        self.assertEqual(report["output_bytes"], 0)
        self.assertEqual(report["const_bytes"], 0)
        self.assertEqual(report["placements"]["arena_a"]["offset"], 0)
        self.assertEqual(report["placements"]["arena_b"]["offset"], 0)
        self.assertEqual(report["placements"]["arena_a"]["interval"], [0, 1])
        self.assertEqual(report["placements"]["arena_b"]["interval"], [1, 2])

    def test_rejects_empty_explicit_command_list_by_default(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "empty.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 64, "lifetime": "transient"}],
            "dispatches": [],
            "commands": [],
            "peak_transient_bytes": 64,
        }

        with self.assertRaisesRegex(ValueError, "empty command list"):
            verifier.verify_schedule(schedule)

    def test_rejects_missing_explicit_command_list_by_default(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "missing.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 64, "lifetime": "transient"}],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 0,
                            "length": 64,
                            "access": "rw",
                        }
                    ],
                }
            ],
            "peak_transient_bytes": 64,
        }

        with self.assertRaisesRegex(ValueError, "explicit command list"):
            verifier.verify_schedule(schedule)

    def test_rejects_missing_dispatch_command_coverage(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "coverage.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 64, "lifetime": "transient"}],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 0,
                            "length": 64,
                            "access": "rw",
                        }
                    ],
                },
                {
                    "ordinal": 1,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 0,
                            "length": 64,
                            "access": "rw",
                        }
                    ],
                },
            ],
            "commands": [{"type": "dispatch", "dispatch_index": 0}],
            "peak_transient_bytes": 64,
        }

        with self.assertRaisesRegex(ValueError, "missing dispatch command"):
            verifier.verify_schedule(schedule)

    def test_rejects_partial_overlap_within_same_arena_allocation(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "alias.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 128, "lifetime": "transient"}],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 0,
                            "length": 80,
                            "access": "rw",
                        },
                        {
                            "ordinal": 1,
                            "base": "arena",
                            "offset": 40,
                            "length": 80,
                            "access": "rw",
                        },
                    ],
                }
            ],
            "commands": [{"type": "dispatch", "dispatch_index": 0}],
            "peak_transient_bytes": 128,
        }

        with self.assertRaisesRegex(ValueError, "overlapping slices"):
            verifier.verify_schedule(schedule)

    def test_rejects_binding_outside_allocation(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "bounds.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 64, "lifetime": "transient"}],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 32,
                            "length": 64,
                            "access": "rw",
                        }
                    ],
                }
            ],
            "commands": [{"type": "dispatch", "dispatch_index": 0}],
            "peak_transient_bytes": 64,
        }

        with self.assertRaisesRegex(ValueError, "exceeds allocation"):
            verifier.verify_schedule(schedule)

    def test_cli_writes_json_report(self):
        schedule = {
            "schema": "rrtos.iree_hal_schedule.v1",
            "source": "unit.hal.mlir",
            "fixed_shape": True,
            "is_linear": True,
            "allocations": [{"id": "arena", "size": 64, "lifetime": "transient"}],
            "dispatches": [
                {
                    "ordinal": 0,
                    "workgroups": [1, 1, 1],
                    "constants": [],
                    "bindings": [
                        {
                            "ordinal": 0,
                            "base": "arena",
                            "offset": 0,
                            "length": 64,
                            "access": "rw",
                        }
                    ],
                }
            ],
            "commands": [{"type": "dispatch", "dispatch_index": 0}],
            "peak_transient_bytes": 64,
        }

        with tempfile.TemporaryDirectory() as tmp:
            schedule_path = Path(tmp) / "schedule.json"
            output_path = Path(tmp) / "report.json"
            write_schedule(schedule_path, schedule)

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/verify_static_direct_schedule.py",
                    "--schedule",
                    str(schedule_path),
                    "--output",
                    str(output_path),
                ],
                cwd=Path(__file__).resolve().parents[1],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            report = json.loads(output_path.read_text(encoding="utf-8"))
            self.assertEqual(report["status"], "ok")
            self.assertEqual(report["arena_kib"], 0.062)


if __name__ == "__main__":
    unittest.main()
