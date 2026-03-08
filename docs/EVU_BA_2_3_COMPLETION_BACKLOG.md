# EVU-BA-2.3 Completion Backlog

This backlog assumes the current `be_u1000` board lane is already the `EVU-BA-2.3` target in practice. The purpose here is to rank what is still unfinished.

## Priority summary

| Priority | Area | Current state | Why it matters next |
| --- | --- | --- | --- |
| `P1` | `QSPI1 / Flash` closure | partial | The repo can boot flash-linked images and now has a minimal read-only flash HAL plus simulated flash-identification semantics, but it still lacks silicon-backed flash evidence and any justified write/erase closure. |
| `P1` | `CANFD` closure | partial | The repo now has minimal HAL-backed internal loopback plus simulated CLIC IRQ-path validation with FIFO/status logging, but it still lacks silicon-backed proof and transceiver/bus validation. |
| `P1` | EVU-BA-2.3 silicon evidence | partial | Current board acceptance is still simulation-heavy, so the strongest board claims remain underpowered. |
| `P2` | Board identity cleanup | partial | The active lane is EVU-BA-2.3-shaped, but naming and docs still mix generic `EVU-BA` and exact revision language. |
| `P2` | Debug workflow beyond DFU | partial | DFU is documented, but stronger JTAG/debug automation is still missing from the repo workflow. |
| `P2` | Multicore evidence | experimental | Secondary-hart and SMP work exists but should stay behind single-core board completion. |
| `P3` | USB runtime support | missing | USB boot exists, but runtime USB support is not exposed as an application-ready board capability. |
| `P3` | ADC / PWM / I2S / WDT / DMA | missing | The board config exposes these blocks, but the repo does not yet implement or validate them. |
| `P4` | Generic RTOS debt | mixed | Items like `os_event_wait`, stubbed syscalls, and broader platform services are real gaps, but they are not the first blockers for EVU-BA-2.3 board completion. |

## Detailed backlog

### P1 - Finish the board lane you already have

1. `QSPI1 / Flash`
   - current evidence: `apps/be_u1000_demo/main.c` uses `hal_flash` for a safe mapped-window read plus simulated JEDEC/page/sector/capacity identification, and `boards/be_u1000/DFU_FLASHING_GUIDE.md` documents flash-linked images
   - missing: silicon-backed flash identification confidence and any justified write/erase acceptance rule

2. `CANFD`
   - current evidence: `hal_board_apply_pinmux_group()` supports `CANFD0/1` groups, the demo runs HAL-backed internal loopback plus simulated CLIC IRQ delivery, and the boot-log checker validates FIFO/status/frame and IRQ markers
   - missing: silicon-backed controller behavior and transceiver or bus-level proof

3. real-board validation
   - current evidence: `docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md`, `docs/BE_U1000_READINESS_ACCEPTANCE.md`, and `logs/` are strong for Renode-backed validation
   - missing: silicon-backed proof for each claimed EVU-BA-2.3 peripheral

### P2 - Make the board lane easier to trust and use

1. board naming cleanup
   - make the docs and planning consistently say `EVU-BA-2.3` when the evidence is revision-specific

2. debug workflow
   - keep DFU, but add stronger JTAG/OpenOCD/GDB or equivalent workflows if daily board work needs them

3. multicore safety and evidence
   - keep SMP experimental until single-core completion is closed and non-primary hart behavior is better evidenced

### P3 - Expand only if the board goal requires it

1. runtime USB
2. ADC
3. PWM / PWMA / PWMG
4. I2S
5. WDT
6. DMA

These all appear in `boards/be_u1000/board_config.h`, but they do not have matching board-ready HAL and validation flows in the current repo.

### P4 - Broader RTOS/platform debt

1. `os_event_wait` is declared in `kernel/include/os_kernel.h` but not implemented in the current IPC path.
2. several picolibc syscalls are still bare-metal stubs.
3. networking, filesystem, and broader middleware remain intentionally thin.

These are real gaps, but they should not displace `Flash`, `CANFD`, and board validation if the immediate goal is EVU-BA-2.3 board completion.

## Recommended execution order

1. keep the existing EVU-BA-2.3 single-core lane stable
2. close `QSPI1 / Flash`
3. close `CANFD`
4. capture silicon-backed EVU-BA-2.3 evidence
5. decide whether additional peripheral families are actually needed
6. revisit multicore promotion only after the single-core board lane is solid
