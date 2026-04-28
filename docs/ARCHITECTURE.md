# Architecture

`rrtos` is a bare-metal RISC-V RTOS project organized around two supported
firmware lanes:

- `rv32` on `qemu_virt`, with the canonical AI validation app.
- `be-u1000` on the BE-U1000 / EVU-BA-2.3-shaped board lane.

The repository still contains historical experiments, but supported behavior is
defined by `docs/SUPPORTED_MATRIX.md` and by the Pixi validation tasks in
`pixi.toml`.

## Layering

The build is assembled from these layers:

| Layer | Main paths | Responsibility |
| --- | --- | --- |
| Architecture | `arch/riscv/`, `cmake/riscv*_*.cmake` | RISC-V startup, trap/context code, linker scripts, target ISA/ABI selection |
| Board | `boards/be_u1000/` | Immutable BE-U1000 hardware facts, startup/linker selection, board headers |
| Kernel | `kernel/`, `memory/`, `multicore/` | Scheduler, timer, IPC, memory primitives, SMP scaffolding |
| HAL | `hal/include/`, `hal/src/` | Generic HAL contracts and board-specific peripheral implementations |
| AI runtime | `ai/`, `apps/mnist_app/generated/`, `third_party/iree/` | Registry-backed IREE runtime integration for the supported MNIST path |
| Apps | `apps/` | Supported firmware entrypoints and observation apps |
| Validation | `scripts/`, `tests/`, `.github/workflows/` | Local and CI gates for supported behavior |

`CMakeLists.txt` is the central build-policy file. Board headers publish facts;
the top-level build selects the active runtime policy and exports it through
`OS_CFG_*` compile definitions.

## Supported Lanes

### RV32 AI Lane

The `rv32` lane uses:

- `CONFIG_BOARD=qemu_virt`
- `ARCH_BITS=32`
- `OS_AI_EN=ON`
- `apps/mnist_app`
- IREE runtime only, with compiler and GPU-oriented runtime pieces disabled

The supported validation target is:

```bash
pixi run -e rv32 validate-supported-rv32
```

That task runs contract checks, builds the firmware, reports footprint, reports
AI footprint attribution, and runs the QEMU MNIST validation target.

### BE-U1000 Lane

The `be-u1000` lane uses:

- `CONFIG_BOARD=be_u1000`
- `RISCV_MARCH=rv32imafc_zifencei`
- `RISCV_MABI=ilp32f`
- `OS_AI_EN=OFF`
- `apps/be_u1000_demo` by default
- Renode-based board validation

The supported validation target is:

```bash
pixi run -e be-u1000 validate-supported
```

The BE-U1000 build keeps AI out of the board lane by default. HAL app variants
can be selected with `BE_U1000_APP`, and `RRTOS_HAL_FEATURES=auto` maps each app
to the minimum HAL feature set needed by that app.

## Kernel Model

The kernel currently exposes:

- cooperative and timer-driven scheduling primitives in `kernel/src/os_sched.c`
- timer behavior in `kernel/src/os_timer.c`
- IPC semantics in `kernel/src/os_ipc.c`
- memory helpers in `memory/src/os_mem.c`
- RISC-V trap/context assembly in `arch/riscv/src/`

Kernel behavior is covered by the host-side semantic harness:

```bash
pixi run validate-kernel-semantics
```

The BE-U1000 lane has physical multicore facts and SMP scaffolding, but the
supported board gate remains single-core execution. Experimental SMP build lanes
are validated as build/configuration evidence, not as proof of promoted SMP
runtime support.

## HAL Model

The HAL is split into generic headers and board-specific implementations.
Generic contracts live under `hal/include/`; BE-U1000 implementations live under
`hal/src/be_u1000/`.

Feature selection is build-time:

- `RRTOS_HAL_FEATURES=auto` lets the build derive features from `BE_U1000_APP`.
- A semicolon-separated `RRTOS_HAL_FEATURES` value can be used for explicit
  feature selection.
- Disabled HAL features are not added to the driver library target.

See `docs/HAL_CONFIGURATION.md` for the current feature map.

## AI Runtime Model

The supported AI path is registry-backed:

- model declarations: `ai_models.yaml`
- generated code: `apps/mnist_app/generated/`
- public registry API: `ai/include/ai_model_registry.h`
- runtime implementation: `ai/src/ai_model_registry.c`
- supported validation entrypoint: `apps/mnist_app/src/validation_main.c`

`third_party/iree` is used as a pinned runtime dependency. CI intentionally
checks out only the runtime submodule subset needed by the supported lane, not
the complete recursive IREE compiler/GPU dependency graph.

Generated wrappers are convenience glue. The supported proof path uses the
public registry API directly.

## Validation Architecture

The blocking firmware workflow is `.github/workflows/firmware-supported-matrix.yml`.
It runs:

- `rv32-supported`
- `be-u1000-supported`

The workflow also handles the IREE checkout carefully:

- RV32 checks out `third_party/iree` and only the runtime submodules required by
  the build.
- BE-U1000 does not need IREE and does not checkout submodules recursively.

This avoids depending on historical nested IREE compiler refs that are not part
of the supported runtime surface.

