#!/usr/bin/env python3
"""Extract a fixed-shape static-direct schedule from compact IREE HAL MLIR."""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
from pathlib import Path

try:
    from scripts import analyze_iree_compile_dump as dump_analyzer
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import analyze_iree_compile_dump as dump_analyzer


SCHEMA = "rrtos.iree_hal_schedule.v1"
SSA_RE = r"%[A-Za-z0-9_.$-]+"
DYNAMIC_VALUE_RE = re.compile(rf"^\s*({SSA_RE}) = hal\.buffer_view\.dim<")
FUNC_RE = re.compile(r"^\s*util\.func\b")
FUNC_END_RE = re.compile(r"^\s{2}\}\s*$")
BLOCK_LABEL_RE = re.compile(r"^\s*\^(?P<name>bb\d+)(?:\((?P<args>[^)]*)\))?:")
COND_BR_RE = re.compile(
    rf"^\s*cf\.cond_br (?P<cond>{SSA_RE}), (?P<true>\^bb\d+(?:\([^)]*\))?), "
    rf"(?P<false>\^bb\d+(?:\([^)]*\))?)"
)
BR_RE = re.compile(r"^\s*cf\.br (?P<target>\^bb\d+(?:\([^)]*\))?)")
INT_CONSTANT_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.constant (-?\d+) : (?:index|i\d+)\b"
)
BOOL_CONSTANT_RE = re.compile(rf"^\s*({SSA_RE}) = arith\.constant (true|false)\b")
INT_BINOP_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.(addi|subi|muli|divsi|divui|shli|shrui|andi) "
    rf"({SSA_RE}), ({SSA_RE})(?: [^:]*)? : (?:index|i\d+)\b"
)
CEILDIV_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.(ceildivui|ceildivsi) ({SSA_RE}), ({SSA_RE})"
    rf"(?: [^:]*)? : (?:index|i\d+)\b"
)
CAST_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.(?:index_castui|index_cast|extui|extsi|trunci) "
    rf"({SSA_RE}) : .*$"
)
CMPI_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.cmpi "
    rf"(eq|ne|slt|sle|sgt|sge|ult|ule|ugt|uge), ({SSA_RE}), ({SSA_RE})"
    rf"(?: [^:]*)? : (?:index|i\d+)\b"
)
SELECT_RE = re.compile(
    rf"^\s*({SSA_RE}) = arith\.select ({SSA_RE}), ({SSA_RE}), ({SSA_RE}) : .*$"
)
BUFFER_VIEW_BUFFER_RE = re.compile(
    rf"^\s*(?P<result>{SSA_RE}) = hal\.buffer_view\.buffer<(?P<source>{SSA_RE})"
    rf"\s*:\s*!hal\.buffer_view>\s*:\s*!hal\.buffer\b"
)
GLOBAL_BUFFER_RE = re.compile(
    rf"^\s*(?P<result>{SSA_RE}) = util\.global\.load immutable @(?P<symbol>[^\s]+)"
    rf"\s*:\s*!hal\.buffer\b"
)
ALLOCA_RE = re.compile(
    rf"^\s*(?P<result>{SSA_RE}) = hal\.device\.queue\.alloca<.*?"
    rf": !hal\.buffer\{{(?P<size>{SSA_RE}|-?\d+)\}}"
)
DEALLOCA_RE = re.compile(
    rf"hal\.device\.queue\.dealloca<.*?>(?:\s+(?P<plain>{SSA_RE})\b|.*?buffer\((?P<typed>{SSA_RE})"
    rf"\s*:\s*!hal\.buffer\))"
)
SUBSPAN_RE = re.compile(
    rf"^\s*(?P<result>{SSA_RE}) = hal\.buffer\.subspan<(?P<base>{SSA_RE})"
    rf"\s*:\s*!hal\.buffer>\[(?P<offset>{SSA_RE}|-?\d+),\s*(?P<length>{SSA_RE}|-?\d+)\]"
)
BINDING_RE = re.compile(
    rf"(?:"
    rf"(?P<compact_ref>{SSA_RE})\[(?P<compact_offset>{SSA_RE}|-?\d+),\s*"
    rf"(?P<compact_length>{SSA_RE}|-?\d+)\]\s*:\s*!hal\.buffer"
    rf"|"
    rf"\((?P<typed_ref>{SSA_RE})\s*:\s*(?:!hal\.buffer|index)\)\["
    rf"(?P<typed_offset>{SSA_RE}|-?\d+),\s*(?P<typed_length>{SSA_RE}|-?\d+)\]"
    rf")"
)
ORDINAL_RE = re.compile(rf"target\([^)]*\)\[(?P<ordinal>{SSA_RE}|-?\d+)\]")
WORKGROUPS_RE = re.compile(r"workgroups\(\[(?P<values>[^\]]*)\]\)", re.DOTALL)
CONSTANTS_RE = re.compile(r"constants\(\[(?P<values>[^\]]*)\]\)", re.DOTALL)
BINDINGS_RE = re.compile(r"bindings\(\[(?P<body>.*?)\]\)", re.DOTALL)
FILL_RE = re.compile(
    rf"^\s*hal\.command_buffer\.fill_buffer<.*?"
    rf"target\((?P<ref>{SSA_RE})\s*:\s*(?:!hal\.buffer|index)\)\["
    rf"(?P<offset>{SSA_RE}|-?\d+),\s*(?P<length>{SSA_RE}|-?\d+)\]\s*"
    rf"pattern\((?P<pattern>{SSA_RE}|-?\d+)\s*:\s*i8\)"
)


