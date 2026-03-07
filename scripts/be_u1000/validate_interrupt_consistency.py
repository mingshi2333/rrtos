#!/usr/bin/env python3

import argparse
import re
from pathlib import Path
from typing import Any, cast


DEFINE_RE = re.compile(r"^\s*#define\s+([A-Za-z0-9_]+)\s+(.+?)\s*$")
TOKEN_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
SDK_CLIC_IRQ_RE = re.compile(r"\b(CLIC_[A-Za-z0-9_]+_IRQn)\s*=\s*(\d+)")


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

    value = int(cast(Any, eval(expanded, {"__builtins__": {}}, {})))
    cache[name] = value
    stack.remove(name)
    return value


def parse_repl(path: Path) -> dict[str, int | str | bool]:
    txt = path.read_text(encoding="utf-8", errors="ignore")
    out: dict[str, int | str | bool] = {}

    clint_m = re.search(r"^\s*clint:.*@\s*sysbus\s*(0x[0-9a-fA-F]+)", txt, re.MULTILINE)
    freq_m = re.search(r"^\s*frequency:\s*([0-9]+)", txt, re.MULTILINE)
    uart0_m = re.search(r"^\s*uart0:.*@\s*sysbus\s*(0x[0-9a-fA-F]+)", txt, re.MULTILINE)
    route_m = re.search(r"uart0:[\s\S]*?->\s*([A-Za-z0-9_]+)@(\d+)", txt)
    cpu_count = len(re.findall(r"^\s*cpu[0-9]+:\s*CPU\.RiscV32", txt, re.MULTILINE))

    if clint_m:
        out["CLINT_BASE"] = int(clint_m.group(1), 0)
    if freq_m:
        out["TIMER_FREQ"] = int(freq_m.group(1), 10)
    if uart0_m:
        out["UART0_BASE"] = int(uart0_m.group(1), 0)
    if route_m:
        out["UART0_ROUTE_TARGET"] = route_m.group(1)
        out["UART0_ROUTE_IRQ"] = int(route_m.group(2), 10)
    out["CPU_COUNT"] = cpu_count
    out["CLIC_NOTE"] = "CLIC-compatible direct IRQ wiring" in txt

    return out


def parse_sdk_clic_irqs(path: Path) -> dict[str, int]:
    text = path.read_text(encoding="utf-8", errors="ignore")
    return {
        match.group(1): int(match.group(2), 10)
        for match in SDK_CLIC_IRQ_RE.finditer(text)
    }


