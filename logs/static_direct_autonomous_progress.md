# Static-Direct Autonomous Progress

Date: 2026-05-07

## Saved Worktree

Before continuing, the current worktree was saved without dropping the saved
copies:

- Main repo stash: `wip-before-static-direct-autonomous-2026-05-07`
- IREE submodule stash: `wip-before-rrtos-static-direct-autonomous-2026-05-07`

Both stashes were applied back to the working tree so development can continue
from the saved state.

## Stage 1: Static-Direct Schedule Verifier

Added `scripts/verify_static_direct_schedule.py`.

The verifier checks:

- schedule is fixed-shape and linear,
- explicit command list exists by default,
- every dispatch is covered exactly once by the command list,
- command slices reference known allocations,
- command slices stay inside allocation bounds,
- partially overlapping slices within one arena allocation are rejected unless
  they are the exact same range,
- every pair of transient allocations with overlapping live intervals has
  non-overlapping placed arena ranges,
- lifecycle arena placement does not overlap different transient allocations
  that are visible in the same command,
- report sizes are emitted in KiB.

Unit tests:

```sh
python3 -m unittest tests.test_static_direct_schedule_verifier \
  tests.test_static_direct_descriptor_generator \
  tests.test_iree_hal_schedule_extractor
```

Current result after Stage 2 safety fixes: `26` tests passed.

### Real Schedule Results

Current MiniResNet QEMU dump:

| Schedule | Dispatches | Commands | Arena KiB | Packed KiB | Peak KiB | Source | Warnings |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: |
| `build/iree_dumps/miniresnet/hal_schedule_test.json` | 33 | 37 | 342.875 | 342.875 | 342.875 | packed_liveness | 2 |

Fresh STM32AI zoo extraction from `.11.hal.mlir`:

| Model | Result | Dispatches | Commands | Arena KiB | Packed KiB | Peak KiB | Source | Warnings |
| --- | --- | ---: | ---: | ---: | ---: | ---: | --- | ---: |
| `miniresnetv2_s1_64x50_tl_int8` | OK_WITH_WARNINGS | 34 | 38 | 525.125 | 525.125 | 525.125 | packed_liveness | 1 |
| `mobilenetv2_a035_128_int8` | OK_WITH_WARNINGS | 165 | 183 | 1937.125 | 1937.125 | 1937.125 | packed_liveness | 1 |
| `resnet8_32_tfs_int8` | OK_WITH_WARNINGS | 49 | 56 | 527.312 | 395.188 | 527.312 | peak_transient | 1 |
| `squeezenetv11_128_tfs_int8` | OK_WITH_WARNINGS | 89 | 97 | 2229.125 | 2229.125 | 2229.125 | packed_liveness | 1 |
| `st_yololcv1_192_int8` | OK_WITH_WARNINGS | 28 | 37 | 1653.875 | 1653.875 | 1653.875 | packed_liveness | 1 |
| `fastdepth_224_int8` | FAIL | 0 | 0 | n/a | n/a | n/a |
| `handlandmarks_full_224_int8` | FAIL | 0 | 0 | n/a | n/a | n/a |

The two failures are now explicit fail-closed cases instead of being counted as
descriptor-ready no-VM/HAL size wins. Both need the Stage 2 indirect/memoized
command-buffer work.

## Stage 2: Memoized Indirect Command Buffers

Added support in `scripts/extract_iree_hal_schedule.py` for the IREE pattern
where public `main` runs `hal.device.queue.execute.indirect` and the actual
command buffer is built by private `__main_memoize_apply()`.

The extractor now:

- parses the `queue.execute.indirect bindings([...])` list in `main`,
- maps memoized command-buffer index bindings like `(%c2 : index)` back to the
  actual public `main` buffer binding,
- parses memoized `hal.command_buffer.dispatch`,
  `hal.command_buffer.fill_buffer`, and barriers as a linear command timeline,
- still lets the verifier reject unsupported or empty command lists.

Unit tests:

```sh
python3 -m unittest tests.test_iree_hal_schedule_extractor \
  tests.test_static_direct_schedule_verifier \
  tests.test_static_direct_descriptor_generator
```

Result after indirect bounds hardening: `26` tests passed.

Review hardening added after the first Stage 2 adversarial pass:

- transient `hal.device.queue.alloca` buffers now fail if a command binding
  exceeds the original allocation size instead of silently widening,
- memoized command-buffer index bindings now enforce
  `offset + length <= execute.indirect slot length`,
