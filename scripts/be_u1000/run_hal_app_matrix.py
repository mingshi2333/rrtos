#!/usr/bin/env python3

from __future__ import annotations

import argparse
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
import subprocess
import sys
import tempfile
import textwrap


@dataclass(frozen=True)
class HalApp:
    name: str
    checker: tuple[str, ...]


HAL_APPS: tuple[HalApp, ...] = (
    HalApp(
        "demo",
        (
            "scripts/be_u1000/check_boot_log.py",
            "--expect-irq-model",
            "CLIC",
            "--expect-single-core-fallback",
            "--expect-qspi-signature",
            "0x31505351",
            "0x5F4C444D",
            "0x00010010",
            "0xA55A3CC3",
        ),
    ),
    HalApp("gpio_ledblink", ("scripts/be_u1000/check_gpio_ledblink_log.py",)),
    HalApp("gpio_inputpolling", ("scripts/be_u1000/check_gpio_inputpolling_log.py",)),
    HalApp("uart_printf", ("scripts/be_u1000/check_uart_printf_log.py",)),
    HalApp("canfd_polling", ("scripts/be_u1000/check_canfd_polling_log.py",)),
    HalApp("qspi_flash", ("scripts/be_u1000/check_qspi_flash_log.py",)),
    HalApp("i2c_polling", ("scripts/be_u1000/check_i2c_polling_log.py",)),
    HalApp("tim_timebase", ("scripts/be_u1000/check_tim_timebase_log.py",)),
    HalApp("adc_single", ("scripts/be_u1000/check_adc_single_log.py",)),
    HalApp("wdt_heartbeat", ("scripts/be_u1000/check_wdt_heartbeat_log.py",)),
    HalApp("pwmg_outputcompare", ("scripts/be_u1000/check_pwmg_outputcompare_log.py",)),
    HalApp("dma_mem2mem", ("scripts/be_u1000/check_dma_mem2mem_log.py",)),
    HalApp("i2s_tx", ("scripts/be_u1000/check_i2s_tx_log.py",)),
    HalApp("pwma_timebase", ("scripts/be_u1000/check_pwma_timebase_log.py",)),
    HalApp("usb_runtime", ("scripts/be_u1000/check_usb_runtime_log.py",)),
)


@dataclass
class Result:
    app: str
    build: str
    run: str
    check: str
    log: str
    error: str

    @property
    def ok(self) -> bool:
        return self.build == "PASS" and self.run in ("PASS", "SKIP") and self.check in (
            "PASS",
            "SKIP",
        )


def display(path: Path, repo_root: Path) -> str:
    try:
        return str(path.relative_to(repo_root))
    except ValueError:
        return str(path)


def run_cmd(cmd: list[str], repo_root: Path) -> tuple[int, str]:
    proc = subprocess.run(
        cmd,
        cwd=repo_root,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )
    return proc.returncode, proc.stdout


