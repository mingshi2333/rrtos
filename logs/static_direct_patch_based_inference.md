# Patch-Based Inference Prototype

This is not a runtime validation. It is a static memory-model estimate that asks what happens if one full-shape accumulator component is replaced by a tile-sized scratch component inside the existing static-direct schedule.

The intended runtime policy is still one fixed arena: model create binds the arena, invoke reuses tile scratch, and destroy is a metadata cleanup/no-op for static arena deployments.

| Model | Allocation | Base KiB | Component KiB | Tile Rows | Tile Count | Patch Scratch KiB | Projected KiB | Save KiB | Save % |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `miniresnet_static_direct_schedule_verify` | `transient_buffer_1` | 342.875 | 200.000 | 1 | 32 | 6.250 | 149.125 | 193.750 | 56.51 |
| `miniresnet_static_direct_schedule_verify` | `transient_buffer_1` | 342.875 | 200.000 | 2 | 16 | 12.500 | 155.375 | 187.500 | 54.68 |
| `miniresnet_static_direct_schedule_verify` | `transient_buffer_1` | 342.875 | 200.000 | 4 | 8 | 25.000 | 167.875 | 175.000 | 51.04 |
| `miniresnet_static_direct_schedule_verify` | `transient_buffer_1` | 342.875 | 200.000 | 8 | 4 | 50.000 | 192.875 | 150.000 | 43.75 |
| `miniresnet_static_direct_schedule_verify` | `transient_buffer_1` | 342.875 | 200.000 | 16 | 2 | 100.000 | 242.875 | 100.000 | 29.17 |

Notes:

- `Base KiB` is command-liveness plus best-fit packing, not a firmware run.
- `Patch Scratch KiB` is only the tile-sized replacement for the selected accumulator component.
- A real kernel still must prove bit-exact or tolerance-equivalent output against the current QEMU oracle.
- Per-patch create/destroy is intentionally excluded; patch loops should reuse the already-bound arena scratch.
