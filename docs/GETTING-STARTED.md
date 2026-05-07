# Getting Started

This project is developed and validated through Pixi. Use Pixi tasks instead of
manually assembling compiler, CMake, QEMU, Renode, and Python command lines.

## Prerequisites

Install or provide:

- Pixi
- Git
- QEMU RISC-V system emulator for the RV32 lane
- Renode for the BE-U1000 simulation lane
- A RISC-V GCC installation that can provide RV32 `libgcc.a`

The Pixi environments provide Clang, CMake, Ninja, Python, LLVM tools, Picolibc,
and compiler-rt dependencies used by the supported lanes.

## Clone And Initialize

```bash
git clone https://github.com/mingshi2333/rrtos.git
cd rrtos
pixi install
pixi install -e be-u1000
pixi run init-iree
```

`pixi run init-iree` initializes only the IREE runtime subset used by the
supported RV32 AI lane. It intentionally does not recursively fetch every nested
IREE compiler/GPU submodule.

## Validate The Supported RV32 Lane

```bash
pixi run -e rv32 validate-supported-rv32
```

This task performs:

- configuration authority checks
- SMP configuration matrix checks
- BSP boundary checks
- support contract checks
- AI contract checks
- CMake configure/build
- footprint reports
- AI footprint attribution
- QEMU MNIST runtime validation

The normative runtime output includes:

- `AI_VALIDATION_SAMPLESET`
- one `AI_VALIDATION_METRICS` line per sample
- `AI_VALIDATION_PASS count=5`
- `OS_TIMER_CALLBACK_PASS`

## Validate The Supported BE-U1000 Lane

```bash
pixi run -e be-u1000 validate-supported
```

This task performs:

- configuration authority checks
- SMP configuration matrix checks
- BSP boundary checks
- support contract checks
- BE-U1000 ABI cache checks
- CMake configure/build
- footprint report generation
- interrupt map validation
- Renode boot/selftest validation

The default BE-U1000 app is `apps/be_u1000_demo`.

## Common Development Commands

```bash
pixi run validate-kernel-semantics
pixi run validate-support-contract
pixi run -e rv32 validate-supported-ai
pixi run -e rv32 validate-mnist-runtime
pixi run -e rv32 validate-mnist-quant-runtime
pixi run -e be-u1000 validate-hal-apps
```

`validate-hal-apps` is an observation lane. It builds and runs the BE-U1000 HAL
app matrix, but it is not part of the blocking supported gate unless promoted in
`docs/SUPPORTED_MATRIX.md`.

## Manual Configure Commands

Prefer Pixi tasks, but the supported tasks currently expand to these CMake
configuration patterns:

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 \
  -DARCH=riscv \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DOS_SMP_EN=OFF \
  -DOS_AI_EN=ON

cmake -B build-be_u1000 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 \
  -DCONFIG_BOARD=be_u1000 \
  -DRISCV_MARCH=rv32imafc_zifencei \
  -DRISCV_MABI=ilp32f \
  -DRISCV_ABI=ilp32d \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DOS_SMP_EN=OFF \
  -DOS_AI_EN=OFF
```

## Where To Read Next

- `docs/SUPPORTED_MATRIX.md` for support status.
- `docs/ARCHITECTURE.md` for repository layering.
- `docs/CONFIGURATION.md` for build options and Pixi tasks.
- `docs/TESTING.md` for validation lanes.
- `docs/AI_CANONICAL_PATH.md` for the supported AI contract.
- `docs/HAL_CONFIGURATION.md` for BE-U1000 HAL feature selection.
