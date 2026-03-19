# Kernel Microbench Report

This lane collects host-proxy microbench evidence only.
It is intentionally separate from semantic correctness suites and hard tick-baseline threshold checks.
The values below are not hardware cycles; they come from the `os_cycle_get()` host monotonic proxy under `OS_TEST_HARNESS`.

## Runner Hardening

### Enforced
- locale pinned to C
- timezone pinned to UTC
- Python hash seed pinned to 0
- common host math thread pools capped at 1
- CPU affinity pinned to logical CPU 0 for microbench collection

### Best-effort only
- None

## Microbench Metrics

| Metric | Units | Source | Samples | Min | Median | p95 | Max | Stddev |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| queue_send_to_blocked_recv_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 1187 | 1187 | 2375 | 2375 | 318 |
| mutex_unlock_to_waiter_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 0 | 1187 | 2374 | 2375 | 425 |
| sem_give_to_blocked_waiter_handoff | host-monotonic-ns | host-monotonic-raw-ns-proxy | 64 | 1187 | 1187 | 2374 | 2375 | 318 |

## Notes

- `sem_give_to_blocked_waiter_handoff` is required collection evidence in this lane.
- Microbench values remain report-only and do not participate in the hard benchmark gate.
