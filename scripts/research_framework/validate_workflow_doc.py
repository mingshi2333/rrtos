#!/usr/bin/env python3
"""Validate workflow draft document for required states and gates."""

from __future__ import annotations

import argparse
from pathlib import Path


REQUIRED_STATES = [
    "intake",
    "plan",
    "execute",
    "collect",
    "evaluate",
    "propose_claim",
    "human_review",
    "accept_or_reject",
    "publish",
]

REQUIRED_GATES = ["G1", "G2", "G3", "G4", "G5"]

REQUIRED_SECTIONS = [
    "State Machine",
    "Mandatory Gates",
    "Agent Action Contract",
    "Audit Event Schema",
]


def validate_workflow_doc(path: Path) -> list[str]:
    errors: list[str] = []

    if not path.exists() or not path.is_file():
        return [f"workflow doc not found: {path}"]

    content = path.read_text(encoding="utf-8", errors="ignore")

    for section in REQUIRED_SECTIONS:
        if section not in content:
            errors.append(f"missing required section: {section}")

    for state in REQUIRED_STATES:
        if state not in content:
            errors.append(f"missing required workflow state token: {state}")

    for gate in REQUIRED_GATES:
        if f"Gate {gate}" not in content:
            errors.append(f"missing required gate token: Gate {gate}")

    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate workflow draft doc")
    parser.add_argument("--doc", required=True, help="Workflow markdown file path")
    args = parser.parse_args()

    errors = validate_workflow_doc(Path(args.doc))
    if errors:
        print("WORKFLOW_VALIDATION_FAILED")
        for err in errors:
            print(f"- {err}")
        return 1

    print("WORKFLOW_VALIDATION_OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
