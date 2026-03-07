# RRTOS Architecture and AI Review

## Scope

This review covers the current `rrtos` architecture, board/platform integration, validation story, and AI support surfaces.

Primary goal:
- stabilize the core architecture before broader AI or feature expansion

This is a review and planning artifact, not an implementation log.

## Executive Assessment

`rrtos` already contains several strong ingredients:
- a working bare-metal kernel/scheduler path
- active BE-U1000 board bring-up and Renode validation
- an embedded IREE-based inference path
- codegen/model integration tooling

However, the repository currently behaves more like multiple partially merged architecture experiments than a stabilized RTOS platform. The highest risk is not missing features; it is disagreement between build wiring, config truth, board boundaries, and AI runtime surfaces.

## What Is Working

- The top-level build composes the main runtime modules and supports a BE-U1000 flash path via `CMakeLists.txt:81` and `boards/be_u1000/link_be_u1000_flash.ld:7`.
- BE-U1000 base addresses and simulation path were recently aligned, and the current boot log confirms corrected SPI/I2C addresses in `logs/be_u1000_boot_sim.log:8`.
- The active AI runtime path does work through the registry/IREE route compiled by `ai/CMakeLists.txt:1`.
- The repo has working script-based validation for BE-U1000 interrupt-map and boot-log checks.

## Top Architecture Risks

### 1. Config truth is fragmented

Symptoms:
- CMake exposes `OS_SMP_EN`, `OS_AI_EN`, and `OS_FL_EN` as options in `CMakeLists.txt:41`.
- `config/os_config.h:68` still hardcodes single-core operation.
- `config/os_config.h:151` and `config/os_config.h:175` duplicate the AI runtime block.
- `boards/be_u1000/CMakeLists.txt:33` declares `CONFIG_NUM_CORES=2`, while `config/os_config.h:74` still says `1`.

Impact:
- different layers can compile under incompatible assumptions
- multicore and AI support can look enabled without being consistently wired

### 2. Kernel/HAL layering is cyclic

Symptoms:
- `kernel/CMakeLists.txt:20` links kernel to drivers
- `hal/CMakeLists.txt:26` links drivers back to kernel

Impact:
- layering becomes brittle
- link order and public header boundaries become harder to reason about
- subsystem contracts are not cleanly enforced

### 3. BSP boundaries are porous

Symptoms:
- `apps/be_u1000_demo/main.c:4` directly includes kernel/config/hal headers through relative paths
- `apps/be_u1000_demo/main.c:14` pulls board config directly into the app
- app code still performs explicit board bring-up decisions in `apps/be_u1000_demo/main.c:80`

Impact:
- platform knowledge leaks upward into apps
- board migration and reuse become harder
- generic app surfaces are not actually generic

### 4. Build graph does not match repository surface

Symptoms:
- main build wires `memory`, `kernel`, `hal`, and `ai` in `CMakeLists.txt:144`
- non-BE builds only expose `apps/ai_demo` and `apps/mnist_app` in `CMakeLists.txt:173`
- active OpenSpec roadmap assumes much wider subsystem maturity than the main build reflects

Impact:
- stale or experimental paths appear first-class
- repo surface area overstates what is truly supported

### 5. Validation is firmware-light

Symptoms:
- CI currently checks research-framework docs only in `.github/workflows/research-framework-gates.yml:1`
- there is no main firmware build/sim/boot matrix in CI

Impact:
- API drift and stale build paths can survive for long periods
- architecture regressions are detected manually instead of automatically

## Top AI Support Risks

### 1. Split-brain AI API

Symptoms:
- `ai/include/ai_runtime.h:79` defines one lifecycle and tensor model
- `ai/include/ai_model_registry.h:110` defines another
- only the registry path is compiled in `ai/CMakeLists.txt:1`

Impact:
- multiple apps target incompatible APIs
- public AI surface is misleading

### 2. Stale app/demo integration paths

Symptoms:
- `apps/mnist_app/src/main.c:28` uses the active registry path
- the removed legacy `apps/ai_test` path expected the old `ai_runtime.h` contract
- the removed legacy `apps/demo` path also targeted the old contract
- `apps/ai_demo/main.c:36` hardcodes board addresses unrelated to the active BE-U1000 path

