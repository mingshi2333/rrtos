# MiniResNet Runtime Validation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Link and run the STM32AI Zoo `miniresnetv1_s1_64x50_tl_int8` model in the existing rv32/QEMU AI runtime path, collecting image size, peak heap, cycles, and instruction count.

**Architecture:** Keep the canonical FP32 and quantized MNIST validation artifacts unchanged. Generate a separate `apps/mnist_app/generated_miniresnet` registry library, add one experimental `miniresnet_validation` executable, and drive it with a dedicated QEMU log checker. The validation uses deterministic synthetic int8 input because the goal is runtime footprint/latency, not ESC10 accuracy.

**Tech Stack:** Existing IREE TFLite codegen, RRTOS AI registry API, QEMU rv32 virt, Python unittest.

---

### Task 1: Add MiniResNet Codegen Config

**Files:**
- Create: `ai_models_stm32ai_miniresnet.yaml`

- [ ] Point the model to `models/stm32ai_zoo/miniresnetv1_s1_64x50_tl_int8.tflite`.
- [ ] Use the rv32 `ilp32f` size-focused defaults already used by `ai_models_mnist_quant.yaml`.
- [ ] Write generated artifacts to `apps/mnist_app/generated_miniresnet`.

### Task 2: Add Runtime Validation Target

**Files:**
- Modify: `apps/mnist_app/CMakeLists.txt`
- Create: `apps/mnist_app/src/miniresnet_validation_main.c`

- [ ] Add `generated_miniresnet` as an independent generated registry library.
- [ ] Add `miniresnet_validation` linked only against `rv_aios_miniresnet_models`.
- [ ] In the validation task, assert tensor metadata is input `[1,64,50,1] int8` and output `[1,10] fp32`.
- [ ] Emit `MINIRESNET_MODEL_PEAK` for `init` and `invoke`.
- [ ] Emit `MINIRESNET_RESULT` with `argmax`, `hash`, `top_score_q`, `latency_us`, `latency_cycles`, `latency_instructions`, and `total`.
- [ ] Emit `MINIRESNET_PASS count=1`.

### Task 3: Add QEMU Runner And Tests

**Files:**
- Create: `scripts/run_miniresnet_validation.py`
- Modify: `tests/test_ai_mnist_validation_scripts.py`

- [ ] Validate pass/result/peak tokens and reject fail tokens.
- [ ] Run QEMU with `-icount shift=0,align=off,sleep=off`.

### Task 4: Generate, Build, Run, And Report

**Commands:**

```bash
python3 scripts/ai_codegen.py --config ai_models_stm32ai_miniresnet.yaml
pixi run -e rv32 configure
pixi run -e rv32 build
python3 scripts/run_miniresnet_validation.py --kernel build/apps/mnist_app/miniresnet_validation --log logs/miniresnet_validation_qemu.log
python3 -m unittest tests.test_ai_mnist_validation_scripts tests.test_stm32ai_zoo_sweep tests.test_ai_model_registry_runtime
git diff --check
```

Expected: build succeeds, QEMU log contains `MINIRESNET_PASS count=1`, and tests pass.
