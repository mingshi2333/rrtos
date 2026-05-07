# IREE Static-Direct HAL Oracle Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace handwritten static-direct dispatch scheduling with a generated descriptor that preserves IREE's compiler-planned HAL schedule and transient memory plan while keeping the runtime free of VM/HAL.

**Architecture:** Treat the compiler-generated HAL schedule as the oracle first, then replay it with a small static-direct runner, and only after that move descriptor emission into a forked IREE compiler pass. `.7.stream.mlir` is used for lifetime/slab attribution, `.11.hal.mlir` is used as the schedule ABI oracle, and generated VM-C `.h` is used only for cross-checks. The first supported runtime mode is fixed-shape, synchronous, single-queue execution; unsupported dynamic or asynchronous forms must fail closed during descriptor generation.

**Tech Stack:** Python parsers/generators, IREE phase dumps, C static-direct runner, QEMU validation, existing RTOS heap/HAL trace instrumentation.

---

## Design Decisions

- Do not parse generated VM-C `.h` as the primary schedule source. It is lowered C, not a stable descriptor ABI.
- Do not use RTOS heap peak as model activation peak. Report `heap_peak`, `hal_allocation_peak`, and `static_direct_arena_peak` as separate metrics.
- Do not hand-maintain dispatch tables. Every static-direct descriptor must be generated from IREE compiler artifacts.
- Use `.11.hal.mlir` as the first schedule oracle because it contains final dispatch ordinals, workgroup counts, constants, buffer subspans, barriers, queue allocas, and deallocas.
- Use `.7.stream.mlir` to explain transient slab composition and lifetime planning, not as the final native dispatch ABI.
- Stage 1 and Stage 2 reject unresolved dynamic dimensions, non-linear fence DAGs, unsupported local memory pages, unknown subspans, and unbound dispatch bindings.

## Files

- Create: `scripts/extract_iree_hal_schedule.py`
- Create: `scripts/generate_static_direct_descriptor.py`
- Create: `tests/test_iree_hal_schedule_extractor.py`
- Create: `tests/test_static_direct_descriptor_generator.py`
- Create: `ai/include/ai_static_direct.h`
- Create: `ai/src/ai_static_direct_runner.c`
- Create: `apps/mnist_app/src/static_direct_descriptor_validation_main.c`
- Create: `scripts/run_mnist_static_direct_descriptor_validation.py`
- Modify: `apps/mnist_app/CMakeLists.txt`
- Modify: `pixi.toml`
- Modify: `scripts/ai_codegen.py` only if a shared hook is needed for dump/descriptor generation
- Modify: `docs/AI_IREE_STATIC_DIRECT_PROBE.md`

---

### Task 1: HAL Schedule Extractor Schema And Tests

**Files:**
- Create: `tests/test_iree_hal_schedule_extractor.py`
- Create: `scripts/extract_iree_hal_schedule.py`

- [x] **Step 1: Write extractor unit tests**

Add tests that build a small HAL MLIR fixture containing:

```mlir
%c0 = arith.constant 0 : index
%c40 = arith.constant 40 : index
%c576 = arith.constant 576 : index
%arena = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%arena_ready) : !hal.buffer{%c576}
%out = hal.device.queue.alloca<%device : !hal.device> wait(%ready) signal(%out_ready) : !hal.buffer{%c40}
%slice = hal.buffer.subspan<%arena : !hal.buffer>[%c0, %c576] : !hal.buffer
hal.command_buffer.dispatch<%cmd : !hal.command_buffer> target(%exe : !hal.executable)[%c1] workgroups([%c1, %c1, %c1]) constants([]) bindings([
  %slice[%c0, %c576] : !hal.buffer,
  %out[%c0, %c40] : !hal.buffer
])
hal.device.queue.dealloca<%device : !hal.device> %arena wait(%done) signal(%freed)
```

Assertions:

