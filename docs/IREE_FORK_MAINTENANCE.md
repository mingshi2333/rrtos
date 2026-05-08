# IREE Fork Maintenance

RRTOS uses a forked IREE submodule so local embedded patches are carried as
normal Git commits instead of dirty files under `third_party/iree`.

## Repository Layout

- Parent repository: `https://github.com/mingshi2333/rrtos.git`
- IREE fork: `https://github.com/mingshi2333/iree.git`
- Upstream IREE: `https://github.com/iree-org/iree.git`
- Submodule path: `third_party/iree`
- Active branch: `rrtos/v3.1-local-patches`
- Pinned commit: `c1c8b34c8ae31d3bb0e9325c53e2ffd0cdd65e45`

The parent repository pins the exact IREE commit. The branch in `.gitmodules`
is a convenience for `git submodule update --remote`; it is not a replacement
for the pinned gitlink in the parent commit.

## Fork Branches

- `rrtos/v3.1-local-patches`
  - Base: previous local IREE commit `d2242207764230ad398585a5771f9d54ce91b4c8`
  - Commit: `c1c8b34c8ae31d3bb0e9325c53e2ffd0cdd65e45`
  - Purpose: active branch preserving the tested IREE 3.1 baseline plus the
    local embedded patches.

- `rrtos/v3.4-patches`
  - Base: IREE `v3.4.0`
  - Commit: `87a3f1d6f05f92a402ac4d84ef24b613fadc91e8`
  - Purpose: future migration candidate. Do not repin the parent repository to
    this branch until the compiler/toolchain and firmware validation lanes have
    been rerun against IREE 3.4.

## Carried Patches

The active v3.1 branch carries three RRTOS-specific changes:

1. `build_tools/cmake/iree_bitcode_library.cmake`
   - Adds cache variables for RISC-V ukernel bitcode ABI and optional `-march`.
   - This lets embedded builds choose ABI values that match the generated
     LLVMCPU module instead of always using IREE's default `ilp32d` / `lp64d`.

2. `runtime/src/iree/hal/allocator_heap.c`
   - Adds a weak `rrtos_iree_hal_allocation_trace_record(...)` hook.
   - This preserves the old HAL heap allocation tracing path without requiring
     every firmware build to define the hook.

3. `compiler/src/iree/compiler/Dialect/LinalgExt/Transforms/ConvertConv2DToWinograd.cpp`
   - Fixes the template constructor spelling that caused the local compiler
     build to reject the inline constructor form.

The v3.4 candidate branch carries only the first two changes because IREE
`v3.4.0` already contains the corrected Winograd constructor spelling.

## Local Remote Setup

Expected remotes inside `third_party/iree`:

```bash
git -C third_party/iree remote -v
```

Expected shape:

```text
origin   https://github.com/mingshi2333/iree.git
upstream https://github.com/iree-org/iree.git
```

The local checkout may also keep a `fork` remote pointing at the same user fork
for compatibility with older commands.

## Updating The Fork

To update the active 3.1 patch branch:

```bash
git -C third_party/iree fetch --no-recurse-submodules origin rrtos/v3.1-local-patches
git -C third_party/iree switch rrtos/v3.1-local-patches
git -C third_party/iree push origin rrtos/v3.1-local-patches
git add third_party/iree
git commit -m "chore(iree): update forked patch branch"
```

To continue the IREE 3.4 migration candidate, use the separate
`rrtos/v3.4-patches` branch and repin the parent repository only after a fresh
compiler/toolchain validation pass.

Use `--no-recurse-submodules` when fetching IREE unless you explicitly need the
nested IREE dependency checkouts. IREE `v3.4.0` records a nested
`third_party/llvm-project` gitlink at
`ffc081f9bc85994f3d716a7177062f761e4df90b`, which was not fetchable from the
configured public nested remotes during this migration. The RRTOS static-direct
QEMU canary does not require that nested checkout.

## Validation Commands

After changing the IREE fork commit or parent submodule pin, run:

```bash
git submodule status third_party/iree
git -C third_party/iree status --short --branch
python3 -m py_compile scripts/validate_static_direct_package.py scripts/check_no_iree_vm_symbols.py scripts/run_miniresnet_patch_validation.py
python3 scripts/validate_static_direct_package.py --package logs/static_direct_packages/miniresnetv2_s1_64x50_tl_int8_fresh/static_direct.package.json
python3 scripts/check_no_iree_vm_symbols.py --map build-qemu-test/apps/mnist_app/miniresnet_patch_validation.map
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation --log logs/miniresnet_patch_validation_qemu_test.log --memory 64M
```
