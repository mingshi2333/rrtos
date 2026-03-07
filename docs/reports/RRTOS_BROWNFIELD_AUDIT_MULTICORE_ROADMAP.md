# RRTOS Brownfield Audit & Multicore Roadmap (Grounded)

Task definition source: `.opencode/specs/deep-interview-rrtos-assessment-multicore.md`.

## Evidence labels used in this document

- **[Verified]** directly supported by code/config/build artifacts/logs in this repo.
- **[Inferred]** reasoned from verified artifacts, but not directly proven by runtime proof.
- **[Gap]** required evidence is missing from current repo context.

---

## Canonical task list (executed)

| ID | Scope | Dependencies | Expected validation | Execution approach |
|---|---|---|---|---|
| T01 | Lock deliverable scope from interview spec | None | 1:1 mapping from acceptance criteria to report sections | Treat `.opencode/specs/deep-interview-rrtos-assessment-multicore.md` as source of truth |
| T02 | Build current-state map (supported path + board/app linkage) | T01 | Every current-state claim tied to concrete file refs | Prioritize current supported docs + active build wiring |
| T03 | Build kernel/image occupancy baseline | T02 | Baseline table cites concrete artifacts and commands | Use existing build outputs first, then linker/config cross-check |
| T04 | Produce prioritized slimming opportunities | T02, T03 | Each item has expected gain/risk/dependencies + evidence class | Rank by likely impact on constrained memory and decision value |
| T05 | Assess multicore reality and practical route | T02, T03 | Distinguish implemented code vs build/runtime wiring; no overclaim | Combine code-path audit with explicit `OS_SMP_EN=ON` build probe |
| T06 | Recover/assess BV mapping | T02, T05 | Either concrete mapping or explicit external-context gap | Search repo for BV anchors, avoid domain guessing |

---

## 1) Current implementation status

### 1.1 Supported mainline and validation lanes

- **[Verified]** Current supported lanes are explicitly scoped to:
  - `qemu_virt + apps/mnist_app` and `be_u1000 + apps/be_u1000_demo` in `README.md:7` and `README.md:8`.
  - Same supported matrix is reiterated in `docs/SUPPORTED_MATRIX.md:5` and `docs/SUPPORTED_MATRIX.md:6`.
- **[Verified]** Supported validation lanes are codified as:
  - `pixi run -e rv32 validate-supported-rv32` (`README.md:10`, `docs/SUPPORTED_MATRIX.md:9`, `pixi.toml:54`).
  - `pixi run -e be-u1000 validate-supported` (`README.md:11`, `docs/SUPPORTED_MATRIX.md:11`, `pixi.toml:85`).
- **[Verified]** CMake currently routes apps by board:
  - `apps/be_u1000_demo` for `CONFIG_BOARD=be_u1000` in `CMakeLists.txt:213` and `CMakeLists.txt:214`.
  - `apps/mnist_app` for non-BE board path when AI is enabled in `CMakeLists.txt:216` and `CMakeLists.txt:217`.

### 1.2 Subsystem status (brownfield snapshot)

- **[Verified]** Core RTOS modules are actively built from `memory`, `kernel`, `hal`: `CMakeLists.txt:185`, `CMakeLists.txt:186`, `CMakeLists.txt:187`.
- **[Verified]** AI runtime lane is built through `ai/src/ai_model_registry.c` only (`ai/CMakeLists.txt:1`, `ai/CMakeLists.txt:2`, `ai/CMakeLists.txt:5`), with canonical API on registry path (`docs/AI_CANONICAL_PATH.md:3`).
- **[Verified]** Experimental/historical content is intentionally retained but out-of-matrix (`docs/SUPPORTED_MATRIX.md:13`, `docs/SUPPORTED_MATRIX.md:17`, `docs/README.md:12`, `docs/README.md:14`).
- **[Verified]** Historical memory/optimization docs are explicitly marked historical and tied to older `ai_demo` context (`docs/MEMORY_ANALYSIS.md:3`, `docs/OPTIMIZATION_REPORT.md:3`).

### 1.3 Board/application linkage (current path)

