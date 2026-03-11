# RTOS Stabilization Review Checklist

This document is the reviewer-facing gate for deciding whether `rrtos` is converging toward a credible RTOS repository or still presenting multiple partially merged stories. Use it for brownfield reviews, proposal triage, and promotion decisions.

## Review contract

- The repo passes only when the top-level story, build defaults, docs, and blocking CI all agree on a supported RTOS-first core with explicitly scoped secondary AI and board lanes.
- Each section below must end with a reviewer disposition: `block-now`, `fix-before-promotion`, or `hardening-follow-on`.
- `block-now` means the repo identity or support contract is not trustworthy enough to promote more scope.
- `fix-before-promotion` means the lane can keep moving, but the named gap must close before claiming a cleaner support level.
- `hardening-follow-on` means the issue is real but should stay visible behind the first architecture blockers unless it already breaks scheduler credibility.

## Narrative gate: is this still an RTOS repo first?

Question

Does the repo present itself first as a supported RTOS kernel plus named firmware lanes, with AI, federated-learning, and extra platform work clearly subordinate to that story?

Pass standard

- `README.md` leads with the RTOS identity and supported firmware lanes.
- `CMakeLists.txt` defaults build only the declared supported board and app lanes.
- Historical or experimental AI/platform material is labeled as secondary and does not redefine the repo's primary purpose.

Fail triggers

- Top-level build or project description still markets the repo as a broad AI or federated-learning platform ahead of the RTOS core.
- Docs and supported matrix disagree on what is first-class.
- Experimental lanes look equally supported from the top-level story.

Required evidence

- `README.md:3`
- `README.md:5`
- `README.md:70`
- `CMakeLists.txt:8`
- `CMakeLists.txt:217`
- `CMakeLists.txt:224`
- `docs/reports/RRTOS_ARCHITECTURE_AI_REVIEW.md:20`

Reviewer notes

- `README.md` already leads with a bare-metal RTOS identity and names two supported validation lanes.
- Resolved/current: `CMakeLists.txt` now describes the project as `"Bare-metal RISC-V RTOS with supported RV32 and BE-U1000 firmware lanes"`, which matches the RTOS-first support story.
- The architecture review still correctly warns that the repo behaves like multiple partially merged experiments.

Disposition

- `block-now`

## Pillar 1: configuration authority

Question

Is there one authoritative ownership chain for board facts, active core policy, timer and CPU frequencies, and AI or FL toggles?

Pass standard

- Immutable board facts stay in `boards/<board>/board_config.h`.
- selected-lane policy is owned by top-level build definitions.
- `config/os_config.h` acts only as fallback defaults when build-selected values are absent.
- active core count never disagrees with board maximums or selected SMP policy.

Fail triggers

- The same runtime choice is owned in multiple places.
- fallback headers silently disagree with the selected build lane.
- board facts and schedulable-core policy are mixed together without a clear precedence rule.

Required evidence

- `CMakeLists.txt:41`
- `CMakeLists.txt:62`
- `config/os_config.h:29`
- `config/os_config.h:74`
- `config/os_config.h:175`
- `config/os_config.h:203`
- `boards/be_u1000/board_config.h:19`

Reviewer notes

- The repo has improved the ownership model by exporting effective lane policy from `CMakeLists.txt` and treating `config/os_config.h` as fallback defaults.
- The remaining visible tension is that fallback values still advertise AI and FL enablement by default, while the top-level supported story is narrower and board policy remains partly split between board facts and build policy.
- This is a first-wave trust boundary because every later scheduler, BSP, and support claim depends on configuration truth being unambiguous.

Disposition

- `block-now`

## Pillar 2: BSP isolation

Question

Do applications consume stable HAL or BSP-facing contracts instead of board-private facts or bring-up logic?

Pass standard

- board-private constants stay behind `hal_board` or board-owned seams.
- applications include stable public headers rather than board-private config headers.
- board bring-up and hardware profile discovery stay in HAL/BSP code.

Fail triggers

- app code includes `board_config.h` or hardcodes board addresses.
- app code owns board pinmux, clocking, or peripheral-routing decisions directly.
- generic app surfaces cannot move to another board without editing board facts in the app.

Required evidence

- `hal/src/hal_board.c:13`
- `hal/src/hal_board.c:200`
- `hal/src/hal_board.c:409`
- `apps/be_u1000_demo/main.c:5`
- `apps/be_u1000_demo/main.c:78`
- `apps/be_u1000_demo/main.c:540`

Reviewer notes

- `apps/be_u1000_demo/main.c` now consumes public HAL headers and board execution or self-test profiles rather than directly including board-private configuration.
- `hal/src/hal_board.c` owns the BE-U1000 pinmux groups, diagnostic profiles, and execution-role metadata, which is the right seam.
- The app is still intentionally board-specific, so the review should verify that future board growth reuses the same `hal_board` contract instead of reopening direct board leakage.

Disposition

- `fix-before-promotion`

## Pillar 3: supported-surface honesty

Question

Do docs, build defaults, and planning artifacts consistently distinguish supported, observational, and experimental surfaces?