def compare_sdk_header(
    defs: dict[str, str], cache: dict[str, int], sdk_path: Path
) -> list[str]:
    failures: list[str] = []
    sdk_defs = parse_defines(sdk_path)
    sdk_irqs = parse_sdk_clic_irqs(sdk_path)
    sdk_cache: dict[str, int] = {}

    irq_pairs = {
        "BE_U1000_CLIC_NUM_IRQS": None,
        "BE_U1000_IRQ_SW": "CLIC_MS_IRQn",
        "BE_U1000_IRQ_TIMER": "CLIC_MT_IRQn",
        "BE_U1000_IRQ_CLIC_SW": "CLIC_CS_IRQn",
        "BE_U1000_IRQ_QSPI0": "CLIC_QSPI0_IRQn",
        "BE_U1000_IRQ_QSPI1": "CLIC_QSPI1_IRQn",
        "BE_U1000_IRQ_UART0": "CLIC_UART0_IRQn",
        "BE_U1000_IRQ_UART1": "CLIC_UART1_IRQn",
        "BE_U1000_IRQ_UART2": "CLIC_UART2_IRQn",
        "BE_U1000_IRQ_I2C0": "CLIC_I2C0_IRQn",
        "BE_U1000_IRQ_I2C1": "CLIC_I2C1_IRQn",
        "BE_U1000_IRQ_I2S0": "CLIC_I2S0_IRQn",
        "BE_U1000_IRQ_TIM0_CH0": "CLIC_TIM0_Channel0_IRQn",
        "BE_U1000_IRQ_TIM0_CH1": "CLIC_TIM0_Channel1_IRQn",
        "BE_U1000_IRQ_TIM0_CH2": "CLIC_TIM0_Channel2_IRQn",
        "BE_U1000_IRQ_TIM0_CH3": "CLIC_TIM0_Channel3_IRQn",
        "BE_U1000_IRQ_WDT0": "CLIC_WDT0_IRQn",
        "BE_U1000_IRQ_GPIO0": "CLIC_GPIO0_IRQn",
        "BE_U1000_IRQ_PWMG0": "CLIC_PWMG0_IRQn",
        "BE_U1000_IRQ_SPI0": "CLIC_SPI0_IRQn",
        "BE_U1000_IRQ_SPI1": "CLIC_SPI1_IRQn",
        "BE_U1000_IRQ_CANFD0": "CLIC_CANFD0_IRQn",
        "BE_U1000_IRQ_UART3": "CLIC_UART3_IRQn",
        "BE_U1000_IRQ_UART4": "CLIC_UART4_IRQn",
        "BE_U1000_IRQ_UART5": "CLIC_UART5_IRQn",
        "BE_U1000_IRQ_I2C2": "CLIC_I2C2_IRQn",
        "BE_U1000_IRQ_I2C3": "CLIC_I2C3_IRQn",
        "BE_U1000_IRQ_I2S1": "CLIC_I2S1_IRQn",
        "BE_U1000_IRQ_TIM1_CH0": "CLIC_TIM1_Channel0_IRQn",
        "BE_U1000_IRQ_TIM1_CH1": "CLIC_TIM1_Channel1_IRQn",
        "BE_U1000_IRQ_TIM1_CH2": "CLIC_TIM1_Channel2_IRQn",
        "BE_U1000_IRQ_TIM1_CH3": "CLIC_TIM1_Channel3_IRQn",
        "BE_U1000_IRQ_GPIO1": "CLIC_GPIO1_IRQn",
        "BE_U1000_IRQ_PWMG1": "CLIC_PWMG1_IRQn",
        "BE_U1000_IRQ_SPI2": "CLIC_SPI2_IRQn",
        "BE_U1000_IRQ_SPI3": "CLIC_SPI3_IRQn",
        "BE_U1000_IRQ_CANFD1": "CLIC_CANFD1_IRQn",
        "BE_U1000_IRQ_ADC0": "CLIC_ADC0_IRQn",
        "BE_U1000_IRQ_ADC1": "CLIC_ADC1_IRQn",
        "BE_U1000_IRQ_ADC2": "CLIC_ADC2_IRQn",
        "BE_U1000_IRQ_PWMA0": "CLIC_PWMA0_IRQn",
        "BE_U1000_IRQ_QE0": "CLIC_QE0_IRQn",
        "BE_U1000_IRQ_PWMA1": "CLIC_PWMA1_IRQn",
        "BE_U1000_IRQ_QE1": "CLIC_QE1_IRQn",
        "BE_U1000_IRQ_PWMA2": "CLIC_PWMA2_IRQn",
        "BE_U1000_IRQ_QE2": "CLIC_QE2_IRQn",
        "BE_U1000_IRQ_PWMA3": "CLIC_PWMA3_IRQn",
        "BE_U1000_IRQ_QE3": "CLIC_QE3_IRQn",
        "BE_U1000_IRQ_GPIO2": "CLIC_GPIO2_IRQn",
        "BE_U1000_IRQ_UART6": "CLIC_UART6_IRQn",
        "BE_U1000_IRQ_UART7": "CLIC_UART7_IRQn",
        "BE_U1000_IRQ_USB": "CLIC_USB_IRQn",
        "BE_U1000_IRQ_DMA0": "CLIC_DMA0_IRQn",
        "BE_U1000_IRQ_DMA1": "CLIC_DMA1_IRQn",
        "BE_U1000_IRQ_USB_DMA": "CLIC_USB_DMA_IRQn",
        "BE_U1000_IRQ_CORE2": "CLIC_CORE2_IRQn",
        "BE_U1000_IRQ_MAILBOX0": "CLIC_CORE2_MB0_IRQn",
        "BE_U1000_IRQ_MAILBOX1": "CLIC_CORE2_MB1_IRQn",
        "BE_U1000_IRQ_EXTI_PA": "CLIC_EXTI_PA_IRQn",
        "BE_U1000_IRQ_EXTI_PB": "CLIC_EXTI_PB_IRQn",
        "BE_U1000_IRQ_EXTI_PC": "CLIC_EXTI_PC_IRQn",
        "BE_U1000_IRQ_BUS_ERROR_CORE0": "CLIC_CORE0_BusError_IRQn",
        "BE_U1000_IRQ_BUS_ERROR_CORE1": "CLIC_CORE1_BusError_IRQn",
        "BE_U1000_IRQ_PLL_UNLOCK": "CLIC_PLL_Unlock_IRQn",
    }
    addr_pairs = {
        "BE_U1000_UART0_BASE": "UART0_BASE",
        "BE_U1000_GPIO2_BASE": "GPIO2_BASE",
        "BE_U1000_CRU_BASE": "CRU_BASE",
        "BE_U1000_MB0_BASE": "MB0_BASE",
        "BE_U1000_MB1_BASE": "MB1_BASE",
        "BE_U1000_TCMA_BASE": "TCMA_BASE",
        "BE_U1000_C2_TCMA_SYS_BASE": "CORE2_TCMA_SYS_BASE",
        "BE_U1000_C2_TCMA_BASE": "CORE2_TCMA_BASE",
    }

    for board_name, sdk_name in irq_pairs.items():
        board_value = resolve_int(board_name, defs, cache, set())
        if sdk_name is None:
            sdk_value = max(sdk_irqs.values()) + 1
        else:
            sdk_value = sdk_irqs.get(sdk_name)
            if sdk_value is None:
                failures.append(f"missing {sdk_name} in sdk header")
                continue
        if board_value != sdk_value:
            failures.append(
                f"{board_name} mismatch vs sdk: board={board_value} sdk={sdk_value}"
            )

    for board_name, sdk_name in addr_pairs.items():
        board_value = resolve_int(board_name, defs, cache, set())
        sdk_value = resolve_int(sdk_name, sdk_defs, sdk_cache, set())
        if board_value != sdk_value:
            failures.append(
                f"{board_name} mismatch vs sdk: board=0x{board_value:x} sdk=0x{sdk_value:x}"
            )

    return failures


