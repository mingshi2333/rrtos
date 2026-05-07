import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from scripts import extract_iree_hal_schedule as extractor


COMPACT_HAL = """
module {
  util.func public @main() {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c40 = arith.constant 40 : index
    %c576 = arith.constant 576 : index
    %arena = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%arena_ready) : !hal.buffer{%c576}
    %out = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%out_ready) : !hal.buffer{%c40}
    %slice = hal.buffer.subspan<%arena : !hal.buffer>[%c0, %c576] : !hal.buffer
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c1] workgroups([%c1, %c1, %c1]) constants([]) bindings([
      %slice[%c0, %c576] : !hal.buffer,
      %out[%c0, %c40] : !hal.buffer
    ])
    hal.device.queue.dealloca<%device : !hal.device> %arena wait(%done) signal(%freed)
  }
}
""".strip()


HAL_WITH_FILL = """
module {
  util.func public @main() {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c-128_i8 = arith.constant -128 : i8
    %arena = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%arena_ready) : !hal.buffer{%c128}
    hal.command_buffer.fill_buffer<%cmd : !hal.command_buffer> target(%arena : !hal.buffer)[%c64, %c64] pattern(%c-128_i8 : i8)
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c1] workgroups([%c1, %c1, %c1]) constants([]) bindings([
      (%arena : !hal.buffer)[%c0, %c128]
    ]) flags("None")
  }
}
""".strip()


REALISTIC_HAL = """
module {
  util.func public @main(%arg0: !hal.buffer_view) -> (!hal.buffer_view) {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2 = arith.constant 2 : index
    %c8 = arith.constant 8 : index
    %c16 = arith.constant 16 : index
    %c40 = arith.constant 40 : index
    %c64 = arith.constant 64 : index
    %c128 = arith.constant 128 : index
    %c3200 = arith.constant 3200 : index
    %c32_i64 = arith.constant 32 : i64
    %c-128_i32 = arith.constant -128 : i32
    %const = util.global.load immutable @__constant_tensor_weights : !hal.buffer
    %batch = hal.buffer_view.dim<%arg0 : !hal.buffer_view>[0] : index
    %input = hal.buffer_view.buffer<%arg0 : !hal.buffer_view> : !hal.buffer
    %input_len = arith.muli %batch, %c3200 : index
    %ceil = arith.ceildivui %input_len, %c8 : index
    %aligned = util.align %ceil, %c64 : index
    %cond = arith.cmpi eq, %batch, %c1 : index
    cf.cond_br %cond, ^bb1, ^bb2(%input : !hal.buffer)
  ^bb1:
    %arena = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%arena_ready) : !hal.buffer{%aligned}
    %low = arith.index_castui %batch : index to i32
    %wide = arith.index_castui %batch : index to i64
    %high64 = arith.shrui %wide, %c32_i64 : i64
    %high = arith.trunci %high64 : i64 to i32
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c2] workgroups([%c2, %c8, %c1]) constants([%low, %high, %c-128_i32]) bindings([
      (%input : !hal.buffer)[%c0, %input_len],
      (%const : !hal.buffer)[%c0, %c64],
      (%arena : !hal.buffer)[%c0, %aligned]
    ]) flags("None")
    %slice = hal.buffer.subspan<%arena : !hal.buffer>[%c16, %c40] : !hal.buffer
    cf.br ^bb3(%slice : !hal.buffer)
  ^bb2(%dead: !hal.buffer):
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c99] workgroups([%c1, %c1, %c1]) constants([]) bindings([
      (%dead : !hal.buffer)[%c0, %c40]
    ]) flags("None")
  ^bb3(%out_alias: !hal.buffer):
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c1] workgroups([%c1, %c1, %c1]) constants([]) bindings([
      (%out_alias : !hal.buffer)[%c0, %c40]
    ]) flags("None")
    hal.device.queue.dealloca<%device : !hal.device> wait(%done) signal(%freed) buffer(%arena : !hal.buffer)
    util.return %arg0 : !hal.buffer_view
  }
}
""".strip()

