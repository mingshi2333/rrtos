# rrtos

`rrtos` is a bare-metal RISC-V RTOS repository with one supported AI runtime path and one supported board bring-up path.

## Supported paths

- `qemu_virt` + `apps/mnist_app` + `ai/include/ai_model_registry.h`
- `be_u1000` + `apps/be_u1000_demo`
- Supported validation:
  - `pixi run -e rv32 validate-supported-rv32`
  - `pixi run -e be-u1000 validate-supported`

## Quick start

### 1. Initialize dependencies

```bash
pixi install -e rv32
pixi install -e be-u1000
pixi run init-iree
```

### 2. Validate the canonical AI path

```bash
pixi run -e rv32 validate-supported-rv32
```

This builds the supported RV32 path and runs the canonical QEMU AI runtime validation for `apps/mnist_app`.

### 3. Validate the supported BE-U1000 lane

```bash
pixi run -e be-u1000 validate-supported
```

This builds the board image and runs the supported interrupt and boot-log checks.

## AI runtime validation

The canonical AI runtime validation target is `build/apps/mnist_app/mnist_validation`.

Useful commands:

```bash
pixi run -e rv32 validate-mnist-runtime
pixi run -e rv32 observe-mnist-runtime-renode
pixi run -e rv32 compare-mnist-runtime-platforms
pixi run -e rv32 validate-mnist-runtime-dual
```

What this validates:

- registry-backed runtime initialization
- one supported model: `st_mnist_28`
- a five-sample official MNIST batch with label-based pass/fail on QEMU
- optional QEMU-vs-Renode output cross-check using per-sample metrics and hashes

The runtime gate now passes only when the committed sample batch predicts the correct MNIST labels. The logs still emit per-sample hashes so QEMU and Renode runs can be compared for drift.

## Project map

- `docs/README.md` - documentation index
- `docs/SUPPORTED_MATRIX.md` - supported vs experimental scope
- `docs/AI_CANONICAL_PATH.md` - canonical AI contract, tooling, and validation rules
- `docs/switching_guide.md` - target and validation selection guide
- `boards/be_u1000/DFU_FLASHING_GUIDE.md` - BE-U1000 flashing notes
- `docs/reports/RRTOS_ARCHITECTURE_AI_REVIEW.md` - architecture review snapshot

## Experimental and historical content

Some files remain in-tree for research or bring-up history, but they are not part of the supported matrix. Prefer the docs above over older notes that mention `ai_demo`, `mobilenet_test`, or legacy RV64 simulation workflows.
