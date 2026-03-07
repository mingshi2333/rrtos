#!/usr/bin/env python3

from __future__ import annotations

import argparse
from datetime import datetime, timezone
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build-be_u1000_smp_probe/rrtos_be_u1000.elf",
        help="Path to the experimental SMP kernel image",
    )
    parser.add_argument(
        "--binary",
        default="build-be_u1000_smp_probe/rrtos_be_u1000.bin",
        help="Path to the binary image loaded into TCMA for simulation",
    )
    parser.add_argument(
        "--log",
        default="logs/be_u1000_smp_boot.log",
        help="Path to save the captured Renode UART log",
    )
    parser.add_argument(
        "--sleep-seconds",
        type=int,
        default=4,
        help="How long the Renode script should run before quitting",
    )
    parser.add_argument(
        "--summary",
        default="logs/be_u1000_smp_runtime_probe.md",
        help="Path to save a markdown runtime summary",
    )
    parser.add_argument(
        "--check-bootlog",
        action="store_true",
        help="Run check_boot_log.py against the captured UART log",
    )
    parser.add_argument(
        "--expect-irq-model",
        default="CLIC",
        help="Expected IRQ model for optional boot log validation",
    )
    parser.add_argument(
        "--expect-smp-online-count",
        type=int,
        help="Expected SMP online count for optional boot log validation",
    )
    parser.add_argument(
        "--require-smp-ipi",
        action="store_true",
        help="Require the CPU1 IPI marker during optional boot log validation",
    )
    parser.add_argument(
        "--expect-single-core-fallback",
        action="store_true",
        help="Require the single-core fallback task-map marker during boot log validation",
    )
    parser.add_argument(
        "--expect-task-affinity",
        action="append",
        default=[],
        metavar="TASK:CORE",
        help="Require a task affinity marker during optional boot log validation",
    )
    parser.add_argument(
        "--expect-qspi-signature",
        nargs="+",
        help="Optional QSPI1 signature expectation for boot log validation",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parents[2]
    kernel = (repo_root / args.kernel).resolve()
    binary = (repo_root / args.binary).resolve()
    log_path = (repo_root / args.log).resolve()
    summary_path = (repo_root / args.summary).resolve()
    repl_path = (repo_root / "scripts/simulation/be_u1000.repl").resolve()

    if not kernel.exists():
        print(f"BE-U1000 SMP runtime error: kernel not found: {kernel}")
        return 2
    if not binary.exists():
        print(f"BE-U1000 SMP runtime error: binary not found: {binary}")
        return 2

    log_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    if log_path.exists():
        log_path.unlink()

    started = datetime.now(timezone.utc).isoformat()

    script = textwrap.dedent(
        f"""
        :name: be_u1000_smp_runtime
        :description: BE-U1000 SMP runtime probe

        $bin=@{kernel}
        $raw=@{binary}
        $log=@{log_path}

        mach create "be_u1000"
        machine LoadPlatformDescription @{repl_path}
        sysbus.uart0 CreateFileBackend $log
        sysbus LoadELF $bin
        sysbus LoadBinary $raw 0x40010000
        sysbus WriteDoubleWord 0x90000000 0x31505351
        sysbus WriteDoubleWord 0x90000004 0x5F4C444D
        sysbus WriteDoubleWord 0x90000008 0x00010010
        sysbus WriteDoubleWord 0x9000000C 0xA55A3CC3
        start
        sleep {args.sleep_seconds}
        quit
        """
    ).strip()

    with tempfile.NamedTemporaryFile("w", suffix=".resc", delete=False) as tmp:
        tmp.write(script)
        script_path = Path(tmp.name)

    try:
        proc = subprocess.run(
            ["renode", "--console", "--disable-xwt", str(script_path)],
            cwd=repo_root,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
    finally:
        script_path.unlink(missing_ok=True)

    sys.stdout.write(proc.stdout)

    status = "passed"
    first_error = "none"
    bootlog_check_status = "not-run"
    if proc.returncode != 0:
        status = f"renode-exit-{proc.returncode}"

    for line in proc.stdout.splitlines():
        stripped = line.strip()
        if (
            "CPU abort" in stripped
            or "Trying to execute code outside RAM or ROM" in stripped
        ):
            status = "cpu-abort"
            first_error = stripped
            break

    if proc.returncode != 0:
        summary = [
            "# BE-U1000 SMP Runtime Probe",
            "",
            f"- Started: {started}",
            f"- Finished: {datetime.now(timezone.utc).isoformat()}",
            f"- Status: `{status}`",
            f"- Kernel: `{kernel}`",
            f"- Binary: `{binary}`",
            f"- UART log: `{log_path}`",
            f"- Boot log check: `{bootlog_check_status}`",
            f"- First error: `{first_error}`",
            "",
            "This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.",
        ]
        summary_path.write_text("\n".join(summary), encoding="utf-8")
        print(f"BE-U1000 SMP runtime error: Renode exited with {proc.returncode}")
        return 1

    if not log_path.exists():
        first_error = "uart log not produced"
        summary = [
            "# BE-U1000 SMP Runtime Probe",
            "",
            f"- Started: {started}",
            f"- Finished: {datetime.now(timezone.utc).isoformat()}",
            f"- Status: `{status}`",
            f"- Kernel: `{kernel}`",
            f"- Binary: `{binary}`",
            f"- UART log: `{log_path}`",
            f"- Boot log check: `{bootlog_check_status}`",
            f"- First error: `{first_error}`",
            "",
            "This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.",
        ]
        summary_path.write_text("\n".join(summary), encoding="utf-8")
        print(f"BE-U1000 SMP runtime error: log not produced: {log_path}")
        return 1

    if args.check_bootlog:
        check_cmd = [
            sys.executable,
            str(repo_root / "scripts/be_u1000/check_boot_log.py"),
            "--log",
            str(log_path),
            "--expect-irq-model",
            args.expect_irq_model,
        ]
        if args.expect_smp_online_count is not None:
            check_cmd.extend(
                ["--expect-smp-online-count", str(args.expect_smp_online_count)]
            )
        if args.require_smp_ipi:
            check_cmd.append("--require-smp-ipi")
        if args.expect_single_core_fallback:
            check_cmd.append("--expect-single-core-fallback")
        for affinity_spec in args.expect_task_affinity:
            check_cmd.extend(["--expect-task-affinity", affinity_spec])
        if args.expect_qspi_signature:
            check_cmd.append("--expect-qspi-signature")
            check_cmd.extend(args.expect_qspi_signature)

        check_proc = subprocess.run(
            check_cmd,
            cwd=repo_root,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
        sys.stdout.write(check_proc.stdout)
        bootlog_check_status = (
            "passed"
            if check_proc.returncode == 0
            else f"failed-{check_proc.returncode}"
        )
        if check_proc.returncode != 0:
            status = f"bootlog-check-{check_proc.returncode}"
            first_error = "boot log validation failed"

    sys.stdout.write(log_path.read_text(encoding="utf-8", errors="ignore"))
    summary = [
        "# BE-U1000 SMP Runtime Probe",
        "",
        f"- Started: {started}",
        f"- Finished: {datetime.now(timezone.utc).isoformat()}",
        f"- Status: `{status}`",
        f"- Kernel: `{kernel}`",
        f"- Binary: `{binary}`",
        f"- UART log: `{log_path}`",
        f"- Boot log check: `{bootlog_check_status}`",
        f"- First error: `{first_error}`",
        "",
        "This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.",
    ]
    summary_path.write_text("\n".join(summary), encoding="utf-8")

    if status != "passed":
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
