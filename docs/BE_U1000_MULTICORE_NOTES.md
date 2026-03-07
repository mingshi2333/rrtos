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
- Experimental `be_u1000` SMP build wiring is now enabled behind `OS_SMP_EN=ON`.
- Runtime validation remains experimental and should not be treated as supported status.
- Renode runtime probing now preloads the TCMA image, so boot markers appear and the experimental SMP runtime lane can execute.
- The current experimental lane proves secondary-core online, IPI reschedule, and task-affinity markers for `control -> Core0` and `worker -> Core1`; see `logs/be_u1000_smp_runtime_affinity_probe.md` and `logs/be_u1000_smp_boot.log`.
- Renode still reports several unmapped peripheral warnings during board bring-up, so this remains experimental evidence rather than supported-matrix proof.
- `boards/be_u1000/board_config.h` is now re-audited against the vendor SDK for CLIC numbering and Core2 system-side TCM windows, reducing one of the previously noted board-definition gaps.
- `hal/src/hal_board.c` now follows EVU-BA board defaults for UART0 (`PA6/PA7`), user LED/button (`PC0` / `PC13`), and the board header `I2C0` / `SPI1` pins inferred from the EVU-BA-2.3 technical description tail schematic pages.
- `docs/EVU_BA_PINMAP.md` now captures the recovered EVU-BA-2.3 connector mapping, and `hal_board_apply_pinmux_group()` exposes the board-supported pinmux groups for optional paths such as `QSPI1` and CANFD.
- `apps/be_u1000_demo/main.c` now includes a board self-test banner for EVU-BA bring-up that exercises `UART0`, LED/button GPIO, header `I2C0`, header `SPI1`, and readiness reporting for `QSPI1` and optional CANFD pinmux groups.
- Renode now includes a minimal `QSPI1` memory window preload plus an `SPI1` controller stub, so `pixi run -e be-u1000 validate-selftest-sim` can verify a non-destructive `QSPI1` read signature in simulation.
- `apps/be_u1000_demo/main.c` now also includes an unpinned `balance` task in SMP builds so the experimental runtime lane can prove that a schedulable non-affined task is observed on both Core0 and Core1.

## Experimental affinity validation path

- `scripts/be_u1000/check_boot_log.py` now accepts repeated `--expect-task-affinity TASK:CoreN` checks.
- The matcher is intentionally tolerant and looks for affinity evidence in `[SMP]`, `[AFFINITY]`, or generic `task ... affinity ... Core0/Core1` UART lines.
- `scripts/be_u1000/run_smp_runtime.py --check-bootlog` can capture the UART log and immediately run the boot-log validator, including optional affinity checks.
- `pixi run -e be-u1000 validate-smp-affinity-experimental` is an experimental lane only. It currently validates the existing SMP boot markers and leaves room for future affinity markers once the runtime path is healthy.
- `pixi run -e be-u1000 validate-smp-affinity-experimental` now exercises the experimental SMP runtime lane and validates the current `control -> Core0`, `worker -> Core1` demo mapping.
- `pixi run -e be-u1000 validate-smp-balance-experimental` now validates the scheduler-balancing smoke check by asserting that the unpinned `balance` task appears on both SMP cores.
- `pixi run -e be-u1000 validate-runtime-stages-experimental` now bundles single-core fallback, SMP affinity, and SMP balancing smoke checks into one experimental staged-runtime acceptance command.
- When the demo/runtime path starts emitting affinity lines, run the validator directly to assert them, for example: `python scripts/be_u1000/run_smp_runtime.py --check-bootlog --expect-smp-online-count 2 --require-smp-ipi --expect-task-affinity control:Core0 --expect-task-affinity worker:Core1`.
