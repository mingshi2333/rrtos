# Canonical AI Path

- Public API: `ai/include/ai_model_registry.h`
- Supported app entrypoint: `apps/mnist_app`
- Supported FP32 runtime validation target: `build/apps/mnist_app/mnist_validation`
- Quantized MNIST runtime validation target: `build/apps/mnist_app/mnist_quant_validation`
- Generated FP32 artifacts live under `apps/<app>/generated/`; the separated ST MNISTv1 INT8 artifacts live under `apps/mnist_app/generated_quant/`
- Toolchain resolution uses `IREE_TOOLCHAIN_ROOT` when set, otherwise tries the locked env from `iree-version.lock` before falling back to tools on `PATH`
- Model source resolution uses `RRTOS_AI_MODEL_DIR` when set, otherwise resolves relative paths from `ai_models.yaml`
- The current supported `st_mnist_28` source is `third_party/iree/samples/models/mnist.mlir`
- The current quantized MNIST source is `models/st_mnistv1_28_tfs_int8.tflite`, generated from `ai_models_mnist_quant.yaml`
- IREE runtime source is pinned by `third_party/iree` and `iree-version.lock`; the current runtime tag is `v3.1.0`

## Validation fixtures

- Runtime sample set header: `apps/mnist_app/src/mnist_validation_samples.h`
- Fixture generator: `scripts/generate_mnist_validation_samples.py`
- Fixture source dataset:
  - `https://storage.googleapis.com/cvdf-datasets/mnist/t10k-images-idx3-ubyte.gz`
  - `https://storage.googleapis.com/cvdf-datasets/mnist/t10k-labels-idx1-ubyte.gz`

## Expectations

- The supported path uses registry-backed synchronous inference.
- The supported validation entrypoint is `apps/mnist_app/src/validation_main.c`, and it proves the public contract through `ai_model_find_by_name()`, `ai_model_get_input_info()`, `ai_model_get_output_info()`, and `ai_infer_sync()`.
- Generated wrappers such as `ai_st_mnist_28_run()` remain convenience glue and do not replace the public registry contract as the supported proof path.
- The maintained pixi AI task surface is `validate-supported-ai`, `validate-mnist-runtime`, `validate-mnist-quant-runtime`, `observe-mnist-runtime-renode`, and `compare-mnist-runtime-platforms`; redundant composite aliases should not be reintroduced.
- `ai_infer_async()` is a compatibility wrapper and must not be treated as queued execution.
- Generated model descriptors must include model name, tensor metadata, and memory estimate fields when available.
- Legacy `ai_runtime.h` consumers are experimental until migrated to the registry contract.
- QEMU `virt` is the preferred runtime validation lane because it matches the supported `qemu_virt` software path and is faster to automate than Renode.
- Renode remains the preferred lane for BE-U1000 board bring-up, interrupt, and peripheral behavior validation.
- The supported AI gate uses committed multi-sample dataset labels as the pass/fail criterion.
- The validation path normalizes official MNIST byte pixels to the model's expected floating-point input range before inference.
- The quantized validation path shifts official MNIST byte pixels by the INT8 input zero point before inference.
- QEMU validation runners use `-icount shift=0,align=off,sleep=off` so counters advance from guest execution instead of host wall-clock speed. Do not treat `latency_us` as target-board inference time; it is derived from CLINT `mtime`. Use `latency_instructions` for simulator-side relative comparisons, and convert `latency_cycles` with `OS_CFG_CPU_FREQ_HZ` only as a rough target-time estimate.
- A secondary Renode observation lane may compare the canonical AI batch output metrics and hashes against QEMU, but QEMU remains the normative pass/fail gate.

## Code Requirements

- `ai_models.yaml` is the source of truth for the canonical FP32 generated model; `ai_models_mnist_quant.yaml` is the source of truth for the separated quantized MNIST validation model.
- `apps/mnist_app/CMakeLists.txt` should keep the supported app surface to `mnist_app`, `mnist_validation`, `mnist_quant_validation`, and `mnist_preemption_probe` unless the supported matrix is deliberately expanded.
- New supported models must update the relevant `ai_models*.yaml`, generated artifacts under the matching generated directory, validation fixtures, pixi tasks, this document, and `docs/SUPPORTED_MATRIX.md` in the same change.
- `zoo/` conversion helpers are optional tooling. A generated zoo artifact is not supported until it is wired through the registry contract and validation matrix.
- Third-party IREE runtime changes are not the project integration point for model ABI glue; compatibility should live in generated wrappers, registry code, or project-owned helper code.
- CI initializes only the IREE runtime submodule subset needed by this lane: `benchmark`, `cpuinfo`, `flatcc`, and `googletest`.
- Do not reintroduce recursive IREE submodule checkout into the supported CI path; the compiler/GPU nested dependencies are outside the supported runtime surface.
