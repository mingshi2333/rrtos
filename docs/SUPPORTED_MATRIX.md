# Supported Matrix

This file is the normative support-status registry for the repository. The repository is presented first as an RTOS with explicit firmware lanes; AI and board-specific extensions are documented within that lane structure.

## Supported

- Firmware lanes:
  - `qemu_virt` with the canonical RTOS validation entrypoint `apps/mnist_app`
  - `be_u1000` with the canonical RTOS board entrypoint `apps/be_u1000_demo` (current EVU-BA-2.3-shaped board path)
- Supported AI extension within the `qemu_virt` lane:
  - API: `ai/include/ai_model_registry.h`
  - App: `apps/mnist_app`
  - Model: `st_mnist_28` from `ai_models.yaml`
  - Proof path: `apps/mnist_app/src/validation_main.c` uses the registry contract directly; generated wrappers remain convenience-only glue
- Validation lanes:
  - `pixi run -e rv32 validate-supported-rv32` as the normative supported rv32 gate
  - `pixi run -e rv32 validate-supported-ai` as the canonical AI contract check before the full supported rv32 gate
  - `pixi run -e rv32 observe-mnist-runtime-renode` and `pixi run -e rv32 compare-mnist-runtime-platforms` as optional cross-platform AI observation lanes
  - `pixi run -e be-u1000 validate-supported` as the normative supported BE-U1000 gate
  - `pixi run -e be-u1000 validate-hal-apps` as an explicit BE-U1000 HAL app matrix observation lane; it is not part of the blocking supported gate until promoted here
- HAL configuration:
  - `RRTOS_HAL_FEATURES=auto` maps each `BE_U1000_APP` to an explicit lightweight HAL feature set
  - BE-U1000 non-AI lanes use `OS_AI_EN=OFF`; the supported RV32 AI lane uses `OS_AI_EN=ON`
  - The supported BE-U1000 lane validates `RISCV_MARCH=rv32imafc_zifencei`, generated compile flags containing `-march=rv32imafc_zifencei_zicsr`, and `RISCV_MABI=ilp32f` so it cannot drift back to the generic RV32 `rv32imafd/ilp32d` defaults.

## Experimental

- Legacy AI API: `ai/include/ai_runtime.h`
- Legacy demo app: `apps/ai_demo`
- Incomplete SMP enablement outside the declared supported firmware lanes
- Historical RV64 and model-specific simulation workflows not covered by current automation
- Optional `zoo/` model conversion artifacts until they are promoted through `ai_models.yaml`, the registry contract, and a validation lane
- BE-U1000 HAL app matrix entries outside `apps/be_u1000_demo`; these are useful regression and bring-up evidence, but they remain observation lanes until promoted

## Rules

- Supported firmware lanes must build in automation and have a documented validation lane.
- Experimental paths may stay in-tree, but they are not the main evidence of correctness.
- Optional observation or comparison lanes do not promote a path to supported status.
- New supported boards or AI entrypoints must update this file and the validation matrix together.
- New supported AI models must update `ai_models.yaml`, generated artifacts, deterministic runtime validation, and the maintained pixi task surface together.
