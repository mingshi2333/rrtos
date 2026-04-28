# BE-U1000 Multicore Notes

## Official facts we are using

- BE-U1000 / Baikal-U is a 3-core RISC-V MCU.
- Public Baikal material describes it as `2 x CloudBEAR BR-350 + 1 x CloudBEAR BM-310` at up to 200 MHz.
- Public boot documentation indicates BootROM starts from Core0.

## Repo assumptions

- `boards/be_u1000/board_config.h` treats Core0/Core1 as the main RTOS cores.
- Core2 is currently treated as reserved instead of being placed into the first SMP bring-up path.
- Current simulation models only Core0/Core1 for the experimental SMP path.

## Current implementation strategy

- Stage 1: prove deterministic dual-core bring-up on Core0/Core1.
- Stage 2: verify IPI, scheduler start, and shared-memory assumptions in simulation.
- Stage 3: keep Core2 reserved until stronger silicon documentation or application demand justifies using it.

## Current status

- Supported lanes still run with `OS_SMP_EN=OFF`.
- With `OS_SMP_EN=OFF`, nonzero harts halt in BE-U1000 startup instead of entering the experimental secondary wait path.
- The BE-U1000 linker scripts reserve an explicit two-stack startup bank for the main cluster: hart1 uses the lower 4 KiB stack and hart0 uses the upper 4 KiB stack. These symbols are for experimental SMP bring-up and do not promote SMP into the supported lane.
- Experimental `be_u1000` SMP build wiring remains an internal bring-up topic behind `OS_SMP_EN=ON`, but it is no longer exposed as a maintained pixi task surface.
- Runtime validation remains non-supported and should not be treated as supported status.
- Historical Renode SMP probing logs still document secondary-core online, IPI reschedule, and task-affinity markers for `control -> Core0` and `worker -> Core1`; see `logs/be_u1000_smp_runtime_affinity_probe.md` and `logs/be_u1000_smp_boot.log`.
- Renode still reports several unmapped peripheral warnings during board bring-up, so this remains experimental evidence rather than supported-matrix proof.
- `boards/be_u1000/board_config.h` is now re-audited against the vendor SDK for CLIC numbering and Core2 system-side TCM windows, reducing one of the previously noted board-definition gaps.
- `hal/src/hal_board.c` now follows EVU-BA board defaults for UART0 (`PA6/PA7`), user LED/button (`PC0` / `PC13`), and the board header `I2C0` / `SPI1` pins checked against the EVU-BA-2.3 instruction.
- `docs/EVU_BA_PINMAP.md` now captures the official EVU-BA-2.3 connector mapping, and `hal_board_apply_pinmux_group()` exposes the board-supported pinmux groups for optional paths such as `QSPI1` and CANFD.
- `apps/be_u1000_demo/main.c` now includes a board self-test banner for EVU-BA bring-up that exercises `UART0`, LED/button GPIO, header `I2C0`, header `SPI1`, and readiness reporting for `QSPI1` and CANFD pinmux groups.
- Renode now includes a minimal `QSPI1` memory window preload plus an `SPI1` controller stub, so `pixi run -e be-u1000 validate-selftest-sim` can verify a non-destructive `QSPI1` read signature in simulation.
- `apps/be_u1000_demo/main.c` now also includes an unpinned `balance` task in SMP builds so the experimental runtime lane can prove that a schedulable non-affined task is observed on both Core0 and Core1.

## Historical affinity validation notes

- `scripts/be_u1000/check_boot_log.py` now accepts repeated `--expect-task-affinity TASK:CoreN` checks.
- The matcher is intentionally tolerant and looks for affinity evidence in `[SMP]`, `[AFFINITY]`, or generic `task ... affinity ... Core0/Core1` UART lines.
- `scripts/be_u1000/run_smp_runtime.py --check-bootlog` can capture the UART log and immediately run the boot-log validator, including optional affinity checks.
- These SMP notes are kept for architecture context only; the maintained `be_u1000` pixi surface now stops at `validate-supported`, `validate-irq-map`, and `validate-selftest-sim`.
- Historical logs still record the prior affinity and scheduler-balancing smoke evidence for later architecture work.
- If SMP runtime work is reintroduced, it should return through a new reviewed change instead of reviving the old task names by drift.
