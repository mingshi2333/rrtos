# RRTOS AI Ukernel RISC-V32 V0 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a minimal project-owned RV32 TinyML ukernel catalog that can be built as LLVM bitcode and later used by static-direct TransformIR experiments.

**Architecture:** Keep the first slice independent of IREE pass changes and existing dirty CMake files. Add a small freestanding C API for fixed-shape tile kernels, a shell build helper that emits `.bc` and `.o`, and source-contract tests that lock the exported kernel names and compile command.

**Tech Stack:** C11, Clang LLVM bitcode, RV32 `rv32imafc_zifencei_zicsr` / `ilp32f`, Python unittest source checks, shell build helper.

---

### File Structure

- `ai/ukernel/rrtos_ai_ukernel.h`: public C ABI for project-owned TinyML tile kernels.
- `ai/ukernel/rrtos_ai_ukernel.c`: freestanding scalar reference implementation for the first kernel set.
- `scripts/build_rrtos_ai_ukernel.sh`: reproducible Clang command for `.bc` and `.o`.
- `tests/test_rrtos_ai_ukernel.py`: source-contract and local build tests.

This first slice intentionally avoids `ai/CMakeLists.txt`, `pixi.toml`, and static-direct generator files because the current worktree already has unrelated edits there. Integration into CMake/package generation is a follow-up task after the bitcode artifact is proven.

### Task 1: Public Ukernel ABI

**Files:**
- Create: `ai/ukernel/rrtos_ai_ukernel.h`
- Test: `tests/test_rrtos_ai_ukernel.py`

- [x] **Step 1: Write source-contract test**

Create `tests/test_rrtos_ai_ukernel.py` with checks for the public ABI:

```python
import pathlib
import unittest


PROJECT_ROOT = pathlib.Path(__file__).resolve().parents[1]


class RrtosAiUkernelTest(unittest.TestCase):
    def test_header_exports_v0_kernel_catalog(self):
        header = (PROJECT_ROOT / "ai" / "ukernel" / "rrtos_ai_ukernel.h").read_text()
        self.assertIn("RRTOS_AI_UKERNEL_ABI_VERSION 1u", header)
        self.assertIn("rrtos_ai_ukernel_status_t", header)
        self.assertIn("rrtos_ai_ukernel_conv2d_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_requant_i32_to_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_maxpool_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_add_i8_tile", header)
        self.assertIn("rrtos_ai_ukernel_clamp_i8", header)
```

- [x] **Step 2: Run test to verify it fails**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel.RrtosAiUkernelTest.test_header_exports_v0_kernel_catalog
```

Expected: FAIL because `ai/ukernel/rrtos_ai_ukernel.h` does not exist.

- [x] **Step 3: Add header**

Create `ai/ukernel/rrtos_ai_ukernel.h` with a stable freestanding C ABI:

```c
#ifndef RRTOS_AI_UKERNEL_H
#define RRTOS_AI_UKERNEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RRTOS_AI_UKERNEL_ABI_VERSION 1u

typedef enum rrtos_ai_ukernel_status {
    RRTOS_AI_UKERNEL_OK = 0,
    RRTOS_AI_UKERNEL_INVALID_ARGUMENT = -1,
} rrtos_ai_ukernel_status_t;

typedef struct rrtos_ai_ukernel_conv2d_i8_tile_params {
    int32_t input_h;
    int32_t input_w;
    int32_t input_c;
    int32_t output_h;
    int32_t output_w;
    int32_t output_c;
    int32_t kernel_h;
    int32_t kernel_w;
    int32_t stride_h;
    int32_t stride_w;
    int32_t pad_top;
    int32_t pad_left;
    int32_t tile_row_begin;
    int32_t tile_row_count;
    int32_t input_zero_point;
    int32_t weight_zero_point;
} rrtos_ai_ukernel_conv2d_i8_tile_params_t;

typedef struct rrtos_ai_ukernel_requant_i32_to_i8_params {
    int32_t output_zero_point;
    int32_t activation_min;
    int32_t activation_max;
} rrtos_ai_ukernel_requant_i32_to_i8_params_t;

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_tile(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    int32_t *accumulator,
    const rrtos_ai_ukernel_conv2d_i8_tile_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_requant_i32_to_i8_tile(
    const int32_t *input,
    int8_t *output,
    int32_t count,
    int32_t multiplier,
    int32_t shift,
    const rrtos_ai_ukernel_requant_i32_to_i8_params_t *params);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_maxpool_i8_tile(
    const int8_t *input,
    int8_t *output,
    int32_t input_h,
    int32_t input_w,
    int32_t channels,
    int32_t pool_h,
    int32_t pool_w,
    int32_t stride_h,
    int32_t stride_w,
    int32_t tile_row_begin,
    int32_t tile_row_count);

rrtos_ai_ukernel_status_t rrtos_ai_ukernel_add_i8_tile(
    const int8_t *lhs,
    const int8_t *rhs,
    int8_t *output,
    int32_t count,
    int32_t activation_min,
    int32_t activation_max);

void rrtos_ai_ukernel_clamp_i8(
    int8_t *data,
    int32_t count,
    int32_t activation_min,
    int32_t activation_max);

#ifdef __cplusplus
}
#endif

#endif /* RRTOS_AI_UKERNEL_H */
```

- [x] **Step 4: Run test to verify it passes**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel.RrtosAiUkernelTest.test_header_exports_v0_kernel_catalog
```

Expected: PASS.

### Task 2: Scalar Reference Kernels

**Files:**
- Create: `ai/ukernel/rrtos_ai_ukernel.c`
- Modify: `tests/test_rrtos_ai_ukernel.py`

- [x] **Step 1: Add source-contract test for implementation**

Append this test to `RrtosAiUkernelTest`:

