# EVU-BA-2.3 Phase-One Validation Plan

This plan defines phase one for the current `EVU-BA-2.3` board lane. It assumes the repo is already on the correct board family and focuses on finishing board support instead of inventing a new migration target.

## Phase boundary

- Phase one: single-core boot, board banner, console, timer sanity, then closure of the currently claimed EVU-BA-2.3 peripherals
- Phase two: multicore promotion, mailbox/IPI runtime validation, and lower-priority peripheral families

## Minimum boot gate before peripheral signoff

The EVU-BA-2.3 lane should not claim further board completion until all of the following hold:

1. `CONFIG_BOARD=be_u1000` builds and links in the chosen memory model
2. startup reaches `hal_board_init()` and prints the EVU-BA board banner
3. `UART0` emits a stable boot log
4. the IRQ model is reported as expected for the board lane
5. CLINT time proves monotonic progress and at least one delayed task prints successfully

## Phase-one peripheral status and pass criteria

| Peripheral | Current repo status | Pass criterion for EVU-BA-2.3 phase one | Evidence required |
| --- | --- | --- | --- |
| `UART0` | complete enough | boot banner and repeatable console output on `PA6/PA7` | boot log with banner and markers |
| `Timer` | complete enough | CLINT monotonic check plus delayed task progress | timer sanity log and task output |
| `GPIO` | complete enough | user LED `PC0` toggles and user button `PC13` reads | self-test log naming both signals |
| `I2C0` | complete enough | controller init succeeds on `PA4/PA5` and performs at least one stable transaction or approved probe | log naming bus speed and result |
| `SPI1` | complete enough | controller init succeeds on `PA8..PA11` and completes a known transfer | transfer log with instance and result |
| `QSPI1 / Flash` | partial | keep the current safe read path plus simulated flash identification, then add write/erase only if needed for board claims | log naming controller, memory window, flash ID or geometry, and test mode |
| `CANFD0/1` | partial | keep the current internal loopback plus simulated interrupt-driven proof, then add bus-level or silicon-backed evidence before calling the lane complete | log naming controller, mode, bitrate assumptions, FIFO/status path, interrupt result, and frame result |

## Explicit non-goals for phase one

- claiming multicore support as supported
- claiming runtime USB, ADC, PWM, I2S, WDT, or DMA support unless their drivers and validation are actually added
- treating Renode-only evidence as final EVU-BA-2.3 signoff for every peripheral

## Evidence policy

Each phase-one item needs repo-visible evidence, not only code presence:

- build artifact or command
- boot or self-test log
- a clear pass/fail statement tied to the board lane

For partial items such as `QSPI1/Flash` and `CANFD`, the evidence must prove more than pinmux application.

## Phase-two follow-up

Phase two begins only after the single-core EVU-BA-2.3 board lane is clearly closed and covers:

- non-primary hart startup behavior
- mailbox and IPI routing
- affinity and multicore runtime validation
- any lower-priority peripheral families that are truly needed

## Current blockers

- The repo now has a minimal read-only `hal_flash` path plus simulated flash-identification metadata, but not yet a fuller silicon-backed flash-device closure.
- The repo now has a minimal `hal_canfd` plus simulated internal loopback and CLIC IRQ-path signoff, but not yet silicon-backed or bus-level CANFD signoff.
- Current high-confidence validation is stronger in simulation than on real hardware.
