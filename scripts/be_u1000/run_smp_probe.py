#!/usr/bin/env python3

import argparse
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path


def run_step(cmd, log_file):
    proc = subprocess.run(cmd, text=True, capture_output=True)
    with log_file.open("a", encoding="utf-8") as fh:
        fh.write(f"$ {' '.join(cmd)}\n")
        if proc.stdout:
            fh.write(proc.stdout)
            if not proc.stdout.endswith("\n"):
                fh.write("\n")
        if proc.stderr:
            fh.write(proc.stderr)
            if not proc.stderr.endswith("\n"):
                fh.write("\n")
        fh.write(f"[exit {proc.returncode}]\n\n")
    return proc


def extract_error(text):
    for line in text.splitlines():
        stripped = line.strip()
        if "error:" in stripped.lower() or "undefined symbol" in stripped.lower():
            return stripped
    return "none"


def main():
    parser = argparse.ArgumentParser(
        description="Run an experimental BE-U1000 SMP build probe."
    )
    parser.add_argument("--build-dir", default="build-be_u1000_smp_probe")
    parser.add_argument("--log", default="logs/be_u1000_smp_probe.log")
    parser.add_argument("--summary", default="logs/be_u1000_smp_probe.md")
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parents[2]
    build_dir = repo_root / args.build_dir
    log_path = repo_root / args.log
    summary_path = repo_root / args.summary
    log_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.parent.mkdir(parents=True, exist_ok=True)

    log_path.write_text("", encoding="utf-8")
    started = datetime.now(timezone.utc).isoformat()

    configure_cmd = [
        "cmake",
        "-B",
        str(build_dir),
        "-DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake",
        "-DARCH_BITS=32",
        "-DCONFIG_BOARD=be_u1000",
        "-DCMAKE_BUILD_TYPE=MinSizeRel",
        "-DOS_SMP_EN=ON",
    ]
    build_cmd = ["cmake", "--build", str(build_dir)]

    configure = run_step(configure_cmd, log_path)
    build = run_step(build_cmd, log_path) if configure.returncode == 0 else None

    status = "passed"
    rc = 0
    error_line = "none"
    if configure.returncode != 0:
        status = "failed-configure"
        rc = configure.returncode
        error_line = extract_error(configure.stdout + "\n" + configure.stderr)
    elif build and build.returncode != 0:
        status = "failed-build"
        rc = build.returncode
        error_line = extract_error(build.stdout + "\n" + build.stderr)

    summary = [
        "# BE-U1000 SMP Probe",
        "",
        f"- Started: {started}",
        f"- Finished: {datetime.now(timezone.utc).isoformat()}",
        f"- Status: `{status}`",
        f"- Build dir: `{build_dir}`",
        f"- Log: `{log_path}`",
        f"- First error: `{error_line}`",
        "",
        "This probe is experimental. A passing result does not promote SMP to supported status by itself; runtime validation is still required.",
    ]
    summary_path.write_text("\n".join(summary), encoding="utf-8")

    sys.exit(rc)


if __name__ == "__main__":
    main()
