# Graph-Aware Static-Direct Research Implementation Plan

> **For agentic workers:** Use this plan task-by-task. Keep changes
> fail-closed: unsupported models must produce explicit rejection reasons, not
> partial artifacts that look validated.

**Goal:** Turn the current IREE static-direct and MiniResNet patch experiments
into a reproducible research pipeline, then use that pipeline to develop
graph-aware TinyML memory scheduling.

**Architecture:** Build the pipeline in layers. First formalize experiment
manifests and static-direct package generation. Then generate remap plans and
patch/fusion candidates. Only after the sidecar artifacts and oracles are
stable should an IREE compiler pass be attempted.

**Design Reference:** `docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md`

**Tech Stack:** Python standard library, existing IREE compile dumps, generated
C descriptors, QEMU/Renode validation, optional local IREE compiler lane.

---

### Task 0: Git Slash And Review Boundary

**Files:**
- Modify: `docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md`
- Modify: `docs/superpowers/plans/2026-05-07-graph-aware-static-direct-research.md`

- [ ] **Step 1: Keep docs and implementation commits separate**

Use docs commits for design and plan material. Do not mix generated descriptor
C files, runtime changes, or validation logs into the same commit as the design
decision.

- [ ] **Step 2: Keep GSD planning filtering intact**

Do not move this plan into `.planning/` unless the work is intentionally routed
through GSD. If later GSD artifacts are introduced, let `$gsd-pr-branch` handle
the `.planning/` filter instead of inventing another PR cleanup path.

- [ ] **Step 3: Stop before implementation without approval**

After the design draft is updated, report the planned implementation slices and
wait for explicit execution approval before editing scripts, runtime code, or
generated model artifacts.

### Task 1: MCU AI Experiment Manifest

**Files:**
- Create: `docs/research_framework/templates/mcu_ai_case.template.yaml`
- Create: `docs/research_framework/MCU_AI_EXPERIMENT_MANIFEST.md`
- Optional modify: `docs/research_framework/05_CANONICAL_SCHEMA_V0.yaml`

- [ ] **Step 1: Define MCU AI case fields**

Include model source URI/path, model sha256, license, compiler flags, IREE
version, target triple, ABI, board/simulator target, validation sample hash,
expected output policy, and replay command.

- [ ] **Step 2: Define metric names and units**

Use KiB for all memory/flash numbers. Required metrics:

```text
flash_text_kib
flash_rodata_kib
image_total_kib
ram_bss_kib
ram_stack_kib
input_kib
output_kib
const_kib
arena_static_kib
heap_peak_kib
latency_ticks
latency_ms
```

- [ ] **Step 3: Add claim-grade policy**

Reject accepted claims unless they include model hash, git commit, tool
versions, replay command, artifact hashes, metrics JSON, and output oracle.

### Task 2: Static-Direct Package Pipeline

**Files:**
- Modify: `scripts/extract_iree_hal_schedule.py`
- Modify: `scripts/verify_static_direct_schedule.py`
- Modify: `scripts/generate_static_direct_descriptor.py`
- Create: `scripts/generate_static_direct_model.py`
- Optional modify: `scripts/ai_codegen.py`

- [x] **Step 1: Make schedule JSON the canonical ScheduleIR**

Ensure the schedule includes commands, dispatches, allocation classes, fixed
shape status, unsupported feature flags, warnings, and source metadata.

- [x] **Step 2: Move placement authority into verification**

The verifier should produce the authoritative `MemoryPlanIR`: placements,
lifetime intervals, arena bytes, peak transient bytes, and rejection reasons.
The descriptor generator should consume this output rather than independently
choosing arena placement.

- [ ] **Step 3: Generate model packages**

`generate_static_direct_model.py` should produce:

```text
<model>_static_direct_desc.c/.h
<model>_static_direct_const.c/.h
<model>_static_direct_model.c/.h
static_direct.schedule.json
static_direct.verify.json
static_direct.package.json
```

Progress: `verify_static_direct_schedule.py` now reports input/output/const
byte totals alongside arena placement, and
`generate_static_direct_descriptor.py` can consume that verify report as its
`memory_plan`. `scripts/generate_static_direct_model.py` emits
`static_direct.schedule.json`, `static_direct.verify.json`,
`<model>_static_direct_desc.c/.h`, `static_direct.package.json`, and optional
remap plan/C/H artifacts. Package manifests include SHA-256 hashes for emitted
artifacts. Clean constant C/H and wrapper C/H are still later package-lane
slices.

- [x] **Step 4: Add no-VM and output gates**

Every package lane must have a map-symbol gate and either exact output vector
validation or a declared tolerance policy.

Progress: `static_direct.package.json` now records deploy gates. The validator
checks artifact hashes, no-VM map symbols, and can run an output oracle when the
package is bound to a firmware image with `--run-oracle`.

