#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = REPO_ROOT / "build" / "kernel-semantics"
SOURCE_DIR = REPO_ROOT / "tests" / "kernel"
TEST_BIN = BUILD_DIR / "kernel_semantics_tests"
DEFAULT_MD = REPO_ROOT / "logs" / "kernel_baseline_report.md"
DEFAULT_JSON = REPO_ROOT / "logs" / "kernel_baseline_report.json"
DEFAULT_MICROBENCH_MD = REPO_ROOT / "logs" / "kernel_microbench_report.md"
DEFAULT_MICROBENCH_JSON = REPO_ROOT / "logs" / "kernel_microbench_report.json"
MNIST_LOG = REPO_ROOT / "logs" / "mnist_validation_qemu.log"
PREEMPT_LOG = REPO_ROOT / "logs" / "mnist_preemption_probe_qemu.log"
RV32_FOOTPRINT = REPO_ROOT / "build" / "apps" / "mnist_app" / "mnist_validation_footprint.md"
BEU1000_FOOTPRINT = REPO_ROOT / "build-be_u1000" / "rrtos_be_u1000_footprint.md"

BASELINE_RE = re.compile(
    r"BASELINE_METRIC name=(?P<name>\S+) value=(?P<value>\d+) units=(?P<units>\S+) "
    r"source=(?P<source>\S+) scenario=(?P<scenario>\S+)"
)
MICROBENCH_RE = re.compile(
    r"MICROBENCH_METRIC name=(?P<name>\S+) value=(?P<value>\d+) units=(?P<units>\S+) "
    r"source=(?P<source>\S+) scenario=(?P<scenario>\S+) samples=(?P<samples>\d+) "
    r"aggregation=(?P<aggregation>\S+)"
)
TIMER_LOG_RE = re.compile(r"OS_TIMER_CALLBACK_PASS count=(?P<count>\d+) tick=(?P<tick>\d+)")
PREEMPT_LOG_RE = re.compile(r"PREEMPT_PASS tick=(?P<tick>\d+) counter=(?P<counter>\d+)")

SEMANTIC_THRESHOLDS = {
    "scheduler_wake_latency": {"min": 2, "max": 2},
    "ipc_handoff_latency": {"min": 0, "max": 0},
    "timer_latency": {"min": 3, "max": 3},
    "timer_jitter": {"min": 0, "max": 0},
    "timer_periodic_jitter": {"min": 0, "max": 0},
    "timer_long_delay_boundary": {"min": 260, "max": 260},
}

FOOTPRINT_THRESHOLDS = {
    "mnist_validation": {"kernel_archive_bytes_max": 14705},
    "rrtos_be_u1000": {
        "kernel_archive_bytes_max": 14169,
        "total_bytes_max": 122401,
    },
}

REQUIRED_MICROBENCH_METRICS = {
    "queue_send_to_blocked_recv_handoff",
    "mutex_unlock_to_waiter_handoff",
    "sem_give_to_blocked_waiter_handoff",
}
REQUIRED_MICROBENCH_AGGREGATIONS = ("min", "median", "p95", "max", "stddev")
MICROBENCH_ORDER = [
    "queue_send_to_blocked_recv_handoff",
    "mutex_unlock_to_waiter_handoff",
    "sem_give_to_blocked_waiter_handoff",
]
HARDENED_ENV = {
    "LC_ALL": "C",
    "LANG": "C",
    "TZ": "UTC",
    "PYTHONHASHSEED": "0",
    "OMP_NUM_THREADS": "1",
    "OPENBLAS_NUM_THREADS": "1",
    "MKL_NUM_THREADS": "1",
    "NUMEXPR_NUM_THREADS": "1",
}
RUNNER_HARDENING = {
    "enforced": [
        "locale pinned to C",
        "timezone pinned to UTC",
        "Python hash seed pinned to 0",
        "common host math thread pools capped at 1",
    ],
    "best_effort": [],
}


