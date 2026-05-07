#!/usr/bin/env python3
"""Generate a static-direct model package from a HAL schedule JSON."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import json
from pathlib import Path
from typing import Any

try:
    from scripts import generate_static_direct_descriptor as descriptor_generator
    from scripts import generate_static_direct_remap_plan as remap_generator
    from scripts import verify_static_direct_schedule as schedule_verifier
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import generate_static_direct_descriptor as descriptor_generator
    import generate_static_direct_remap_plan as remap_generator
    import verify_static_direct_schedule as schedule_verifier


SCHEMA = "rrtos.static_direct_package.v1"


@dataclass(frozen=True)
class RemapConfig:
    name: str
    patch_allocation: str
    patch_internal_offset: int
    produced_bytes: int
    continuation_command_index: int
    remap_end_command: int
    scratch_allocation: str | None = None
    scratch_end_command: int | None = None
    macro_prefix: str = "STATIC_DIRECT_REMAP"
    header_guard: str = "STATIC_DIRECT_REMAP_H"
    function_name: str = "static_direct_invoke_remapped"


@dataclass(frozen=True)
class DeploymentConfig:
    map_file: Path | None = None
    oracle_runner: Path | None = None
    oracle_kernel: Path | None = None
    oracle_log: Path | None = None
    oracle_memory: str = "64M"
    oracle_timeout_seconds: int = 30


def _kib(value: int) -> float:
    return round(value / 1024.0, 3)


def _write_json(path: Path, data: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def _copy_schedule(schedule: dict[str, Any]) -> dict[str, Any]:
    return json.loads(json.dumps(schedule, sort_keys=True))


def _file_ref(output_dir: Path, path: Path) -> str:
    return path.relative_to(output_dir).as_posix()


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _remap_file_prefix(model_ident: str, remap_name: str) -> str:
    remap_ident = descriptor_generator._c_identifier(remap_name)
    return f"{model_ident}_{remap_ident}_remap"


def _generate_remap(
    verify_report: dict[str, Any],
    *,
    model_ident: str,
    output_dir: Path,
    remap: RemapConfig,
) -> dict[str, Any]:
    file_prefix = _remap_file_prefix(model_ident, remap.name)
    plan = remap_generator.build_remap_plan(
        verify_report,
        name=remap.name,
        patch_allocation=remap.patch_allocation,
        patch_internal_offset=remap.patch_internal_offset,
        produced_bytes=remap.produced_bytes,
        continuation_command_index=remap.continuation_command_index,
        remap_end_command=remap.remap_end_command,
        scratch_allocation=remap.scratch_allocation,
        scratch_end_command=remap.scratch_end_command,
    )

    plan_json = output_dir / f"{file_prefix}_plan.json"
    plan_md = output_dir / f"{file_prefix}_plan.md"
    remap_c = output_dir / f"{file_prefix}.c"
    remap_h = output_dir / f"{file_prefix}.h"
    _write_json(plan_json, plan)
    plan_md.write_text(remap_generator.render_markdown(plan), encoding="utf-8")
    remap_h.write_text(
        remap_generator.render_c_header(
            plan,
            header_guard=remap.header_guard,
            macro_prefix=remap.macro_prefix,
            function_name=remap.function_name,
        ),
        encoding="ascii",
    )
    remap_c.write_text(
        remap_generator.render_c_source(
            plan,
            header_name=remap_h.name,
            macro_prefix=remap.macro_prefix,
            function_name=remap.function_name,
        ),
        encoding="ascii",
    )

    return {
        "name": remap.name,
        "arena_bytes": plan["arena_bytes"],
        "arena_kib": plan["arena_kib"],
        "savings_bytes": plan["savings_bytes"],
        "savings_kib": plan["savings_kib"],
        "files": {
            "plan_json": _file_ref(output_dir, plan_json),
            "plan_md": _file_ref(output_dir, plan_md),
            "source_c": _file_ref(output_dir, remap_c),
            "header_h": _file_ref(output_dir, remap_h),
        },
        "artifact_sha256": {
            "plan_json": _sha256(plan_json),
            "plan_md": _sha256(plan_md),
            "source_c": _sha256(remap_c),
            "header_h": _sha256(remap_h),
        },
    }


def _deployment_manifest(
    deployment: DeploymentConfig | None,
    *,
    package_json: Path,
) -> dict[str, Any]:
    no_vm_command = None
    no_vm_status = "not_configured"
    if deployment is not None and deployment.map_file is not None:
        no_vm_command = [
            "python3",
            "scripts/check_no_iree_vm_symbols.py",
            "--map",
            str(deployment.map_file),
        ]
        no_vm_status = "configured"

    oracle_command = None
    oracle_status = "not_configured"
    if (
        deployment is not None
        and deployment.oracle_runner is not None
        and deployment.oracle_kernel is not None
        and deployment.oracle_log is not None
    ):
        oracle_command = [
            "python3",
            str(deployment.oracle_runner),
            "--kernel",
            str(deployment.oracle_kernel),
            "--log",
            str(deployment.oracle_log),
            "--memory",
            deployment.oracle_memory,
            "--timeout-seconds",
            str(deployment.oracle_timeout_seconds),
        ]
        oracle_status = "configured"

    deploy_ready = no_vm_status == "configured" and oracle_status == "configured"
    validate_command = [
        "python3",
        "scripts/validate_static_direct_package.py",
        "--package",
        str(package_json),
    ]
    if oracle_status == "configured":
        validate_command.append("--run-oracle")

    return {
        "deploy_ready": deploy_ready,
        "validate_command": validate_command,
        "gates": {
            "artifact_hash": {
                "status": "configured",
            },
            "no_vm_map": {
                "status": no_vm_status,
                "map": str(deployment.map_file)
                if deployment is not None and deployment.map_file is not None
                else None,
                "command": no_vm_command,
            },
            "output_oracle": {
                "status": oracle_status,
                "runner": str(deployment.oracle_runner)
                if deployment is not None and deployment.oracle_runner is not None
                else None,
                "kernel": str(deployment.oracle_kernel)
                if deployment is not None and deployment.oracle_kernel is not None
                else None,
                "log": str(deployment.oracle_log)
                if deployment is not None and deployment.oracle_log is not None
                else None,
                "memory": deployment.oracle_memory if deployment is not None else None,
                "timeout_seconds": deployment.oracle_timeout_seconds
                if deployment is not None
                else None,
                "command": oracle_command,
            },
        },
    }


def generate_package(
    schedule: dict[str, Any],
    *,
    model_name: str,
    entry: str,
    output_dir: Path,
    native_header: Path | None = None,
    native_object: Path | None = None,
    remap: RemapConfig | None = None,
    deployment: DeploymentConfig | None = None,
) -> dict[str, Any]:
    model_ident = descriptor_generator._c_identifier(model_name)
    output_dir.mkdir(parents=True, exist_ok=True)

    schedule_copy = _copy_schedule(schedule)
    verify_report = schedule_verifier.verify_schedule(schedule_copy)

    schedule_json = output_dir / "static_direct.schedule.json"
    verify_json = output_dir / "static_direct.verify.json"
    descriptor_c = output_dir / f"{model_ident}_static_direct_desc.c"
    descriptor_h = output_dir / f"{model_ident}_static_direct_desc.h"

    _write_json(schedule_json, schedule_copy)
    _write_json(verify_json, verify_report)
    descriptor_generator.generate_descriptor(
        schedule_copy,
        model_name=model_name,
        entry=entry,
        output_c=descriptor_c,
        output_h=descriptor_h,
        native_header=native_header,
        native_object=native_object,
        memory_plan=verify_report,
    )

    remaps = []
    if remap is not None:
        remaps.append(
            _generate_remap(
                verify_report,
                model_ident=model_ident,
                output_dir=output_dir,
                remap=remap,
            )
        )

    package_json = output_dir / "static_direct.package.json"
    manifest: dict[str, Any] = {
        "schema": SCHEMA,
        "model_name": model_name,
        "entry_name": entry,
        "source": schedule_copy.get("source", ""),
        "metrics": {
            "input_bytes": descriptor_generator._u32(
                verify_report.get("input_bytes", 0),
                "verify input bytes",
            )
            if "input_bytes" in verify_report
            else 0,
            "input_kib": _kib(verify_report.get("input_bytes", 0)),
            "const_bytes": descriptor_generator._u32(
                verify_report.get("const_bytes", 0),
                "verify const bytes",
            )
            if "const_bytes" in verify_report
            else 0,
            "const_kib": _kib(verify_report.get("const_bytes", 0)),
            "output_bytes": descriptor_generator._u32(
                verify_report.get("output_bytes", 0),
                "verify output bytes",
            )
            if "output_bytes" in verify_report
            else 0,
            "output_kib": _kib(verify_report.get("output_bytes", 0)),
            "arena_bytes": verify_report["arena_bytes"],
            "arena_kib": _kib(verify_report["arena_bytes"]),
            "dispatch_count": verify_report["dispatch_count"],
            "command_count": verify_report["command_count"],
            "allocation_count": verify_report["allocation_count"],
        },
        "files": {
            "schedule_json": _file_ref(output_dir, schedule_json),
            "verify_json": _file_ref(output_dir, verify_json),
            "memory_plan_json": _file_ref(output_dir, verify_json),
            "descriptor_c": _file_ref(output_dir, descriptor_c),
            "descriptor_h": _file_ref(output_dir, descriptor_h),
        },
        "artifact_sha256": {
            "schedule_json": _sha256(schedule_json),
            "verify_json": _sha256(verify_json),
            "memory_plan_json": _sha256(verify_json),
            "descriptor_c": _sha256(descriptor_c),
            "descriptor_h": _sha256(descriptor_h),
        },
        "native": {
            "header": str(native_header) if native_header is not None else None,
            "object": str(native_object) if native_object is not None else None,
        },
        "remaps": remaps,
        "deployment": _deployment_manifest(
            deployment,
            package_json=package_json,
        ),
    }
    _write_json(package_json, manifest)
    return manifest


def _load_schedule(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("schedule JSON must contain an object")
    return data


def _optional_remap(args: argparse.Namespace) -> RemapConfig | None:
    if args.remap_name is None:
        return None
    required = {
        "--patch-allocation": args.patch_allocation,
        "--patch-internal-offset": args.patch_internal_offset,
        "--produced-bytes": args.produced_bytes,
        "--continuation-command-index": args.continuation_command_index,
        "--remap-end-command": args.remap_end_command,
        "--remap-macro-prefix": args.remap_macro_prefix,
        "--remap-header-guard": args.remap_header_guard,
        "--remap-function-name": args.remap_function_name,
    }
    missing = [name for name, value in required.items() if value is None]
    if missing:
        raise ValueError(
            f"{', '.join(missing)} required when --remap-name is provided"
        )
    return RemapConfig(
        name=args.remap_name,
        patch_allocation=args.patch_allocation,
        patch_internal_offset=args.patch_internal_offset,
        produced_bytes=args.produced_bytes,
        continuation_command_index=args.continuation_command_index,
        remap_end_command=args.remap_end_command,
        scratch_allocation=args.scratch_allocation,
        scratch_end_command=args.scratch_end_command,
        macro_prefix=args.remap_macro_prefix,
        header_guard=args.remap_header_guard,
        function_name=args.remap_function_name,
    )


def _optional_deployment(args: argparse.Namespace) -> DeploymentConfig | None:
    if (
        args.map_file is None
        and args.oracle_runner is None
        and args.oracle_kernel is None
        and args.oracle_log is None
    ):
        return None
    return DeploymentConfig(
        map_file=args.map_file,
        oracle_runner=args.oracle_runner,
        oracle_kernel=args.oracle_kernel,
        oracle_log=args.oracle_log,
        oracle_memory=args.oracle_memory,
        oracle_timeout_seconds=args.oracle_timeout_seconds,
    )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Generate a static-direct package from a schedule JSON."
    )
    parser.add_argument("--schedule", required=True, type=Path)
    parser.add_argument("--model-name", required=True)
    parser.add_argument("--entry", required=True)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--native-header", type=Path)
    parser.add_argument("--native-object", type=Path)
    parser.add_argument("--remap-name")
    parser.add_argument("--patch-allocation")
    parser.add_argument("--patch-internal-offset", type=int)
    parser.add_argument("--produced-bytes", type=int)
    parser.add_argument("--continuation-command-index", type=int)
    parser.add_argument("--remap-end-command", type=int)
    parser.add_argument("--scratch-allocation")
    parser.add_argument("--scratch-end-command", type=int)
    parser.add_argument("--remap-macro-prefix")
    parser.add_argument("--remap-header-guard")
    parser.add_argument("--remap-function-name")
    parser.add_argument("--map-file", type=Path)
    parser.add_argument("--oracle-runner", type=Path)
    parser.add_argument("--oracle-kernel", type=Path)
    parser.add_argument("--oracle-log", type=Path)
    parser.add_argument("--oracle-memory", default="64M")
    parser.add_argument("--oracle-timeout-seconds", type=int, default=30)
    args = parser.parse_args(argv)

    try:
        generate_package(
            _load_schedule(args.schedule),
            model_name=args.model_name,
            entry=args.entry,
            output_dir=args.output_dir,
            native_header=args.native_header,
            native_object=args.native_object,
            remap=_optional_remap(args),
            deployment=_optional_deployment(args),
        )
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        parser.exit(1, f"error: {exc}\n")
    print(f"Wrote {args.output_dir / 'static_direct.package.json'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
