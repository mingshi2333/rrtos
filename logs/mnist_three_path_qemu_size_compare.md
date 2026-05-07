# MNIST Three-Path QEMU Size Comparison

Date: 2026-05-07

Scope:

- Simulator only: QEMU `virt`
- QEMU RAM: `--memory 64M`
- Linker: `arch/riscv/link_qemu_test.ld`
- Model: `st_mnist_28`
- Renode / BE-U1000 fixed memory model was not used or changed for this test.

## Paths

| Path | Build | Runtime HAL mode | Kernel |
| --- | --- | --- | --- |
| IREE static/native | `build-qemu-test` | `static_library` | `build-qemu-test/apps/mnist_app/mnist_validation` |
| IREE VMVX inline | `build-qemu-vmvx-test` | `inline` | `build-qemu-vmvx-test/apps/mnist_app/mnist_vmvx_validation` |
| Static direct no VM/HAL | `build-qemu-test` | no AI runtime linked | `build-qemu-test/apps/mnist_app/mnist_static_direct_validation` |

## Size

`bin` is the final binary image size. `dec` is `llvm-size` text + data + bss.

| Path | text KiB | data KiB | bss KiB | dec KiB | bin KiB | bin delta vs static |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| IREE static/native | 554.1 | 3.0 | 85.7 | 642.8 | 557.1 | baseline |
| IREE VMVX inline | 465.3 | 1.4 | 85.6 | 552.3 | 466.7 | -90.4 KiB (-16.2%) |
| Static direct no VM/HAL | 416.6 | 0.1 | 87.9 | 504.6 | 416.7 | -140.4 KiB (-25.2%) |

Direct vs VMVX image delta: `-50.0 KiB (-10.7%)`.

## QEMU Runtime Check

| Path | Result | Invoke heap peak | First-sample latency |
| --- | --- | ---: | ---: |
| IREE static/native | `AI_VALIDATION_PASS count=5` | 5.7 KiB | 408 us |
| IREE VMVX inline | `AI_VALIDATION_PASS count=5` | 6.3 KiB | 3158 us |
| Static direct no VM/HAL | `MNIST_STATIC_DIRECT_PASS count=1` | 0.0 KiB | 2726 CLINT ticks |

The static-direct no-VM/HAL map gate passed:

```text
NO_IREE_VM_SYMBOLS_PASS map=build-qemu-test/apps/mnist_app/mnist_static_direct_validation.map
```

Logs:

- `logs/mnist_static_validation_qemu_test.log`
- `logs/mnist_vmvx_validation_qemu_test.log`
- `logs/mnist_static_direct_validation_qemu_test.log`
