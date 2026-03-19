#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True)
    parser.add_argument("--min-lines", type=int, default=3)
    args = parser.parse_args()

    log_path = Path(args.log)
    if not log_path.exists():
        print(f"ADC_SINGLE_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[ADC_APP] RUN_MARKER:",
        "[ADC_APP] adc_single ready",
        "[ADC_APP] Initializing kernel...",
        "[ADC_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[ADC_APP] tick " in line and " rc=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} adc lines, found {len(lines)}"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"ADC_SINGLE_CHECK_FAIL: {failure}")
        return 1

    print(f"ADC_SINGLE_CHECK_OK: found {len(lines)} ADC markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
