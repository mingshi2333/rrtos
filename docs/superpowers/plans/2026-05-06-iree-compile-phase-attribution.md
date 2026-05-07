# IREE Compile Phase Attribution Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Dump IREE compiler phases for MiniResNet and identify which compiler resource corresponds to the runtime 315,264-byte HAL allocation.

**Architecture:** Extend the existing `scripts/ai_codegen.py` with optional diagnostic output flags rather than creating one-off shell commands. Add a small analyzer script that scans dumped phase IR for numeric allocation sizes and nearby resource operations. Use the same MiniResNet config so runtime and compiler diagnostics remain comparable.

**Tech Stack:** IREE `iree-compile --dump-compilation-phases-to`, existing TFLite import path, Python standard library.

---

### Task 1: Add Codegen Dump Flags

**Files:**
- Modify: `scripts/ai_codegen.py`
- Modify: `tests/test_be_u1000_ai_micro_demo.py`

- [x] Add CLI flags `--dump-compile-phases-to` and `--dump-executable-intermediates-to`.
- [x] Create those directories before invoking `iree-compile`.
- [x] Append `--dump-compilation-phases-to=<dir>` and `--iree-hal-dump-executable-intermediates-to=<dir>` to the compile command when provided.
- [x] Keep default behavior unchanged.

### Task 2: Add Phase Analyzer

**Files:**
- Create: `scripts/analyze_iree_compile_dump.py`
- Create/Modify: tests if lightweight parser coverage is needed

- [x] Accept `--dump-dir` and `--needle-size`.
- [x] Print exact matches for the size and nearby lines.
- [x] Print candidate `stream.resource`, `hal.interface.binding`, and `hal.command_buffer` lines around matches.

### Task 3: Run MiniResNet Diagnostics

**Commands:**

```bash
python3 scripts/ai_codegen.py \
  --config ai_models_stm32ai_miniresnet.yaml \
  --dump-compile-phases-to build/iree_dumps/miniresnet/phases \
  --dump-executable-intermediates-to build/iree_dumps/miniresnet/executables

python3 scripts/analyze_iree_compile_dump.py \
  --dump-dir build/iree_dumps/miniresnet/phases \
  --needle-size 315264
```

Expected: phase dumps exist for `stream`, `hal`, and `vm`. The analyzer either finds a literal `315264` match or reports closest resource-size candidates for the next compiler instrumentation step.

Observed:

- IREE produced phase dumps `1.input.mlir` through `12.vm.mlir` under `build/iree_dumps/miniresnet/phases`.
- The runtime `315264`-byte HAL allocation is not emitted as a literal in the dump; it is computed through SSA.
- The analyzer resolves it to `miniresnetv1_s1_64x50_tl_int8.7.stream.mlir:1233` (`stream.resource.alloca`, `%19`) and `miniresnetv1_s1_64x50_tl_int8.11.hal.mlir:236` (`hal.device.queue.alloca`, `%19`).
- For batch size 1, `%19 = align(3920, 64) + 204800 + 58752 + 13312 + 14336 + 14336 + 5760 = 315264`.

### Task 4: Validate

**Commands:**

```bash
python3 -m unittest tests.test_ai_mnist_validation_scripts tests.test_stm32ai_zoo_sweep tests.test_ai_model_registry_runtime tests.test_be_u1000_ai_micro_demo
git diff --check
```
