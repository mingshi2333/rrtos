#!/usr/bin/env python3

import argparse
import re
from pathlib import Path
from typing import Any, cast


DEFINE_RE = re.compile(r"^\s*#define\s+([A-Za-z0-9_]+)\s+(.+?)\s*$")
TOKEN_RE = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")


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


def main() -> int:
    ap = argparse.ArgumentParser()
    _ = ap.add_argument("--board-config", required=True)
    _ = ap.add_argument("--repl", required=True)
    args = ap.parse_args()

    board_path = Path(cast(str, args.board_config))
    repl_path = Path(cast(str, args.repl))
    if not board_path.exists() or not repl_path.exists():
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
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
