#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


MNIST_STATIC_DIRECT_PROBE_PASS_RE = re.compile(
    r"^MNIST_STATIC_DIRECT_PROBE_PASS exports=3\s*$", re.MULTILINE
)
MNIST_STATIC_DIRECT_RESULT_RE = re.compile(
    r"^MNIST_STATIC_DIRECT_RESULT: sample=\S+ label=7 argmax=7 top_hex=0x[0-9a-fA-F]+ latency_ticks=\d+\s*$",
    re.MULTILINE,
)
MNIST_STATIC_DIRECT_STATIC_RAM_RE = re.compile(
    r"^MNIST_STATIC_DIRECT_STATIC_RAM: input_bytes=\d+ scratch_bytes=\d+ output_bytes=\d+ io_scratch_bytes=\d+ task_stack_bytes=\d+\s*$",
    re.MULTILINE,
)
MNIST_STATIC_DIRECT_MODEL_PEAK_RE = re.compile(
    r"^MNIST_STATIC_DIRECT_MODEL_PEAK: phase=(?P<phase>invoke) heap_current_bytes=\d+ heap_peak_bytes=\d+ heap_alloc_count=\d+ heap_free_count=\d+\s*$",
    re.MULTILINE,
)
MNIST_STATIC_DIRECT_PASS_RE = re.compile(
    r"^MNIST_STATIC_DIRECT_PASS count=(?P<count>\d+)\s*$", re.MULTILINE
)


def validate_mnist_static_direct_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "MNIST_STATIC_DIRECT_FAIL" in output:
        errors.append("MNIST_STATIC_DIRECT_FAIL token present")

    if not MNIST_STATIC_DIRECT_PROBE_PASS_RE.search(output):
        errors.append("missing MNIST_STATIC_DIRECT_PROBE_PASS line")

    if not MNIST_STATIC_DIRECT_RESULT_RE.search(output):
        errors.append("missing MNIST_STATIC_DIRECT_RESULT line")

    if not MNIST_STATIC_DIRECT_STATIC_RAM_RE.search(output):
        errors.append("missing MNIST_STATIC_DIRECT_STATIC_RAM line")

    peak_phases = {
        match.group("phase")
        for match in MNIST_STATIC_DIRECT_MODEL_PEAK_RE.finditer(output)
    }
    if "invoke" not in peak_phases:
        errors.append("missing MNIST_STATIC_DIRECT_MODEL_PEAK phase=invoke")

    pass_matches = list(MNIST_STATIC_DIRECT_PASS_RE.finditer(output))
    if not pass_matches:
        errors.append("missing MNIST_STATIC_DIRECT_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple MNIST_STATIC_DIRECT_PASS count lines")
    elif int(pass_matches[0].group("count")) != 1:
        errors.append("MNIST_STATIC_DIRECT_PASS count must be 1")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/mnist_static_direct_validation",
        help="Path to the static-direct validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/mnist_static_direct_validation_qemu.log",
        help="Path to save the captured QEMU log",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=int,
        default=20,
        help="Timeout for the QEMU run",
    )
    parser.add_argument(
        "--memory",
        default="4M",
        help='QEMU RAM size; use "--memory 64M" for large QEMU-only tests',
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()

    if not kernel.exists():
        print(f"MNIST static-direct validation error: kernel not found: {kernel}")
        return 2

    log_path.parent.mkdir(parents=True, exist_ok=True)

    cmd = [
        "timeout",
        str(args.timeout_seconds),
        "qemu-system-riscv32",
        "-machine",
        "virt",
        "-icount",
        "shift=0,align=off,sleep=off",
        "-nographic",
        "-m",
        args.memory,
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
        print(f"MNIST static-direct validation error: QEMU exited with {proc.returncode}")
        return 1

    validation_errors = validate_mnist_static_direct_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"MNIST static-direct validation error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
