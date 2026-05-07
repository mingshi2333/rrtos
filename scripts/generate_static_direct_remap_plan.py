#!/usr/bin/env python3
"""Generate a static-direct remap plan from verified arena placements."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any


SCHEMA = "rrtos.static_direct_remap_plan.v1"
ALIGNMENT = 64


def _kib(value: int) -> float:
    return round(value / 1024.0, 3)


def _non_negative(value: Any, field: str) -> int:
    if isinstance(value, bool) or not isinstance(value, int) or value < 0:
        raise ValueError(f"{field} must be a non-negative integer")
    return value


def _positive(value: Any, field: str) -> int:
    value = _non_negative(value, field)
    if value == 0:
        raise ValueError(f"{field} must be a positive integer")
    return value


def _command_index(value: Any, field: str) -> int:
    return _non_negative(value, field)


def _align_up(value: int, alignment: int = ALIGNMENT) -> int:
    if alignment <= 1:
        return value
    return ((value + alignment - 1) // alignment) * alignment


def _placement(summary: dict[str, Any], allocation: str) -> dict[str, Any]:
    placements = summary.get("placements")
    if not isinstance(placements, dict):
        raise ValueError("verify summary must contain placements")
    placement = placements.get(allocation)
    if not isinstance(placement, dict):
        raise ValueError(f"allocation {allocation!r} not found in placements")
    return placement


def _placement_interval(placement: dict[str, Any], allocation: str) -> list[int]:
    interval = placement.get("interval")
    if not (
        isinstance(interval, list)
        and len(interval) == 2
        and all(isinstance(item, int) and item >= 0 for item in interval)
        and interval[0] < interval[1]
    ):
        raise ValueError(f"allocation {allocation!r} placement interval must be [start, end]")
    return [interval[0], interval[1]]


def _placement_offset(placement: dict[str, Any], allocation: str) -> int:
    return _non_negative(placement.get("offset"), f"allocation {allocation!r} offset")


def _placement_size(placement: dict[str, Any], allocation: str) -> int:
    return _positive(placement.get("size"), f"allocation {allocation!r} size")


def _validate_command_range(start: int, end: int, field: str) -> None:
    if start >= end:
        raise ValueError(f"{field} must end after it starts")


def build_remap_plan(
    verify_summary: dict[str, Any],
    *,
    name: str,
    patch_allocation: str,
    patch_internal_offset: int,
    produced_bytes: int,
    continuation_command_index: int,
    remap_end_command: int,
    scratch_allocation: str | None = None,
    scratch_end_command: int | None = None,
    alignment: int = ALIGNMENT,
) -> dict[str, Any]:
    if not name:
        raise ValueError("name must be present")
    if not patch_allocation:
        raise ValueError("patch allocation must be present")

    patch_internal_offset = _non_negative(
        patch_internal_offset,
        "patch internal offset",
    )
    produced_bytes = _positive(produced_bytes, "produced bytes")
    continuation_command_index = _command_index(
        continuation_command_index,
        "continuation command index",
    )
    remap_end_command = _command_index(remap_end_command, "remap end command")
    _validate_command_range(
        continuation_command_index,
        remap_end_command,
        "remap command range",
    )

    patch_placement = _placement(verify_summary, patch_allocation)
    patch_interval = _placement_interval(patch_placement, patch_allocation)
    patch_offset = _placement_offset(patch_placement, patch_allocation)
    patch_size = _placement_size(patch_placement, patch_allocation)

    if patch_internal_offset >= patch_size:
        raise ValueError(
            f"patch internal offset {patch_internal_offset} must be inside "
            f"allocation {patch_allocation!r} size {patch_size}"
        )

    rebased_bytes = patch_size - patch_internal_offset
    if produced_bytes > rebased_bytes:
        raise ValueError(
            f"produced bytes {produced_bytes} exceed rebased region {rebased_bytes}"
        )
    if continuation_command_index > patch_interval[1] or remap_end_command > patch_interval[1]:
        raise ValueError(
            f"remap command range [{continuation_command_index}, {remap_end_command}) "
            f"exceeds allocation {patch_allocation!r} interval {patch_interval}"
        )

    scratch_remaps: list[dict[str, Any]] = []
    arena_bytes = rebased_bytes
    if scratch_allocation is not None:
        if scratch_end_command is None:
            raise ValueError("scratch end command is required when scratch allocation is set")
        if not scratch_allocation:
            raise ValueError("scratch allocation must be present")
        scratch_end_command = _command_index(
            scratch_end_command,
            "scratch end command",
        )
        _validate_command_range(
            continuation_command_index,
            scratch_end_command,
            "scratch command range",
        )
        scratch_placement = _placement(verify_summary, scratch_allocation)
        scratch_interval = _placement_interval(scratch_placement, scratch_allocation)
        scratch_offset = _placement_offset(scratch_placement, scratch_allocation)
        scratch_size = _placement_size(scratch_placement, scratch_allocation)
        if scratch_end_command > scratch_interval[1]:
            raise ValueError(
                f"scratch command range [{continuation_command_index}, "
                f"{scratch_end_command}) exceeds allocation {scratch_allocation!r} "
                f"interval {scratch_interval}"
            )
        scratch_new_offset = _align_up(rebased_bytes, alignment)
        scratch_remaps.append(
            {
                "allocation": scratch_allocation,
                "command_range": [continuation_command_index, scratch_end_command],
                "old_offset": scratch_offset,
                "old_length": scratch_size,
                "new_offset": scratch_new_offset,
                "new_length": scratch_size,
            }
        )
        arena_bytes = max(arena_bytes, scratch_new_offset + scratch_size)

    old_actual_base = patch_offset + patch_internal_offset
    original_arena_bytes = _positive(
        verify_summary.get("arena_bytes"),
        "verify summary arena bytes",
    )

    slice_remaps = [
        {
            "kind": "binding_window_rebase",
            "allocation": patch_allocation,
            "command_range": [continuation_command_index, remap_end_command],
            "old_offset": patch_offset,
            "old_length": patch_size,
            "new_offset": 0,
            "new_length": rebased_bytes,
            "requires_exact_binding": True,
        },
        {
            "kind": "absolute_arena_range_shift",
            "allocation": patch_allocation,
            "command_range": [continuation_command_index, remap_end_command],
            "old_offset_begin": old_actual_base,
            "old_offset_end": patch_offset + patch_size,
            "addend": -old_actual_base,
        },
    ]

    savings = max(0, original_arena_bytes - arena_bytes)
    return {
        "schema": SCHEMA,
        "name": name,
        "source": verify_summary.get("source", ""),
        "continuation_command_index": continuation_command_index,
        "replaced_command_range": [0, continuation_command_index],
        "original_arena_bytes": original_arena_bytes,
        "original_arena_kib": _kib(original_arena_bytes),
        "arena_bytes": arena_bytes,
        "arena_kib": _kib(arena_bytes),
        "savings_bytes": savings,
        "savings_kib": _kib(savings),
        "rebased_region": {
            "allocation": patch_allocation,
            "old_binding_offset": patch_offset,
            "old_binding_bytes": patch_size,
            "old_internal_base": patch_internal_offset,
            "old_actual_base": old_actual_base,
            "new_offset": 0,
            "bytes": rebased_bytes,
            "produced_bytes": produced_bytes,
            "command_range": [continuation_command_index, remap_end_command],
        },
        "slice_remaps": slice_remaps,
        "constant_remaps": [
            {
                "allocation": patch_allocation,
                "command_range": [continuation_command_index, remap_end_command],
                "old_value_begin": patch_internal_offset,
                "old_value_end": patch_size,
                "addend": -patch_internal_offset,
            }
        ],
        "scratch_remaps": scratch_remaps,
        "fail_closed_rules": [
            "Only remap constants in the declared allocation value range.",
            "Only shrink bindings that exactly match the declared original window.",
            "Reject slices that fall between the original binding base and rebased region.",
            "Reject retained commands after a remap end unless they use an explicit scratch rule.",
        ],
    }


def render_markdown(plan: dict[str, Any]) -> str:
    rebased = plan["rebased_region"]
    lines = [
        "# Static-Direct Remap Plan",
        "",
        f"Plan `{plan['name']}` rebases one verified static-direct arena region "
        "after a generated patch replaces the earlier command range.",
        "",
        "| Metric | Bytes | KiB |",
        "| --- | ---: | ---: |",
        f"| Original arena | {plan['original_arena_bytes']} | "
        f"{plan['original_arena_kib']:.3f} |",
        f"| Remapped arena | {plan['arena_bytes']} | {plan['arena_kib']:.3f} |",
        f"| Saved | {plan['savings_bytes']} | {plan['savings_kib']:.3f} |",
        "",
        "| Region | Allocation | Old | New | Bytes | Commands |",
        "| --- | --- | ---: | ---: | ---: | --- |",
        f"| rebased | `{rebased['allocation']}` | {rebased['old_actual_base']} | "
        f"{rebased['new_offset']} | {rebased['bytes']} | "
        f"[{rebased['command_range'][0]}, {rebased['command_range'][1]}) |",
    ]
    for scratch in plan["scratch_remaps"]:
        lines.append(
            f"| scratch | `{scratch['allocation']}` | {scratch['old_offset']} | "
            f"{scratch['new_offset']} | {scratch['new_length']} | "
            f"[{scratch['command_range'][0]}, {scratch['command_range'][1]}) |"
        )

    lines.extend(
        [
            "",
            "Fail-closed rules:",
            "",
        ]
    )
    for rule in plan["fail_closed_rules"]:
        lines.append(f"- {rule}")
    lines.append("")
    return "\n".join(lines)


def _single_scratch(plan: dict[str, Any]) -> dict[str, Any]:
    scratch_remaps = plan.get("scratch_remaps")
    if not isinstance(scratch_remaps, list) or len(scratch_remaps) != 1:
        raise ValueError("C renderer currently requires exactly one scratch remap")
    scratch = scratch_remaps[0]
    if not isinstance(scratch, dict):
        raise ValueError("scratch remap must be an object")
    return scratch


def render_c_header(
    plan: dict[str, Any],
    *,
    header_guard: str,
    macro_prefix: str,
    function_name: str,
) -> str:
    rebased = plan["rebased_region"]
    return "\n".join(
        [
            "/* Generated by scripts/generate_static_direct_remap_plan.py. */",
            f"#ifndef {header_guard}",
            f"#define {header_guard}",
            "",
            "#include <stdint.h>",
            "",
            '#include "ai_static_direct.h"',
            "",
            "#ifdef __cplusplus",
            'extern "C" {',
            "#endif",
            "",
            f"#define {macro_prefix}_ARENA_BYTES {plan['arena_bytes']}u",
            f"#define {macro_prefix}_FIRST_OUTPUT_OFFSET {rebased['new_offset']}u",
            "",
            f"int {function_name}(",
            "    const ai_static_direct_descriptor_t *descriptor,",
            "    uint32_t command_start,",
            "    const iree_hal_executable_library_v0_t *library,",
            "    const iree_hal_executable_environment_v0_t *environment,",
            "    ai_static_direct_context_t *context);",
            "",
            "#ifdef __cplusplus",
            "}",
            "#endif",
            "",
            f"#endif /* {header_guard} */",
            "",
        ]
    )


def render_c_source(
    plan: dict[str, Any],
    *,
    header_name: str,
    macro_prefix: str,
    function_name: str,
) -> str:
    rebased = plan["rebased_region"]
    scratch = _single_scratch(plan)
    return "\n".join(
        [
            "/* Generated by scripts/generate_static_direct_remap_plan.py. */",
            f"/* Plan: {plan['name']} */",
            f'#include "{header_name}"',
            "",
            "#include <stddef.h>",
            "#include <stdint.h>",
            "",
            "enum {",
            f"    {macro_prefix}_MAX_BINDINGS = 8,",
            f"    {macro_prefix}_MAX_CONSTANTS = 16,",
            f"    {macro_prefix}_OLD_BIG_BINDING_OFFSET = "
            f"{rebased['old_binding_offset']},",
            f"    {macro_prefix}_OLD_BIG_BINDING_BYTES = "
            f"{rebased['old_binding_bytes']},",
            f"    {macro_prefix}_OLD_BIG_INTERNAL_BASE = "
            f"{rebased['old_internal_base']},",
            f"    {macro_prefix}_OLD_BIG_ACTUAL_BASE =",
            f"        {macro_prefix}_OLD_BIG_BINDING_OFFSET +",
            f"        {macro_prefix}_OLD_BIG_INTERNAL_BASE,",
            f"    {macro_prefix}_OLD_ARENA_BYTES = {plan['original_arena_bytes']},",
            f"    {macro_prefix}_BIG_SUFFIX_BYTES =",
            f"        {macro_prefix}_OLD_ARENA_BYTES -",
            f"        {macro_prefix}_OLD_BIG_ACTUAL_BASE,",
            f"    {macro_prefix}_WORK_BUFFER_OLD_OFFSET = {scratch['old_offset']},",
            f"    {macro_prefix}_WORK_BUFFER_BYTES = {scratch['old_length']},",
            f"    {macro_prefix}_WORK_BUFFER_NEW_OFFSET = {scratch['new_offset']},",
            f"    {macro_prefix}_BIG_SUFFIX_END_COMMAND = "
            f"{rebased['command_range'][1]},",
            f"    {macro_prefix}_WORK_BUFFER_END_COMMAND = "
            f"{scratch['command_range'][1]},",
            "};",
            "",
            "static int remap_arena_slice(ai_static_direct_slice_t *slice,",
            "                             uint32_t command_index,",
            "                             int *out_big_suffix_binding) {",
            "    if (slice == NULL || out_big_suffix_binding == NULL) {",
            "        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;",
            "    }",
            "    if (slice->base != AI_STATIC_DIRECT_BUFFER_ARENA) {",
            "        return AI_STATIC_DIRECT_OK;",
            "    }",
            "    const uint32_t original_offset = slice->offset;",
            "",
            f"    if (slice->offset == {macro_prefix}_OLD_BIG_BINDING_OFFSET &&",
            f"        slice->length == {macro_prefix}_OLD_BIG_BINDING_BYTES) {{",
            f"        if (command_index >= {macro_prefix}_BIG_SUFFIX_END_COMMAND) {{",
            "            return AI_STATIC_DIRECT_ERR_INVALID_SLICE;",
            "        }",
            "        slice->offset = 0u;",
            f"        slice->length = {macro_prefix}_BIG_SUFFIX_BYTES;",
            "        *out_big_suffix_binding = 1;",
            "        return AI_STATIC_DIRECT_OK;",
            "    }",
            "",
            f"    if (command_index < {macro_prefix}_WORK_BUFFER_END_COMMAND &&",
            f"        slice->offset >= {macro_prefix}_WORK_BUFFER_OLD_OFFSET &&",
            f"        slice->offset < {macro_prefix}_WORK_BUFFER_BYTES) {{",
            "        if (slice->length >",
            f"            {macro_prefix}_WORK_BUFFER_BYTES - slice->offset) {{",
            "            return AI_STATIC_DIRECT_ERR_INVALID_SLICE;",
            "        }",
            f"        slice->offset += {macro_prefix}_WORK_BUFFER_NEW_OFFSET;",
            "        return AI_STATIC_DIRECT_OK;",
            "    }",
            "",
            f"    if (command_index < {macro_prefix}_BIG_SUFFIX_END_COMMAND &&",
            f"        slice->offset >= {macro_prefix}_OLD_BIG_ACTUAL_BASE &&",
            f"        slice->offset < {macro_prefix}_OLD_ARENA_BYTES) {{",
            "        if (slice->length >",
            f"            {macro_prefix}_OLD_ARENA_BYTES - slice->offset) {{",
            "            return AI_STATIC_DIRECT_ERR_INVALID_SLICE;",
            "        }",
            f"        slice->offset -= {macro_prefix}_OLD_BIG_ACTUAL_BASE;",
            "    }",
            "",
            f"    if (command_index < {macro_prefix}_BIG_SUFFIX_END_COMMAND &&",
            f"        original_offset >= {macro_prefix}_OLD_BIG_BINDING_OFFSET &&",
            f"        original_offset < {macro_prefix}_OLD_BIG_ACTUAL_BASE) {{",
            "        return AI_STATIC_DIRECT_ERR_INVALID_SLICE;",
            "    }",
            "",
            "    return AI_STATIC_DIRECT_OK;",
            "}",
            "",
            "static void remap_big_suffix_constants(uint32_t *constants,",
            "                                       uint32_t constant_count) {",
            "    for (uint32_t i = 0; i < constant_count; ++i) {",
            f"        if (constants[i] >= {macro_prefix}_OLD_BIG_INTERNAL_BASE &&",
            f"            constants[i] < {macro_prefix}_OLD_BIG_BINDING_BYTES) {{",
            f"            constants[i] -= {macro_prefix}_OLD_BIG_INTERNAL_BASE;",
            "        }",
            "    }",
            "}",
            "",
            "static int invoke_one_remapped_command(",
            "    const ai_static_direct_descriptor_t *descriptor,",
            "    uint32_t command_index,",
            "    const ai_static_direct_command_t *command,",
            "    const iree_hal_executable_library_v0_t *library,",
            "    const iree_hal_executable_environment_v0_t *environment,",
            "    ai_static_direct_context_t *context) {",
            "    ai_static_direct_command_t remapped_command = *command;",
            "    ai_static_direct_dispatch_t remapped_dispatch;",
            "    ai_static_direct_slice_t remapped_bindings",
            f"        [{macro_prefix}_MAX_BINDINGS];",
            f"    uint32_t remapped_constants[{macro_prefix}_MAX_CONSTANTS];",
            "    int big_suffix_binding = 0;",
            "",
            "    if (command->type == AI_STATIC_DIRECT_COMMAND_DISPATCH) {",
            "        const ai_static_direct_dispatch_t *dispatch =",
            "            command->config.dispatch.dispatch;",
            "        if (dispatch == NULL) {",
            "            return AI_STATIC_DIRECT_ERR_INVALID_DESCRIPTOR;",
            "        }",
            f"        if (dispatch->binding_count > {macro_prefix}_MAX_BINDINGS ||",
            f"            dispatch->constant_count > {macro_prefix}_MAX_CONSTANTS) {{",
            "            return AI_STATIC_DIRECT_ERR_UNSUPPORTED_DISPATCH;",
            "        }",
            "",
            "        remapped_dispatch = *dispatch;",
            "        for (uint32_t i = 0; i < dispatch->binding_count; ++i) {",
            "            remapped_bindings[i] = dispatch->bindings[i];",
            "            int rc = remap_arena_slice(&remapped_bindings[i],",
            "                                       command_index,",
            "                                       &big_suffix_binding);",
            "            if (rc != AI_STATIC_DIRECT_OK) {",
            "                return rc;",
            "            }",
            "        }",
            "        for (uint32_t i = 0; i < dispatch->constant_count; ++i) {",
            "            remapped_constants[i] = dispatch->constants[i];",
            "        }",
            "        if (big_suffix_binding) {",
            "            remap_big_suffix_constants(remapped_constants,",
            "                                       dispatch->constant_count);",
            "        }",
            "",
            "        remapped_dispatch.bindings = remapped_bindings;",
            "        remapped_dispatch.constants = remapped_constants;",
            "        remapped_command.config.dispatch.dispatch = &remapped_dispatch;",
            "    } else if (command->type == AI_STATIC_DIRECT_COMMAND_FILL) {",
            "        int rc = remap_arena_slice(&remapped_command.config.fill.target,",
            "                                   command_index,",
            "                                   &big_suffix_binding);",
            "        if (rc != AI_STATIC_DIRECT_OK) {",
            "            return rc;",
            "        }",
            "    }",
            "",
            "    ai_static_direct_descriptor_t one_command_descriptor = *descriptor;",
            f"    one_command_descriptor.arena_bytes = {macro_prefix}_ARENA_BYTES;",
            "    one_command_descriptor.commands = &remapped_command;",
            "    one_command_descriptor.command_count = 1u;",
            "    int rc = ai_static_direct_invoke(&one_command_descriptor,",
            "                                     library,",
            "                                     environment,",
            "                                     context);",
            "    return rc;",
            "}",
            "",
            f"int {function_name}(",
            "    const ai_static_direct_descriptor_t *descriptor,",
            "    uint32_t command_start,",
            "    const iree_hal_executable_library_v0_t *library,",
            "    const iree_hal_executable_environment_v0_t *environment,",
            "    ai_static_direct_context_t *context) {",
            "    if (descriptor == NULL || descriptor->commands == NULL ||",
            "        command_start > descriptor->command_count) {",
            "        return AI_STATIC_DIRECT_ERR_INVALID_ARGUMENT;",
            "    }",
            "",
            "    for (uint32_t i = command_start; i < descriptor->command_count; ++i) {",
            "        int rc = invoke_one_remapped_command(descriptor,",
            "                                             i,",
            "                                             &descriptor->commands[i],",
            "                                             library,",
            "                                             environment,",
            "                                             context);",
            "        if (rc != AI_STATIC_DIRECT_OK) {",
            "            return rc;",
            "        }",
            "    }",
            "    return AI_STATIC_DIRECT_OK;",
            "}",
            "",
        ]
    )


def _load_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError("input JSON must contain an object")
    return data


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Generate static-direct remap TransformIR from verify-summary placements."
    )
    parser.add_argument("--verify-summary", required=True, type=Path)
    parser.add_argument("--name", required=True)
    parser.add_argument("--patch-allocation", required=True)
    parser.add_argument("--patch-internal-offset", required=True, type=int)
    parser.add_argument("--produced-bytes", required=True, type=int)
    parser.add_argument("--continuation-command-index", required=True, type=int)
    parser.add_argument("--remap-end-command", required=True, type=int)
    parser.add_argument("--scratch-allocation")
    parser.add_argument("--scratch-end-command", type=int)
    parser.add_argument("--output-json", required=True, type=Path)
    parser.add_argument("--output-md", type=Path)
    parser.add_argument("--output-c", type=Path)
    parser.add_argument("--output-h", type=Path)
    parser.add_argument("--macro-prefix")
    parser.add_argument("--header-guard")
    parser.add_argument("--function-name")
    args = parser.parse_args(argv)

    if (args.output_c is None) != (args.output_h is None):
        parser.exit(1, "error: --output-c and --output-h must be used together\n")
    if args.output_c is not None and (
        args.macro_prefix is None
        or args.header_guard is None
        or args.function_name is None
    ):
        parser.exit(
            1,
            "error: --macro-prefix, --header-guard, and --function-name are "
            "required with --output-c/--output-h\n",
        )

    try:
        plan = build_remap_plan(
            _load_json(args.verify_summary),
            name=args.name,
            patch_allocation=args.patch_allocation,
            patch_internal_offset=args.patch_internal_offset,
            produced_bytes=args.produced_bytes,
            continuation_command_index=args.continuation_command_index,
            remap_end_command=args.remap_end_command,
            scratch_allocation=args.scratch_allocation,
            scratch_end_command=args.scratch_end_command,
        )
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        parser.exit(1, f"error: {exc}\n")

    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(
        json.dumps(plan, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    if args.output_md is not None:
        args.output_md.parent.mkdir(parents=True, exist_ok=True)
        args.output_md.write_text(render_markdown(plan), encoding="utf-8")
    if args.output_c is not None and args.output_h is not None:
        args.output_h.parent.mkdir(parents=True, exist_ok=True)
        args.output_c.parent.mkdir(parents=True, exist_ok=True)
        args.output_h.write_text(
            render_c_header(
                plan,
                header_guard=args.header_guard,
                macro_prefix=args.macro_prefix,
                function_name=args.function_name,
            ),
            encoding="ascii",
        )
        args.output_c.write_text(
            render_c_source(
                plan,
                header_name=args.output_h.name,
                macro_prefix=args.macro_prefix,
                function_name=args.function_name,
            ),
            encoding="ascii",
        )
    print(f"Wrote {args.output_json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
