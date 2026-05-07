#!/usr/bin/env python3
"""Verify static-direct command replay and liveness arena placement."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

try:
    from scripts import generate_static_direct_descriptor as generator
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import generate_static_direct_descriptor as generator


def _kib(byte_count: int) -> float:
    return round(byte_count / 1024.0, 3)


def _allocation_sizes(schedule: dict[str, Any]) -> dict[str, int]:
    sizes: dict[str, int] = {}
    for index, allocation in enumerate(schedule.get("allocations", [])):
        if not isinstance(allocation, dict):
            raise ValueError(f"allocation {index} must be an object")
        alloc_id = str(allocation.get("id", ""))
        if not alloc_id:
            raise ValueError(f"allocation {index} id must be present")
        if alloc_id in sizes:
            raise ValueError(f"duplicate allocation id {alloc_id!r}")
        sizes[alloc_id] = generator._u32(
            allocation.get("size"),
            f"allocation {alloc_id} size",
        )
    return sizes


def _command_slices(
    command: dict[str, Any],
    dispatches: list[dict[str, Any]],
    command_index: int,
) -> list[dict[str, Any]]:
    command_type = str(command.get("type", ""))
    if command_type == "dispatch":
        dispatch_index = generator._u32(
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
        return bindings
    if command_type == "fill":
        target = command.get("target")
        if not isinstance(target, dict):
            raise ValueError(f"command {command_index} fill target must be an object")
        return [target]
    raise ValueError(f"command {command_index} has unsupported type {command_type!r}")


def _check_slice_bounds(
    schedule: dict[str, Any],
    allocation_sizes: dict[str, int],
    commands: list[dict[str, Any]],
) -> None:
    dispatches = schedule["dispatches"]
    for command_index, command in enumerate(commands):
        for slice_index, slice_info in enumerate(
            _command_slices(command, dispatches, command_index)
        ):
            base = str(slice_info.get("base", ""))
            if base not in allocation_sizes:
                raise ValueError(
                    f"command {command_index} slice {slice_index} references "
                    f"unknown allocation {base!r}"
                )
            offset = generator._u32(
                slice_info.get("offset"),
                f"command {command_index} slice {slice_index} offset",
            )
            length = generator._u32(
                slice_info.get("length"),
                f"command {command_index} slice {slice_index} length",
            )
            if offset > allocation_sizes[base] or length > allocation_sizes[base] - offset:
                raise ValueError(
                    f"command {command_index} slice {slice_index} exceeds allocation "
                    f"{base!r}: offset={offset} length={length} size={allocation_sizes[base]}"
                )


def _check_dispatch_command_coverage(
    dispatches: list[dict[str, Any]],
    commands: list[dict[str, Any]],
) -> None:
    seen: dict[int, int] = {}
    for command_index, command in enumerate(commands):
        if str(command.get("type", "")) != "dispatch":
            continue
        dispatch_index = generator._u32(
            command.get("dispatch_index"),
            f"command {command_index} dispatch_index",
        )
        seen[dispatch_index] = seen.get(dispatch_index, 0) + 1

    expected = set(range(len(dispatches)))
    actual = set(seen)
    missing = sorted(expected - actual)
    extra = sorted(actual - expected)
    repeated = sorted(index for index, count in seen.items() if count != 1)
    if missing:
        raise ValueError(f"missing dispatch command(s): {missing}")
    if extra:
        raise ValueError(f"unknown dispatch command index(es): {extra}")
    if repeated:
        raise ValueError(f"repeated dispatch command index(es): {repeated}")


def _check_transient_conflicts(
    schedule: dict[str, Any],
    class_by_id: dict[str, str],
    offset_by_id: dict[str, int],
    commands: list[dict[str, Any]],
) -> None:
    dispatches = schedule["dispatches"]
    for command_index, command in enumerate(commands):
        active: list[tuple[str, int, int]] = []
        for slice_index, slice_info in enumerate(
            _command_slices(command, dispatches, command_index)
        ):
            base = str(slice_info.get("base", ""))
            if class_by_id.get(base) != "arena":
                continue
            offset = generator._u32(
                slice_info.get("offset"),
                f"command {command_index} slice {slice_index} offset",
            )
            length = generator._u32(
                slice_info.get("length"),
                f"command {command_index} slice {slice_index} length",
            )
            placed_offset = offset_by_id[base] + offset
            for other_base, other_offset, other_length in active:
                if (
                    base == other_base
                    and placed_offset == other_offset
                    and length == other_length
                ):
                    continue
                if generator._ranges_overlap(
                    placed_offset,
                    length,
                    other_offset,
                    other_length,
                ):
                    if base == other_base:
                        raise ValueError(
                            f"command {command_index} has overlapping slices within "
                            f"allocation {base!r}: {placed_offset}+{length} overlaps "
                            f"{other_offset}+{other_length}"
                        )
                    raise ValueError(
                        f"command {command_index} has overlapping arena ranges "
                        f"after placement: {base}@{placed_offset}+{length} overlaps "
                        f"{other_base}@{other_offset}+{other_length}"
                    )
            active.append((base, placed_offset, length))


def _check_transient_pairwise_placement(
    class_by_id: dict[str, str],
    offset_by_id: dict[str, int],
    allocation_sizes: dict[str, int],
    intervals: dict[str, tuple[int, int]],
) -> None:
    arena_ids = [
        alloc_id
        for alloc_id, alloc_class in class_by_id.items()
        if alloc_class == "arena" and alloc_id in intervals
    ]
    for lhs_index, lhs_id in enumerate(arena_ids):
        lhs_interval = intervals[lhs_id]
        lhs_offset = offset_by_id[lhs_id]
        lhs_size = allocation_sizes[lhs_id]
        for rhs_id in arena_ids[lhs_index + 1 :]:
            rhs_interval = intervals[rhs_id]
            if not generator._intervals_overlap(lhs_interval, rhs_interval):
                continue
            rhs_offset = offset_by_id[rhs_id]
            rhs_size = allocation_sizes[rhs_id]
            if generator._ranges_overlap(lhs_offset, lhs_size, rhs_offset, rhs_size):
                raise ValueError(
                    f"overlapping transient placements for live intervals: "
                    f"{lhs_id}@{lhs_offset}+{lhs_size} interval={lhs_interval} "
                    f"overlaps {rhs_id}@{rhs_offset}+{rhs_size} interval={rhs_interval}"
                )


def verify_schedule(
    schedule: dict[str, Any],
    *,
    require_explicit_commands: bool = True,
    allow_empty_commands: bool = False,
) -> dict[str, Any]:
    generator._validate_schedule(schedule)
    if require_explicit_commands and "commands" not in schedule:
        raise ValueError("schedule must contain an explicit command list")
    commands = generator._normalized_commands(schedule)
    if not commands and not allow_empty_commands:
        raise ValueError("empty command list is not descriptor-ready")

    allocation_sizes = _allocation_sizes(schedule)
    class_by_id, offset_by_id, byte_totals, max_arena_allocation = (
        generator._classify_allocations(
            schedule["allocations"],
            schedule["dispatches"],
            commands,
        )
    )
    _check_slice_bounds(schedule, allocation_sizes, commands)
    _check_dispatch_command_coverage(schedule["dispatches"], commands)
    _check_transient_conflicts(schedule, class_by_id, offset_by_id, commands)

    peak_transient = generator._u32(
        schedule.get("peak_transient_bytes", byte_totals["arena"]),
        "peak_transient_bytes",
    )
    arena_bytes = max(byte_totals["arena"], peak_transient, max_arena_allocation)
    if arena_bytes == byte_totals["arena"]:
        effective_arena_source = "packed_liveness"
    elif arena_bytes == peak_transient and peak_transient >= byte_totals["arena"]:
        effective_arena_source = "peak_transient"
    elif arena_bytes == max_arena_allocation:
        effective_arena_source = "max_arena_allocation"
    else:
        effective_arena_source = "packed_liveness"
    intervals = generator._transient_intervals(
        class_by_id,
        schedule["dispatches"],
        commands,
    )
    _check_transient_pairwise_placement(
        class_by_id,
        offset_by_id,
        allocation_sizes,
        intervals,
    )

    placements: dict[str, dict[str, Any]] = {}
    for alloc_id, alloc_class in class_by_id.items():
        if alloc_class != "arena":
            continue
        interval = intervals.get(alloc_id)
        placements[alloc_id] = {
            "offset": offset_by_id[alloc_id],
            "size": allocation_sizes[alloc_id],
            "size_kib": _kib(allocation_sizes[alloc_id]),
            "interval": list(interval) if interval else None,
        }

    return {
        "status": "ok",
        "source": schedule.get("source", ""),
        "dispatch_count": len(schedule["dispatches"]),
        "command_count": len(commands),
        "allocation_count": len(schedule["allocations"]),
        "input_bytes": byte_totals["input"],
        "input_kib": _kib(byte_totals["input"]),
        "output_bytes": byte_totals["output"],
        "output_kib": _kib(byte_totals["output"]),
        "const_bytes": byte_totals["const"],
        "const_kib": _kib(byte_totals["const"]),
        "arena_bytes": arena_bytes,
        "arena_kib": _kib(arena_bytes),
        "packed_arena_bytes": byte_totals["arena"],
        "packed_arena_kib": _kib(byte_totals["arena"]),
        "peak_transient_bytes": peak_transient,
        "peak_transient_kib": _kib(peak_transient),
        "effective_arena_source": effective_arena_source,
        "packed_vs_effective_delta_bytes": max(0, arena_bytes - byte_totals["arena"]),
        "packed_vs_effective_delta_kib": _kib(max(0, arena_bytes - byte_totals["arena"])),
        "max_arena_allocation_bytes": max_arena_allocation,
        "max_arena_allocation_kib": _kib(max_arena_allocation),
        "placements": placements,
        "warnings": list(schedule.get("warnings", [])),
    }


def _load_schedule(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("schedule JSON must contain an object")
    return data


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--schedule", required=True, type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--allow-empty-commands", action="store_true")
    parser.add_argument("--allow-synthesized-commands", action="store_true")
    args = parser.parse_args(argv)

    try:
        report = verify_schedule(
            _load_schedule(args.schedule),
            require_explicit_commands=not args.allow_synthesized_commands,
            allow_empty_commands=args.allow_empty_commands,
        )
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        parser.exit(1, f"error: {exc}\n")
    report_text = json.dumps(report, indent=2, sort_keys=True) + "\n"
    if args.output is None:
        print(report_text, end="")
    else:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(report_text, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
