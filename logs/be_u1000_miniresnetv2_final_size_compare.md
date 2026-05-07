# BE-U1000 MiniResNetV2 Final Firmware Test

- Model: `miniresnetv2_s1_64x50_tl_int8`
- Date: 2026-05-07
- Board memory model: `BE_U1000_MEMORY_MODEL=flash`
- Toolchain: Pixi RV32 Clang, `rv32imafc_zifencei`, `ilp32f`

## Results

| Path | Final firmware result | Flash image | ELF size | Runtime result |
| --- | ---: | ---: | ---: | --- |
| Static IREE HAL/VM | link failed | n/a | `.text=414988 B` | not runnable |
| VMVX inline IREE VM/HAL | linked | `198836 B` | `text=197400 data=1436 bss=178784` | Renode inference failed |
| No VM/HAL static-direct hello probe | linked | `88016 B` | `text=87932 data=84 bss=163712` | Renode inference passed |

## Key Evidence

- Static final link failure: `.text` region overflowed BE-U1000 `FLASH` by `152844 B`.
- VMVX final strict size gate: `BE_U1000_AI_DEMO_SIZE_PASS bytes=198836 max=262144 headroom=63308`.
- VMVX Renode log reaches model load but fails inference:
  - `[AI_ALLOC] calloc_fail count=1 size=404788`
  - `[AI] ERROR: vm_invoke failed: RESOURCE_EXHAUSTED`
  - `hal_alloc_top[0] size=133120`
- No VM/HAL hello probe:
  - `NO_IREE_VM_SYMBOLS_PASS`
  - `BE_U1000_STATIC_DIRECT_INFER_PASS`

## Notes

- VMVX is flash-small enough for this model, but it is not RAM-small enough on the current BE-U1000 on-chip memory layout.
- The flash linker now uses the full contiguous TCMA+TCMB window for runtime RAM (`TCM`, 160 KiB). This lets the first VMVX `133120 B` buffer allocation pass, but the later `404788 B` allocation still cannot fit.
- There is no generic no-VM/HAL final firmware runner for `miniresnetv2_s1_64x50_tl_int8` yet. Existing no-VM/HAL final proof is the hand-scheduled hello probe only; descriptor/proxy sizes must not be reported as final firmware sizes.

## Commands

```sh
cmake --build build-be_u1000_miniresnetv2_static_final --clean-first -j5
cmake --build build-be_u1000_miniresnetv2_vmvx_final --clean-first -j5
python3 scripts/check_be_u1000_ai_demo_size.py --binary build-be_u1000_miniresnetv2_vmvx_final/rrtos_be_u1000.bin --max-bytes 262144
python3 scripts/be_u1000/run_smp_runtime.py --kernel build-be_u1000_miniresnetv2_vmvx_final/rrtos_be_u1000.elf --binary build-be_u1000_miniresnetv2_vmvx_final/rrtos_be_u1000.bin --log logs/be_u1000_miniresnetv2_vmvx_final_runtime.log --summary logs/be_u1000_miniresnetv2_vmvx_final_runtime.md --sleep-seconds 12 --expect-uart-marker 'BE_U1000_AI_MICRO_MODEL_PASS name=miniresnetv2_s1_64x50_tl_int8'
cmake --build build-be_u1000_static_direct_probe_current --clean-first -j5
python3 scripts/check_no_iree_vm_symbols.py --map build-be_u1000_static_direct_probe_current/rrtos_be_u1000.map
python3 scripts/be_u1000/run_smp_runtime.py --kernel build-be_u1000_static_direct_probe_current/rrtos_be_u1000.elf --binary build-be_u1000_static_direct_probe_current/rrtos_be_u1000.bin --log logs/be_u1000_static_direct_probe_current_runtime.log --summary logs/be_u1000_static_direct_probe_current_runtime.md --sleep-seconds 5 --expect-uart-marker 'BE_U1000_STATIC_DIRECT_INFER_PASS'
python3 -m unittest tests.test_be_u1000_ai_micro_demo tests.test_stm32ai_zoo_sweep tests.test_static_direct_descriptor_generator tests.test_iree_hal_schedule_extractor tests.test_no_iree_vm_symbol_gate tests.test_be_u1000_runtime_runner
```
