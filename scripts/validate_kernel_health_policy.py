#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path


CHANGE_NAME = "做一个全面评估,关于我目前的kernel设计,关于其耦合话以及其他各种方面的设计,以及内核是否健全,性能是否足够好,并且以测试为导向,严格编排"
REPO_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_MD = REPO_ROOT / "logs" / "kernel_health_policy_report.md"
DEFAULT_JSON = REPO_ROOT / "logs" / "kernel_health_policy_report.json"
BASELINE_REPORT = REPO_ROOT / "logs" / "kernel_baseline_report.md"
SMP_RUNTIME_REPORT = REPO_ROOT / "logs" / "be_u1000_selftest_runtime.md"
PIXI_FILE = REPO_ROOT / "pixi.toml"


@dataclass
class Evidence:
    name: str
    semantic_gate: bool
    integration_gate: bool
    baseline_report: bool
    explicit_acceptance_artifact: bool
    known_blockers: list[str]
    missing_semantic_gaps: list[str]
    smp_experimental_label: bool
    smp_supported_claim: bool


@dataclass
class CaseExpectation:
    evidence: Evidence
    expect_ready: bool
    expected_reasons: list[str]


def require_text(path: Path) -> str:
    if not path.exists():
        raise SystemExit(f"Kernel health policy error: missing required artifact: {path}")
    return path.read_text(encoding="utf-8")


def evaluate(evidence: Evidence) -> dict[str, object]:
    reasons: list[str] = []

    if not evidence.explicit_acceptance_artifact:
        reasons.append("missing explicit acceptance artifact")
    if not evidence.semantic_gate:
        reasons.append("missing pure-kernel semantic proof")
    if not evidence.integration_gate:
        reasons.append("missing supported integration proof")

    for blocker in evidence.known_blockers:
        reasons.append(f"known blocker remains: {blocker}")
    for gap in evidence.missing_semantic_gaps:
        reasons.append(f"missing semantic coverage: {gap}")

    if evidence.integration_gate and not evidence.semantic_gate:
        reasons.append("simulator-only evidence is insufficient")

    correctness_gap = bool(
        evidence.known_blockers
        or evidence.missing_semantic_gaps
        or (evidence.integration_gate and not evidence.semantic_gate)
        or not evidence.semantic_gate
    )
    if evidence.baseline_report and correctness_gap:
        reasons.append("performance evidence cannot waive correctness gaps")

    if not evidence.smp_experimental_label:
        reasons.append("SMP evidence is not labeled experimental")
    if evidence.smp_supported_claim:
        reasons.append("SMP scope expanded beyond the supported single-core target")

    ready = len(reasons) == 0
    return {
        "ready": ready,
        "reasons": reasons,
        "proof_summary": {
            "semantic_gate": evidence.semantic_gate,
            "integration_gate": evidence.integration_gate,
            "baseline_report": evidence.baseline_report,
            "explicit_acceptance_artifact": evidence.explicit_acceptance_artifact,
            "smp_experimental_label": evidence.smp_experimental_label,
            "smp_supported_claim": evidence.smp_supported_claim,
        },
    }