```text
allocations[0].id == "arena"
allocations[0].size == 576
dispatches[0].ordinal == 1
dispatches[0].workgroups == [1, 1, 1]
dispatches[0].bindings[0].base == "arena"
dispatches[0].bindings[0].offset == 0
dispatches[0].bindings[0].length == 576
schedule.is_linear == true
```

- [x] **Step 2: Run the failing tests**

Run:

```bash
python3 -m unittest tests.test_iree_hal_schedule_extractor
```

Expected: import failure for `scripts.extract_iree_hal_schedule`.

- [x] **Step 3: Implement the minimal extractor**

Implement `scripts/extract_iree_hal_schedule.py` with:

- `parse_hal_schedule(path: Path, batch_size: int = 1) -> dict`
- allocation parsing for `hal.device.queue.alloca`
- deallocation parsing for `hal.device.queue.dealloca`
- subspan parsing for `hal.buffer.subspan`
- dispatch parsing for `hal.command_buffer.dispatch`
- constant expression evaluation by reusing the arithmetic style from `scripts/analyze_iree_compile_dump.py`
- closed failure for unresolved SSA values

JSON shape:

```json
{
  "schema": "rrtos.iree_hal_schedule.v1",
  "source": "model.11.hal.mlir",
  "fixed_shape": true,
  "is_linear": true,
  "allocations": [{"id": "arena", "size": 576, "lifetime": "transient"}],
  "dispatches": [{
    "ordinal": 1,
    "workgroups": [1, 1, 1],
    "constants": [],
    "bindings": [{"ordinal": 0, "base": "arena", "offset": 0, "length": 576, "access": "rw"}]
  }],
  "max_transient_allocation_bytes": 576,
  "peak_transient_bytes": 616
}
```

- [x] **Step 4: Run extractor tests**

Run:

```bash
python3 -m unittest tests.test_iree_hal_schedule_extractor
```

Expected: all tests pass.

---

### Task 2: Cross-Check Stream, HAL, And Native Export Metadata

**Files:**
- Modify: `scripts/extract_iree_hal_schedule.py`
- Modify: `tests/test_iree_hal_schedule_extractor.py`

- [x] **Step 1: Add tests for cross-check failures**

Add tests that fail when:

- `.7.stream.mlir` max transient alloca is smaller than `.11.hal.mlir` max transient alloca.
- native executable export count is lower than the largest dispatch ordinal plus one.
- a dispatch binding points at an unknown allocation/subspan.
- a dispatch uses unresolved dynamic dimensions.

- [x] **Step 2: Implement cross-check options**

Add CLI options:

```bash
python3 scripts/extract_iree_hal_schedule.py \
  --hal build/iree_dumps/miniresnet/phases/miniresnetv1_s1_64x50_tl_int8.11.hal.mlir \
  --stream build/iree_dumps/miniresnet/phases/miniresnetv1_s1_64x50_tl_int8.7.stream.mlir \
  --object apps/mnist_app/generated/st_mnist_28.o \
  --output build/iree_dumps/miniresnet/hal_schedule.json
```

Extractor behavior:

- parse `.7.stream.mlir` with existing allocation scanner for attribution
- parse `.11.hal.mlir` as the schedule source
- use `llvm-nm` or `${CMAKE_NM}` if provided to count `*_linked_library_query` and dispatch-related symbols
- emit a warning section for stream/HAL differences
- return nonzero on unknown binding, unsupported dynamic dimension, or export count mismatch

- [x] **Step 3: Run tests**

Run:

```bash
python3 -m unittest tests.test_iree_hal_schedule_extractor tests.test_iree_compile_dump_analyzer
```

Expected: all tests pass.

---

### Task 3: Static-Direct Descriptor Generator

**Files:**
- Create: `scripts/generate_static_direct_descriptor.py`
- Create: `tests/test_static_direct_descriptor_generator.py`

- [x] **Step 1: Write generator tests**

Use a tiny `hal_schedule.json` fixture and assert generated C contains:

```c
const ai_static_direct_dispatch_t test_model_dispatches[] = {
```

and a binding slice with:

