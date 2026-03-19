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
        print(f"UART_PRINTF_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[UART_APP] RUN_MARKER:",
        "[UART_APP] uart_printf ready",
        "[UART_APP] Initializing kernel...",
        "[UART_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[UART_APP] UART printf example tick " in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} uart printf lines, found {len(lines)}"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"UART_PRINTF_CHECK_FAIL: {failure}")
        return 1

    print(f"UART_PRINTF_CHECK_OK: found {len(lines)} uart printf markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
