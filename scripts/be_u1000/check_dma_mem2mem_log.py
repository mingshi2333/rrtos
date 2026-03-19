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
        print(f"DMA_MEM2MEM_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[DMA_APP] RUN_MARKER:",
        "[DMA_APP] dma_mem2mem ready",
        "[DMA_APP] Initializing kernel...",
        "[DMA_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[DMA_APP] tick " in line and "match=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} dma lines, found {len(lines)}"
        )

    if lines and not any("rc=0" in line and "match=1" in line for line in lines):
        failures.append("no dma line shows successful mem2mem copy semantics")

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"DMA_MEM2MEM_CHECK_FAIL: {failure}")
        return 1

    print(f"DMA_MEM2MEM_CHECK_OK: found {len(lines)} DMA markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