```c
.base = AI_STATIC_DIRECT_BUFFER_ARENA,
.offset = 0u,
.length = 576u,
```

- [x] **Step 2: Implement generator**

CLI:

```bash
python3 scripts/generate_static_direct_descriptor.py \
  --schedule build/iree_dumps/miniresnet/hal_schedule.json \
  --model-name st_mnist_28 \
  --entry predict \
  --header apps/mnist_app/generated/st_mnist_28.h \
  --object apps/mnist_app/generated/st_mnist_28.o \
  --output-c build/generated/st_mnist_28_static_direct_desc.c \
  --output-h build/generated/st_mnist_28_static_direct_desc.h
```

Generated descriptor must include:

- model name and entry name
- input/output byte sizes
- const buffer byte size
- transient arena size and alignment
- dispatch ordinal, workgroups, constants
- binding base, offset, length, access
- unsupported-feature flags set to zero for accepted models

- [x] **Step 3: Run tests**

Run:

```bash
python3 -m unittest tests.test_static_direct_descriptor_generator
```

Expected: all tests pass.

Implementation status on 2026-05-06:

- Extractor handles the real MiniResNet HAL dump's batch-1 path, including `cf.cond_br`,
  `cf.br`, block-argument buffer aliases, parenthesized HAL bindings, external input
  buffers, immutable global constant buffers, integer typed constants, casts, shifts,
  compares, selects, division, unsigned ceil-div, and iterative `util.align` values.
- `peak_transient_bytes` is live HAL transient peak. `max_transient_allocation_bytes`
  is the largest single transient allocation and is the correct comparison target for
  the existing `.7.stream.mlir` max-allocation scanner.
- Verified MiniResNet batch-1 extraction:
  `dispatch_count=33`, `allocation_count=14`,
  `max_transient_allocation_bytes=315264`, `peak_transient_bytes=351104`.
- Verified descriptor generation from that schedule:
  `input_bytes=3200`, `const_bytes=130240`, `arena_bytes=351104`,
  `dispatch_count=33`.
- Re-verified on 2026-05-07 across every current `.11.hal.mlir` under `build/`:
  14/14 HAL dumps parsed successfully with HAL-only peak extraction.
  MiniResNet live peaks were either `351104` bytes for baseline/minpeak/data-tiling
  paths or `336768` bytes for stream-debug/static-ukernel size-compare paths.
  The simple BE-U1000 hello static path reported `1220` bytes live peak.

---

### Task 4: Generic Static-Direct Runner

**Files:**
- Create: `ai/include/ai_static_direct.h`
- Create: `ai/src/ai_static_direct_runner.c`
- Modify: root or AI CMake file that builds `rv_aios_ai` only if this runner is placed under `ai/`

- [ ] **Step 1: Define the runner ABI**

Add public structs:

```c
typedef enum {
    AI_STATIC_DIRECT_BUFFER_INPUT = 0,
    AI_STATIC_DIRECT_BUFFER_OUTPUT = 1,
    AI_STATIC_DIRECT_BUFFER_CONST = 2,
    AI_STATIC_DIRECT_BUFFER_ARENA = 3,
} ai_static_direct_buffer_base_t;

typedef struct {
    ai_static_direct_buffer_base_t base;
    uint32_t index;
    uint32_t offset;
    uint32_t length;
    uint32_t access;
} ai_static_direct_slice_t;

typedef struct {
    uint32_t ordinal;
    uint32_t workgroup_x;
    uint32_t workgroup_y;
    uint32_t workgroup_z;
    const uint32_t *constants;
    uint16_t constant_count;
    const ai_static_direct_slice_t *bindings;
    uint8_t binding_count;
} ai_static_direct_dispatch_t;
```

- [ ] **Step 2: Implement runner checks**

`ai_static_direct_run()` must:

- query native executable library
- reject missing exports
- verify descriptor dispatch count does not exceed export count
- verify export attrs `constant_count`, `binding_count`, and `local_memory_pages`
- range-check every binding against input/output/const/arena lengths
- build `binding_ptrs[]` as `base + offset`
- build `binding_lengths[]` as `length`
- run workgroups synchronously in x/y/z nested loops
- track `arena_peak_bytes` as descriptor arena size, not heap peak