- regression tests cover both overrun cases.

Fresh STM32AI zoo extraction now verifies all seven models:

| Model | Dispatches | Commands | Arena KiB | Packed KiB | Peak KiB | Source | Warnings |
| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: |
| `miniresnetv2_s1_64x50_tl_int8` | 34 | 38 | 525.125 | 525.125 | 525.125 | packed_liveness | 1 |
| `mobilenetv2_a035_128_int8` | 165 | 183 | 1937.125 | 1937.125 | 1937.125 | packed_liveness | 1 |
| `resnet8_32_tfs_int8` | 49 | 56 | 527.312 | 395.188 | 527.312 | peak_transient | 1 |
| `squeezenetv11_128_tfs_int8` | 89 | 97 | 2229.125 | 2229.125 | 2229.125 | packed_liveness | 1 |
| `st_yololcv1_192_int8` | 28 | 37 | 1653.875 | 1653.875 | 1653.875 | packed_liveness | 1 |
| `fastdepth_224_int8` | 82 | 101 | 5439.875 | 5439.875 | 5439.875 | packed_liveness | 0 |
| `handlandmarks_full_224_int8` | 89 | 106 | 2752.188 | 2752.188 | 2752.188 | packed_liveness | 0 |

This changes the earlier conclusion: `fastdepth` and `handlandmarks` are no
longer empty descriptor/proxy cases after fresh extraction. They are now
schedule/verifier-ready, but still need generated descriptor build and runtime
oracle before they can be counted as proven runnable no-VM/HAL firmware.

## Stage 3: MiniResNet Static-Direct Runtime Oracle

Strengthened the QEMU runtime lane for
`miniresnet_static_direct_validation`:

- the model now runs three consecutive static-direct invocations,
- each invocation checks arena guard bytes before and after dispatch replay,
- every run must match the known oracle:
  `argmax=3`, `hash=3045847227`, `top_score_q=99609`,
- heap peak is measured across the full three-run invoke phase.

Verification:

```sh
python3 -m unittest tests.test_miniresnet_static_direct_validation
pixi run build-miniresnet-static-direct-qemu-test
pixi run validate-miniresnet-static-direct-qemu-test
```

QEMU result:

| Metric | Value |
| --- | ---: |
| runs | 3 |
| arena guard failures | 0 |
| heap peak KiB | 0.000 |
| latency ticks per run | 977885 |
| pass count | 3 |

Firmware size from the QEMU build:

| text KiB | data KiB | bss KiB | total KiB |
| ---: | ---: | ---: | ---: |
| 272.949 | 0.160 | 430.375 | 703.484 |

## Stage 4: Arena Planner Sweep

Added `scripts/sweep_static_direct_arena_planners.py` and a regression test
covering a synthetic fragmentation case where order-sensitive planning matters.

The sweep compares:

- `linear_no_reuse`,
- current `input_order_first_fit`,
- `size_desc_first_fit`,
- `start_size_first_fit`,
- `size_desc_best_fit`.

Verification:

```sh
python3 -m unittest tests.test_static_direct_arena_planner_sweep
python3 -m py_compile scripts/sweep_static_direct_arena_planners.py
python3 scripts/sweep_static_direct_arena_planners.py \
  --schedule logs/miniresnetv2_s1_64x50_tl_int8_fresh_hal_schedule.json \
  --schedule logs/mobilenetv2_a035_128_int8_fresh_hal_schedule.json \
  --schedule logs/resnet8_32_tfs_int8_fresh_hal_schedule.json \
  --schedule logs/squeezenetv11_128_tfs_int8_fresh_hal_schedule.json \
  --schedule logs/st_yololcv1_192_int8_fresh_hal_schedule.json \
  --schedule logs/fastdepth_224_int8_fresh_hal_schedule.json \
  --schedule logs/handlandmarks_full_224_int8_fresh_hal_schedule.json \
  --output logs/static_direct_arena_planner_sweep.md \
  --json-output logs/static_direct_arena_planner_sweep.json
```

Result:

