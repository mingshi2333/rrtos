# IREE Memory Slab Attribution Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Extend the IREE dump analyzer so a runtime peak HAL allocation can be broken down into the compiler resource slab components and the dispatches that use them.

**Architecture:** Keep this as an offline diagnostic script over dumped `.stream.mlir` / `.hal.mlir` files. Reuse the existing SSA size evaluator, add subrange access parsing for `stream.cmd.*` lines, and summarize accesses grouped by the alloca resource argument that backs the slab.

**Tech Stack:** Python standard library, IREE Stream/HAL MLIR dump text, existing `scripts/analyze_iree_compile_dump.py`.

---

### Task 1: Add Stream Slab Access Parsing

**Files:**
- Modify: `scripts/analyze_iree_compile_dump.py`
- Modify: `tests/test_iree_compile_dump_analyzer.py`

- [x] Parse `stream.cmd.execute ... with(... as %argN: !stream.resource<...>{%size})` captures so alloca resources can be mapped to block arguments.
- [x] Parse `ro/rw/wo %argN[%offset for %length]` access lines inside the execute block.
- [x] Resolve `%offset`, `%length`, and backing `%size` through the existing SSA evaluator.
- [x] Add a unit test with one `stream.resource.alloca`, one execute capture, and two access lines.

### Task 2: Print Peak Slab Component Summary

**Files:**
- Modify: `scripts/analyze_iree_compile_dump.py`

- [x] Reuse `--needle-size` to print a component table for matching alloca resources.
- [x] For each matching resource, print access mode, offset, length, dispatch/fill line, and text context.
- [x] Keep default output compact and avoid printing dense constants unless explicitly requested.

### Task 3: Run MiniResNet Attribution

**Commands:**

```bash
python3 scripts/analyze_iree_compile_dump.py \
  --dump-dir build/iree_dumps/miniresnet/phases \
  --needle-size 315264
```

Expected: output identifies `%result_0` / `%arg4` as the 315264B slab and shows the large `204800` region around the first conv + elementwise chain.

Observed:

- Baseline and min-peak dumps both contain the same 315264-byte slab.
- The slab is `%result_0` captured as `%arg4` in the first `stream.cmd.execute`.
- The dominant component is a 204800-byte `tensor<?x32x25x64xi32>` accumulator, located at offset 3968.
- `--iree-stream-resource-max-allocation-size` is not a useful lever for this activation slab in the current IREE source: `LayoutSlices.cpp` uses alignment from `ResourceConfigAttr`, while max allocation size is only checked by `PackConstants.cpp`.

### Task 4: Validate

**Commands:**

```bash
python3 -m py_compile scripts/analyze_iree_compile_dump.py
python3 -m unittest tests.test_iree_compile_dump_analyzer
python3 -m unittest tests.test_ai_mnist_validation_scripts tests.test_stm32ai_zoo_sweep tests.test_ai_model_registry_runtime tests.test_be_u1000_ai_micro_demo tests.test_iree_compile_dump_analyzer
git diff --check
```
