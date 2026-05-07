# MiniResNet Patch Arena Remap Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Convert the MiniResNet first-layer patch canary from output-only validation into a reduced-arena runtime validation.

**Architecture:** Keep the existing generated static-direct descriptor as the source of dispatch metadata, but run the continuation through a model-specific remap layer. The remap moves the original large transient suffix from old arena offset `244608` to new arena offset `0`, moves the overlapping live range of the `0..35840` work buffer to `106496`, and leaves later low-address lifetime reuses in their existing offsets.

**Tech Stack:** C firmware validation app, existing `ai_static_direct` runtime, QEMU validation scripts, `unittest`.

---

### Task 1: Model-Specific Remap Runtime

**Files:**
- Create: `apps/mnist_app/src/miniresnet_patch_arena_remap.h`
- Create: `apps/mnist_app/src/miniresnet_patch_arena_remap.c`
- Modify: `apps/mnist_app/CMakeLists.txt`

- [x] **Step 1: Add remap constants and invoke API**

Create a header that defines:

```c
#define MINIRESNET_PATCH_REMAP_ARENA_BYTES 142336u
#define MINIRESNET_PATCH_REMAP_FIRST_OUTPUT_OFFSET 0u
```

and declares `miniresnet_patch_invoke_continuation_remapped(...)`.

- [x] **Step 2: Implement remapped one-command invoke**

Implement a wrapper that copies each continuation command, remaps arena slices and high constants, then invokes `ai_static_direct_invoke()` for that single remapped command.

Rules:

```text
big suffix binding: old offset=35840 length=315264 -> new offset=0 length=106496
big suffix constants: 208768 <= value < 315264 -> value -= 208768
live 35 KiB work buffer: old offset=0 length=35840 -> new offset=106496 length=35840 while command_index < 17
big suffix fill targets: 244608 <= offset < 351104 -> offset -= 244608
```

- [x] **Step 3: Add the new source to `miniresnet_patch_validation`**

Link `miniresnet_patch_arena_remap.c` into the patch validation target.

### Task 2: Patch Validation Uses Reduced Arena

**Files:**
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer.h`
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer.c`
- Modify: `apps/mnist_app/src/miniresnet_patch_validation_main.c`
- Modify: `scripts/run_miniresnet_patch_validation.py`

- [x] **Step 1: Make first-layer output offset configurable**

Change `miniresnet_patch_run_first_layer()` to accept an output offset so the old canary can write at `244608` and the reduced remap can write at `0`.

- [x] **Step 2: Shrink patch validation storage**

Use `MINIRESNET_PATCH_REMAP_ARENA_BYTES` for `patch_arena_storage`, context `arena_bytes`, guard checks, and printed static RAM.

- [x] **Step 3: Invoke the remapped continuation**

Replace the direct continuation `ai_static_direct_invoke()` call with `miniresnet_patch_invoke_continuation_remapped()`.

- [x] **Step 4: Update validator parsing**

Allow the validator to parse the reduced `arena_bytes` value and keep checking output hash/argmax/score.

### Task 3: Verification And Report

**Files:**
- Modify: `logs/miniresnet_patch_first_layer_real_inference_qemu.md`

- [x] **Step 1: Run fast checks**

Run:

```bash
python3 -m py_compile scripts/run_miniresnet_patch_validation.py scripts/run_miniresnet_static_direct_validation.py
python3 -m unittest tests.test_miniresnet_patch_validation tests.test_miniresnet_static_direct_validation tests.test_patch_based_inference_prototype
```

- [x] **Step 2: Run QEMU validation**

Run:

```bash
pixi run build-miniresnet-patch-qemu-test
pixi run validate-miniresnet-patch-qemu-test
pixi run validate-miniresnet-static-direct-qemu-test
```

- [x] **Step 3: Capture size**

Run:

```bash
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation build-qemu-test/apps/mnist_app/miniresnet_static_direct_validation
```

- [x] **Step 4: Update the report**

Record output equality, latency delta, arena KiB delta, static RAM delta, heap peak, and image section size.
