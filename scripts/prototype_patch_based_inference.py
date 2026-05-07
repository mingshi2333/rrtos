#!/usr/bin/env python3
"""Estimate patch-based inference memory impact for static-direct schedules."""

from __future__ import annotations

import argparse
from dataclasses import replace
import json
from pathlib import Path
from typing import Any

try:
    from scripts import sweep_static_direct_arena_planners as planner_sweep
except ModuleNotFoundError:  # pragma: no cover - direct script execution fallback.
    import sweep_static_direct_arena_planners as planner_sweep


def _kib(value: int) -> float:
    return round(value / 1024.0, 3)


def _model_name(path: Path) -> str:
    name = path.stem
    suffix = "_fresh_hal_schedule"
    return name[: -len(suffix)] if name.endswith(suffix) else name


def _command_liveness_peak(allocations: list[planner_sweep.TransientAllocation]) -> int:
    if not allocations:
        return 0
    command_count = max(allocation.end for allocation in allocations)
    peak = 0
    for command_index in range(command_count):
        live_bytes = sum(
            allocation.size
            for allocation in allocations
            if allocation.start <= command_index < allocation.end
        )
        peak = max(peak, live_bytes)
    return peak


def _pack_best_size_desc(allocations: list[planner_sweep.TransientAllocation]) -> int:
    ordered = sorted(
        allocations,
        key=lambda allocation: (
            -allocation.size,
            allocation.start,
            allocation.end,
            allocation.alloc_id,
        ),
    )
    packed_bytes, _ = planner_sweep._pack_best_fit(ordered)
    return packed_bytes


def _effective_arena_bytes(allocations: list[planner_sweep.TransientAllocation]) -> int:
    if not allocations:
        return 0
    packed = _pack_best_size_desc(allocations)
    peak = _command_liveness_peak(allocations)
    max_single = max(allocation.size for allocation in allocations)
    return max(packed, peak, max_single)


def _find_allocation(
    allocations: list[planner_sweep.TransientAllocation],
    allocation_id: str,
) -> planner_sweep.TransientAllocation:
    for allocation in allocations:
        if allocation.alloc_id == allocation_id:
            return allocation
    raise ValueError(f"allocation {allocation_id!r} not found")