def main() -> int:
    ap = argparse.ArgumentParser()
    _ = ap.add_argument("--board-config", required=True)
    _ = ap.add_argument("--repl", required=True)
    _ = ap.add_argument("--sdk-header")
    args = ap.parse_args()

    board_path = Path(cast(str, args.board_config))
    repl_path = Path(cast(str, args.repl))
    sdk_header_arg = cast(str | None, args.sdk_header)
    sdk_path = Path(sdk_header_arg) if sdk_header_arg else None
    if (
        not board_path.exists()
        or not repl_path.exists()
        or (sdk_path is not None and not sdk_path.exists())
    ):
        print("INTERRUPT_CONSISTENCY_ERROR: input file missing")
        return 2

    defs = parse_defines(board_path)
    cache: dict[str, int] = {}
    expected = {
        "CLINT_BASE": resolve_int("BE_U1000_CLINT_BASE", defs, cache, set()),
        "TIMER_FREQ": resolve_int("BE_U1000_TIMER_FREQ", defs, cache, set()),
        "UART0_BASE": resolve_int("BE_U1000_CONSOLE_UART_BASE", defs, cache, set()),
        "UART0_IRQ": resolve_int("BE_U1000_CONSOLE_UART_IRQ", defs, cache, set()),
        "CPU_COUNT": resolve_int("BE_U1000_NUM_MAIN_CORES", defs, cache, set()),
    }

    actual = parse_repl(repl_path)

    failures: list[str] = []
    for key in ("CLINT_BASE", "TIMER_FREQ", "UART0_BASE", "CPU_COUNT"):
        if key not in actual:
            failures.append(f"missing {key} in repl")
            continue
        if int(actual[key]) != expected[key]:
            failures.append(
                f"{key} mismatch: board=0x{expected[key]:x} repl=0x{int(actual[key]):x}"
                if "BASE" in key
                else f"{key} mismatch: board={expected[key]} repl={int(actual[key])}"
            )

    if not bool(actual.get("CLIC_NOTE", False)):
        failures.append("repl must include explicit CLIC-compatible routing note")

    if "UART0_ROUTE_IRQ" not in actual:
        failures.append("missing UART0 route irq in repl")
    elif int(actual["UART0_ROUTE_IRQ"]) != expected["UART0_IRQ"]:
        failures.append(
            f"UART0 route IRQ mismatch: board={expected['UART0_IRQ']} repl={int(actual['UART0_ROUTE_IRQ'])}"
        )

    if actual.get("UART0_ROUTE_TARGET") != "cpu0":
        failures.append(
            "UART0 route target must be cpu0 for CLIC-compatible direct routing"
        )

    if sdk_path is not None:
        failures.extend(compare_sdk_header(defs, cache, sdk_path))

    if failures:
        print("INTERRUPT_CONSISTENCY_FAILED")
        for f in failures:
            print(f"- {f}")
        return 1

    print("INTERRUPT_CONSISTENCY_OK")
    summary = (
        f"summary: clint=0x{expected['CLINT_BASE']:x}, "
        f"uart0=0x{expected['UART0_BASE']:x}, "
        f"uart0_irq={expected['UART0_IRQ']}, "
        f"timer={expected['TIMER_FREQ']}, "
        f"cpus={expected['CPU_COUNT']}"
    )
    print(summary)
    if sdk_path is not None:
        print(f"sdk_header: {sdk_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
