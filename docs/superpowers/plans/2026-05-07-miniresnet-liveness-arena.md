# MiniResNet Liveness Arena Reuse Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reduce MiniResNet static-direct RAM by reusing static arena offsets for transient buffers whose lifetimes do not overlap.

**Architecture:** The HAL schedule extractor already emits a linear command list. The descriptor generator will derive each transient allocation's first and last command use, then greedily pack non-overlapping lifetimes into the same arena address range. The runtime stays unchanged because generated slices still resolve to one flat static arena.

**Tech Stack:** Python schedule generator tests, generated C descriptor sources, CMake QEMU firmware, QEMU validation scripts.

---

### Task 1: Add Liveness-Aware Arena Placement Tests

**Files:**
- Modify: `tests/test_static_direct_descriptor_generator.py`

- [x] **Step 1: Test disjoint lifetimes reuse one offset**

Run: `python3 -m unittest tests.test_static_direct_descriptor_generator`
Expected before implementation: failure because arena offsets are currently linear and do not reuse space.

### Task 2: Implement Arena Packing

**Files:**
- Modify: `scripts/generate_static_direct_descriptor.py`

- [x] **Step 1: Compute command intervals**

For each transient allocation, scan generated commands and record `[first_command, last_command + 1)`.

- [x] **Step 2: Pack intervals**

Use 64-byte alignment and place each allocation at the lowest offset that does not overlap any currently live allocation.

- [x] **Step 3: Keep legacy fallback**

Schedules without `commands` synthesize one dispatch command per dispatch.

### Task 3: Regenerate MiniResNet Descriptor

**Files:**
- Modify: `apps/mnist_app/src/miniresnet_static_direct_desc.c`
- Modify: `apps/mnist_app/src/miniresnet_static_direct_desc.h`

- [x] **Step 1: Regenerate from `build/iree_dumps/miniresnet/hal_schedule_test.json`**

Run: `python3 scripts/generate_static_direct_descriptor.py ...`

### Task 4: Verify QEMU Correctness And Footprint

**Files:**
- Modify: `logs/miniresnet_static_direct_qemu_compare.md`

- [x] **Step 1: Build and run**

Run:

```sh
pixi run -e rv32 build-miniresnet-static-direct-qemu-test
pixi run -e rv32 validate-miniresnet-static-direct-qemu-test
```

Expected: `argmax=3 hash=3045847227 top_score_q=99609`.

- [x] **Step 2: Record arena and size**

Run:

```sh
llvm-size build-qemu-test/apps/mnist_app/miniresnet_static_direct_validation
```

Result: `.bss` dropped from `544.9 KiB` to `430.2 KiB`; static arena dropped
from `457.5 KiB` to `342.9 KiB`.
