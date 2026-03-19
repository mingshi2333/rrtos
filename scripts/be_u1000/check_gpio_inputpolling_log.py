#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True)
    parser.add_argument("--min-samples", type=int, default=3)
    args = parser.parse_args()

    log_path = Path(args.log)
    if not log_path.exists():
        print(f"GPIO_INPUTPOLL_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[GPIO_POLL] RUN_MARKER:",
        "[GPIO_POLL] inputpolling ready",
        "[GPIO_POLL] Initializing kernel...",
        "[GPIO_POLL] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    samples = [line for line in text.splitlines() if "[GPIO_POLL] tick " in line]
    if len(samples) < args.min_samples:
        failures.append(
            f"expected at least {args.min_samples} polling lines, found {len(samples)}"
        )

    if samples and not any(
        "button=0 led=1" in line or "button=1 led=0" in line for line in samples
    ):
        failures.append("polling lines do not show expected LED/button relationship")

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"GPIO_INPUTPOLL_CHECK_FAIL: {failure}")
        return 1

    print(
        f"GPIO_INPUTPOLL_CHECK_OK: found {len(samples)} polling markers in {log_path}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
