#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


PASS_PATTERN = re.compile(r"PREEMPT_PASS tick=(\d+) counter=(\d+)")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/mnist_preemption_probe",
        help="Path to the preemption probe kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/mnist_preemption_probe_qemu.log",
        help="Path to save the captured QEMU log",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=int,
        default=10,
        help="Timeout for the QEMU run",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()

    if not kernel.exists():
        print(f"Preemption integration error: kernel not found: {kernel}")
        return 2

    log_path.parent.mkdir(parents=True, exist_ok=True)

    cmd = [
        "timeout",
        str(args.timeout_seconds),
        "qemu-system-riscv32",
        "-machine",
        "virt",
        "-nographic",
        "-m",
        "4M",
        "-bios",
        "none",
        "-kernel",
        str(kernel),
    ]

    proc = subprocess.run(
        cmd,
        cwd=repo_root,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )

    output = proc.stdout
    log_path.write_text(output, encoding="utf-8")
    sys.stdout.write(output)

    if proc.returncode not in (0, 124):
        print(f"Preemption integration error: QEMU exited with {proc.returncode}")
        return 1

    if "PREEMPT_FAIL" in output:
        print("Preemption integration error: probe reported PREEMPT_FAIL")
        return 1

    match = PASS_PATTERN.search(output)
    if not match:
        print("Preemption integration error: missing PREEMPT_PASS token")
        return 1

    tick = int(match.group(1))
    counter = int(match.group(2))
    if tick < 3 or counter <= 0:
        print(
            "Preemption integration error: invalid PASS payload "
            f"tick={tick} counter={counter}"
        )
        return 1

    if "PREEMPT_LOW_READY" not in output:
        print("Preemption integration error: missing PREEMPT_LOW_READY token")
        return 1

    print(f"Preemption integration passed: tick={tick} counter={counter}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
