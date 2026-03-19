#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True)
    parser.add_argument("--min-blinks", type=int, default=3)
    args = parser.parse_args()

    log_path = Path(args.log)
    if args.min_blinks < 1:
        print(
            f"GPIO_LEDBLINK_CHECK_ERROR: --min-blinks must be >= 1: {args.min_blinks}"
        )
        return 2
    if not log_path.exists() or not log_path.is_file():
        print(f"GPIO_LEDBLINK_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[GPIO_APP] RUN_MARKER:",
        "[GPIO_APP] ledblink ready",
        "[GPIO_APP] Initializing kernel...",
        "[GPIO_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    blink_lines = [
        line for line in text.splitlines() if "[GPIO_APP] blink tick " in line
    ]
    if len(blink_lines) < args.min_blinks:
        failures.append(
            f"expected at least {args.min_blinks} blink lines, found {len(blink_lines)}"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"GPIO_LEDBLINK_CHECK_FAIL: {failure}")
        return 1

    print(
        f"GPIO_LEDBLINK_CHECK_OK: found {len(blink_lines)} blink markers in {log_path}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