- **[Verified]** BE-U1000 board startup is injected into the app executable (`apps/be_u1000_demo/CMakeLists.txt:7`, `apps/be_u1000_demo/CMakeLists.txt:8`, `boards/be_u1000/CMakeLists.txt:8`, `boards/be_u1000/CMakeLists.txt:9`).
- **[Verified]** Board initialization wires UART/CLINT/IRQ model from platform config (`hal/src/hal_board.c:12` to `hal/src/hal_board.c:20`, `config/os_config.h:269` to `config/os_config.h:280`).
- **[Verified]** BE-U1000 validation currently checks boot, IRQ model, and board peripheral checks—not multicore behavior (`scripts/be_u1000/check_boot_log.py:97` to `scripts/be_u1000/check_boot_log.py:106`).

---

## 2) Current kernel footprint / occupancy baseline

## 2.1 Evidence sources used

- **[Verified]** Built artifacts present:
  - `build/apps/mnist_app/mnist_validation`
  - `build-be_u1000/rrtos_be_u1000.elf`
- **[Verified]** Linker memory models:
  - QEMU/RV32 linker `arch/riscv/link.ld:4` to `arch/riscv/link.ld:8`.
  - BE-U1000 TCM linker `boards/be_u1000/link_be_u1000_tcm.ld:19` to `boards/be_u1000/link_be_u1000_tcm.ld:25`.
- **[Verified]** Build-mode context from caches:
  - RV32 supported build: `build/CMakeCache.txt:24`, `build/CMakeCache.txt:330`, `build/CMakeCache.txt:828`.
  - BE-U1000 supported build: `build-be_u1000/CMakeCache.txt:18`, `build-be_u1000/CMakeCache.txt:327`, `build-be_u1000/CMakeCache.txt:831`.

## 2.2 Baseline numbers (local artifact measurement)

Command used on current artifacts:

```bash
llvm-size --format=sysv build/apps/mnist_app/mnist_validation build-be_u1000/rrtos_be_u1000.elf
llvm-size build/kernel/librv_aios_kernel.a
llvm-size build-be_u1000/kernel/librv_aios_kernel.a
```

### A) Supported AI lane image (`qemu_virt + mnist_validation`)

- **[Verified]** Section sizes from `mnist_validation`:
  - `.text` = `564424`
  - `.data` = `3084`
  - `.bss` = `68036`
  - `.stack` = `16396`
- **[Inferred]** Static RAM occupancy is high versus kernel-only target because this artifact includes AI + model runtime path; it is useful as a supported-lane image baseline, not a pure kernel-only number.

### B) Supported BE-U1000 lane image (`be_u1000_demo`)

- **[Verified]** Section sizes from `rrtos_be_u1000.elf`:
  - `.text` = `7948`
  - `.data` = `12`
  - `.bss` = `4980`
  - `.heap` = `94208`
  - `.stack` = `4096`
  - `.shared` = `0`
- **[Verified]** This aligns with linker reservation strategy where TCMB is explicitly split into heap+stack (`boards/be_u1000/link_be_u1000_tcm.ld:116` to `boards/be_u1000/link_be_u1000_tcm.ld:130`).

### C) Kernel archive baseline (component-level, pre-link-gc view)

- **[Verified]** `build/kernel/librv_aios_kernel.a` total (summed): `text=9517 data=64 bss=3360 dec=12941`.
- **[Verified]** `build-be_u1000/kernel/librv_aios_kernel.a` total (summed): `text=9597 data=64 bss=4584 dec=14245`.
- **[Inferred]** This is an upper-bound component baseline before final link-time garbage collection and app-specific pulls.

## 2.3 Baseline quality and remaining gaps

- **[Gap]** No committed map-file based section-to-symbol attribution was found for supported artifacts, limiting fine-grained ownership accounting.
- **[Gap]** No automated “kernel-only occupancy” target is present in current supported CI lanes; current baseline is image-level + archive-level.

---

## 3) Prioritized slimming / optimization opportunities

