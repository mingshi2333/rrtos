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

    def test_cmake_exposes_static_direct_probe_lane(self):
        cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()

        self.assertIn("ai_static_direct_probe", cmake_text)
        self.assertIn('BE_U1000_APP STREQUAL "ai_static_direct_probe"', cmake_text)
        self.assertIn("apps/be_u1000_ai_static_direct_probe", cmake_text)
        self.assertNotIn("ai_static_direct_probe requires OS_AI_EN=ON", cmake_text)

    def test_cmake_exposes_static_direct_mnist_probe_lane(self):
        cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()

        self.assertIn("ai_static_direct_mnist_probe", cmake_text)
        self.assertIn(
            'BE_U1000_APP STREQUAL "ai_static_direct_mnist_probe"', cmake_text
        )
        self.assertIn("apps/be_u1000_ai_static_direct_mnist_probe", cmake_text)
        self.assertNotIn(
            "ai_static_direct_mnist_probe requires OS_AI_EN=ON", cmake_text
        )

    def test_pixi_exposes_static_direct_probe_tasks(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("configure-ai-static-direct", pixi_text)
        self.assertIn("build-ai-static-direct", pixi_text)
        self.assertIn("validate-ai-static-direct-map", pixi_text)
        self.assertIn("validate-ai-static-direct-renode", pixi_text)
        self.assertIn("validate-ai-static-direct", pixi_text)
        self.assertIn("check_no_iree_vm_symbols.py", pixi_text)
        self.assertIn("rrtos_be_u1000.map", pixi_text)
        self.assertIn("-DOS_AI_EN=OFF", pixi_text)
        self.assertIn("-DRRTOS_CXX_EN=OFF", pixi_text)
        self.assertIn("-DBE_U1000_APP=ai_static_direct_probe", pixi_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_PROBE_PASS", pixi_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_INFER_PASS", pixi_text)

    def test_pixi_exposes_static_direct_mnist_probe_tasks(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("configure-ai-static-direct-mnist", pixi_text)
        self.assertIn("build-ai-static-direct-mnist", pixi_text)
        self.assertIn("validate-ai-static-direct-mnist-map", pixi_text)
        self.assertIn("validate-ai-static-direct-mnist-renode", pixi_text)
        self.assertIn("validate-ai-static-direct-mnist", pixi_text)
        self.assertIn("build-be_u1000_ai_static_direct_mnist_probe", pixi_text)
        self.assertIn("-DOS_AI_EN=OFF", pixi_text)
        self.assertIn("-DRRTOS_CXX_EN=OFF", pixi_text)
        self.assertIn("-DBE_U1000_APP=ai_static_direct_mnist_probe", pixi_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_MNIST_PROBE_PASS", pixi_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_MNIST_INFER_PASS", pixi_text)

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
        self.assertIn("BE_U1000_AI_MODELS_DIR", app_cmake_text)
        self.assertIn('"${CMAKE_CURRENT_SOURCE_DIR}/generated"', app_cmake_text)
        self.assertIn('add_subdirectory("${BE_U1000_AI_MODELS_DIR}" generated)', app_cmake_text)
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
        self.assertIn("MODEL_PEAK", main_text)
        self.assertIn("os_heap_stats_get", main_text)
        self.assertIn("os_heap_stats_reset_peak", main_text)
        self.assertIn("phase=init", main_text)
        self.assertIn("phase=invoke", main_text)
        self.assertIn("static_assert", main_text)
        self.assertIn("add_subdirectory", cmake_text)
        self.assertIn("BE_U1000_AI_MODELS_DIR", cmake_text)
        self.assertIn("be_u1000_ai_micro_demo/generated", cmake_text)
        self.assertIn("${BE_U1000_AI_MODELS_DIR}", cmake_text)
        self.assertIn("rv_aios_cxx", cmake_text)
        self.assertIn("rv_aios_models", cmake_text)
        self.assertIn("check_be_u1000_ai_demo_size.py", cmake_text)

    def test_static_direct_probe_links_raw_static_object_without_vm_runtime(self):
        app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_static_direct_probe"
        main_text = (app_dir / "main.c").read_text()
        cmake_text = (app_dir / "CMakeLists.txt").read_text()

        self.assertIn("be_u1000_hello_world_float_linked_library_query", main_text)
        self.assertIn("IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST", main_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_INFER_PASS", main_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_PROBE_PASS", main_text)
        self.assertIn("iree/hal/local/executable_library.h", main_text)
        self.assertIn("be_u1000_hello_world_float.o", cmake_text)
        self.assertIn("model_const.c", cmake_text)
        self.assertNotIn("rv_aios_ai", cmake_text)
        self.assertNotIn("rv_aios_models", cmake_text)
        self.assertNotIn("iree_vm_context_run_function", main_text)

    def test_static_direct_probe_reuses_generated_weights_without_model_runtime(self):
        const_text = (
            PROJECT_ROOT / "apps" / "be_u1000_ai_static_direct_probe" / "model_const.c"
        ).read_text()

        self.assertIn("EMITC_IMPLEMENTATION", const_text)
        self.assertIn('generated/be_u1000_hello_world_float.h"', const_text)
        self.assertIn("be_u1000_hello_world_float__const", const_text)
        self.assertIn("be_u1000_hello_world_float_direct_const_data", const_text)

    def test_static_direct_mnist_probe_links_raw_static_object_without_vm_runtime(self):
        app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_static_direct_mnist_probe"
        main_text = (app_dir / "main.c").read_text()
        cmake_text = (app_dir / "CMakeLists.txt").read_text()

        self.assertIn("st_mnist_28_linked_library_query", main_text)
        self.assertIn("IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST", main_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_MNIST_INFER_PASS", main_text)
        self.assertIn("BE_U1000_STATIC_DIRECT_MNIST_PROBE_PASS", main_text)
        self.assertIn("iree/hal/local/executable_library.h", main_text)
        self.assertIn("st_mnist_28.o", cmake_text)
        self.assertIn("model_const.c", cmake_text)
        self.assertNotIn("rv_aios_ai", cmake_text)
        self.assertNotIn("rv_aios_models", cmake_text)
        self.assertNotIn("iree_vm_context_run_function", main_text)

    def test_static_direct_mnist_probe_reuses_generated_weights_and_samples(self):
        app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_static_direct_mnist_probe"
        main_text = (app_dir / "main.c").read_text()
        const_text = (app_dir / "model_const.c").read_text()

        self.assertIn("mnist_validation_samples.h", main_text)
        self.assertIn("k_mnist_validation_samples", main_text)
        self.assertIn("MNIST_DIRECT_CONST_BYTES", main_text)
        self.assertIn("MNIST_DIRECT_SCRATCH_BYTES", main_text)
        self.assertIn("EMITC_IMPLEMENTATION", const_text)
        self.assertIn('generated/st_mnist_28.h"', const_text)
        self.assertIn("st_mnist_28__const", const_text)
        self.assertIn("st_mnist_28_direct_const_data", const_text)

    def test_static_direct_probe_doc_captures_reusable_design(self):
        doc_text = (PROJECT_ROOT / "docs" / "AI_IREE_STATIC_DIRECT_PROBE.md").read_text()

        self.assertIn("Implementation Walkthrough", doc_text)
        self.assertIn("Reusable Static Direct Design", doc_text)
        self.assertIn("Reuse Decision Table", doc_text)
        self.assertIn("Generator Contract", doc_text)
        self.assertIn("Porting Checklist", doc_text)
        self.assertIn("assets/iree-static-direct-architecture.svg", doc_text)
        self.assertIn("apps/be_u1000_ai_static_direct_probe/main.c", doc_text)
        self.assertIn("probe_issue_dispatch", doc_text)
        self.assertIn("iree_hal_executable_dispatch_state_v0_t", doc_text)
        self.assertIn("scripts/check_no_iree_vm_symbols.py", doc_text)
        self.assertIn("validate-ai-static-direct-map", doc_text)
        self.assertIn("validate-ai-static-direct-renode", doc_text)

    def test_static_direct_probe_architecture_svg_exists(self):
        svg_text = (
            PROJECT_ROOT / "docs" / "assets" / "iree-static-direct-architecture.svg"
        ).read_text()

        self.assertIn("IREE Static Direct Architecture", svg_text)
        self.assertIn("Normal IREE static-library runtime path", svg_text)
        self.assertIn("Current BE-U1000 static direct probe", svg_text)
        self.assertIn("Reusable generated static direct design", svg_text)
        self.assertIn("Bypassed: registry, IREE VM, HAL module, static loader", svg_text)
        self.assertIn("ai_codegen.py", svg_text)

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
        self.assertIn("model_library_name", codegen_text)
        self.assertIn('config.get("model_library_name", "rv_aios_models")', codegen_text)
        self.assertIn('"i8": {"c": "int8_t", "dtype": "AI_DTYPE_INT8"}', codegen_text)
        self.assertIn("--dump-compile-phases-to", codegen_text)
        self.assertIn("--dump-compilation-phases-to=", codegen_text)
        self.assertIn("--iree-hal-dump-executable-intermediates-to=", codegen_text)
        self.assertIn("extra_iree_compile_flags", codegen_text)
        self.assertIn("collect_extra_iree_compile_flags", codegen_text)
        self.assertIn("explicit_output_dir = config.get(\"output_dir\")", codegen_text)

    def test_be_u1000_ukernel_codegen_lane_is_explicit(self):
        config = yaml.safe_load(
            (PROJECT_ROOT / "ai_models_be_u1000_ai_micro_ukernel.yaml").read_text()
        )
        defaults = config["defaults"]

        self.assertEqual(config["app_name"], "be_u1000_ai_micro_demo")
        self.assertEqual(
            config["output_dir"],
            "apps/be_u1000_ai_micro_demo/generated_ukernel",
        )
        self.assertEqual(config["toolchain"]["compiler_root_env"], "IREE_LOCAL_COMPILER_ROOT")
        self.assertEqual(defaults["target_abi"], "ilp32f")
        self.assertIn("+f", defaults["cpu_features"])
        self.assertNotIn("+d", defaults["cpu_features"])
        self.assertIs(defaults["enable_llvmcpu_microkernels"], True)
        self.assertIs(defaults["enable_data_tiling"], False)

    def test_be_u1000_vmvx_inline_codegen_lane_is_explicit(self):
        config = yaml.safe_load(
            (
                PROJECT_ROOT / "ai_models_be_u1000_ai_micro_vmvx_inline.yaml"
            ).read_text()
        )
        defaults = config["defaults"]

        self.assertEqual(config["app_name"], "be_u1000_ai_micro_demo")
        self.assertEqual(
            config["output_dir"],
            "apps/be_u1000_ai_micro_demo/generated_vmvx_inline",
        )
        self.assertEqual(defaults["iree_backend"], "vmvx_inline")
        self.assertEqual(defaults["target_abi"], "ilp32f")
        self.assertIs(defaults["enable_llvmcpu_microkernels"], False)
        self.assertIs(defaults["enable_data_tiling"], False)

    def test_vmvx_inline_codegen_preserves_same_shape_constants(self):
        codegen_text = (PROJECT_ROOT / "scripts" / "ai_codegen.py").read_text()

        self.assertIn('backend == "vmvx_inline"', codegen_text)
        self.assertIn("--iree-execution-model=inline-static", codegen_text)
        self.assertIn("--iree-hal-target-backends=vmvx-inline", codegen_text)
        self.assertIn("--iree-flow-inline-constants-max-byte-length=0", codegen_text)

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
