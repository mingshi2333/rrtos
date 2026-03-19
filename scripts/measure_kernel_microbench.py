#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path

from measure_kernel_baseline import (
    DEFAULT_MICROBENCH_JSON,
    DEFAULT_MICROBENCH_MD,
    build_microbench_payload,
    write_microbench_report,
)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_MICROBENCH_MD), help="Markdown report path")
    parser.add_argument("--json", default=str(DEFAULT_MICROBENCH_JSON), help="JSON report path")
    args = parser.parse_args()

    payload = build_microbench_payload()
    write_microbench_report(Path(args.output), Path(args.json), payload)
    print(f"Microbench report written to {args.output}")
    print(f"Microbench JSON written to {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
