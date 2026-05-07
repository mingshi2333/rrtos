# MiniResNet Patch First-Layer Real Inference QEMU

Date: 2026-05-07

Commands:

```bash
python3 -m py_compile scripts/run_miniresnet_patch_validation.py scripts/run_miniresnet_static_direct_validation.py
python3 -m unittest tests.test_miniresnet_patch_validation tests.test_miniresnet_static_direct_validation tests.test_patch_based_inference_prototype
pixi run build-miniresnet-patch-qemu-test
pixi run validate-miniresnet-static-direct-qemu-test
pixi run validate-miniresnet-patch-qemu-test
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation build-qemu-test/apps/mnist_app/miniresnet_static_direct_validation
```

This is a real patched runtime canary. It replaces the first MiniResNet
convolution/requant step with a local C implementation, then resumes the
existing static-direct descriptor at command index 5 through a small remap
layer. The remap moves the old large transient suffix from arena offset `244608`
to offset `0`, and moves the overlapping 35 KiB work-buffer live range to
offset `106496`.

## Output

The patched canary matches the static-direct baseline output exactly on the
synthetic QEMU sample.

| Path | Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Static direct | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977885 |
| Static direct | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977885 |
| Static direct | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977884 |
| Patch remap | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1360870 |
| Patch remap | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1360870 |
| Patch remap | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1370879 |

| Metric | Static Direct | Patch Remap | Delta |
| --- | ---: | ---: | ---: |
| Average latency | 977884.7 ticks | 1364206.3 ticks | +386321.7 ticks |
| At 250 MHz | 3.912 ms | 5.457 ms | +1.545 ms |
| Slowdown | 1.000x | 1.395x | +39.5% |

## Memory

The remapped canary now uses a smaller static arena. Heap peak remains zero
because the validation path uses fixed storage, not per-invoke allocation.

| Field | Static Direct | Patch Remap | Delta |
| --- | ---: | ---: | ---: |
| Input | 3.125 KiB | 3.125 KiB | 0.000 KiB |
| Arena | 342.875 KiB | 139.000 KiB | -203.875 KiB |
| Arena guards | 0.125 KiB | 0.125 KiB | 0.000 KiB |
| Output | 0.039 KiB | 0.039 KiB | 0.000 KiB |
| Input + arena + guards + output | 346.164 KiB | 142.289 KiB | -203.875 KiB |
| Task stack | 64.000 KiB | 64.000 KiB | 0.000 KiB |
| Invoke heap peak | 0.000 KiB | 0.000 KiB | 0.000 KiB |

## Image Size

Image sections from `llvm-size -A`:

| Section | Static Direct | Patch Remap | Delta |
| --- | ---: | ---: | ---: |
| `.text` | 273.137 KiB | 278.012 KiB | +4.875 KiB |
| `.data` | 0.160 KiB | 0.160 KiB | 0.000 KiB |
| `.bss` | 414.371 KiB | 210.496 KiB | -203.875 KiB |
| `.stack` | 16.004 KiB | 16.004 KiB | 0.000 KiB |
| Total | 703.792 KiB | 504.792 KiB | -199.000 KiB |

## Interpretation

This result proves the hook point is viable and that the first arena remap is
real: app-level output stays identical while static arena storage drops by
203.875 KiB.

The cost is latency. The hand-written first layer plus one-command remap wrapper
is about 39.5% slower than the current static-direct baseline. The next useful
optimization is not more arena work; it is replacing the floating-point requant
in the first-layer patch with a fixed-point requant and, after that, avoiding
the per-command wrapper overhead by materializing a generated remapped
descriptor.

## Generated Remap TransformIR Canary

The arena remap constants are now reproducible from the verified static-direct
memory summary instead of existing only as hand-written C rules.

Command:

```bash
python3 scripts/generate_static_direct_remap_plan.py \
  --verify-summary logs/miniresnet_static_direct_schedule_verify.json \
  --name miniresnet_first_layer_patch \
  --patch-allocation transient_buffer_1 \
  --patch-internal-offset 208768 \
  --produced-bytes 58752 \
  --continuation-command-index 5 \
  --remap-end-command 13 \
  --scratch-allocation transient_buffer \
  --scratch-end-command 17 \
  --output-json logs/miniresnet_patch_remap_plan.json \
  --output-md logs/miniresnet_patch_remap_plan.md
```

Generated plan:

| Metric | Bytes | KiB |
| --- | ---: | ---: |
| Original arena | 351104 | 342.875 |
| Remapped arena | 142336 | 139.000 |
| Saved | 208768 | 203.875 |

The plan captures the same rules as the current canary: rebase
`transient_buffer_1` from old actual offset `244608` to new offset `0` for
`106496` bytes, relocate internal constants in `[208768, 315264)` by
`-208768`, and move `transient_buffer` scratch to new offset `106496` through
command range `[5, 17)`.

The same generator now renders the firmware remap C/H as well:

