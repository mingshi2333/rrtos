# EVU-BA-2.3 RTOS Bring-Up Baseline

This baseline compares the current repo against mainstream RTOS expectations for an already-selected evaluation board, `EVU-BA-2.3`, using the `BE-U1000` SoC. The goal is not to copy Zephyr or NuttX exactly; it is to identify what remains unfinished for a credible EVU-BA-2.3 board-support story.

## Comparison table

| Capability | Current repo state | EVU-BA-2.3 completion need | Priority |
| --- | --- | --- | --- |
| Kernel primitives | scheduler, tasks, delays, IPC, traps, and timer tick path are present | reuse; no board-driven kernel rewrite needed | low |
| Board identity | current board lane is already EVU-BA-shaped and partly revision-backed by docs | clean up docs and planning so the target is explicitly `EVU-BA-2.3` rather than a hypothetical new board | medium |
| Startup and linker closure | dedicated startup plus `tcm` and `flash` linker flows exist | keep and validate; no new board lane required first | low |
| Console path | `UART0` boot log and board banner already exist | keep as the boot gate for all further board signoff | low |
| GPIO / I2C / SPI | controller drivers and board self-test path already exist | convert current smoke tests into stable acceptance evidence | medium |
| QSPI / flash | flash-linked build exists; demo now uses a minimal read-only `hal_flash` path, validates the mapped window, and reports simulated flash ID/geometry | extend toward silicon-backed flash identification and any justified write/erase acceptance rule | high |
| CANFD | pinmux, minimal `hal_canfd`, simulated internal loopback, and simulated CLIC IRQ-path validation now exist | extend toward silicon-backed controller behavior, bus-level proof, and board validation | high |
| USB runtime support | USB boot and DFU are documented for the board | decide whether runtime USB support is required or stays out of scope | medium |
| ADC / PWM / I2S / WDT / DMA | register/IRQ constants exist only in board config | implement only if needed for current EVU-BA-2.3 goals | medium |
| Debug and flashing workflow | DFU path is documented; automation is limited and JTAG workflows are not operationalized in repo | add stronger JTAG/OpenOCD/GDB or equivalent workflow if needed for board development | medium |
| Validation discipline | current supported lane is simulation-backed and board-specific docs exist | add stronger silicon-backed evidence for EVU-BA-2.3 closure | high |
| Multicore | experimental only | keep out of phase-one board completion | medium |

## Mainstream RTOS expectation lens

### 1. Kernel layer

Expected:

- kernel primitives are already stable before board completion becomes the main focus

Current repo:

- this expectation is largely met for the EVU-BA lane

Implication:

- the biggest remaining work is board and subsystem closure, not scheduler redesign

### 2. Board-support layer

Expected:

- board lane has a stable boot path, console path, and per-peripheral validation rules

Current repo:

- boot path and console path are present
- validation is strong for `UART0`, timer sanity, GPIO, `I2C0`, and `SPI1`
- `Flash/QSPI` and `CANFD` are still only partially closed, but both now have dedicated HAL-facing validation slices in the repo

Implication:

- EVU-BA-2.3 completion should prioritize the unfinished peripherals first

### 3. Peripheral coverage

Expected:

- a credible board port closes the peripherals that the board lane claims to support

Current repo:

- complete enough now: `UART0`, CLINT timer sanity, LED/button GPIO, `I2C0`, `SPI1`
- partial: `QSPI1/Flash`, `CANFD` (simulation-backed loopback plus IRQ-path proof only)
- missing HAL surface: `USB`, `ADC`, `PWM`, `I2S`, `WDT`, `DMA`

Implication:

- do not overclaim broad EVU-BA-2.3 support until the partial and missing items are explicitly addressed or scoped out

### 4. Validation maturity

Expected:

- bring-up claims are backed by repeatable artifacts from the real board, not only simulation

Current repo:

- Renode and log-based validation are good for development
- real-silicon signoff is still incomplete

Implication:

- validation maturity is a first-order backlog item, not just polish

## Immediate conclusion

- The repo is already on the correct board family: `EVU-BA-2.3` is the right current target.
- The highest-priority missing work is `Flash/QSPI`, `CANFD`, and stronger EVU-BA-2.3 evidence.
- Broader peripheral families and multicore should come after those board-completion gaps, unless your product need says otherwise.
