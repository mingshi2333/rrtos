# BE-U1000 SMP Runtime Probe

- Started: 2026-05-06T23:06:18.388687+00:00
- Finished: 2026-05-06T23:06:26.871995+00:00
- Status: `passed`
- Kernel: `build-be_u1000_static_direct_probe_current/rrtos_be_u1000.elf`
- Binary: `build-be_u1000_static_direct_probe_current/rrtos_be_u1000.bin`
- UART log: `logs/be_u1000_static_direct_probe_current_runtime.log`
- Boot log check: `not-run`
- UART marker check: `passed`
- First error: `none`

This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.