```bash
python3 scripts/generate_static_direct_remap_plan.py \
  --verify-summary logs/miniresnet_static_direct_schedule_verify.json \
  --name miniresnet_first_layer_patch \
  --patch-allocation transient_buffer_1 \
  --patch-internal-offset 208768 \
  --produced-bytes 58752 \
  --continuation-command-index 5 \
  --remap-end-command 13 \
  --scratch-allocation transient_buffer \
  --scratch-end-command 17 \
  --output-json logs/miniresnet_patch_remap_plan.json \
  --output-md logs/miniresnet_patch_remap_plan.md \
  --output-c apps/mnist_app/src/miniresnet_patch_arena_remap.c \
  --output-h apps/mnist_app/src/miniresnet_patch_arena_remap.h \
  --macro-prefix MINIRESNET_PATCH_REMAP \
  --header-guard MINIRESNET_PATCH_ARENA_REMAP_H \
  --function-name miniresnet_patch_invoke_continuation_remapped
```

Post-render QEMU validation:

| Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| ---: | ---: | ---: | ---: | --- | ---: |
| 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402112 |
| 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402113 |
| 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402114 |

Image sections after generated C/H are unchanged from the q31 patch build:
flash payload is about `280.359 KiB`, static RAM is about `226.500 KiB`, and
`llvm-size -A Total` is about `506.979 KiB`.

## RV32 Ukernel Catalog Integration Canary

After adding `ai/ukernel/rrtos_ai_ukernel.c`, the MiniResNet first-layer patch
was changed to call `rrtos_ai_ukernel_conv2d_i8_tile()` one output row at a
time. This proves the project-owned kernel catalog can be linked into the
static-direct patch path without changing the output oracle.

Commands:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
bash scripts/build_rrtos_ai_ukernel.sh
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py \
  --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation \
  --log logs/miniresnet_patch_validation_qemu_test.log \
  --memory 64M
```

Output stayed identical:

| Path | Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Patch + RV32 ukernel | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 3279597 |
| Patch + RV32 ukernel | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 3279597 |
| Patch + RV32 ukernel | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 3279598 |

| Metric | Previous Hand-Written Patch | Patch + RV32 Ukernel | Delta |
| --- | ---: | ---: | ---: |
| Average latency | 1364206.3 ticks | 3279597.3 ticks | +1915391.0 ticks |
| At 250 MHz | 5.457 ms | 13.118 ms | +7.662 ms |
| Slowdown | 1.000x | 2.404x | +140.4% |

The static arena remains `139.000 KiB`. Image `.text` grows from
`278.012 KiB` to `278.762 KiB`, so the integration cost is small in flash, but
the current generic row-tile kernel is too slow. The next kernel-catalog step
should specialize the first-conv path or add a fixed-point fused
conv+requant-row kernel instead of routing every MAC through the generic
parameterized loop.

## Fused RV32 Ukernel First-Conv Canary

The MiniResNet patch now calls a catalog-owned fast path:
`rrtos_ai_ukernel_conv2d_i8_ch1_ohwi_7x7_s2_pad3_requant_row()`. This keeps the
reusable ukernel boundary, but removes the generic conv descriptor setup,
temporary row accumulator, and second requant/writeback loop from the app patch.

Commands:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
bash scripts/build_rrtos_ai_ukernel.sh
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py \
  --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation \
  --log logs/miniresnet_patch_validation_qemu_test.log \
  --memory 64M
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation
```

Output stayed identical:

| Path | Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Patch + fused RV32 ukernel | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1335683 |
| Patch + fused RV32 ukernel | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1335684 |
| Patch + fused RV32 ukernel | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1335687 |

| Metric | Hand-Written Patch | Generic Ukernel Patch | Fused Ukernel Patch |
| --- | ---: | ---: | ---: |
| Average latency | 1364206.3 ticks | 3279597.3 ticks | 1335684.7 ticks |
| At 250 MHz | 5.457 ms | 13.118 ms | 5.343 ms |
| Relative to hand-written patch | 1.000x | 2.404x | 0.979x |
| Relative to generic ukernel patch | 0.416x | 1.000x | 0.407x |

Image sections from `llvm-size -A`:

| Section | Hand-Written Patch | Generic Ukernel Patch | Fused Ukernel Patch |
| --- | ---: | ---: | ---: |
| `.text` | 278.012 KiB | 278.762 KiB | 278.324 KiB |
| `.data` | 0.160 KiB | 0.160 KiB | 0.160 KiB |
| `.bss` | 210.496 KiB | 210.496 KiB | 210.496 KiB |
| `.stack` | 16.004 KiB | 16.004 KiB | 16.004 KiB |
| Total | 504.792 KiB | 505.542 KiB | 505.104 KiB |

Interpretation: the generic catalog entry is useful as a correctness scaffold,
but not as the performance path. A narrow fused ukernel gives the reusable ABI
boundary we want for future descriptor generation while recovering the latency
lost to the generic indexing path. The remaining first-layer gap versus static
direct is still the C first-conv implementation itself and the continuation
wrapper/remap overhead, not the ukernel call boundary.

## Descriptor Tile Ukernel Revision