| Priority | Opportunity | Expected benefit | Cost / risk | Dependencies | Evidence class |
|---|---|---|---|---|---|
| P0 | Right-size BE-U1000 TCMB static reservations (`.heap`/`.stack`) and parameterize from config | High RAM recovery for board lane; immediate occupancy control in 96KB TCMB | Medium; requires linker/config contract and runtime retest | `boards/be_u1000/link_be_u1000_tcm.ld:116` to `boards/be_u1000/link_be_u1000_tcm.ld:130`, `config/os_config.h:109` | [Verified]+[Inferred] |
| P1 | Add release profile that disables kernel debug/stats guards in supported release builds | Moderate text+bss reduction and lower runtime overhead | Low-medium; impacts diagnostics visibility | `config/os_config.h:254`, `config/os_config.h:260`, `config/os_config.h:65` | [Verified]+[Inferred] |
| P2 | Reduce static scheduler table dimensions for board lane (`OS_CFG_TASK_MAX`, `OS_CFG_PRIO_MAX`, `OS_CFG_CPU_MAX`) | Moderate bss reduction in constrained memory | Medium; requires workload-informed limits | `kernel/src/os_sched.c:11` to `kernel/src/os_sched.c:19`, `config/os_config.h:47`, `config/os_config.h:56`, `CMakeLists.txt:64` to `CMakeLists.txt:66` | [Verified]+[Inferred] |
| P3 | Remove/guard stale legacy AI path from active build/docs to reduce drift and audit noise | Medium engineering efficiency, lower maintenance burden, reduced confusion in footprint work | Low | `ai/CMakeLists.txt:1` to `ai/CMakeLists.txt:3`, `docs/README.md:12` to `docs/README.md:21`, `docs/MEMORY_ANALYSIS.md:3`, `docs/OPTIMIZATION_REPORT.md:3` | [Verified]+[Inferred] |
| P4 | Introduce committed footprint evidence lane (size + map + diff) for both supported targets | High decision quality; prevents regression and over-claim | Low-medium initial setup | Supported lanes in `pixi.toml:54` and `pixi.toml:85` | [Inferred] |

---

## 4) Current multicore support reality (code vs wiring)

## 4.1 Implemented multicore surface (exists in code)

- **[Verified]** SMP APIs and logic exist in source:
  - `multicore/include/os_smp.h:18` (SMP-gated API surface)
  - `multicore/src/os_smp.c:231` (init), `multicore/src/os_smp.c:266` (start CPUs), `multicore/src/os_smp.c:359` (IPI handler)
- **[Verified]** Scheduler has SMP-conditional fields/paths (`kernel/src/os_sched.c:21`, `kernel/src/os_sched.c:190`, `kernel/src/os_sched.c:474`, `kernel/src/os_sched.c:518`).
- **[Verified]** Trap/startup paths include SMP conditionals (`arch/riscv/src/startup.S:8`, `arch/riscv/src/startup.S:26`, `arch/riscv/src/trap.c:11`, `arch/riscv/src/trap.c:39`).
- **[Verified]** BE-U1000 startup has multicore boot choreography and hart wait symbols (`boards/be_u1000/startup_be_u1000.S:34` to `boards/be_u1000/startup_be_u1000.S:40`, `boards/be_u1000/startup_be_u1000.S:119`, `boards/be_u1000/startup_be_u1000.S:353`).

## 4.2 Build/runtime wiring reality (current supported path)

- **[Verified]** Supported flows force SMP off (`pixi.toml:68`, `pixi.toml:80`, `build/CMakeCache.txt:828`, `build-be_u1000/CMakeCache.txt:831`).
- **[Verified]** The kernel now wires `multicore/src/os_smp.c` when `OS_SMP_EN=ON` and exposes the multicore include path (`kernel/CMakeLists.txt:7` to `kernel/CMakeLists.txt:11`, `kernel/CMakeLists.txt:21` to `kernel/CMakeLists.txt:23`).
- **[Verified]** Trap-side IPI handling now matches the SMP handler contract (`arch/riscv/src/trap.c:10` to `arch/riscv/src/trap.c:13`, `arch/riscv/src/trap.c:38` to `arch/riscv/src/trap.c:41`).
- **[Verified]** Local experimental `OS_SMP_EN=ON` probe now configures and builds successfully for `be_u1000`; see `logs/be_u1000_smp_probe.md` and `logs/be_u1000_smp_probe.log`.
- **[Verified]** Renode runtime probing now preloads the TCMA execution image as well as the ELF LMA segments, allowing the BE-U1000 image to boot in simulation instead of aborting at `PC=0x1010`; see `scripts/be_u1000/run_smp_runtime.py:56` to `scripts/be_u1000/run_smp_runtime.py:69`, `scripts/simulation/be_u1000.resc:20` to `scripts/simulation/be_u1000.resc:22`.
- **[Verified]** The experimental SMP runtime lane now proves `CPU1` online, IPI reschedule, and the current demo task mapping `control -> Core0`, `worker -> Core1`; see `logs/be_u1000_smp_runtime_affinity_probe.md` and `logs/be_u1000_smp_boot.log:14` to `logs/be_u1000_smp_boot.log:22`.
- **[Verified]** Current supported BE boot log evidence shows single-lane scheduler start and task ticks, but no explicit multi-hart online evidence (`logs/be_u1000_boot_sim.log:13` to `logs/be_u1000_boot_sim.log:16`).