class ScheduleParseError(RuntimeError):
    """Raised when a HAL schedule cannot be safely converted."""


def _id(ref: str) -> str:
    return ref[1:] if ref.startswith("%") else ref


def _main_function_lines(lines: list[str]) -> list[str]:
    function_lines = _function_lines(lines, "main")
    return function_lines if function_lines is not None else lines


def _function_lines(lines: list[str], function_name: str) -> list[str] | None:
    symbol = f"@{function_name}"
    for start, line in enumerate(lines):
        if FUNC_RE.match(line) and symbol in line:
            for end in range(start + 1, len(lines)):
                if FUNC_END_RE.match(lines[end]):
                    return lines[start : end + 1]
            raise ScheduleParseError("unterminated util.func @main")
    return None


def _parse_values(lines: list[str], batch_size: int) -> tuple[dict[str, int], set[str]]:
    values, _ = dump_analyzer.parse_values(lines, batch_size)
    dim_refs: set[str] = set()

    changed = True
    while changed:
        changed = False
        for line in lines:
            if match := INT_CONSTANT_RE.match(line):
                ref, literal = match.groups()
                if values.get(ref) != int(literal):
                    values[ref] = int(literal)
                    changed = True
                continue
            if match := BOOL_CONSTANT_RE.match(line):
                ref, literal = match.groups()
                value = 1 if literal == "true" else 0
                if values.get(ref) != value:
                    values[ref] = value
                    changed = True
                continue
            if match := DYNAMIC_VALUE_RE.match(line):
                ref = match.group(1)
                dim_refs.add(ref)
                if values.get(ref) != batch_size:
                    values[ref] = batch_size
                    changed = True
                continue
            if match := INT_BINOP_RE.match(line):
                dest, op, lhs_ref, rhs_ref = match.groups()
                if lhs_ref not in values or rhs_ref not in values:
                    continue
                lhs = values[lhs_ref]
                rhs = values[rhs_ref]
                if op == "addi":
                    value = lhs + rhs
                elif op == "subi":
                    value = lhs - rhs
                elif op == "muli":
                    value = lhs * rhs
                elif op in {"divsi", "divui"}:
                    if rhs == 0:
                        raise ScheduleParseError(f"division by zero while evaluating {dest}")
                    value = lhs // rhs
                elif op == "shli":
                    value = lhs << rhs
                elif op == "shrui":
                    value = (lhs & ((1 << 64) - 1)) >> rhs
                else:
                    value = lhs & rhs
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
                continue
            if match := CEILDIV_RE.match(line):
                dest, _op, lhs_ref, rhs_ref = match.groups()
                if lhs_ref not in values or rhs_ref not in values:
                    continue
                lhs = values[lhs_ref]
                rhs = values[rhs_ref]
                if rhs <= 0:
                    raise ScheduleParseError(f"invalid ceildiv divisor while evaluating {dest}")
                value = (lhs + rhs - 1) // rhs
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
                continue
            if match := dump_analyzer.ALIGN_RE.match(line):
                dest, value_ref, alignment_ref = match.groups()
                if value_ref not in values or alignment_ref not in values:
                    continue
                value = dump_analyzer.align_to(values[value_ref], values[alignment_ref])
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
                continue
            if match := CAST_RE.match(line):
                dest, source_ref = match.groups()
                if source_ref not in values:
                    continue
                value = values[source_ref]
                if " to i32" in line:
                    value &= 0xFFFFFFFF
                elif " to i16" in line:
                    value &= 0xFFFF
                elif " to i8" in line:
                    value &= 0xFF
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
                continue
            if match := CMPI_RE.match(line):
                dest, predicate, lhs_ref, rhs_ref = match.groups()
                if lhs_ref not in values or rhs_ref not in values:
                    continue
                lhs = values[lhs_ref]
                rhs = values[rhs_ref]
                if predicate == "eq":
                    value = int(lhs == rhs)
                elif predicate == "ne":
                    value = int(lhs != rhs)
                elif predicate in {"slt", "ult"}:
                    value = int(lhs < rhs)
                elif predicate in {"sle", "ule"}:
                    value = int(lhs <= rhs)
                elif predicate in {"sgt", "ugt"}:
                    value = int(lhs > rhs)
                else:
                    value = int(lhs >= rhs)
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
                continue
            if match := SELECT_RE.match(line):
                dest, condition_ref, true_ref, false_ref = match.groups()
                if (
                    condition_ref not in values
                    or true_ref not in values
                    or false_ref not in values
                ):
                    continue
                value = values[true_ref] if values[condition_ref] else values[false_ref]
                if values.get(dest) != value:
                    values[dest] = value
                    changed = True
    return values, dim_refs


