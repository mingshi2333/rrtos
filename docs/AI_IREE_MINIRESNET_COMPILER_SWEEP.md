# IREE MiniResNet Compiler Sweep

Date: 2026-05-06

Model: `models/stm32ai_zoo/miniresnetv1_s1_64x50_tl_int8.tflite`

Goal: find whether existing IREE compiler flags can reduce the MiniResNet peak
transient allocation without changing model semantics.

## Compiler-Dump Sweep

Command:

```bash
python3 scripts/sweep_iree_miniresnet_compile_variants.py --clean
```

| Variant | Status | Stream peak | HAL peak | 315264 slab |
| --- | --- | ---: | ---: | --- |
| `baseline` | ok | 315264 | 315264 | yes |
| `minpeak_alias` | ok | 315264 | 315264 | yes |
| `aggressive_fusion` | compile failed | - | - | - |
| `data_tiling` | ok | 315264 | 315264 | yes |
| `experimental_data_tiling` | ok | 315264 | 315264 | yes |
| `stream_debug` | ok | 300928 | 300928 | no |

`aggressive_fusion` currently fails in LLVM CPU translation with `memref.alloca`
hoisting and workgroup distribution verifier errors, so it is not a valid bare
metal path for this model as configured.

## Runtime Check

Fresh QEMU validation was run after building the three app-linked MiniResNet
lanes.

| Target | Text | Data | BSS | HAL peak | Largest live block | Invoke heap peak | Latency us | Instructions |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `miniresnet_validation` | 447464 | 3136 | 87776 | 315264 | 315448 | 486792 | 100877 | 50900551 |
| `miniresnet_minpeak_validation` | 447144 | 3136 | 87776 | 315264 | 315448 | 486792 | 100792 | 50815266 |
| `miniresnet_stream_debug_validation` | 447912 | 3136 | 87776 | 300928 | 301112 | 472456 | 100841 | 50864206 |

All three lanes produced the same output hash:

```text
hash=3045847227 argmax=3 top_score_q=99609
```

Logs:

- `logs/miniresnet_validation_qemu_fresh.log`
- `logs/miniresnet_minpeak_validation_qemu_fresh.log`
- `logs/miniresnet_stream_debug_validation_qemu.log`

## Interpretation

`--iree-stream-partitioning-favor=debug` is the only tested public flag that
reduced runtime peak memory. It saves 14336 bytes of HAL transient allocation and
14336 bytes of largest live heap block versus baseline/minpeak.

This is still not the fundamental optimization. The 204800-byte
`tensor<?x32x25x64xi32>` first-convolution accumulator remains materialized in
the 300928-byte slab:

```text
align(3920, 64) + 204800 + 58752 + 13312 + 14336 + 5760 = 300928
```

Baseline/minpeak have one additional 14336-byte i32 transient component:

```text
align(3920, 64) + 204800 + 58752 + 13312 + 14336 + 14336 + 5760 = 315264
```

So `stream_debug` is a useful near-term lane, but it does not solve the real
MiniResNet peak-memory problem. The compiler-side target remains: prevent the
full first-convolution i32 accumulator from becoming a full-shape transient
resource, likely by tiling/fusing the quantized convolution and following
requantization before Stream allocation planning.
