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
    if args.min_lines < 1:
        print(f"CANFD_POLL_CHECK_ERROR: --min-lines must be >= 1: {args.min_lines}")
        return 2
    if not log_path.exists() or not log_path.is_file():
        print(f"CANFD_POLL_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[CANFD_APP] RUN_MARKER:",
        "[CANFD_APP] canfd_polling ready",
        "[CANFD_APP] Initializing kernel...",
        "[CANFD_APP] Starting scheduler...",
        "[CHK] CANFD0 loopback: OK",
        "[CHK] CANFD1 loopback: OK",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[CANFD_APP] tick " in line and "canfd lane alive" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} canfd app heartbeat lines, found {len(lines)}"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"CANFD_POLL_CHECK_FAIL: {failure}")
        return 1

    print(
        f"CANFD_POLL_CHECK_OK: found {len(lines)} CANFD app heartbeat markers in {log_path}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
