# Configuration

`rrtos` configuration is controlled by Pixi tasks and CMake cache options. Board
headers describe hardware facts; the top-level CMake build selects the active
runtime policy for a lane.

## Pixi Environments

| Environment | Purpose |
| --- | --- |
| `default` | Alias for the supported `rv32` environment |
| `rv32` | QEMU RV32 lane with AI enabled |
| `be-u1000` | BE-U1000 / EVU-BA-2.3 board lane with AI disabled |

Install both supported environments with:

```bash
pixi install
pixi install -e be-u1000
```

## Dependency Initialization

```bash
pixi run init-iree
```

This initializes:

- `third_party/iree`
- `third_party/iree/third_party/benchmark`
- `third_party/iree/third_party/cpuinfo`
- `third_party/iree/third_party/flatcc`
- `third_party/iree/third_party/googletest`

It intentionally does not recursively initialize every IREE nested submodule.
The supported runtime build does not need historical compiler/GPU dependency
refs, and CI should not depend on them.

## Core CMake Options

| Option | Supported values / default | Notes |
| --- | --- | --- |
| `CONFIG_BOARD` | `qemu_virt`, `be_u1000`; default `qemu_virt` | Selects board policy |
| `ARCH_BITS` | `32`, `64`; supported lanes use `32` | 64-bit paths are historical unless documented otherwise |
| `OS_SMP_EN` | `OFF` by default | Supported BE-U1000 gate is single-core; SMP build lanes are experimental |
| `OS_IPC_EN` | `ON` by default | Builds semaphore, mutex, queue, and event primitives |
| `OS_TIMER_EN` | `ON` by default | Builds software timers; requires `OS_IPC_EN=ON` |
| `OS_LIBC_SHIM_EN` | `ON` by default | Builds picolibc syscall bridge inside `librv_aios_kernel.a`; requires heap |
| `OS_HEAP_EN` | `ON` by default | Enables the RTOS heap allocator used by libc and AI shims |
| `OS_AI_EN` | `ON` for `qemu_virt`, `OFF` for `be_u1000` | Supported RV32 AI lane uses ON; BE-U1000 AI micro lanes are explicit opt-in bring-up paths |
| `OS_FL_EN` | `OFF` | Federated learning is not a supported lane |
| `RRTOS_CXX_EN` | `OFF` | Enables the optional freestanding C++/ETL layer |
| `RRTOS_BUILD_EXPERIMENTAL_APPS` | `OFF` | Enables historical apps outside the supported matrix |
| `RRTOS_HAL_FEATURES` | `auto` by default | Semicolon-separated HAL feature override |
| `BE_U1000_APP` | `demo` by default | Selects a BE-U1000 app lane |
| `BE_U1000_MEMORY_MODEL` | `tcm` by default; `flash` for larger firmware | Selects the BE-U1000 board linker script |
| `LINKER_SCRIPT` | board or architecture default | Can be overridden for manual builds |

## Supported RV32 Configuration

The Pixi task is:

```bash
pixi run -e rv32 configure
```

Current configuration:

```text
CONFIG_BOARD=qemu_virt
ARCH_BITS=32
OS_SMP_EN=OFF
OS_AI_EN=ON
CMAKE_BUILD_TYPE=MinSizeRel
CMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake
```

The full supported gate is:

```bash
pixi run -e rv32 validate-supported-rv32
```

## Supported BE-U1000 Configuration

The Pixi task is:

```bash
pixi run -e be-u1000 configure
```

Current configuration:

```text
CONFIG_BOARD=be_u1000
ARCH_BITS=32
RISCV_MARCH=rv32imafc_zifencei
RISCV_MABI=ilp32f
RISCV_ABI=ilp32d
OS_SMP_EN=OFF
OS_AI_EN=OFF
CMAKE_BUILD_TYPE=MinSizeRel
CMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake
```

The full supported gate is:

```bash
pixi run -e be-u1000 validate-supported
```

## BE-U1000 App Selection

`BE_U1000_APP` can select:

```text
demo
gpio_ledblink
gpio_inputpolling
uart_printf
canfd_polling
qspi_flash
i2c_polling
tim_timebase
adc_single
wdt_heartbeat
pwmg_outputcompare
dma_mem2mem
i2s_tx
pwma_timebase
usb_runtime
ai_micro_demo
ai_micro_demo_cpp
etl_smoke
```

With `RRTOS_HAL_FEATURES=auto`, the top-level build maps each app to a
minimal HAL feature set. See `docs/HAL_CONFIGURATION.md` for the feature map.

`etl_smoke` is an experimental C++/ETL proof lane. It requires
`RRTOS_CXX_EN=ON` and keeps `OS_AI_EN=OFF`. ETLCPP is installed by Pixi through
the local `etlcpp` package recipe, not fetched by CMake:

```bash
pixi run -e be-u1000 configure-etl-smoke
pixi run -e be-u1000 build-etl-smoke
```

`ai_micro_demo_cpp` is an experimental C++ app layer over the same BE-U1000 AI
micro model and runtime contract used by the C `ai_micro_demo` lane. It
requires `OS_AI_EN=ON`, `RRTOS_CXX_EN=ON`, and the `flash` memory model because
the IREE-backed AI runtime is too large for the TCM-only linker layout:

```bash
pixi run -e be-u1000 configure-ai-micro-cpp
pixi run -e be-u1000 build-ai-micro-cpp
```

The C++ app includes `ai/include/ai_model_registry_c_api.h`, a narrow C ABI that
exposes model lookup, tensor metadata, and synchronous inference without pulling
IREE headers into freestanding C++ translation units. The full registry header
remains `ai/include/ai_model_registry.h` for runtime/model descriptor code.

## RISC-V Toolchain And Libgcc

The supported Pixi toolchain uses Clang and lld. `cmake/riscv32-pixi.cmake`
probes a GCC installation for RV32 `libgcc.a` because compiler helper routines
may still be required at link time.

The probe checks:

- `riscv64-unknown-elf-gcc`
- `riscv64-linux-gnu-gcc`

It accepts both `lib32/<abi>` style multilib layouts and `rv32`/`ilp32` style
unknown-elf layouts, then passes the discovered library directory and archive to
the linker.

## IREE Runtime Configuration

When `OS_AI_EN=ON`, the top-level CMake disables unsupported IREE surfaces:

- compiler build
- MLIR
- samples
- tests
- default HAL drivers
- GPU-oriented drivers
- dynamic executable plugin defaults
- runtime tracing
- file I/O

The supported runtime uses:

- local-sync HAL driver
- static library executable loader
- project-owned `IREE_USER_CONFIG_H`

## Generated Configuration Evidence

Validation scripts check that supported lane configuration does not drift:

```bash
pixi run validate-config-authority
pixi run validate-smp-config-matrix
pixi run validate-be-u1000-abi-cache
```

These checks are part of the blocking supported gates.