INDIRECT_HAL = """
module {
  util.func private @__main_memoize_apply() -> !hal.command_buffer {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c16 = arith.constant 16 : index
    %c32 = arith.constant 32 : index
    %c64 = arith.constant 64 : index
    %c-128_i8 = arith.constant -128 : i8
    %device = util.global.load immutable @__device_0 : !hal.device
    %exe = util.global.load immutable @__device_0_executable_linked : !hal.executable
    %cmd = hal.command_buffer.create device(%device : !hal.device) mode("None") categories("Transfer|Dispatch") affinity(%c-1_i64) bindings(%c2) : !hal.command_buffer
    hal.command_buffer.fill_buffer<%cmd : !hal.command_buffer> target(%c1 : index)[%c16, %c16] pattern(%c-128_i8 : i8)
    hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c1] workgroups([%c1, %c1, %c1]) constants([]) bindings([
      (%c0 : index)[%c0, %c32],
      (%c1 : index)[%c0, %c64]
    ]) flags("None")
    hal.command_buffer.finalize<%cmd : !hal.command_buffer>
    util.return %cmd : !hal.command_buffer
  }
  util.func public @main(%arg0: !hal.buffer_view) -> (!hal.buffer_view) {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2 = arith.constant 2 : index
    %c32 = arith.constant 32 : index
    %c64 = arith.constant 64 : index
    %ready = util.global.load immutable @ready : !hal.fence
    %cmd = util.global.load immutable @__main_memoize_result_0_device_0 : !hal.command_buffer
    %input = hal.buffer_view.buffer<%arg0 : !hal.buffer_view> : !hal.buffer
    %arena = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%arena_ready) : !hal.buffer{%c64}
    hal.device.queue.execute.indirect<%device : !hal.device> affinity(%c-1_i64) wait(%ready) signal(%done) commands(%cmd) bindings([
      (%input : !hal.buffer)[%c0, %c32],
      (%arena : !hal.buffer)[%c0, %c64]
    ])
    util.return %arg0 : !hal.buffer_view
  }
}
""".strip()


