# RTOS Split CXX ETL Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Split RRTOS build modules first, then add a disabled-by-default C++/ETL lane that proves the seam without rewriting the C kernel.

**Architecture:** Preserve the existing C/ASM kernel ABI and the `librv_aios_kernel.a` footprint artifact while introducing object-library submodules for kernel source ownership. Split BE-U1000 board support into core, pinmux, and selftest implementation files behind the same public `hal_board.h` interface. Add `RRTOS_CXX_EN` as an opt-in profile and validate it with a small BE-U1000 ETL smoke app.

**Tech Stack:** C11, ASM, optional freestanding C++17, CMake object libraries, ETLCPP header-only library, Pixi RV32/BE-U1000 toolchains.

---

### Task 1: Kernel Module Split

**Files:**
- Modify: `kernel/CMakeLists.txt`
- Modify: `config/os_config.h`

- [ ] Replace recursive kernel globbing with explicit source lists grouped into object libraries: `rv_aios_kernel_arch`, `rv_aios_kernel_core`, `rv_aios_kernel_ipc`, `rv_aios_kernel_timer`, `rv_aios_kernel_libc`, and conditional `rv_aios_kernel_smp`.
- [ ] Keep `rv_aios_kernel` as a static aggregate built from the object libraries so existing footprint scripts still read `build*/kernel/librv_aios_kernel.a`.
- [ ] Add build-selected feature macros for IPC, software timers, libc syscall shim, and heap, defaulting to the current enabled behavior.
- [ ] Run `python3 scripts/validate_config_authority.py`.

### Task 2: HAL Board Split

**Files:**
- Modify: `hal/src/hal_board.c`
- Create: `hal/src/hal_board_be_u1000_internal.h`
- Create: `hal/src/hal_board_be_u1000_pinmux.c`
- Create: `hal/src/hal_board_selftest.c`
- Modify: `hal/CMakeLists.txt`
- Modify: `scripts/validate_bsp_seam.py`

- [ ] Move BE-U1000 CRU and pinmux implementation into `hal_board_be_u1000_pinmux.c`.
- [ ] Move board selftest profiles and runners into `hal_board_selftest.c`.
- [ ] Keep board init, identity, banner, execution profile, task mapping, and SMP helpers in `hal_board.c`.
- [ ] Wire `RRTOS_HAL_HAS_BOARD_SELFTEST` so selftest source is compiled only when the `board_selftest` feature is present.
- [ ] Run `python3 scripts/validate_bsp_seam.py`.

### Task 3: Optional C++/ETL Layer

**Files:**
- Modify: `CMakeLists.txt`
- Create: `cmake/rrtos_cxx.cmake`
- Create: `cxx/CMakeLists.txt`
- Create: `cxx/include/etl_profile.h`
- Create: `cxx/include/rrtos/cxx/static_queue.hpp`
- Create: `cxx/src/cxx_runtime.cpp`
- Create: `apps/be_u1000_etl_smoke/CMakeLists.txt`
- Create: `apps/be_u1000_etl_smoke/main.cpp`

- [ ] Add `RRTOS_CXX_EN=OFF` and only enable C++/ETL when the option is ON.
- [ ] Fetch or use ETLCPP as an interface dependency pinned to `mingshi2333/etl` commit `293c7dfcfc9582cf955a4d72264f92db4bba3c8e`.
- [ ] Compile C++ with `-ffreestanding -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit`.
- [ ] Add `rrtos::StaticQueue<T, N>` as the first C++ facade, backed by ETL fixed-capacity storage.
- [ ] Add `BE_U1000_APP=etl_smoke`, requiring `RRTOS_CXX_EN=ON`.

### Task 4: Footprint Budget Hooks and Docs

**Files:**
- Modify: `scripts/report_build_footprint.py`
- Modify: `README.md`
- Modify: `docs/ARCHITECTURE.md`
- Modify: `docs/CONFIGURATION.md`
- Modify: `docs/HAL_CONFIGURATION.md`

- [ ] Let footprint reports accept optional `--max-image-bytes` and `--max-kernel-dec-bytes` budgets, failing when exceeded.
- [ ] Document the kernel/HAL split and the opt-in C++/ETL lane.
- [ ] Keep supported-lane defaults unchanged.

### Task 5: Verification

**Commands:**
- `python3 scripts/validate_config_authority.py`
- `python3 scripts/validate_bsp_seam.py`
- `python3 -m unittest tests.test_be_u1000_ai_micro_demo tests.test_be_u1000_pixi_toolchain tests.test_ai_model_registry_runtime`
- `pixi run validate-kernel-semantics`
- `pixi run -e be-u1000 configure`
- `pixi run -e be-u1000 build`
- `cmake -S . -B build-be_u1000_etl_smoke -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake -DARCH_BITS=32 -DCONFIG_BOARD=be_u1000 -DRISCV_MARCH=rv32imafc_zifencei -DRISCV_MABI=ilp32f -DRISCV_ABI=ilp32d -DCMAKE_BUILD_TYPE=MinSizeRel -DOS_SMP_EN=OFF -DOS_AI_EN=OFF -DRRTOS_CXX_EN=ON -DBE_U1000_APP=etl_smoke`
- `cmake --build build-be_u1000_etl_smoke`