def _dynamic_refs(lines: list[str]) -> set[str]:
    return set()


def _eval_index(ref_or_literal: str, values: dict[str, int], dynamic_refs: set[str]) -> int:
    token = ref_or_literal.strip()
    if re.fullmatch(r"-?\d+", token):
        return int(token)
    if token in dynamic_refs:
        raise ScheduleParseError(f"unsupported dynamic dimension: {token}")
    if token not in values:
        raise ScheduleParseError(f"unresolved value: {token}")
    return values[token]


def _eval_u32_constant(
    ref_or_literal: str, values: dict[str, int], dynamic_refs: set[str]
) -> int:
    return _eval_index(ref_or_literal, values, dynamic_refs) & 0xFFFFFFFF


def _split_value_list(value_list: str) -> list[str]:
    return [part.strip() for part in value_list.split(",") if part.strip()]


def _parse_block_args(args_text: str | None) -> list[str]:
    if not args_text:
        return []
    return re.findall(rf"({SSA_RE})\s*:", args_text)


def _parse_branch_target(target: str) -> tuple[str, list[str]]:
    stripped = target.strip()
    label_match = re.match(r"\^(bb\d+)", stripped)
    if not label_match:
        raise ScheduleParseError(f"unsupported branch target: {target}")
    label = label_match.group(1)
    operands: list[str] = []
    if "(" in stripped:
        operand_body = stripped[stripped.find("(") + 1 : stripped.rfind(")")]
        operand_body = operand_body.split(" : ", 1)[0]
        operands = re.findall(SSA_RE, operand_body)
    return label, operands


def _resolve_alias(ref: str, aliases: dict[str, str]) -> str:
    resolved = ref
    seen: set[str] = set()
    while resolved in aliases and resolved not in seen:
        seen.add(resolved)
        resolved = aliases[resolved]
    return resolved


def _split_blocks(lines: list[str]) -> tuple[dict[str, list[str]], dict[str, list[str]]]:
    blocks: dict[str, list[str]] = {"bb0": []}
    block_args: dict[str, list[str]] = {"bb0": []}
    current = "bb0"

    for line in lines:
        if FUNC_RE.match(line) or FUNC_END_RE.match(line):
            continue
        if match := BLOCK_LABEL_RE.match(line):
            current = match.group("name")
            blocks[current] = []
            block_args[current] = _parse_block_args(match.group("args"))
            continue
        blocks.setdefault(current, []).append(line)
    return blocks, block_args


