import subprocess
import tempfile
import unittest
import re
from pathlib import Path

import yaml


PROJECT_ROOT = Path(__file__).resolve().parents[1]


class BeU1000AiMicroDemoTest(unittest.TestCase):
    def test_cmake_exposes_explicit_ai_micro_demo_lane(self):
        cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()

        self.assertIn('BE_U1000_APP STREQUAL "ai_micro_demo"', cmake_text)
        self.assertIn("apps/be_u1000_ai_micro_demo", cmake_text)
        self.assertIn("ai_micro_demo requires OS_AI_EN=ON", cmake_text)

    def test_cmake_exposes_cpp_ai_micro_demo_lane(self):
        cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("ai_micro_demo_cpp", cmake_text)
        self.assertIn('BE_U1000_APP STREQUAL "ai_micro_demo_cpp"', cmake_text)
        self.assertIn("apps/be_u1000_ai_micro_demo_cpp", cmake_text)
        self.assertIn("ai_micro_demo_cpp requires OS_AI_EN=ON", cmake_text)
        self.assertIn("ai_micro_demo_cpp requires RRTOS_CXX_EN=ON", cmake_text)
        self.assertIn("configure-ai-micro-cpp", pixi_text)
        self.assertIn("build-ai-micro-cpp", pixi_text)
        self.assertIn("-DBE_U1000_MEMORY_MODEL=flash", pixi_text)

    def test_demo_sources_and_model_contract_are_present(self):
        app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_micro_demo"
        main_text = (app_dir / "main.c").read_text()
        app_cmake_text = (app_dir / "CMakeLists.txt").read_text()
        config_text = (PROJECT_ROOT / "ai_models_be_u1000_ai_micro.yaml").read_text()
        model_path = app_dir / "model" / "hello_world_float.tflite"

        self.assertTrue(model_path.exists(), "Official TFLite hello_world float model must exist")

        self.assertIn("BE_U1000_TFLITE_HELLO_DEMO_PASS", main_text)
        self.assertIn("AI_MICRO_TASK_STACK_SIZE = 16 * 1024", main_text)
        self.assertIn('section(".shared.ai_stack")', main_text)
        self.assertIn("ai_micro_halt", main_text)
        self.assertIn('ai_model_find_by_name("be_u1000_hello_world_float")', main_text)
        self.assertIn("ai_model_get_input_info", main_text)
        self.assertIn("ai_model_get_output_info", main_text)
        self.assertIn("ai_infer_sync", main_text)
        self.assertIn("float_to_bits", main_text)
        self.assertIn("y_hex", main_text)
        self.assertIn("HELLO_WORLD_INPUT_BITS", main_text)
        self.assertNotIn("score_to_milli(float", main_text)
        self.assertNotIn("ai_be_u1000_hello_world_float_run(&input, &output)", main_text)
        self.assertIn("add_subdirectory(generated)", app_cmake_text)
        self.assertIn("$<COMPILE_LANGUAGE:C>", app_cmake_text)
        self.assertIn("be_u1000_hello_world_float", config_text)
        self.assertIn("hello_world_float.tflite", config_text)
        self.assertGreater(model_path.stat().st_size, 1024)
        self.assertLess(model_path.stat().st_size, 4096)

    def test_cpp_demo_reuses_c_ai_contract_through_cxx_app_layer(self):
        app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_micro_demo_cpp"
        main_path = app_dir / "main.cpp"
        cmake_path = app_dir / "CMakeLists.txt"

        self.assertTrue(main_path.exists(), "C++ AI micro demo must have a main.cpp")
        self.assertTrue(cmake_path.exists(), "C++ AI micro demo must have a CMakeLists.txt")

        main_text = main_path.read_text()
        cmake_text = cmake_path.read_text()
        c_api_text = (PROJECT_ROOT / "ai" / "include" / "ai_model_registry_c_api.h").read_text()
        registry_text = (PROJECT_ROOT / "ai" / "include" / "ai_model_registry.h").read_text()

        self.assertIn('extern "C"', main_text)
        self.assertIn("ai_model_registry_c_api.h", main_text)
        self.assertNotIn("ai_model_registry.h", main_text)
        self.assertNotIn("iree/", c_api_text)
        self.assertIn('include "ai_model_registry_c_api.h"', registry_text)
        self.assertIn("HelloWorldRunner", main_text)
        self.assertIn("BE_U1000_TFLITE_HELLO_CPP_DEMO_PASS", main_text)
        self.assertIn('ai_model_find_by_name("be_u1000_hello_world_float")', main_text)
        self.assertIn("ai_model_get_input_info", main_text)
        self.assertIn("ai_model_get_output_info", main_text)
        self.assertIn("ai_infer_sync", main_text)
        self.assertIn("static_assert", main_text)
        self.assertIn("add_subdirectory", cmake_text)
        self.assertIn("be_u1000_ai_micro_demo/generated", cmake_text)
        self.assertIn("rv_aios_cxx", cmake_text)
        self.assertIn("rv_aios_models", cmake_text)
        self.assertIn("check_be_u1000_ai_demo_size.py", cmake_text)

    def test_codegen_targets_be_u1000_single_float_abi(self):
        config = yaml.safe_load(
            (PROJECT_ROOT / "ai_models_be_u1000_ai_micro.yaml").read_text()
        )
        codegen_text = (PROJECT_ROOT / "scripts" / "ai_codegen.py").read_text()
        defaults = config["defaults"]

        self.assertEqual(defaults["target_arch"], "rv32")
        self.assertEqual(defaults["target_abi"], "ilp32f")
        self.assertIn("+f", defaults["cpu_features"])
        self.assertIn("+a", defaults["cpu_features"])
        self.assertNotIn("+c", defaults["cpu_features"])
        self.assertNotIn("+d", defaults["cpu_features"])
        self.assertIs(defaults["enable_llvmcpu_microkernels"], False)
        self.assertIs(defaults["enable_data_tiling"], False)
        self.assertEqual(config["toolchain"]["tflite_toolchain_env"], "iree-toolchain310")
        self.assertIn("tflite_toolchain_env", codegen_text)

    def test_be_u1000_startup_avoids_board_config_macro_equ_names(self):
        startup_text = (PROJECT_ROOT / "boards" / "be_u1000" / "startup_be_u1000.S").read_text()

        self.assertNotIn(".equ CSR_MTVT", startup_text)
        self.assertNotIn(".equ CSR_NMITVEC", startup_text)
        self.assertNotIn(".equ MTVEC_MODE_CLIC", startup_text)
        self.assertIn("BEU1000_ASM_CSR_MTVT", startup_text)
        self.assertIn("BEU1000_ASM_MTVEC_MODE_CLIC", startup_text)

    def test_size_checker_accepts_under_limit_and_rejects_over_limit(self):
        script = PROJECT_ROOT / "scripts" / "check_be_u1000_ai_demo_size.py"

        with tempfile.TemporaryDirectory() as tmpdir:
            tmpdir_path = Path(tmpdir)
            small_bin = tmpdir_path / "small.bin"
            large_bin = tmpdir_path / "large.bin"
            small_bin.write_bytes(b"\0" * 16)
            large_bin.write_bytes(b"\0" * 17)

            ok = subprocess.run(
                [
                    "python3",
                    str(script),
                    "--binary",
                    str(small_bin),
                    "--max-bytes",
                    "16",
                ],
                cwd=PROJECT_ROOT,
                check=False,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
            )
            fail = subprocess.run(
                [
                    "python3",
                    str(script),
                    "--binary",
                    str(large_bin),
                    "--max-bytes",
                    "16",
                ],
                cwd=PROJECT_ROOT,
                check=False,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
            )

        self.assertEqual(ok.returncode, 0, ok.stdout)
        self.assertIn("BE_U1000_AI_DEMO_SIZE_PASS", ok.stdout)
        self.assertNotEqual(fail.returncode, 0, fail.stdout)
        self.assertIn("BE_U1000_AI_DEMO_SIZE_FAIL", fail.stdout)

    def test_be_u1000_linkers_copy_got_with_initialized_data(self):
        for linker_name, memory_region in (
            ("link_be_u1000_flash.ld", "TCMB"),
            ("link_be_u1000_tcm.ld", "TCMA"),
        ):
            with self.subTest(linker=linker_name):
                linker_text = (PROJECT_ROOT / "boards" / "be_u1000" / linker_name).read_text()
                data_match = re.search(
                    rf"\.data\s*:.*?\{{(?P<body>.*?)\n\s*\}}\s*>\s*{memory_region}",
                    linker_text,
                    re.S,
                )

                self.assertIsNotNone(data_match)
                data_body = data_match.group("body")
                self.assertIn("*(.got)", data_body)
                self.assertIn("*(.got.plt)", data_body)
                self.assertLess(data_body.index("*(.got)"), data_body.index("_data_end"))
                self.assertNotRegex(linker_text[data_match.end():], r"\.got\s*:")


if __name__ == "__main__":
    unittest.main()
