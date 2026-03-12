# rrtos

[![DeepWiki](https://img.shields.io/badge/DeepWiki-Project%20Docs-00C7B7?logo=bookstack&logoColor=white)](https://deepwiki.com/mingshi2333/rrtos)

`rrtos` is a bare-metal RISC-V RTOS repository with two explicit supported firmware lanes: a virtual RV32 validation lane on `qemu_virt` and a board bring-up lane on `be_u1000`.

AI integration remains part of the supported story, but it is a subordinate extension on top of the RTOS-first validation lanes rather than the primary identity of the repository.

## Supported firmware lanes

- `qemu_virt` + `apps/mnist_app` + `ai/include/ai_model_registry.h`
- `be_u1000` + `apps/be_u1000_demo` (current EVU-BA-2.3-shaped board path)
- Supported validation:
  - `pixi run -e rv32 validate-supported-rv32`
  - `pixi run -e be-u1000 validate-supported`
- Optional observation lanes do not promote a path to supported status.

## Quick start

### 1. Initialize dependencies

```bash
pixi install
pixi install -e be-u1000
pixi run init-iree
```

The default pixi environment targets the supported `rv32` lane.

### 2. Validate the supported RV32 firmware lane

```bash
pixi run -e rv32 validate-supported-rv32
```

This builds the supported RV32 firmware lane and runs the canonical QEMU validation flow for `apps/mnist_app`.

### 3. Validate the supported BE-U1000 firmware lane

```bash
pixi run -e be-u1000 validate-supported
```

This builds the board image and runs the supported interrupt and boot-log checks.

## AI runtime extension

The canonical AI runtime validation target is `build/apps/mnist_app/mnist_validation`. It is the supported AI extension attached to the `qemu_virt` firmware lane.

Useful commands:

```bash
pixi run -e rv32 validate-mnist-runtime
pixi run -e rv32 observe-mnist-runtime-renode
pixi run -e rv32 compare-mnist-runtime-platforms
```

What this validates on top of the RTOS lane:

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

Some files remain in-tree for research or bring-up history, but they are not part of the supported matrix. Prefer the docs above over older notes that mention `ai_demo`, `mobilenet_test`, legacy RV64 simulation workflows, or board experiments outside the declared firmware lanes.

The maintained pixi task surface is intentionally smaller than the full historical repo surface. Prefer the supported and observation commands documented here over older staged SMP, RV64, or legacy demo aliases.
