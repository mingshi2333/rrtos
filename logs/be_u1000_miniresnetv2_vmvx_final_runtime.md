# BE-U1000 SMP Runtime Probe

- Started: 2026-05-06T23:05:13.922452+00:00
- Finished: 2026-05-06T23:05:29.389486+00:00
- Status: `uart-marker-check-failed`
- Kernel: `build-be_u1000_miniresnetv2_vmvx_final/rrtos_be_u1000.elf`
- Binary: `build-be_u1000_miniresnetv2_vmvx_final/rrtos_be_u1000.bin`
- UART log: `logs/be_u1000_miniresnetv2_vmvx_final_runtime.log`
- Boot log check: `not-run`
- UART marker check: `uart-marker-check-failed`
- First error: `missing UART marker(s): BE_U1000_AI_MICRO_MODEL_PASS name=miniresnetv2_s1_64x50_tl_int8`

This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.