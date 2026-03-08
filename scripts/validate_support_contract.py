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
    supported_matrix = (REPO_ROOT / "docs/SUPPORTED_MATRIX.md").read_text(
        encoding="utf-8"
    )
    be_u1000_matrix = (
        REPO_ROOT / "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md"
    ).read_text(encoding="utf-8")
    pixi = (REPO_ROOT / "pixi.toml").read_text(encoding="utf-8")
    workflow = (
        REPO_ROOT / ".github/workflows/firmware-supported-matrix.yml"
    ).read_text(encoding="utf-8")

    require(
        supported_matrix,
        "This file is the normative support-status registry for the repository.",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "pixi run -e rv32 validate-supported-rv32",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "pixi run -e be-u1000 validate-supported",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "pixi run -e rv32 validate-mnist-runtime-dual",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "Optional observation or comparison lanes do not promote a path to supported status.",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "Legacy AI API: `ai/include/ai_runtime.h`",
        "docs/SUPPORTED_MATRIX.md",
    )

    require(
        be_u1000_matrix,
        "`S0` | Single-core fallback behavior",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )
    require(
        be_u1000_matrix,
        "`S1` | Experimental SMP build wiring closes successfully",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )
    require(
        be_u1000_matrix,
        "`S0` is the supported fallback behavior: both demo tasks share a single scheduler lane (`OS_CFG_CPU_COUNT=1` with `OS_SMP_EN=OFF`)",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )
    require(
        be_u1000_matrix,
        "Promotion of any later stage requires the support registry, command surface, and blocking CI to be updated in the same change.",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )

    require(
        pixi,
        'validate-support-contract = "python scripts/validate_support_contract.py"',
        "pixi.toml",
    )
    require(
        pixi,
        'validate-supported-rv32 = { depends-on = ["validate-config-authority", "validate-bsp-seam", "validate-support-contract", "validate-supported-ai", "configure", "build", "report-footprint", "validate-mnist-runtime"] }',
        "pixi.toml",
    )
    require(
        pixi,
        'validate-supported = { depends-on = ["validate-config-authority", "validate-bsp-seam", "validate-support-contract", "configure", "build", "report-footprint", "validate-irq-map", "validate-selftest-sim"] }',
        "pixi.toml",
    )
    require(
        pixi,
        'validate-mnist-runtime-dual = { depends-on = ["validate-mnist-runtime", "observe-mnist-runtime-renode", "compare-mnist-runtime-platforms"] }',
        "pixi.toml",
    )
    require(
        pixi,
        'validate-runtime-stages-experimental = { depends-on = ["probe-smp-build", "validate-selftest-sim", "validate-smp-affinity-experimental", "validate-smp-balance-experimental"] }',
        "pixi.toml",
    )

    require(
        workflow,
        '      - "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md"',
        ".github/workflows/firmware-supported-matrix.yml",
    )
    require(
        workflow,
        '      - "scripts/validate_support_contract.py"',
        ".github/workflows/firmware-supported-matrix.yml",
    )
    require(
        workflow,
        '      - "scripts/validate_bsp_seam.py"',
        ".github/workflows/firmware-supported-matrix.yml",
    )
    require(
        workflow,
        "# Blocking CI for supported lanes only. Observation and experimental lanes",
        ".github/workflows/firmware-supported-matrix.yml",
    )
    require(
        workflow,
        "run: pixi run -e rv32 validate-supported-rv32",
        ".github/workflows/firmware-supported-matrix.yml",
    )
    require(
        workflow,
        "run: pixi run -e be-u1000 validate-supported",
        ".github/workflows/firmware-supported-matrix.yml",
    )
    forbid(
        workflow,
        "validate-runtime-stages-experimental",
        ".github/workflows/firmware-supported-matrix.yml",
    )
    forbid(
        workflow,
        "validate-smp-affinity-experimental",
        ".github/workflows/firmware-supported-matrix.yml",
    )
    forbid(
        workflow,
        "validate-smp-balance-experimental",
        ".github/workflows/firmware-supported-matrix.yml",
    )

    print("Support contract checks passed.")


if __name__ == "__main__":
    main()
