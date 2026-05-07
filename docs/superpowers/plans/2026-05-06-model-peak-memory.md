# Model Peak Memory Instrumentation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add first-stage model peak memory measurement for the BE-U1000 IREE AI demo.

**Architecture:** Track live and peak bytes in the RTOS heap allocator, then bracket model runtime init and inference in the C++ AI demo. This captures the current IREE path because IREE host/HAL heap allocations are routed through `os_malloc`/`os_free`.

**Tech Stack:** C RTOS heap allocator, C++ BE-U1000 AI demo, Python unittest source checks, kernel C semantic tests.

---

### Task 1: Heap High-Water API

**Files:**
- Modify: `memory/include/os_mem.h`
- Modify: `memory/src/os_mem.c`
- Test: `tests/kernel/test_mem_semantics.c`

- [ ] **Step 1: Write the failing kernel semantic test**

Add a test in `tests/kernel/test_mem_semantics.c` that resets the kernel, reads `os_heap_stats_get()`, allocates two heap blocks, verifies `current_used_bytes`, `peak_used_bytes`, allocation/free counters, calls `os_heap_stats_reset_peak()`, and verifies the reset keeps current live bytes while lowering peak to current live bytes.

- [ ] **Step 2: Run the kernel memory test to verify RED**

Run:

```bash
cmake -S tests/kernel -B build-kernel-tests
cmake --build build-kernel-tests
ctest --test-dir build-kernel-tests -R kernel-semantic-red --output-on-failure
```

Expected: build fails or the memory suite fails because `os_heap_stats_t`, `os_heap_stats_get`, and `os_heap_stats_reset_peak` do not exist.

- [ ] **Step 3: Add heap stats API and implementation**

Add `os_heap_stats_t` to `memory/include/os_mem.h` under `OS_CFG_HEAP_EN`:

```c
typedef struct os_heap_stats {
    os_size_t current_used_bytes;
    os_size_t peak_used_bytes;
    os_size_t allocation_count;
    os_size_t free_count;
} os_heap_stats_t;

os_heap_stats_t os_heap_stats_get(void);
void os_heap_stats_reset_peak(void);
```

Update `memory/src/os_mem.c` so `os_heap_init()` zeros all stats, `os_malloc()` increments current/peak/allocation count by the aligned payload size actually assigned to the block, and `os_free()` decrements current/free count before coalescing. `os_heap_stats_reset_peak()` must set peak to current live bytes, not zero, so a scope that begins with persistent model allocations remains honest.

- [ ] **Step 4: Run the kernel memory test to verify GREEN**

Run:

```bash
cmake --build build-kernel-tests
ctest --test-dir build-kernel-tests -R kernel-semantic-red --output-on-failure
```

Expected: `kernel-semantic-red` passes.

### Task 2: AI Demo Model Peak Log

**Files:**
- Modify: `apps/be_u1000_ai_micro_demo_cpp/main.cpp`
- Test: `tests/test_be_u1000_ai_micro_demo.py`

- [ ] **Step 1: Write the failing source contract test**

Extend `tests/test_be_u1000_ai_micro_demo.py::test_cpp_demo_reuses_c_ai_contract_through_cxx_app_layer` to require the C++ demo to include `MODEL_PEAK`, `os_heap_stats_get`, `os_heap_stats_reset_peak`, `phase=init`, and `phase=invoke`.

- [ ] **Step 2: Run the focused Python test to verify RED**

Run:

```bash
python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_cpp_demo_reuses_c_ai_contract_through_cxx_app_layer
```

Expected: fails because the model peak log strings and heap stats calls are not present.

- [ ] **Step 3: Add model peak logging in the C++ demo**

In `HelloWorldRunner::init()`, call `os_heap_stats_reset_peak()` immediately before `ai_runtime_init()`, then print:

```text
[AI_MICRO_CPP] MODEL_PEAK model=be_u1000_hello_world_float phase=init heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u
```

In `HelloWorldRunner::run_once()`, call `os_heap_stats_reset_peak()` after metadata lookup and before the inference loop, then print a single invoke summary after all inference iterations:

```text
[AI_MICRO_CPP] MODEL_PEAK model=be_u1000_hello_world_float phase=invoke heap_current_bytes=%u heap_peak_bytes=%u heap_alloc_count=%u heap_free_count=%u
```

- [ ] **Step 4: Run the focused Python test to verify GREEN**

Run:

```bash
python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_cpp_demo_reuses_c_ai_contract_through_cxx_app_layer
```

Expected: focused Python test passes.

### Task 3: Integrated Verification

**Files:**
- No new files

- [ ] **Step 1: Run unit tests for touched surfaces**

Run:

```bash
python3 -m unittest tests.test_be_u1000_ai_micro_demo
cmake --build build-kernel-tests
ctest --test-dir build-kernel-tests --output-on-failure
```

Expected: all tests pass.

- [ ] **Step 2: Build BE-U1000 C++ AI demo**

Run:

```bash
pixi run -e be-u1000 configure-ai-micro-cpp
pixi run -e be-u1000 build-ai-micro-cpp
```

Expected: firmware builds successfully.

- [ ] **Step 3: Run simulator if available**

Run the existing BE-U1000 Renode lane used for the AI demo, then inspect the UART log for both `MODEL_PEAK ... phase=init` and `MODEL_PEAK ... phase=invoke`.

Expected: demo still prints `BE_U1000_TFLITE_HELLO_CPP_DEMO_PASS`, latency lines, and two model peak memory lines.
