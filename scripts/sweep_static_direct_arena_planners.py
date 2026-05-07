#!/usr/bin/env python3
"""Compare static-direct transient arena placement heuristics."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import json
from pathlib import Path
from typing import Any, Callable

try:
    from scripts import generate_static_direct_descriptor as generator
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import generate_static_direct_descriptor as generator


ALIGNMENT = 64


@dataclass(frozen=True)
class TransientAllocation:
    alloc_id: str
    size: int
    start: int
    end: int


@dataclass(frozen=True)
class Placement:
    alloc_id: str
    offset: int
    size: int
    start: int
    end: int


def _kib(value: int) -> float:
    return round(value / 1024.0, 3)


def _load_schedule(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def _align_up(value: int, alignment: int = ALIGNMENT) -> int:
    if alignment <= 1:
        return value
    return ((value + alignment - 1) // alignment) * alignment


def _intervals_overlap(lhs: TransientAllocation, rhs: Placement) -> bool:
    return lhs.start < rhs.end and rhs.start < lhs.end


def _ranges_overlap(lhs_offset: int, lhs_size: int, rhs_offset: int, rhs_size: int) -> bool:
    return lhs_offset < rhs_offset + rhs_size and rhs_offset < lhs_offset + lhs_size


def _transient_allocations(schedule: dict[str, Any]) -> tuple[list[TransientAllocation], int]:
    generator._validate_schedule(schedule)
    commands = generator._normalized_commands(schedule)
    dispatches = schedule["dispatches"]
    class_by_id: dict[str, str] = {}
    sizes_by_id: dict[str, int] = {}
    max_single = 0

    for allocation in schedule["allocations"]:
        alloc_id = str(allocation.get("id", ""))
        alloc_class = generator._allocation_class(allocation)
        size = generator._u32(allocation.get("size"), f"allocation {alloc_id} size")
        class_by_id[alloc_id] = alloc_class
        if alloc_class == "arena":
            sizes_by_id[alloc_id] = size
            max_single = max(max_single, size)

    intervals = generator._transient_intervals(class_by_id, dispatches, commands)
    allocations = [
        TransientAllocation(alloc_id, sizes_by_id[alloc_id], start, end)
        for alloc_id, (start, end) in intervals.items()
    ]
    return allocations, max_single


def _pack_first_fit(allocations: list[TransientAllocation]) -> tuple[int, list[Placement]]:
    placed: list[Placement] = []
    arena_bytes = 0

    for allocation in allocations:
        candidate = 0
        while True:
            candidate = _align_up(candidate)
            next_candidate: int | None = None
            for placed_allocation in placed:
                if not _intervals_overlap(allocation, placed_allocation):
                    continue
                if _ranges_overlap(
                    candidate,
                    allocation.size,
                    placed_allocation.offset,
                    placed_allocation.size,
                ):
                    next_candidate = placed_allocation.offset + placed_allocation.size
                    break
            if next_candidate is None:
                break
            candidate = next_candidate

        placed.append(
            Placement(
                allocation.alloc_id,
                candidate,
                allocation.size,
                allocation.start,
                allocation.end,
            )
        )
        arena_bytes = max(arena_bytes, candidate + allocation.size)

    return arena_bytes, placed


def _pack_best_fit(allocations: list[TransientAllocation]) -> tuple[int, list[Placement]]:
    placed: list[Placement] = []
    arena_bytes = 0

    for allocation in allocations:
        candidates = {0}
        for placed_allocation in placed:
            if _intervals_overlap(allocation, placed_allocation):
                candidates.add(placed_allocation.offset + placed_allocation.size)

        valid_offsets: list[int] = []
        for candidate in sorted(_align_up(candidate) for candidate in candidates):
            if all(
                not _intervals_overlap(allocation, placed_allocation)
                or not _ranges_overlap(
                    candidate,
                    allocation.size,
                    placed_allocation.offset,
                    placed_allocation.size,
                )
                for placed_allocation in placed
            ):
                valid_offsets.append(candidate)

        if not valid_offsets:
            raise ValueError(f"no valid arena placement for {allocation.alloc_id}")

        offset = min(valid_offsets, key=lambda value: (max(arena_bytes, value + allocation.size), value))
        placed.append(
            Placement(
                allocation.alloc_id,
                offset,
                allocation.size,
                allocation.start,
                allocation.end,
            )
        )
        arena_bytes = max(arena_bytes, offset + allocation.size)

    return arena_bytes, placed


def _pack_linear_no_reuse(allocations: list[TransientAllocation]) -> tuple[int, list[Placement]]:
    placed: list[Placement] = []
    cursor = 0
    for allocation in allocations:
        cursor = _align_up(cursor)
        placed.append(
            Placement(
                allocation.alloc_id,
                cursor,
                allocation.size,
                allocation.start,
                allocation.end,
            )
        )
        cursor += allocation.size
    return cursor, placed


def _ordered(
    allocations: list[TransientAllocation],
    key: Callable[[TransientAllocation], tuple[Any, ...]],
) -> list[TransientAllocation]:
    return sorted(allocations, key=key)


def evaluate_schedule(schedule: dict[str, Any], name: str) -> dict[str, Any]:
    allocations, max_single = _transient_allocations(schedule)
    input_order = {allocation.alloc_id: index for index, allocation in enumerate(allocations)}
    peak_transient = generator._u32(
        schedule.get("peak_transient_bytes", 0),
        "peak_transient_bytes",
    )
    if peak_transient == 0:
        peak_transient = sum(allocation.size for allocation in allocations)

    planner_inputs: dict[str, tuple[str, list[TransientAllocation]]] = {
        "linear_no_reuse": ("linear", allocations),
        "input_order_first_fit": ("first_fit", allocations),
        "size_desc_first_fit": (
            "first_fit",
            _ordered(
                allocations,
                lambda allocation: (
                    -allocation.size,
                    allocation.start,
                    allocation.end,
                    input_order[allocation.alloc_id],
                ),
            ),
        ),
        "start_size_first_fit": (
            "first_fit",
            _ordered(
                allocations,
                lambda allocation: (
                    allocation.start,
                    -allocation.size,
                    allocation.end,
                    input_order[allocation.alloc_id],
                ),
            ),
        ),
        "size_desc_best_fit": (
            "best_fit",
            _ordered(
                allocations,
                lambda allocation: (
                    -allocation.size,
                    allocation.start,
                    allocation.end,
                    input_order[allocation.alloc_id],
                ),
            ),
        ),
    }

    planners: dict[str, dict[str, Any]] = {}
    for planner_name, (packer_name, ordered_allocations) in planner_inputs.items():
        if packer_name == "linear":
            packed_bytes, placements = _pack_linear_no_reuse(ordered_allocations)
        elif packer_name == "first_fit":
            packed_bytes, placements = _pack_first_fit(ordered_allocations)
        elif packer_name == "best_fit":
            packed_bytes, placements = _pack_best_fit(ordered_allocations)
        else:  # pragma: no cover - defensive guard.
            raise ValueError(f"unknown packer {packer_name}")

        effective_bytes = max(packed_bytes, peak_transient, max_single)
        planners[planner_name] = {
            "packed_bytes": packed_bytes,
            "packed_kib": _kib(packed_bytes),
            "effective_bytes": effective_bytes,
            "effective_kib": _kib(effective_bytes),
            "placement_count": len(placements),
        }

    winner = min(
        planners,
        key=lambda planner_name: (
            planners[planner_name]["effective_bytes"],
            planners[planner_name]["packed_bytes"],
            planner_name,
        ),
    )
    current = planners["input_order_first_fit"]
    linear = planners["linear_no_reuse"]

    return {
        "name": name,
        "allocation_count": len(allocations),
        "peak_transient_bytes": peak_transient,
        "peak_transient_kib": _kib(peak_transient),
        "max_single_allocation_bytes": max_single,
        "max_single_allocation_kib": _kib(max_single),
        "current_planner": "input_order_first_fit",
        "winner": winner,
        "current_effective_kib": current["effective_kib"],
        "winner_effective_kib": planners[winner]["effective_kib"],
        "savings_vs_current_kib": _kib(
            current["effective_bytes"] - planners[winner]["effective_bytes"]
        ),
        "savings_vs_linear_kib": _kib(
            linear["effective_bytes"] - planners[winner]["effective_bytes"]
        ),
        "planners": planners,
    }


def render_markdown(results: list[dict[str, Any]]) -> str:
    lines = [
        "# Static-Direct Arena Planner Sweep",
        "",
        "| Model | Allocs | Linear KiB | Current KiB | Best KiB | Best Planner | Save vs Current KiB | Save vs Linear KiB |",
        "| --- | ---: | ---: | ---: | ---: | --- | ---: | ---: |",
    ]
    for result in results:
        planners = result["planners"]
        linear = planners["linear_no_reuse"]["effective_kib"]
        current = result["current_effective_kib"]
        best = result["winner_effective_kib"]
        lines.append(
            f"| `{result['name']}` | {result['allocation_count']} | {linear:.3f} | "
            f"{current:.3f} | {best:.3f} | `{result['winner']}` | "
            f"{result['savings_vs_current_kib']:.3f} | {result['savings_vs_linear_kib']:.3f} |"
        )

    lines.extend(
        [
            "",
            "Planner definitions:",
            "",
            "- `linear_no_reuse`: place every transient allocation sequentially.",
            "- `input_order_first_fit`: current generator behavior.",
            "- `size_desc_first_fit`: first-fit after placing larger live ranges first.",
            "- `start_size_first_fit`: first-fit by lifetime start, with larger ties first.",
            "- `size_desc_best_fit`: larger live ranges first, choosing the candidate that minimizes arena growth.",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--schedule", action="append", required=True, help="HAL schedule JSON")
    parser.add_argument("--output", required=True, help="Markdown report path")
    parser.add_argument("--json-output", help="Optional JSON report path")
    args = parser.parse_args()

    results = []
    for schedule_arg in args.schedule:
        path = Path(schedule_arg)
        results.append(evaluate_schedule(_load_schedule(path), path.stem))

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(render_markdown(results), encoding="utf-8")

    if args.json_output:
        json_path = Path(args.json_output)
        json_path.parent.mkdir(parents=True, exist_ok=True)
        json_path.write_text(json.dumps(results, indent=2, sort_keys=True) + "\n", encoding="utf-8")

    print(f"Wrote {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
