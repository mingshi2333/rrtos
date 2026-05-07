# Testing

The project uses a small set of blocking gates and several observation lanes.
Blocking gates define supported behavior. Observation lanes provide evidence but
do not promote a feature to supported status by themselves.

## Blocking Gates

### Kernel Semantic Harness

```bash
pixi run validate-kernel-semantics
```

This builds and runs host-side kernel semantic tests from `tests/kernel/`.
Current coverage includes:

- harness canary
- scheduler timeout behavior
- IPC semantics
- timer semantics
- memory semantics

### Supported RV32 Firmware Lane

```bash
pixi run -e rv32 validate-supported-rv32
```

This is the normative supported RV32 gate. It runs:

- `validate-config-authority`
- `validate-smp-config-matrix`
- `validate-bsp-seam`
- `validate-support-contract`
- `validate-supported-ai`
- CMake configure/build
- `report-footprint`
- `report-ai-footprint`
- `validate-mnist-runtime`
- `validate-mnist-quant-runtime`

The runtime validation runs QEMU and expects the committed five-sample MNIST
batch to classify correctly.

The AI validation metrics expose `latency_us`, `latency_cycles`, and
`latency_instructions`. The MNIST QEMU runners use
`-icount shift=0,align=off,sleep=off` so counters advance from guest execution
instead of host wall-clock speed. `latency_us` is still CLINT `mtime` based
simulator time, not target-board inference time. Use `latency_instructions` for
simulator-side relative comparison, and treat
`latency_cycles / OS_CFG_CPU_FREQ_HZ` as a rough target-time estimate.

The separated quantized MNIST lane can be run explicitly:

```bash
pixi run -e rv32 validate-mnist-quant-runtime
```

It runs QEMU against `mnist_quant_validation` and checks the ST MNISTv1 INT8
digit-7 sample without changing the FP32 baseline target.

### Supported BE-U1000 Firmware Lane

```bash
pixi run -e be-u1000 validate-supported
```

This is the normative supported BE-U1000 gate. It runs:

- `validate-config-authority`
- `validate-smp-config-matrix`
- `validate-bsp-seam`
- `validate-support-contract`
- CMake configure/build
- `validate-be-u1000-abi-cache`
- `report-footprint`
- `validate-irq-map`
- `validate-selftest-sim`

The runtime proof uses Renode and `scripts/be_u1000/check_boot_log.py`.

## Observation Lanes

Observation lanes are useful during bring-up, but they are not blocking unless
listed as supported in `docs/SUPPORTED_MATRIX.md`.

```bash
pixi run -e rv32 observe-mnist-runtime-renode
pixi run -e rv32 compare-mnist-runtime-platforms
pixi run -e be-u1000 validate-hal-apps
```

`validate-hal-apps` builds and runs the BE-U1000 HAL app matrix with app-specific
UART log checkers.

## Unit-Style Python Tests

Some project-owned validation scripts have Python `unittest` coverage:

```bash
pixi run python -m unittest \
  tests.test_ai_mnist_validation_scripts \
  tests.test_ai_model_registry_runtime \
  tests.test_report_ai_footprint_attribution
```

The default Pixi environment does not require `pytest`; these tests are written
to run with the Python standard library `unittest` module.

## GitHub Actions

The firmware CI workflow is:

```text
.github/workflows/firmware-supported-matrix.yml
```

It has two jobs:

- `rv32-supported`
- `be-u1000-supported`

The RV32 job checks out only the IREE runtime submodules required by the
supported build. It does not recursively fetch the full IREE dependency graph.

The BE-U1000 job does not need IREE and leaves submodules disabled at checkout.

## Expected Evidence Files

Generated local evidence is emitted under build and log paths, including:

- `build/apps/mnist_app/mnist_validation_footprint.md`
- `build/apps/mnist_app/mnist_validation_ai_footprint.md`
- `build-be_u1000/rrtos_be_u1000_footprint.md`
- `logs/be_u1000_selftest_runtime.md`
- `logs/be_u1000_hal_matrix.md`

Avoid committing timestamp-only changes to generated logs unless refreshing
evidence is the purpose of the change.

## Failure Triage

1. If checkout fails in CI, inspect submodule policy first.
2. If linker errors mention `-lgcc`, inspect `cmake/riscv32-pixi.cmake` and the
   runner's RISC-V GCC multilib layout.
3. If QEMU MNIST validation fails, inspect `logs/mnist_validation_qemu.log` and
   the `AI_VALIDATION_METRICS` labels/argmax values.
4. If BE-U1000 Renode validation fails, inspect `logs/be_u1000_boot_sim.log` and
   the checker output from `scripts/be_u1000/check_boot_log.py`.
