# BV Application Integration Plan

This note closes the current OpenSpec BV tasks by recording what is known, what is not known, and how core assignment stays blocked until the missing context is supplied.

## What BV means in the current repo

- There is no authoritative repo-owned expansion of the term `BV` in current project sources.
- The current board entrypoint is `apps/be_u1000_demo/main.c`, which contains:
  - a `control` task for boot/logging/control-plane flow
  - a `worker` task for board-side compute/worker flow
  - an unpinned `balance` task used only for experimental SMP validation
- For current planning, `BV` is therefore treated as an external application workload term that would attach to the board-side worker/compute path, not as an already-implemented repo feature.

## Current board-path mapping

| Concern | Current repo mapping | Confidence |
| --- | --- | --- |
| Control plane | `control_task_entry()` in `apps/be_u1000_demo/main.c` | high |
| Compute / worker placeholder | `worker_task_entry()` in `apps/be_u1000_demo/main.c` | medium |
| Shared board self-test / validation utility | `board_bus_self_check()` and the experimental `balance` task | high |
| Explicit BV business logic | not present in repo-owned code | high |

## Data flow and memory ownership

- Control-plane messages and scheduler actions originate on Core0.
- Worker-side execution currently uses the same RTOS image and TCM-backed memory layout as the rest of the board demo.
- Shared data assumptions remain those already documented in `docs/RRTOS_MULTICORE_BOUNDARIES.md` and `docs/BE_U1000_MULTICORE_NOTES.md`:
  - Core0/Core1 share the RTOS image and scheduler state.
  - Core2 is reserved/helper and not part of the first-stage SMP path.
  - TCMA/TCMB remain the primary execution/storage areas for the board lane, with shared SRAM reserved for explicit board-level sharing.

## Real-time constraints

- No repo-owned file currently defines authoritative BV latency, throughput, or jitter limits.
- The only current timing evidence is board bring-up and experimental multicore validation logs.
- Therefore BV real-time requirements remain an external-context dependency and cannot be safely inferred from the current demo app.

## Provisional core assignment rule

When a real BV workload is later mapped into this repo, use the following staged default unless that workload proves otherwise:

1. Core0 owns control, logging, interrupt-heavy board work, and coordination.
2. Core1 owns BV/AI/heavy worker execution.
3. Core2 remains reserved/helper until a separate helper-image or mailbox-backed need is approved.

## Blocking conditions

BV partitioning remains blocked until all of the following are provided:

- the exact expansion of `BV` in this project context
- the real code path or deliverable that implements it
- its memory ownership model
- its latency / deadline constraints
- any required core affinity or helper-core dependency

Until then, BV placement is recorded as an explicit blocked dependency rather than an inferred architecture choice.