def run(cmd: list[str], *, stable_cpu: bool = False) -> str:
    env = os.environ.copy()
    env.update(HARDENED_ENV)
    actual_cmd = cmd
    pinned = False

    if stable_cpu:
        taskset = shutil.which("taskset")
        if taskset:
            actual_cmd = [taskset, "--cpu-list", "0", *cmd]
            pinned = True
        elif "CPU affinity pinning unavailable (taskset not found)" not in RUNNER_HARDENING["best_effort"]:
            RUNNER_HARDENING["best_effort"].append(
                "CPU affinity pinning unavailable (taskset not found)"
            )

    proc = subprocess.run(
        actual_cmd,
        cwd=REPO_ROOT,
        env=env,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )

    if pinned and proc.returncode != 0 and proc.stdout.startswith("taskset:"):
        note = "CPU affinity pinning requested but fell back to default scheduling"
        if note not in RUNNER_HARDENING["best_effort"]:
            RUNNER_HARDENING["best_effort"].append(note)
        sys.stdout.write(proc.stdout)
        proc = subprocess.run(
            cmd,
            cwd=REPO_ROOT,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
    elif pinned:
        note = "CPU affinity pinned to logical CPU 0 for microbench collection"
        if note not in RUNNER_HARDENING["enforced"]:
            RUNNER_HARDENING["enforced"].append(note)

    sys.stdout.write(proc.stdout)
    if proc.returncode != 0:
        raise SystemExit(proc.returncode)
    return proc.stdout


def ensure_semantic_binary() -> None:
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    run(["cmake", "-S", str(SOURCE_DIR), "-B", str(BUILD_DIR), "-G", "Ninja"])
    run(["cmake", "--build", str(BUILD_DIR)])


def require_metric_names(items: list[dict[str, object]], required: set[str], kind: str) -> None:
    found = {str(item["name"]) for item in items}
    missing = sorted(required - found)
    if missing:
        joined = ", ".join(missing)
        raise SystemExit(f"Benchmark measurement error: missing {kind} metrics: {joined}")


def collect_semantic_metrics() -> list[dict[str, object]]:
    output = run([str(TEST_BIN), "--suite", "baseline"], stable_cpu=True)
    metrics: list[dict[str, object]] = []
    for match in BASELINE_RE.finditer(output):
        metrics.append(
            {
                "name": match.group("name"),
                "value": int(match.group("value")),
                "units": match.group("units"),
                "source": match.group("source"),
                "scenario": match.group("scenario"),
            }
        )
    require_metric_names(metrics, set(SEMANTIC_THRESHOLDS), "semantic baseline")
    return metrics


def evaluate_semantic_thresholds(metrics: list[dict[str, object]]) -> list[dict[str, object]]:
    by_name = {str(item["name"]): item for item in metrics}
    checks: list[dict[str, object]] = []
    failures: list[str] = []

    for name, limits in SEMANTIC_THRESHOLDS.items():
        item = by_name[name]
        value = int(item["value"])
        passed = limits["min"] <= value <= limits["max"]
        checks.append(
            {
                "name": name,
                "value": value,
                "units": item["units"],
                "scenario": item["scenario"],
                "source": item["source"],
                "threshold_min": limits["min"],
                "threshold_max": limits["max"],
                "status": "pass" if passed else "fail",
            }
        )
        if not passed:
            failures.append(
                f"{name}={value} outside [{limits['min']}, {limits['max']}]"
            )

    if failures:
        raise SystemExit("Hard baseline threshold failure: " + "; ".join(failures))

    return checks


def collect_microbench_metrics() -> list[dict[str, object]]:
    output = run([str(TEST_BIN), "--suite", "microbench"], stable_cpu=True)
    metrics: list[dict[str, object]] = []
    for match in MICROBENCH_RE.finditer(output):
        metrics.append(
            {
                "name": match.group("name"),
                "value": int(match.group("value")),
                "units": match.group("units"),
                "source": match.group("source"),
                "scenario": match.group("scenario"),
                "samples": int(match.group("samples")),
                "aggregation": match.group("aggregation"),
            }
        )

    require_metric_names(metrics, REQUIRED_MICROBENCH_METRICS, "microbench")

    variants = {(str(item["name"]), str(item["aggregation"])) for item in metrics}
    missing_variants: list[str] = []
    for name in sorted(REQUIRED_MICROBENCH_METRICS):
        for aggregation in REQUIRED_MICROBENCH_AGGREGATIONS:
            if (name, aggregation) not in variants:
                missing_variants.append(f"{name}:{aggregation}")
    if missing_variants:
        raise SystemExit(
            "Benchmark measurement error: missing microbench aggregations: "
            + ", ".join(missing_variants)
        )

    return metrics


def group_microbench_metrics(metrics: list[dict[str, object]]) -> list[dict[str, object]]:
    grouped: dict[str, dict[str, object]] = {}
    for item in metrics:
        name = str(item["name"])
        group = grouped.setdefault(
            name,
            {
                "name": name,
                "units": item["units"],
                "source": item["source"],
                "scenario": item["scenario"],
                "samples": item["samples"],
                "stats": {},
            },
        )
        group["stats"][str(item["aggregation"])] = item["value"]

    result = []
    for name in MICROBENCH_ORDER:
        if name in grouped:
            result.append(grouped[name])
    return result


def require_text(path: Path) -> str:
    if not path.exists():
        raise SystemExit(f"Benchmark measurement error: missing required artifact: {path}")
    return path.read_text(encoding="utf-8")


def parse_timer_log() -> dict[str, object]:
    text = require_text(MNIST_LOG)
    match = TIMER_LOG_RE.search(text)
    if not match:
        raise SystemExit("Benchmark measurement error: missing OS_TIMER_CALLBACK_PASS token")
    return {
        "name": "timer_runtime_callback_tick",
        "value": int(match.group("tick")),
        "units": "ticks",
        "source": "supported-rv32-log",
        "scenario": "mnist-validation-runtime",
        "callback_count": int(match.group("count")),
        "artifact": str(MNIST_LOG.relative_to(REPO_ROOT)),
    }


def parse_preempt_log() -> dict[str, object]:
    text = require_text(PREEMPT_LOG)
    match = PREEMPT_LOG_RE.search(text)
    if not match:
        raise SystemExit("Benchmark measurement error: missing PREEMPT_PASS token")
    return {
        "name": "preemption_resume_tick",
        "value": int(match.group("tick")),
        "units": "ticks",
        "source": "supported-rv32-log",
        "scenario": "mnist-preemption-probe",
        "counter": int(match.group("counter")),
        "artifact": str(PREEMPT_LOG.relative_to(REPO_ROOT)),
    }


def parse_footprint(path: Path, image_key: str) -> dict[str, object]:
    text = require_text(path)
    total_match = re.search(r"\| Total \| (\d+) ", text)
    kernel_match = re.search(r"\| dec \| (\d+) ", text)
    if not total_match or not kernel_match:
        raise SystemExit(f"Benchmark measurement error: could not parse footprint report: {path}")
    return {
        "image": image_key,
        "total_bytes": int(total_match.group(1)),
        "kernel_archive_bytes": int(kernel_match.group(1)),
        "artifact": str(path.relative_to(REPO_ROOT)),
    }


def evaluate_footprint_thresholds(footprints: list[dict[str, object]]) -> list[dict[str, object]]:
    by_image = {str(item["image"]): item for item in footprints}
    checks: list[dict[str, object]] = []
    failures: list[str] = []

    for image, limits in FOOTPRINT_THRESHOLDS.items():
        item = by_image.get(image)
        if item is None:
            failures.append(f"missing footprint snapshot for {image}")
            continue

        for metric, threshold_max in limits.items():
            value_key = metric.removesuffix("_max")
            value = int(item[value_key])
            passed = value <= threshold_max
            checks.append(
                {
                    "image": image,
                    "metric": value_key,
                    "value": value,
                    "threshold_max": threshold_max,
                    "artifact": item["artifact"],
                    "status": "pass" if passed else "fail",
                }
            )
            if not passed:
                failures.append(f"{image}.{value_key}={value} exceeds {threshold_max}")

    if failures:
        raise SystemExit("Hard footprint threshold failure: " + "; ".join(failures))

    return checks


def write_microbench_report(md_path: Path, json_path: Path, payload: dict[str, object]) -> None:
    md_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.parent.mkdir(parents=True, exist_ok=True)

    lines = [
        "# Kernel Microbench Report",
        "",
        "This lane collects host-proxy microbench evidence only.",
        "It is intentionally separate from semantic correctness suites and hard tick-baseline threshold checks.",
        "The values below are not hardware cycles; they come from the `os_cycle_get()` host monotonic proxy under `OS_TEST_HARNESS`.",
        "",
        "## Runner Hardening",
        "",
        "### Enforced",
    ]

    for note in payload["runner_hardening"]["enforced"]:
        lines.append(f"- {note}")

    lines.extend(["", "### Best-effort only"])
    best_effort = payload["runner_hardening"]["best_effort"]
    if best_effort:
        for note in best_effort:
            lines.append(f"- {note}")
    else:
        lines.append("- None")

    lines.extend(
        [
            "",
            "## Microbench Metrics",
            "",
            "| Metric | Units | Source | Samples | Min | Median | p95 | Max | Stddev |",
            "| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |",
        ]
    )

    for item in payload["microbench_metrics"]:
        stats = item["stats"]
        lines.append(
            f"| {item['name']} | {item['units']} | {item['source']} | {item['samples']} | {stats['min']} | {stats['median']} | {stats['p95']} | {stats['max']} | {stats['stddev']} |"
        )

    lines.extend(
        [
            "",
            "## Notes",
            "",
            "- `sem_give_to_blocked_waiter_handoff` is required collection evidence in this lane.",
            "- Microbench values remain report-only and do not participate in the hard benchmark gate.",
            "",
        ]
    )

    md_path.write_text("\n".join(lines), encoding="utf-8")
    json_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def write_reports(md_path: Path, json_path: Path, payload: dict[str, object]) -> None:
    md_path.parent.mkdir(parents=True, exist_ok=True)
    json_path.parent.mkdir(parents=True, exist_ok=True)

    semantic = payload["semantic_metrics"]
    semantic_thresholds = payload["semantic_thresholds"]
    microbench = payload["microbench_metrics"]
    runtime = payload["runtime_evidence"]
    footprints = payload["footprints"]
    footprint_thresholds = payload["footprint_thresholds"]

    lines = [
        "# Kernel Baseline Report",
        "",
        "This report records reproducible benchmark evidence after the semantic and supported-lane prerequisites pass.",
        "Semantic-harness tick baselines are now hard threshold checks for the benchmark gate.",
            "Microbench output stays report-only, runtime evidence stays proof-only, selected kernel archive metrics stay hard thresholds, and one stable total image byte metric now acts as a conservative hard upper-bound while other total image bytes remain observational.",
        "",
        "## Runner Hardening",
        "",
        "### Enforced",
    ]

    for note in payload["runner_hardening"]["enforced"]:
        lines.append(f"- {note}")

    lines.extend(["", "### Best-effort only"])
    best_effort = payload["runner_hardening"]["best_effort"]
    if best_effort:
        for note in best_effort:
            lines.append(f"- {note}")
    else:
        lines.append("- None")

    lines.extend(
        [
            "",
            "## Semantic Baselines",
            "",
            "| Metric | Value | Threshold | Units | Source | Scenario | Gate |",
            "| --- | ---: | --- | --- | --- | --- | --- |",
        ]
    )
    threshold_by_name = {item["name"]: item for item in semantic_thresholds}
    for item in semantic:
        threshold = threshold_by_name[item["name"]]
        lines.append(
            f"| {item['name']} | {item['value']} | [{threshold['threshold_min']}, {threshold['threshold_max']}] | {item['units']} | {item['source']} | {item['scenario']} | {threshold['status']} |"
        )

    lines.extend(
        [
            "",
            "## Harness Cycle Microbenchmarks",
            "",
            "These values use the `os_cycle_get()` harness backend, which maps to a host monotonic proxy under `OS_TEST_HARNESS`.",
            "They are useful for relative regression tracking only and must not be read as real hardware cycle counts.",
            "",
            "| Metric | Units | Source | Samples | Min | Median | p95 | Max | Stddev |",
            "| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |",
        ]
    )
    for item in microbench:
        stats = item["stats"]
        lines.append(
            f"| {item['name']} | {item['units']} | {item['source']} | {item['samples']} | {stats['min']} | {stats['median']} | {stats['p95']} | {stats['max']} | {stats['stddev']} |"
        )

    lines.extend(
        [
            "",
            "## Runtime Evidence",
            "",
            "| Metric | Value | Units | Source | Scenario | Artifact |",
            "| --- | ---: | --- | --- | --- | --- |",
        ]
    )
    for item in runtime:
        lines.append(
            f"| {item['name']} | {item['value']} | {item['units']} | {item['source']} | {item['scenario']} | `{item['artifact']}` |"
        )

    lines.extend(
        [
            "",
            "## Footprint Thresholds",
            "",
            "| Image | Metric | Value | Threshold | Artifact | Gate |",
            "| --- | --- | ---: | --- | --- | --- |",
        ]
    )
    for item in footprint_thresholds:
        lines.append(
            f"| {item['image']} | {item['metric']} | {item['value']} | <= {item['threshold_max']} | `{item['artifact']}` | {item['status']} |"
        )

    lines.extend(
        [
            "",
            "## Footprint Snapshots",
            "",
            "| Image | Total Bytes | Kernel Archive Bytes | Total Gate | Archive Gate | Artifact |",
            "| --- | ---: | ---: | --- | --- | --- |",
        ]
    )
    threshold_by_image_metric = {
        (item["image"], item["metric"]): item for item in footprint_thresholds
    }
    for item in footprints:
        total_threshold = threshold_by_image_metric.get((item["image"], "total_bytes"))
        archive_threshold = threshold_by_image_metric.get((item["image"], "kernel_archive_bytes"))
        total_gate = total_threshold["status"] if total_threshold else "report-only"
        archive_gate = archive_threshold["status"] if archive_threshold else "report-only"
        lines.append(
            f"| {item['image']} | {item['total_bytes']} | {item['kernel_archive_bytes']} | {total_gate} | {archive_gate} | `{item['artifact']}` |"
        )

    lines.extend(
        [
            "",
            "## Notes",
            "",
            "- Hard threshold drift in any semantic-harness tick metric fails this benchmark gate immediately.",
            "- `scheduler_wake_latency`, `ipc_handoff_latency`, `timer_latency`, `timer_jitter`, `timer_periodic_jitter`, and `timer_long_delay_boundary` all remain deterministic semantic-harness tick metrics.",
            "- Microbench metrics keep correctness gating separate and now report `min`, `median`, `p95`, `max`, and `stddev` for each required handoff path.",
            "- `timer_runtime_callback_tick` and `preemption_resume_tick` come from supported RV32 QEMU logs and remain proof, not thresholds.",
            "- `kernel_archive_bytes` remains a hard upper-bound threshold for `mnist_validation` and `rrtos_be_u1000`.",
            "- `rrtos_be_u1000.total_bytes` is now a hard upper-bound threshold because the current board image layout is the most stable total-image promotion candidate.",
            "- `mnist_validation.total_bytes` remains collected for visibility only in this rollout phase.",
            "- SMP remains experimental and is intentionally excluded from this report.",
            "",
        ]
    )

    md_path.write_text("\n".join(lines), encoding="utf-8")
    json_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def build_microbench_payload() -> dict[str, object]:
    ensure_semantic_binary()
    return {
        "microbench_metrics": group_microbench_metrics(collect_microbench_metrics()),
        "notes": {
            "microbench_basis": "host monotonic proxy via os_cycle_get() under OS_TEST_HARNESS",
            "gating": "report-only lane",
        },
        "runner_hardening": RUNNER_HARDENING,
    }


def build_full_payload() -> dict[str, object]:
    ensure_semantic_binary()
    semantic_metrics = collect_semantic_metrics()
    semantic_thresholds = evaluate_semantic_thresholds(semantic_metrics)
    microbench_metrics = group_microbench_metrics(collect_microbench_metrics())
    runtime_evidence = [parse_timer_log(), parse_preempt_log()]
    footprints = [
        parse_footprint(RV32_FOOTPRINT, "mnist_validation"),
        parse_footprint(BEU1000_FOOTPRINT, "rrtos_be_u1000"),
    ]
    footprint_thresholds = evaluate_footprint_thresholds(footprints)

    return {
        "semantic_metrics": semantic_metrics,
        "semantic_thresholds": semantic_thresholds,
        "microbench_metrics": microbench_metrics,
        "runtime_evidence": runtime_evidence,
        "footprints": footprints,
        "footprint_thresholds": footprint_thresholds,
        "notes": {
            "latency_basis": "deterministic semantic-harness ticks",
            "microbench_basis": "host monotonic proxy via os_cycle_get() under OS_TEST_HARNESS",
            "runtime_basis": "supported-lane QEMU logs",
            "footprint_basis": "kernel archive hard ceilings with one conservative total image ceiling and observation-only remaining total image snapshots",
            "smp_scope": "experimental and excluded",
        },
        "runner_hardening": RUNNER_HARDENING,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", default=str(DEFAULT_MD), help="Markdown report path")
    parser.add_argument("--json", default=str(DEFAULT_JSON), help="JSON report path")
    args = parser.parse_args()

    payload = build_full_payload()
    write_reports(Path(args.output), Path(args.json), payload)
    print(f"Baseline report written to {args.output}")
    print(f"Baseline JSON written to {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
