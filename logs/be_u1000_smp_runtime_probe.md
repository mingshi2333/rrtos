# BE-U1000 SMP Runtime Probe

- Started: 2026-03-07T16:14:46.274028+00:00
- Finished: 2026-03-07T16:14:53.705259+00:00
- Status: `cpu-abort`
- Kernel: `/home/mingshi/Project/PF/rrtos/build-be_u1000_smp_probe/rrtos_be_u1000.elf`
- UART log: `/home/mingshi/Project/PF/rrtos/logs/be_u1000_smp_boot.log`
- First error: `19:14:49.4268 [ERROR] cpu0: CPU abort [PC=0x1010]: Trying to execute code outside RAM or ROM at 0x00001010.`

This probe is experimental. Passing requires both board boot markers and SMP markers; current failures should be treated as validation blockers, not proof of SMP support.