def _linearize_reachable_lines(
    lines: list[str], values: dict[str, int]
) -> tuple[list[str], dict[str, str]]:
    blocks, block_args = _split_blocks(lines)
    if not any("cf.br" in line or "cf.cond_br" in line for block in blocks.values() for line in block):
        return blocks.get("bb0", []), {}

    current = "bb0"
    aliases: dict[str, str] = {}
    emitted: list[str] = []
    visited: set[str] = set()

    while True:
        if current in visited:
            raise ScheduleParseError(f"non-linear CFG loop at ^{current}")
        visited.add(current)
        if current not in blocks:
            raise ScheduleParseError(f"unknown CFG block: ^{current}")
        block = blocks[current]
        emitted.extend(block)

        branch_line = next(
            (line for line in reversed(block) if "cf.cond_br" in line or "cf.br" in line),
            None,
        )
        if branch_line is None:
            return emitted, aliases

        if match := COND_BR_RE.match(branch_line):
            condition_ref = match.group("cond")
            if condition_ref not in values:
                raise ScheduleParseError(f"unresolved branch condition: {condition_ref}")
            target = match.group("true") if values[condition_ref] else match.group("false")
        elif match := BR_RE.match(branch_line):
            target = match.group("target")
        else:
            raise ScheduleParseError(f"unsupported branch form: {branch_line.strip()}")

        current, operands = _parse_branch_target(target)
        formals = block_args.get(current, [])
        if len(operands) != len(formals):
            raise ScheduleParseError(
                f"branch to ^{current} passes {len(operands)} operands for {len(formals)} args"
            )
        for formal, operand in zip(formals, operands):
            aliases[formal] = _resolve_alias(operand, aliases)


def _dispatch_blocks(lines: list[str]) -> list[str]:
    blocks: list[str] = []
    active: list[str] = []
    saw_bindings = False
    for line in lines:
        if not active and "hal.command_buffer.dispatch" not in line:
            continue
        active.append(line.strip())
        if "bindings([" in line:
            saw_bindings = True
        if saw_bindings and re.search(r"^\s*\]\)\s*(?:flags\b|:|$)", line):
            blocks.append(" ".join(active))
            active = []
            saw_bindings = False
    if active:
        raise ScheduleParseError("unterminated hal.command_buffer.dispatch")
    return blocks


def _command_blocks(lines: list[str]) -> list[tuple[str, str]]:
    commands: list[tuple[str, str]] = []
    active: list[str] = []
    saw_bindings = False

    for line in lines:
        if active:
            active.append(line.strip())
            if "bindings([" in line:
                saw_bindings = True
            if saw_bindings and re.search(r"^\s*\]\)\s*(?:flags\b|:|$)", line):
                commands.append(("dispatch", " ".join(active)))
                active = []
                saw_bindings = False
            continue

        if "hal.command_buffer.dispatch" in line:
            active.append(line.strip())
            if "bindings([" in line:
                saw_bindings = True
            continue
        if "hal.command_buffer.fill_buffer" in line:
            commands.append(("fill", line))

    if active:
        raise ScheduleParseError("unterminated hal.command_buffer.dispatch")
    return commands


def _execute_indirect_blocks(lines: list[str]) -> list[str]:
    blocks: list[str] = []
    active: list[str] = []
    saw_bindings = False

    for line in lines:
        if active:
            active.append(line.strip())
            if "bindings([" in line:
                saw_bindings = True
            if saw_bindings and re.search(r"^\s*\]\)\s*(?:flags\b|:|$)", line):
                blocks.append(" ".join(active))
                active = []
                saw_bindings = False
            continue

        if "hal.device.queue.execute.indirect" in line:
            active.append(line.strip())
            if "bindings([" in line:
                saw_bindings = True

    if active:
        raise ScheduleParseError("unterminated hal.device.queue.execute.indirect")
    return blocks


def _parse_allocations(
    lines: list[str], values: dict[str, int], dynamic_refs: set[str]
) -> dict[str, dict[str, object]]:
    allocations: dict[str, dict[str, object]] = {}
    for line in lines:
        match = ALLOCA_RE.match(line)
        if not match:
            continue
        ref = match.group("result")
        allocations[ref] = {
            "id": _id(ref),
            "size": _eval_index(match.group("size"), values, dynamic_refs),
            "lifetime": "transient",
        }
    return allocations


def _external_buffers(lines: list[str]) -> dict[str, dict[str, object]]:
    buffers: dict[str, dict[str, object]] = {}
    for line in lines:
        if match := BUFFER_VIEW_BUFFER_RE.match(line):
            ref = match.group("result")
            buffers[ref] = {"id": _id(ref), "size": 0, "lifetime": "input"}
            continue
        if match := GLOBAL_BUFFER_RE.match(line):
            ref = match.group("result")
            buffers[ref] = {"id": _id(ref), "size": 0, "lifetime": "constant"}
    return buffers


