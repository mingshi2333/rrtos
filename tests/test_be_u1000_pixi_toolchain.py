import unittest
from pathlib import Path

import yaml


PROJECT_ROOT = Path(__file__).resolve().parents[1]


class BeU1000PixiToolchainTest(unittest.TestCase):
    def test_rv32_picolibc_package_is_single_float_for_be_u1000(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()
        arch_text = (PROJECT_ROOT / "cmake" / "riscv_arch.cmake").read_text()
        root_cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()
        toolchain_text = (PROJECT_ROOT / "cmake" / "riscv32-pixi.cmake").read_text()
        ai_config = yaml.safe_load((PROJECT_ROOT / "ai_models.yaml").read_text())
        rv32_recipe = yaml.safe_load(
            (PROJECT_ROOT / "packages" / "picolibc-rv32" / "recipe.yaml").read_text()
        )
        rv32_recipe_text = (
            PROJECT_ROOT / "packages" / "picolibc-rv32" / "recipe.yaml"
        ).read_text()

        self.assertIn('picolibc-rv32 = { path = "packages/picolibc-rv32" }', pixi_text)
        self.assertNotIn("picolibc-rv32f", pixi_text)
        self.assertEqual(rv32_recipe["context"]["march"], "rv32imafc_zifencei")
        self.assertEqual(rv32_recipe["context"]["mabi"], "ilp32f")
        self.assertEqual(ai_config["defaults"]["target_abi"], "ilp32f")
        self.assertIs(ai_config["defaults"]["enable_llvmcpu_microkernels"], False)
        self.assertIs(ai_config["defaults"]["enable_data_tiling"], False)
        self.assertIs(ai_config["defaults"]["enable_stream_memory_flags"], False)
        self.assertIn('set(RISCV_MABI "ilp32f" CACHE STRING "RISC-V mabi")', arch_text)
        self.assertIn('RISCV_MABI STREQUAL "ilp32f"', root_cmake_text)
        self.assertIn("-DRISCV_MABI=ilp32f", pixi_text)
        self.assertEqual(rv32_recipe["package"]["name"], "picolibc-${{ arch }}")
        self.assertIn("libgcc_compat.c", rv32_recipe_text)
        self.assertIn("lib/libgcc.a", rv32_recipe_text)
        self.assertIn("${PICOLIBC_ROOT}/lib/libgcc.a", toolchain_text)
        self.assertIn('set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT}" CACHE STRING "Executable linker flags" FORCE)', toolchain_text)
        self.assertNotIn("ilp32 multilib fallback", toolchain_text)


if __name__ == "__main__":
    unittest.main()
