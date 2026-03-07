#!/usr/bin/env python3

import argparse
import re
from pathlib import Path
from typing import cast


DEFINE_RE = re.compile(r"^\s*#define\s+([A-Za-z0-9_]+)\s+(.+?)\s*$")
TOKEN_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")


def parse_task_affinity(spec: str) -> tuple[str, str]:
    task, sep, core = spec.partition(":")
    if not sep or not task or not core:
        raise ValueError(
            f"invalid affinity expectation '{spec}', expected TASK:Core0 or TASK:Core1"
        )

    normalized_core = core.strip().lower()
    if normalized_core not in {"core0", "core1", "cpu0", "cpu1"}:
        raise ValueError(
            f"invalid affinity core '{core}' in '{spec}', expected Core0/Core1"
        )

    return task.strip(), f"Core{normalized_core[-1]}"


def parse_expected_qspi_signature(spec: str) -> list[int]:
    parts = spec.replace(",", " ").split()
    if not parts:
        raise ValueError("empty QSPI signature specification")
    return [int(part, 0) for part in parts]


def parse_expected_qspi_signature_args(specs: list[str]) -> list[int]:
    words: list[int] = []
    for spec in specs:
        words.extend(parse_expected_qspi_signature(spec))
    return words


def affinity_marker_found(log_text: str, task_name: str, core_name: str) -> bool:
    task_pattern = re.escape(task_name)
    core_index = core_name[-1]
    core_pattern = rf"(?:Core|CPU|core|cpu){core_index}"
    patterns = [
        rf"\[SMP\].*?{task_pattern}.*?{core_pattern}",
        rf"\[AFFINITY\].*?{task_pattern}.*?{core_pattern}",
        rf"\b{task_pattern}\b.*?affinity.*?{core_pattern}",
        rf"\b{task_pattern}\b.*?(?:on|@|->)\s*{core_pattern}",
    ]
    return any(re.search(pattern, log_text) for pattern in patterns)


def normalize_expr(expr: str) -> str:
    expr = expr.split("/*", 1)[0].strip()
    expr = expr.split("//", 1)[0].strip()
    return expr


def strip_integer_suffixes(expr: str) -> str:
    def repl(match: re.Match[str]) -> str:
        return re.sub(r"[uUlL]+$", "", match.group(0))

    return re.sub(r"(?:0x[0-9a-fA-F]+|\d+)[uUlL]+", repl, expr)


def parse_defines(path: Path) -> dict[str, str]:
    data: dict[str, str] = {}
    for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        m = DEFINE_RE.match(line)
        if not m:
            continue
        name, expr = m.group(1), normalize_expr(m.group(2))
        if expr:
            data[name] = expr
    return data


def resolve_int(
    name: str, defs: dict[str, str], cache: dict[str, int], stack: set[str]
) -> int:
    if name in cache:
        return cache[name]
    if name in stack:
        raise ValueError(f"cyclic define: {name}")
    if name not in defs:
        raise ValueError(f"missing define: {name}")

    stack.add(name)
    expr = defs[name]

    if re.fullmatch(r"[0-9]+", expr) or re.fullmatch(r"0x[0-9a-fA-F]+", expr):
        value = int(expr, 0)
        cache[name] = value
        stack.remove(name)
        return value

    def repl(match: re.Match[str]) -> str:
        token = match.group(0)
        if token in defs:
            return str(resolve_int(token, defs, cache, stack))
        return token

    expanded = TOKEN_RE.sub(repl, expr)
    expanded = strip_integer_suffixes(expanded)
    if not re.fullmatch(r"[0-9xXa-fA-F_+\-*/()\s]+", expanded):
        raise ValueError(f"unsafe expression for {name}: {expanded}")

    raw_value = cast(object, eval(expanded, {"__builtins__": {}}, {}))
    if not isinstance(raw_value, (int, float)):
        raise ValueError(f"non-numeric expression for {name}: {expanded}")
    value = int(raw_value)
    cache[name] = value
    stack.remove(name)
    return value