def _ensure_external_allocation(
    ref: str,
    required_size: int,
    allocations: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
) -> None:
    if ref in allocations:
        current_size = int(allocations[ref]["size"])
        if allocations[ref].get("lifetime") == "transient" and required_size > current_size:
            raise ScheduleParseError(
                f"binding exceeds allocation {_id(ref)}: required {required_size} bytes, "
                f"allocation has {current_size} bytes"
            )
        allocations[ref]["size"] = max(int(allocations[ref]["size"]), required_size)
        return

    if ref in external_buffers:
        allocation = dict(external_buffers[ref])
    elif "__constant" in _id(ref):
        allocation = {"id": _id(ref), "size": 0, "lifetime": "constant"}
    else:
        raise ScheduleParseError(f"unknown binding: {ref}")
    allocation["size"] = max(int(allocation["size"]), required_size)
    allocations[ref] = allocation


def _parse_subspans(
    lines: list[str],
    values: dict[str, int],
    dynamic_refs: set[str],
    allocations: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
    aliases: dict[str, str],
) -> dict[str, dict[str, object]]:
    subspans: dict[str, dict[str, object]] = {}
    for line in lines:
        match = SUBSPAN_RE.match(line)
        if not match:
            continue
        base = _resolve_alias(match.group("base"), aliases)
        if base not in allocations:
            _ensure_external_allocation(base, 0, allocations, external_buffers)
        if base not in allocations:
            raise ScheduleParseError(f"unknown subspan base: {base}")
        subspans[match.group("result")] = {
            "base": base,
            "offset": _eval_index(match.group("offset"), values, dynamic_refs),
            "length": _eval_index(match.group("length"), values, dynamic_refs),
        }
    return subspans


def _parse_deallocations(lines: list[str]) -> list[str]:
    deallocations: list[str] = []
    for line in lines:
        if match := DEALLOCA_RE.search(line):
            deallocations.append(match.group("plain") or match.group("typed"))
    return deallocations


def _peak_live_transient_bytes(
    lines: list[str], allocations: dict[str, dict[str, object]]
) -> int:
    active: dict[str, int] = {}
    live_bytes = 0
    peak = 0

    for line in lines:
        if match := ALLOCA_RE.match(line):
            ref = match.group("result")
            allocation = allocations.get(ref)
            if allocation and allocation.get("lifetime") == "transient":
                size = int(allocation["size"])
                if ref not in active:
                    active[ref] = size
                    live_bytes += size
                    peak = max(peak, live_bytes)
            continue
        if match := DEALLOCA_RE.search(line):
            ref = match.group("plain") or match.group("typed")
            size = active.pop(ref, None)
            if size is not None:
                live_bytes -= size
    return peak


def _binding_parts(match: re.Match[str]) -> tuple[str, str, str]:
    ref = match.group("compact_ref") or match.group("typed_ref")
    offset = match.group("compact_offset") or match.group("typed_offset")
    length = match.group("compact_length") or match.group("typed_length")
    return ref, offset, length


def _resolve_binding_base(
    ref: str,
    aliases: dict[str, str],
    subspans: dict[str, dict[str, object]],
    values: dict[str, int] | None = None,
    indirect_slots: dict[int, dict[str, int | str]] | None = None,
) -> tuple[str, int, int | None]:
    if values is not None and indirect_slots is not None and ref in values:
        slot = indirect_slots.get(values[ref])
        if slot is not None:
            return str(slot["base"]), int(slot["offset"]), int(slot["length"])

    resolved_ref = _resolve_alias(ref, aliases)
    offset = 0
    seen: set[str] = set()
    while resolved_ref in subspans:
        if resolved_ref in seen:
            raise ScheduleParseError(f"cyclic subspan alias: {ref}")
        seen.add(resolved_ref)
        subspan = subspans[resolved_ref]
        offset += int(subspan["offset"])
        resolved_ref = _resolve_alias(str(subspan["base"]), aliases)
        if values is not None and indirect_slots is not None and resolved_ref in values:
            slot = indirect_slots.get(values[resolved_ref])
            if slot is not None:
                available_length = int(slot["length"]) - offset
                if available_length < 0:
                    raise ScheduleParseError(
                        f"subspan offset {offset} exceeds execute.indirect slot "
                        f"{values[resolved_ref]} length {slot['length']}"
                    )
                resolved_ref = str(slot["base"])
                offset += int(slot["offset"])
                return resolved_ref, offset, available_length
    return resolved_ref, offset, None


