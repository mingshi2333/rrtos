# BE-U1000 Runtime Validation Matrix

This matrix records the staged runtime validation story for the `be_u1000` board path.

It elaborates board-stage evidence for `be_u1000`, but it does not replace the
normative support-status decisions in `docs/SUPPORTED_MATRIX.md`.

## Status labels

- `supported`: part of the default supported workflow
- `historical`: older staged SMP notes retained for context, but no longer exposed as maintained pixi tasks
- `unresolved`: explicit gap remains

## Validation stages

| Stage | What it proves | Command | Status | Evidence |
| --- | --- | --- | --- | --- |
| `S0` | Single-core fallback behavior with `OS_SMP_EN=OFF`, board self-test, and non-destructive QSPI window read | `pixi run -e be-u1000 validate-selftest-sim` | supported | `logs/be_u1000_selftest_runtime.md`, `logs/be_u1000_boot_sim.log` |
| `S1` | Historical SMP build wiring note | maintained pixi task removed | historical | `logs/be_u1000_smp_probe.md`, `logs/be_u1000_smp_probe.log` |
| `S2` | Historical secondary-core boot / IPI note | maintained pixi task removed | historical | `logs/be_u1000_smp_runtime_affinity_probe.md`, `logs/be_u1000_smp_boot.log` |
| `S3` | Historical task-affinity note for `control -> Core0` and `worker -> Core1` | maintained pixi task removed | historical | `logs/be_u1000_smp_boot.log` affinity markers |
| `S4` | Historical scheduler-balancing smoke note for an unpinned `balance` task | maintained pixi task removed | historical | `logs/be_u1000_smp_runtime_balance_probe.md`, `logs/be_u1000_smp_balance.log` |
| `S5` | Historical combined staged runtime note | maintained pixi task removed | historical | combined logs above |

## Current interpretation

- `S0` is the supported fallback behavior: both demo tasks share a single scheduler lane (`OS_CFG_CPU_COUNT=1` with `OS_SMP_EN=OFF`) and the board self-test passes.
- `S1` through `S5` remain historical notes because they relied on approximate Renode SMP probing and are not part of the declared supported matrix or maintained pixi task surface.
- Core2 remains reserved/helper only; no current stage promotes it into generic SMP.
- `boards/be_u1000/board_config.h` owns immutable BE-U1000 facts, while the top-level build selects the active lane policy exposed through `OS_CFG_*` values.
- Promotion of any later stage requires the support registry, command surface, and blocking CI to be updated in the same change.

## Remaining unresolved items

- Real-silicon validation is still unresolved.
- The current scheduler-balancing proof is a smoke check based on an unpinned task being observed on both SMP cores, not a full migration or throughput benchmark.
- Any promotion from experimental to supported requires updating `docs/SUPPORTED_MATRIX.md` and the acceptance workflow in the same change.
