# Supported Matrix

This file is the normative support-status registry for the repository.

## Supported

- Board: `qemu_virt` with the canonical AI registry path via `apps/mnist_app`
- Board: `be_u1000` with the canonical board entrypoint `apps/be_u1000_demo`
- AI API: `ai/include/ai_model_registry.h`
- Validation lanes:
  - `pixi run -e rv32 validate-supported-rv32` as the normative supported rv32 gate
  - `pixi run -e rv32 validate-mnist-runtime-dual` as an optional cross-platform AI observation lane
  - `pixi run -e be-u1000 validate-supported` as the normative supported BE-U1000 gate

## Experimental

- Legacy AI API: `ai/include/ai_runtime.h`
- Legacy demo app: `apps/ai_demo`
- Incomplete SMP enablement outside the declared supported matrix
- Historical RV64 and model-specific simulation workflows not covered by current automation

## Rules

- Supported paths must build in automation and have a documented validation lane.
- Experimental paths may stay in-tree, but they are not the main evidence of correctness.
- Optional observation or comparison lanes do not promote a path to supported status.
- New supported boards or AI entrypoints must update this file and the validation matrix together.
