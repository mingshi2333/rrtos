import subprocess
import tempfile
import unittest
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]
SCRIPT = PROJECT_ROOT / "scripts" / "report_ai_footprint_attribution.sh"


class ReportAiFootprintAttributionTest(unittest.TestCase):
    def test_accepts_custom_map_needles_for_non_generated_model_targets(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            source = tmp_path / "tiny.c"
            image = tmp_path / "tiny"
            wrapper_obj = tmp_path / "wrapper.o"
            dispatch_obj = tmp_path / "dispatch.o"
            runtime_obj = tmp_path / "runtime.o"
            iree_root = tmp_path / "iree" / "runtime" / "src"
            map_file = tmp_path / "tiny.map"
            output = tmp_path / "report.md"

            source.write_text("int main(void) { return 0; }\n", encoding="utf-8")
            subprocess.run(["cc", "-c", str(source), "-o", str(wrapper_obj)], check=True)
            subprocess.run(["cc", "-c", str(source), "-o", str(dispatch_obj)], check=True)
            subprocess.run(["cc", "-c", str(source), "-o", str(runtime_obj)], check=True)
            subprocess.run(["cc", str(source), "-o", str(image)], check=True)

            for component in ("iree/base", "iree/vm", "iree/hal", "iree/modules", "iree/io"):
                component_dir = iree_root / component
                component_dir.mkdir(parents=True, exist_ok=True)
                subprocess.run(
                    ["cc", "-c", str(source), "-o", str(component_dir / "tiny.obj")],
                    check=True,
                )

            map_file.write_text(
                "\n".join(
                    [
                        "00000000 00000000 00000011 4 custom_wrapper:(.text)",
                        "00000000 00000000 00000022 4 custom_dispatch:(.text)",
                        "00000000 00000000 00000033 4 librv_aios_ai.a(ai_model_registry.c.obj):(.text)",
                        "00000000 00000000 00000044 4 third_party/iree/runtime/src/iree/vm/test.obj:(.text)",
                    ]
                ),
                encoding="utf-8",
            )

            subprocess.run(
                [
                    "bash",
                    str(SCRIPT),
                    "--label",
                    "custom",
                    "--image",
                    str(image),
                    "--map",
                    str(map_file),
                    "--model-wrapper-obj",
                    str(wrapper_obj),
                    "--dispatch-obj",
                    str(dispatch_obj),
                    "--ai-runtime-obj",
                    str(runtime_obj),
                    "--iree-root",
                    str(iree_root),
                    "--map-model-wrapper-needle",
                    "custom_wrapper",
                    "--map-dispatch-needle",
                    "custom_dispatch",
                    "--regeneration-command",
                    "pixi run -e rv32 report-custom",
                    "--output",
                    str(output),
                ],
                check=True,
            )

            report = output.read_text(encoding="utf-8")

        self.assertIn("| generated model wrapper | 17 |", report)
        self.assertIn("| generated dispatch object | 34 |", report)
        self.assertIn("pixi run -e rv32 report-custom", report)


if __name__ == "__main__":
    unittest.main()
