#!/usr/bin/env python3
"""Validate research case YAML against canonical schema v0 (basic rules).

Usage:
  python scripts/research_framework/validate_case.py \
    --schema docs/research_framework/05_CANONICAL_SCHEMA_V0.yaml \
    --case docs/research_framework/examples/RC-0001.case.yaml
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from typing import Any, Mapping, cast

import yaml


REQUIRED_TOP_LEVEL = [
    "case_id",
    "title",
    "owner",
    "objective",
    "hypothesis",
    "environment",
    "procedure",
    "artifacts",
    "results",
    "conclusion",
    "replay",
    "provenance",
]

REQUIRED_PROVENANCE = [
    "source_commit",
    "source_branch",
    "actor",
    "generated_by",
    "generated_at",
]


class ValidationError(Exception):
    pass


def load_yaml(path: Path) -> dict[str, Any]:
    if not path.exists():
        raise ValidationError(f"File not found: {path}")
    with path.open("r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
    if not isinstance(data, dict):
        raise ValidationError(f"YAML root must be a mapping: {path}")
    return cast(dict[str, Any], data)


def non_empty_string(value: object) -> bool:
    return isinstance(value, str) and value.strip() != ""


def validate_case(
    schema: Mapping[str, Any], case: Mapping[str, Any], strict: bool
) -> list[str]:
    errors: list[str] = []

    for key in REQUIRED_TOP_LEVEL:
        if key not in case:
            errors.append(f"missing top-level key: {key}")

    if errors:
        return errors

    id_patterns_obj = schema.get("id_patterns", {})
    if isinstance(id_patterns_obj, Mapping):
        pattern_candidate = id_patterns_obj.get("case_id", r"^RC-[0-9]{4}$")
    else:
        pattern_candidate = r"^RC-[0-9]{4}$"
    case_id_pattern = (
        pattern_candidate if isinstance(pattern_candidate, str) else r"^RC-[0-9]{4}$"
    )
    case_id_obj = case.get("case_id")
    case_id = case_id_obj if isinstance(case_id_obj, str) else ""
    if not non_empty_string(case_id) or re.match(case_id_pattern, case_id) is None:
        errors.append(
            f"invalid case_id: {case_id!r} (expected pattern {case_id_pattern})"
        )

    for key in ["title", "owner", "objective"]:
        if not non_empty_string(case.get(key, "")):
            errors.append(f"invalid {key}: must be non-empty string")

    conclusion_obj = case.get("conclusion", {})
    conclusion: Mapping[str, Any] = (
        cast(Mapping[str, Any], conclusion_obj)
        if isinstance(conclusion_obj, Mapping)
        else {}
    )
    evidence_grade = conclusion.get("evidence_grade")

    enums_obj = schema.get("enums", {})
    enum_map: Mapping[str, Any] = (
        cast(Mapping[str, Any], enums_obj) if isinstance(enums_obj, Mapping) else {}
    )
    grades_obj = enum_map.get("evidence_grade", ["G0", "G1", "G2", "G3", "G4"])
    allowed_grades = {x for x in grades_obj if isinstance(x, str)}
    if evidence_grade not in allowed_grades:
        errors.append(
            f"invalid evidence_grade: {evidence_grade!r}; allowed={sorted(allowed_grades)}"
        )

    replay_obj = case.get("replay", {})
    replay: Mapping[str, Any] = (
        cast(Mapping[str, Any], replay_obj) if isinstance(replay_obj, Mapping) else {}
    )
    if evidence_grade != "G0":
        if not non_empty_string(replay.get("command", "")):
            errors.append("replay.command is required when evidence_grade != G0")

    provenance_obj = case.get("provenance", {})
    provenance: Mapping[str, Any] = (
        cast(Mapping[str, Any], provenance_obj)
        if isinstance(provenance_obj, Mapping)
        else {}
    )
    if not isinstance(provenance_obj, Mapping):
        errors.append("provenance must be a mapping")
    else:
        for key in REQUIRED_PROVENANCE:
            if not non_empty_string(provenance.get(key, "")):
                errors.append(f"missing provenance.{key}")

    procedure_obj = case.get("procedure", {})
    procedure: Mapping[str, Any] = (
        cast(Mapping[str, Any], procedure_obj)
        if isinstance(procedure_obj, Mapping)
        else {}
    )
    if not isinstance(procedure_obj, Mapping):
        errors.append("procedure must be a mapping")
    else:
        measured_count = procedure.get("measured_count", 0)
        if not isinstance(measured_count, int) or measured_count <= 0:
            errors.append("procedure.measured_count must be a positive integer")

    if strict:
        results_obj = case.get("results", {})
        results: Mapping[str, Any] = (
            cast(Mapping[str, Any], results_obj)
            if isinstance(results_obj, Mapping)
            else {}
        )
        metrics = results.get("metrics", [])
        if not isinstance(metrics, list) or len(metrics) == 0:
            errors.append(
                "strict mode: results.metrics must contain at least one metric"
            )

        artifacts_obj = case.get("artifacts", {})
        artifacts: Mapping[str, Any] = (
            cast(Mapping[str, Any], artifacts_obj)
            if isinstance(artifacts_obj, Mapping)
            else {}
        )
        if not isinstance(artifacts_obj, Mapping):
            errors.append("strict mode: artifacts must be a mapping")
        else:
            has_any_artifact = any(
                isinstance(v, list) and len(v) > 0 for v in artifacts.values()
            )
            if not has_any_artifact:
                errors.append(
                    "strict mode: at least one artifact list must be non-empty"
                )

    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate research case YAML")
    parser.add_argument("--schema", required=True, help="Path to canonical schema YAML")
    parser.add_argument("--case", required=True, help="Path to case YAML")
    parser.add_argument("--strict", action="store_true", help="Enable stricter checks")
    args = parser.parse_args()

    try:
        schema = load_yaml(Path(args.schema))
        case = load_yaml(Path(args.case))
        errors = validate_case(schema, case, strict=args.strict)
        if errors:
            print("VALIDATION_FAILED")
            for err in errors:
                print(f"- {err}")
            return 1

        print("VALIDATION_OK")
        print(f"case_id={case.get('case_id')}")
        return 0
    except ValidationError as exc:
        print(f"VALIDATION_ERROR: {exc}")
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
