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
        print(f"QSPI_FLASH_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[QSPI_APP] RUN_MARKER:",
        "[QSPI_APP] qspi_flash ready",
        "[QSPI_APP] Initializing kernel...",
        "[QSPI_APP] Starting scheduler...",
        "31505351",
        "5f4c444d",
        "00ef4018",
    ]

    failures: list[str] = []
    lower_text = text.lower()
    for token in required:
        haystack = lower_text if token[0].isdigit() else text
        if token not in haystack:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[QSPI_APP] tick " in line and "sig=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} qspi lines, found {len(lines)}"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"QSPI_FLASH_CHECK_FAIL: {failure}")
        return 1

    print(f"QSPI_FLASH_CHECK_OK: found {len(lines)} QSPI flash markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