def _non_negative(value: Any, field: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value < 0:
        raise ValueError(f"{field} must be a non-negative integer")
    return value


def _allocations_from_verify_summary(
    summary: dict[str, Any],
) -> list[planner_sweep.TransientAllocation]:
    placements = summary.get("placements")
    if not isinstance(placements, dict):
        raise ValueError("static-direct verify summary must contain placements")

    allocations: list[planner_sweep.TransientAllocation] = []
    for alloc_id, placement in placements.items():
        if not isinstance(placement, dict):
            raise ValueError(f"placement {alloc_id!r} must be an object")
        interval = placement.get("interval")
        if not (
            isinstance(interval, list)
            and len(interval) == 2
            and all(isinstance(item, int) and item >= 0 for item in interval)
            and interval[0] < interval[1]
        ):
            raise ValueError(f"placement {alloc_id!r} interval must be [start, end]")
        allocations.append(
            planner_sweep.TransientAllocation(
                str(alloc_id),
                _non_negative(placement.get("size"), f"placement {alloc_id!r} size"),
                interval[0],
                interval[1],
            )
        )
    return allocations


def _source_allocations(schedule_or_summary: dict[str, Any]) -> tuple[list[planner_sweep.TransientAllocation], str]:
    if "allocations" in schedule_or_summary and "dispatches" in schedule_or_summary:
        allocations, _ = planner_sweep._transient_allocations(schedule_or_summary)
        return allocations, "hal_schedule"
    if "placements" in schedule_or_summary:
        return _allocations_from_verify_summary(schedule_or_summary), "static_direct_verify_summary"
    raise ValueError("input must be a HAL schedule or static-direct verify summary")


def _with_allocation_size(
    allocations: list[planner_sweep.TransientAllocation],
    allocation_id: str,
    new_size: int,
) -> list[planner_sweep.TransientAllocation]:
    return [
        replace(allocation, size=new_size)
        if allocation.alloc_id == allocation_id
        else allocation
        for allocation in allocations
    ]


def _positive(value: int, field: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value <= 0:
        raise ValueError(f"{field} must be a positive integer")
    return value


def evaluate_patch_candidates(
    schedule: dict[str, Any],
    *,
    name: str,
    allocation_id: str,
    component_bytes: int,
    output_rows: int,
    output_cols: int,
    output_channels: int,
    element_bytes: int,
    tile_rows: list[int],
    extra_scratch_bytes: int = 0,
) -> dict[str, Any]:
    component_bytes = _positive(component_bytes, "component bytes")
    output_rows = _positive(output_rows, "output rows")
    output_cols = _positive(output_cols, "output cols")
    output_channels = _positive(output_channels, "output channels")
    element_bytes = _positive(element_bytes, "element bytes")
    if isinstance(extra_scratch_bytes, bool) or extra_scratch_bytes < 0:
        raise ValueError("extra scratch bytes must be a non-negative integer")
    if not tile_rows:
        raise ValueError("at least one tile row candidate is required")

    allocations, source_kind = _source_allocations(schedule)
    target = _find_allocation(allocations, allocation_id)
    if component_bytes > target.size:
        raise ValueError(
            f"component bytes {component_bytes} exceed allocation "
            f"{allocation_id!r} size {target.size}"
        )

    component_shape_bytes = output_rows * output_cols * output_channels * element_bytes
    base_effective = _effective_arena_bytes(allocations)
    base_live_peak = _command_liveness_peak(allocations)
    base_packed = _pack_best_size_desc(allocations)
    candidates: list[dict[str, Any]] = []

    for rows in sorted(set(tile_rows)):
        rows = _positive(rows, "tile rows")
        if rows > output_rows:
            raise ValueError("tile rows must not exceed output rows")

        patch_scratch = rows * output_cols * output_channels * element_bytes
        patched_size = target.size - component_bytes + patch_scratch + extra_scratch_bytes
        patched_allocations = _with_allocation_size(
            allocations,
            allocation_id,
            patched_size,
        )
        projected_effective = _effective_arena_bytes(patched_allocations)
        projected_live_peak = _command_liveness_peak(patched_allocations)
        projected_packed = _pack_best_size_desc(patched_allocations)
        savings = max(0, base_effective - projected_effective)
        tile_count = (output_rows + rows - 1) // rows
        candidates.append(
            {
                "tile_rows": rows,
                "tile_count": tile_count,
                "patch_scratch_bytes": patch_scratch,
                "patch_scratch_kib": _kib(patch_scratch),
                "extra_scratch_bytes": extra_scratch_bytes,
                "extra_scratch_kib": _kib(extra_scratch_bytes),
                "patched_allocation_bytes": patched_size,
                "patched_allocation_kib": _kib(patched_size),
                "projected_effective_bytes": projected_effective,
                "projected_effective_kib": _kib(projected_effective),
                "projected_live_peak_bytes": projected_live_peak,
                "projected_live_peak_kib": _kib(projected_live_peak),
                "projected_packed_bytes": projected_packed,
                "projected_packed_kib": _kib(projected_packed),
                "savings_bytes": savings,
                "savings_kib": _kib(savings),
                "savings_percent": round(
                    (savings * 100.0 / base_effective) if base_effective else 0.0,
                    2,
                ),
            }
        )

    return {
        "name": name,
        "source_kind": source_kind,
        "allocation": allocation_id,
        "allocation_bytes": target.size,
        "allocation_kib": _kib(target.size),
        "allocation_interval": [target.start, target.end],
        "component_bytes": component_bytes,
        "component_kib": _kib(component_bytes),
        "component_shape_bytes": component_shape_bytes,
        "component_shape_kib": _kib(component_shape_bytes),
        "component_shape_matches": component_shape_bytes == component_bytes,
        "output_shape": {
            "rows": output_rows,
            "cols": output_cols,
            "channels": output_channels,
            "element_bytes": element_bytes,
        },
        "base_effective_bytes": base_effective,
        "base_effective_kib": _kib(base_effective),
        "base_live_peak_bytes": base_live_peak,
        "base_live_peak_kib": _kib(base_live_peak),
        "base_packed_bytes": base_packed,
        "base_packed_kib": _kib(base_packed),
        "candidates": candidates,
        "verdict": "memory_model_only",
    }


def render_markdown(results: list[dict[str, Any]]) -> str:
    lines = [
        "# Patch-Based Inference Prototype",
        "",
        "This is not a runtime validation. It is a static memory-model estimate that asks what happens if one full-shape accumulator component is replaced by a tile-sized scratch component inside the existing static-direct schedule.",
        "",
        "The intended runtime policy is still one fixed arena: model create binds the arena, invoke reuses tile scratch, and destroy is a metadata cleanup/no-op for static arena deployments.",
        "",
        "| Model | Allocation | Base KiB | Component KiB | Tile Rows | Tile Count | Patch Scratch KiB | Projected KiB | Save KiB | Save % |",
        "| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |",
    ]
    for result in results:
        for candidate in result["candidates"]:
            lines.append(
                f"| `{result['name']}` | `{result['allocation']}` | "
                f"{result['base_effective_kib']:.3f} | "
                f"{result['component_kib']:.3f} | "
                f"{candidate['tile_rows']} | {candidate['tile_count']} | "
                f"{candidate['patch_scratch_kib']:.3f} | "
                f"{candidate['projected_effective_kib']:.3f} | "
                f"{candidate['savings_kib']:.3f} | "
                f"{candidate['savings_percent']:.2f} |"
            )

    lines.extend(
        [
            "",
            "Notes:",
            "",
            "- `Base KiB` is command-liveness plus best-fit packing, not a firmware run.",
            "- `Patch Scratch KiB` is only the tile-sized replacement for the selected accumulator component.",
            "- A real kernel still must prove bit-exact or tolerance-equivalent output against the current QEMU oracle.",
            "- Per-patch create/destroy is intentionally excluded; patch loops should reuse the already-bound arena scratch.",
            "",
        ]
    )
    return "\n".join(lines)


def _load_schedule(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("schedule JSON must contain an object")
    return data


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Estimate patch-based inference memory impact for static-direct schedules."
    )
    parser.add_argument("--schedule", required=True, type=Path)
    parser.add_argument("--allocation", required=True)
    parser.add_argument("--component-bytes", required=True, type=int)
    parser.add_argument("--output-rows", required=True, type=int)
    parser.add_argument("--output-cols", required=True, type=int)
    parser.add_argument("--output-channels", required=True, type=int)
    parser.add_argument("--element-bytes", required=True, type=int)
    parser.add_argument("--tile-rows", action="append", required=True, type=int)
    parser.add_argument("--extra-scratch-bytes", default=0, type=int)
    parser.add_argument("--name")
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--json-output", type=Path)
    args = parser.parse_args(argv)

    try:
        result = evaluate_patch_candidates(
            _load_schedule(args.schedule),
            name=args.name or _model_name(args.schedule),
            allocation_id=args.allocation,
            component_bytes=args.component_bytes,
            output_rows=args.output_rows,
            output_cols=args.output_cols,
            output_channels=args.output_channels,
            element_bytes=args.element_bytes,
            tile_rows=args.tile_rows,
            extra_scratch_bytes=args.extra_scratch_bytes,
        )
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        parser.exit(1, f"error: {exc}\n")

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(render_markdown([result]), encoding="utf-8")
    if args.json_output is not None:
        args.json_output.parent.mkdir(parents=True, exist_ok=True)
        args.json_output.write_text(
            json.dumps(result, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
    print(f"Wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
