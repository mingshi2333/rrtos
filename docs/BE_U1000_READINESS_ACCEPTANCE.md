# BE-U1000 Readiness Acceptance

This checklist captures the current acceptance bundle for the active OpenSpec change `update-rrtos-footprint-multicore-readiness`.

## Acceptance scope

- supported-lane build and footprint evidence for `qemu_virt + mnist_app`
- supported-lane build and board validation for `be_u1000 + be_u1000_demo`
- experimental staged runtime validation for `be_u1000` multicore behavior
- board-level EVU-BA pin and self-test coverage in Renode

## Acceptance commands

### Supported lanes

- `pixi run -e rv32 validate-supported-rv32`
- `pixi run -e be-u1000 validate-supported`

### Experimental board/runtime lanes

- `pixi run -e be-u1000 validate-selftest-sim`
- `pixi run -e be-u1000 probe-smp-build`
- `pixi run -e be-u1000 validate-smp-affinity-experimental`
- `pixi run -e be-u1000 validate-smp-balance-experimental`
- `pixi run -e be-u1000 validate-runtime-stages-experimental`

### OpenSpec

- `openspec validate update-rrtos-footprint-multicore-readiness --strict`

## Acceptance interpretation

- Passing supported-lane commands means the current supported matrix is still healthy and footprint evidence is regenerated from current artifacts.
- Passing experimental commands means the current repo proves a staged multicore path in simulation only; this does not promote SMP to supported status.
- Any future supported-status promotion must update `docs/SUPPORTED_MATRIX.md`, the acceptance commands above, and the OpenSpec task state together.