def ensure_artifact_contract() -> dict[str, object]:
    baseline_text = require_text(BASELINE_REPORT)
    smp_runtime_text = require_text(SMP_RUNTIME_REPORT)
    pixi_text = require_text(PIXI_FILE)
    rv32_config = (
        'configure = "cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake '
        '-DARCH_BITS=32 -DARCH=riscv -DCMAKE_BUILD_TYPE=MinSizeRel -DOS_SMP_EN=OFF"'
    )
    be_u1000_config = (
        'configure = "cmake -B build-be_u1000 -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake '
        '-DARCH_BITS=32 -DCONFIG_BOARD=be_u1000 -DCMAKE_BUILD_TYPE=MinSizeRel -DOS_SMP_EN=OFF"'
    )

    checks = [
        {
            "name": "baseline-report-keeps-smp-experimental",
            "passed": "SMP remains experimental and is intentionally excluded from this report." in baseline_text,
            "artifact": str(BASELINE_REPORT.relative_to(REPO_ROOT)),
            "detail": "Kernel baseline report must fence SMP out of the supported health conclusion.",
        },
        {
            "name": "smp-runtime-report-disclaims-supported-status",
            "passed": "This probe is experimental." in smp_runtime_text
            and "not proof of SMP support" in smp_runtime_text,
            "artifact": str(SMP_RUNTIME_REPORT.relative_to(REPO_ROOT)),
            "detail": "SMP runtime evidence must stay observational and non-promotional.",
        },
        {
            "name": "supported-rv32-lane-forces-single-core",
            "passed": rv32_config in pixi_text,
            "artifact": str(PIXI_FILE.relative_to(REPO_ROOT)),
            "detail": "Supported RV32 gate must continue building with SMP disabled.",
        },
        {
            "name": "supported-be-u1000-lane-forces-single-core",
            "passed": be_u1000_config in pixi_text,
            "artifact": str(PIXI_FILE.relative_to(REPO_ROOT)),
            "detail": "Supported BE-U1000 gate must continue building with SMP disabled.",
        },
    ]

    failed = [item for item in checks if not item["passed"]]
    if failed:
        names = ", ".join(item["name"] for item in failed)
        raise SystemExit(f"Kernel health policy error: artifact contract failed: {names}")

    return {
        "checks": checks,
        "artifacts": {
            "baseline_report": str(BASELINE_REPORT.relative_to(REPO_ROOT)),
            "smp_runtime_report": str(SMP_RUNTIME_REPORT.relative_to(REPO_ROOT)),
            "pixi_file": str(PIXI_FILE.relative_to(REPO_ROOT)),
        },
    }


def build_cases() -> list[CaseExpectation]:
    artifact_contract = ensure_artifact_contract()
    base = Evidence(
        name="explicit-proof-acceptance",
        semantic_gate=True,
        integration_gate=True,
        baseline_report=True,
        explicit_acceptance_artifact=True,
        known_blockers=[],
        missing_semantic_gaps=[],
        smp_experimental_label=True,
        smp_supported_claim=False,
    )

    cases = [
        CaseExpectation(
            evidence=base,
            expect_ready=True,
            expected_reasons=[],
        ),
        CaseExpectation(
            evidence=Evidence(
                name="blocker-rejection",
                semantic_gate=True,
                integration_gate=True,
                baseline_report=True,
                explicit_acceptance_artifact=True,
                known_blockers=["ipc wakeup reinsertion blocker"],
                missing_semantic_gaps=[],
                smp_experimental_label=True,
                smp_supported_claim=False,
            ),
            expect_ready=False,
            expected_reasons=[
                "known blocker remains: ipc wakeup reinsertion blocker",
                "performance evidence cannot waive correctness gaps",
            ],
        ),
        CaseExpectation(
            evidence=Evidence(
                name="missing-semantic-gap-rejection",
                semantic_gate=True,
                integration_gate=True,
                baseline_report=False,
                explicit_acceptance_artifact=True,
                known_blockers=[],
                missing_semantic_gaps=["event wait deterministic semantic coverage"],
                smp_experimental_label=True,
                smp_supported_claim=False,
            ),
            expect_ready=False,
            expected_reasons=[
                "missing semantic coverage: event wait deterministic semantic coverage",
            ],
        ),
        CaseExpectation(
            evidence=Evidence(
                name="simulator-only-rejection",
                semantic_gate=False,
                integration_gate=True,
                baseline_report=False,
                explicit_acceptance_artifact=True,
                known_blockers=[],
                missing_semantic_gaps=[],
                smp_experimental_label=True,
                smp_supported_claim=False,
            ),
            expect_ready=False,
            expected_reasons=[
                "missing pure-kernel semantic proof",
                "simulator-only evidence is insufficient",
            ],
        ),
        CaseExpectation(
            evidence=Evidence(
                name="performance-does-not-waive-correctness",
                semantic_gate=True,
                integration_gate=True,
                baseline_report=True,
                explicit_acceptance_artifact=True,
                known_blockers=[],
                missing_semantic_gaps=["queue timeout semantic coverage"],
                smp_experimental_label=True,
                smp_supported_claim=False,
            ),
            expect_ready=False,
            expected_reasons=[
                "missing semantic coverage: queue timeout semantic coverage",
                "performance evidence cannot waive correctness gaps",
            ],
        ),
        CaseExpectation(
            evidence=Evidence(
                name="smp-scope-expansion-rejection",
                semantic_gate=True,
                integration_gate=True,
                baseline_report=True,
                explicit_acceptance_artifact=True,
                known_blockers=[],
                missing_semantic_gaps=[],
                smp_experimental_label=False,
                smp_supported_claim=True,
            ),
            expect_ready=False,
            expected_reasons=[
                "SMP evidence is not labeled experimental",
                "SMP scope expanded beyond the supported single-core target",
            ],
        ),
    ]

    for case in cases:
        case.evidence.explicit_acceptance_artifact = (
            case.evidence.explicit_acceptance_artifact and bool(artifact_contract["checks"])
        )

    return cases


