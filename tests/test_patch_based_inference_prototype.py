import unittest

from scripts import prototype_patch_based_inference as patch


def schedule_with_patchable_component():
    return {
        "schema": "rrtos.iree_hal_schedule.v1",
        "source": "patchable.hal.mlir",
        "fixed_shape": True,
        "is_linear": True,
        "allocations": [
            {"id": "a", "size": 300, "lifetime": "transient"},
            {"id": "b", "size": 80, "lifetime": "transient"},
        ],
        "dispatches": [
            {
                "ordinal": 0,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {"ordinal": 0, "base": "a", "offset": 0, "length": 300, "access": "rw"},
                    {"ordinal": 1, "base": "b", "offset": 0, "length": 80, "access": "rw"},
                ],
            },
            {
                "ordinal": 1,
                "workgroups": [1, 1, 1],
                "constants": [],
                "bindings": [
                    {"ordinal": 0, "base": "a", "offset": 0, "length": 300, "access": "rw"},
                ],
            },
        ],
        "commands": [
            {"type": "dispatch", "dispatch_index": 0},
            {"type": "dispatch", "dispatch_index": 1},
        ],
        "peak_transient_bytes": 380,
    }


def verify_summary_with_patchable_component():
    return {
        "allocation_count": 2,
        "arena_bytes": 400,
        "placements": {
            "a": {
                "interval": [0, 2],
                "offset": 0,
                "size": 300,
                "size_kib": 0.293,
            },
            "b": {
                "interval": [0, 1],
                "offset": 320,
                "size": 80,
                "size_kib": 0.078,
            },
        },
    }


class PatchBasedInferencePrototypeTest(unittest.TestCase):
    def test_patch_candidate_replaces_component_with_tile_scratch(self):
        result = patch.evaluate_patch_candidates(
            schedule_with_patchable_component(),
            name="toy",
            allocation_id="a",
            component_bytes=200,
            output_rows=4,
            output_cols=5,
            output_channels=5,
            element_bytes=2,
            tile_rows=[1, 2],
        )

        one_row = result["candidates"][0]
        two_rows = result["candidates"][1]

        self.assertEqual(result["base_effective_bytes"], 400)
        self.assertEqual(one_row["tile_rows"], 1)
        self.assertEqual(one_row["tile_count"], 4)
        self.assertEqual(one_row["patch_scratch_bytes"], 50)
        self.assertEqual(one_row["patched_allocation_bytes"], 150)
        self.assertEqual(one_row["projected_live_peak_bytes"], 230)
        self.assertEqual(one_row["projected_effective_bytes"], 272)
        self.assertEqual(one_row["savings_bytes"], 128)
        self.assertEqual(two_rows["tile_rows"], 2)
        self.assertEqual(two_rows["patch_scratch_bytes"], 100)
        self.assertEqual(two_rows["projected_live_peak_bytes"], 280)
        self.assertEqual(two_rows["projected_effective_bytes"], 336)

    def test_accepts_static_direct_verify_summary_placements(self):
        result = patch.evaluate_patch_candidates(
            verify_summary_with_patchable_component(),
            name="toy_verify",
            allocation_id="a",
            component_bytes=200,
            output_rows=4,
            output_cols=5,
            output_channels=5,
            element_bytes=2,
            tile_rows=[1],
        )

        self.assertEqual(result["base_effective_bytes"], 400)
        self.assertEqual(result["candidates"][0]["projected_effective_bytes"], 272)
        self.assertEqual(result["source_kind"], "static_direct_verify_summary")

    def test_rejects_component_larger_than_allocation(self):
        with self.assertRaisesRegex(ValueError, "component bytes"):
            patch.evaluate_patch_candidates(
                schedule_with_patchable_component(),
                name="toy",
                allocation_id="a",
                component_bytes=301,
                output_rows=4,
                output_cols=5,
                output_channels=5,
                element_bytes=2,
                tile_rows=[1],
            )

    def test_markdown_marks_report_as_unverified_runtime_estimate(self):
        result = patch.evaluate_patch_candidates(
            schedule_with_patchable_component(),
            name="toy",
            allocation_id="a",
            component_bytes=200,
            output_rows=4,
            output_cols=5,
            output_channels=5,
            element_bytes=2,
            tile_rows=[1],
        )

        report = patch.render_markdown([result])

        self.assertIn("Patch-Based Inference Prototype", report)
        self.assertIn("not a runtime validation", report)
        self.assertIn("Projected KiB", report)
        self.assertIn("`toy`", report)


if __name__ == "__main__":
    unittest.main()