```python
    def test_source_is_freestanding_and_exports_kernel_functions(self):
        source = (PROJECT_ROOT / "ai" / "ukernel" / "rrtos_ai_ukernel.c").read_text()
        self.assertIn('#include "rrtos_ai_ukernel.h"', source)
        self.assertNotIn("#include <stdio.h>", source)
        self.assertNotIn("#include <stdlib.h>", source)
        self.assertIn("static int8_t saturate_i8", source)
        self.assertIn("static int32_t multiply_by_quantized_multiplier", source)
        self.assertIn("rrtos_ai_ukernel_conv2d_i8_tile(", source)
        self.assertIn("rrtos_ai_ukernel_requant_i32_to_i8_tile(", source)
        self.assertIn("rrtos_ai_ukernel_maxpool_i8_tile(", source)
```

- [x] **Step 2: Run test to verify it fails**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel.RrtosAiUkernelTest.test_source_is_freestanding_and_exports_kernel_functions
```

Expected: FAIL because `ai/ukernel/rrtos_ai_ukernel.c` does not exist.

- [x] **Step 3: Add scalar implementation**

Create `ai/ukernel/rrtos_ai_ukernel.c`. Implement only deterministic scalar kernels. Use no heap, no libc I/O, and no global mutable state. The conv kernel writes an i32 accumulator tile; requant is a separate step so tile search can choose whether to materialize or immediately consume the accumulator.

- [x] **Step 4: Run tests**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel
```

Expected: PASS.

### Task 3: RV32 Bitcode Build Helper

**Files:**
- Create: `scripts/build_rrtos_ai_ukernel.sh`
- Modify: `tests/test_rrtos_ai_ukernel.py`

- [x] **Step 1: Add build-script contract test**

Append this test to `RrtosAiUkernelTest`:

```python
    def test_build_script_emits_rv32_bitcode_and_object(self):
        script = (PROJECT_ROOT / "scripts" / "build_rrtos_ai_ukernel.sh").read_text()
        self.assertIn("--target=riscv32-unknown-elf", script)
        self.assertIn("-march=${RRTOS_AI_UKERNEL_RISCV_MARCH}", script)
        self.assertIn("-mabi=${RRTOS_AI_UKERNEL_RISCV_ABI}", script)
        self.assertIn("-emit-llvm", script)
        self.assertIn("rrtos_ai_ukernel_riscv32.bc", script)
        self.assertIn("rrtos_ai_ukernel_riscv32.o", script)
```

- [x] **Step 2: Run test to verify it fails**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel.RrtosAiUkernelTest.test_build_script_emits_rv32_bitcode_and_object
```

Expected: FAIL because `scripts/build_rrtos_ai_ukernel.sh` does not exist.

- [x] **Step 3: Add build helper**

Create `scripts/build_rrtos_ai_ukernel.sh` with:

```bash
#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUT_DIR="${1:-${PROJECT_ROOT}/build/rrtos_ai_ukernel}"

CLANG="${CLANG:-clang}"
RRTOS_AI_UKERNEL_RISCV_MARCH="${RRTOS_AI_UKERNEL_RISCV_MARCH:-rv32imafc_zifencei_zicsr}"
RRTOS_AI_UKERNEL_RISCV_ABI="${RRTOS_AI_UKERNEL_RISCV_ABI:-ilp32f}"
SRC="${PROJECT_ROOT}/ai/ukernel/rrtos_ai_ukernel.c"

mkdir -p "${OUT_DIR}"

COMMON_FLAGS=(
  --target=riscv32-unknown-elf
  -march=${RRTOS_AI_UKERNEL_RISCV_MARCH}
  -mabi=${RRTOS_AI_UKERNEL_RISCV_ABI}
  -Oz
  -ffreestanding
  -fno-builtin
  -Wall
  -Wextra
  -Werror
  -I"${PROJECT_ROOT}/ai/ukernel"
)

"${CLANG}" "${COMMON_FLAGS[@]}" -emit-llvm -c "${SRC}" \
  -o "${OUT_DIR}/rrtos_ai_ukernel_riscv32.bc"

"${CLANG}" "${COMMON_FLAGS[@]}" -c "${SRC}" \
  -o "${OUT_DIR}/rrtos_ai_ukernel_riscv32.o"

echo "Wrote ${OUT_DIR}/rrtos_ai_ukernel_riscv32.bc"
echo "Wrote ${OUT_DIR}/rrtos_ai_ukernel_riscv32.o"
```

- [x] **Step 4: Run tests and build helper**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel
bash scripts/build_rrtos_ai_ukernel.sh
llvm-size -A build/rrtos_ai_ukernel/rrtos_ai_ukernel_riscv32.o
```

Expected: unittest passes, `.bc` and `.o` are created, and `llvm-size` reports `.text` sections for the kernel object.

### Task 4: Follow-Up Integration Boundary

**Files:**
- Modify: `docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md`

- [x] **Step 1: Add short integration note**

Add a short note that this ukernel catalog is a sidecar artifact for static-direct TransformIR first. It is not yet an IREE builtin ukernel replacement.

- [x] **Step 2: Verify formatting**

Run:

```bash
rg -n '[[:blank:]]$' docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md docs/superpowers/plans/2026-05-07-rrtos-ai-ukernel-riscv32-v0.md
git diff --check -- docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md docs/superpowers/plans/2026-05-07-rrtos-ai-ukernel-riscv32-v0.md
```

Expected: no output.

### Self-Review

- Spec coverage: the plan creates a project-owned `.bc`-ready kernel catalog and preserves the later IREE-pass migration boundary.
- Placeholder scan: no `TBD` or undefined follow-up implementation steps are required for v0.
- Type consistency: all public functions use `rrtos_ai_ukernel_*` names and freestanding integer types from `<stdint.h>`.