def run_cases() -> dict[str, object]:
    artifact_contract = ensure_artifact_contract()
    case_results: list[dict[str, object]] = []

    for case in build_cases():
        result = evaluate(case.evidence)
        reasons = list(result["reasons"])
        missing = [item for item in case.expected_reasons if item not in reasons]
        passed = bool(result["ready"] == case.expect_ready and not missing)
        case_results.append(
            {
                "name": case.evidence.name,
                "expected_ready": case.expect_ready,
                "actual_ready": result["ready"],
                "expected_reasons": case.expected_reasons,
                "actual_reasons": reasons,
                "proof_summary": result["proof_summary"],
                "status": "pass" if passed else "fail",
                "missing_expected_reasons": missing,
            }
        )
        token = "PASS" if passed else "FAIL"
        print(f"KERNEL_HEALTH_POLICY_{token} case={case.evidence.name}")

    failures = [item for item in case_results if item["status"] != "pass"]
    if failures:
        names = ", ".join(item["name"] for item in failures)
        raise SystemExit(f"Kernel health policy error: failing policy cases: {names}")

    return {
        "change": CHANGE_NAME,
        "artifact_contract": artifact_contract,
        "cases": case_results,
        "summary": {
            "total_cases": len(case_results),
            "passed_cases": len(case_results),
            "failed_cases": 0,
        },
    }


def write_reports(md_path: Path, json_path: Path, payload: dict[str, object]) -> None:
    md_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.parent.mkdir(parents=True, exist_ok=True)

    lines = [
        "# Kernel Health Policy Report",
        "",
        "This report proves the assessment policy remains correctness-first.",
        "It exercises positive and negative readiness cases so blockers, semantic gaps, simulator-only evidence, and performance-only evidence cannot be misreported as kernel health.",
        "",
        "## Artifact Contract Checks",
        "",
        "| Check | Artifact | Result | Detail |",
        "| --- | --- | --- | --- |",
    ]

    for item in payload["artifact_contract"]["checks"]:
        result = "pass" if item["passed"] else "fail"
        lines.append(
            f"| {item['name']} | `{item['artifact']}` | {result} | {item['detail']} |"
        )

    lines.extend(
        [
            "",
            "## Policy Cases",
            "",
            "| Case | Expected Ready | Actual Ready | Result | Notes |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for item in payload["cases"]:
        note = "; ".join(item["actual_reasons"]) or "explicit proof accepted"
        lines.append(
            f"| {item['name']} | {str(item['expected_ready']).lower()} | {str(item['actual_ready']).lower()} | {item['status']} | {note} |"
        )

    lines.extend(
        [
            "",
            "## Notes",
            "",
            "- The positive case requires semantic proof, supported integration proof, explicit acceptance evaluation, and the existing single-core SMP fence.",
            "- The negative cases intentionally prove that remaining blockers, missing semantic coverage, simulator-only evidence, and baseline reports without correctness cannot pass readiness.",
            "- SMP scope remains fenced by supported build flags and report wording rather than widened by observational runtime probes.",
            "",
        ]
    )

    md_path.write_text("\n".join(lines), encoding="utf-8")
    json_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_MD), help="Markdown report path")
    parser.add_argument("--json", default=str(DEFAULT_JSON), help="JSON report path")
    args = parser.parse_args()

    payload = run_cases()
    write_reports(Path(args.output), Path(args.json), payload)
    print(f"Kernel health policy report written to {args.output}")
    print(f"Kernel health policy JSON written to {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