| Model | Linear KiB | Current KiB | Best KiB | Best planner | Save vs current KiB |
| --- | ---: | ---: | ---: | --- | ---: |
| `miniresnetv2_s1_64x50_tl_int8` | 805.039 | 525.125 | 525.125 | `input_order_first_fit` | 0.000 |
| `mobilenetv2_a035_128_int8` | 3891.969 | 1937.125 | 1937.125 | `input_order_first_fit` | 0.000 |
| `resnet8_32_tfs_int8` | 774.977 | 527.312 | 527.312 | `size_desc_best_fit` | 0.000 |
| `squeezenetv11_128_tfs_int8` | 2229.520 | 2229.125 | 2229.125 | `input_order_first_fit` | 0.000 |
| `st_yololcv1_192_int8` | 1653.875 | 1653.875 | 1653.875 | `input_order_first_fit` | 0.000 |
| `fastdepth_224_int8` | 5439.875 | 5439.875 | 5439.875 | `input_order_first_fit` | 0.000 |
| `handlandmarks_full_224_int8` | 2752.188 | 2752.188 | 2752.188 | `input_order_first_fit` | 0.000 |

Conclusion: the current planner is already at the effective lower bound on
these real schedules. `resnet8` can reduce packed placement with best-fit, but
the final arena remains pinned by `peak_transient_bytes`, so changing the
generator planner alone would not reduce firmware RAM yet.

## Stage 5: Spill/Fusion Prototype

Added `scripts/prototype_static_direct_spill_experiment.py`.

This is an upper-bound experiment for future IREE compiler-pass work. It does
not claim the generated firmware is smaller today. It separates the verified
descriptor baseline from a command-liveness/best-fit model baseline, then ranks
tiling/fusion/spill candidates only against the model baseline.

Verification:

```sh
python3 -m unittest tests.test_static_direct_spill_experiment
python3 -m py_compile scripts/prototype_static_direct_spill_experiment.py
python3 scripts/prototype_static_direct_spill_experiment.py \
  --schedule logs/miniresnetv2_s1_64x50_tl_int8_fresh_hal_schedule.json \
  --schedule logs/mobilenetv2_a035_128_int8_fresh_hal_schedule.json \
  --schedule logs/resnet8_32_tfs_int8_fresh_hal_schedule.json \
  --schedule logs/squeezenetv11_128_tfs_int8_fresh_hal_schedule.json \
  --schedule logs/st_yololcv1_192_int8_fresh_hal_schedule.json \
  --schedule logs/fastdepth_224_int8_fresh_hal_schedule.json \
  --schedule logs/handlandmarks_full_224_int8_fresh_hal_schedule.json \
  --output logs/static_direct_spill_fusion_prototype.md \
  --json-output logs/static_direct_spill_fusion_prototype.json \
  --top 5
```

Best candidates:

| Model | Verified KiB | Model KiB | Model gap KiB | Best allocation | Alloc KiB | Best tile | Tile model-save KiB | Aggressive model-save KiB |
| --- | ---: | ---: | ---: | --- | ---: | --- | ---: | ---: |
| `miniresnetv2_s1_64x50_tl_int8` | 525.125 | 525.125 | 0.000 | `transient_buffer_1` | 395.125 | `tile4` | 291.125 | 291.125 |
| `mobilenetv2_a035_128_int8` | 1937.125 | 1937.125 | 0.000 | `transient_buffer_1` | 1857.125 | `tile4` | 1328.812 | 1395.688 |
| `resnet8_32_tfs_int8` | 527.312 | 288.000 | 239.312 | `transient_buffer` | 160.000 | `tile2` | 52.812 | 52.812 |
| `squeezenetv11_128_tfs_int8` | 2229.125 | 2229.125 | 0.000 | `transient_buffer_1` | 2229.000 | `tile4` | 1671.750 | 2228.875 |
| `st_yololcv1_192_int8` | 1653.875 | 1653.875 | 0.000 | `transient_buffer_1` | 1637.000 | `tile4` | 1227.750 | 1637.000 |
| `fastdepth_224_int8` | 5439.875 | 5439.875 | 0.000 | `transient_buffer_1` | 5390.875 | `tile4` | 4043.125 | 5390.875 |
| `handlandmarks_full_224_int8` | 2752.188 | 2752.188 | 0.000 | `transient_buffer_1` | 2751.562 | `tile4` | 2063.625 | 2751.562 |

Conclusion: the useful compiler target is not a better arena packer; it is the
dominant long-lived `transient_buffer_1` style allocation. A pass should first
try tiling/partial accumulation around that allocation. External spilling alone
only moves the memory unless the backing store is genuinely streamed or
non-RAM. `resnet8` has a 239.312 KiB verified-vs-model accounting gap, so its
Stage 5 savings must be read as model savings, not proven firmware RAM savings.
