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
        print(f"PWMA_TIMEBASE_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[PWMA_APP] RUN_MARKER:",
        "[PWMA_APP] pwma_timebase ready",
        "[PWMA_APP] irq-enable rc=0",
        "[PWMA_APP] counter-enable rc=0",
        "[PWMA_APP] update-disable rc=0 after-update=1",
        "[PWMA_APP] repetition-set rc=0 after-update=1 rcr=1",
        "[PWMA_APP] Initializing kernel...",
        "[PWMA_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[PWMA_APP] tick " in line
        and "counter=" in line
        and "rcr=" in line
        and "uif=" in line
        and "uie=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} pwma lines, found {len(lines)}"
        )

    if lines and not any("counter=1" in line or "counter=2" in line for line in lines):
        failures.append("pwma counter never advanced from reset")

    if lines and not any("uif=1" in line for line in lines):
        failures.append("pwma lane never reported an update event")

    if lines and not any("uif=1" in line and "uie=0" in line for line in lines):
        failures.append(
            "pwma lane never showed update pending after update interrupt disable"
        )

    if lines and not any("rcr=1" in line for line in lines):
        failures.append("pwma lane never reported the reprogrammed repetition counter")

    if lines and not any("rcr=1" in line and "updates=2" in line for line in lines):
        failures.append(
            "pwma lane never reached a second update after RCR reprogramming"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"PWMA_TIMEBASE_CHECK_FAIL: {failure}")
        return 1

    print(f"PWMA_TIMEBASE_CHECK_OK: found {len(lines)} PWMA markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
