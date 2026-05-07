# IREE HAL Allocation Attribution Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Attribute MiniResNet peak heap usage to IREE HAL buffer allocation metadata so the largest 315KB block can be tied to memory type/usage instead of only the generic system allocator.

**Architecture:** Add a tiny weak hook call in IREE's heap HAL allocator. Implement the hook in project-owned RRTOS AI runtime code, recording top HAL buffer allocations without allocating memory. Extend MiniResNet validation to print the HAL allocation trace immediately after invoke.

**Tech Stack:** IREE runtime C, RRTOS heap stats, QEMU rv32 validation.

---

### Task 1: IREE Runtime Hook

**Files:**
- Modify: `third_party/iree/runtime/src/iree/hal/allocator_heap.c`

- [ ] Add weak declarations for `rrtos_iree_hal_allocation_trace_reset` and `rrtos_iree_hal_allocation_trace_record`.
- [ ] Call the record hook after successful `iree_hal_heap_buffer_create`.
- [ ] Pass allocation size, memory type, buffer usage, compatibility parameters, and created buffer pointer as primitive integer values.

### Task 2: Project-Owned Trace Store

**Files:**
- Modify: `ai/include/ai_model_registry.h`
- Modify: `ai/src/ai_model_registry.c`

- [ ] Add fixed-capacity top-N trace structs for HAL allocations.
- [ ] Implement reset/get functions and the hook function called by patched IREE.
- [ ] Keep implementation allocation-free and safe on bare-metal.

### Task 3: MiniResNet Log Output

**Files:**
- Modify: `apps/mnist_app/src/miniresnet_validation_main.c`
- Modify: `scripts/run_miniresnet_validation.py`
- Modify: `tests/test_ai_mnist_validation_scripts.py`

- [ ] Reset HAL allocation trace before `ai_infer_sync`.
- [ ] Print peak and top HAL allocation records after invoke.
- [ ] Require at least one HAL trace record in the QEMU log validator.

### Task 4: Build And Validate

**Commands:**

```bash
pixi run -e rv32 build
python3 scripts/run_miniresnet_validation.py --kernel build/apps/mnist_app/miniresnet_validation --log logs/miniresnet_validation_qemu.log
python3 scripts/validate_kernel_semantics.py
python3 scripts/validate_supported_ai_path.py
python3 -m unittest tests.test_ai_mnist_validation_scripts tests.test_stm32ai_zoo_sweep tests.test_ai_model_registry_runtime
git diff --check
```

Expected: QEMU log contains `MINIRESNET_HAL_TRACE_*` lines, and the largest HAL allocation matches the 315KB heap allocation scale.