class IreeHalScheduleExtractorTest(unittest.TestCase):
    def write_fixture(self, name: str, text: str) -> Path:
        path = Path(self.tmp.name) / name
        path.write_text(text, encoding="utf-8")
        return path

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()

    def tearDown(self):
        self.tmp.cleanup()

    def test_parse_compact_hal_schedule(self):
        hal_path = self.write_fixture("model.11.hal.mlir", COMPACT_HAL)

        schedule = extractor.parse_hal_schedule(hal_path)

        self.assertEqual(schedule["schema"], "rrtos.iree_hal_schedule.v1")
        self.assertEqual(schedule["source"], "model.11.hal.mlir")
        self.assertTrue(schedule["fixed_shape"])
        self.assertTrue(schedule["is_linear"])
        self.assertEqual(schedule["allocations"][0]["id"], "arena")
        self.assertEqual(schedule["allocations"][0]["size"], 576)
        self.assertEqual(schedule["dispatches"][0]["ordinal"], 1)
        self.assertEqual(schedule["dispatches"][0]["workgroups"], [1, 1, 1])
        self.assertEqual(schedule["dispatches"][0]["bindings"][0]["base"], "arena")
        self.assertEqual(schedule["dispatches"][0]["bindings"][0]["offset"], 0)
        self.assertEqual(schedule["dispatches"][0]["bindings"][0]["length"], 576)
        self.assertEqual(schedule["commands"], [{"type": "dispatch", "dispatch_index": 0}])
        self.assertEqual(schedule["max_transient_allocation_bytes"], 576)
        self.assertEqual(schedule["peak_transient_bytes"], 616)
        self.assertEqual(schedule["warnings"], [])

    def test_parse_fill_buffer_commands_in_order(self):
        hal_path = self.write_fixture("fill.11.hal.mlir", HAL_WITH_FILL)

        schedule = extractor.parse_hal_schedule(hal_path)

        self.assertEqual(
            schedule["commands"],
            [
                {
                    "type": "fill",
                    "target": {
                        "base": "arena",
                        "offset": 64,
                        "length": 64,
                        "access": "write",
                    },
                    "pattern": 128,
                },
                {"type": "dispatch", "dispatch_index": 0},
            ],
        )

    def test_parse_realistic_hal_schedule_with_external_buffers_and_cfg_aliases(self):
        hal_path = self.write_fixture("real.11.hal.mlir", REALISTIC_HAL)

        schedule = extractor.parse_hal_schedule(hal_path)

        self.assertEqual([dispatch["ordinal"] for dispatch in schedule["dispatches"]], [2, 1])
        allocations = {allocation["id"]: allocation for allocation in schedule["allocations"]}
        self.assertEqual(allocations["input"]["lifetime"], "input")
        self.assertEqual(allocations["input"]["size"], 3200)
        self.assertEqual(allocations["const"]["lifetime"], "constant")
        self.assertEqual(allocations["const"]["size"], 64)
        self.assertEqual(allocations["arena"]["size"], 448)
        self.assertEqual(schedule["dispatches"][0]["constants"], [1, 0, 4294967168])
        self.assertEqual(schedule["dispatches"][1]["bindings"][0]["base"], "arena")
        self.assertEqual(schedule["dispatches"][1]["bindings"][0]["offset"], 16)
        self.assertEqual(schedule["dispatches"][1]["bindings"][0]["length"], 40)
        self.assertEqual(
            schedule["commands"],
            [{"type": "dispatch", "dispatch_index": 0}, {"type": "dispatch", "dispatch_index": 1}],
        )
        self.assertEqual(schedule["warnings"], ["assumed batch_size=1 for 1 hal.buffer_view.dim value(s)"])

    def test_parse_memoized_indirect_command_buffer_schedule(self):
        hal_path = self.write_fixture("indirect.11.hal.mlir", INDIRECT_HAL)

        schedule = extractor.parse_hal_schedule(hal_path)

        self.assertEqual([dispatch["ordinal"] for dispatch in schedule["dispatches"]], [1])
        allocations = {allocation["id"]: allocation for allocation in schedule["allocations"]}
        self.assertEqual(allocations["input"]["lifetime"], "input")
        self.assertEqual(allocations["input"]["size"], 32)
        self.assertEqual(allocations["arena"]["lifetime"], "transient")
        self.assertEqual(allocations["arena"]["size"], 64)
        self.assertEqual(
            schedule["commands"],
            [
                {
                    "type": "fill",
                    "target": {
                        "base": "arena",
                        "offset": 16,
                        "length": 16,
                        "access": "write",
                    },
                    "pattern": 128,
                },
                {"type": "dispatch", "dispatch_index": 0},
            ],
        )
        self.assertEqual(schedule["dispatches"][0]["bindings"][0]["base"], "input")
        self.assertEqual(schedule["dispatches"][0]["bindings"][0]["length"], 32)
        self.assertEqual(schedule["dispatches"][0]["bindings"][1]["base"], "arena")
        self.assertEqual(schedule["dispatches"][0]["bindings"][1]["length"], 64)

    def test_memoized_indirect_binding_cannot_exceed_execute_slot(self):
        hal_path = self.write_fixture(
            "bad_indirect.11.hal.mlir",
            INDIRECT_HAL.replace(
                "(%c1 : index)[%c0, %c64]",
                "(%c1 : index)[%c16, %c64]",
            ),
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "execute.indirect slot"):
            extractor.parse_hal_schedule(hal_path)

    def test_transient_dispatch_binding_cannot_exceed_allocation(self):
        hal_path = self.write_fixture(
            "bad_allocation.11.hal.mlir",
            HAL_WITH_FILL.replace(
                "(%arena : !hal.buffer)[%c0, %c128]",
                "(%arena : !hal.buffer)[%c64, %c128]",
            ),
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "binding exceeds allocation"):
            extractor.parse_hal_schedule(hal_path)

    def test_cli_writes_schedule_json(self):
        hal_path = self.write_fixture("model.11.hal.mlir", COMPACT_HAL)
        output_path = Path(self.tmp.name) / "hal_schedule.json"

        result = subprocess.run(
            [
                sys.executable,
                "scripts/extract_iree_hal_schedule.py",
                "--hal",
                str(hal_path),
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
        schedule = json.loads(output_path.read_text(encoding="utf-8"))
        self.assertEqual(schedule["dispatches"][0]["ordinal"], 1)

    def test_unknown_dispatch_binding_raises(self):
        hal_path = self.write_fixture(
            "bad.11.hal.mlir",
            COMPACT_HAL.replace("%slice[%c0, %c576] : !hal.buffer", "%missing[%c0, %c576] : !hal.buffer"),
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "unknown binding"):
            extractor.parse_hal_schedule(hal_path)

    def test_cli_returns_nonzero_for_unknown_binding(self):
        hal_path = self.write_fixture(
            "bad.11.hal.mlir",
            COMPACT_HAL.replace("%slice[%c0, %c576] : !hal.buffer", "%missing[%c0, %c576] : !hal.buffer"),
        )
        output_path = Path(self.tmp.name) / "hal_schedule.json"

        result = subprocess.run(
            [
                sys.executable,
                "scripts/extract_iree_hal_schedule.py",
                "--hal",
                str(hal_path),
                "--output",
                str(output_path),
            ],
            cwd=Path(__file__).resolve().parents[1],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )

        self.assertNotEqual(result.returncode, 0)
        self.assertIn("unknown binding", result.stderr)
        self.assertFalse(output_path.exists())

    def test_unresolved_dynamic_workgroup_dimension_raises(self):
        hal_path = self.write_fixture(
            "dynamic.11.hal.mlir",
            COMPACT_HAL.replace("workgroups([%c1, %c1, %c1])", "workgroups([%dynamic, %c1, %c1])"),
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "unresolved value"):
            extractor.parse_hal_schedule(hal_path)

    def test_stream_transient_smaller_than_hal_peak_raises(self):
        hal_path = self.write_fixture("model.11.hal.mlir", COMPACT_HAL)
        stream_path = self.write_fixture(
            "model.7.stream.mlir",
            "\n".join(
                [
                    "module {",
                    "  util.func public @main() {",
                    "    %c128 = arith.constant 128 : index",
                    "    %result, %timepoint = stream.resource.alloca uninitialized => !stream.resource<transient>{%c128} => !stream.timepoint",
                    "  }",
                    "}",
                ]
            ),
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "stream transient peak"):
            extractor.parse_hal_schedule(hal_path, stream_path=stream_path)

    def test_stream_peak_compares_against_max_single_allocation_not_live_peak(self):
        hal_path = self.write_fixture("model.11.hal.mlir", COMPACT_HAL)
        stream_path = self.write_fixture(
            "model.7.stream.mlir",
            "\n".join(
                [
                    "module {",
                    "  util.func public @main() {",
                    "    %c576 = arith.constant 576 : index",
                    "    %result, %timepoint = stream.resource.alloca uninitialized => !stream.resource<transient>{%c576} => !stream.timepoint",
                    "  }",
                    "}",
                ]
            ),
        )

        schedule = extractor.parse_hal_schedule(hal_path, stream_path=stream_path)

        self.assertEqual(schedule["max_transient_allocation_bytes"], 576)
        self.assertEqual(schedule["peak_transient_bytes"], 616)
        self.assertTrue(
            any("differs from HAL live peak 616" in warning for warning in schedule["warnings"])
        )

    def test_native_export_count_lower_than_largest_dispatch_ordinal_raises(self):
        schedule = {"dispatches": [{"ordinal": 0}, {"ordinal": 2}]}
        nm_output = "\n".join(
            [
                "00000000 T model_dispatch_0",
                "00000010 T model_dispatch_1",
                "00000020 T model_linked_library_query",
            ]
        )

        with self.assertRaisesRegex(extractor.ScheduleParseError, "native executable export count"):
            extractor.check_native_export_count(schedule, nm_output=nm_output)

    def test_native_export_count_accepts_query_symbol_plus_enough_dispatches(self):
        schedule = {"dispatches": [{"ordinal": 0}, {"ordinal": 2}]}
        nm_output = "\n".join(
            [
                "00000000 T main_dispatch_0_slow_memcpy",
                "00000010 T main_dispatch_1_conv",
                "00000020 T main_dispatch_2_elementwise",
                "00000030 T model_linked_library_query",
            ]
        )

        warnings = extractor.check_native_export_count(schedule, nm_output=nm_output)

        self.assertEqual(warnings, [])


if __name__ == "__main__":
    unittest.main()
