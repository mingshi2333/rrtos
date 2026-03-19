#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--kernel",
        default="build/apps/mnist_app/mnist_validation",
        help="Path to the validation kernel image",
    )
    parser.add_argument(
        "--log",
        default="logs/mnist_validation_renode.log",
        help="Path to save the captured Renode UART log",
    )
    parser.add_argument(
        "--sleep-seconds",
        type=int,
        default=10,
        help="How long the Renode script should run before quitting",
    )
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    kernel = (repo_root / args.kernel).resolve()
    log_path = (repo_root / args.log).resolve()
    repl_path = (repo_root / "scripts/simulation/riscv32_virt.repl").resolve()

    if not kernel.exists():
        print(f"AI Renode validation error: kernel not found: {kernel}")
        return 2

    log_path.parent.mkdir(parents=True, exist_ok=True)
    if log_path.exists():
        log_path.unlink()

    script = textwrap.dedent(
        f"""
        :name: mnist_validation_virt_rv32
        :description: RISC-V 32-bit virt machine for mnist validation

        $bin=@{kernel}
        $log=@{log_path}

        mach create \"riscv32-virt\"
        machine LoadPlatformDescription @{repl_path}
        sysbus.uart0 CreateFileBackend $log
        sysbus LoadELF $bin
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

    if proc.returncode != 0:
        print(f"AI Renode validation error: Renode exited with {proc.returncode}")
        return 1

    if not log_path.exists():
        print(f"AI Renode validation error: log not produced: {log_path}")
        return 1

    log_text = log_path.read_text(encoding="utf-8", errors="ignore")
    sys.stdout.write(log_text)

    if "AI_VALIDATION_PASS" not in log_text:
        print("AI Renode validation error: missing AI_VALIDATION_PASS token")
        return 1

    if "AI_VALIDATION_METRICS:" not in log_text:
        print("AI Renode validation error: missing AI_VALIDATION_METRICS token")
        return 1

    if "OS_TIMER_CALLBACK_PASS" not in log_text:
        print("AI Renode validation error: missing OS_TIMER_CALLBACK_PASS token")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