### Task 3: Header Manifest Extraction

**Files:**
- Create: `scripts/extract_iree_emitc_header_manifest.py`
- Create: `tests/test_iree_emitc_header_manifest.py`

- [ ] **Step 1: Extract ABI metadata from generated `.h`**

Extract exports, import list, module dependency names, const symbol, const
alignment, const byte count, rodata names, target triple, ABI, CPU features,
data layout, native vector size, and linked static object path.

- [ ] **Step 2: Use the manifest for admissibility**

The static-direct pipeline should fail closed if the header imports HAL
features not covered by the runtime, or if target metadata does not match the
native object/package metadata.

### Task 4: Patch/Remap Plan Generator

**Files:**
- Create: `scripts/generate_static_direct_remap_plan.py`
- Create: `tests/test_static_direct_remap_plan.py`
- Modify: `apps/mnist_app/src/miniresnet_patch_arena_remap.c` only after the
  generated plan reproduces the current canary.

- [x] **Step 1: Define TransformIR**

Represent replaced command range, produced logical tensors, preserved live
tensors, replacement scratch, continuation command index, and explicit constant
relocation records.

- [x] **Step 2: Generate a MiniResNet remap plan**

Replace hard-coded `OLD_BIG_*` and work-buffer offsets with generated metadata.
The generated plan must reproduce the current QEMU output vector and arena size.

- [ ] **Step 3: Reject unsafe remaps**

Reject remaps when retained dispatch constants cannot be proven to refer to
relocatable arena offsets.

### Task 5: Graph-Aware Candidate Analyzer

**Files:**
- Create: `scripts/analyze_graph_memory_candidates.py`
- Create: `tests/test_graph_memory_candidates.py`

- [ ] **Step 1: Build minimal GraphIR extraction**

Start with the smallest useful subset: tensor shapes, Conv/DepthwiseConv,
requant/rescale, clamp, add/residual, and output boundaries.

- [ ] **Step 2: Rank patch/fusion candidates**

For each candidate, report estimated RAM saved, tile rows, halo/cache bytes,
recompute cost, required kernel template, and validation readiness.

- [ ] **Step 3: Connect candidates to ScheduleIR**

Each candidate must identify the schedule allocation/command range it would
replace. If the graph region cannot be mapped to a HAL schedule region, mark it
analysis-only.

### Task 6: MiniResNet Fixed-Point Patch V2

**Files:**
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer.c`
- Modify: `tests/test_miniresnet_patch_validation.py`
- Modify: `logs/miniresnet_patch_first_layer_real_inference_qemu.md`

- [ ] **Step 1: Replace slow requant path**

Replace floating-point requant with fixed-point arithmetic that is bit-exact or
within the declared tolerance.

- [ ] **Step 2: Generate RAM/latency Pareto**

Run several tile row/cache policies and report KiB saved, latency ticks,
latency at 250 MHz, and output oracle status.

- [ ] **Step 3: Promote only validated claims**

Do not claim a generic patch-based result until at least MiniResNet, MNIST, and
one depthwise/residual zoo model have either passed validation or produced
explicit fail-closed reasons.

### Task 7: Compiler-Pass Preparation

**Files:**
- Modify only after Tasks 1-6 establish stable sidecar artifacts.

- [ ] **Step 1: Golden sidecar oracle**

Freeze representative schedule/package/transform JSON files as compiler-pass
golden outputs.

- [ ] **Step 2: HAL descriptor export prototype**

Prototype an IREE HAL pass that emits descriptor metadata equivalent to the
sidecar extractor.

- [ ] **Step 3: Graph-level transform prototype**

Prototype Linalg/Flow-side identification of conv->requant tiling opportunities
without changing runtime behavior first.

---

## Verification Commands

Use these after each implementation slice, adjusted for the touched files:

```bash
python3 -m py_compile scripts/extract_iree_hal_schedule.py scripts/verify_static_direct_schedule.py scripts/generate_static_direct_descriptor.py
python3 -m unittest tests.test_static_direct_schedule_verifier tests.test_static_direct_descriptor_generator tests.test_iree_hal_schedule_extractor
pixi run build-miniresnet-static-direct-qemu-test
pixi run validate-miniresnet-static-direct-qemu-test
pixi run validate-miniresnet-patch-qemu-test
git diff --check
```

## Exit Criteria

- The static-direct package path is generated for admitted models.
- Unsupported models report deterministic fail-closed reasons.
- MiniResNet patch/remap has generated metadata, no hand-written offset policy,
  and QEMU output oracle evidence.
- Metrics are emitted as machine-readable JSON before Markdown reports.
- The next IREE compiler-pass step has golden sidecar outputs to compare
  against.

## Stop Point For This Draft

Stop after Task 0 and design review unless the user explicitly approves
execution. The next executable slice should be Task 1, not script/runtime
changes directly.
