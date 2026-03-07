# RRTOS Multicore Boundaries

## Responsibility split

```text
                +--------------------------------------+
                |              Applications            |
                | control tasks / BV / AI / workers    |
                +-------------------+------------------+
                                    |
                                    v
                +--------------------------------------+
                |          kernel/ (portable policy)   |
                | - task priority/state                |
                | - CPU affinity                       |
                | - ready queue selection              |
                | - reschedule / migration policy      |
                +-------------------+------------------+
                                    |
                                    v
                +--------------------------------------+
                |        arch/riscv/ (CPU mechanism)   |
                | - hart id                            |
                | - trap / context switch              |
                | - timer / software interrupt hooks   |
                +-------------------+------------------+
                                    |
                                    v
                +--------------------------------------+
                |      boards/be_u1000/ + hal/         |
                | - which cores join RTOS              |
                | - Core1/Core2 reset vectors          |
                | - CLINT/CLIC wiring                  |
                | - mailbox / memory map / board facts |
                +--------------------------------------+
```

## What belongs where

### kernel/

- Owns scheduling policy.
- Decides which ready task can run on which CPU.
- Enforces `os_task_set_affinity()`.
- Handles per-CPU idle selection and reschedule decisions.

Relevant files:

- `kernel/src/os_sched.c`
- `kernel/include/os_kernel.h`
- `kernel/include/os_types.h`

### arch/riscv/

- Owns CPU-level mechanisms shared by RISC-V boards.
- Provides `os_cpu_id()` and trap/timer entry points.
- Bridges scheduler decisions to CSR/interrupt behavior.

Relevant files:

- `arch/riscv/include/riscv_csr.h`
- `arch/riscv/src/trap.c`
- `arch/riscv/src/context.S`

### boards/be_u1000/ and hal/

- Own which cores exist and how they are started.
- Define whether a core participates in RTOS SMP or stays reserved.
- Own board-specific interrupt routing, reset control, mailbox, and linker layout.

Relevant files:

- `boards/be_u1000/board_config.h`
- `boards/be_u1000/startup_be_u1000.S`
- `hal/include/hal_clint.h`
- `hal/src/hal_clint.c`

## Recommended BE-U1000 model

```text
Core0 -> system core
  - boot
  - board init
  - drivers / interrupt-heavy work
  - control plane / logging

Core1 -> worker core
  - BV / AI / heavy compute
  - data-plane workers

Core2 -> reserved or AMP-like helper
  - mailbox service
  - health monitor / tiny helper image
  - not part of first-stage generic SMP scheduler
```

## Why this split matters

- Scheduling policy must stay portable across boards, so it belongs in `kernel/`.
- Core startup and hardware ownership are board facts, so they belong in `boards/` and `hal/`.
- `BE-U1000` is best treated as `Core0/Core1 SMP + Core2 reserved/helper`, not immediate symmetric 3-core SMP.

## Current implementation status

- `kernel/src/os_sched.c` now performs affinity-aware ready task selection for SMP builds.
- `boards/be_u1000/` still decides that `Core0/Core1` are the RTOS main cores and `Core2` remains reserved for now.
- Runtime SMP validation is still experimental and not yet promoted to supported status.
