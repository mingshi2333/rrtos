# MNIST Descriptor Runtime Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a QEMU-validated MNIST execution path that uses a descriptor-driven reusable op runtime instead of IREE VM/HAL or generated static dispatch functions.

**Architecture:** The prototype introduces a small reusable descriptor runtime under `ai/` with tensor-region descriptors, op descriptors, and generic f32 fully-connected/softmax kernels. `apps/mnist_app` provides the MNIST-specific descriptor and validation entry, reusing the generated constant blob only as model parameters.

**Tech Stack:** C11, CMake, QEMU riscv32, existing RTOS heap statistics and CLINT timing helpers.

---

### Task 1: Tests And Runner Contract

**Files:**
- Modify: `tests/test_mnist_static_direct_validation.py`
- Create: `scripts/run_mnist_descriptor_validation.py`

- [ ] **Step 1: Write failing tests**

Add tests that import `validate_mnist_descriptor_validation_output`, validate the expected `MNIST_DESCRIPTOR_*` log contract, and assert that CMake declares `mnist_descriptor_validation`.

- [ ] **Step 2: Run test to verify failure**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: FAIL because `scripts.run_mnist_descriptor_validation` does not exist yet.

- [ ] **Step 3: Implement the minimal runner**

Create `scripts/run_mnist_descriptor_validation.py` with QEMU invocation matching `run_mnist_static_direct_validation.py`, but validating `MNIST_DESCRIPTOR_PROBE_PASS`, `MNIST_DESCRIPTOR_STATIC_RAM`, `MNIST_DESCRIPTOR_MODEL_PEAK`, `MNIST_DESCRIPTOR_RESULT`, and `MNIST_DESCRIPTOR_PASS`.

- [ ] **Step 4: Run tests to verify runner behavior**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: PASS for validator unit tests, CMake target test still failing until Task 3.

### Task 2: Reusable Descriptor Runtime

**Files:**
- Create: `ai/include/ai_descriptor_runtime.h`
- Create: `ai/src/ai_descriptor_runtime.c`
- Modify: `ai/CMakeLists.txt`

- [ ] **Step 1: Write failing structural tests**

Extend tests to assert that `rv_aios_ai_descriptor` exists, the runtime source has no `iree/` includes, and the public header defines tensor-region/op descriptors.

- [ ] **Step 2: Run test to verify failure**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: FAIL because descriptor runtime files and CMake target do not exist.

- [ ] **Step 3: Implement runtime**

Define regions (`INPUT`, `OUTPUT`, `ARENA`, `CONST`), tensor descriptors, op descriptors, model descriptor, context, `ai_descriptor_required_arena_bytes`, and `ai_descriptor_invoke`. Implement f32 fully-connected with optional ReLU and f32 softmax.

- [ ] **Step 4: Run tests**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: PASS for runtime structure checks.

### Task 3: MNIST Descriptor Validation Target

**Files:**
- Create: `apps/mnist_app/src/descriptor_mnist_model.h`
- Create: `apps/mnist_app/src/descriptor_mnist_model.c`
- Create: `apps/mnist_app/src/descriptor_validation_main.c`
- Modify: `apps/mnist_app/CMakeLists.txt`

- [ ] **Step 1: Write failing CMake tests**

Assert that `mnist_descriptor_validation` links `rv_aios_ai_descriptor`, uses `descriptor_validation_main.c`, does not link `MNIST_STATIC_DIRECT_MODEL_OBJECT`, and keeps IREE runtime out of the descriptor target.

- [ ] **Step 2: Run test to verify failure**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: FAIL because the target and sources do not exist.

- [ ] **Step 3: Implement MNIST descriptor**

Use constant offsets for the IREE sample MNIST parameter layout: fc1 weights offset `0`, fc1 bias `401408`, fc2 weights `401920`, fc2 bias `407040`. Execute FC+ReLU, FC, and in-place softmax.

- [ ] **Step 4: Wire CMake target**

Add `configure_mnist_descriptor_target` and `mnist_descriptor_validation`, linked only with `rv_aios_ai_descriptor` and the existing runtime libs.

- [ ] **Step 5: Run tests**

Run: `python3 -m unittest tests.test_mnist_static_direct_validation`
Expected: PASS.

### Task 4: Build And QEMU Verification

**Files:**
- Modify: `pixi.toml`
- Create: `logs/mnist_descriptor_qemu_compare.md`

- [ ] **Step 1: Add pixi tasks**

Expose `build-mnist-descriptor-qemu-test` and `validate-mnist-descriptor-qemu-test` for the QEMU-only 64M lane.

- [ ] **Step 2: Build**

Run: `pixi run -e rv32 build-mnist-descriptor-qemu-test`
Expected: target builds and prints size.

- [ ] **Step 3: Run QEMU validation**

Run: `pixi run -e rv32 validate-mnist-descriptor-qemu-test`
Expected: `MNIST_DESCRIPTOR_PASS count=1`.

- [ ] **Step 4: Check symbols and size**

Run `scripts/check_no_iree_vm_symbols.py` against the map and `llvm-size` against descriptor/static-direct/static/vmvx binaries.

- [ ] **Step 5: Save comparison**

Write `logs/mnist_descriptor_qemu_compare.md` with binary size, static RAM, peak heap, and latency ticks.