def write_process_log(path: Path, cmd: list[str], output: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("$ " + " ".join(cmd) + "\n\n" + output, encoding="utf-8")


def configure_and_build(app: str, build_dir: Path, repo_root: Path) -> tuple[str, str]:
    configure = [
        "cmake",
        "-B",
        str(build_dir),
        "-DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake",
        "-DARCH_BITS=32",
        "-DCONFIG_BOARD=be_u1000",
        "-DCMAKE_BUILD_TYPE=MinSizeRel",
        "-DOS_SMP_EN=OFF",
        "-DOS_AI_EN=OFF",
        f"-DBE_U1000_APP={app}",
    ]
    rc, output = run_cmd(configure, repo_root)
    write_process_log(repo_root / "logs" / "be_u1000_hal" / f"{app}_configure.log", configure, output)
    if rc != 0:
        return "FAIL", f"configure exited {rc}"

    build = ["cmake", "--build", str(build_dir)]
    rc, output = run_cmd(build, repo_root)
    write_process_log(repo_root / "logs" / "be_u1000_hal" / f"{app}_build.log", build, output)
    if rc != 0:
        return "FAIL", f"build exited {rc}"
    return "PASS", ""


def renode_run(app: str, build_dir: Path, log_path: Path, sleep_seconds: int, repo_root: Path) -> tuple[str, str]:
    elf = build_dir / "rrtos_be_u1000.elf"
    binary = build_dir / "rrtos_be_u1000.bin"
    repl = repo_root / "scripts" / "simulation" / "be_u1000.repl"
    if not elf.exists():
        return "FAIL", f"missing image: {display(elf, repo_root)}"
    if not binary.exists():
        return "FAIL", f"missing binary: {display(binary, repo_root)}"

    log_path.parent.mkdir(parents=True, exist_ok=True)
    if log_path.exists():
        log_path.unlink()

    script = textwrap.dedent(
        f"""
        :name: be_u1000_hal_{app}
        :description: BE-U1000 HAL app validation for {app}

        $bin=@{elf}
        $raw=@{binary}
        $log=@{log_path}

        mach create "be_u1000"
        machine LoadPlatformDescription @{repl}
        sysbus.uart0 CreateFileBackend $log
        sysbus LoadELF $bin
        sysbus LoadBinary $raw 0x40010000
        sysbus WriteDoubleWord 0x90000000 0x31505351
        sysbus WriteDoubleWord 0x90000004 0x5F4C444D
        sysbus WriteDoubleWord 0x90000008 0x00010010
        sysbus WriteDoubleWord 0x9000000C 0xA55A3CC3
        sysbus WriteDoubleWord 0x90000010 0x00EF4018
        sysbus WriteDoubleWord 0x90000014 0x00000100
        sysbus WriteDoubleWord 0x90000018 0x00001000
        sysbus WriteDoubleWord 0x9000001C 0x01000000
        start
        sleep {sleep_seconds}
        quit
        """
    ).strip()

    with tempfile.NamedTemporaryFile("w", suffix=".resc", delete=False) as tmp:
        tmp.write(script)
        script_path = Path(tmp.name)

    cmd = ["renode", "--console", "--disable-xwt", str(script_path)]
    try:
        rc, output = run_cmd(cmd, repo_root)
    finally:
        script_path.unlink(missing_ok=True)

    write_process_log(repo_root / "logs" / "be_u1000_hal" / f"{app}_renode.log", cmd, output)
    if rc != 0:
        return "FAIL", f"renode exited {rc}"
    if not log_path.exists():
        return "FAIL", f"missing UART log: {display(log_path, repo_root)}"
    return "PASS", ""


def run_checker(app: HalApp, log_path: Path, repo_root: Path) -> tuple[str, str]:
    checker_path = repo_root / app.checker[0]
    if not checker_path.exists():
        return "FAIL", f"missing checker: {app.checker[0]}"

    cmd = [sys.executable, str(checker_path), "--log", str(log_path), *app.checker[1:]]
    rc, output = run_cmd(cmd, repo_root)
    write_process_log(repo_root / "logs" / "be_u1000_hal" / f"{app.name}_check.log", cmd, output)
    sys.stdout.write(output)
    if rc != 0:
        return "FAIL", f"checker exited {rc}"
    return "PASS", ""


def write_summary(results: list[Result], summary_path: Path, repo_root: Path) -> None:
    lines = [
        "# BE-U1000 HAL App Matrix",
        "",
        f"- Generated: {datetime.now(timezone.utc).isoformat()}",
        "",
        "| App | Build | Run | Check | UART log | Error |",
        "| --- | --- | --- | --- | --- | --- |",
    ]
    for result in results:
        lines.append(
            f"| `{result.app}` | {result.build} | {result.run} | {result.check} | "
            f"`{result.log}` | {result.error or 'none'} |"
        )
    lines.extend(
        [
            "",
            "The matrix builds each BE-U1000 HAL example as a separate `BE_U1000_APP` "
            "configuration, runs it under Renode unless disabled, and validates the "
            "captured UART log with the app-specific checker.",
        ]
    )
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text("\n".join(lines), encoding="utf-8")
    print(f"HAL_MATRIX_SUMMARY: {display(summary_path, repo_root)}")


def select_apps(names: list[str]) -> list[HalApp]:
    by_name = {app.name: app for app in HAL_APPS}
    if not names:
        return list(HAL_APPS)
    selected: list[HalApp] = []
    for name in names:
        if name not in by_name:
            valid = ", ".join(sorted(by_name))
            raise SystemExit(f"unknown app '{name}', valid apps: {valid}")
        selected.append(by_name[name])
    return selected


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apps", nargs="*", default=[], help="Subset of BE_U1000_APP names to validate")
    parser.add_argument("--build-root", default="build-be_u1000-hal", help="Root directory for per-app builds")
    parser.add_argument("--log-dir", default="logs/be_u1000_hal", help="Directory for UART and process logs")
    parser.add_argument("--summary", default="logs/be_u1000_hal_matrix.md", help="Markdown summary output")
    parser.add_argument("--sleep-seconds", type=int, default=4, help="Renode runtime per app")
    parser.add_argument("--build-only", action="store_true", help="Only configure and build each app")
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parents[2]
    build_root = (repo_root / args.build_root).resolve()
    log_dir = (repo_root / args.log_dir).resolve()
    summary_path = (repo_root / args.summary).resolve()
    apps = select_apps(args.apps)
    results: list[Result] = []

    for app in apps:
        print(f"HAL_MATRIX_APP_BEGIN: {app.name}")
        build_dir = build_root / app.name
        log_path = log_dir / f"{app.name}.uart.log"
        build_status, error = configure_and_build(app.name, build_dir, repo_root)
        run_status = "SKIP"
        check_status = "SKIP"

        if build_status == "PASS" and not args.build_only:
            run_status, error = renode_run(app.name, build_dir, log_path, args.sleep_seconds, repo_root)
            if run_status == "PASS":
                check_status, error = run_checker(app, log_path, repo_root)

        result = Result(
            app=app.name,
            build=build_status,
            run=run_status,
            check=check_status,
            log=display(log_path, repo_root),
            error=error,
        )
        results.append(result)
        print(
            f"HAL_MATRIX_APP_RESULT: app={result.app} build={result.build} "
            f"run={result.run} check={result.check} error={result.error or 'none'}"
        )

    write_summary(results, summary_path, repo_root)
    if all(result.ok for result in results):
        print(f"HAL_MATRIX_PASS count={len(results)}")
        return 0
    print(f"HAL_MATRIX_FAIL count={sum(not result.ok for result in results)}")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
