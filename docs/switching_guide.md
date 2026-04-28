# Target and Validation Guide

This repository currently has two supported working lanes and a smaller set of experimental paths.

## Supported lanes

### 1. Canonical AI lane

Use this when you are working on the supported AI runtime path.

```bash
pixi run -e rv32 configure
pixi run -e rv32 build
pixi run -e rv32 validate-supported-rv32
```

Use QEMU for the normative gate:

- `pixi run -e rv32 validate-mnist-runtime`

Use Renode only for comparison or observation:

- `pixi run -e rv32 observe-mnist-runtime-renode`
- `pixi run -e rv32 compare-mnist-runtime-platforms`

Keep the maintained AI task surface small. Prefer the four commands above and do not reintroduce composite aliases on the supported path.

The supported model in this lane is `st_mnist_28` from `ai_models.yaml`. Historical direct, ST-specific, or zoo-only conversion targets are not part of the supported AI gate unless they are explicitly added to `docs/SUPPORTED_MATRIX.md`.

### 2. Supported BE-U1000 lane

Use this when you are validating the current supported board bring-up path.

```bash
pixi run -e be-u1000 validate-supported
```

This lane covers:

- board build
- interrupt-map consistency
- simulated boot-log validation

## Experimental lanes

The following remain in-tree, but they are not the main evidence of correctness:

- `apps/ai_demo`
- RV64 historical experiments
- older model-specific simulation notes under `docs/reports/`
- historical model-specific Renode scripts under `scripts/simulation/`

## Which simulator to use

- Use `QEMU` for canonical AI runtime regression on `qemu_virt`
- Use `Renode` for BE-U1000 board, interrupt, and peripheral behavior checks
- Use optional `QEMU` vs `Renode` AI comparison only when you want cross-platform confidence on the canonical AI sample batch