Pass standard

- `docs/SUPPORTED_MATRIX.md` is the normative support registry.
- default builds expose only the supported matrix unless an explicit experimental flag is enabled.
- historical reports stay useful for archaeology but are not mistaken for current truth.

Fail triggers

- build defaults compile experimental paths as if they were supported.
- top-level docs or planning language imply wider subsystem maturity than the support registry allows.
- historical audit docs are easier to discover than the current support contract.

Required evidence

- `docs/reports/RRTOS_ARCHITECTURE_AI_REVIEW.md:66`
- `CMakeLists.txt:45`
- `CMakeLists.txt:227`
- `docs/SUPPORTED_MATRIX.md:3`
- `docs/README.md:3`
- `README.md:61`

Reviewer notes

- The support registry, doc index, and top-level README now name the supported lanes and explicitly downgrade historical material.
- Resolved/current: the top-level CMake description now matches the supported matrix instead of advertising older federated-learning breadth.
- Reviewers should fail this pillar whenever any top-level marketing claim becomes easier to see than the supported matrix.

Disposition

- `fix-before-promotion`

## Pillar 4: firmware-centered CI

Question

Does blocking automation prove the supported lanes with build plus deterministic firmware evidence instead of document-only checks?

Pass standard

- supported RV32 and BE-U1000 lanes run in blocking CI.
- CI commands map to the same supported commands named in `README.md` and `docs/SUPPORTED_MATRIX.md`.
- evidence includes boot, scheduler or tick, timer or trap, and minimal app/runtime validation suitable for the declared supported surface.

Fail triggers

- CI only checks docs, scripts, or research-framework metadata.
- supported commands in docs differ from the blocking workflow.
- runtime validation is optional or observational for a lane that is claimed as supported.

Required evidence

- `.github/workflows/firmware-supported-matrix.yml:63`
- `.github/workflows/firmware-supported-matrix.yml:80`
- `.github/workflows/firmware-supported-matrix.yml:107`
- `README.md:9`
- `docs/SUPPORTED_MATRIX.md:10`
- `docs/BE_U1000_RUNTIME_VALIDATION_MATRIX.md:18`

Reviewer notes

- The repo now has a blocking supported-lane firmware workflow for both the canonical RV32 AI path and the BE-U1000 lane.
- The BE-U1000 validation matrix still keeps later SMP stages experimental, which is honest and should remain that way until promotion work lands.
- This pillar passes only if the workflow, docs, and runtime evidence stay aligned in the same change.

Disposition

- `fix-before-promotion`

## Pillar 5: kernel-hardening sequencing

Question

Are timer semantics and preemptive trap or context-switch completeness explicitly classified as immediate blockers or as a visible follow-on hardening tranche?

Pass standard

- the review names the timer-wheel and long-delay risk clearly.
- the review names trap save or restore and preemptive context-switch coverage clearly.
- reviewers explicitly choose whether those issues block credibility now or remain queued behind architecture cleanup.

Fail triggers

- timer and preemption risks are hidden because repo-governance cleanup is louder.
- hardening work is promised vaguely without naming the scheduler-credibility threshold.
- a supposedly supported preemptive lane lacks evidence for the necessary trap and context coverage.

Required evidence

- `kernel/src/os_timer.c:19`
- `kernel/src/os_timer.c:90`
- `arch/riscv/src/context.S:30`
- `arch/riscv/src/trap.c:19`
- `docs/reports/RRTOS_ARCHITECTURE_AI_REVIEW.md:150`

Reviewer notes

- `kernel/src/os_timer.c` uses a fixed 256-slot wheel and wrap-around heuristics that deserve explicit scrutiny for long-delay behavior and expiry bookkeeping.
- `arch/riscv/src/context.S` saves a narrow register set, and `arch/riscv/src/trap.c` handles timer, IPI, and external interrupts with limited visible context coverage in this review slice.
- Keep these issues visible, but default them to follow-on hardening unless current supported-lane evidence shows scheduler credibility already breaks before the architecture blockers above are resolved.

Disposition

- `hardening-follow-on`

## Current baseline decision

- Overall repo status: fail the checklist today.
- First-wave blockers: narrative gate and configuration authority.
- Fix-before-promotion items: BSP isolation proof, supported-surface honesty cleanup, and continued CI/doc alignment.
- Visible follow-on hardening: timer semantics and preemptive trap/context completeness unless a supported lane starts failing on that basis sooner.

## Minimum exit criteria for the next review

- Replace broad AI or federated-learning-first repo wording with RTOS-first language everywhere the top-level project identity appears.
- Keep `CMakeLists.txt`, `config/os_config.h`, and board facts aligned under one clearly documented precedence model.
- Preserve the `hal_board` seam so board-specific app work does not regress into direct board-config usage.
- Keep `docs/SUPPORTED_MATRIX.md`, `README.md`, and `.github/workflows/firmware-supported-matrix.yml` synchronized whenever a lane is promoted or demoted.
- Re-run this checklist before treating timer or preemption hardening as the main story.
