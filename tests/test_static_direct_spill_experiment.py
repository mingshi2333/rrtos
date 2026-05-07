import unittest

from scripts import prototype_static_direct_spill_experiment as spill
from tests.test_static_direct_arena_planner_sweep import schedule_with_fragmentation


class StaticDirectSpillExperimentTest(unittest.TestCase):
    def test_elide_candidate_reports_projected_savings(self):
        result = spill.evaluate_spill_candidates(schedule_with_fragmentation(), "fragmented", top=3)
        best = result["top_candidates"][0]

        self.assertEqual(result["current_effective_bytes"], 228)
        self.assertEqual(result["model_base_effective_bytes"], 198)
        self.assertEqual(result["verified_to_model_gap_bytes"], 30)
        self.assertEqual(best["allocation"], "a")
        self.assertEqual(best["transform"], "elide_or_external_spill")
        self.assertEqual(best["projected_effective_bytes"], 100)
        self.assertEqual(best["savings_bytes"], 98)
        self.assertEqual(best["savings_basis"], "command_liveness_model")
        self.assertEqual(best["external_spill_bytes"], 70)

    def test_markdown_report_includes_external_spill_warning_table(self):
        result = spill.evaluate_spill_candidates(schedule_with_fragmentation(), "fragmented", top=1)
        report = spill.render_markdown([result])

        self.assertIn("Static-Direct Spill/Fusion Prototype", report)
        self.assertIn("upper-bound compiler experiment", report)
        self.assertIn("`elide_or_external_spill`", report)
        self.assertIn("Model Gap KiB", report)
        self.assertIn("External KiB", report)


if __name__ == "__main__":
    unittest.main()
