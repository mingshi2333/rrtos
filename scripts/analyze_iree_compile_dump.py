#!/usr/bin/env python3
"""Scan IREE phase dumps for allocation/resource-size clues."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import re
from pathlib import Path


RESOURCE_PATTERNS = (
    "stream.resource",
    "hal.interface.binding",
    "hal.command_buffer",
    "hal.executable",
    "flow.dispatch",
    "tensor.empty",
    "memref.alloc",
)
INTEGER_RE = re.compile(r"\b\d+\b")
SSA_RE = r"%[A-Za-z0-9_.$-]+"
CONSTANT_RE = re.compile(rf"^\s*({SSA_RE}) = arith\.constant (-?\d+) : index\b")
DIM_RE = re.compile(rf"^\s*({SSA_RE}) = hal\.buffer_view\.dim<")
BINOP_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.(addi|subi|muli) ({SSA_RE}), ({SSA_RE})(?: [^:]*)? : index\b"
)
ALIGN_RE = re.compile(rf"^\s*({SSA_RE}) = util\.align ({SSA_RE}), ({SSA_RE}) : index\b")
ALLOCATION_RE = re.compile(
    rf"(?P<kind>stream\.resource\.alloca|hal\.device\.queue\.alloca|hal\.allocator\.allocate)"
    rf".*\{{(?P<size>{SSA_RE})\}}"
)
EXECUTE_CAPTURE_RE = re.compile(
    rf"(?P<source>{SSA_RE}) as (?P<arg>{SSA_RE}): !stream\.resource<(?P<lifetime>\w+)>"
    rf"\{{(?P<size>{SSA_RE})\}}"
)
ACCESS_RE = re.compile(
    rf"^\s*(?P<mode>ro|rw|wo)\s+(?P<arg>{SSA_RE})"
    rf"\[(?P<offset>{SSA_RE}) for (?P<length>{SSA_RE})\]"
)
FILL_ACCESS_RE = re.compile(
    rf"stream\.cmd\.fill .*?,\s*(?P<arg>{SSA_RE})"
    rf"\[(?P<offset>{SSA_RE}) for (?P<length>{SSA_RE})\]"
)
TENSOR_TYPE_RE = re.compile(
    r"tensor<(?P<shape>(?:\?|\d+)(?:x(?:\?|\d+))*)x(?P<element>[a-z]+\d+)>"
)


@dataclass(frozen=True)
class ValueInfo:
    value: int
    expression: str
    op: str
    operands: tuple[str, ...] = ()


@dataclass(frozen=True)
class AllocationRecord:
    line_no: int
    kind: str
    result_ref: str | None
    size_ref: str
    size_value: int | None
    expression: str | None
    line: str


@dataclass(frozen=True)
class ResourceCapture:
    line_no: int
    source_ref: str
    arg_ref: str
    lifetime: str
    size_ref: str
    size_value: int | None


@dataclass(frozen=True)
class ResourceAccess:
    line_no: int
    execute_line_no: int
    source_ref: str
    arg_ref: str
    mode: str
    offset_ref: str
    offset_value: int | None
    length_ref: str
    length_value: int | None
    operation: str
    line: str


def sort_key(path: Path) -> tuple[int, str]:
    match = re.search(r"\.(\d+)\.", path.name)
    return (int(match.group(1)) if match else 9999, path.name)


def context(lines: list[str], index: int, radius: int) -> list[tuple[int, str]]:
    start = max(0, index - radius)
    end = min(len(lines), index + radius + 1)
    return [(line_no + 1, lines[line_no].rstrip()) for line_no in range(start, end)]


def compact_line(line: str, max_len: int = 220) -> str:
    line = line.rstrip()
    if len(line) <= max_len:
        return line
    return line[: max_len - 3] + "..."


def print_context(path: Path, lines: list[str], index: int, radius: int) -> None:
    print(f"\n== {path}:{index + 1} ==")
    for line_no, line in context(lines, index, radius):
        print(f"{line_no}: {compact_line(line)}")


def scan_exact_matches(path: Path, needle: str, radius: int) -> int:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    count = 0
    for index, line in enumerate(lines):
        if needle in line:
            print_context(path, lines, index, radius)
            count += 1
    return count


def scan_resource_lines(path: Path, radius: int, limit: int) -> int:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    count = 0
    for index, line in enumerate(lines):
        if any(pattern in line for pattern in RESOURCE_PATTERNS):
            if "alloc" in line or "resource" in line or "binding" in line:
                if count >= limit:
                    return count
                print_context(path, lines, index, radius)
                count += 1
    return count


def nearest_numbers(path: Path, needle_size: int, limit: int) -> list[tuple[int, int, str]]:
    candidates: list[tuple[int, int, str]] = []
    for line_no, line in enumerate(
        path.read_text(encoding="utf-8", errors="replace").splitlines(), start=1
    ):
        if "dense<" in line or len(line) > 1000:
            continue
        for match in INTEGER_RE.finditer(line):
            value = int(match.group(0))
            if value == 0:
                continue
            distance = abs(value - needle_size)
            if distance <= max(4096, needle_size // 8):
                candidates.append((distance, line_no, compact_line(line)))
    candidates.sort(key=lambda item: (item[0], item[1]))
    return candidates[:limit]


def align_to(value: int, alignment: int) -> int:
    if alignment <= 0:
        return value
    return ((value + alignment - 1) // alignment) * alignment


def parse_value_info(lines: list[str], batch_size: int) -> dict[str, ValueInfo]:
    values: dict[str, ValueInfo] = {}
    for line in lines:
        if match := CONSTANT_RE.match(line):
            ref = match.group(1)
            value = int(match.group(2))
            values[ref] = ValueInfo(value, match.group(2), "constant")
            continue
        if match := DIM_RE.match(line):
            ref = match.group(1)
            values[ref] = ValueInfo(batch_size, f"batch({batch_size})", "dim")
            continue
        if match := BINOP_RE.match(line):
            dest, op, lhs_ref, rhs_ref = match.groups()
            if lhs_ref not in values or rhs_ref not in values:
                continue
            lhs = values[lhs_ref].value
            rhs = values[rhs_ref].value
            lhs_expression = values[lhs_ref].expression
            rhs_expression = values[rhs_ref].expression
            if op == "addi":
                values[dest] = ValueInfo(
                    lhs + rhs,
                    f"({lhs_expression} + {rhs_expression})",
                    op,
                    (lhs_ref, rhs_ref),
                )
            elif op == "subi":
                values[dest] = ValueInfo(
                    lhs - rhs,
                    f"({lhs_expression} - {rhs_expression})",
                    op,
                    (lhs_ref, rhs_ref),
                )
            else:
                values[dest] = ValueInfo(
                    lhs * rhs,
                    f"({lhs_expression} * {rhs_expression})",
                    op,
                    (lhs_ref, rhs_ref),
                )
            continue
        if match := ALIGN_RE.match(line):
            dest, value_ref, alignment_ref = match.groups()
            if value_ref in values and alignment_ref in values:
                values[dest] = ValueInfo(
                    align_to(values[value_ref].value, values[alignment_ref].value),
                    f"align({values[value_ref].expression}, {values[alignment_ref].expression})",
                    "align",
                    (value_ref, alignment_ref),
                )
    return values


def parse_values(lines: list[str], batch_size: int) -> tuple[dict[str, int], dict[str, str]]:
    value_info = parse_value_info(lines, batch_size)
    return (
        {ref: info.value for ref, info in value_info.items()},
        {ref: info.expression for ref, info in value_info.items()},
    )


def additive_terms(ref: str, value_info: dict[str, ValueInfo]) -> list[tuple[str, int, str]]:
    info = value_info.get(ref)
    if not info:
        return []
    if info.op == "addi" and len(info.operands) == 2:
        return additive_terms(info.operands[0], value_info) + additive_terms(
            info.operands[1], value_info
        )
    return [(ref, info.value, info.expression)]


def tensor_element_size(element: str) -> int | None:
    if element in ("i1", "ui8", "si8", "i8"):
        return 1
    if element in ("i16", "f16", "bf16"):
        return 2
    if element in ("i32", "f32"):
        return 4
    if element in ("i64", "f64"):
        return 8
    return None


def tensor_shape_size(shape: str, element: str, batch_size: int) -> int | None:
    element_size = tensor_element_size(element)
    if element_size is None:
        return None
    total = element_size
    for dim in shape.split("x"):
        total *= batch_size if dim == "?" else int(dim)
    return total


def scan_tensor_shapes(path: Path, batch_size: int) -> dict[int, list[str]]:
    labels: dict[int, list[str]] = {}
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    for line_no, line in enumerate(lines, start=1):
        if "tensor<" not in line:
            continue
        for match in TENSOR_TYPE_RE.finditer(line):
            size = tensor_shape_size(match.group("shape"), match.group("element"), batch_size)
            if size is None:
                continue
            label = f"{match.group(0)} at {path.name}:{line_no}"
            bucket = labels.setdefault(size, [])
            if label not in bucket:
                bucket.append(label)
    return labels


def scan_allocations(path: Path, batch_size: int) -> list[AllocationRecord]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    values, expressions = parse_values(lines, batch_size)
    allocations: list[AllocationRecord] = []
    for line_no, line in enumerate(lines, start=1):
        match = ALLOCATION_RE.search(line)
        if not match:
            continue
        size_ref = match.group("size")
        result_match = re.match(rf"^\s*(?P<result>{SSA_RE})(?:\s*,\s*{SSA_RE})*\s*=", line)
        allocations.append(
            AllocationRecord(
                line_no=line_no,
                kind=match.group("kind"),
                result_ref=result_match.group("result") if result_match else None,
                size_ref=size_ref,
                size_value=values.get(size_ref),
                expression=expressions.get(size_ref),
                line=compact_line(line),
            )
        )
    return allocations


def scan_stream_resource_accesses(
    path: Path, batch_size: int
) -> tuple[list[ResourceCapture], list[ResourceAccess]]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    values, _ = parse_values(lines, batch_size)
    captures: list[ResourceCapture] = []
    accesses: list[ResourceAccess] = []
    active_execute_args: dict[str, ResourceCapture] = {}
    active_execute_line_no = 0
    current_operation = ""

    for line_no, line in enumerate(lines, start=1):
        if "stream.cmd.execute" in line and " with(" in line:
            active_execute_args.clear()
            active_execute_line_no = line_no
            for match in EXECUTE_CAPTURE_RE.finditer(line):
                capture = ResourceCapture(
                    line_no=line_no,
                    source_ref=match.group("source"),
                    arg_ref=match.group("arg"),
                    lifetime=match.group("lifetime"),
                    size_ref=match.group("size"),
                    size_value=values.get(match.group("size")),
                )
                captures.append(capture)
                active_execute_args[capture.arg_ref] = capture
            current_operation = compact_line(line)
            continue

        if "stream.cmd.dispatch" in line:
            current_operation = compact_line(line)
        elif "stream.cmd.fill" in line:
            current_operation = compact_line(line)
            match = FILL_ACCESS_RE.search(line)
            if match and match.group("arg") in active_execute_args:
                capture = active_execute_args[match.group("arg")]
                accesses.append(
                    ResourceAccess(
                        line_no=line_no,
                        execute_line_no=active_execute_line_no,
                        source_ref=capture.source_ref,
                        arg_ref=match.group("arg"),
                        mode="fill",
                        offset_ref=match.group("offset"),
                        offset_value=values.get(match.group("offset")),
                        length_ref=match.group("length"),
                        length_value=values.get(match.group("length")),
                        operation=current_operation,
                        line=compact_line(line),
                    )
                )
            continue

        match = ACCESS_RE.match(line)
        if match and match.group("arg") in active_execute_args:
            capture = active_execute_args[match.group("arg")]
            accesses.append(
                ResourceAccess(
                    line_no=line_no,
                    execute_line_no=active_execute_line_no,
                    source_ref=capture.source_ref,
                    arg_ref=match.group("arg"),
                    mode=match.group("mode"),
                    offset_ref=match.group("offset"),
                    offset_value=values.get(match.group("offset")),
                    length_ref=match.group("length"),
                    length_value=values.get(match.group("length")),
                    operation=current_operation,
                    line=compact_line(line),
                )
            )

    return captures, accesses


def tensor_notes_for_expression(
    expression: str, tensor_shapes: dict[int, list[str]], exact_value: int
) -> list[str]:
    candidates = [exact_value]
    candidates.extend(
        value
        for value in (int(value) for value in INTEGER_RE.findall(expression))
        if value >= 1024 or value == exact_value
    )
    notes: list[str] = []
    seen: set[str] = set()
    for value in candidates:
        for label in tensor_shapes.get(value, [])[:3]:
            if label not in seen:
                notes.append(label)
                seen.add(label)
    return notes[:4]


def print_allocation_explanation(path: Path, allocation: AllocationRecord, batch_size: int) -> None:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    value_info = parse_value_info(lines, batch_size)
    tensor_shapes = scan_tensor_shapes(path, batch_size)

    if allocation.size_ref in value_info:
        terms = additive_terms(allocation.size_ref, value_info)
        if len(terms) > 1:
            print("  components:")
            offset = 0
            for ref, value, expression in terms:
                print(
                    f"    {ref}: offset={offset} length={value} "
                    f"expr={compact_line(expression, 120)}"
                )
                for note in tensor_notes_for_expression(expression, tensor_shapes, value):
                    print(f"      matches {note}")
                offset += value

    if not allocation.result_ref:
        return
    captures, accesses = scan_stream_resource_accesses(path, batch_size)
    matching_captures = [
        capture for capture in captures if capture.source_ref == allocation.result_ref
    ]
    if not matching_captures:
        return
    print("  stream accesses:")
    for capture in matching_captures:
        print(
            f"    capture {capture.source_ref} as {capture.arg_ref} "
            f"size={capture.size_ref} value={capture.size_value}"
        )
        access_count = 0
        for access in accesses:
            if access.source_ref != capture.source_ref or access.execute_line_no != capture.line_no:
                continue
            access_count += 1
            offset = "unknown" if access.offset_value is None else str(access.offset_value)
            length = "unknown" if access.length_value is None else str(access.length_value)
            print(
                f"      {path.name}:{access.line_no}: {access.mode} "
                f"offset={access.offset_ref}({offset}) length={access.length_ref}({length})"
            )
            print(f"        op: {access.operation}")
        if access_count == 0:
            print("      no parsed stream.cmd resource accesses")


def print_allocation_table(phase_files: list[Path], needle_size: int, batch_size: int) -> None:
    print(f"\n# Allocation table assuming batch size {batch_size}")
    rows: list[tuple[Path, AllocationRecord]] = []
    for path in phase_files:
        if any(stage in path.name for stage in (".7.", ".11.")):
            for row in scan_allocations(path, batch_size):
                rows.append((path, row))

    if not rows:
        print("No stream/HAL allocation operations found.")
        return

    rows.sort(
        key=lambda row: (
            abs((row[1].size_value if row[1].size_value is not None else -1) - needle_size),
            sort_key(row[0]),
            row[1].line_no,
        )
    )
    for path, allocation in rows:
        size_value = allocation.size_value
        value = "unknown" if size_value is None else str(size_value)
        marker = " <-- needle" if size_value == needle_size else ""
        print(
            f"{path.name}:{allocation.line_no}: {allocation.kind} "
            f"result={allocation.result_ref or '?'} size={allocation.size_ref} "
            f"value={value}{marker}"
        )
        if allocation.expression and size_value == needle_size:
            print(f"  expression: {compact_line(allocation.expression, 320)}")
            if ".7." in path.name:
                print_allocation_explanation(path, allocation, batch_size)
        print(f"  {allocation.line}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--dump-dir", required=True)
    parser.add_argument("--needle-size", type=int, required=True)
    parser.add_argument("--batch-size", type=int, default=1)
    parser.add_argument("--context-lines", type=int, default=3)
    parser.add_argument("--nearest-limit", type=int, default=30)
    parser.add_argument("--resource-limit", type=int, default=0)
    args = parser.parse_args()

    dump_dir = Path(args.dump_dir).expanduser().resolve()
    if not dump_dir.exists():
        print(f"error: dump dir not found: {dump_dir}")
        return 2

    phase_files = sorted(dump_dir.glob("*.mlir"), key=sort_key)
    if not phase_files:
        print(f"error: no .mlir phase files found in {dump_dir}")
        return 2

    needle = str(args.needle_size)
    exact_count = 0
    print(f"# Exact matches for {needle}")
    for path in phase_files:
        exact_count += scan_exact_matches(path, needle, args.context_lines)
    if exact_count == 0:
        print("No literal matches.")

    print(f"\n# Nearby numeric candidates for {needle}")
    for path in phase_files:
        candidates = nearest_numbers(path, args.needle_size, args.nearest_limit)
        if not candidates:
            continue
        print(f"\n== {path} ==")
        for distance, line_no, line in candidates:
            print(f"{line_no}: distance={distance}: {line}")

    print_allocation_table(phase_files, args.needle_size, args.batch_size)

    print("\n# Resource-oriented lines")
    resource_count = 0
    if args.resource_limit == 0:
        print("Skipped. Pass --resource-limit N to print resource context.")
        return 0
    for path in phase_files:
        if any(stage in path.name for stage in (".7.", ".8.", ".9.", ".10.", ".11.", ".12.")):
            remaining = max(args.resource_limit - resource_count, 0)
            if remaining == 0:
                break
            resource_count += scan_resource_lines(path, args.context_lines, remaining)
    if resource_count == 0:
        print("No resource-oriented lines found.")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