Impact:
- AI support is not one subsystem; it is several overlapping experiments

### 3. Tooling is not reproducible enough

Symptoms:
- `ai_models.yaml:12` contains a machine-local absolute model path
- `scripts/ai_codegen.py:11` defaults to a machine-local IREE toolchain path
- `zoo/scripts/tflite_to_iree_c.sh:8` also hardcodes a toolchain path

Impact:
- model regeneration is not portable
- CI-grade artifact reproduction is blocked

### 4. Embedded resource governance is advisory

Symptoms:
- descriptors expose `arena_size` and `peak_memory` in `ai/include/ai_model_registry.h:84`
- runtime uses `iree_allocator_system()` in `ai/src/ai_model_registry.c:44`
- `ai_get_time_us()` is stubbed to `0` in `ai/src/ai_model_registry.c:38`
- async inference is only a sync wrapper in `ai/src/ai_model_registry.c:393`

Impact:
- memory/performance data is not trustworthy enough for embedded planning

### 5. AI validation path is stale

Symptoms:
- before cleanup, the visible AI task story centered on a stale `test-registry` task instead of a supported runtime lane
- deterministic runtime validation was not presented as a first-class supported path

Impact:
- AI success is measured more by compile viability than by deterministic target validation

## Root Causes

- no single source of truth for system configuration
- experimental and supported paths share the same visibility
- BSP contract is not strong enough to isolate board-specific knowledge
- public APIs were allowed to drift without CI enforcement
- AI integration evolved by addition rather than convergence

## Stabilization Order

### Phase 1: Foundation truth

1. define the supported board/app/subsystem matrix
2. unify configuration sources across CMake, board config, and `os_config.h`
3. mark unsupported or experimental paths clearly

### Phase 2: Boundary cleanup

1. remove kernel/HAL link-cycle coupling
2. move board bring-up behind a BSP entrypoint
3. stop apps from knowing board internals directly

### Phase 3: Validation baseline

1. add firmware build matrix to CI
2. add BE-U1000 boot/simulation checks to CI
3. add AI API/build-path consistency checks

### Phase 4: AI convergence

1. choose one public AI contract
2. deprecate stale app/demo paths
3. standardize the model generation and artifact pipeline

### Phase 5: Deeper optimization

1. only after the above, address SMP readiness, scheduler scaling, and broader AI async/perf work

## What Not To Optimize Yet

- scheduler micro-optimizations
- large SMP work
- new device model / filesystem / network stack expansion
- AI async execution and performance tuning beyond correctness and observability
- model zoo growth before reproducibility and canonical-path cleanup

## Prioritized Optimization Roadmap

### Priority 0: Stop architectural drift

- create a focused stabilization proposal
- define canonical support boundaries
- document experimental vs supported components

### Priority 1: Single source of truth

- generate or centralize configuration for core count, AI enablement, FL enablement, memory model, and IRQ mode
- eliminate duplicated AI config blocks in `config/os_config.h`

### Priority 2: Honest subsystem boundaries

- break the kernel/HAL cycle
- introduce a board/BSP init contract
- remove relative-path and board-direct includes from apps

### Priority 3: AI contract unification

- keep one AI header/API as canonical
- explicitly deprecate stale runtime/demo surfaces
- align `apps/mnist_app` and future generated apps to the canonical path

### Priority 4: Reproducible AI toolchain

- remove absolute paths from `ai_models.yaml` and scripts
- make generators consume one lock/config source
- produce deterministic model metadata and artifact manifests

### Priority 5: CI and validation coverage

- add firmware build checks for RV32 and BE-U1000
- add Renode/boot-log smoke validation
- add AI sample/build sanity checks for the canonical AI path

## Recommended OpenSpec Themes

- `refactor-config-single-source-truth`
- `refactor-bsp-hal-boundaries`
- `add-firmware-validation-matrix`
- `unify-ai-runtime-contract`
- `standardize-ai-artifact-pipeline`

## Immediate Recommendation

Do not start with performance tuning or feature expansion.

Start with a narrow architecture-stabilization change that addresses:
- config truth
- BSP/HAL/kernel boundaries
- validation baseline

Then, once the platform is honest and reproducible, open the dedicated AI runtime unification change.