The first fused version above proved the performance shape, but exposed a
MiniResNet-specific symbol in the public ukernel ABI. After review, the public
entry point was changed to:

```c
rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile(
    input, weights, bias, multipliers, padded_output, &params)
```

The descriptor-style params carry `struct_size`, `abi_version`, flags,
`tile_row_begin`, `tile_row_count`, shape, padding, zero points, activation
clamp, and padded-output bounds. The public dispatcher uses a private
ch1/7x7/s2/pad3/weight-zp-0 fast path when the params match MiniResNet; other
OHWI shapes fall back to the generic tile path. Range validation avoids signed
overflow-prone `begin + count` checks.

Additional validation:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
bash scripts/build_rrtos_ai_ukernel.sh
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py \
  --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation \
  --log logs/miniresnet_patch_validation_qemu_test.log \
  --memory 64M
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation
```

The final tile API result still matches the output oracle:

| Path | Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Patch + descriptor tile ukernel | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1341586 |
| Patch + descriptor tile ukernel | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1341588 |
| Patch + descriptor tile ukernel | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1341589 |

| Metric | Static Direct | Hand-Written Patch | Generic Ukernel Patch | Descriptor Tile Ukernel |
| --- | ---: | ---: | ---: | ---: |
| Average latency | 977884.7 ticks | 1364206.3 ticks | 3279597.3 ticks | 1341587.7 ticks |
| At 250 MHz | 3.912 ms | 5.457 ms | 13.118 ms | 5.366 ms |
| Relative to static direct | 1.000x | 1.395x | 3.354x | 1.372x |
| Relative to hand-written patch | 0.717x | 1.000x | 2.404x | 0.983x |
| Relative to generic ukernel patch | 0.298x | 0.416x | 1.000x | 0.409x |

Section accounting from `llvm-size -A`:

| Field | Bytes | KiB | Meaning |
| --- | ---: | ---: | --- |
| `.text` | 286092 | 279.387 | flash/load code section |
| `.data` | 164 | 0.160 | flash-load + RAM data |
| `.bss` | 215548 | 210.496 | static RAM, not flash payload |
| `.stack` | 16388 | 16.004 | reserved stack region |
| `.riscv.attributes` | 123 | 0.120 | ELF metadata |
| `llvm-size -A Total` | 518315 | 506.167 | section sum, not flash-only size |

Flash payload should be read as `.text + .data`, about `279.547 KiB` here.
Static RAM sections should be read separately as `.bss + .stack`, about
`226.500 KiB`; invoke heap peak remains `0.000 KiB` in this validation path.

## Fixed-Point Requant Trial

The MiniResNet first-layer patch now has a q31 fixed-point fused tile entry:
`rrtos_ai_ukernel_conv2d_i8_ohwi_requant_tile_q31()`. The float entry remains
as a comparison/fallback API, but the MiniResNet caller uses per-channel
`miniresnet_patch_conv1_multiplier_q31[]` and `miniresnet_patch_conv1_shift[]`.

Validation commands:

```bash
python3 -m unittest tests.test_rrtos_ai_ukernel tests.test_miniresnet_patch_validation
bash scripts/build_rrtos_ai_ukernel.sh
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py \
  --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation \
  --log logs/miniresnet_patch_validation_qemu_test.log \
  --memory 64M
llvm-size -A build-qemu-test/apps/mnist_app/miniresnet_patch_validation
```

The output oracle still matches exactly:

| Path | Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| Patch + q31 descriptor tile ukernel | 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402112 |
| Patch + q31 descriptor tile ukernel | 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402113 |
| Patch + q31 descriptor tile ukernel | 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 1402114 |

| Metric | Static Direct | Float Descriptor Tile | Q31 Descriptor Tile |
| --- | ---: | ---: | ---: |
| Average latency | 977884.7 ticks | 1341587.7 ticks | 1402113.0 ticks |
| At 250 MHz | 3.912 ms | 5.366 ms | 5.608 ms |
| Relative to static direct | 1.000x | 1.372x | 1.434x |
| Relative to float descriptor tile | 0.729x | 1.000x | 1.045x |

Section accounting from `llvm-size -A`:

| Field | Bytes | KiB | Meaning |
| --- | ---: | ---: | --- |
| `.text` | 286924 | 280.199 | flash/load code section |
| `.data` | 164 | 0.160 | flash-load + RAM data |
| `.bss` | 215548 | 210.496 | static RAM, not flash payload |
| `.stack` | 16388 | 16.004 | reserved stack region |
| `.riscv.attributes` | 123 | 0.120 | ELF metadata |
| `llvm-size -A Total` | 519147 | 506.979 | section sum, not flash-only size |

Interpretation: the q31 path is useful for removing float requant semantics from
the generated descriptor path, but it is not a latency win in this build. Even
after replacing the q31 helper's 64-bit division by a sign-safe shift, the
current RV32/QEMU result is about 4.5% slower than the float descriptor tile
path. The likely next lever is not just "fixed-point", but lowering the q31
high-mul/requant sequence into target-friendly RV32 instructions or keeping the
float path when the target ABI makes it cheaper.