- [ ] **Step 3: Add runner unit tests where host build support exists**

If existing host unit test wiring can link this file, add tests for:

- unknown base rejection
- out-of-range slice rejection
- export count mismatch rejection
- nonzero local memory rejection

If host linkage is not available, add compile-only target and cover the behavior through QEMU in Task 5.

---

### Task 5: MNIST Descriptor Validation Lane

**Files:**
- Create: `apps/mnist_app/src/static_direct_descriptor_validation_main.c`
- Create: `scripts/run_mnist_static_direct_descriptor_validation.py`
- Modify: `apps/mnist_app/CMakeLists.txt`
- Modify: `pixi.toml`
- Modify: `tests/test_mnist_static_direct_validation.py` or create `tests/test_mnist_static_direct_descriptor_validation.py`

- [ ] **Step 1: Add generated descriptor to build**

Add a CMake custom command that:

1. ensures MNIST phase dumps exist
2. runs `extract_iree_hal_schedule.py`
3. runs `generate_static_direct_descriptor.py`
4. compiles descriptor C next to `st_mnist_28.o`

- [ ] **Step 2: Add descriptor validation app**

The app must:

- use the generic runner
- use the generated MNIST descriptor
- use the same sample as the handwritten static-direct lane
- print:

```text
MNIST_STATIC_DIRECT_DESC_PROBE_PASS exports=3 dispatches=3
MNIST_STATIC_DIRECT_DESC_STATIC_RAM: input_bytes=3136 arena_bytes=576 output_bytes=40 task_stack_bytes=65536
MNIST_STATIC_DIRECT_DESC_MODEL_PEAK: heap_peak_bytes=0 arena_peak_bytes=576
MNIST_STATIC_DIRECT_DESC_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_hex=0x3f7f6fb2 latency_ticks=<n>
MNIST_STATIC_DIRECT_DESC_PASS count=1
```

- [ ] **Step 3: Add validation script**

Script command:

```bash
python3 scripts/run_mnist_static_direct_descriptor_validation.py \
  --kernel build/apps/mnist_app/mnist_static_direct_descriptor_validation \
  --log logs/mnist_static_direct_descriptor_validation_qemu.log
```

Pass conditions:

- no `FAIL` token
- result argmax matches handwritten static-direct
- `arena_peak_bytes=576`
- `heap_peak_bytes=0`
- dispatch count is 3

- [ ] **Step 4: Run QEMU and map gates**

Run:

```bash
pixi run -e rv32 configure
pixi run -e rv32 build-mnist-static-direct
cmake --build build --target mnist_static_direct_descriptor_validation
python3 scripts/run_mnist_static_direct_descriptor_validation.py
python3 scripts/check_no_iree_vm_symbols.py --map build/apps/mnist_app/mnist_static_direct_descriptor_validation.map
```

Expected: validation passes and map gate contains no VM/HAL runtime symbols beyond the native executable ABI header types.

---

### Task 6: Oracle Comparison Report

**Files:**
- Create: `scripts/compare_static_direct_oracle.py`
- Modify: `docs/AI_IREE_STATIC_DIRECT_PROBE.md`

- [ ] **Step 1: Implement comparison script**

Compare:

- VM/HAL MNIST output log
- handwritten static-direct output log
- descriptor static-direct output log
- HAL schedule JSON

Report:

```text
model=st_mnist_28
vm_hal_result=pass
handwritten_static_direct_result=pass
descriptor_static_direct_result=pass
hal_peak_bytes=<unknown for MNIST until HAL trace is added>
descriptor_arena_peak_bytes=576
heap_peak_bytes_vm_hal=<from log>
heap_peak_bytes_static_direct=0
dispatches_match=true
outputs_match=true
```

- [ ] **Step 2: Document metric separation**

Update the static-direct probe doc with a table that separates:

