#!/usr/bin/env python3

from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent


def fail(message: str) -> None:
    print(f"ERROR: {message}")
    sys.exit(1)


def require(text: str, needle: str, context: str) -> None:
    if needle not in text:
        fail(f"missing '{needle}' in {context}")


def forbid(text: str, needle: str, context: str) -> None:
    if needle in text:
        fail(f"unexpected '{needle}' in {context}")


def main() -> None:
    root_cmake = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    board_cmake = (REPO_ROOT / "boards/be_u1000/CMakeLists.txt").read_text(
        encoding="utf-8"
    )
    os_config = (REPO_ROOT / "config/os_config.h").read_text(encoding="utf-8")
    os_sched = (REPO_ROOT / "kernel/src/os_sched.c").read_text(encoding="utf-8")
    trap = (REPO_ROOT / "arch/riscv/src/trap.c").read_text(encoding="utf-8")
    ai_registry = (REPO_ROOT / "ai/src/ai_model_registry.c").read_text(encoding="utf-8")
    be_u1000_matrix = (
        REPO_ROOT / "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md"
    ).read_text(encoding="utf-8")

    require(root_cmake, "set(OS_CFG_CPU_MAX_VALUE 3)", "CMakeLists.txt")
    require(root_cmake, "if(OS_SMP_EN)", "CMakeLists.txt")
    require(root_cmake, "set(OS_CFG_CPU_COUNT_VALUE 2)", "CMakeLists.txt")
    require(root_cmake, "set(OS_CFG_CPU_COUNT_VALUE 1)", "CMakeLists.txt")
    require(root_cmake, "project(rv_aios", "CMakeLists.txt")
    require(root_cmake, "set(OS_SELECTED_LANE_DEFINITIONS", "CMakeLists.txt")
    require(
        root_cmake, 'option(OS_FL_EN "Enable Federated Learning" OFF)', "CMakeLists.txt"
    )
    require(
        root_cmake,
        "BOARD_LINKER_SCRIPT must be exported by boards/be_u1000/CMakeLists.txt",
        "CMakeLists.txt",
    )
    require(root_cmake, "OS_CFG_CPU_COUNT=${OS_CFG_CPU_COUNT_VALUE}", "CMakeLists.txt")
    forbid(root_cmake, "CONFIG_IRQ_MODEL_CLIC", "CMakeLists.txt")

    require(
        board_cmake,
        "This file owns board-local sources and linker / memory-model selection only.",
        "boards/be_u1000/CMakeLists.txt",
    )
    forbid(board_cmake, "CONFIG_NUM_CORES", "boards/be_u1000/CMakeLists.txt")
    forbid(board_cmake, "CONFIG_TIMER_FREQ", "boards/be_u1000/CMakeLists.txt")
    forbid(board_cmake, "BOARD_DEFINITIONS", "boards/be_u1000/CMakeLists.txt")

    require(
        os_config,
        "#define OS_CFG_AI_EN                0",
        "config/os_config.h",
    )
    require(
        os_config,
        "#define OS_CFG_FL_EN                0",
        "config/os_config.h",
    )
    require(
        os_config,
        '#error "OS_CFG_CPU_COUNT must be at least 1"',
        "config/os_config.h",
    )
    require(
        os_config,
        '#error "OS_CFG_CPU_COUNT must not exceed OS_CFG_CPU_MAX"',
        "config/os_config.h",
    )
    require(
        os_config,
        '#error "OS_CFG_CPU_COUNT must be 1 when OS_CFG_SMP_EN is disabled"',
        "config/os_config.h",
    )
    require(
        os_config,
        '#error "OS_CFG_CPU_MAX must not exceed BE_U1000_NUM_CORES"',
        "config/os_config.h",
    )
    require(
        os_config,
        '#error "OS_CFG_CPU_COUNT must not exceed BE_U1000_NUM_MAIN_CORES"',
        "config/os_config.h",
    )

    board_config = (REPO_ROOT / "boards/be_u1000/board_config.h").read_text(
        encoding="utf-8"
    )
    require(
        board_config,
        "This header is the single source of truth for immutable board facts only.",
        "boards/be_u1000/board_config.h",
    )
    require(
        board_config,
        "Build-selected lane policy such as OS_CFG_CPU_COUNT, OS_CFG_SMP_EN, and",
        "boards/be_u1000/board_config.h",
    )

    require(
        os_sched, "#define OS_SCHED_CPU_SLOTS OS_CFG_CPU_COUNT", "kernel/src/os_sched.c"
    )
    require(
        os_sched, "OS_CFG_TIMER_FREQ_HZ / OS_CFG_TICK_FREQ_HZ", "kernel/src/os_sched.c"
    )
    require(os_sched, "return OS_CFG_CPU_COUNT;", "kernel/src/os_sched.c")
    require(
        os_sched,
        "if (affinity != OS_CPU_ANY && affinity >= OS_CFG_CPU_COUNT)",
        "kernel/src/os_sched.c",
    )
    forbid(
        os_sched, "OS_CFG_CPU_FREQ_HZ / OS_CFG_TICK_FREQ_HZ", "kernel/src/os_sched.c"
    )
    require(trap, "OS_CFG_TIMER_FREQ_HZ / OS_CFG_TICK_FREQ_HZ", "arch/riscv/src/trap.c")
    forbid(trap, "OS_CFG_CPU_FREQ_HZ / OS_CFG_TICK_FREQ_HZ", "arch/riscv/src/trap.c")
    require(ai_registry, "OS_CFG_TIMER_FREQ_HZ", "ai/src/ai_model_registry.c")
    forbid(ai_registry, "OS_CFG_CPU_FREQ_HZ", "ai/src/ai_model_registry.c")

    require(
        be_u1000_matrix,
        "`OS_CFG_CPU_COUNT=1` with `OS_SMP_EN=OFF`",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )

    print("Config authority checks passed.")


if __name__ == "__main__":
    main()
