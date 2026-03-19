#!/usr/bin/env python3

from pathlib import Path
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = REPO_ROOT / "build" / "kernel-semantics"
SOURCE_DIR = REPO_ROOT / "tests" / "kernel"
TEST_BIN = BUILD_DIR / "kernel_semantics_tests"


def run(cmd: list[str], *, expect_success: bool = True) -> int:
    print(f"$ {' '.join(cmd)}")
    proc = subprocess.run(cmd, cwd=REPO_ROOT)
    if expect_success and proc.returncode != 0:
        raise SystemExit(proc.returncode)
    return proc.returncode


def main() -> None:
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    run(["cmake", "-S", str(SOURCE_DIR), "-B", str(BUILD_DIR), "-G", "Ninja"])
    run(["cmake", "--build", str(BUILD_DIR)])

    print("Running harness canary...")
    run([str(TEST_BIN), "--suite", "canary"])

    print("Running core semantic suites...")
    run([str(TEST_BIN), "--suite", "core"])

    print("Kernel semantic validation passed.")


if __name__ == "__main__":
    main()
