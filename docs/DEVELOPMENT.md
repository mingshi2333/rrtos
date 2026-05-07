# Development

Development should keep the supported matrix small and explicit. Historical
apps, reports, and simulations may stay in the repository, but new supported
behavior must be promoted through code, validation, and documentation together.

## Working Rules

- Use Pixi environments for builds and validation.
- Keep OpenSpec out of the workflow for this repository unless the project owner
  explicitly re-enables it.
- Do not patch `third_party/iree` for project integration glue. Put compatibility
  code in generated wrappers, registry code, or project-owned helpers.
- Do not recursively depend on the full IREE compiler/GPU submodule graph for
  supported CI.
- Keep generated model artifacts under `apps/mnist_app/generated/` for the
  supported AI path.
- Treat `zoo/` as optional conversion tooling until a model is promoted through
  `ai_models.yaml` and validation.

## Repository Areas

| Area | Main paths |
| --- | --- |
| Kernel | `kernel/`, `memory/`, `multicore/` |
| RISC-V arch | `arch/riscv/` |
| Board support | `boards/be_u1000/` |
| HAL | `hal/include/`, `hal/src/` |
| AI runtime | `ai/`, `apps/mnist_app/generated/` |
| Supported apps | `apps/mnist_app/`, `apps/be_u1000_demo/` |
| BE-U1000 observation apps | `apps/be_u1000_*` |
| Validation scripts | `scripts/`, `tests/` |
| Documentation | `README.md`, `docs/`, app-specific READMEs |

## Adding Kernel Behavior

1. Add or update the kernel implementation under `kernel/` or `memory/`.
2. Add host semantic tests under `tests/kernel/`.
3. Run:

```bash
pixi run validate-kernel-semantics
```

4. If the behavior affects supported firmware, also run the supported lane gate:

```bash
pixi run -e rv32 validate-supported-rv32
pixi run -e be-u1000 validate-supported
```

Do not rely only on firmware smoke tests for scheduler, IPC, timer, or memory
semantics. The host semantic harness should encode the behavior directly.

## Adding BE-U1000 HAL Features

1. Add the generic contract under `hal/include/` if one does not already exist.
2. Add the BE-U1000 implementation under `hal/src/be_u1000/`.
3. Wire the feature in `hal/CMakeLists.txt`.
4. Add or update a BE-U1000 app under `apps/be_u1000_*` when runtime evidence is
   needed.
5. Update the `BE_U1000_APP` to HAL feature mapping in `CMakeLists.txt`.
6. Run:

```bash
pixi run -e be-u1000 validate-hal-apps
pixi run -e be-u1000 validate-supported
```

Only the default demo lane is blocking today. HAL app matrix entries remain
observation evidence until promoted in `docs/SUPPORTED_MATRIX.md`.

## Adding AI Models

The supported path currently declares canonical FP32 `st_mnist_28` and the
separated ST MNISTv1 INT8 validation model `st_mnistv1_28_tfs_int8`.

To promote another model:

1. Update the relevant `ai_models*.yaml`.
2. Regenerate model code through `scripts/ai_codegen.py` or the documented model
   conversion workflow.
3. Commit generated artifacts under the matching `apps/mnist_app/generated*/` directory.
4. Add deterministic validation fixtures.
5. Update `docs/AI_CANONICAL_PATH.md` and `docs/SUPPORTED_MATRIX.md`.
6. Run:

```bash
pixi run -e rv32 validate-supported-ai
pixi run -e rv32 validate-supported-rv32
```

Model output must be validated by labels or deterministic expectations, not only
by "runtime did not crash".

## CI Changes

The blocking firmware workflow is:

```text
.github/workflows/firmware-supported-matrix.yml
```

If a change touches build policy, toolchain files, supported apps, HAL, AI,
kernel, tests, scripts, or docs that define the supported matrix, update the
workflow path filters as needed. Toolchain changes under `cmake/**` must trigger
firmware CI.

## Commit Hygiene

Keep commits scoped:

- source changes with their focused tests
- generated artifacts with the generation source and validation update
- docs updates with the code facts they describe
- third-party submodule pointer updates as explicit commits

Do not mix unrelated generated build logs or timestamp-only report churn into a
source fix.
