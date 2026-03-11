# Documentation Index

## Authoritative docs

- `docs/SUPPORTED_MATRIX.md` - RTOS-first supported firmware lanes, subordinate AI extensions, and experimental scope
- `docs/AI_CANONICAL_PATH.md` - canonical AI API, codegen path, and runtime validation rules
- `docs/CURRENT_FOOTPRINT_BASELINE.md` - current supported-lane footprint summary from generated build artifacts
- `docs/FOOTPRINT_OPTIMIZATION_BACKLOG.md` - ranked low-risk and medium-risk footprint work with expected gain, risk, and dependency
- `docs/BE_U1000_MULTICORE_NOTES.md` - board facts, repo assumptions, and staged multicore strategy for BE-U1000
- `docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md` - supported runtime validation story plus historical SMP notes for BE-U1000
- `docs/BE_U1000_SDK_COMPARISON.md` - SDK-to-rrtos feature comparison and added board HAL supplements
- `docs/EVU_BA_PINMAP.md` - EVU-BA-2.3 connector/header pin mapping recovered from the schematic tail pages
- `docs/EVU_BA_2_3_BOARD_INVENTORY.md` - proof that the active `be_u1000` lane is already EVU-BA-2.3-shaped and what is complete vs partial
- `docs/EVU_BA_2_3_RTOS_BRINGUP_BASELINE.md` - repo-vs-mainstream RTOS baseline for the current EVU-BA-2.3 board lane
- `docs/EVU_BA_2_3_PHASE1_VALIDATION_PLAN.md` - phase-one validation plan for the current EVU-BA-2.3 board lane
- `docs/EVU_BA_2_3_COMPLETION_BACKLOG.md` - prioritized unfinished work for EVU-BA-2.3 board completion
- `docs/BV_APPLICATION_INTEGRATION_PLAN.md` - explicit BV mapping status, blocking gaps, and provisional core-assignment rule
- `docs/BE_U1000_READINESS_ACCEPTANCE.md` - acceptance checklist for the current BE-U1000 readiness change
- `docs/RRTOS_MULTICORE_BOUNDARIES.md` - kernel/arch/board layering and recommended core-role split
- `docs/switching_guide.md` - which target or validator to use for a given task
- `docs/reports/RRTOS_BROWNFIELD_AUDIT_MULTICORE_ROADMAP.md` - current audit and staged optimization/multicore roadmap
- `apps/mnist_app/README.md` - canonical AI app and validation target details
- `boards/be_u1000/DFU_FLASHING_GUIDE.md` - board flashing instructions
- `zoo/workflow.md` - current model-codegen workflow for the canonical AI path

Generated build evidence for the current supported lanes is emitted next to the built artifacts:

- `build/apps/mnist_app/mnist_validation_footprint.md`
- `build-be_u1000/rrtos_be_u1000_footprint.md`
- `logs/be_u1000_smp_probe.md` for the experimental SMP probe

## Historical reports

These files capture prior investigation or migration context. They are useful for archaeology, but they are not the source of truth for the current supported matrix.

- `docs/reports/RRTOS_ARCHITECTURE_AI_REVIEW.md`
- `docs/reports/*.md`
- `docs/MOBILENET_GENERATION_GUIDE.md`
- `docs/MEMORY_ANALYSIS.md`
- `docs/OPTIMIZATION_REPORT.md`
- root-level reports such as `AI_TESTING_REPORT.md`

## Reading order for new contributors

1. `README.md`
2. `docs/SUPPORTED_MATRIX.md`
3. `docs/switching_guide.md`
4. `docs/AI_CANONICAL_PATH.md`
5. `boards/be_u1000/DFU_FLASHING_GUIDE.md` or `apps/mnist_app/README.md`
