import os
import tempfile
import unittest
from pathlib import Path
from unittest import mock

from scripts import ai_codegen


PROJECT_ROOT = Path(__file__).resolve().parents[1]


def write_fake_tool(directory: Path, name: str) -> Path:
    path = directory / name
    path.write_text("#!/bin/sh\nexit 0\n", encoding="utf-8")
    path.chmod(0o755)
    return path


class IreeLocalCompilerToolchainTest(unittest.TestCase):
    def setUp(self):
        ai_codegen.TOOLCHECK_CACHE.clear()

    def test_compiler_root_env_overrides_compile_and_opt_only(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            compiler_root = root / "local_compiler"
            path_root = root / "path_tools"
            compiler_root.mkdir()
            path_root.mkdir()

            local_compile = write_fake_tool(compiler_root, "iree-compile")
            local_opt = write_fake_tool(compiler_root, "iree-opt")
            path_import = write_fake_tool(path_root, "iree-import-tflite")
            write_fake_tool(path_root, "iree-compile")
            write_fake_tool(path_root, "iree-opt")

            config = {"toolchain": {"compiler_root_env": "IREE_LOCAL_COMPILER_ROOT"}}
            env = {
                "IREE_LOCAL_COMPILER_ROOT": str(compiler_root),
                "PATH": str(path_root),
            }

            with mock.patch.dict(os.environ, env, clear=True):
                tools = ai_codegen.resolve_iree_tools(
                    config,
                    ["iree-import-tflite", "iree-opt", "iree-compile"],
                )

        self.assertEqual(tools["iree-compile"], str(local_compile))
        self.assertEqual(tools["iree-opt"], str(local_opt))
        self.assertTrue(tools["iree-import-tflite"].endswith("/iree-import-tflite"))
        self.assertNotEqual(tools["iree-import-tflite"], str(compiler_root / "iree-import-tflite"))

    def test_missing_compiler_root_tool_fails_loudly(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            compiler_root = root / "local_compiler"
            compiler_root.mkdir()
            write_fake_tool(compiler_root, "iree-compile")

            config = {"toolchain": {"compiler_root_env": "IREE_LOCAL_COMPILER_ROOT"}}

            with mock.patch.dict(
                os.environ,
                {"IREE_LOCAL_COMPILER_ROOT": str(compiler_root), "PATH": ""},
                clear=True,
            ):
                with self.assertRaises(SystemExit):
                    ai_codegen.resolve_iree_tools(config, ["iree-opt", "iree-compile"])

    def test_local_compiler_build_task_is_declared(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text(encoding="utf-8")
        script = PROJECT_ROOT / "scripts" / "build_local_iree_compiler.sh"

        self.assertTrue(script.exists())
        script_text = script.read_text(encoding="utf-8")

        self.assertIn("build-local-iree-compiler", pixi_text)
        self.assertIn("third_party/llvm-project", script_text)
        self.assertIn("IREE_LOCAL_COMPILER_CC=\"${IREE_LOCAL_COMPILER_CC:-/usr/bin/clang}\"", script_text)
        self.assertIn("IREE_LOCAL_COMPILER_CXX=\"${IREE_LOCAL_COMPILER_CXX:-/usr/bin/clang++}\"", script_text)
        self.assertIn("IREE_LOCAL_COMPILER_CPU_TARGETS=\"${IREE_LOCAL_COMPILER_CPU_TARGETS:-X86;RISCV}\"", script_text)
        self.assertIn(
            'IREE_LOCAL_COMPILER_RISCV32_UKERNEL_ABI="${IREE_LOCAL_COMPILER_RISCV32_UKERNEL_ABI:-ilp32f}"',
            script_text,
        )
        self.assertIn(
            'IREE_LOCAL_COMPILER_RISCV32_UKERNEL_MARCH="${IREE_LOCAL_COMPILER_RISCV32_UKERNEL_MARCH:-rv32imafc_zifencei_zicsr}"',
            script_text,
        )
        self.assertIn("CMAKE_C_COMPILER:[^=]+=", script_text)
        self.assertIn("-DCMAKE_C_COMPILER=", script_text)
        self.assertIn("-DCMAKE_CXX_COMPILER=", script_text)
        self.assertIn("-DIREE_UKERNEL_BITCODE_RISCV32_ABI=", script_text)
        self.assertIn("-DIREE_UKERNEL_BITCODE_RISCV32_MARCH=", script_text)
        self.assertIn("-DIREE_BUILD_COMPILER=ON", script_text)
        self.assertIn("-DIREE_ENABLE_WERROR_FLAG=OFF", script_text)
        self.assertIn("-DIREE_DEFAULT_CPU_LLVM_TARGETS=", script_text)
        self.assertIn("-DCLANG_ENABLE_ARCMT=OFF", script_text)
        self.assertIn("-DCLANG_ENABLE_STATIC_ANALYZER=OFF", script_text)
        self.assertIn("-DIREE_HAL_DRIVER_DEFAULTS=OFF", script_text)
        self.assertIn("-DIREE_HAL_DRIVER_VULKAN=OFF", script_text)
        self.assertIn("-DIREE_TARGET_BACKEND_DEFAULTS=OFF", script_text)
        self.assertIn("-DIREE_TARGET_BACKEND_LLVM_CPU=ON", script_text)
        self.assertIn("-DIREE_TARGET_BACKEND_VMVX=ON", script_text)
        self.assertIn("-DIREE_INPUT_TOSA=ON", script_text)
        self.assertIn("--target iree-compile iree-opt", script_text)

    def test_iree_riscv_ukernel_bitcode_abi_is_configurable(self):
        helper = (
            PROJECT_ROOT
            / "third_party"
            / "iree"
            / "build_tools"
            / "cmake"
            / "iree_bitcode_library.cmake"
        )
        helper_text = helper.read_text(encoding="utf-8")

        self.assertIn('set(IREE_UKERNEL_BITCODE_RISCV32_ABI "ilp32d" CACHE STRING', helper_text)
        self.assertIn('set(IREE_UKERNEL_BITCODE_RISCV32_MARCH "" CACHE STRING', helper_text)
        self.assertIn('set(IREE_UKERNEL_BITCODE_RISCV64_ABI "lp64d" CACHE STRING', helper_text)
        self.assertIn('set(IREE_UKERNEL_BITCODE_RISCV64_MARCH "" CACHE STRING', helper_text)
        self.assertIn('-march=${IREE_UKERNEL_BITCODE_RISCV32_MARCH}', helper_text)
        self.assertIn('-mabi=${IREE_UKERNEL_BITCODE_RISCV32_ABI}', helper_text)
        self.assertIn('-march=${IREE_UKERNEL_BITCODE_RISCV64_MARCH}', helper_text)
        self.assertIn('-mabi=${IREE_UKERNEL_BITCODE_RISCV64_ABI}', helper_text)
        self.assertNotIn('list(APPEND _COPTS "-mabi=ilp32d")', helper_text)
        self.assertNotIn('list(APPEND _COPTS "-mabi=lp64d")', helper_text)


if __name__ == "__main__":
    unittest.main()
