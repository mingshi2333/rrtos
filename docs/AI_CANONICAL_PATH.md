# Canonical AI Path

- Public API: `ai/include/ai_model_registry.h`
- Supported app entrypoint: `apps/mnist_app`
- Supported runtime validation target: `build/apps/mnist_app/mnist_validation`
- Generated artifacts live under `apps/<app>/generated/`
- Toolchain resolution uses `IREE_TOOLCHAIN_ROOT` when set, otherwise tries the locked env from `iree-version.lock` before falling back to tools on `PATH`
- Model source resolution uses `RRTOS_AI_MODEL_DIR` when set, otherwise resolves relative paths from `ai_models.yaml`
- The current supported `st_mnist_28` source is `third_party/iree/samples/models/mnist.mlir`

## Validation fixtures

- Runtime sample set header: `apps/mnist_app/src/mnist_validation_samples.h`
- Fixture generator: `scripts/generate_mnist_validation_samples.py`
- Fixture source dataset:
  - `https://storage.googleapis.com/cvdf-datasets/mnist/t10k-images-idx3-ubyte.gz`
  - `https://storage.googleapis.com/cvdf-datasets/mnist/t10k-labels-idx1-ubyte.gz`

## Expectations

- The supported path uses registry-backed synchronous inference.
- `ai_infer_async()` is a compatibility wrapper and must not be treated as queued execution.
- Generated model descriptors must include model name, tensor metadata, and memory estimate fields when available.
- Legacy `ai_runtime.h` consumers are experimental until migrated to the registry contract.
- QEMU `virt` is the preferred runtime validation lane because it matches the supported `qemu_virt` software path and is faster to automate than Renode.
- Renode remains the preferred lane for BE-U1000 board bring-up, interrupt, and peripheral behavior validation.
- The supported AI gate uses committed multi-sample dataset labels as the pass/fail criterion.
- The validation path normalizes official MNIST byte pixels to the model's expected floating-point input range before inference.
- A secondary Renode observation lane may compare the canonical AI batch output metrics and hashes against QEMU, but QEMU remains the normative pass/fail gate.
