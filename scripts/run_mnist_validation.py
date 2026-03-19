#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/mnist_validation",
        help="Path to the validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/mnist_validation_qemu.log",
        help="Path to save the captured QEMU log",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=int,
        default=20,
        help="Timeout for the QEMU run",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()

    if not kernel.exists():
        print(f"AI runtime validation error: kernel not found: {kernel}")
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
        print(f"AI runtime validation error: QEMU exited with {proc.returncode}")
        return 1

    if "AI_VALIDATION_PASS" not in output:
        print("AI runtime validation error: missing AI_VALIDATION_PASS token")
        return 1

    if "AI_VALIDATION_METRICS:" not in output:
        print("AI runtime validation error: missing AI_VALIDATION_METRICS token")
        return 1

    if "OS_TIMER_CALLBACK_PASS" not in output:
        print("AI runtime validation error: missing OS_TIMER_CALLBACK_PASS token")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
