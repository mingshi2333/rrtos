# Kernel Baseline Report

This report records reproducible benchmark evidence after the semantic and supported-lane prerequisites pass.
Semantic-harness tick baselines are now hard threshold checks for the benchmark gate.
Microbench output stays report-only, runtime evidence stays proof-only, selected kernel archive metrics stay hard thresholds, and one stable total image byte metric now acts as a conservative hard upper-bound while other total image bytes remain observational.

## Runner Hardening

### Enforced
- locale pinned to C
- timezone pinned to UTC
- Python hash seed pinned to 0
- common host math thread pools capped at 1
- CPU affinity pinned to logical CPU 0 for microbench collection

### Best-effort only
- None

## Semantic Baselines

| Metric | Value | Threshold | Units | Source | Scenario | Gate |
| --- | ---: | --- | --- | --- | --- | --- |
| scheduler_wake_latency | 2 | [2, 2] | ticks | semantic-harness | task-delay-timeout | pass |
| ipc_handoff_latency | 0 | [0, 0] | ticks | semantic-harness | queue-send-to-blocked-recv | pass |
| timer_latency | 3 | [3, 3] | ticks | semantic-harness | one-shot-expiry | pass |
| timer_jitter | 0 | [0, 0] | ticks | semantic-harness | one-shot-expiry | pass |
| timer_periodic_jitter | 0 | [0, 0] | ticks | semantic-harness | periodic-rearm-stability | pass |
| timer_long_delay_boundary | 260 | [260, 260] | ticks | semantic-harness | timer-wheel-wrap-boundary | pass |

## Harness Cycle Microbenchmarks

These values use the `os_cycle_get()` harness backend, which maps to a host monotonic proxy under `OS_TEST_HARNESS`.
They are useful for relative regression tracking only and must not be read as real hardware cycle counts.

| Metric | Units | Source | Samples | Min | Median | p95 | Max | Stddev |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| queue_send_to_blocked_recv_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 0 | 1187 | 1188 | 1188 | 147 |
| mutex_unlock_to_waiter_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 0 | 1187 | 2374 | 2375 | 419 |
| sem_give_to_blocked_waiter_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 0 | 1187 | 1188 | 2374 | 209 |

## Runtime Evidence

| Metric | Value | Units | Source | Scenario | Artifact |
| --- | ---: | --- | --- | --- | --- |
| timer_runtime_callback_tick | 20 | ticks | supported-rv32-log | mnist-validation-runtime | `logs/mnist_validation_qemu.log` |
| preemption_resume_tick | 3 | ticks | supported-rv32-log | mnist-preemption-probe | `logs/mnist_preemption_probe_qemu.log` |

## Footprint Thresholds

| Image | Metric | Value | Threshold | Artifact | Gate |
| --- | --- | ---: | --- | --- | --- |
| mnist_validation | kernel_archive_bytes | 14705 | <= 14705 | `build/apps/mnist_app/mnist_validation_footprint.md` | pass |
| rrtos_be_u1000 | kernel_archive_bytes | 14169 | <= 14169 | `build-be_u1000/rrtos_be_u1000_footprint.md` | pass |
| rrtos_be_u1000 | total_bytes | 122401 | <= 122401 | `build-be_u1000/rrtos_be_u1000_footprint.md` | pass |

## Footprint Snapshots

| Image | Total Bytes | Kernel Archive Bytes | Total Gate | Archive Gate | Artifact |
| --- | ---: | ---: | --- | --- | --- |
| mnist_validation | 657575 | 14705 | report-only | pass | `build/apps/mnist_app/mnist_validation_footprint.md` |
| rrtos_be_u1000 | 122401 | 14169 | pass | pass | `build-be_u1000/rrtos_be_u1000_footprint.md` |

## Notes

- Hard threshold drift in any semantic-harness tick metric fails this benchmark gate immediately.
- `scheduler_wake_latency`, `ipc_handoff_latency`, `timer_latency`, `timer_jitter`, `timer_periodic_jitter`, and `timer_long_delay_boundary` all remain deterministic semantic-harness tick metrics.
- Microbench metrics keep correctness gating separate and now report `min`, `median`, `p95`, `max`, and `stddev` for each required handoff path.
- `timer_runtime_callback_tick` and `preemption_resume_tick` come from supported RV32 QEMU logs and remain proof, not thresholds.
- `kernel_archive_bytes` remains a hard upper-bound threshold for `mnist_validation` and `rrtos_be_u1000`.
- `rrtos_be_u1000.total_bytes` is now a hard upper-bound threshold because the current board image layout is the most stable total-image promotion candidate.
- `mnist_validation.total_bytes` remains collected for visibility only in this rollout phase.
- SMP remains experimental and is intentionally excluded from this report.
