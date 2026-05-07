# STM32AI Zoo Candidate Sweep Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add and run a repeatable sweep for the STM32AI Model Zoo candidates previously identified, recording which models compile through the current RRTOS/IREE TFLite path and their generated footprint.

**Architecture:** Keep the production MNIST validation app untouched. Add a standalone Python sweep tool that owns a curated candidate manifest, downloads model artifacts into `models/stm32ai_zoo/`, generates one temporary YAML per model under `build/stm32ai_zoo_sweep/`, invokes `scripts/ai_codegen.py`, and writes CSV/Markdown reports. ONNX candidates are included in the manifest but reported as unsupported by the current generator until an ONNX import path is added.

**Tech Stack:** Python 3 standard library, existing `scripts/ai_codegen.py`, IREE TFLite toolchain from `iree-toolchain310`, existing rv32 target defaults.

---

### Task 1: Add The Sweep Tool

**Files:**
- Create: `scripts/sweep_stm32ai_zoo_candidates.py`
- Test: `tests/test_stm32ai_zoo_sweep.py`

- [ ] **Step 1: Create a script with an embedded candidate manifest**

The script must define `ST_ZOO_BASE_URL`, `Candidate`, and `CANDIDATES` for these artifacts:

```text
st_mnistv1_28_tfs_int8.tflite
st_dense_freq_1channel_512_int8.tflite
miniresnetv1_s1_64x50_tl_int8.tflite
yamnet_e256_64x96_tl_int8.tflite
blazeface_front_128_int8.tflite
fdmobilenet_a025_128_tfs_int8.tflite
mobilenetv1_a025_96_int8.tflite
mobilenetv2_a035_128_qdq_w4_85.64%_w8_14.36%_a8_100%_acc_43.53.onnx
```

- [ ] **Step 2: Implement download and YAML generation**

The script must download missing artifacts to `models/stm32ai_zoo/` and write one config per candidate to `build/stm32ai_zoo_sweep/<candidate>/config.yaml` using the same rv32 defaults as `ai_models_mnist_quant.yaml`.

- [ ] **Step 3: Implement compile and footprint collection**

For supported suffixes `.tflite` and `.mlir`, invoke:

```bash
python3 scripts/ai_codegen.py --config build/stm32ai_zoo_sweep/<candidate>/config.yaml
```

Then record source size, generated object size, header size, wrapper source size, and object section totals from `llvm-size` or `size`.

- [ ] **Step 4: Report unsupported formats**

For `.onnx`, do not invoke codegen. Record status `unsupported_source_type` with note `scripts/ai_codegen.py currently supports .tflite and .mlir only`.

### Task 2: Add Unit Tests

**Files:**
- Create: `tests/test_stm32ai_zoo_sweep.py`

- [ ] **Step 1: Test candidate manifest**

Verify every candidate has a stable name, upstream path, filename, suffix, and URL under the official ST repository raw base.

- [ ] **Step 2: Test generated YAML**

Generate a config for one candidate in a temporary directory and assert it contains:

```yaml
defaults:
  target_arch: rv32
  target_abi: ilp32f
toolchain:
  tflite_toolchain_env: iree-toolchain310
models:
  - name: <candidate-name>
```

- [ ] **Step 3: Test ONNX classification**

Verify the mixed-precision MobileNetV2 ONNX candidate returns `unsupported_source_type` without invoking codegen.

### Task 3: Run The Sweep And Verify

**Files:**
- Generated: `models/stm32ai_zoo/*`
- Generated: `build/stm32ai_zoo_sweep/stm32ai_zoo_sweep.csv`
- Generated: `build/stm32ai_zoo_sweep/stm32ai_zoo_sweep.md`

- [ ] **Step 1: Run unit tests**

```bash
python3 -m unittest tests.test_stm32ai_zoo_sweep
```

Expected: PASS.

- [ ] **Step 2: Run sweep**

```bash
python3 scripts/sweep_stm32ai_zoo_candidates.py
```

Expected: all `.tflite` candidates are downloaded and attempted; ONNX is reported unsupported.

- [ ] **Step 3: Run focused regression**

```bash
python3 -m unittest tests.test_stm32ai_zoo_sweep tests.test_ai_mnist_validation_scripts tests.test_ai_model_registry_runtime
```

Expected: PASS.

- [ ] **Step 4: Summarize results**

Report candidate statuses, object sizes, and any compile failures with the exact failing stage.
