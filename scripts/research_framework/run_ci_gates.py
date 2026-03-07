#!/usr/bin/env python3
"""Run research-framework CI gates.

Gates include:
1) schema-based validation for all example case files
2) workflow document integrity checks
3) unresolved placeholder scan in docs
"""

from __future__ import annotations

import argparse
from pathlib import Path
import sys

REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.append(str(REPO_ROOT))

from scripts.research_framework.scan_placeholders import scan_for_placeholders
from scripts.research_framework.validate_case import (
    ValidationError,
    load_yaml,
    validate_case,
)
from scripts.research_framework.validate_workflow_doc import validate_workflow_doc


def run_case_validation(schema_path: Path, cases_dir: Path) -> tuple[int, int]:
    schema = load_yaml(schema_path)
    case_files = sorted(cases_dir.glob("*.case.yaml"))

    if not case_files:
        print(f"CASE_VALIDATION_FAILED: no case files found in {cases_dir}")
        return 1, 0

    failures = 0
    for case_path in case_files:
        case = load_yaml(case_path)
        errors = validate_case(schema, case, strict=True)
        if errors:
            failures += 1
            print(f"CASE_VALIDATION_FAILED: {case_path}")
            for err in errors:
                print(f"- {err}")
        else:
            print(f"CASE_VALIDATION_OK: {case_path}")

    return failures, len(case_files)


def run_workflow_validation(workflow_doc: Path) -> int:
    errors = validate_workflow_doc(workflow_doc)
    if errors:
        print(f"WORKFLOW_VALIDATION_FAILED: {workflow_doc}")
        for err in errors:
            print(f"- {err}")
        return 1

    print(f"WORKFLOW_VALIDATION_OK: {workflow_doc}")
    return 0


def run_placeholder_scan(docs_dir: Path) -> int:
    findings = scan_for_placeholders(docs_dir, exclude_dirs={"templates"})
    if findings:
        print(f"PLACEHOLDER_SCAN_FAILED: {docs_dir}")
        for item in findings:
            print(f"- {item}")
        return 1

    print(f"PLACEHOLDER_SCAN_OK: {docs_dir}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Run research framework CI gates")
    parser.add_argument("--schema", required=True, help="Canonical schema YAML")
    parser.add_argument("--cases-dir", required=True, help="Directory with *.case.yaml")
    parser.add_argument(
        "--workflow-doc", required=True, help="Workflow markdown document"
    )
    parser.add_argument(
        "--docs-dir", required=True, help="Research docs root for placeholder scan"
    )
    args = parser.parse_args()

    schema_path = Path(args.schema)
    cases_dir = Path(args.cases_dir)
    workflow_doc = Path(args.workflow_doc)
    docs_dir = Path(args.docs_dir)

    try:
        case_failures, case_total = run_case_validation(schema_path, cases_dir)
    except ValidationError as exc:
        print(f"CASE_VALIDATION_ERROR: {exc}")
        return 2

    workflow_failures = run_workflow_validation(workflow_doc)
    placeholder_failures = run_placeholder_scan(docs_dir)

    total_failures = case_failures + workflow_failures + placeholder_failures
    summary = "CI_GATE_SUMMARY: " + ", ".join(
        [
            f"cases={case_total}",
            f"case_failures={case_failures}",
            f"workflow_failures={workflow_failures}",
            f"placeholder_failures={placeholder_failures}",
        ]
    )
    print(summary)

    if total_failures > 0:
        print("CI_GATES_FAILED")
        return 1

    print("CI_GATES_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
