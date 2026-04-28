#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


AI_VALIDATION_PASS_RE = re.compile(r"^AI_VALIDATION_PASS count=(?P<count>\d+)\s*$", re.MULTILINE)
AI_VALIDATION_METRICS_RE = re.compile(r"^AI_VALIDATION_METRICS:", re.MULTILINE)
OS_TIMER_CALLBACK_PASS_RE = re.compile(r"^OS_TIMER_CALLBACK_PASS count=\d+ tick=\d+\s*$", re.MULTILINE)


def validate_ai_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "AI_VALIDATION_FAIL" in output:
        errors.append("AI_VALIDATION_FAIL token present")

    pass_matches = list(AI_VALIDATION_PASS_RE.finditer(output))
    pass_count: int | None = None
    if not pass_matches:
        errors.append("missing AI_VALIDATION_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple AI_VALIDATION_PASS count lines")
    else:
        pass_count = int(pass_matches[0].group("count"))
        if pass_count <= 0:
            errors.append("AI_VALIDATION_PASS count must be positive")

    metrics_count = len(list(AI_VALIDATION_METRICS_RE.finditer(output)))
    if pass_count is not None and metrics_count != pass_count:
        errors.append(
            f"AI_VALIDATION_METRICS count {metrics_count} does not match pass count {pass_count}"
        )
    elif pass_count is None and metrics_count == 0:
        errors.append("missing AI_VALIDATION_METRICS lines")

    if not OS_TIMER_CALLBACK_PASS_RE.search(output):
        errors.append("missing OS_TIMER_CALLBACK_PASS line")

    return errors


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

    validation_errors = validate_ai_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"AI runtime validation error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
