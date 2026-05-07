#!/usr/bin/env python3
"""Validate deploy gates recorded in a static-direct package manifest."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
from typing import Any

try:
    from scripts import check_no_iree_vm_symbols as no_vm_symbols
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import check_no_iree_vm_symbols as no_vm_symbols


def _load_package(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("package JSON must contain an object")
    return data


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def _artifact_path(package_dir: Path, rel_path: str) -> Path:
    path = package_dir / rel_path
    if not path.exists():
        raise FileNotFoundError(f"artifact not found: {path}")
    return path


def _validate_hashes(package_path: Path, package_data: dict[str, Any]) -> list[str]:
    package_dir = package_path.parent
    files = package_data.get("files", {})
    hashes = package_data.get("artifact_sha256", {})
    if not isinstance(files, dict) or not isinstance(hashes, dict):
        return ["package must contain files and artifact_sha256 objects"]

    errors: list[str] = []
    checked = 0
    for key, expected in hashes.items():
        rel_path = files.get(key)
        if not isinstance(rel_path, str):
            errors.append(f"hash key {key!r} has no matching file entry")
            continue
        try:
            actual = _sha256(_artifact_path(package_dir, rel_path))
        except OSError as exc:
            errors.append(str(exc))
            continue
        checked += 1
        if actual != expected:
            errors.append(
                f"{key}: expected {expected} actual {actual}"
            )

    for remap_index, remap in enumerate(package_data.get("remaps", [])):
        if not isinstance(remap, dict):
            errors.append(f"remap {remap_index} must be an object")
            continue
        remap_files = remap.get("files", {})
        remap_hashes = remap.get("artifact_sha256", {})
        if not isinstance(remap_files, dict) or not isinstance(remap_hashes, dict):
            errors.append(f"remap {remap_index} missing files/artifact_sha256")
            continue
        for key, expected in remap_hashes.items():
            rel_path = remap_files.get(key)
            if not isinstance(rel_path, str):
                errors.append(f"remap {remap_index} hash key {key!r} has no file")
                continue
            try:
                actual = _sha256(_artifact_path(package_dir, rel_path))
            except OSError as exc:
                errors.append(str(exc))
                continue
            checked += 1
            if actual != expected:
                errors.append(
                    f"remap {remap_index} {key}: expected {expected} actual {actual}"
                )

    if errors:
        print(f"STATIC_DIRECT_PACKAGE_HASH_FAIL checked={checked}")
        for error in errors:
            print(error)
    else:
        print(f"STATIC_DIRECT_PACKAGE_HASH_PASS checked={checked}")
    return errors


def _validate_no_vm_map(package_data: dict[str, Any]) -> list[str]:
    gate = (
        package_data.get("deployment", {})
        .get("gates", {})
        .get("no_vm_map", {})
    )
    if not isinstance(gate, dict) or gate.get("status") != "configured":
        print("NO_IREE_VM_SYMBOLS_SKIP not_configured")
        return []
    map_path_raw = gate.get("map")
    if not isinstance(map_path_raw, str):
        return ["no_vm_map gate missing map path"]
    map_path = Path(map_path_raw)
    if not map_path.exists():
        return [f"no_vm_map map not found: {map_path}"]
    matches = no_vm_symbols.find_forbidden_lines(
        map_path.read_text(encoding="utf-8", errors="ignore"),
        no_vm_symbols.DEFAULT_FORBIDDEN_MARKERS,
    )
    if matches:
        print(
            "NO_IREE_VM_SYMBOLS_FAIL "
            f"matches={len(matches)} map={map_path}"
        )
        for line in matches[:20]:
            print(line)
        return [f"forbidden symbols in {map_path}"]
    print(f"NO_IREE_VM_SYMBOLS_PASS map={map_path}")
    return []


def _run_output_oracle(package_data: dict[str, Any], *, run_oracle: bool) -> list[str]:
    gate = (
        package_data.get("deployment", {})
        .get("gates", {})
        .get("output_oracle", {})
    )
    if not isinstance(gate, dict) or gate.get("status") != "configured":
        print("STATIC_DIRECT_PACKAGE_ORACLE_SKIP not_configured")
        return []
    command = gate.get("command")
    if not isinstance(command, list) or not all(isinstance(item, str) for item in command):
        return ["output_oracle gate missing command"]
    if not run_oracle:
        print("STATIC_DIRECT_PACKAGE_ORACLE_SKIP run_oracle_not_requested")
        return []
    proc = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    sys.stdout.write(proc.stdout)
    if proc.returncode != 0:
        return [f"output oracle command failed with {proc.returncode}"]
    print("STATIC_DIRECT_PACKAGE_ORACLE_PASS")
    return []


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Validate static-direct package artifact hashes and deploy gates."
    )
    parser.add_argument("--package", required=True, type=Path)
    parser.add_argument("--run-oracle", action="store_true")
    args = parser.parse_args(argv)

    try:
        package_data = _load_package(args.package)
        errors = []
        errors.extend(_validate_hashes(args.package, package_data))
        errors.extend(_validate_no_vm_map(package_data))
        errors.extend(_run_output_oracle(package_data, run_oracle=args.run_oracle))
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(f"STATIC_DIRECT_PACKAGE_VALIDATE_FAIL {exc}")
        return 2

    if errors:
        for error in errors:
            print(f"STATIC_DIRECT_PACKAGE_VALIDATE_FAIL {error}")
        return 1
    print(f"STATIC_DIRECT_PACKAGE_VALIDATE_PASS package={args.package}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
