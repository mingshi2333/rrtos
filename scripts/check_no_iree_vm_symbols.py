#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import sys


DEFAULT_FORBIDDEN_MARKERS = (
    "iree_vm_",
    "iree_modules_hal_",
    "iree_hal_module_",
    "iree_hal_static_library_loader",
)


def find_forbidden_lines(
    map_text: str,
    forbidden_markers: tuple[str, ...],
) -> list[str]:
    matches: list[str] = []
    for line in map_text.splitlines():
        if any(marker in line for marker in forbidden_markers):
            stripped = line.strip()
            if stripped:
                matches.append(stripped)
    return matches


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Reject final map files that link IREE VM/HAL module symbols."
    )
    parser.add_argument("--map", required=True, help="Path to the linker map file")
    parser.add_argument(
        "--forbid",
        action="append",
        default=[],
        metavar="TEXT",
        help="Additional forbidden substring; may be repeated",
    )
    parser.add_argument(
        "--max-lines",
        type=int,
        default=20,
        help="Maximum matching lines to print on failure",
    )
    args = parser.parse_args()

    map_path = Path(args.map)
    if not map_path.exists():
        print(f"NO_IREE_VM_SYMBOLS_FAIL map not found: {map_path}")
        return 2

    forbidden = DEFAULT_FORBIDDEN_MARKERS + tuple(args.forbid)
    matches = find_forbidden_lines(
        map_path.read_text(encoding="utf-8", errors="ignore"),
        forbidden,
    )

    if matches:
        print(
            "NO_IREE_VM_SYMBOLS_FAIL "
            f"matches={len(matches)} map={map_path} forbidden={','.join(forbidden)}"
        )
        for line in matches[: args.max_lines]:
            print(line)
        if len(matches) > args.max_lines:
            print(f"... {len(matches) - args.max_lines} more")
        return 1

    print(f"NO_IREE_VM_SYMBOLS_PASS map={map_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
