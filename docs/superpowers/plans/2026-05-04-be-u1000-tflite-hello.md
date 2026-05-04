# BE-U1000 TFLite Hello Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Run a real pretrained official TFLite Micro hello_world float model on BE-U1000 through the project IREE static-library runtime path.

**Architecture:** Keep the existing `be_u1000_ai_micro_demo` app surface, replace the synthetic MLIR model with official `hello_world_float.tflite`, generate IREE VM C/static object with the IREE 3.1 TFLite toolchain, and pin the vendored IREE runtime to v3.1.0 so static-library ABI versions match.

**Tech Stack:** C, CMake, TFLite, IREE 3.1 `llvm-cpu` static object generation, Python unittest, Renode.

---

### Task 1: Use Official Pretrained TFLite Model

**Files:**
- Create: `apps/be_u1000_ai_micro_demo/model/hello_world_float.tflite`
- Modify: `ai_models_be_u1000_ai_micro.yaml`
- Modify: `tests/test_be_u1000_ai_micro_demo.py`

- [x] Download TensorFlow Lite Micro `hello_world_float.tflite` into the BE-U1000 AI demo model directory.
- [x] Point `ai_models_be_u1000_ai_micro.yaml` at `be_u1000_hello_world_float`.
- [x] Require `toolchain.tflite_toolchain_env: iree-toolchain310` because the local 3.8 TFLite importer emits incompatible TOSA bytecode for this model.
- [x] Update tests to assert the official `.tflite` file, model name, and BE-U1000 pass token.

### Task 2: Update App Runtime Proof

**Files:**
- Modify: `apps/be_u1000_ai_micro_demo/main.c`

- [x] Use generated `ai_be_u1000_hello_world_float_input_t` and output structs.
- [x] Feed `x=1.0f` using bit construction to avoid Renode compressed-float decode issues in app code.
- [x] Print `x_hex`, `y_hex`, and an expected output range.
- [x] Pass when output is in `[0.75f, 1.0f]`, matching host TFLite output near `0.863`.

### Task 3: Align IREE Runtime ABI

**Files:**
- Modify: `third_party/iree`
- Modify: `iree-version.lock`
- Modify: `docs/AI_CANONICAL_PATH.md`
- Modify: `ai/src/ai_model_registry.c`
- Modify: `tests/test_ai_model_registry_runtime.py`

- [x] Switch vendored IREE runtime to `v3.1.0` / `d2242207764230ad398585a5771f9d54ce91b4c8`.
- [x] Update IREE version lock and canonical docs from `v3.8.0` to `v3.1.0`.
- [x] Adjust registry code for the 3.1 `iree_hal_module_create` signature.
- [x] Use `IREE_WHOLE_BUFFER` via a project compatibility macro.
- [x] Keep IREE compatibility in project-owned registry code; do not carry local patches in `third_party/iree`.

### Task 4: Regenerate And Verify

**Files:**
- Regenerate: `apps/be_u1000_ai_micro_demo/generated/*`

- [x] Run `python3 scripts/ai_codegen.py --config ai_models_be_u1000_ai_micro.yaml`.
- [x] Run `cmake --build build-be_u1000_ai_micro -j`.
- [x] Run `python3 -m unittest tests.test_be_u1000_ai_micro_demo tests.test_be_u1000_pixi_toolchain tests.test_ai_model_registry_runtime`.
- [x] Run `python3 scripts/check_be_u1000_ai_demo_size.py --binary build-be_u1000_ai_micro/rrtos_be_u1000.bin --max-bytes 262144`.
- [x] Run Renode and confirm `BE_U1000_TFLITE_HELLO_DEMO_PASS`.
