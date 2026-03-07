# Current Footprint Baseline

This document summarizes the current supported-lane footprint evidence generated from build artifacts on 2026-03-07.

## Source artifacts

- `build/apps/mnist_app/mnist_validation_footprint.md`
- `build-be_u1000/rrtos_be_u1000_footprint.md`

## Supported-lane summary

| Lane | Board | SMP | Text | Data | BSS | Heap | Stack | Kernel archive dec |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `mnist_validation` | `qemu_virt` | `OFF` | 564424 | 3084 | 68036 | n/a | 16396 | 12941 |
| `rrtos_be_u1000` | `be_u1000` | `OFF` | 7948 | 12 | 4980 | 94208 | 4096 | 14245 |

## Notes

- These values come from current built artifacts, not from historical `ai_demo` or YOLO reports.
- The supported validation commands now regenerate these reports as part of `pixi run -e rv32 validate-supported-rv32` and `pixi run -e be-u1000 validate-supported`.
- `be_u1000` shows linker-defined `.heap` and `.stack` sections explicitly in the image.
- The supported lanes still run with `OS_SMP_EN=OFF`; multicore remains experimental pending runtime validation.
