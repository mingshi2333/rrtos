#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


MINIRESNET_PASS_RE = re.compile(
    r"^MINIRESNET_PASS count=(?P<count>\d+)\s*$", re.MULTILINE
)
MINIRESNET_RESULT_RE = re.compile(
    r"^MINIRESNET_RESULT: .* argmax=\d+ hash=\d+ top_score_q=-?\d+ "
    r"latency_us=\d+ latency_cycles=\d+ latency_instructions=\d+ total=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_PEAK_RE = re.compile(
    r"^MINIRESNET_MODEL_PEAK: phase=(?P<phase>init|invoke) "
    r"heap_current_bytes=\d+ heap_peak_bytes=\d+ heap_alloc_count=\d+ "
    r"heap_free_count=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_TRACE_PEAK_RE = re.compile(
    r"^MINIRESNET_HEAP_TRACE_PEAK: phase=invoke seq=\d+ size=\d+ "
    r"current_used_bytes=\d+ ptr=\d+ caller=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_TRACE_TOP_RE = re.compile(
    r"^MINIRESNET_HEAP_TRACE_TOP: phase=invoke rank=\d+ seq=\d+ size=\d+ "
    r"current_used_bytes=\d+ ptr=\d+ caller=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_HAL_TRACE_PEAK_RE = re.compile(
    r"^MINIRESNET_HAL_TRACE_PEAK: phase=invoke seq=\d+ allocation_size=\d+ "
    r"requested_type=\d+ requested_usage=\d+ requested_access=\d+ "
    r"compat_type=\d+ compat_usage=\d+ compat_access=\d+ alignment=\d+ "
    r"buffer=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_HAL_TRACE_TOP_RE = re.compile(
    r"^MINIRESNET_HAL_TRACE_TOP: phase=invoke rank=\d+ seq=\d+ allocation_size=\d+ "
    r"requested_type=\d+ requested_usage=\d+ requested_access=\d+ "
    r"compat_type=\d+ compat_usage=\d+ compat_access=\d+ alignment=\d+ "
    r"buffer=\d+\s*$",
    re.MULTILINE,
)


def validate_miniresnet_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "MINIRESNET_FAIL" in output:
        errors.append("MINIRESNET_FAIL token present")

    pass_matches = list(MINIRESNET_PASS_RE.finditer(output))
    pass_count: int | None = None
    if not pass_matches:
        errors.append("missing MINIRESNET_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple MINIRESNET_PASS count lines")
    else:
        pass_count = int(pass_matches[0].group("count"))
        if pass_count <= 0:
            errors.append("MINIRESNET_PASS count must be positive")

    result_count = len(list(MINIRESNET_RESULT_RE.finditer(output)))
    if pass_count is not None and result_count != pass_count:
        errors.append(
            f"MINIRESNET_RESULT count {result_count} does not match pass count {pass_count}"
        )
    elif pass_count is None and result_count == 0:
        errors.append("missing MINIRESNET_RESULT lines")

    phases = {match.group("phase") for match in MINIRESNET_PEAK_RE.finditer(output)}
    for phase in ("init", "invoke"):
        if phase not in phases:
            errors.append(f"missing MINIRESNET_MODEL_PEAK phase={phase}")

    if not MINIRESNET_TRACE_PEAK_RE.search(output):
        errors.append("missing MINIRESNET_HEAP_TRACE_PEAK phase=invoke")

    if not MINIRESNET_TRACE_TOP_RE.search(output):
        errors.append("missing MINIRESNET_HEAP_TRACE_TOP phase=invoke")

    if not MINIRESNET_HAL_TRACE_PEAK_RE.search(output):
        errors.append("missing MINIRESNET_HAL_TRACE_PEAK phase=invoke")

    if not MINIRESNET_HAL_TRACE_TOP_RE.search(output):
        errors.append("missing MINIRESNET_HAL_TRACE_TOP phase=invoke")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/miniresnet_validation",
        help="Path to the MiniResNet validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/miniresnet_validation_qemu.log",
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
        print(f"MiniResNet validation error: kernel not found: {kernel}")
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
        print(f"MiniResNet validation error: QEMU exited with {proc.returncode}")
        return 1

    validation_errors = validate_miniresnet_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"MiniResNet validation error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
