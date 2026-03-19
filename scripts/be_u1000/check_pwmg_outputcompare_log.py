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
        print(f"PWMG_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[PWM_APP] RUN_MARKER:",
        "[PWM_APP] pwmg_outputcompare ready",
        "[PWM_APP] match-enable rc=0",
        "[PWM_APP] counter-enable rc=0",
        "[PWM_APP] match-disable rc=0 after-match=1",
        "[PWM_APP] Initializing kernel...",
        "[PWM_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[PWM_APP] tick " in line
        and "counter=" in line
        and "compare=" in line
        and "match=" in line
        and "ccie=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} pwm lines, found {len(lines)}"
        )

    if lines and not any("match=1" in line for line in lines):
        failures.append("pwmg lane never reported a compare match event")

    if lines and not any("match=1" in line and "ccie=0" in line for line in lines):
        failures.append(
            "pwmg lane never showed compare-match pending after compare interrupt disable"
        )

    counters: list[int] = []
    for line in lines:
        marker = "counter="
        start = line.find(marker)
        if start < 0:
            continue
        start += len(marker)
        end = line.find(" ", start)
        token = line[start:] if end < 0 else line[start:end]
        try:
            counters.append(int(token))
        except ValueError:
            failures.append(f"bad counter token in line: {line}")
            break

    if len(counters) >= 2 and not any(
        curr < prev for prev, curr in zip(counters, counters[1:])
    ):
        failures.append(
            "pwmg counter never wrapped back to the configured autoreload window"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"PWMG_CHECK_FAIL: {failure}")
        return 1

    print(f"PWMG_CHECK_OK: found {len(lines)} PWMG markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
