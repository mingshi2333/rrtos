# Kernel Benchmark Gate Decision Table

Use this table when deciding where a new kernel-related metric belongs in the current gate stack.

The short rule is: keep functional correctness in correctness gates, keep deterministic host-harness tick numbers in the hard baseline gate, keep supported-lane runtime observations as proof, keep only mature size ceilings in the footprint gate, and keep host-proxy handoff timings in the microbench report-only lane.

## Decision Table

| If the new metric is mainly about... | Put it here | Criterion | Current owning scripts/files | Status |
| --- | --- | --- | --- | --- |
| Required behavior being correct at all | Correctness gate | The signal proves semantics or support behavior, and failure means the feature is wrong or unsupported, not just slower/larger | `scripts/validate_kernel_semantics.py`, `tests/kernel/test_main.c`, `scripts/run_mnist_validation.py`, `scripts/validate_preemption_integration.py`, `pixi.toml` | hard-gated |
| Deterministic tick counts from the host semantic harness | Tick baseline hard gate | The metric is emitted by the `OS_TEST_HARNESS` semantic baseline suite and should stay stable enough for exact or narrow tick thresholds | `tests/kernel/test_baseline_metrics.c`, `scripts/measure_kernel_baseline.py`, `logs/kernel_baseline_report.md`, `logs/kernel_baseline_report.json` | hard-gated |
| Supported-lane runtime evidence from QEMU or board-sim logs | Runtime proof | The signal is useful proof that a supported runtime path still behaves as expected, but it is too environment-shaped to enforce as a benchmark threshold | `scripts/run_mnist_validation.py`, `scripts/validate_preemption_integration.py`, `scripts/measure_kernel_baseline.py`, `logs/mnist_validation_qemu.log`, `logs/mnist_preemption_probe_qemu.log` | proof-only |
| Mature, low-noise size ceilings | Footprint gate | The metric is a stable size snapshot with a reviewed upper bound; growth should fail immediately, shrinkage is acceptable | `scripts/report_build_footprint.py`, `scripts/measure_kernel_baseline.py`, `pixi.toml`, `build/apps/mnist_app/mnist_validation_footprint.md`, `build-be_u1000/rrtos_be_u1000_footprint.md` | hard-gated |
| Relative host-side latency or handoff timing | Microbench report-only lane | The metric comes from the `os_cycle_get()` host monotonic proxy and is useful for trend watching, not for correctness or release gating | `tests/kernel/test_microbench_metrics.c`, `tests/kernel/support/kernel_test_runtime.c`, `scripts/measure_kernel_microbench.py`, `scripts/measure_kernel_baseline.py`, `logs/kernel_microbench_report.md` | report-only |

## How To Classify A New Metric

| Question | If yes | If no |
| --- | --- | --- |
| Does failure mean the kernel or supported lane is functionally wrong? | correctness gate | keep checking |
| Is it a deterministic tick count emitted by `tests/kernel/test_baseline_metrics.c` under `OS_TEST_HARNESS`? | tick baseline hard gate | keep checking |
| Does it come from QEMU or Renode runtime logs and serve mainly as deployment/runtime evidence? | runtime proof | keep checking |
| Is it a size/byte count with low enough variance to justify a reviewed ceiling? | footprint gate | keep checking |
| Is it measured through `os_cycle_get()` host timing and mainly useful for regression visibility? | microbench report-only lane | reconsider whether it is the right metric |

## Current Repo Examples

| Category | Examples in this repo |
| --- | --- |
| Correctness gate | `AI_VALIDATION_PASS` and `AI_VALIDATION_METRICS` token checks in `scripts/run_mnist_validation.py`; `PREEMPT_PASS` and `PREEMPT_LOW_READY` checks in `scripts/validate_preemption_integration.py`; canary/core suite pass-fail in `scripts/validate_kernel_semantics.py` |
| Tick baseline hard gate | `scheduler_wake_latency`, `ipc_handoff_latency`, `timer_latency`, `timer_jitter`, `timer_periodic_jitter`, `timer_long_delay_boundary` |
| Runtime proof | `timer_runtime_callback_tick`, `preemption_resume_tick` |
| Footprint gate | `mnist_validation.kernel_archive_bytes`, `rrtos_be_u1000.kernel_archive_bytes`, `rrtos_be_u1000.total_bytes` |
| Microbench report-only lane | `queue_send_to_blocked_recv_handoff`, `mutex_unlock_to_waiter_handoff`, `sem_give_to_blocked_waiter_handoff` |

## Maintainer Workflow

1. Classify the metric first.
   - If it proves correctness, add it to the relevant validation gate instead of `measure_kernel_baseline.py` threshold tables.
   - If it is a deterministic baseline tick metric, emit it from `tests/kernel/test_baseline_metrics.c` and wire its threshold in `scripts/measure_kernel_baseline.py`.
   - If it is runtime evidence, add parsing in the runtime validator or log parser, then surface it in `scripts/measure_kernel_baseline.py` as proof-only.
   - If it is footprint-related, first extend the footprint source in `scripts/report_build_footprint.py` or the produced footprint markdown, then decide whether it stays observational or gets a ceiling in `scripts/measure_kernel_baseline.py`.
   - If it is microbench-only, emit it from `tests/kernel/test_microbench_metrics.c`, make sure the aggregation set is complete, and keep it in `scripts/measure_kernel_microbench.py` / `scripts/measure_kernel_baseline.py` report output.

2. Update the owning gate, not just the report.
   - Correctness metrics usually change the validator script plus the task chain in `pixi.toml`.
   - Tick baseline and footprint metrics usually change both the emitter/parser and the threshold/report tables in `scripts/measure_kernel_baseline.py`.
   - Microbench metrics usually change the measurement suite plus required metric/aggregation lists in `scripts/measure_kernel_baseline.py`.

3. Update the rationale when promotion happens.
   - The benchmark rollout policy lives in `openspec/changes/add-kernel-benchmark-rollout/`.
   - Kernel archive threshold policy lives in `openspec/changes/add-kernel-footprint-threshold-gate/`.
   - Selected total-image threshold policy lives in `openspec/changes/add-total-image-footprint-threshold-gate/`.

## Promotion Rules That Match Current Policy

- Promote into the correctness gate when the metric is a must-pass behavioral invariant.
- Promote into the tick baseline hard gate only when the value is deterministic in the semantic harness.
- Keep runtime metrics as proof when they depend on supported-lane execution logs rather than harness determinism.
- Promote footprint metrics conservatively: only stable, reviewer-approved byte ceilings should hard-fail.
- Keep host-proxy microbench numbers report-only unless the project deliberately creates a new policy wave to change that.
