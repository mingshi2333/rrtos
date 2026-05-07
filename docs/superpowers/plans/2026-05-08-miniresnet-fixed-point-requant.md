# MiniResNet Fixed-Point Requant Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the MiniResNet first-layer fused tile's floating-point requant path with per-channel fixed-point multiplier/shift data while keeping the QEMU output oracle unchanged.

**Architecture:** Keep the existing float fused tile as a comparison/fallback entry point. Add a fixed-point sibling entry point that consumes per-channel `int32_t multiplier` and `int32_t shift`, reuses the scalar `multiply_by_quantized_multiplier()` helper, and make the MiniResNet patch call the fixed-point entry. The fixed-point constants live next to the existing first-layer data so future descriptor generation can emit the same arrays.

**Tech Stack:** C99, RV32 freestanding clang build, QEMU validation, Python unittest source-contract and host oracle tests.

---

### Task 1: Red Tests

**Files:**
- Modify: `tests/test_rrtos_ai_ukernel.py`
- Modify: `tests/test_miniresnet_patch_validation.py`

- [x] **Step 1: Add API/source tests**

Require `rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31()` in the ukernel header/source and require the MiniResNet patch to call it without passing `miniresnet_patch_conv1_multiplier`.

- [x] **Step 2: Add host oracle coverage**

Extend the existing C oracle to compare fixed-point requant outputs against the same quantized multiplier helper used by the ukernel.

- [x] **Step 3: Verify red**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
```

Expected: FAIL because the fixed-point API, data arrays, and caller wiring do not exist yet.

### Task 2: Ukernel Fixed-Point Entry

**Files:**
- Modify: `ai/ukernel/rrtos_ai_ukernel.h`
- Modify: `ai/ukernel/rrtos_ai_ukernel.c`

- [x] **Step 1: Export fixed-point fused tile**

Add:

```c
rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const int32_t *multipliers,
    const int32_t *shifts,
    int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params);
```

- [x] **Step 2: Implement fixed-point paths**

Reuse the same descriptor validation and fast/generic conv loops, replacing `round_float_to_i32((float)acc * multiplier)` with `multiply_by_quantized_multiplier(acc, multipliers[oc], shifts[oc])`.

### Task 3: MiniResNet Data And Wiring

**Files:**
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer_data.h`
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer.c`

- [x] **Step 1: Add per-channel fixed-point constants**

Add `miniresnet_patch_conv1_multiplier_q31[64]` and `miniresnet_patch_conv1_shift[64]` generated from the existing float multipliers using the same q31 quantization convention as `multiply_by_quantized_multiplier()`.

- [x] **Step 2: Wire the patch**

Call `rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31()` with the fixed arrays.

### Task 4: Verification

**Files:**
- Update: `logs/miniresnet_patch_first_layer_real_inference_qemu.md`
- Update: `docs/AI_GRAPH_AWARE_STATIC_DIRECT_RESEARCH.md`

- [x] **Step 1: Run unit tests**

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
```

- [x] **Step 2: Build RV32 ukernel artifact**

```bash
bash scripts/build_rrtos_ai_ukernel.sh
```

- [x] **Step 3: Build QEMU target**

```bash
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
```

- [x] **Step 4: Run QEMU inference**

```bash
python3 scripts/run_miniresnet_patch_validation.py --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation --log logs/miniresnet_patch_validation_qemu_test.log --memory 64M
```

Expected: `MINIRESNET_PATCH_PASS count=3`, hash `3045847227`, argmax `3`, heap peak `0`.

Result: the q31 path is output-correct, but slower than the float descriptor
tile path in the current RV32/QEMU build (`1402113.0` ticks versus
`1341587.7` ticks). Keep the result as evidence for backend/codegen work rather
than as a latency improvement.
