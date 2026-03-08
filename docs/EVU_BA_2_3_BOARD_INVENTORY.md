# EVU-BA-2.3 Board Inventory

This note corrects the earlier `BD2` framing. The current `be_u1000` board path in this repo already targets the `BE-U1000` SoC on the `EVU-BA` evaluation board family and is materially aligned with the `EVU-BA-2.3` technical description.

## Alignment summary

- `boards/be_u1000/board_config.h` declares the board variant as `EVU-BA` and encodes the same board-facing pins used in the repo self-test path.
- `hal/src/hal_board.c` reports the board name as `BE-U1000/EVU-BA` and applies EVU-BA pinmux groups.
- `boards/be_u1000/DFU_FLASHING_GUIDE.md` explicitly documents `EVU-BA-2.3` boot straps and DFU setup.
- `docs/EVU_BA_PINMAP.md` was recovered from the `EVU-BA-2.3` technical description and matches the active board pinmux groups for `UART0`, `I2C0`, `SPI1`, `QSPI1`, user LED, and user button.

## Repo evidence anchors

- `boards/be_u1000/board_config.h`
- `hal/src/hal_board.c`
- `apps/be_u1000_demo/main.c`
- `docs/EVU_BA_PINMAP.md`
- `boards/be_u1000/DFU_FLASHING_GUIDE.md`

## Inventory classification

| Area | Current repo anchor | EVU-BA-2.3 status | Why |
| --- | --- | --- | --- |
| Board identity | `boards/be_u1000/board_config.h`, `hal/src/hal_board.c`, `boards/be_u1000/DFU_FLASHING_GUIDE.md` | aligned | The repo is already EVU-BA-shaped; the only mismatch is that the code names the board generically `EVU-BA` instead of revision-locking to `EVU-BA-2.3`. |
| Startup and linker flow | `boards/be_u1000/startup_be_u1000.S`, `boards/be_u1000/link_be_u1000_tcm.ld`, `boards/be_u1000/link_be_u1000_flash.ld` | usable | The board lane already supports TCM and flash memory models for the current EVU-BA path. |
| Build lane | `boards/be_u1000/CMakeLists.txt`, `CMakeLists.txt`, `pixi.toml` | usable | `CONFIG_BOARD=be_u1000` is the current EVU-BA lane, with supported self-test and documented flash builds. |
| Console and boot markers | `hal/src/hal_board.c`, `apps/be_u1000_demo/main.c` | complete enough for board bring-up | Banner, IRQ model print, timer sanity, and UART0 console are already wired into the demo path. |
| LED and button | `boards/be_u1000/board_config.h`, `apps/be_u1000_demo/main.c`, `docs/EVU_BA_PINMAP.md` | complete enough | `PC0` user LED and `PC13` user button match the EVU-BA-2.3 technical description and current self-test. |
| Header I2C0 | `boards/be_u1000/board_config.h`, `hal/src/hal_board.c`, `apps/be_u1000_demo/main.c` | complete enough | `PA4/PA5` matches the current EVU-BA connector mapping and controller init path. |
| Header SPI1 | `boards/be_u1000/board_config.h`, `hal/src/hal_board.c`, `apps/be_u1000_demo/main.c` | complete enough | `PA8..PA11` matches the current EVU-BA connector mapping and controller transfer smoke test. |
| QSPI1 / flash path | `boards/be_u1000/board_config.h`, `apps/be_u1000_demo/main.c`, `boards/be_u1000/DFU_FLASHING_GUIDE.md`, `hal/include/hal_flash.h`, `hal/src/be_u1000/hal_flash_be_u1000.c` | partial | The board lane now has a minimal read-only `hal_flash` path, boot-log validation, and simulated flash ID/geometry reporting, but it still lacks silicon-backed flash identification confidence plus any justified write/erase acceptance rule. |
| CANFD0 / CANFD1 | `boards/be_u1000/board_config.h`, `hal/src/hal_board.c`, `apps/be_u1000_demo/main.c`, `docs/EVU_BA_PINMAP.md`, `hal/include/hal_canfd.h`, `hal/src/be_u1000/hal_canfd_be_u1000.c`, `scripts/simulation/be_u1000.repl` | partial | The repo now has a minimal `hal_canfd` interface plus Renode-backed internal loopback and CLIC IRQ-path validation with FIFO/status markers, but it still lacks silicon-backed proof and bus/transceiver validation. |
| USB OTG / USB boot | `boards/be_u1000/DFU_FLASHING_GUIDE.md`, EVU-BA-2.3 technical description | partial | The board and docs support USB boot and DFU, but the repo does not expose a board-level USB runtime driver for applications. |
| ADC / PWM / I2S / WDT / DMA | `boards/be_u1000/board_config.h` | missing at HAL level | Base addresses and IRQs are defined, but there are no matching HAL drivers or validation flows in the current repo. |
| Multicore | `boards/be_u1000/startup_be_u1000.S`, `multicore/`, `docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md` | experimental | Core topology and mailbox helpers exist, but supported status is still single-core fallback only. |
| Validation maturity | `docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md`, `docs/BE_U1000_READINESS_ACCEPTANCE.md`, `logs/` | partial | Current evidence is strong for Renode-backed bring-up, not for silicon-backed signoff of every peripheral. |

## What this means

- The repo does not need a new board target to start EVU-BA-2.3 work; the active `be_u1000` lane is already that board family in practice.
- The real issue is not board identity but board completion: `Flash/QSPI`, `CANFD`, broader peripheral coverage, and stronger real-board validation are still unfinished.

## Confidence and limits

- High confidence: `UART0`, `I2C0`, `SPI1`, `QSPI1`, user LED, and user button match the current EVU-BA-2.3 documentation and repo pinmux groups.
- Medium confidence: the repo is clearly EVU-BA-2.3-shaped, but code-level naming still uses the generic `EVU-BA` label instead of the exact revision.
- Lower confidence: `CANFD0` and `CANFD1` mux groups are modeled from SDK examples and board recovery notes, and current closure is still simulation-heavy rather than silicon-backed.
