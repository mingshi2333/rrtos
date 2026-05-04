# BE-U1000 Dual-Core Bring-Up Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build and real-board validate the minimal BE-U1000 Core0/Core1 SMP image while keeping Core2 reserved.

**Architecture:** Use the existing `OS_SMP_EN=ON` BE-U1000 lane. Core0 performs board/kernel init, Core1 waits in `startup_be_u1000.S` until `_hart_ready[1]` is set, and the demo pins `control` to Core0 and `worker` to Core1. Validation is UART-log driven and must prove CPU1 online, IPI reschedule, affinity mapping, and stable task ticks.

**Tech Stack:** CMake, RV32 bare-metal RISC-V GCC toolchain, BE-U1000 CLIC/CLINT startup, DFU flashing, UART boot-log validation.

---

### Task 1: Configure And Build The SMP Image

**Files:**
- Read: `CMakeLists.txt`
- Read: `config/os_config.h`
- Read: `boards/be_u1000/startup_be_u1000.S`
- Output: `build-be_u1000_smp_real/rrtos_be_u1000.bin`

- [ ] **Step 1: Configure a clean dual-core build**

Run:

```bash
cmake -S . -B build-be_u1000_smp_real \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 \
  -DCONFIG_BOARD=be_u1000 \
  -DRISCV_MARCH=rv32imafc_zifencei \
  -DRISCV_MABI=ilp32f \
  -DRISCV_ABI=ilp32d \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DOS_SMP_EN=ON \
  -DOS_AI_EN=OFF \
  -DBE_U1000_APP=demo \
  -DBE_U1000_MEMORY_MODEL=flash
```

Expected: CMake configures with `OS_CFG_SMP_EN=1`, `OS_CFG_CPU_COUNT=2`, `OS_CFG_CPU_MAX=3`, and `LINKER_SCRIPT=boards/be_u1000/link_be_u1000_flash.ld`.

- [ ] **Step 2: Build the image**

Run:

```bash
cmake --build build-be_u1000_smp_real
```

Expected: `rrtos_be_u1000.elf` and `rrtos_be_u1000.bin` are produced.

### Task 2: Real-Board SMP Smoke Test

**Files:**
- Input: `build-be_u1000_smp_real/rrtos_be_u1000.bin`
- Evidence: `/tmp/beu_smp_real_*.log`

- [ ] **Step 1: Start UART capture**

Run:

```bash
bash -lc 'log=/tmp/beu_smp_real_$(date +%Y%m%d_%H%M%S).log; echo "[MON] $log"; while true; do while [ ! -e /dev/ttyUSB0 ]; do sleep 0.2; done; stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb -ixon -ixoff -echo raw 2>/dev/null || true; cat /dev/ttyUSB0 2>/dev/null | tee -a "$log"; sleep 0.2; done'
```

Expected: UART capture waits across DFU disconnect/reconnect.

- [ ] **Step 2: Flash over DFU alt 2**

Run after the user places the board in DFU mode:

```bash
env SHELL=/bin/bash pkexec dfu-util --device cafe:2303 --alt 2 --download /home/mingshi/Project/PF/rrtos/build-be_u1000_smp_real/rrtos_be_u1000.bin
```

Expected: Download reaches 100%. `LIBUSB_ERROR_NO_DEVICE` after manifest is acceptable because alt 2 auto-runs.

- [ ] **Step 3: Check the SMP markers**

Run:

```bash
python scripts/be_u1000/check_boot_log.py \
  --log /tmp/beu_smp_real_<timestamp>.log \
  --expect-irq-model CLIC \
  --expect-smp-online-count 2 \
  --require-smp-ipi \
  --expect-task-affinity control:Core0 \
  --expect-task-affinity worker:Core1
```

Expected: `BOOT_LOG_CHECK_OK`. If it fails, the failure must identify which marker is missing before changing code.

### Task 3: Minimal Fixes Only If The Smoke Test Fails

**Files:**
- Modify only if needed: `multicore/src/os_smp.c`
- Modify only if needed: `boards/be_u1000/startup_be_u1000.S`
- Modify only if needed: `apps/be_u1000_demo/main.c`
- Modify only if needed: `scripts/be_u1000/check_boot_log.py`

- [ ] **Step 1: If CPU1 does not come online, inspect startup release**

Expected evidence to inspect:

```text
[SMP] CPU1 secondary online
[SMP] online-count: 2
```

If missing, check whether `_hart_ready[1]` is set by `os_smp_start_cpus()` and whether Core1 reaches `os_smp_secondary_start()`.

- [ ] **Step 2: If IPI is missing, inspect CLINT MSIP**

Expected evidence to inspect:

```text
[SMP] CPU1 IPI reschedule
```

If missing, check `hal_clint_send_ipi()` and `os_ipi_handler()` before changing scheduler policy.

- [ ] **Step 3: If task affinity is wrong, inspect demo binding**

Expected evidence to inspect:

```text
[SMP] task-map: control->core0 worker->core1 core2=reserved
[CTRL] core0 tick
[WORK] core1 tick
```

If missing, check `hal_board_bind_demo_tasks()` and the task print paths before enabling load balancing changes.