- image footprint
- RTOS heap peak
- HAL allocation peak
- static-direct arena peak
- CLINT ticks / VM latency metrics

---

### Task 7: MiniResNet Descriptor Probe After MNIST

**Files:**
- Modify: `scripts/extract_iree_hal_schedule.py`
- Add generated descriptor files under `build/`, not source, unless a checked-in fixture is needed
- Add tests only after MNIST descriptor lane passes

- [ ] **Step 1: Run extractor on MiniResNet baseline and stream_debug**

Run:

```bash
python3 scripts/extract_iree_hal_schedule.py \
  --hal build/iree_dumps/miniresnet/phases/miniresnetv1_s1_64x50_tl_int8.11.hal.mlir \
  --stream build/iree_dumps/miniresnet/phases/miniresnetv1_s1_64x50_tl_int8.7.stream.mlir \
  --output build/iree_dumps/miniresnet/hal_schedule.json

python3 scripts/extract_iree_hal_schedule.py \
  --hal build/iree_dumps/miniresnet_stream_debug/phases/miniresnetv1_s1_64x50_tl_int8.11.hal.mlir \
  --stream build/iree_dumps/miniresnet_stream_debug/phases/miniresnetv1_s1_64x50_tl_int8.7.stream.mlir \
  --output build/iree_dumps/miniresnet_stream_debug/hal_schedule.json
```

Expected:

- baseline peak transient is `315264`
- stream_debug peak transient is `300928`
- extractor either emits a valid linear schedule or fails with an explicit unsupported feature reason

- [ ] **Step 2: Do not run MiniResNet static-direct until unsupported features are resolved**

If MiniResNet schedule contains subspan, dynamic dim, local memory, or non-linear fence forms not supported by the runner, stop at schedule extraction and document the blocker. Do not handwrite a MiniResNet dispatch table.

---

### Task 8: Compiler-Pass Export Only After Descriptor Replay Works

**Files:**
- Modify: `third_party/iree/compiler/src/iree/compiler/Dialect/Stream/Transforms/LayoutSlices.cpp`
- Modify: `third_party/iree/compiler/src/iree/compiler/Dialect/Stream/Transforms/Passes.td`
- Create: `tests/test_iree_static_direct_plan_export.py`

- [ ] **Step 1: Add no-op design gate first**

Before editing IREE, write a golden diff test with a checked-in JSON fixture named `tests/fixtures/mnist_hal_schedule.json`. The test compares that fixture against `extract_iree_hal_schedule.py` output for MNIST and asserts equality for dispatch ordinals, workgroups, binding offsets/lengths, and transient peak.

- [ ] **Step 2: Add compiler emission at the correct point**

The target point is inside `LayoutSlices.cpp` after offsets are assigned and before `packOp.erase()`. At that point, slice offsets are final and `ResourcePackOp` still has enough slice structure to preserve lifetime attribution.

- [ ] **Step 3: Require oracle equality**

The compiler-emitted plan is accepted only if it matches the HAL oracle for:

- dispatch count
- dispatch ordinal order
- workgroup counts
- binding base/offset/length
- transient peak
- fixed-shape constraints

Any mismatch fails CI.

---

## Verification Summary

Run before claiming completion:

```bash
python3 -m unittest \
  tests.test_iree_compile_dump_analyzer \
  tests.test_iree_miniresnet_compile_variant_sweep \
  tests.test_iree_hal_schedule_extractor \
  tests.test_static_direct_descriptor_generator

pixi run -e rv32 configure
pixi run -e rv32 validate-mnist-runtime
pixi run -e rv32 validate-mnist-static-direct-runtime
python3 scripts/run_mnist_static_direct_descriptor_validation.py
python3 scripts/check_no_iree_vm_symbols.py --map build/apps/mnist_app/mnist_static_direct_descriptor_validation.map
python3 scripts/compare_static_direct_oracle.py
```

Completion requires all checks passing and the report showing output equivalence between VM/HAL and descriptor static-direct.
