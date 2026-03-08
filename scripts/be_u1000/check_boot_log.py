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
    _ = ap.add_argument(
        "--expect-qspi-offset",
        type=lambda value: int(value, 0),
        default=0,
        help="Expected flash window sample offset for boot log validation",
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
    expect_qspi_offset = cast(int, args.expect_qspi_offset)

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
        "[CHK] FLASH init: OK",
        "[CHK] FLASH read: OK",
        "[CHK] FLASH identify: OK",
        "[CHK] CANFD0 irq-arm: OK",
        "[CHK] CANFD0 init: OK",
        "[CHK] CANFD0 state: OK",
        "[CHK] CANFD0 path: OK",
        "[IRQ] CANFD0 external: OK",
        "[CHK] CANFD0 irq-fire: OK",
        "[CHK] CANFD0 loopback: OK",
        "[CHK] CANFD0 settle: OK",
        "[CHK] CANFD1 irq-arm: OK",
        "[CHK] CANFD1 init: OK",
        "[CHK] CANFD1 state: OK",
        "[CHK] CANFD1 path: OK",
        "[IRQ] CANFD1 external: OK",
        "[CHK] CANFD1 irq-fire: OK",
        "[CHK] CANFD1 loopback: OK",
        "[CHK] CANFD1 settle: OK",
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
        expected_led_gpio_base = resolve_int(
            "BE_U1000_USER_LED_GPIO_BASE", defs, cache, set()
        )
        expected_led_gpio_pin = resolve_int(
            "BE_U1000_USER_LED_GPIO_PIN", defs, cache, set()
        )
        expected_button_gpio_base = resolve_int(
            "BE_U1000_USER_BTN_GPIO_BASE", defs, cache, set()
        )
        expected_button_gpio_pin = resolve_int(
            "BE_U1000_USER_BTN_GPIO_PIN", defs, cache, set()
        )
        expected_spi_base = resolve_int("BE_U1000_DIAG_SPI_BASE", defs, cache, set())
        expected_spi_div = resolve_int("BE_U1000_DIAG_SPI_BAUD_DIV", defs, cache, set())
        expected_i2c_base = resolve_int("BE_U1000_DIAG_I2C_BASE", defs, cache, set())
        expected_i2c_hz = resolve_int("BE_U1000_DIAG_I2C_BUS_HZ", defs, cache, set())
        expected_flash_base = resolve_int("BE_U1000_QSPI1_BASE", defs, cache, set())
        expected_flash_size = resolve_int("BE_U1000_QSPI1_SIZE", defs, cache, set())
        expected_flash_jedec = resolve_int(
            "BE_U1000_DIAG_FLASH_JEDEC_ID", defs, cache, set()
        )
        expected_flash_page_size = resolve_int(
            "BE_U1000_DIAG_FLASH_PAGE_SIZE", defs, cache, set()
        )
        expected_flash_sector_size = resolve_int(
            "BE_U1000_DIAG_FLASH_SECTOR_SIZE", defs, cache, set()
        )
        expected_flash_capacity = resolve_int(
            "BE_U1000_DIAG_FLASH_CAPACITY_BYTES", defs, cache, set()
        )
        expected_canfd0_base = resolve_int(
            "BE_U1000_DIAG_CANFD0_BASE", defs, cache, set()
        )
        expected_canfd1_base = resolve_int(
            "BE_U1000_DIAG_CANFD1_BASE", defs, cache, set()
        )
        expected_canfd_bitrate = resolve_int(
            "BE_U1000_DIAG_CANFD_BITRATE", defs, cache, set()
        )
        expected_canfd_frame_len = resolve_int(
            "BE_U1000_DIAG_CANFD_FRAME_LEN", defs, cache, set()
        )
        expected_canfd0_id = resolve_int(
            "BE_U1000_DIAG_CANFD0_FRAME_ID", defs, cache, set()
        )
        expected_canfd1_id = resolve_int(
            "BE_U1000_DIAG_CANFD1_FRAME_ID", defs, cache, set()
        )
        expected_canfd0_irq = resolve_int("BE_U1000_IRQ_CANFD0", defs, cache, set())
        expected_canfd1_irq = resolve_int("BE_U1000_IRQ_CANFD1", defs, cache, set())
    except ValueError as exc:
        failures.append(f"failed to resolve board config defines: {exc}")
        expected_led_gpio_base = 0
        expected_led_gpio_pin = 0
        expected_button_gpio_base = 0
        expected_button_gpio_pin = 0
        expected_spi_base = 0
        expected_spi_div = 0
        expected_i2c_base = 0
        expected_i2c_hz = 0
        expected_flash_base = 0
        expected_flash_size = 0
        expected_flash_jedec = 0
        expected_flash_page_size = 0
        expected_flash_sector_size = 0
        expected_flash_capacity = 0
        expected_canfd0_base = 0
        expected_canfd1_base = 0
        expected_canfd_bitrate = 0
        expected_canfd_frame_len = 0
        expected_canfd0_id = 0
        expected_canfd1_id = 0
        expected_canfd0_irq = 0
        expected_canfd1_irq = 0

    gpio_lines = re.findall(
        r"\[CHK\] GPIO init: OK \(base=0x([0-9a-fA-F]+) pin=(\d+)\)", txt
    )
    if not gpio_lines:
        failures.append("missing detailed GPIO init line")
    else:
        actual_gpio_pairs = {(int(base, 16), int(pin, 10)) for base, pin in gpio_lines}
        expected_gpio_pairs = {
            (expected_led_gpio_base, expected_led_gpio_pin),
            (expected_button_gpio_base, expected_button_gpio_pin),
        }
        for expected_base, expected_pin in expected_gpio_pairs:
            if (expected_base, expected_pin) not in actual_gpio_pairs:
                failures.append(
                    f"missing GPIO init evidence for base=0x{expected_base:x} pin={expected_pin}"
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

    flash_init_line = re.search(
        r"\[CHK\] FLASH init: OK \(base=0x([0-9a-fA-F]+) size=0x([0-9a-fA-F]+)\)",
        txt,
    )
    if flash_init_line is None:
        failures.append("missing detailed FLASH init line")
    else:
        flash_base = int(flash_init_line.group(1), 16)
        flash_size = int(flash_init_line.group(2), 16)
        if flash_base != expected_flash_base:
            failures.append(
                f"FLASH base mismatch: board=0x{expected_flash_base:x} log=0x{flash_base:x}"
            )
        if flash_size != expected_flash_size:
            failures.append(
                f"FLASH size mismatch: board=0x{expected_flash_size:x} log=0x{flash_size:x}"
            )

    flash_read_line = re.search(
        r"\[CHK\] FLASH read: OK \(offset=0x([0-9a-fA-F]+) len=(\d+)\)",
        txt,
    )
    if flash_read_line is None:
        failures.append("missing detailed FLASH read line")
    else:
        flash_offset = int(flash_read_line.group(1), 16)
        flash_len = int(flash_read_line.group(2), 10)
        expected_flash_len = 16
        if expect_qspi_signature is not None:
            try:
                expected_flash_len = (
                    len(parse_expected_qspi_signature_args(expect_qspi_signature)) * 4
                )
            except ValueError as exc:
                failures.append(str(exc))
        if flash_offset != expect_qspi_offset:
            failures.append(
                f"FLASH read offset mismatch: expected {expect_qspi_offset} got {flash_offset}"
            )
        if flash_len != expected_flash_len:
            failures.append(
                f"FLASH read length mismatch: expected {expected_flash_len} got {flash_len}"
            )

    flash_identify_line = re.search(
        r"\[CHK\] FLASH identify: OK \(jedec=0x([0-9a-fA-F]+) page=(\d+) sector=(\d+) size=0x([0-9a-fA-F]+)\)",
        txt,
    )
    if flash_identify_line is None:
        failures.append("missing detailed FLASH identify line")
    else:
        flash_jedec = int(flash_identify_line.group(1), 16)
        flash_page_size = int(flash_identify_line.group(2), 10)
        flash_sector_size = int(flash_identify_line.group(3), 10)
        flash_capacity = int(flash_identify_line.group(4), 16)
        if flash_jedec != expected_flash_jedec:
            failures.append(
                f"FLASH jedec mismatch: board=0x{expected_flash_jedec:x} log=0x{flash_jedec:x}"
            )
        if flash_page_size != expected_flash_page_size:
            failures.append(
                f"FLASH page-size mismatch: board={expected_flash_page_size} log={flash_page_size}"
            )
        if flash_sector_size != expected_flash_sector_size:
            failures.append(
                f"FLASH sector-size mismatch: board={expected_flash_sector_size} log={flash_sector_size}"
            )
        if flash_capacity != expected_flash_capacity:
            failures.append(
                f"FLASH capacity mismatch: board=0x{expected_flash_capacity:x} log=0x{flash_capacity:x}"
            )

    for controller_name, expected_base, expected_id, expected_irq_num in [
        ("CANFD0", expected_canfd0_base, expected_canfd0_id, expected_canfd0_irq),
        ("CANFD1", expected_canfd1_base, expected_canfd1_id, expected_canfd1_irq),
    ]:
        irq_arm_line = re.search(
            rf"\[CHK\] {controller_name} irq-arm: OK \(irq=(\d+)\)",
            txt,
        )
        if irq_arm_line is None:
            failures.append(f"missing detailed {controller_name} irq-arm line")
        else:
            irq_arm = int(irq_arm_line.group(1), 10)
            if irq_arm != expected_irq_num:
                failures.append(
                    f"{controller_name} irq-arm mismatch: board={expected_irq_num} log={irq_arm}"
                )

        canfd_init_line = re.search(
            rf"\[CHK\] {controller_name} init: OK \(base=0x([0-9a-fA-F]+) bitrate=(\d+) loopback=(\d+)\)",
            txt,
        )
        if canfd_init_line is None:
            failures.append(f"missing detailed {controller_name} init line")
        else:
            canfd_base = int(canfd_init_line.group(1), 16)
            canfd_bitrate = int(canfd_init_line.group(2), 10)
            canfd_loopback = int(canfd_init_line.group(3), 10)
            if canfd_base != expected_base:
                failures.append(
                    f"{controller_name} base mismatch: board=0x{expected_base:x} log=0x{canfd_base:x}"
                )
            if canfd_bitrate != expected_canfd_bitrate:
                failures.append(
                    f"{controller_name} bitrate mismatch: board={expected_canfd_bitrate} log={canfd_bitrate}"
                )
            if canfd_loopback != 1:
                failures.append(
                    f"{controller_name} loopback flag mismatch: expected 1 got {canfd_loopback}"
                )

        for stage_name, expected_irqs, expected_txflr, expected_rxflr in [
            ("state", {0}, 0, 0),
            ("path", {0, 3}, 0, 1),
            ("settle", {0}, 0, 0),
        ]:
            canfd_state_line = re.search(
                rf"\[CHK\] {controller_name} {stage_name}: OK \(status=0x([0-9a-fA-F]+) irq=0x([0-9a-fA-F]+) err=0x([0-9a-fA-F]+) txflr=(\d+) rxflr=(\d+)\)",
                txt,
            )
            if canfd_state_line is None:
                failures.append(f"missing detailed {controller_name} {stage_name} line")
            else:
                canfd_status = int(canfd_state_line.group(1), 16)
                canfd_irq = int(canfd_state_line.group(2), 16)
                canfd_err = int(canfd_state_line.group(3), 16)
                canfd_txflr = int(canfd_state_line.group(4), 10)
                canfd_rxflr = int(canfd_state_line.group(5), 10)
                if canfd_status == 0:
                    failures.append(
                        f"{controller_name} {stage_name} status must be non-zero"
                    )
                if canfd_irq not in expected_irqs:
                    failures.append(
                        f"{controller_name} {stage_name} irq mismatch: expected one of {sorted(expected_irqs)} got {canfd_irq}"
                    )
                if canfd_err != 0:
                    failures.append(
                        f"{controller_name} {stage_name} error mismatch: expected 0 got {canfd_err}"
                    )
                if canfd_txflr != expected_txflr:
                    failures.append(
                        f"{controller_name} {stage_name} txflr mismatch: expected {expected_txflr} got {canfd_txflr}"
                    )
                if canfd_rxflr != expected_rxflr:
                    failures.append(
                        f"{controller_name} {stage_name} rxflr mismatch: expected {expected_rxflr} got {canfd_rxflr}"
                    )

        irq_external_line = re.search(
            rf"\[IRQ\] {controller_name} external: OK \(irq=(\d+) count=(\d+)\)",
            txt,
        )
        if irq_external_line is None:
            failures.append(f"missing detailed {controller_name} external irq line")
        else:
            irq_external_num = int(irq_external_line.group(1), 10)
            irq_external_count = int(irq_external_line.group(2), 10)
            if irq_external_num != expected_irq_num:
                failures.append(
                    f"{controller_name} external irq mismatch: board={expected_irq_num} log={irq_external_num}"
                )
            if irq_external_count < 1:
                failures.append(
                    f"{controller_name} external irq count mismatch: expected >=1 got {irq_external_count}"
                )

        irq_fire_line = re.search(
            rf"\[CHK\] {controller_name} irq-fire: OK \(irq=(\d+) count=(\d+)\)",
            txt,
        )
        if irq_fire_line is None:
            failures.append(f"missing detailed {controller_name} irq-fire line")
        else:
            irq_fire_num = int(irq_fire_line.group(1), 10)
            irq_fire_count = int(irq_fire_line.group(2), 10)
            if irq_fire_num != expected_irq_num:
                failures.append(
                    f"{controller_name} irq-fire mismatch: board={expected_irq_num} log={irq_fire_num}"
                )
            if irq_fire_count < 1:
                failures.append(
                    f"{controller_name} irq-fire count mismatch: expected >=1 got {irq_fire_count}"
                )

        canfd_loopback_line = re.search(
            rf"\[CHK\] {controller_name} loopback: OK \(id=0x([0-9a-fA-F]+) len=(\d+)\)",
            txt,
        )
        if canfd_loopback_line is None:
            failures.append(f"missing detailed {controller_name} loopback line")
        else:
            canfd_id = int(canfd_loopback_line.group(1), 16)
            canfd_len = int(canfd_loopback_line.group(2), 10)
            if canfd_id != expected_id:
                failures.append(
                    f"{controller_name} frame-id mismatch: board=0x{expected_id:x} log=0x{canfd_id:x}"
                )
            if canfd_len != expected_canfd_frame_len:
                failures.append(
                    f"{controller_name} frame-len mismatch: board={expected_canfd_frame_len} log={canfd_len}"
                )

    if expect_qspi_signature is not None:
        try:
            expected_words = parse_expected_qspi_signature_args(expect_qspi_signature)
        except ValueError as exc:
            failures.append(str(exc))
        else:
            qspi_line = re.search(r"\[SELFTEST\] .+ window sample: (.+)", txt)
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
