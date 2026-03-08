# BE-U1000 Runtime Validation Matrix

This matrix records the staged runtime validation story for the `be_u1000` board path.

It elaborates board-stage evidence for `be_u1000`, but it does not replace the
normative support-status decisions in `docs/SUPPORTED_MATRIX.md`.

## Status labels

- `supported`: part of the default supported workflow
- `experimental`: automated and evidence-backed, but not promoted to the supported matrix
- `unresolved`: explicit gap remains

## Validation stages

| Stage | What it proves | Command | Status | Evidence |
| --- | --- | --- | --- | --- |
| `S0` | Single-core fallback behavior with `OS_SMP_EN=OFF`, board self-test, and non-destructive QSPI window read | `pixi run -e be-u1000 validate-selftest-sim` | supported | `logs/be_u1000_selftest_runtime.md`, `logs/be_u1000_boot_sim.log` |
| `S1` | Experimental SMP build wiring closes successfully | `pixi run -e be-u1000 probe-smp-build` | experimental | `logs/be_u1000_smp_probe.md`, `logs/be_u1000_smp_probe.log` |
| `S2` | Secondary-core boot, online-count, and IPI delivery | `pixi run -e be-u1000 validate-smp-affinity-experimental` | experimental | `logs/be_u1000_smp_runtime_affinity_probe.md`, `logs/be_u1000_smp_boot.log` |
| `S3` | Task affinity for `control -> Core0` and `worker -> Core1` | `pixi run -e be-u1000 validate-smp-affinity-experimental` | experimental | `logs/be_u1000_smp_boot.log` affinity markers |
| `S4` | Scheduler balancing smoke check via an unpinned `balance` task observed on both Core0 and Core1 | `pixi run -e be-u1000 validate-smp-balance-experimental` | experimental | `logs/be_u1000_smp_runtime_balance_probe.md`, `logs/be_u1000_smp_balance.log` |
| `S5` | Combined staged runtime proof for fallback + SMP affinity + scheduler-balancing smoke checks | `pixi run -e be-u1000 validate-runtime-stages-experimental` | experimental | combined logs above |

## Current interpretation

- `S0` is the supported fallback behavior: both demo tasks share a single scheduler lane (`OS_CFG_CPU_COUNT=1` with `OS_SMP_EN=OFF`) and the board self-test passes.
- `S1` through `S5` remain experimental because they rely on an approximate Renode model and are not part of the declared supported matrix.
- Core2 remains reserved/helper only; no current stage promotes it into generic SMP.
- Promotion of any later stage requires the support registry, command surface, and blocking CI to be updated in the same change.

## Remaining unresolved items

- Real-silicon validation is still unresolved.
- The current scheduler-balancing proof is a smoke check based on an unpinned task being observed on both SMP cores, not a full migration or throughput benchmark.
- Any promotion from experimental to supported requires updating `docs/SUPPORTED_MATRIX.md` and the acceptance workflow in the same change.
