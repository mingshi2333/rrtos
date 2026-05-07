import unittest

from scripts import sweep_static_direct_arena_planners as sweep


def schedule_with_fragmentation():
    return {
        "schema": "rrtos.iree_hal_schedule.v1",
        "source": "fragmented.11.hal.mlir",
        "fixed_shape": True,
        "is_linear": True,
        "allocations": [
            {"id": "a", "size": 70, "lifetime": "transient"},
            {"id": "b", "size": 100, "lifetime": "transient"},
            {"id": "c", "size": 100, "lifetime": "transient"},
        ],
        "dispatches": [
            {
                "ordinal": 0,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {"ordinal": 0, "base": "a", "offset": 0, "length": 70, "access": "rw"},
                    {"ordinal": 1, "base": "b", "offset": 0, "length": 100, "access": "rw"},
                ],
            },
            {
                "ordinal": 1,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {"ordinal": 0, "base": "a", "offset": 0, "length": 70, "access": "rw"},
                    {"ordinal": 1, "base": "c", "offset": 0, "length": 100, "access": "rw"},
                ],
            },
        ],
        "commands": [
            {"type": "dispatch", "dispatch_index": 0},
            {"type": "dispatch", "dispatch_index": 1},
        ],
        "peak_transient_bytes": 170,
    }


class StaticDirectArenaPlannerSweepTest(unittest.TestCase):
    def test_size_desc_planner_can_beat_current_input_order(self):
        result = sweep.evaluate_schedule(schedule_with_fragmentation(), "fragmented")
        planners = result["planners"]

        self.assertEqual(planners["input_order_first_fit"]["packed_bytes"], 228)
        self.assertEqual(planners["size_desc_first_fit"]["packed_bytes"], 198)
        self.assertEqual(result["winner"], "size_desc_best_fit")
        self.assertEqual(result["winner_effective_kib"], round(198 / 1024.0, 3))

    def test_markdown_report_contains_current_and_winner(self):
        result = sweep.evaluate_schedule(schedule_with_fragmentation(), "fragmented")
        report = sweep.render_markdown([result])

        self.assertIn("Static-Direct Arena Planner Sweep", report)
        self.assertIn("`fragmented`", report)
        self.assertIn("`size_desc_best_fit`", report)
        self.assertIn("Current KiB", report)


if __name__ == "__main__":
    unittest.main()
