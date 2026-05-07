#!/usr/bin/env python3
"""Prototype compiler-side spill/fusion targets for static-direct schedules."""

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


def _with_reduced_allocation(
    allocations: list[planner_sweep.TransientAllocation],
    alloc_id: str,
    new_size: int,
) -> list[planner_sweep.TransientAllocation]:
    reduced: list[planner_sweep.TransientAllocation] = []
    for allocation in allocations:
        if allocation.alloc_id != alloc_id:
            reduced.append(allocation)
            continue
        if new_size > 0:
            reduced.append(replace(allocation, size=new_size))
    return reduced


def evaluate_spill_candidates(schedule: dict[str, Any], name: str, top: int = 5) -> dict[str, Any]:
    allocations, _ = planner_sweep._transient_allocations(schedule)
    base = planner_sweep.evaluate_schedule(schedule, name)
    current_effective_bytes = base["planners"]["input_order_first_fit"]["effective_bytes"]
    model_base_effective_bytes = _effective_arena_bytes(allocations)
    verified_to_model_gap_bytes = max(0, current_effective_bytes - model_base_effective_bytes)

    candidates: list[dict[str, Any]] = []
    for allocation in allocations:
        transforms = [
            ("tile2", max(1, (allocation.size + 1) // 2)),
            ("tile4", max(1, (allocation.size + 3) // 4)),
            ("elide_or_external_spill", 0),
        ]
        for transform, new_size in transforms:
            reduced = _with_reduced_allocation(allocations, allocation.alloc_id, new_size)
            projected = _effective_arena_bytes(reduced)
            savings = max(0, model_base_effective_bytes - projected)
            candidates.append(
                {
                    "allocation": allocation.alloc_id,
                    "transform": transform,
                    "original_bytes": allocation.size,
                    "original_kib": _kib(allocation.size),
                    "new_bytes": new_size,
                    "new_kib": _kib(new_size),
                    "interval": [allocation.start, allocation.end],
                    "projected_effective_bytes": projected,
                    "projected_effective_kib": _kib(projected),
                    "savings_bytes": savings,
                    "savings_kib": _kib(savings),
                    "savings_basis": "command_liveness_model",
                    "external_spill_bytes": allocation.size if transform == "elide_or_external_spill" else 0,
                    "external_spill_kib": _kib(
                        allocation.size if transform == "elide_or_external_spill" else 0
                    ),
                }
            )

    candidates.sort(
        key=lambda candidate: (
            -candidate["savings_bytes"],
            candidate["projected_effective_bytes"],
            -candidate["original_bytes"],
            candidate["allocation"],
            candidate["transform"],
        )
    )
    best_tiling = next(
        (
            candidate
            for candidate in candidates
            if candidate["transform"] in {"tile2", "tile4"}
        ),
        None,
    )

    return {
        "name": name,
        "allocation_count": len(allocations),
        "current_effective_bytes": current_effective_bytes,
        "current_effective_kib": _kib(current_effective_bytes),
        "model_base_effective_bytes": model_base_effective_bytes,
        "model_base_effective_kib": _kib(model_base_effective_bytes),
        "command_liveness_effective_bytes": model_base_effective_bytes,
        "command_liveness_effective_kib": _kib(model_base_effective_bytes),
        "verified_to_model_gap_bytes": verified_to_model_gap_bytes,
        "verified_to_model_gap_kib": _kib(verified_to_model_gap_bytes),
        "best_tiling_candidate": best_tiling,
        "top_candidates": candidates[:top],
    }


def render_markdown(results: list[dict[str, Any]]) -> str:
    lines = [
        "# Static-Direct Spill/Fusion Prototype",
        "",
        "This is an upper-bound compiler experiment. The verified base is the current descriptor accounting. The model base is command-liveness plus best-fit packing and is used only for candidate ranking. Candidate savings are measured against the model base, not against proven firmware RAM.",
        "",
        "`elide_or_external_spill` means the allocation is removed from the static arena model; it only reduces total RAM if a later compiler/runtime implementation fuses it away, tiles it, or spills it outside RAM.",
        "",
        "| Model | Verified KiB | Model KiB | Model Gap KiB | Best Transform | Allocation | Alloc KiB | Projected KiB | Model Save KiB | External KiB | Best Tile | Tile Save KiB | Interval |",
        "| --- | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | --- | ---: | --- |",
    ]
    for result in results:
        if not result["top_candidates"]:
            continue
        best = result["top_candidates"][0]
        best_tile = result.get("best_tiling_candidate")
        tile_label = "n/a"
        tile_save = 0.0
        if best_tile is not None:
            tile_label = f"`{best_tile['transform']}:{best_tile['allocation']}`"
            tile_save = best_tile["savings_kib"]
        interval = best["interval"]
        lines.append(
            f"| `{result['name']}` | {result['current_effective_kib']:.3f} | "
            f"{result['model_base_effective_kib']:.3f} | "
            f"{result['verified_to_model_gap_kib']:.3f} | "
            f"`{best['transform']}` | `{best['allocation']}` | "
            f"{best['original_kib']:.3f} | {best['projected_effective_kib']:.3f} | "
            f"{best['savings_kib']:.3f} | {best['external_spill_kib']:.3f} | "
            f"{tile_label} | {tile_save:.3f} | "
            f"[{interval[0]}, {interval[1]}) |"
        )

    lines.extend(
        [
            "",
            "Interpretation:",
            "",
            "- `tile2` and `tile4` approximate compiler tiling or partial accumulation.",
            "- `elide_or_external_spill` is the aggressive bound for fusion/streaming/spill.",
            "- A large `External KiB` means arena pressure moves elsewhere unless the data is truly fused or streamed.",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--schedule", action="append", required=True, help="HAL schedule JSON")
    parser.add_argument("--output", required=True, help="Markdown report path")
    parser.add_argument("--json-output", help="Optional JSON report path")
    parser.add_argument("--top", type=int, default=5, help="Candidates per model")
    args = parser.parse_args()

    results = []
    for schedule_arg in args.schedule:
        path = Path(schedule_arg)
        schedule = json.loads(path.read_text(encoding="utf-8"))
        results.append(evaluate_spill_candidates(schedule, _model_name(path), args.top))

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