## 4.3 Reality conclusion

- **[Verified]** Multicore code surface exists.
- **[Verified]** Current supported build/validation lanes do **not** prove SMP as wired and working.
- **[Verified]** Experimental `OS_SMP_EN=ON` build wiring and Renode runtime probing now pass for `be_u1000`, including CPU online/IPI/task-affinity evidence.
- **[Inferred]** Current repo state is best described as **experimental build- and simulation-validated multicore support**, not production-wired SMP/AMP support, because it is not yet part of the supported matrix and still relies on approximate board simulation.

---

## 5) Practical board/application-facing multicore path

Recommended route: **staged enablement with hard gates**, starting from BE-U1000 board/app lane.

1. **Build wiring gate (must-pass)**
   - `multicore` objects are now added conditionally when `OS_SMP_EN=ON`.
   - Keep `OS_SMP_EN=OFF` as default until gate is green.
   - Evidence anchors: `kernel/CMakeLists.txt:7` to `kernel/CMakeLists.txt:11`, `multicore/src/os_smp.c:359`, `arch/riscv/src/trap.c:38` to `arch/riscv/src/trap.c:41`.

2. **Control-flow correctness gate (must-pass)**
   - The IPI handler signature is now unified between trap and SMP implementation.
   - Ensure secondary hart startup path enters secondary kernel path only when SMP mode is on.
   - Evidence anchors: `arch/riscv/src/trap.c:10` to `arch/riscv/src/trap.c:13`, `multicore/src/os_smp.c:359`, `boards/be_u1000/startup_be_u1000.S:159` to `boards/be_u1000/startup_be_u1000.S:162`.

3. **Board validation gate (must-pass)**
   - A dedicated experimental SMP lane now checks CPU online, IPI, and task-affinity markers.
   - Promote it to a supported lane only after the Renode/platform approximations and board ownership warnings are resolved.
   - Evidence anchors: `pixi.toml:86`, `scripts/be_u1000/check_boot_log.py:101` to `scripts/be_u1000/check_boot_log.py:150`, `logs/be_u1000_smp_runtime_affinity_probe.md`.

4. **Application-facing gate (must-pass)**
   - The BE-U1000 demo now provides one app scenario with affinity pinning and cross-core wakeup evidence.
   - Evidence anchors: `apps/be_u1000_demo/main.c:66` to `apps/be_u1000_demo/main.c:136`, `kernel/src/os_sched.c:62`, `kernel/src/os_sched.c:166`, `kernel/src/os_sched.c:591`.

5. **SMP vs AMP decision note**
   - **[Inferred]** For this repo’s present state, a **staged path** is safer than immediate broad SMP claims:
     - Stage A: deterministic dual-core bring-up + IPI correctness.
     - Stage B: scheduler-affinity and migration semantics.
     - Stage C: promote to supported matrix only after automated proof.

---

## 6) BV mapping status

- **[Verified]** Repo-internal search did not find authoritative BV meaning in project-owned sources (excluding `third_party` and lockfiles).
- **[Gap]** “BV” remains unresolved from repository context; it is an external application/domain context dependency.

### Minimum follow-up needed to close BV gap

1. Provide BV expansion (exact term) in this project context.
2. Identify owning app/module and expected data path.
3. State whether BV has core-affinity/latency constraints relevant to multicore design.

Until those are supplied, BV mapping should be treated as **external-context gap**, not inferred.

---

## Final decision-safe summary

- **[Verified]** Current supported paths are clear and validated (`qemu_virt+mnist_app`, `be_u1000+be_u1000_demo`).
- **[Verified]** Current occupancy baseline can be grounded from existing built artifacts, with explicit limits noted.
- **[Verified]** Multicore implementation exists in source and now passes an experimental `SMP=ON` build probe, but supported build/runtime flows still do not validate multicore behavior.
- **[Gap]** BV mapping is not recoverable from current repo content and requires external business/application clarification.
