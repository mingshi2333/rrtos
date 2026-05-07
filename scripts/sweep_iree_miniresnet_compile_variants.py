#!/usr/bin/env python3
"""Compile MiniResNet with selected IREE flag variants and summarize peak slabs."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
import shutil
import subprocess
import sys

import yaml

try:
    from scripts import analyze_iree_compile_dump as analyzer
except ModuleNotFoundError:
    import analyze_iree_compile_dump as analyzer


PROJECT_ROOT = Path(__file__).resolve().parent.parent
MODEL_NAME = "miniresnetv1_s1_64x50_tl_int8"
MODEL_PATH = PROJECT_ROOT / "models" / "stm32ai_zoo" / f"{MODEL_NAME}.tflite"
DEFAULT_OUTPUT_ROOT = PROJECT_ROOT / "build" / "iree_miniresnet_variants"
NEEDLE_ALLOCATION_SIZE = 315264


@dataclass(frozen=True)
class Variant:
    name: str
    enable_data_tiling: bool = False
    enable_stream_memory_flags: bool = False
    extra_flags: tuple[str, ...] = ()
    description: str = ""


VARIANTS = (
    Variant(
        name="baseline",
        description="Current MiniResNet static compile path.",
    ),
    Variant(
        name="minpeak_alias",
        enable_stream_memory_flags=True,
        description="Stream min-peak partitioning plus mutable binding aliasing.",
    ),
    Variant(
        name="aggressive_fusion",
        extra_flags=(
            "--iree-dispatch-creation-enable-aggressive-fusion",
            "--iree-dispatch-creation-fuse-multi-use",
            "--iree-dispatch-creation-enable-fuse-padding-into-linalg-consumer-ops",
            "--iree-dispatch-creation-enable-fuse-padding-into-linalg-producer-ops",
        ),
        description="Dispatch fusion and pad-fusion knobs without Stream min-peak flags.",
    ),
    Variant(
        name="data_tiling",
        enable_data_tiling=True,
        description="Enable the stable IREE data-tiling option.",
    ),
    Variant(
        name="experimental_data_tiling",
        extra_flags=(
            "--iree-dispatch-creation-experimental-data-tiling",
            "--iree-dispatch-creation-pad-factor=8",
        ),
        description="Experimental dispatch data tiling with explicit pad factor.",
    ),
    Variant(
        name="stream_debug",
        extra_flags=("--iree-stream-partitioning-favor=debug",),
        description="Debug Stream partitioning for scheduling comparison.",
    ),
)


def variant_by_name(name: str) -> Variant:
    for variant in VARIANTS:
        if variant.name == name:
            return variant
    known = ", ".join(variant.name for variant in VARIANTS)
    raise ValueError(f"unknown variant {name!r}; known variants: {known}")


def generated_config(variant: Variant, output_dir: Path) -> dict:
    return {
        "project_name": f"rrtos_iree_miniresnet_{variant.name}",
        "output_dir": str(output_dir),
        "model_library_name": f"rv_aios_miniresnet_{variant.name}_models",
        "defaults": {
            "target_arch": "rv32",
            "target_abi": "ilp32f",
            "cpu_features": "+m,+a,+f,+zicsr",
            "enable_llvmcpu_microkernels": False,
            "enable_data_tiling": variant.enable_data_tiling,
            "enable_stream_memory_flags": variant.enable_stream_memory_flags,
            "extra_iree_compile_flags": list(variant.extra_flags),
            "optimization": "size",
            "memory_strategy": "static",
        },
        "toolchain": {
            "root_env": "IREE_TOOLCHAIN_ROOT",
            "tflite_toolchain_env": "iree-toolchain310",
        },
        "models": [
            {
                "name": MODEL_NAME,
                "file": str(MODEL_PATH),
            }
        ],
    }


def write_config(variant: Variant, output_dir: Path, variant_root: Path) -> Path:
    variant_root.mkdir(parents=True, exist_ok=True)
    config_path = variant_root / "config.yaml"
    config_path.write_text(
        yaml.safe_dump(generated_config(variant, output_dir), sort_keys=False),
        encoding="utf-8",
    )
    return config_path


def max_allocation_for_stage(dump_dir: Path, stage_marker: str) -> int | None:
    values: list[int] = []
    for path in sorted(dump_dir.glob("*.mlir"), key=analyzer.sort_key):
        if stage_marker not in path.name:
            continue
        for allocation in analyzer.scan_allocations(path, batch_size=1):
            if allocation.size_value is not None:
                values.append(allocation.size_value)
    return max(values) if values else None


def summarize_dump(
    dump_dir: Path, needle_size: int = NEEDLE_ALLOCATION_SIZE
) -> dict[str, int | bool | None]:
    stream_peak = max_allocation_for_stage(dump_dir, ".7.")
    hal_peak = max_allocation_for_stage(dump_dir, ".11.")
    has_needle = False
    allocation_count = 0
    for path in sorted(dump_dir.glob("*.mlir"), key=analyzer.sort_key):
        if ".7." not in path.name and ".11." not in path.name:
            continue
        for allocation in analyzer.scan_allocations(path, batch_size=1):
            allocation_count += 1
            if allocation.size_value == needle_size:
                has_needle = True
    return {
        "max_stream_allocation": stream_peak,
        "max_hal_allocation": hal_peak,
        "has_315264": has_needle,
        "allocation_count": allocation_count,
    }


def run_codegen(config_path: Path, phases_dir: Path) -> subprocess.CompletedProcess[str]:
    cmd = [
        sys.executable,
        str(PROJECT_ROOT / "scripts" / "ai_codegen.py"),
        "--config",
        str(config_path),
        "--dump-compile-phases-to",
        str(phases_dir),
    ]
    return subprocess.run(
        cmd,
        cwd=PROJECT_ROOT,
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )


def compact_error(output: str, limit: int = 5) -> str:
    lines = [line.strip() for line in output.splitlines() if line.strip()]
    if not lines:
        return ""
    compiler_errors = [
        line
        for line in lines
        if "error:" in line or line.startswith("failed to ")
    ]
    if compiler_errors:
        return " | ".join(compiler_errors[:limit])
    return " | ".join(lines[-limit:])


def run_variant(variant: Variant, output_root: Path, clean: bool) -> dict:
    variant_root = output_root / variant.name
    output_dir = variant_root / "generated"
    phases_dir = variant_root / "phases"
    log_path = variant_root / "compile.log"

    if clean and variant_root.exists():
        shutil.rmtree(variant_root)
    variant_root.mkdir(parents=True, exist_ok=True)
    output_dir.mkdir(parents=True, exist_ok=True)
    phases_dir.mkdir(parents=True, exist_ok=True)

    config_path = write_config(variant, output_dir, variant_root)
    result = {
        "variant": variant.name,
        "status": "not_run",
        "config": str(config_path),
        "log": str(log_path),
        "max_stream_allocation": None,
        "max_hal_allocation": None,
        "has_315264": False,
        "allocation_count": 0,
        "error": "",
    }

    if not MODEL_PATH.exists():
        result["status"] = "missing_model"
        result["error"] = str(MODEL_PATH)
        return result

    proc = run_codegen(config_path, phases_dir)
    log_path.write_text(proc.stdout, encoding="utf-8")
    if proc.returncode != 0:
        result["status"] = "compile_failed"
        result["error"] = compact_error(proc.stdout)
        return result

    summary = summarize_dump(phases_dir)
    result.update(summary)
    result["status"] = "ok"
    return result


def format_size(value: int | None) -> str:
    return "-" if value is None else str(value)


def print_results(results: list[dict]) -> None:
    print(
        f"{'variant':26} {'status':15} {'stream_peak':>12} "
        f"{'hal_peak':>12} {'has_315264':>10}"
    )
    print("-" * 82)
    for result in results:
        print(
            f"{result['variant']:26} {result['status']:15} "
            f"{format_size(result['max_stream_allocation']):>12} "
            f"{format_size(result['max_hal_allocation']):>12} "
            f"{str(result['has_315264']).lower():>10}"
        )
        if result["status"] != "ok" and result["error"]:
            print(f"  error: {result['error']}")


def selected_variants(raw: str | None) -> list[Variant]:
    if not raw:
        return list(VARIANTS)
    return [variant_by_name(name.strip()) for name in raw.split(",") if name.strip()]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output-root",
        default=str(DEFAULT_OUTPUT_ROOT),
        help="Directory for generated variant configs, logs, outputs, and phase dumps.",
    )
    parser.add_argument(
        "--variants",
        help="Comma-separated variant names. Defaults to all built-in variants.",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Delete each selected variant directory before compiling it.",
    )
    args = parser.parse_args()

    output_root = Path(args.output_root).expanduser()
    if not output_root.is_absolute():
        output_root = PROJECT_ROOT / output_root
    output_root.mkdir(parents=True, exist_ok=True)

    results = [
        run_variant(variant, output_root=output_root, clean=args.clean)
        for variant in selected_variants(args.variants)
    ]
    print_results(results)

    return 0 if any(result["status"] == "ok" for result in results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
