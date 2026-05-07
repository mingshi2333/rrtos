# Patch-Based Inference Prototype Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a small, testable prototype that estimates whether MiniResNet's first large i32 accumulator can be reduced by patch/tile execution.

**Architecture:** Add a standalone analysis script that consumes the existing HAL schedule JSON, replaces one full-shape accumulator component with a tile-sized scratch component, repacks the existing liveness arena model, and emits Markdown/JSON reports. This does not change firmware inference behavior yet; it is a gate before writing a real patched kernel.

**Tech Stack:** Python standard library, existing `scripts/sweep_static_direct_arena_planners.py`, `unittest`, existing `logs/*hal_schedule.json`.

---

### Task 1: Patch Memory Model Tests

**Files:**
- Create: `tests/test_patch_based_inference_prototype.py`

- [ ] **Step 1: Write failing tests**

Create tests that verify:

- replacing a component inside a larger transient allocation reduces arena pressure
- tile rows are converted to patch scratch bytes
- invalid component sizes are rejected
- Markdown says this is an estimate, not a verified runtime result

- [ ] **Step 2: Run failing tests**

Run:

```bash
python3 -m unittest tests.test_patch_based_inference_prototype
```

Expected: import failure because the prototype script does not exist yet.

### Task 2: Prototype Script

**Files:**
- Create: `scripts/prototype_patch_based_inference.py`

- [ ] **Step 1: Implement evaluator**

Implement functions that load a static-direct HAL schedule, find one transient allocation, replace a known accumulator component with `tile_rows * output_cols * output_channels * element_bytes`, and compute projected effective arena bytes using the same command-liveness packing model as the existing spill/fusion prototype.

- [ ] **Step 2: Implement report CLI**

Support:

```bash
python3 scripts/prototype_patch_based_inference.py \
  --schedule logs/miniresnetv2_s1_64x50_tl_int8_fresh_hal_schedule.json \
  --allocation transient_buffer_1 \
  --component-bytes 204800 \
  --output-rows 32 \
  --output-cols 25 \
  --output-channels 64 \
  --element-bytes 4 \
  --tile-rows 1 --tile-rows 2 --tile-rows 4 --tile-rows 8 --tile-rows 16 \
  --output logs/static_direct_patch_based_inference.md \
  --json-output logs/static_direct_patch_based_inference.json
```

- [ ] **Step 3: Run tests**

Run:

```bash
python3 -m unittest tests.test_patch_based_inference_prototype
```

Expected: pass.

### Task 3: MiniResNet Report

**Files:**
- Create: `logs/static_direct_patch_based_inference.md`
- Create: `logs/static_direct_patch_based_inference.json`

- [ ] **Step 1: Generate report**

Run the command from Task 2 against the MiniResNet schedule.

- [ ] **Step 2: Inspect report**

Confirm the report includes baseline KiB, projected KiB, savings KiB, tile count, and a warning that runtime output is not yet verified.
