#!/usr/bin/env python3
"""Scan research-framework docs for unresolved placeholders."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


DEFAULT_PATTERN = re.compile(
    r"\b(TODO|TBD|FIXME)\b|<fill|<placeholder|\{\{[^}]+\}\}",
    re.IGNORECASE,
)
DEFAULT_SUFFIXES = {".md", ".yaml", ".yml", ".json", ".toml", ".txt", ".py"}


def scan_for_placeholders(
    root: Path,
    exclude_dirs: set[str],
    pattern: re.Pattern[str] = DEFAULT_PATTERN,
) -> list[str]:
    findings: list[str] = []

    for path in sorted(root.rglob("*")):
        if not path.is_file():
            continue
        if path.suffix.lower() not in DEFAULT_SUFFIXES:
            continue

        rel = path.relative_to(root)
        if any(part in exclude_dirs for part in rel.parts):
            continue

        content = path.read_text(encoding="utf-8", errors="ignore")
        for idx, line in enumerate(content.splitlines(), start=1):
            if pattern.search(line):
                findings.append(f"{rel}:{idx}: {line.strip()}")

    return findings


def main() -> int:
    parser = argparse.ArgumentParser(description="Scan docs for placeholder markers")
    parser.add_argument("--root", required=True, help="Root directory to scan")
    parser.add_argument(
        "--exclude",
        action="append",
        default=[],
        help="Directory name to exclude (can be repeated)",
    )
    args = parser.parse_args()

    root = Path(args.root)
    if not root.exists() or not root.is_dir():
        print(f"PLACEHOLDER_SCAN_ERROR: invalid root directory: {root}")
        return 2

    excludes = set(args.exclude)
    findings = scan_for_placeholders(root, excludes)
    if findings:
        print("PLACEHOLDER_SCAN_FAILED")
        for item in findings:
            print(f"- {item}")
        return 1

    print("PLACEHOLDER_SCAN_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