def _check_indirect_bounds(
    ref: str, offset: int, length: int, available_length: int | None
) -> None:
    if offset < 0 or length < 0:
        raise ScheduleParseError(f"negative binding range for {ref}: offset={offset}, length={length}")
    if available_length is not None and offset + length > available_length:
        raise ScheduleParseError(
            f"binding {ref}[{offset}, {length}] exceeds execute.indirect slot "
            f"length {available_length}"
        )


def _parse_dispatch_block(
    block: str,
    values: dict[str, int],
    dynamic_refs: set[str],
    allocations: dict[str, dict[str, object]],
    subspans: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
    aliases: dict[str, str],
    indirect_slots: dict[int, dict[str, int | str]] | None = None,
) -> dict[str, object]:
    ordinal_match = ORDINAL_RE.search(block)
    workgroups_match = WORKGROUPS_RE.search(block)
    constants_match = CONSTANTS_RE.search(block)
    bindings_match = BINDINGS_RE.search(block)
    if not ordinal_match or not workgroups_match or not bindings_match:
        raise ScheduleParseError(f"unsupported dispatch form: {block[:160]}")

    workgroups = [
        _eval_index(token, values, dynamic_refs)
        for token in _split_value_list(workgroups_match.group("values"))
    ]
    if len(workgroups) != 3:
        raise ScheduleParseError(f"unsupported workgroup rank: {workgroups}")

    constants = []
    if constants_match:
        constants = [
            _eval_u32_constant(token, values, dynamic_refs)
            for token in _split_value_list(constants_match.group("values"))
        ]

    bindings: list[dict[str, object]] = []
    for binding_ordinal, binding_match in enumerate(
        BINDING_RE.finditer(bindings_match.group("body"))
    ):
        ref, offset_ref, length_ref = _binding_parts(binding_match)
        offset = _eval_index(offset_ref, values, dynamic_refs)
        length = _eval_index(length_ref, values, dynamic_refs)
        base_ref, base_offset, available_length = _resolve_binding_base(
            ref,
            aliases,
            subspans,
            values,
            indirect_slots,
        )
        _check_indirect_bounds(ref, offset, length, available_length)
        _ensure_external_allocation(
            base_ref, base_offset + offset + length, allocations, external_buffers
        )
        if base_ref not in allocations:
            raise ScheduleParseError(f"unknown binding: {ref}")
        bindings.append(
            {
                "ordinal": binding_ordinal,
                "base": _id(base_ref),
                "offset": base_offset + offset,
                "length": length,
                "access": "rw",
            }
        )

    return {
        "ordinal": _eval_index(ordinal_match.group("ordinal"), values, dynamic_refs),
        "workgroups": workgroups,
        "constants": constants,
        "bindings": bindings,
    }


def _parse_fill_command(
    line: str,
    values: dict[str, int],
    dynamic_refs: set[str],
    allocations: dict[str, dict[str, object]],
    subspans: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
    aliases: dict[str, str],
    indirect_slots: dict[int, dict[str, int | str]] | None = None,
) -> dict[str, object]:
    match = FILL_RE.match(line)
    if not match:
        raise ScheduleParseError(f"unsupported fill_buffer form: {line.strip()}")

    ref = match.group("ref")
    offset = _eval_index(match.group("offset"), values, dynamic_refs)
    length = _eval_index(match.group("length"), values, dynamic_refs)
    pattern = _eval_index(match.group("pattern"), values, dynamic_refs) & 0xFF
    base_ref, base_offset, available_length = _resolve_binding_base(
        ref,
        aliases,
        subspans,
        values,
        indirect_slots,
    )
    _check_indirect_bounds(ref, offset, length, available_length)
    _ensure_external_allocation(
        base_ref, base_offset + offset + length, allocations, external_buffers
    )
    if base_ref not in allocations:
        raise ScheduleParseError(f"unknown fill target: {ref}")

    return {
        "type": "fill",
        "target": {
            "base": _id(base_ref),
            "offset": base_offset + offset,
            "length": length,
            "access": "write",
        },
        "pattern": pattern,
    }


