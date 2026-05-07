# MiniResNet Fused Ukernel Fast Path Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Move the MiniResNet first-layer patch from a generic conv tile plus separate requant loop to a reusable project ukernel fast path with identical QEMU output and lower latency.

**Architecture:** Keep the project-owned `ai/ukernel` catalog as the ABI boundary. Add a descriptor-friendly fused OHWI requant tile entry point, with a private fast path for the current `int8 ch1 7x7 stride2 pad3 weight-zp-0` first layer. The MiniResNet patch passes a shape/range params struct instead of hand-assembling a generic conv tile and temporary accumulator.

**Tech Stack:** C99, RV32 freestanding clang build, QEMU validation, Python unittest source-contract tests.

---

### Task 1: Source Contracts

**Files:**
- Modify: `tests/test_rrtos_ai_ukernel.py`
- Modify: `tests/test_miniresnet_patch_validation.py`

- [x] **Step 1: Write failing tests**

Add tests that require `rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile()` and `rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t` in the ukernel header/source, reject MiniResNet-specific public symbol names, require `struct_size/abi_version/flags/reserved` fields, and require `miniresnet_patch_first_layer.c` to call the tile entry without `MINIRESNET_PATCH_ROW_ACCUMULATOR_COUNT`.

- [x] **Step 2: Verify red**

Run:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
```

Expected: FAIL because the descriptor tile function is not exported or used yet.

### Task 2: Fused Ukernel Implementation

**Files:**
- Modify: `ai/ukernel/rrtos_ai_ukernel.h`
- Modify: `ai/ukernel/rrtos_ai_ukernel.c`
- Modify: `apps/mnist_app/src/miniresnet_patch_first_layer.c`

- [x] **Step 1: Export the fast path**

Add this function to the public ukernel ABI:

```c
rrtos_ai_ukernel_status_t rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
    const int8_t *input,
    const int8_t *weights,
    const int32_t *bias,
    const float *multipliers,
    int8_t *padded_output,
    const rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_params_t *params);
```

- [x] **Step 2: Implement the fast path**

Validate descriptor-style params including ABI metadata, output bounds, tile row range without signed-overflow-prone addition, activation clamp, and zero points. Dispatch to a private fixed-kernel fast path for MiniResNet-compatible `kernel=7`, `stride=2`, `pad=3`, `input_c=1`, `weights=OHWI`, `weight_zero_point=0`; otherwise use the generic OHWI tile path.

- [x] **Step 3: Wire MiniResNet to the fast path**

Replace the local row accumulator and generic `rrtos_ai_ukernel_conv2d_i8_tile` call with one fused tile call for all first-layer output rows.

### Task 3: Verification

**Files:**
- Update: `logs/miniresnet_patch_first_layer_real_inference_qemu.md`

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

- [x] **Step 4: Run real inference canary**

```bash
python3 scripts/run_miniresnet_patch_validation.py --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation --log logs/miniresnet_patch_validation_qemu_test.log --memory 64M
```

Expected: `MINIRESNET_PATCH_PASS count=3`, output hash `3045847227`, argmax `3`, heap peak `0`.

- [x] **Step 5: Measure image sections**

```bash
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation
```

Record flash-load sections, static RAM sections, stack, and `llvm-size -A Total` separately in KiB so `.bss` is not mistaken for flash payload.
