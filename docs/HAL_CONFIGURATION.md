# HAL Configuration

`rrtos` uses a lightweight Zephyr-style split for HAL configuration:

- board headers describe immutable hardware facts
- CMake selects build policy
- apps select the HAL features they need
- the linker still removes unused sections as a final safety net

This keeps the project lighter than a full Kconfig/devicetree stack while making
the build surface explicit.

## Board Facts

Board facts live in `boards/<board>/board_config.h`.

For BE-U1000, `boards/be_u1000/board_config.h` owns:

- peripheral base addresses
- IRQ numbers
- clock facts
- pinmux facts
- board resources such as console UART, user LED, user button, and QSPI window

Do not put per-application feature policy in board headers. A board header may
say a USB controller exists; it should not decide whether a specific firmware
image builds the USB HAL.

## Build Policy

The top-level `CMakeLists.txt` owns the selected build policy.

Important variables:

- `CONFIG_BOARD`: supported values are `qemu_virt` and `be_u1000`
- `BE_U1000_APP`: selects the BE-U1000 firmware app
- `OS_AI_EN`: enables AI/IREE build integration
- `RRTOS_HAL_FEATURES`: semicolon-separated HAL feature list, or `auto`

The default `RRTOS_HAL_FEATURES=auto` maps `BE_U1000_APP` to the smallest
maintained feature set for that app. Example:

```bash
cmake -B build-be_u1000_uart \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 \
  -DCONFIG_BOARD=be_u1000 \
  -DBE_U1000_APP=uart_printf \
  -DOS_AI_EN=OFF
```

This selects `board;uart;clint;irq_clic`.

Manual override is available when bringing up a new combination:

```bash
cmake -B build-be_u1000_custom \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 \
  -DCONFIG_BOARD=be_u1000 \
  -DBE_U1000_APP=uart_printf \
  -DRRTOS_HAL_FEATURES="board;uart;clint;irq_clic;usb" \
  -DOS_AI_EN=OFF
```

## BE-U1000 HAL Features

Common base features:

- `board`
- `uart`
- `clint`
- `irq_clic`

Optional features:

- `board_selftest`
- `gpio`
- `i2c`
- `spi`
- `flash`
- `canfd`
- `adc`
- `dma`
- `i2s`
- `pwma`
- `pwmg`
- `usb`
- `wdt`
- `corectrl`
- `mailbox`

`board_selftest` is intentionally separate from `board` because the current
board self-test touches GPIO, I2C, SPI, flash, and CANFD. Firmware that only
needs board init and a banner should use `board` without `board_selftest`.

## AI Build Policy

The supported RV32 AI lane explicitly uses `OS_AI_EN=ON`.

BE-U1000 non-AI firmware lanes explicitly use `OS_AI_EN=OFF`. This prevents
AI/IREE from entering the build graph for board HAL apps. Link-time garbage
collection already kept AI/IREE out of the BE-U1000 final ELF, but disabling the
build graph reduces build time, build directory noise, and support ambiguity.

## Validation

Use these commands after changing HAL feature mapping:

```bash
pixi run -e be-u1000 validate-hal-apps --build-only
pixi run -e be-u1000 validate-hal-apps
pixi run -e be-u1000 validate-supported
pixi run -e rv32 validate-supported-ai
```

`validate-hal-apps` is an observation lane. It builds every maintained BE-U1000
HAL app separately, runs each one under Renode, and validates the captured UART
log with the app-specific checker.
