#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


EXPECTED_RUN_COUNT = 3
EXPECTED_ARGMAX = 3
EXPECTED_HASH = 3045847227
EXPECTED_TOP_SCORE_Q = 99609

MINIRESNET_STATIC_DIRECT_PROBE_PASS_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_PROBE_PASS exports=\d+ dispatches=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_STATIC_DIRECT_RESULT_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_RESULT: run=(?P<run>\d+) sample=synthetic_pattern "
    r"argmax=(?P<argmax>\d+) hash=(?P<hash>\d+) top_score_q=(?P<top_score_q>-?\d+) "
    r"latency_ticks=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_STATIC_DIRECT_OUTPUT_Q_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_OUTPUT_Q: run=(?P<run>\d+) "
    r"values_q=\[(?P<values>-?\d+(?:,-?\d+)*)\]\s*$",
    re.MULTILINE,
)
MINIRESNET_STATIC_DIRECT_STATIC_RAM_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_STATIC_RAM: input_bytes=\d+ arena_bytes=\d+ "
    r"arena_guard_bytes=\d+ "
    r"output_bytes=\d+ io_scratch_bytes=\d+ task_stack_bytes=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_STATIC_DIRECT_MODEL_PEAK_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_MODEL_PEAK: phase=(?P<phase>invoke) "
    r"heap_current_bytes=\d+ heap_peak_bytes=\d+ heap_alloc_count=\d+ "
    r"heap_free_count=\d+\s*$",
    re.MULTILINE,
)
MINIRESNET_STATIC_DIRECT_PASS_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_PASS count=(?P<count>\d+)\s*$", re.MULTILINE
)
MINIRESNET_STATIC_DIRECT_ARENA_GUARD_RE = re.compile(
    r"^MINIRESNET_STATIC_DIRECT_ARENA_GUARD: run=(?P<run>\d+) "
    r"before=(?P<before>ok|fail) after=(?P<after>ok|fail)\s*$",
    re.MULTILINE,
)


def _expected_runs() -> set[int]:
    return set(range(EXPECTED_RUN_COUNT))


def validate_miniresnet_static_direct_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "MINIRESNET_STATIC_DIRECT_FAIL" in output:
        errors.append("MINIRESNET_STATIC_DIRECT_FAIL token present")

    if not MINIRESNET_STATIC_DIRECT_PROBE_PASS_RE.search(output):
        errors.append("missing MINIRESNET_STATIC_DIRECT_PROBE_PASS line")

    result_matches = list(MINIRESNET_STATIC_DIRECT_RESULT_RE.finditer(output))
    if not result_matches:
        errors.append("missing MINIRESNET_STATIC_DIRECT_RESULT line")
    else:
        result_runs: dict[int, int] = {}
        for match in result_matches:
            run = int(match.group("run"))
            result_runs[run] = result_runs.get(run, 0) + 1
            argmax = int(match.group("argmax"))
            output_hash = int(match.group("hash"))
            top_score_q = int(match.group("top_score_q"))
            if (
                argmax != EXPECTED_ARGMAX
                or output_hash != EXPECTED_HASH
                or top_score_q != EXPECTED_TOP_SCORE_Q
            ):
                errors.append(
                    "unexpected MINIRESNET_STATIC_DIRECT_RESULT "
                    f"run={run} argmax={argmax} hash={output_hash} top_score_q={top_score_q}"
                )
        if set(result_runs) != _expected_runs():
            errors.append(
                "MINIRESNET_STATIC_DIRECT_RESULT runs must be "
                f"0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in result_runs.values()):
            errors.append("duplicate MINIRESNET_STATIC_DIRECT_RESULT run line")

    guard_matches = list(MINIRESNET_STATIC_DIRECT_ARENA_GUARD_RE.finditer(output))
    if not guard_matches:
        errors.append("missing MINIRESNET_STATIC_DIRECT_ARENA_GUARD line")
    else:
        guard_runs: dict[int, int] = {}
        for match in guard_matches:
            run = int(match.group("run"))
            guard_runs[run] = guard_runs.get(run, 0) + 1
            if match.group("before") != "ok" or match.group("after") != "ok":
                errors.append(f"arena guard failed for run={run}")
        if set(guard_runs) != _expected_runs():
            errors.append(
                "MINIRESNET_STATIC_DIRECT_ARENA_GUARD runs must be "
                f"0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in guard_runs.values()):
            errors.append("duplicate MINIRESNET_STATIC_DIRECT_ARENA_GUARD run line")

    if not MINIRESNET_STATIC_DIRECT_STATIC_RAM_RE.search(output):
        errors.append("missing MINIRESNET_STATIC_DIRECT_STATIC_RAM line")

    output_q_matches = list(MINIRESNET_STATIC_DIRECT_OUTPUT_Q_RE.finditer(output))
    if not output_q_matches:
        errors.append("missing MINIRESNET_STATIC_DIRECT_OUTPUT_Q line")
    else:
        output_q_runs: dict[int, int] = {}
        for match in output_q_matches:
            run = int(match.group("run"))
            output_q_runs[run] = output_q_runs.get(run, 0) + 1
            values = [int(value) for value in match.group("values").split(",")]
            if len(values) != 10:
                errors.append(f"output vector for run={run} must contain 10 values")
            elif max(range(len(values)), key=lambda index: values[index]) != EXPECTED_ARGMAX:
                errors.append(f"output vector argmax mismatch for run={run}")
        if set(output_q_runs) != _expected_runs():
            errors.append(
                "MINIRESNET_STATIC_DIRECT_OUTPUT_Q runs must be "
                f"0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in output_q_runs.values()):
            errors.append("duplicate MINIRESNET_STATIC_DIRECT_OUTPUT_Q run line")

    phases = {
        match.group("phase")
        for match in MINIRESNET_STATIC_DIRECT_MODEL_PEAK_RE.finditer(output)
    }
    if "invoke" not in phases:
        errors.append("missing MINIRESNET_STATIC_DIRECT_MODEL_PEAK phase=invoke")

    pass_matches = list(MINIRESNET_STATIC_DIRECT_PASS_RE.finditer(output))
    if not pass_matches:
        errors.append("missing MINIRESNET_STATIC_DIRECT_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple MINIRESNET_STATIC_DIRECT_PASS count lines")
    elif int(pass_matches[0].group("count")) != EXPECTED_RUN_COUNT:
        errors.append(
            f"MINIRESNET_STATIC_DIRECT_PASS count must be {EXPECTED_RUN_COUNT}"
        )

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build-qemu-test/apps/mnist_app/miniresnet_static_direct_validation",
        help="Path to the MiniResNet static-direct validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/miniresnet_static_direct_validation_qemu_test.log",
        help="Path to save the captured QEMU log",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=int,
        default=30,
        help="Timeout for the QEMU run",
    )
    parser.add_argument(
        "--memory",
        default="64M",
        help="QEMU RAM size",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()

    if not kernel.exists():
        print(f"MiniResNet static-direct validation error: kernel not found: {kernel}")
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
        print(
            "MiniResNet static-direct validation error: "
            f"QEMU exited with {proc.returncode}"
        )
        return 1

    validation_errors = validate_miniresnet_static_direct_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"MiniResNet static-direct validation error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