def main() -> int:
    ap = argparse.ArgumentParser()
    _ = ap.add_argument("--log", required=True)
    _ = ap.add_argument("--expect-irq-model", default="CLIC")
    _ = ap.add_argument("--board-config", default="boards/be_u1000/board_config.h")
    _ = ap.add_argument("--expect-smp-online-count", type=int)
    _ = ap.add_argument("--require-smp-ipi", action="store_true")
    _ = ap.add_argument(
        "--expect-single-core-fallback",
        action="store_true",
        help="Require the single-core fallback task-map marker",
    )
    _ = ap.add_argument(
        "--expect-task-affinity",
        action="append",
        default=[],
        metavar="TASK:CORE",
        help="Require a task affinity marker such as worker0:Core0 or worker1:Core1",
    )
    _ = ap.add_argument(
        "--expect-qspi-signature",
        nargs="+",
        help="Require a QSPI1 sample line matching one or more hex words",
    )
    args = ap.parse_args()
    log_arg = cast(str, args.log)
    expect_irq_model = cast(str, args.expect_irq_model)
    board_config_arg = cast(str, args.board_config)
    expect_smp_online_count = cast(int | None, args.expect_smp_online_count)
    require_smp_ipi = cast(bool, args.require_smp_ipi)
    expect_single_core_fallback = cast(bool, args.expect_single_core_fallback)
    expect_task_affinity = cast(list[str], args.expect_task_affinity)
    expect_qspi_signature = cast(list[str] | None, args.expect_qspi_signature)

    path = Path(log_arg)
    board_config_path = Path(board_config_arg)
    if not path.exists():
        print(f"BOOT_LOG_CHECK_ERROR: log not found: {path}")
        return 2
    if not board_config_path.exists():
        print(f"BOOT_LOG_CHECK_ERROR: board config not found: {board_config_path}")
        return 2

    txt = path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[BOOT] RUN_MARKER:",
        f"[BOOT] IRQ model: {expect_irq_model}",
        "[CHK] CLINT mtime monotonic: OK",
        "[CHK] GPIO init: OK",
        "[CHK] SPI init: OK",
        "[CHK] I2C init: OK",
        "[BOOT] Starting scheduler...",
    ]
    ordered_tokens = [
        "[BOOT] RUN_MARKER:",
        f"[BOOT] IRQ model: {expect_irq_model}",
        "[CHK] CLINT mtime monotonic: OK",
        "[BOOT] Initializing kernel...",
        "[BOOT] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in txt:
            failures.append(f"missing token: {token}")

    fail_markers = re.findall(r"\[CHK\].*: FAIL", txt)
    if fail_markers:
        failures.append("found failing check markers in boot log")

    positions: list[int] = []
    for token in ordered_tokens:
        pos = txt.find(token)
        positions.append(pos)
    if any(p == -1 for p in positions):
        pass
    else:
        if positions != sorted(positions):
            failures.append("required boot tokens are out of order")

    run_marker = re.search(r"\[BOOT\] RUN_MARKER: 0x([0-9a-fA-F]+)", txt)
    if run_marker is None:
        failures.append("missing valid RUN_MARKER hex token")

    if expect_smp_online_count is not None:
        role_tokens = [
            "[SMP] role-plan: core0=boot core1=worker core2=reserved",
            "[SMP] role-plan: core0=control core1=worker core2=reserved",
        ]
        if not any(token in txt for token in role_tokens):
            failures.append(
                "missing token: [SMP] role-plan: core0=<boot|control> core1=worker core2=reserved"
            )

        online_token = f"[SMP] online-count: {expect_smp_online_count}"
        if online_token not in txt:
            failures.append(f"missing token: {online_token}")

        for cpu in range(1, expect_smp_online_count):
            token = f"[SMP] CPU{cpu} secondary online"
            if token not in txt:
                failures.append(f"missing token: {token}")

    if require_smp_ipi and "[SMP] CPU1 IPI reschedule" not in txt:
        failures.append("missing token: [SMP] CPU1 IPI reschedule")

    if expect_single_core_fallback:
        fallback_tokens = [
            "[BOOT] task-map: control+worker share single core",
            "[CTRL] single-core tick 0",
            "[WORK] single-core tick 0",
        ]
        for token in fallback_tokens:
            if token not in txt:
                failures.append(f"missing token: {token}")

    for affinity_spec in expect_task_affinity:
        try:
            task_name, core_name = parse_task_affinity(affinity_spec)
        except ValueError as exc:
            failures.append(str(exc))
            continue

        if not affinity_marker_found(txt, task_name, core_name):
            failures.append(
                f"missing affinity marker for task '{task_name}' on {core_name}"
            )

    mtime_line = re.search(
        r"\[CHK\] CLINT mtime monotonic: OK \(0x([0-9a-fA-F]+) -> 0x([0-9a-fA-F]+)\)",
        txt,
    )
    if mtime_line:
        start = int(mtime_line.group(1), 16)
        end = int(mtime_line.group(2), 16)
        if end <= start:
            failures.append("CLINT mtime monotonic check values are not increasing")
    else:
        failures.append("missing detailed CLINT monotonic line")

    try:
        defs = parse_defines(board_config_path)
        cache: dict[str, int] = {}
        expected_gpio_base = resolve_int("BE_U1000_DIAG_GPIO_BASE", defs, cache, set())
        expected_gpio_pin = resolve_int("BE_U1000_DIAG_GPIO_PIN", defs, cache, set())
        expected_spi_base = resolve_int("BE_U1000_DIAG_SPI_BASE", defs, cache, set())
        expected_spi_div = resolve_int("BE_U1000_DIAG_SPI_BAUD_DIV", defs, cache, set())
        expected_i2c_base = resolve_int("BE_U1000_DIAG_I2C_BASE", defs, cache, set())
        expected_i2c_hz = resolve_int("BE_U1000_DIAG_I2C_BUS_HZ", defs, cache, set())
    except ValueError as exc:
        failures.append(f"failed to resolve board config defines: {exc}")
        expected_gpio_base = 0
        expected_gpio_pin = 0
        expected_spi_base = 0
        expected_spi_div = 0
        expected_i2c_base = 0
        expected_i2c_hz = 0

    gpio_line = re.search(
        r"\[CHK\] GPIO init: OK \(base=0x([0-9a-fA-F]+) pin=(\d+)\)", txt
    )
    if gpio_line is None:
        failures.append("missing detailed GPIO init line")
    else:
        gpio_base = int(gpio_line.group(1), 16)
        gpio_pin = int(gpio_line.group(2), 10)
        if gpio_base != expected_gpio_base:
            failures.append(
                f"GPIO base mismatch: board=0x{expected_gpio_base:x} log=0x{gpio_base:x}"
            )
        if gpio_pin != expected_gpio_pin:
            failures.append(
                f"GPIO pin mismatch: board={expected_gpio_pin} log={gpio_pin}"
            )

    spi_line = re.search(
        r"\[CHK\] SPI init: OK \(base=0x([0-9a-fA-F]+) div=(\d+)\)", txt
    )
    if spi_line is None:
        failures.append("missing detailed SPI init line")
    else:
        spi_base = int(spi_line.group(1), 16)
        spi_div = int(spi_line.group(2), 10)
        if spi_base != expected_spi_base:
            failures.append(
                f"SPI base mismatch: board=0x{expected_spi_base:x} log=0x{spi_base:x}"
            )
        if spi_div != expected_spi_div:
            failures.append(
                f"SPI baud-div mismatch: board={expected_spi_div} log={spi_div}"
            )

    i2c_line = re.search(
        r"\[CHK\] I2C init: OK \(base=0x([0-9a-fA-F]+) hz=(\d+)\)", txt
    )
    if i2c_line is None:
        failures.append("missing detailed I2C init line")
    else:
        i2c_base = int(i2c_line.group(1), 16)
        i2c_hz = int(i2c_line.group(2), 10)
        if i2c_base != expected_i2c_base:
            failures.append(
                f"I2C base mismatch: board=0x{expected_i2c_base:x} log=0x{i2c_base:x}"
            )
        if i2c_hz != expected_i2c_hz:
            failures.append(
                f"I2C bus-hz mismatch: board={expected_i2c_hz} log={i2c_hz}"
            )

    if expect_qspi_signature is not None:
        try:
            expected_words = parse_expected_qspi_signature_args(expect_qspi_signature)
        except ValueError as exc:
            failures.append(str(exc))
        else:
            qspi_line = re.search(r"\[SELFTEST\] QSPI1 window sample: (.+)", txt)
            if qspi_line is None:
                failures.append("missing QSPI1 window sample line")
            else:
                actual_words = [
                    int(part, 16)
                    for part in re.findall(r"0x([0-9a-fA-F]+)", qspi_line.group(1))
                ]
                if len(actual_words) < len(expected_words):
                    failures.append(
                        f"QSPI sample too short: expected {len(expected_words)} words got {len(actual_words)}"
                    )
                elif actual_words[: len(expected_words)] != expected_words:
                    failures.append(
                        f"QSPI signature mismatch: expected {expected_words} got {actual_words[: len(expected_words)]}"
                    )

    if failures:
        print("BOOT_LOG_CHECK_FAILED")
        for f in failures:
            print(f"- {f}")
        return 1

    print("BOOT_LOG_CHECK_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
