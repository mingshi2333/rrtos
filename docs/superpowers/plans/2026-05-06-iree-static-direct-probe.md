# IREE Static Direct Probe Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a BE-U1000 probe app that links an IREE `static-library` model object without the IREE VM runtime and prints the executable-library export table.

**Architecture:** The app bypasses `ai_model_registry.c`, `rv_aios_ai`, generated `vm-c`, and HAL modules. It includes IREE's standalone `iree/hal/local/executable_library.h`, calls the generated `be_u1000_hello_world_float_linked_library_query` symbol from `be_u1000_hello_world_float.o`, and reports the export metadata over UART.

**Tech Stack:** C, CMake, BE-U1000 RTOS app lane, IREE static executable library ABI.

---

### Task 1: Register the Probe Lane

**Files:**
- Modify: `CMakeLists.txt`
- Test: `tests/test_be_u1000_ai_micro_demo.py`

- [ ] **Step 1: Add assertions for the new app lane**

```python
def test_cmake_exposes_static_direct_probe_lane(self):
    cmake_text = (PROJECT_ROOT / "CMakeLists.txt").read_text()

    self.assertIn("ai_static_direct_probe", cmake_text)
    self.assertIn('BE_U1000_APP STREQUAL "ai_static_direct_probe"', cmake_text)
    self.assertIn("apps/be_u1000_ai_static_direct_probe", cmake_text)
    self.assertNotIn("ai_static_direct_probe requires OS_AI_EN=ON", cmake_text)
```

- [ ] **Step 2: Run the focused test and expect it to fail before CMake is updated**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_cmake_exposes_static_direct_probe_lane`

Expected: `FAIL` because `ai_static_direct_probe` is not registered yet.

- [ ] **Step 3: Update `CMakeLists.txt`**

Add `ai_static_direct_probe` to the cache string, supported app property, HAL feature auto branch, application dispatch branch, and unsupported-app error text. The lane must not require `OS_AI_EN`.

- [ ] **Step 4: Re-run the focused test**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_cmake_exposes_static_direct_probe_lane`

Expected: `OK`.

### Task 2: Add the VM-Free Query App

**Files:**
- Create: `apps/be_u1000_ai_static_direct_probe/main.c`
- Create: `apps/be_u1000_ai_static_direct_probe/CMakeLists.txt`
- Test: `tests/test_be_u1000_ai_micro_demo.py`

- [ ] **Step 1: Add assertions for direct-link constraints**

```python
def test_static_direct_probe_links_raw_static_object_without_vm_runtime(self):
    app_dir = PROJECT_ROOT / "apps" / "be_u1000_ai_static_direct_probe"
    main_text = (app_dir / "main.c").read_text()
    cmake_text = (app_dir / "CMakeLists.txt").read_text()

    self.assertIn("be_u1000_hello_world_float_linked_library_query", main_text)
    self.assertIn("IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST", main_text)
    self.assertIn("BE_U1000_STATIC_DIRECT_PROBE_PASS", main_text)
    self.assertIn("iree/hal/local/executable_library.h", main_text)
    self.assertIn("be_u1000_hello_world_float.o", cmake_text)
    self.assertNotIn("rv_aios_ai", cmake_text)
    self.assertNotIn("rv_aios_models", cmake_text)
    self.assertNotIn("iree_vm_context_run_function", main_text)
```

- [ ] **Step 2: Run the focused test and expect it to fail before app files exist**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_static_direct_probe_links_raw_static_object_without_vm_runtime`

Expected: `ERROR` or `FAIL` because the app files do not exist yet.

- [ ] **Step 3: Implement `main.c`**

Create a single RTOS task that builds a zeroed `iree_hal_executable_environment_v0_t`, calls `be_u1000_hello_world_float_linked_library_query(IREE_HAL_EXECUTABLE_LIBRARY_VERSION_LATEST, &env)`, casts the first header pointer to `const iree_hal_executable_library_v0_t *`, prints header/export metadata, emits `BE_U1000_STATIC_DIRECT_PROBE_PASS`, and idles.

- [ ] **Step 4: Implement `CMakeLists.txt`**

Create `rrtos_be_u1000` from `main.c`, add the generated `be_u1000_hello_world_float.o` as a source, include `third_party/iree/runtime/src`, link only `rv_aios_rtos`, `c`, `m`, `gcc`, and `gcc`, then generate `.bin`, size output, and footprint markdown.

- [ ] **Step 5: Re-run the focused test**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_static_direct_probe_links_raw_static_object_without_vm_runtime`

Expected: `OK`.

### Task 3: Build and Runtime Verification

**Files:**
- Read-only verification of `build-be_u1000_ai_static_direct_probe/rrtos_be_u1000.map`
- Read-only verification of `logs/be_u1000_ai_static_direct_probe.uart.log`

- [ ] **Step 1: Run the full relevant Python unittest module**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo`

Expected: `OK`.

- [ ] **Step 2: Configure and build the probe**

Run:

```bash
pixi run -e be-u1000 cmake -B build-be_u1000_ai_static_direct_probe \
  -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
  -DARCH_BITS=32 -DCONFIG_BOARD=be_u1000 \
  -DBE_U1000_MEMORY_MODEL=flash \
  -DRISCV_MARCH=rv32imafc_zifencei \
  -DRISCV_MABI=ilp32f -DRISCV_ABI=ilp32d \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DOS_SMP_EN=OFF -DOS_AI_EN=OFF -DRRTOS_CXX_EN=OFF \
  -DBE_U1000_APP=ai_static_direct_probe
pixi run -e be-u1000 cmake --build build-be_u1000_ai_static_direct_probe
```

Expected: build exits 0 and produces `rrtos_be_u1000.elf`, `.bin`, `.map`, and footprint report.

- [ ] **Step 3: Check the map for accidental VM linkage**

Run: `rg -n "iree_vm_|iree_modules_hal|iree_hal_static_library_loader|iree_vm_context" build-be_u1000_ai_static_direct_probe/rrtos_be_u1000.map`

Expected: no matches.

- [ ] **Step 4: Run the probe in Renode**

Run:

```bash
python3 scripts/be_u1000/run_smp_runtime.py \
  --kernel build-be_u1000_ai_static_direct_probe/rrtos_be_u1000.elf \
  --binary build-be_u1000_ai_static_direct_probe/rrtos_be_u1000.bin \
  --log logs/be_u1000_ai_static_direct_probe.uart.log \
  --summary logs/be_u1000_ai_static_direct_probe_runtime.md \
  --sleep-seconds 5
```

Expected: UART log contains `BE_U1000_STATIC_DIRECT_PROBE_PASS` and export metadata. If the script exits non-zero only because it lacks a boot-log checker, inspect the UART log directly.

### Self-Review

- Spec coverage: The plan registers a lane, adds a VM-free direct query app, verifies build output, checks the map for VM symbols, and runs Renode.
- Placeholder scan: No placeholder tasks remain.
- Type consistency: The IREE ABI names match `third_party/iree/runtime/src/iree/hal/local/executable_library.h`.
