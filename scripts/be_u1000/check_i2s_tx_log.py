#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re


LINE_RE = re.compile(
    r"level=(?P<level>\d+).*threshold=(?P<threshold>\d+).*action=(?P<action>[a-z-]+).*post=(?P<post>\d+)"
)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True)
    parser.add_argument("--min-lines", type=int, default=3)
    args = parser.parse_args()

    log_path = Path(args.log)
    if not log_path.exists():
        print(f"I2S_TX_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[I2S_APP] RUN_MARKER:",
        "[I2S_APP] i2s_tx ready",
        "[I2S_APP] tx-enable rc=0",
        "[I2S_APP] Initializing kernel...",
        "[I2S_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[I2S_APP] tick " in line and "sample=0x" in line and "level=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} i2s lines, found {len(lines)}"
        )

    if lines and not any("ready=1" in line for line in lines):
        failures.append("i2s lane never reported a ready TX FIFO condition")

    if lines and not any(
        "level=1" in line
        or "level=2" in line
        or "post=1" in line
        or "post=2" in line
        for line in lines
    ):
        failures.append("i2s fifo level never advanced beyond reset")

    parsed = [LINE_RE.search(line) for line in lines]
    if lines and any(match is None for match in parsed):
        failures.append("unable to parse i2s lifecycle markers")
    else:
        levels = [int(match.group("level")) for match in parsed if match is not None]
        posts = [int(match.group("post")) for match in parsed if match is not None]
        thresholds = [
            int(match.group("threshold")) for match in parsed if match is not None
        ]
        actions = [match.group("action") for match in parsed if match is not None]

        if thresholds and not all(threshold == 3 for threshold in thresholds):
            failures.append("i2s threshold drifted from configured FIFO level")

        if any(level > 16 or post > 16 for level, post in zip(levels, posts)):
            failures.append("i2s fifo level exceeded bounded depth")

        if "flush" not in actions:
            failures.append("i2s lane never exercised an explicit flush action")
        elif not any(
            action == "flush" and post == 0 for action, post in zip(actions, posts)
        ):
            failures.append("i2s flush action did not drain the fifo back to zero")

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"I2S_TX_CHECK_FAIL: {failure}")
        return 1

    print(f"I2S_TX_CHECK_OK: found {len(lines)} I2S markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
