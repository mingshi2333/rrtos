import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from scripts import generate_static_direct_remap_plan as remap_plan


PROJECT_ROOT = Path(__file__).resolve().parents[1]


VERIFY_SUMMARY = {
    "status": "ok",
    "source": "miniresnetv1_s1_64x50_tl_int8.11.hal.mlir",
    "arena_bytes": 351104,
    "placements": {
        "transient_buffer_1": {
            "interval": [0, 13],
            "offset": 35840,
            "size": 315264,
            "size_kib": 307.875,
        },
        "transient_buffer": {
            "interval": [7, 17],
            "offset": 0,
            "size": 35840,
            "size_kib": 35.0,
        },
    },
}


class StaticDirectRemapPlanTest(unittest.TestCase):
    def test_generates_miniresnet_first_layer_remap_plan(self):
        plan = remap_plan.build_remap_plan(
            VERIFY_SUMMARY,
            name="miniresnet_first_layer_patch",
            patch_allocation="transient_buffer_1",
            patch_internal_offset=208768,
            produced_bytes=58752,
            continuation_command_index=5,
            remap_end_command=13,
            scratch_allocation="transient_buffer",
            scratch_end_command=17,
        )

        self.assertEqual(plan["schema"], "rrtos.static_direct_remap_plan.v1")
        self.assertEqual(plan["name"], "miniresnet_first_layer_patch")
        self.assertEqual(plan["continuation_command_index"], 5)
        self.assertEqual(plan["replaced_command_range"], [0, 5])
        self.assertEqual(plan["original_arena_bytes"], 351104)
        self.assertEqual(plan["arena_bytes"], 142336)
        self.assertEqual(plan["arena_kib"], 139.0)
        self.assertEqual(plan["savings_bytes"], 208768)
        self.assertEqual(plan["savings_kib"], 203.875)

        self.assertEqual(
            plan["rebased_region"],
            {
                "allocation": "transient_buffer_1",
                "old_binding_offset": 35840,
                "old_binding_bytes": 315264,
                "old_internal_base": 208768,
                "old_actual_base": 244608,
                "new_offset": 0,
                "bytes": 106496,
                "produced_bytes": 58752,
                "command_range": [5, 13],
            },
        )
        self.assertEqual(
            plan["constant_remaps"],
            [
                {
                    "allocation": "transient_buffer_1",
                    "command_range": [5, 13],
                    "old_value_begin": 208768,
                    "old_value_end": 315264,
                    "addend": -208768,
                }
            ],
        )
        self.assertEqual(
            plan["scratch_remaps"],
            [
                {
                    "allocation": "transient_buffer",
                    "command_range": [5, 17],
                    "old_offset": 0,
                    "old_length": 35840,
                    "new_offset": 106496,
                    "new_length": 35840,
                }
            ],
        )

    def test_rejects_patch_internal_offset_outside_allocation(self):
        with self.assertRaisesRegex(ValueError, "patch internal offset"):
            remap_plan.build_remap_plan(
                VERIFY_SUMMARY,
                name="bad",
                patch_allocation="transient_buffer_1",
                patch_internal_offset=315264,
                produced_bytes=64,
                continuation_command_index=5,
                remap_end_command=13,
            )

    def test_cli_writes_json_and_markdown(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            summary_path = tmp_path / "verify.json"
            json_path = tmp_path / "plan.json"
            md_path = tmp_path / "plan.md"
            summary_path.write_text(
                json.dumps(VERIFY_SUMMARY, indent=2) + "\n",
                encoding="utf-8",
            )

            result = subprocess.run(
                [
                    sys.executable,
                    "scripts/generate_static_direct_remap_plan.py",
                    "--verify-summary",
                    str(summary_path),
                    "--name",
                    "miniresnet_first_layer_patch",
                    "--patch-allocation",
                    "transient_buffer_1",
                    "--patch-internal-offset",
                    "208768",
                    "--produced-bytes",
                    "58752",
                    "--continuation-command-index",
                    "5",
                    "--remap-end-command",
                    "13",
                    "--scratch-allocation",
                    "transient_buffer",
                    "--scratch-end-command",
                    "17",
                    "--output-json",
                    str(json_path),
                    "--output-md",
                    str(md_path),
                    "--output-c",
                    str(tmp_path / "remap.c"),
                    "--output-h",
                    str(tmp_path / "remap.h"),
                    "--macro-prefix",
                    "MINIRESNET_PATCH_REMAP",
                    "--header-guard",
                    "MINIRESNET_PATCH_ARENA_REMAP_H",
                    "--function-name",
                    "miniresnet_patch_invoke_continuation_remapped",
                ],
                cwd=Path(__file__).resolve().parents[1],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            plan = json.loads(json_path.read_text(encoding="utf-8"))
            report = md_path.read_text(encoding="utf-8")
            generated_source = (tmp_path / "remap.c").read_text(encoding="ascii")
            generated_header = (tmp_path / "remap.h").read_text(encoding="ascii")

        self.assertEqual(plan["arena_bytes"], 142336)
        self.assertIn("Static-Direct Remap Plan", report)
        self.assertIn("`miniresnet_first_layer_patch`", report)
        self.assertIn("139.000", report)
        self.assertIn("Generated by scripts/generate_static_direct_remap_plan.py", generated_source)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_BINDING_OFFSET = 35840", generated_source)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_INTERNAL_BASE = 208768", generated_source)
        self.assertIn("#define MINIRESNET_PATCH_REMAP_ARENA_BYTES 142336u", generated_header)

    def test_renders_c_runtime_from_plan_without_manual_offsets(self):
        plan = remap_plan.build_remap_plan(
            VERIFY_SUMMARY,
            name="miniresnet_first_layer_patch",
            patch_allocation="transient_buffer_1",
            patch_internal_offset=208768,
            produced_bytes=58752,
            continuation_command_index=5,
            remap_end_command=13,
            scratch_allocation="transient_buffer",
            scratch_end_command=17,
        )

        header = remap_plan.render_c_header(
            plan,
            header_guard="MINIRESNET_PATCH_ARENA_REMAP_H",
            macro_prefix="MINIRESNET_PATCH_REMAP",
            function_name="miniresnet_patch_invoke_continuation_remapped",
        )
        source = remap_plan.render_c_source(
            plan,
            header_name="miniresnet_patch_arena_remap.h",
            macro_prefix="MINIRESNET_PATCH_REMAP",
            function_name="miniresnet_patch_invoke_continuation_remapped",
        )

        self.assertIn("#define MINIRESNET_PATCH_REMAP_ARENA_BYTES 142336u", header)
        self.assertIn("#define MINIRESNET_PATCH_REMAP_FIRST_OUTPUT_OFFSET 0u", header)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_BINDING_OFFSET = 35840", source)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_BINDING_BYTES = 315264", source)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_INTERNAL_BASE = 208768", source)
        self.assertIn("MINIRESNET_PATCH_REMAP_BIG_SUFFIX_END_COMMAND = 13", source)
        self.assertIn("MINIRESNET_PATCH_REMAP_WORK_BUFFER_END_COMMAND = 17", source)
        self.assertIn("miniresnet_patch_invoke_continuation_remapped", source)

    def test_real_miniresnet_summary_reproduces_current_canary_constants(self):
        summary = json.loads(
            (
                PROJECT_ROOT
                / "logs"
                / "miniresnet_static_direct_schedule_verify.json"
            ).read_text(encoding="utf-8")
        )

        plan = remap_plan.build_remap_plan(
            summary,
            name="miniresnet_first_layer_patch",
            patch_allocation="transient_buffer_1",
            patch_internal_offset=208768,
            produced_bytes=58752,
            continuation_command_index=5,
            remap_end_command=13,
            scratch_allocation="transient_buffer",
            scratch_end_command=17,
        )
        header_text = (
            PROJECT_ROOT
            / "apps"
            / "mnist_app"
            / "src"
            / "miniresnet_patch_arena_remap.h"
        ).read_text(encoding="ascii")
        source_text = (
            PROJECT_ROOT
            / "apps"
            / "mnist_app"
            / "src"
            / "miniresnet_patch_arena_remap.c"
        ).read_text(encoding="ascii")

        self.assertEqual(plan["arena_bytes"], 142336)
        self.assertEqual(plan["rebased_region"]["old_binding_offset"], 35840)
        self.assertEqual(plan["rebased_region"]["old_binding_bytes"], 315264)
        self.assertEqual(plan["rebased_region"]["old_internal_base"], 208768)
        self.assertEqual(plan["rebased_region"]["old_actual_base"], 244608)
        self.assertEqual(plan["rebased_region"]["bytes"], 106496)
        self.assertEqual(plan["scratch_remaps"][0]["new_offset"], 106496)
        self.assertIn("#define MINIRESNET_PATCH_REMAP_ARENA_BYTES 142336u", header_text)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_BINDING_OFFSET = 35840", source_text)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_BINDING_BYTES = 315264", source_text)
        self.assertIn("MINIRESNET_PATCH_REMAP_OLD_BIG_INTERNAL_BASE = 208768", source_text)
        self.assertIn("MINIRESNET_PATCH_REMAP_WORK_BUFFER_NEW_OFFSET", source_text)


if __name__ == "__main__":
    unittest.main()
