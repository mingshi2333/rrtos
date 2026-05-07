#!/usr/bin/env python3
"""Generate static-direct descriptor C sources from an IREE HAL schedule JSON."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any


BUFFER_ENUM_BY_CLASS = {
    "input": "AI_STATIC_DIRECT_BUFFER_INPUT",
    "output": "AI_STATIC_DIRECT_BUFFER_OUTPUT",
    "const": "AI_STATIC_DIRECT_BUFFER_CONST",
    "arena": "AI_STATIC_DIRECT_BUFFER_ARENA",
}

ACCESS_BITS = {
    "ro": 1,
    "read": 1,
    "wo": 2,
    "write": 2,
    "rw": 3,
    "readwrite": 3,
    "read_write": 3,
}


def _c_identifier(name: str) -> str:
    ident = re.sub(r"[^0-9A-Za-z_]", "_", name)
    if not ident or ident[0].isdigit():
        ident = f"_{ident}"
    return ident


def _u32(value: Any, field: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value < 0:
        raise ValueError(f"{field} must be a non-negative integer")
    return value


def _allocation_class(allocation: dict[str, Any]) -> str:
    lifetime = str(allocation.get("lifetime", "")).lower()
    alloc_id = str(allocation.get("id", "")).lower()

    if lifetime in {"input", "parameter_input"} or alloc_id.startswith("input"):
        return "input"
    if lifetime in {"output", "parameter_output"} or alloc_id.startswith("output"):
        return "output"
    if lifetime in {"constant", "const", "rodata"} or alloc_id in {"const", "weights"}:
        return "const"
    if lifetime in {"transient", "arena", "scratch"} or alloc_id in {"arena", "scratch"}:
        return "arena"
    raise ValueError(
        f"unsupported allocation classification for {allocation.get('id')!r}"
    )


def _validate_schedule(schedule: dict[str, Any]) -> None:
    if schedule.get("fixed_shape") is not True:
        raise ValueError("fixed_shape must be true for static-direct generation")
    if schedule.get("is_linear") is not True:
        raise ValueError("is_linear must be true for static-direct generation")
    if not isinstance(schedule.get("allocations"), list):
        raise ValueError("allocations must be a list")
    if not isinstance(schedule.get("dispatches"), list):
        raise ValueError("dispatches must be a list")


def _align_up(value: int, alignment: int) -> int:
    if alignment <= 1:
        return value
    return ((value + alignment - 1) // alignment) * alignment


def _normalized_commands(schedule: dict[str, Any]) -> list[dict[str, Any]]:
    raw_commands = schedule.get("commands")
    if raw_commands is None:
        raw_commands = [
            {"type": "dispatch", "dispatch_index": i}
            for i in range(len(schedule["dispatches"]))
        ]
    if not isinstance(raw_commands, list):
        raise ValueError("commands must be a list")
    for command_index, command in enumerate(raw_commands):
        if not isinstance(command, dict):
            raise ValueError(f"command {command_index} must be an object")
    return raw_commands


def _command_allocation_ids(
    command: dict[str, Any],
    dispatches: list[dict[str, Any]],
    command_index: int,
) -> list[str]:
    command_type = str(command.get("type", ""))
    if command_type == "dispatch":
        dispatch_index = _u32(
            command.get("dispatch_index"),
            f"command {command_index} dispatch_index",
        )
        if dispatch_index >= len(dispatches):
            raise ValueError(
                f"command {command_index} references unknown dispatch {dispatch_index}"
            )
        bindings = dispatches[dispatch_index].get("bindings", [])
        if not isinstance(bindings, list):
            raise ValueError(f"dispatch {dispatch_index} bindings must be a list")
        return [str(binding.get("base", "")) for binding in bindings]
    if command_type == "fill":
        target = command.get("target")
        if not isinstance(target, dict):
            raise ValueError(f"command {command_index} fill target must be an object")
        return [str(target.get("base", ""))]
    raise ValueError(f"command {command_index} has unsupported type {command_type!r}")


def _transient_intervals(
    class_by_id: dict[str, str],
    dispatches: list[dict[str, Any]],
    commands: list[dict[str, Any]],
) -> dict[str, tuple[int, int]]:
    intervals: dict[str, tuple[int, int]] = {}

    for command_index, command in enumerate(commands):
        for alloc_id in _command_allocation_ids(command, dispatches, command_index):
            if class_by_id.get(alloc_id) != "arena":
                continue
            start, end = intervals.get(alloc_id, (command_index, command_index + 1))
            intervals[alloc_id] = (min(start, command_index), max(end, command_index + 1))

    return intervals


def _intervals_overlap(lhs: tuple[int, int], rhs: tuple[int, int]) -> bool:
    return lhs[0] < rhs[1] and rhs[0] < lhs[1]


def _ranges_overlap(lhs_offset: int, lhs_size: int, rhs_offset: int, rhs_size: int) -> bool:
    return lhs_offset < rhs_offset + rhs_size and rhs_offset < lhs_offset + lhs_size


def _place_transient_allocations(
    arena_allocations: list[tuple[str, int]],
    intervals: dict[str, tuple[int, int]],
) -> tuple[dict[str, int], int, int]:
    offset_by_id: dict[str, int] = {}
    placed: list[tuple[str, int, int, tuple[int, int]]] = []
    max_arena_allocation = 0
    arena_bytes = 0

    for alloc_id, size in arena_allocations:
        max_arena_allocation = max(max_arena_allocation, size)
        interval = intervals.get(alloc_id)
        if interval is None:
            offset_by_id[alloc_id] = 0
            continue

        candidate = 0
        while True:
            candidate = _align_up(candidate, 64)
            next_candidate: int | None = None
            for _placed_id, placed_offset, placed_size, placed_interval in placed:
                if not _intervals_overlap(interval, placed_interval):
                    continue
                if _ranges_overlap(candidate, size, placed_offset, placed_size):
                    next_candidate = placed_offset + placed_size
                    break
            if next_candidate is None:
                break
            candidate = next_candidate

        offset_by_id[alloc_id] = candidate
        placed.append((alloc_id, candidate, size, interval))
        arena_bytes = max(arena_bytes, candidate + size)

    return offset_by_id, arena_bytes, max_arena_allocation


def _classify_allocations(
    allocations: list[dict[str, Any]],
    dispatches: list[dict[str, Any]],
    commands: list[dict[str, Any]],
) -> tuple[dict[str, str], dict[str, int], dict[str, int], int]:
    class_by_id: dict[str, str] = {}
    offset_by_id: dict[str, int] = {}
    byte_totals = {"input": 0, "output": 0, "const": 0, "arena": 0}
    max_arena_allocation = 0
    arena_allocations: list[tuple[str, int]] = []

    for allocation in allocations:
        alloc_id = str(allocation.get("id", ""))
        if not alloc_id:
            raise ValueError("allocation id must be present")
        alloc_class = _allocation_class(allocation)
        size = _u32(allocation.get("size"), f"allocation {alloc_id} size")
        class_by_id[alloc_id] = alloc_class
        if alloc_class == "arena":
            arena_allocations.append((alloc_id, size))
        else:
            offset_by_id[alloc_id] = byte_totals[alloc_class]
            byte_totals[alloc_class] += size

    arena_offsets, arena_bytes, max_arena_allocation = _place_transient_allocations(
        arena_allocations,
        _transient_intervals(class_by_id, dispatches, commands),
    )
    offset_by_id.update(arena_offsets)
    byte_totals["arena"] = arena_bytes

    return class_by_id, offset_by_id, byte_totals, max_arena_allocation


def _allocation_sizes(allocations: list[dict[str, Any]]) -> dict[str, int]:
    sizes: dict[str, int] = {}
    for allocation in allocations:
        alloc_id = str(allocation.get("id", ""))
        if not alloc_id:
            raise ValueError("allocation id must be present")
        sizes[alloc_id] = _u32(allocation.get("size"), f"allocation {alloc_id} size")
    return sizes


def _apply_memory_plan(
    *,
    memory_plan: dict[str, Any],
    class_by_id: dict[str, str],
    allocation_sizes: dict[str, int],
    offset_by_id: dict[str, int],
    byte_totals: dict[str, int],
) -> int:
    placements = memory_plan.get("placements")
    if not isinstance(placements, dict):
        raise ValueError("memory plan must contain placements")

    for total_field, total_key in (
        ("input_bytes", "input"),
        ("output_bytes", "output"),
        ("const_bytes", "const"),
    ):
        if total_field in memory_plan:
            expected = _u32(memory_plan[total_field], f"memory plan {total_field}")
            if expected != byte_totals[total_key]:
                raise ValueError(
                    f"memory plan {total_field}={expected} does not match "
                    f"schedule {byte_totals[total_key]}"
                )

    arena_bytes = _u32(memory_plan.get("arena_bytes"), "memory plan arena_bytes")
    for alloc_id, alloc_class in class_by_id.items():
        if alloc_class != "arena":
            continue
        placement = placements.get(alloc_id)
        if not isinstance(placement, dict):
            raise ValueError(f"missing memory-plan placement for {alloc_id!r}")
        placement_size = _u32(
            placement.get("size"),
            f"memory plan placement {alloc_id!r} size",
        )
        if placement_size != allocation_sizes[alloc_id]:
            raise ValueError(
                f"memory plan placement {alloc_id!r} size={placement_size} "
                f"does not match allocation size {allocation_sizes[alloc_id]}"
            )
        placement_offset = _u32(
            placement.get("offset"),
            f"memory plan placement {alloc_id!r} offset",
        )
        if placement_offset > arena_bytes or placement_size > arena_bytes - placement_offset:
            raise ValueError(
                f"memory plan placement {alloc_id!r} exceeds arena_bytes {arena_bytes}"
            )
        offset_by_id[alloc_id] = placement_offset

    byte_totals["arena"] = arena_bytes
    return arena_bytes


def _render_header(
    model_ident: str,
    byte_totals: dict[str, int],
    arena_bytes: int,
    dispatch_count: int,
    command_count: int,
) -> str:
    guard = f"{model_ident.upper()}_STATIC_DIRECT_DESC_H"
    return "\n".join(
        [
            f"#ifndef {guard}",
            f"#define {guard}",
            "",
            '#include "ai_static_direct.h"',
            "",
            f"#define {model_ident.upper()}_INPUT_BYTES {byte_totals['input']}u",
            f"#define {model_ident.upper()}_OUTPUT_BYTES {byte_totals['output']}u",
            f"#define {model_ident.upper()}_CONST_BYTES {byte_totals['const']}u",
            f"#define {model_ident.upper()}_ARENA_BYTES {arena_bytes}u",
            f"#define {model_ident.upper()}_DISPATCH_COUNT {dispatch_count}u",
            f"#define {model_ident.upper()}_COMMAND_COUNT {command_count}u",
            "",
            f"extern const ai_static_direct_descriptor_t {model_ident}_descriptor;",
            "",
            f"#endif /* {guard} */",
            "",
        ]
    )


def render_descriptor(
    schedule: dict[str, Any],
    *,
    model_name: str,
    entry: str,
    header_name: str,
    native_header: str | None = None,
    native_object: str | None = None,
    memory_plan: dict[str, Any] | None = None,
) -> tuple[str, str]:
    _validate_schedule(schedule)
    model_ident = _c_identifier(model_name)
    raw_commands = _normalized_commands(schedule)
    class_by_id, offset_by_id, byte_totals, max_arena_allocation = _classify_allocations(
        schedule["allocations"],
        schedule["dispatches"],
        raw_commands,
    )

    if memory_plan is None:
        peak_transient = _u32(
            schedule.get("peak_transient_bytes", byte_totals["arena"]),
            "peak_transient_bytes",
        )
        arena_bytes = max(byte_totals["arena"], peak_transient, max_arena_allocation)
    else:
        arena_bytes = _apply_memory_plan(
            memory_plan=memory_plan,
            class_by_id=class_by_id,
            allocation_sizes=_allocation_sizes(schedule["allocations"]),
            offset_by_id=offset_by_id,
            byte_totals=byte_totals,
        )

    lines: list[str] = [
        "/* Generated by scripts/generate_static_direct_descriptor.py. */",
        f"/* Source schedule: {schedule.get('source', '')} */",
    ]
    if native_header is not None:
        lines.append(f"/* Native header: {native_header} */")
    if native_object is not None:
        lines.append(f"/* Native object: {native_object} */")
    lines.extend(
        [
            f'#include "{Path(header_name).name}"',
            "",
            "static const ai_static_direct_buffer_base_t "
            f"{model_ident}_buffer_bases[] = {{",
            "    AI_STATIC_DIRECT_BUFFER_INPUT,",
            "    AI_STATIC_DIRECT_BUFFER_OUTPUT,",
            "    AI_STATIC_DIRECT_BUFFER_CONST,",
            "    AI_STATIC_DIRECT_BUFFER_ARENA,",
            "};",
            "",
        ]
    )

    dispatch_entries: list[str] = []
    for dispatch_index, dispatch in enumerate(schedule["dispatches"]):
        ordinal = _u32(dispatch.get("ordinal"), f"dispatch {dispatch_index} ordinal")
        workgroups = dispatch.get("workgroups")
        if not (
            isinstance(workgroups, list)
            and len(workgroups) == 3
            and all(isinstance(item, int) and item >= 0 for item in workgroups)
        ):
            raise ValueError(f"dispatch {dispatch_index} workgroups must be three integers")

        constants = dispatch.get("constants", [])
        if not isinstance(constants, list):
            raise ValueError(f"dispatch {dispatch_index} constants must be a list")
        for constant_index, constant in enumerate(constants):
            _u32(constant, f"dispatch {dispatch_index} constant {constant_index}")

        const_name = f"{model_ident}_dispatch_{dispatch_index}_constants"
        if constants:
            lines.append(f"static const uint32_t {const_name}[] = {{")
            for constant in constants:
                lines.append(f"    {constant}u,")
            lines.append("};")
        else:
            const_name = "0"
        lines.append("")

        bindings = dispatch.get("bindings", [])
        if not isinstance(bindings, list):
            raise ValueError(f"dispatch {dispatch_index} bindings must be a list")
        binding_name = f"{model_ident}_dispatch_{dispatch_index}_bindings"
        lines.append(f"static const ai_static_direct_slice_t {binding_name}[] = {{")
        for binding_index, binding in enumerate(bindings):
            base = str(binding.get("base", ""))
            if base not in class_by_id:
                raise ValueError(
                    f"dispatch {dispatch_index} binding {binding_index} "
                    f"references unknown allocation {base!r}"
                )
            access = str(binding.get("access", "rw")).lower()
            if access not in ACCESS_BITS:
                raise ValueError(
                    f"dispatch {dispatch_index} binding {binding_index} "
                    f"has unsupported access {binding.get('access')!r}"
                )
            bind_ordinal = _u32(
                binding.get("ordinal", binding_index),
                f"dispatch {dispatch_index} binding {binding_index} ordinal",
            )
            offset = _u32(
                binding.get("offset"),
                f"dispatch {dispatch_index} binding {binding_index} offset",
            )
            length = _u32(
                binding.get("length"),
                f"dispatch {dispatch_index} binding {binding_index} length",
            )
            lines.extend(
                [
                    "    {",
                    f"        .base = {BUFFER_ENUM_BY_CLASS[class_by_id[base]]},",
                    f"        .index = {bind_ordinal}u,",
                    f"        .offset = {offset_by_id[base] + offset}u,",
                    f"        .length = {length}u,",
                    f"        .access = {ACCESS_BITS[access]}u,",
                    "    },",
                ]
            )
        lines.append("};")
        lines.append("")

        dispatch_entries.extend(
            [
                "    {",
                f"        .ordinal = {ordinal}u,",
                f"        .workgroups = {{{workgroups[0]}u, {workgroups[1]}u, {workgroups[2]}u}},",
                f"        .constants = {const_name},",
                f"        .constant_count = {len(constants)}u,",
                f"        .bindings = {binding_name},",
                f"        .binding_count = {len(bindings)}u,",
                "    },",
            ]
        )

    lines.append(f"const ai_static_direct_dispatch_t {model_ident}_dispatches[] = {{")
    lines.extend(dispatch_entries)
    lines.append("};")
    lines.append("")

    command_entries: list[str] = []
    for command_index, command in enumerate(raw_commands):
        if not isinstance(command, dict):
            raise ValueError(f"command {command_index} must be an object")
        command_type = str(command.get("type", ""))
        if command_type == "dispatch":
            dispatch_index = _u32(
                command.get("dispatch_index"),
                f"command {command_index} dispatch_index",
            )
            if dispatch_index >= len(schedule["dispatches"]):
                raise ValueError(
                    f"command {command_index} references unknown dispatch {dispatch_index}"
                )
            command_entries.extend(
                [
                    "    {",
                    "        .type = AI_STATIC_DIRECT_COMMAND_DISPATCH,",
                    "        .config.dispatch = {",
                    f"            .dispatch = &{model_ident}_dispatches[{dispatch_index}],",
                    "        },",
                    "    },",
                ]
            )
        elif command_type == "fill":
            target = command.get("target")
            if not isinstance(target, dict):
                raise ValueError(f"command {command_index} fill target must be an object")
            base = str(target.get("base", ""))
            if base not in class_by_id:
                raise ValueError(
                    f"command {command_index} fill references unknown allocation {base!r}"
                )
            offset = _u32(target.get("offset"), f"command {command_index} fill offset")
            length = _u32(target.get("length"), f"command {command_index} fill length")
            pattern = _u32(command.get("pattern"), f"command {command_index} fill pattern")
            command_entries.extend(
                [
                    "    {",
                    "        .type = AI_STATIC_DIRECT_COMMAND_FILL,",
                    "        .config.fill = {",
                    "            .target = {",
                    f"                .base = {BUFFER_ENUM_BY_CLASS[class_by_id[base]]},",
                    "                .index = 0u,",
                    f"                .offset = {offset_by_id[base] + offset}u,",
                    f"                .length = {length}u,",
                    "                .access = 2u,",
                    "            },",
                    f"            .pattern = {pattern & 0xFF}u,",
                    "        },",
                    "    },",
                ]
            )
        else:
            raise ValueError(f"command {command_index} has unsupported type {command_type!r}")

    lines.append(f"const ai_static_direct_command_t {model_ident}_commands[] = {{")
    lines.extend(command_entries)
    lines.append("};")
    lines.append("")
    lines.extend(
        [
            f"const ai_static_direct_descriptor_t {model_ident}_descriptor = {{",
            f"    .model_name = \"{model_name}\",",
            f"    .entry_name = \"{entry}\",",
            f"    .input_bytes = {byte_totals['input']}u,",
            f"    .output_bytes = {byte_totals['output']}u,",
            f"    .const_bytes = {byte_totals['const']}u,",
            f"    .arena_bytes = {arena_bytes}u,",
            "    .arena_alignment = 16u,",
            f"    .dispatches = {model_ident}_dispatches,",
            f"    .dispatch_count = {len(schedule['dispatches'])}u,",
            f"    .commands = {model_ident}_commands,",
            f"    .command_count = {len(raw_commands)}u,",
            "    .unsupported_feature_flags = 0u,",
            f"    .buffer_bases = {model_ident}_buffer_bases,",
            f"    .buffer_base_count = 4u,",
            "};",
            "",
        ]
    )

    return "\n".join(lines), _render_header(
        model_ident,
        byte_totals,
        arena_bytes,
        len(schedule["dispatches"]),
        len(raw_commands),
    )


def generate_descriptor(
    schedule: dict[str, Any],
    *,
    model_name: str,
    entry: str,
    output_c: Path,
    output_h: Path,
    native_header: Path | None = None,
    native_object: Path | None = None,
    memory_plan: dict[str, Any] | None = None,
) -> None:
    c_text, h_text = render_descriptor(
        schedule,
        model_name=model_name,
        entry=entry,
        header_name=output_h.name,
        native_header=str(native_header) if native_header is not None else None,
        native_object=str(native_object) if native_object is not None else None,
        memory_plan=memory_plan,
    )
    output_c.parent.mkdir(parents=True, exist_ok=True)
    output_h.parent.mkdir(parents=True, exist_ok=True)
    output_c.write_text(c_text, encoding="ascii")
    output_h.write_text(h_text, encoding="ascii")


def _load_schedule(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("schedule JSON must contain an object")
    return data


def _load_memory_plan(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("memory plan JSON must contain an object")
    return data


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Generate static-direct descriptor C/H files from a HAL schedule JSON."
    )
    parser.add_argument("--schedule", required=True, type=Path)
    parser.add_argument("--model-name", required=True)
    parser.add_argument("--entry", required=True)
    parser.add_argument("--output-c", required=True, type=Path)
    parser.add_argument("--output-h", required=True, type=Path)
    parser.add_argument("--header", type=Path)
    parser.add_argument("--object", type=Path)
    parser.add_argument("--memory-plan", type=Path)
    args = parser.parse_args(argv)

    try:
        generate_descriptor(
            _load_schedule(args.schedule),
            model_name=args.model_name,
            entry=args.entry,
            output_c=args.output_c,
            output_h=args.output_h,
            native_header=args.header,
            native_object=args.object,
            memory_plan=(
                _load_memory_plan(args.memory_plan)
                if args.memory_plan is not None
                else None
            ),
        )
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        parser.exit(1, f"error: {exc}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
