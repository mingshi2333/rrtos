# Canonical AI Model Codegen Workflow

This document describes the current supported model-codegen workflow used by the canonical AI path in `rrtos`.

## Scope

This workflow is for the supported RV32 registry-backed AI path used by `apps/mnist_app`.

It is not a promise that every historical RV64 or model-specific experiment under `zoo/` is still part of the supported matrix.

## Inputs

- Model manifest: `ai_models.yaml`
- Code generator: `scripts/ai_codegen.py`
- EmitC helper script: `zoo/scripts/tflite_to_iree_c.sh`
- Generated app artifacts: `apps/<app>/generated/`

## Toolchain resolution

The canonical flow resolves tools in this order:

1. `IREE_TOOLCHAIN_ROOT` if set
2. the locked mamba env from `iree-version.lock`
3. tool names from `PATH`

Model source files resolve in this order:

1. `RRTOS_AI_MODEL_DIR` if set
2. paths relative to `ai_models.yaml`

## Generate artifacts

```bash
pixi run -e rv32 ai-gen
```

This regenerates the registry-backed wrappers for the models declared in `ai_models.yaml`.

If `IREE_TOOLCHAIN_ROOT` is unset, `scripts/ai_codegen.py` will automatically try the locked toolchain env named in `iree-version.lock` before falling back to `PATH`.

## Build and validate

```bash
pixi run -e rv32 configure
pixi run -e rv32 build
pixi run -e rv32 validate-mnist-runtime
```

Optional cross-platform observation:

```bash
pixi run -e rv32 observe-mnist-runtime-renode
pixi run -e rv32 compare-mnist-runtime-platforms
```

## Current supported example

- Model name: `st_mnist_28`
- App: `apps/mnist_app`
- Runtime gate: `build/apps/mnist_app/mnist_validation`
- Validation fixture: `apps/mnist_app/src/mnist_validation_samples.h`
