# Static-Direct Spill/Fusion Prototype

This is an upper-bound compiler experiment. The verified base is the current descriptor accounting. The model base is command-liveness plus best-fit packing and is used only for candidate ranking. Candidate savings are measured against the model base, not against proven firmware RAM.

`elide_or_external_spill` means the allocation is removed from the static arena model; it only reduces total RAM if a later compiler/runtime implementation fuses it away, tiles it, or spills it outside RAM.

| Model | Verified KiB | Model KiB | Model Gap KiB | Best Transform | Allocation | Alloc KiB | Projected KiB | Model Save KiB | External KiB | Best Tile | Tile Save KiB | Interval |
| --- | ---: | ---: | ---: | --- | --- | ---: | ---: | ---: | ---: | --- | ---: | --- |
| `miniresnetv2_s1_64x50_tl_int8` | 525.125 | 525.125 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 395.125 | 234.000 | 291.125 | 395.125 | `tile4:transient_buffer_1` | 291.125 | [0, 13) |
| `mobilenetv2_a035_128_int8` | 1937.125 | 1937.125 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 1857.125 | 541.438 | 1395.688 | 1857.125 | `tile4:transient_buffer_1` | 1328.812 | [0, 23) |
| `resnet8_32_tfs_int8` | 527.312 | 288.000 | 239.312 | `elide_or_external_spill` | `transient_buffer` | 160.000 | 235.188 | 52.812 | 160.000 | `tile2:transient_buffer` | 52.812 | [3, 15) |
| `squeezenetv11_128_tfs_int8` | 2229.125 | 2229.125 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 2229.000 | 0.250 | 2228.875 | 2229.000 | `tile4:transient_buffer_1` | 1671.750 | [0, 89) |
| `st_yololcv1_192_int8` | 1653.875 | 1653.875 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 1637.000 | 16.875 | 1637.000 | 1637.000 | `tile4:transient_buffer_1` | 1227.750 | [0, 37) |
| `fastdepth_224_int8` | 5439.875 | 5439.875 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 5390.875 | 49.000 | 5390.875 | 5390.875 | `tile4:transient_buffer_1` | 4043.125 | [0, 101) |
| `handlandmarks_full_224_int8` | 2752.188 | 2752.188 | 0.000 | `elide_or_external_spill` | `transient_buffer_1` | 2751.562 | 0.625 | 2751.562 | 2751.562 | `tile4:transient_buffer_1` | 2063.625 | [0, 106) |

Interpretation:

- `tile2` and `tile4` approximate compiler tiling or partial accumulation.
- `elide_or_external_spill` is the aggressive bound for fusion/streaming/spill.
- A large `External KiB` means arena pressure moves elsewhere unless the data is truly fused or streamed.
