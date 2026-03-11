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
    readme = (REPO_ROOT / "README.md").read_text(encoding="utf-8")
    supported_matrix = (REPO_ROOT / "docs/SUPPORTED_MATRIX.md").read_text(
        encoding="utf-8"
    )
    be_u1000_matrix = (
        REPO_ROOT / "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md"
    ).read_text(encoding="utf-8")
    switching_guide = (REPO_ROOT / "docs/switching_guide.md").read_text(
        encoding="utf-8"
    )
    pixi = (REPO_ROOT / "pixi.toml").read_text(encoding="utf-8")
    workflow = (
        REPO_ROOT / ".github/workflows/firmware-supported-matrix.yml"
    ).read_text(encoding="utf-8")

    require(
        readme,
        "`qemu_virt` + `apps/mnist_app` + `ai/include/ai_model_registry.h`",
        "README.md",
    )
    require(
        readme,
        "`be_u1000` + `apps/be_u1000_demo` (current EVU-BA-2.3-shaped board path)",
        "README.md",
    )
    require(
        readme,
        "Optional observation lanes do not promote a path to supported status.",
        "README.md",
    )
    require(
        readme,
        "The default pixi environment targets the supported `rv32` lane.",
        "README.md",
    )

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
        "pixi run -e rv32 observe-mnist-runtime-renode",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "pixi run -e rv32 compare-mnist-runtime-platforms",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "current EVU-BA-2.3-shaped board path",
        "docs/SUPPORTED_MATRIX.md",
    )
    require(
        supported_matrix,
        "Proof path: `apps/mnist_app/src/validation_main.c` uses the registry contract directly; generated wrappers remain convenience-only glue",
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
        "`S1` | Historical SMP build wiring note",
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
        be_u1000_matrix,
        "maintained pixi task removed",
        "docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md",
    )

    require(
        pixi,
        "# Canonical AI contract check; the full supported AI gate remains validate-supported-rv32.",
        "pixi.toml",
    )
    require(
        pixi,
        'validate-support-contract = "python scripts/validate_support_contract.py"',
        "pixi.toml",
    )
    require(
        pixi,
        'default = ["rv32"]',
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
    forbid(pixi, 'build-picolibc = "pixi build"', "pixi.toml")
    forbid(
        pixi, 'rebuild = { depends-on = ["clean", "configure", "build"] }', "pixi.toml"
    )
    forbid(
        pixi,
        'validate-mnist-runtime-dual = { depends-on = ["validate-mnist-runtime", "observe-mnist-runtime-renode", "compare-mnist-runtime-platforms"] }',
        "pixi.toml",
    )
    forbid(pixi, "validate-irq-map-sdk = ", "pixi.toml")
    forbid(pixi, "validate-bootlog = ", "pixi.toml")
    forbid(pixi, "[feature.rv64.dependencies]", "pixi.toml")
    forbid(pixi, "[feature.rv64.tasks]", "pixi.toml")
    forbid(pixi, 'rv64 = ["rv64"]', "pixi.toml")
    forbid(pixi, "sim-ai = ", "pixi.toml")
    forbid(pixi, "sim-renode-ai = ", "pixi.toml")
    forbid(pixi, "probe-smp-build = ", "pixi.toml")
    forbid(pixi, "run-smp-runtime = ", "pixi.toml")
    forbid(pixi, "check-smp-bootlog = ", "pixi.toml")
    forbid(pixi, "validate-smp-runtime = ", "pixi.toml")
    forbid(pixi, "validate-smp-affinity-experimental = ", "pixi.toml")
    forbid(pixi, "validate-smp-balance-experimental = ", "pixi.toml")
    forbid(pixi, "validate-runtime-stages-experimental = ", "pixi.toml")
    forbid(pixi, "sim-be_u1000 = ", "pixi.toml")

    require(
        switching_guide,
        "pixi run -e rv32 validate-supported-rv32",
        "docs/switching_guide.md",
    )
    require(
        switching_guide,
        "pixi run -e rv32 observe-mnist-runtime-renode",
        "docs/switching_guide.md",
    )
    require(
        switching_guide,
        "pixi run -e rv32 compare-mnist-runtime-platforms",
        "docs/switching_guide.md",
    )
    require(
        switching_guide,
        "Keep the maintained AI task surface small.",
        "docs/switching_guide.md",
    )
    forbid(switching_guide, "validate-mnist-runtime-dual", "docs/switching_guide.md")

    require(
        workflow,
        '      - "README.md"',
        ".github/workflows/firmware-supported-matrix.yml",
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
