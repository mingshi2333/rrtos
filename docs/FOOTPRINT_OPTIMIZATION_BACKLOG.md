# Footprint Optimization Backlog

This backlog ranks current footprint work from low-risk, evidence-backed items to higher-risk structural changes.

## Baseline policy

- Supported-lane evidence comes from `build/apps/mnist_app/mnist_validation_footprint.md`, `build/apps/mnist_app/mnist_validation_ai_footprint.md`, and `build-be_u1000/rrtos_be_u1000_footprint.md`.
- `pixi run -e rv32 validate-supported-rv32` and `pixi run -e be-u1000 validate-supported` now regenerate footprint reports as part of the supported workflow.
- Structural or behavior-changing optimization stays deferred until the supported build lanes and experimental runtime checks remain green after each change.

## Low-risk items first

| Priority | Item | Expected gain | Risk | Dependency |
| --- | --- | --- | --- | --- |
| `L0` | Verify dead-code elimination remains active in supported `MinSizeRel` lanes and keep map/size artifacts next to the built image | Prevents accidental size regressions; preserves current text savings without behavior change | low | current `report-footprint` workflow and linker map generation |
| `L1` | Gate optional diagnostics and board-only self-test verbosity behind build flags for release images | small-to-moderate text and log reduction, especially on `be_u1000` | low | release/profile flag definition and one release acceptance run |
| `L2` | Keep historical AI/demo reports isolated from supported-path docs and automation | engineering-efficiency gain, lower audit noise, fewer false optimization leads | low | current supported-matrix doc set |
| `L3` | Keep binary/report cleanup in the build workflow so supported evidence is always reproducible | avoids stale evidence, improves regression confidence | low | `pixi` task dependencies and footprint report script |

## Medium-risk items next

| Priority | Item | Expected gain | Risk | Dependency |
| --- | --- | --- | --- | --- |
| `M0` | Right-size `be_u1000` TCMB `.heap` and `.stack` reservations from workload evidence instead of static worst-case defaults | high RAM recovery on the 96 KiB TCMB lane | medium | repeatable board acceptance run plus stack/heap headroom observation |
| `M1` | Reduce scheduler table dimensions such as `OS_CFG_TASK_MAX`, `OS_CFG_PRIO_MAX`, and `OS_CFG_CPU_MAX` for board-focused builds | moderate `.bss` reduction | medium | workload-informed ceilings for supported and experimental lanes |
| `M2` | Keep AI footprint attribution current for model constants, IREE runtime text, RAM reservations, and final image fit | prevents optimizing the wrong layer; makes BE-U1000 AI fit decisions evidence-backed | medium | `report-ai-footprint` and canonical AI regression checks |
| `M3` | Isolate historical and optional app paths from default build surfaces | moderate build/link reduction and lower maintenance cost | medium | explicit agreement on what remains supported vs archival |
| `M4` | Audit allocator hot spots and long-lived board buffers before changing allocator behavior | targeted RAM recovery with bounded churn | medium | current artifact baseline and stress validation |

## Deferred structural work

| Priority | Item | Why deferred | Unlock condition |
| --- | --- | --- | --- |
| `D0` | Scheduler algorithm rewrites or deeper SMP load-balancing changes | behavior-changing and not justified until current experimental SMP telemetry is stable | supported lanes stay green and experimental multicore validation remains reproducible |
| `D1` | Full HAL ABI replacement to remove the remaining VM stack/ref/native-module symbols | deeper than the supported registry path and needs generated import shim replacement | canonical AI path stays green |
| `D2` | Broad linker/memory-layout redesign across boards | high blast radius for little immediate evidence-backed return | supported-lane footprint regression history and per-board capacity data |

## Current AI Size Facts

- `mnist_validation` image total: 661791 bytes.
- Generated model wrapper linked contribution: 431820 bytes.
- Generated dispatch object linked contribution: 4739 bytes.
- RRTOS AI registry runtime linked contribution: 3520 bytes.
- IREE runtime linked contribution: 112084 bytes.
- Dominant model constant: `st_mnist_28__const` at 410176 bytes.

These facts mean the next size decisions should first separate model-weight cost from runtime/framework cost. Removing optional model experiments or stale docs does not change the supported image size unless linked objects or generated constants change.

## Execution order

1. Keep `L0` through `L3` green in the default validation workflow.
2. Use those stable artifacts to evaluate `M0` through `M4` one at a time.
3. Do not start `D0` through `D2` until the measured supported and experimental checks remain reproducible.
