#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


QUANT_PASS_RE = re.compile(r"^ST_MNISTV1_QUANT_PASS count=(?P<count>\d+)\s*$", re.MULTILINE)
QUANT_RESULT_RE = re.compile(
    r"^ST_MNISTV1_QUANT_RESULT: .* latency_us=\d+ latency_cycles=\d+ latency_instructions=\d+ total=\d+\s*$",
    re.MULTILINE,
)


def validate_mnist_quant_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "ST_MNISTV1_QUANT_FAIL" in output:
        errors.append("ST_MNISTV1_QUANT_FAIL token present")

    pass_matches = list(QUANT_PASS_RE.finditer(output))
    pass_count: int | None = None
    if not pass_matches:
        errors.append("missing ST_MNISTV1_QUANT_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple ST_MNISTV1_QUANT_PASS count lines")
    else:
        pass_count = int(pass_matches[0].group("count"))
        if pass_count <= 0:
            errors.append("ST_MNISTV1_QUANT_PASS count must be positive")

    result_count = len(list(QUANT_RESULT_RE.finditer(output)))
    if pass_count is not None and result_count != pass_count:
        errors.append(
            f"ST_MNISTV1_QUANT_RESULT count {result_count} does not match pass count {pass_count}"
        )
    elif pass_count is None and result_count == 0:
        errors.append("missing ST_MNISTV1_QUANT_RESULT lines")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/mnist_quant_validation",
        help="Path to the quantized MNIST validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/mnist_quant_validation_qemu.log",
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
        print(f"MNIST quant validation error: kernel not found: {kernel}")
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
        print(f"MNIST quant validation error: QEMU exited with {proc.returncode}")
        return 1

    validation_errors = validate_mnist_quant_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"MNIST quant validation error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