def _parse_commands_and_dispatches(
    lines: list[str],
    values: dict[str, int],
    dynamic_refs: set[str],
    allocations: dict[str, dict[str, object]],
    subspans: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
    aliases: dict[str, str],
    indirect_slots: dict[int, dict[str, int | str]] | None = None,
) -> tuple[list[dict[str, object]], list[dict[str, object]]]:
    dispatches: list[dict[str, object]] = []
    commands: list[dict[str, object]] = []

    for kind, payload in _command_blocks(lines):
        if kind == "dispatch":
            dispatch = _parse_dispatch_block(
                payload,
                values,
                dynamic_refs,
                allocations,
                subspans,
                external_buffers,
                aliases,
                indirect_slots,
            )
            commands.append({"type": "dispatch", "dispatch_index": len(dispatches)})
            dispatches.append(dispatch)
        else:
            commands.append(
                _parse_fill_command(
                    payload,
                    values,
                    dynamic_refs,
                    allocations,
                    subspans,
                    external_buffers,
                    aliases,
                    indirect_slots,
                )
            )

    return dispatches, commands


def _parse_indirect_binding_slots(
    lines: list[str],
    values: dict[str, int],
    dynamic_refs: set[str],
    allocations: dict[str, dict[str, object]],
    subspans: dict[str, dict[str, object]],
    external_buffers: dict[str, dict[str, object]],
    aliases: dict[str, str],
) -> dict[int, dict[str, int | str]]:
    blocks = _execute_indirect_blocks(lines)
    if not blocks:
        return {}
    if len(blocks) != 1:
        raise ScheduleParseError("multiple execute.indirect command buffers are unsupported")

    bindings_match = BINDINGS_RE.search(blocks[0])
    if not bindings_match:
        raise ScheduleParseError(f"unsupported execute.indirect form: {blocks[0][:160]}")

    slots: dict[int, dict[str, int | str]] = {}
    for slot_ordinal, binding_match in enumerate(BINDING_RE.finditer(bindings_match.group("body"))):
        ref, offset_ref, length_ref = _binding_parts(binding_match)
        offset = _eval_index(offset_ref, values, dynamic_refs)
        length = _eval_index(length_ref, values, dynamic_refs)
        base_ref, base_offset, _ = _resolve_binding_base(ref, aliases, subspans)
        _ensure_external_allocation(
            base_ref,
            base_offset + offset + length,
            allocations,
            external_buffers,
        )
        slots[slot_ordinal] = {
            "base": base_ref,
            "offset": base_offset + offset,
            "length": length,
        }
    return slots


def _stream_transient_peak(path: Path, batch_size: int) -> int:
    peak = 0
    for allocation in dump_analyzer.scan_allocations(path, batch_size):
        if allocation.kind == "stream.resource.alloca" and allocation.size_value is not None:
            peak = max(peak, allocation.size_value)
    return peak


def _with_cross_checks(
    schedule: dict[str, object], stream_path: Path | None, batch_size: int
) -> dict[str, object]:
    warnings = list(schedule["warnings"])
    if stream_path is not None:
        stream_peak = _stream_transient_peak(stream_path, batch_size)
        hal_max_allocation = int(
            schedule.get("max_transient_allocation_bytes", schedule["peak_transient_bytes"])
        )
        hal_live_peak = int(schedule["peak_transient_bytes"])
        if stream_peak < hal_max_allocation:
            raise ScheduleParseError(
                f"stream transient peak {stream_peak} is smaller than HAL max allocation "
                f"{hal_max_allocation}"
            )
        if stream_peak != hal_max_allocation:
            warnings.append(
                f"stream transient peak {stream_peak} differs from HAL max allocation "
                f"{hal_max_allocation}"
            )
        if stream_peak != hal_live_peak:
            warnings.append(
                f"stream transient peak {stream_peak} differs from HAL live peak {hal_live_peak}"
            )
    schedule["warnings"] = warnings
    return schedule


