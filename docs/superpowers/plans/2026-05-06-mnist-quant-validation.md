# Quantized MNIST Validation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a reproducible QEMU validation path for the ST MNISTv1 INT8 model without changing the existing FP32 MNIST baseline.

**Architecture:** Keep the current `apps/mnist_app/generated` FP32 model library untouched. Generate a separate `apps/mnist_app/generated_quant` library with a configurable CMake target name, then add a dedicated `mnist_quant_validation` executable that links only that quantized registry.

**Tech Stack:** IREE v3.1.0 toolchain from `iree-version.lock`, TFLite import, existing RRTOS AI registry, QEMU rv32 validation, Pixi tasks.

---

### Task 1: Codegen Library Target Name

**Files:**
- Modify: `scripts/ai_codegen.py`
- Modify: `tests/test_be_u1000_ai_micro_demo.py`

- [x] Add a `model_library_name` config field to `generate_cmake()`, defaulting to `rv_aios_models`.
- [x] Add a source-contract test that checks `scripts/ai_codegen.py` reads `model_library_name`.
- [x] Run `python3 -m unittest tests.test_be_u1000_ai_micro_demo`.

### Task 2: Quantized Model Source and Generated Registry

**Files:**
- Create: `models/st_mnistv1_28_tfs_int8.tflite`
- Create: `ai_models_mnist_quant.yaml`
- Create: `apps/mnist_app/generated_quant/*`

- [x] Download the ST Model Zoo `st_mnistv1_28_tfs_int8.tflite` source model.
- [x] Add `ai_models_mnist_quant.yaml` with `output_dir: apps/mnist_app/generated_quant` and `model_library_name: rv_aios_mnist_quant_models`.
- [x] Run `IREE_TOOLCHAIN_ROOT=/home/mingshi/.mamba/envs/iree-toolchain310/bin python3 scripts/ai_codegen.py --config ai_models_mnist_quant.yaml`.
- [x] Confirm generated tensor types are INT8 and the generated CMake target is `rv_aios_mnist_quant_models`.

### Task 3: Quantized Validation Target

**Files:**
- Modify: `apps/mnist_app/CMakeLists.txt`
- Create: `apps/mnist_app/src/quant_validation_main.c`
- Create/Modify: `scripts/run_mnist_quant_validation.py`
- Modify: `pixi.toml`

- [x] Add `add_subdirectory(generated_quant)`.
- [x] Add `mnist_quant_validation` linked against `rv_aios_mnist_quant_models`.
- [x] Implement a one-sample QEMU validation task for digit `7`, matching the historical ST MNISTv1 pass condition.
- [x] Add a Pixi task `validate-mnist-quant-runtime`.
- [x] Run build and QEMU validation.

### Task 4: Verification and Reporting

**Files:**
- Logs under `logs/`
- Build reports under `build/apps/mnist_app/`

- [x] Run `pixi run -e rv32 configure`.
- [x] Run `pixi run -e rv32 build`.
- [x] Run `pixi run -e rv32 validate-mnist-runtime`.
- [x] Run `pixi run -e rv32 validate-mnist-quant-runtime`.
- [x] Report FP32 and INT8 latency, pass/fail status, and whether the old failed `zoo_mnist_int8_probe` is still unsupported.
