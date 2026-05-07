#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys


EXPECTED_RUN_COUNT = 3
EXPECTED_EXPORTS = 23
EXPECTED_DISPATCHES = 33
EXPECTED_CONTINUATION_COMMAND_INDEX = 5
EXPECTED_INPUT_BYTES = 3200
EXPECTED_ARGMAX = 3
EXPECTED_HASH = 3045847227
EXPECTED_TOP_SCORE_Q = 99609
EXPECTED_OUTPUT_Q = [0, 391, 0, 99609, 0, 0, 0, 0, 0, 0]
EXPECTED_ARENA_BYTES = 142336
EXPECTED_ARENA_GUARD_BYTES = 128
EXPECTED_OUTPUT_BYTES = 40
EXPECTED_IO_SCRATCH_BYTES = 145704
EXPECTED_TASK_STACK_BYTES = 65536

PROBE_PASS_RE = re.compile(
    r"^MINIRESNET_PATCH_PROBE_PASS exports=(?P<exports>\d+) "
    r"dispatches=(?P<dispatches>\d+) "
    r"continuation_command_index=(?P<continuation>\d+)\s*$",
    re.MULTILINE,
)
RESULT_RE = re.compile(
    r"^MINIRESNET_PATCH_RESULT: run=(?P<run>\d+) sample=synthetic_pattern "
    r"argmax=(?P<argmax>\d+) hash=(?P<hash>\d+) top_score_q=(?P<top_score_q>-?\d+) "
    r"latency_ticks=\d+\s*$",
    re.MULTILINE,
)
STATIC_RAM_RE = re.compile(
    r"^MINIRESNET_PATCH_STATIC_RAM: input_bytes=(?P<input>\d+) "
    r"arena_bytes=(?P<arena>\d+) arena_guard_bytes=(?P<guards>\d+) "
    r"output_bytes=(?P<output>\d+) io_scratch_bytes=(?P<scratch>\d+) "
    r"task_stack_bytes=(?P<stack>\d+)\s*$",
    re.MULTILINE,
)
MODEL_PEAK_RE = re.compile(
    r"^MINIRESNET_PATCH_MODEL_PEAK: phase=(?P<phase>invoke) "
    r"heap_current_bytes=(?P<current>\d+) heap_peak_bytes=(?P<peak>\d+) "
    r"heap_alloc_count=(?P<allocs>\d+) heap_free_count=(?P<frees>\d+)\s*$",
    re.MULTILINE,
)
PASS_RE = re.compile(
    r"^MINIRESNET_PATCH_PASS count=(?P<count>\d+)\s*$", re.MULTILINE
)
ARENA_GUARD_RE = re.compile(
    r"^MINIRESNET_PATCH_ARENA_GUARD: run=(?P<run>\d+) "
    r"before=(?P<before>ok|fail) after=(?P<after>ok|fail)\s*$",
    re.MULTILINE,
)
OUTPUT_Q_RE = re.compile(
    r"^MINIRESNET_PATCH_OUTPUT_Q: run=(?P<run>\d+) "
    r"values_q=\[(?P<values>-?\d+(?:,-?\d+)*)\]\s*$",
    re.MULTILINE,
)


def _expected_runs() -> set[int]:
    return set(range(EXPECTED_RUN_COUNT))


