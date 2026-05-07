# MiniResNet Static-Direct QEMU Compare

Date: 2026-05-07

Model: `miniresnetv1_s1_64x50_tl_int8`

## Paths

| Path | Target | IREE VM/HAL runtime | Native dispatch object |
| --- | --- | --- | --- |
| IREE runtime | `miniresnet_validation` | yes | yes |
| Static-direct command descriptor | `miniresnet_static_direct_validation` | no | yes |

The static-direct path replays the extracted HAL command sequence directly:
`fill_buffer` commands plus native static dispatch calls. It does not link the
IREE VM or HAL runtime, but it still reuses the IREE-generated
`miniresnetv1_s1_64x50_tl_int8.o` dispatch implementation.

## Correctness Oracle

Both paths ran the same deterministic synthetic int8 input.

| Path | argmax | hash | top_score_q |
| --- | ---: | ---: | ---: |
| IREE runtime | 3 | 3045847227 | 99609 |
| Static-direct command descriptor | 3 | 3045847227 | 99609 |

The static-direct validator now rejects the earlier all-zero false positive by
checking this exact oracle.

## Footprint

All footprint values below use KiB (`1024 bytes = 1 KiB`).

| Path | Text KiB | Data KiB | BSS KiB | ELF dec KiB | Binary flash KiB |
| --- | ---: | ---: | ---: | ---: | ---: |
| IREE runtime | 437.4 | 3.1 | 85.7 | 526.2 | 440.5 |
| Static-direct command descriptor | 272.2 | 0.2 | 430.2 | 702.6 | 272.4 |

Flash delta: static-direct is `168.1 KiB` smaller than the IREE runtime binary
(`38.2%` smaller).

## RAM And Flash Summary

Flash uses the generated `.bin` size. RAM uses static ELF RAM
(`data + bss`) plus the measured invoke heap peak. Task stack is shown
separately because it is not model-specific.

| Path | Flash KiB | Static RAM KiB | Invoke heap peak KiB | Model RAM peak KiB | Task stack KiB |
| --- | ---: | ---: | ---: | ---: | ---: |
| IREE runtime | 440.5 | 88.8 | 475.4 | 564.2 | 64.0 |
| Static-direct command descriptor | 272.4 | 430.4 | 0.0 | 430.4 | 64.0 |

Static-direct is `168.1 KiB` smaller in flash and `133.8 KiB` smaller in
model RAM peak under this QEMU measurement. The RAM win comes from replacing
IREE invoke-time heap allocations with a packed static arena; the static RAM
section therefore grows while total measured model peak drops.

RAM note: static-direct now uses liveness-aware static arena placement. The
arena dropped from `457.5 KiB` to `342.9 KiB`, saving `114.7 KiB`
without changing the output oracle. The path still reports `heap_peak_bytes=0`
because invoke-time heap allocation is removed; its model working memory is the
static `.bss` arena instead.

| Runtime memory item | IREE runtime KiB | Static-direct KiB |
| --- | ---: | ---: |
| Invoke heap peak | 475.4 | 0.0 |
| Static arena | n/a | 342.9 |
| Input buffer | 3.1 | 3.1 |
| Output buffer | n/a | 0.04 |
| IO scratch total | n/a | 346.0 |
| Task stack | n/a | 64.0 |

## Liveness Reuse

The generator treats the extracted HAL command list as a fixed timeline. For
each transient allocation it records the first and last command that references
the allocation. Two allocations may share the same static arena offset only when
their half-open intervals do not overlap. If a dispatch or fill can observe both
allocations at the same command index, their byte ranges must remain separate.

## QEMU Runtime

| Path | Reported latency |
| --- | ---: |
| IREE runtime | 100879 us / 100878862 cycles / 50902606 instructions |
| Static-direct command descriptor | 977885 CLINT ticks |

The two latency units are not directly equivalent: the IREE runtime path
reports its perf counters, while the static-direct path currently measures
CLINT ticks around the direct command replay. The meaningful comparison in this
run is correctness and footprint.

## Commands

```sh
pixi run -e rv32 build-miniresnet-qemu-test
pixi run -e rv32 validate-miniresnet-qemu-test
pixi run -e rv32 build-miniresnet-static-direct-qemu-test
pixi run -e rv32 validate-miniresnet-static-direct-qemu-test
python scripts/check_no_iree_vm_symbols.py --map build-qemu-test/apps/mnist_app/miniresnet_static_direct_validation.map
```
