# BE-U1000 SDK Comparison

This note compares the SDK in `/home/mingshi/Project/PF/mcu-sdk-linux_2.1.1_20260119` with the current `rrtos` board support and records the supplements added in this repo.

## Current comparison

| SDK capability | SDK evidence | `rrtos` before | `rrtos` now |
| --- | --- | --- | --- |
| CLIC interrupt numbering and Core2 system-side TCM windows | `Drivers/BASIS/Device/BMCU/Include/be_u1000.h` | `board_config.h` used a partially shifted IRQ table and did not expose the system-side Core2 TCMA window | Audited `board_config.h` against the SDK, corrected the IRQ numbering, and added `BE_U1000_C2_TCMA_SYS_BASE` / `BE_U1000_C2_TCMB_SYS_BASE` |
| EVU-BA board pins for UART0 / user LED / user button / header I2C+SPI / QSPI1 | `BSP/EVU_BA_2_1/bsp.h`, `BSP/EVU_BA_2_1/bsp.c`, EVU-BA-2.3 tech description tail schematic pages | Board init only knew peripheral base addresses; it did not apply the BSP pinmux defaults, and the diagnostic GPIO pointed at the user button instead of the LED | `hal_board_init()` now applies EVU-BA UART0 and user LED/button pinmux defaults, also configures header `I2C0` (`PA4/PA5`), header `SPI1` (`PA8..PA11`), and board `QSPI1` (`PB0..PB5`); the diagnostic GPIO now targets the user LED on `PC0` |
| Systematic board pinmux groups | EVU-BA-2.3 tail schematic pages, SDK CANFD examples | Board pinmux knowledge lived in ad-hoc board init code only | Added `hal_board_apply_pinmux_group()` and a documented EVU-BA connector map in `docs/EVU_BA_PINMAP.md` |
| Board bring-up self-test demo | EVU-BA connector map + current HAL driver surfaces | Demo only checked generic GPIO/SPI/I2C init using a single diag tuple | `apps/be_u1000_demo/main.c` now runs a richer EVU-BA self-test covering LED/button GPIO, header `I2C0`, header `SPI1`, and readiness reporting for `QSPI1`, `CANFD0`, and `CANFD1` |
| Renode self-test coverage for SPI/QSPI | `scripts/simulation/be_u1000.repl`, `scripts/be_u1000/run_smp_runtime.py` | Simulation had no `SPI1` controller behavior and no `QSPI1` read validation | Added a minimal `SPI1` Python stub plus preloaded `QSPI1` signature words so simulation can validate polled SPI transfer and non-destructive QSPI window reads |
| Core1/Core2 explicit start/reset control | `Drivers/HAL/Inc/bmcu_cru.h`, `Drivers/BASIS/Device/BMCU/Include/be_u1000.h` | Startup assembly existed, but there was no reusable board HAL API for core control | Added `hal_corectrl_*` board HAL wrappers |
| Core2 helper workflow | `Projects/HAL_examples/GPIO/GPIO_LEDBlink_C2/README.md` | Core2 was documented as reserved, but there was no helper abstraction for image copy/start | Added `hal_corectrl_core2_copy_image()` and explicit Core2 control helpers |
| Mailbox system-side helpers | `Drivers/HAL/Inc/bmcu_mb.h` | Mailbox IRQ/base constants existed, but no board HAL wrapper was exposed | Added `hal_mailbox_*` helpers for system-side DATA0/DATA1 access |
| Core0/Core1 SMP affinity demo | `FreeRTOS ... RISC-V_Baikal_SMPDemo_GCC/main.c` | No explicit affinity-pinned board demo | Experimental demo now validates `control -> Core0`, `worker -> Core1` |

## Added board HAL surfaces

- `hal/include/hal_corectrl.h`
- `hal/src/be_u1000/hal_corectrl_be_u1000.c`
- `hal/include/hal_mailbox.h`
- `hal/src/be_u1000/hal_mailbox_be_u1000.c`

## Added board config audit points

- `boards/be_u1000/board_config.h` now follows the SDK CLIC numbering instead of the earlier approximate table.
- `scripts/be_u1000/validate_interrupt_consistency.py` can now validate both the Renode model and the vendor SDK header in one pass.
- `pixi run -e be-u1000 validate-irq-map` now checks the repo board config against the SDK header as well as the Renode platform file.
- `docs/EVU_BA_PINMAP.md` records the connector/header recovery from the EVU-BA-2.3 tail schematic pages and notes which tables remain partial.

## Why these supplements matter

- They move SDK-only board knowledge into reusable `rrtos` HAL layers instead of leaving it trapped in startup code or ad-hoc examples.
- They prepare the next step after `Core0/Core1` SMP: a `Core2` helper image started and fed by `Core0` through mailbox-style communication.

## Remaining gaps against the SDK

- `rrtos` still does not run a true separate `Core2` helper image end-to-end.
- Current `Renode` modeling is still approximate and not a full substitute for silicon validation.
- Connector-level `XP8` / `XS3` / `XP9` ordering is still only partially recovered from text extraction, but UART0, user LED/button, header `I2C0`, header `SPI1`, `QSPI1`, and optional CANFD pinmux groups are now modeled in board support.