def validate_miniresnet_patch_validation_output(output: str) -> list[str]:
    errors: list[str] = []

    if "MINIRESNET_PATCH_FAIL" in output:
        errors.append("MINIRESNET_PATCH_FAIL token present")

    probe_match = PROBE_PASS_RE.search(output)
    if not probe_match:
        errors.append("missing MINIRESNET_PATCH_PROBE_PASS line")
    else:
        expected_probe_fields = {
            "exports": EXPECTED_EXPORTS,
            "dispatches": EXPECTED_DISPATCHES,
            "continuation": EXPECTED_CONTINUATION_COMMAND_INDEX,
        }
        for field, expected in expected_probe_fields.items():
            actual = int(probe_match.group(field))
            if actual != expected:
                errors.append(
                    "MINIRESNET_PATCH_PROBE_PASS "
                    f"{field} must be {expected}, got {actual}"
                )

    result_matches = list(RESULT_RE.finditer(output))
    if not result_matches:
        errors.append("missing MINIRESNET_PATCH_RESULT line")
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
                    "unexpected MINIRESNET_PATCH_RESULT "
                    f"run={run} argmax={argmax} hash={output_hash} "
                    f"top_score_q={top_score_q}"
                )
        if set(result_runs) != _expected_runs():
            errors.append(
                f"MINIRESNET_PATCH_RESULT runs must be 0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in result_runs.values()):
            errors.append("duplicate MINIRESNET_PATCH_RESULT run line")

    output_q_matches = list(OUTPUT_Q_RE.finditer(output))
    if not output_q_matches:
        errors.append("missing MINIRESNET_PATCH_OUTPUT_Q line")
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
            elif values != EXPECTED_OUTPUT_Q:
                errors.append(
                    f"unexpected MINIRESNET_PATCH_OUTPUT_Q run={run} "
                    f"values={values}"
                )
        if set(output_q_runs) != _expected_runs():
            errors.append(
                f"MINIRESNET_PATCH_OUTPUT_Q runs must be 0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in output_q_runs.values()):
            errors.append("duplicate MINIRESNET_PATCH_OUTPUT_Q run line")

    guard_matches = list(ARENA_GUARD_RE.finditer(output))
    if not guard_matches:
        errors.append("missing MINIRESNET_PATCH_ARENA_GUARD line")
    else:
        guard_runs: dict[int, int] = {}
        for match in guard_matches:
            run = int(match.group("run"))
            guard_runs[run] = guard_runs.get(run, 0) + 1
            if match.group("before") != "ok" or match.group("after") != "ok":
                errors.append(f"arena guard failed for run={run}")
        if set(guard_runs) != _expected_runs():
            errors.append(
                f"MINIRESNET_PATCH_ARENA_GUARD runs must be 0..{EXPECTED_RUN_COUNT - 1}"
            )
        if any(count != 1 for count in guard_runs.values()):
            errors.append("duplicate MINIRESNET_PATCH_ARENA_GUARD run line")

    static_ram_match = STATIC_RAM_RE.search(output)
    if not static_ram_match:
        errors.append("missing MINIRESNET_PATCH_STATIC_RAM line")
    else:
        expected_static_ram_fields = {
            "input": EXPECTED_INPUT_BYTES,
            "arena": EXPECTED_ARENA_BYTES,
            "guards": EXPECTED_ARENA_GUARD_BYTES,
            "output": EXPECTED_OUTPUT_BYTES,
            "scratch": EXPECTED_IO_SCRATCH_BYTES,
            "stack": EXPECTED_TASK_STACK_BYTES,
        }
        for field, expected in expected_static_ram_fields.items():
            actual = int(static_ram_match.group(field))
            if actual != expected:
                errors.append(
                    "MINIRESNET_PATCH_STATIC_RAM "
                    f"{field} must be {expected}, got {actual}"
                )

    peak_matches = list(MODEL_PEAK_RE.finditer(output))
    if not peak_matches:
        errors.append("missing MINIRESNET_PATCH_MODEL_PEAK phase=invoke")
    elif len(peak_matches) > 1:
        errors.append("multiple MINIRESNET_PATCH_MODEL_PEAK phase=invoke lines")
    else:
        peak_match = peak_matches[0]
        expected_heap_fields = {
            "current": 0,
            "peak": 0,
            "allocs": 0,
            "frees": 0,
        }
        for field, expected in expected_heap_fields.items():
            actual = int(peak_match.group(field))
            if actual != expected:
                errors.append(
                    "MINIRESNET_PATCH_MODEL_PEAK "
                    f"{field} must be {expected}, got {actual}"
                )

    pass_matches = list(PASS_RE.finditer(output))
    if not pass_matches:
        errors.append("missing MINIRESNET_PATCH_PASS count line")
    elif len(pass_matches) > 1:
        errors.append("multiple MINIRESNET_PATCH_PASS count lines")
    elif int(pass_matches[0].group("count")) != EXPECTED_RUN_COUNT:
        errors.append(f"MINIRESNET_PATCH_PASS count must be {EXPECTED_RUN_COUNT}")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build-qemu-test/apps/mnist_app/miniresnet_patch_validation",
        help="Path to the MiniResNet patch validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/miniresnet_patch_validation_qemu_test.log",
        help="Path to save the captured QEMU log",
    )
    parser.add_argument("--timeout-seconds", type=int, default=30)
    parser.add_argument("--memory", default="64M")
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()

    if not kernel.exists():
        print(f"MiniResNet patch validation error: kernel not found: {kernel}")
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
        print(f"MiniResNet patch validation error: QEMU exited with {proc.returncode}")
        return 1

    validation_errors = validate_miniresnet_patch_validation_output(output)
    if validation_errors:
        for error in validation_errors:
            print(f"MiniResNet patch validation error: {error}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
