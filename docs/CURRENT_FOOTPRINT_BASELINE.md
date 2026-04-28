# Current Footprint Baseline

This document summarizes the current supported-lane footprint evidence generated from build artifacts on 2026-04-28.

## Source artifacts

- `build/apps/mnist_app/mnist_validation_footprint.md`
- `build/apps/mnist_app/mnist_validation_ai_footprint.md`
- `build-be_u1000/rrtos_be_u1000_footprint.md`

## Supported-lane summary

| Lane | Board | SMP | Text | Data | BSS | Heap | Stack | Kernel archive dec |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `mnist_validation` | `qemu_virt` | `OFF` | 571600 | 3296 | 70276 | n/a | 16396 | 14741 |
| `rrtos_be_u1000` | `be_u1000` | `OFF` | 18544 | 12 | 6128 | 94208 | 4096 | 14205 |

For `mnist_validation`, `Data` is `.data + .got + .got.plt`.

## AI attribution baseline

| Component | Linked bytes |
| --- | ---: |
| Generated model wrapper | 431820 |
| Generated dispatch object | 4739 |
| RRTOS AI registry runtime | 3520 |
| IREE runtime linked contribution | 112084 |

| IREE component | Linked bytes |
| --- | ---: |
| `iree/base` | 7084 |
| `iree/vm` | 21476 |
| `iree/hal` | 44892 |
| `iree/modules` | 38192 |
| `iree/io` | 440 |

The dominant model constant is `st_mnist_28__const` at 410176 bytes.

## Notes

- These values come from current built artifacts, not from historical `ai_demo` or YOLO reports.
- The supported validation commands now regenerate these reports as part of `pixi run -e rv32 validate-supported-rv32` and `pixi run -e be-u1000 validate-supported`.
- `be_u1000` shows linker-defined `.heap` and `.stack` sections explicitly in the image.
- The supported lanes still run with `OS_SMP_EN=OFF`; multicore remains experimental pending runtime validation.
