# Canonical AI Path

- Public API: `ai/include/ai_model_registry.h`
- Supported app entrypoint: `apps/mnist_app`
- Supported runtime validation target: `build/apps/mnist_app/mnist_validation`
- Generated artifacts live under `apps/<app>/generated/`
- Toolchain resolution uses `IREE_TOOLCHAIN_ROOT` when set, otherwise falls back to tools on `PATH`
- Model source resolution uses `RRTOS_AI_MODEL_DIR` when set, otherwise resolves relative paths from `ai_models.yaml`

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
- The supported AI gate currently uses committed multi-sample output fingerprints as the pass/fail criterion.
- The validation logs also emit dataset-label notes for human review when the current model argmax differs from the sample label.
- A secondary Renode observation lane may compare the canonical AI batch output fingerprint against QEMU, but QEMU remains the normative pass/fail gate.
