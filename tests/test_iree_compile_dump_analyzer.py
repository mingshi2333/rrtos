import tempfile
import unittest
from pathlib import Path

from scripts import analyze_iree_compile_dump as analyzer


class IreeCompileDumpAnalyzerTest(unittest.TestCase):
    def test_scan_allocations_evaluates_batched_transient_pack_size(self):
        mlir = "\n".join(
            [
                "module {",
                "  util.func public @main(%arg0: !hal.buffer_view) {",
                "    %c64 = arith.constant 64 : index",
                "    %c3920 = arith.constant 3920 : index",
                "    %c204800 = arith.constant 204800 : index",
                "    %c58752 = arith.constant 58752 : index",
                "    %c13312 = arith.constant 13312 : index",
                "    %c14336 = arith.constant 14336 : index",
                "    %c5760 = arith.constant 5760 : index",
                "    %0 = hal.buffer_view.dim<%arg0 : !hal.buffer_view>[0] : index",
                "    %3 = arith.muli %0, %c3920 : index",
                "    %4 = arith.muli %0, %c204800 : index",
                "    %5 = arith.muli %0, %c58752 : index",
                "    %6 = arith.muli %0, %c13312 : index",
                "    %7 = arith.muli %0, %c14336 : index",
                "    %9 = arith.muli %0, %c5760 : index",
                "    %13 = util.align %3, %c64 : index",
                "    %14 = arith.addi %13, %4 : index",
                "    %15 = arith.addi %14, %5 : index",
                "    %16 = arith.addi %15, %6 : index",
                "    %17 = arith.addi %16, %7 : index",
                "    %18 = arith.addi %17, %7 : index",
                "    %19 = arith.addi %18, %9 : index",
                "    %result, %timepoint = stream.resource.alloca uninitialized => !stream.resource<transient>{%19} => !stream.timepoint",
                "    util.return",
                "  }",
                "}",
            ]
        )

        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "model.7.stream.mlir"
            path.write_text(mlir, encoding="utf-8")

            rows = analyzer.scan_allocations(path, batch_size=1)

        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0].size_ref, "%19")
        self.assertEqual(rows[0].size_value, 315264)
        self.assertIn("align((batch(1) * 3920), 64)", rows[0].expression)

    def test_scan_stream_accesses_maps_execute_captures_to_resource_ranges(self):
        mlir = "\n".join(
            [
                "module {",
                "  util.func public @main(%arg0: !hal.buffer_view) {",
                "    %c0 = arith.constant 0 : index",
                "    %c16 = arith.constant 16 : index",
                "    %c64 = arith.constant 64 : index",
                "    %0 = hal.buffer_view.dim<%arg0 : !hal.buffer_view>[0] : index",
                "    %1 = arith.muli %0, %c64 : index",
                "    %result, %timepoint = stream.resource.alloca uninitialized => !stream.resource<transient>{%1} => !stream.timepoint",
                "    %done = stream.cmd.execute with(%result as %arg1: !stream.resource<transient>{%1}) {",
                "      stream.cmd.fill %c0, %arg1[%c0 for %c16] : i32 -> !stream.resource<transient>{%1}",
                "      stream.cmd.dispatch @main_dispatch_0::@main_dispatch_0[%0]() {",
                "        ro %arg1[%c0 for %c16] : !stream.resource<transient>{%1}",
                "        rw %arg1[%c16 for %1] : !stream.resource<transient>{%1}",
                "      }",
                "    } => !stream.timepoint",
                "    util.return",
                "  }",
                "}",
            ]
        )

        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "model.7.stream.mlir"
            path.write_text(mlir, encoding="utf-8")

            captures, accesses = analyzer.scan_stream_resource_accesses(path, batch_size=1)

        self.assertEqual(len(captures), 1)
        self.assertEqual(captures[0].source_ref, "%result")
        self.assertEqual(captures[0].arg_ref, "%arg1")
        self.assertEqual(captures[0].size_value, 64)
        self.assertEqual([access.mode for access in accesses], ["fill", "ro", "rw"])
        self.assertEqual([access.length_value for access in accesses], [16, 16, 64])


if __name__ == "__main__":
    unittest.main()