def parse_hal_schedule(
    path: Path, batch_size: int = 1, stream_path: Path | None = None
) -> dict[str, object]:
    hal_path = Path(path)
    raw_lines = hal_path.read_text(encoding="utf-8", errors="replace").splitlines()
    main_lines = _main_function_lines(raw_lines)
    values, dim_refs = _parse_values(main_lines, batch_size)
    lines, aliases = _linearize_reachable_lines(main_lines, values)
    dynamic_refs = _dynamic_refs(lines)
    allocations_by_ref = _parse_allocations(lines, values, dynamic_refs)
    external_buffers = _external_buffers(lines)
    subspans = _parse_subspans(
        lines, values, dynamic_refs, allocations_by_ref, external_buffers, aliases
    )
    deallocations = _parse_deallocations(lines)
    for ref in deallocations:
        if ref not in allocations_by_ref:
            raise ScheduleParseError(f"unknown dealloca buffer: {ref}")

    dispatches, commands = _parse_commands_and_dispatches(
        lines,
        values,
        dynamic_refs,
        allocations_by_ref,
        subspans,
        external_buffers,
        aliases,
    )
    if not commands and _execute_indirect_blocks(lines):
        memoize_lines = _function_lines(raw_lines, "__main_memoize_apply")
        if memoize_lines is None:
            raise ScheduleParseError("execute.indirect command buffer producer not found")
        indirect_slots = _parse_indirect_binding_slots(
            lines,
            values,
            dynamic_refs,
            allocations_by_ref,
            subspans,
            external_buffers,
            aliases,
        )
        memoize_values, memoize_dim_refs = _parse_values(memoize_lines, batch_size)
        memoize_linear_lines, memoize_aliases = _linearize_reachable_lines(
            memoize_lines,
            memoize_values,
        )
        memoize_dynamic_refs = _dynamic_refs(memoize_linear_lines)
        external_buffers.update(_external_buffers(memoize_linear_lines))
        dispatches, commands = _parse_commands_and_dispatches(
            memoize_linear_lines,
            memoize_values,
            memoize_dynamic_refs,
            allocations_by_ref,
            subspans,
            external_buffers,
            memoize_aliases,
            indirect_slots,
        )
        dim_refs |= memoize_dim_refs
    allocations = list(allocations_by_ref.values())
    max_transient_allocation_bytes = max(
        (
            int(allocation["size"])
            for allocation in allocations
            if allocation.get("lifetime") == "transient"
        ),
        default=0,
    )
    peak_transient_bytes = _peak_live_transient_bytes(lines, allocations_by_ref)
    warnings = []
    if dim_refs:
        warnings.append(
            f"assumed batch_size={batch_size} for {len(dim_refs)} hal.buffer_view.dim value(s)"
        )
    schedule: dict[str, object] = {
        "schema": SCHEMA,
        "source": hal_path.name,
        "fixed_shape": True,
        "is_linear": True,
        "allocations": allocations,
        "dispatches": dispatches,
        "commands": commands,
        "max_transient_allocation_bytes": max_transient_allocation_bytes,
        "peak_transient_bytes": peak_transient_bytes,
        "warnings": warnings,
    }
    return _with_cross_checks(schedule, Path(stream_path) if stream_path else None, batch_size)


def count_native_dispatch_exports(nm_output: str) -> int:
    ordinals: set[int] = set()
    for match in re.finditer(r"dispatch[_$.-]*(\d+)(?:\b|[_$.-])", nm_output):
        ordinals.add(int(match.group(1)))
    return len(ordinals)


def check_native_export_count(
    schedule: dict[str, object], object_path: Path | None = None, nm_output: str | None = None
) -> list[str]:
    dispatches = schedule.get("dispatches", [])
    largest_ordinal = max((int(dispatch["ordinal"]) for dispatch in dispatches), default=-1)
    required = largest_ordinal + 1
    if required <= 0:
        return []
    if nm_output is None:
        if object_path is None:
            return []
        nm = os.environ.get("CMAKE_NM") or "llvm-nm"
        try:
            result = subprocess.run(
                [nm, str(object_path)],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=False,
            )
        except FileNotFoundError as exc:
            raise ScheduleParseError(f"nm tool not found: {nm}") from exc
        if result.returncode != 0:
            raise ScheduleParseError(result.stderr.strip() or f"nm failed for {object_path}")
        nm_output = result.stdout

    if "_linked_library_query" not in nm_output:
        raise ScheduleParseError("native executable export metadata missing linked_library_query")
    export_count = count_native_dispatch_exports(nm_output)
    if export_count < required:
        raise ScheduleParseError(
            f"native executable export count {export_count} is lower than required {required}"
        )
    return []


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--hal", required=True)
    parser.add_argument("--stream")
    parser.add_argument("--object")
    parser.add_argument("--output", required=True)
    parser.add_argument("--batch-size", type=int, default=1)
    args = parser.parse_args(argv)

    try:
        schedule = parse_hal_schedule(
            Path(args.hal),
            batch_size=args.batch_size,
            stream_path=Path(args.stream) if args.stream else None,
        )
        if args.object:
            warnings = check_native_export_count(schedule, object_path=Path(args.object))
            schedule["warnings"] = list(schedule["warnings"]) + warnings
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            json.dumps(schedule, indent=2, sort_keys=True) + "\n", encoding="utf-8"
        )
    except ScheduleParseError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
