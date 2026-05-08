# IREE Fork V3.4 Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Move `third_party/iree` from the upstream repository plus local dirty patches to a user-owned IREE fork pinned to an `rrtos/v3.4-patches` commit.

**Architecture:** Keep the IREE changes as real commits inside the IREE fork instead of dirty submodule edits. The parent repository records the fork URL, the intended branch, the pinned submodule commit, and a small maintenance note.

**Tech Stack:** Git submodules, GitHub CLI, IREE CMake/runtime/compiler sources, existing RRTOS QEMU/static-direct validation scripts.

---

### Task 1: Create Or Locate The IREE Fork

**Files:**
- Modify: `.gitmodules`
- Modify: `third_party/iree` submodule remote metadata

- [x] **Step 1: Create the fork if it does not exist**

Run:

```bash
gh repo view mingshi2333/iree --json nameWithOwner,url || gh repo fork iree-org/iree --clone=false
```

Expected: `mingshi2333/iree` exists and is writable by the authenticated GitHub user.

- [x] **Step 2: Add a named fork remote inside the submodule**

Run:

```bash
git -C third_party/iree remote add fork https://github.com/mingshi2333/iree.git || git -C third_party/iree remote set-url fork https://github.com/mingshi2333/iree.git
git -C third_party/iree fetch fork
```

Expected: `fork` remote fetches successfully.

- [x] **Step 3: Update parent submodule metadata**

Edit `.gitmodules` so `third_party/iree` uses:

```ini
[submodule "third_party/iree"]
	path = third_party/iree
	url = https://github.com/mingshi2333/iree.git
	branch = rrtos/v3.4-patches
```

Expected: the parent repository records the user fork and the intended maintenance branch.

### Task 2: Preserve The Current V3.1 Patch Set

**Files:**
- Modify inside submodule: `build_tools/cmake/iree_bitcode_library.cmake`
- Modify inside submodule: `compiler/src/iree/compiler/Dialect/LinalgExt/Transforms/ConvertConv2DToWinograd.cpp`
- Modify inside submodule: `runtime/src/iree/hal/allocator_heap.c`

- [x] **Step 1: Commit current local patches on a fork branch**

Run:

```bash
git -C third_party/iree switch -c rrtos/v3.1-local-patches
git -C third_party/iree add build_tools/cmake/iree_bitcode_library.cmake compiler/src/iree/compiler/Dialect/LinalgExt/Transforms/ConvertConv2DToWinograd.cpp runtime/src/iree/hal/allocator_heap.c
git -C third_party/iree commit -m "rrtos: preserve local embedded patches"
git -C third_party/iree push -u fork rrtos/v3.1-local-patches
```

Expected: the previously dirty edits are preserved as a normal branch in the fork.

### Task 3: Build The V3.4 Patch Branch

**Files:**
- Modify inside submodule: `build_tools/cmake/iree_bitcode_library.cmake`
- Modify inside submodule: `runtime/src/iree/hal/allocator_heap.c`

- [x] **Step 1: Create the V3.4 patch branch**

Run:

```bash
git -C third_party/iree switch --detach v3.4.0
git -C third_party/iree switch -c rrtos/v3.4-patches
```

Expected: branch starts at `v3.4.0`.

- [x] **Step 2: Reapply only the patches still needed on V3.4**

Apply the RISC-V ukernel bitcode ABI configurability to `build_tools/cmake/iree_bitcode_library.cmake`.

Apply the weak HAL heap allocation trace hook to `runtime/src/iree/hal/allocator_heap.c`.

Do not reapply the Winograd constructor fix if `v3.4.0` already contains it.

- [x] **Step 3: Commit and push the V3.4 patch branch**

Run:

```bash
git -C third_party/iree add build_tools/cmake/iree_bitcode_library.cmake runtime/src/iree/hal/allocator_heap.c
git -C third_party/iree commit -m "rrtos: carry embedded RISC-V and HAL tracing patches"
git -C third_party/iree push -u fork rrtos/v3.4-patches
```

Expected: the parent repository can pin the submodule to this fork branch commit.

### Task 4: Document And Verify Parent Repository Integration

**Files:**
- Create: `docs/IREE_FORK_MAINTENANCE.md`
- Modify: `.gitmodules`
- Modify: `third_party/iree` gitlink

- [x] **Step 1: Add maintenance documentation**

Create `docs/IREE_FORK_MAINTENANCE.md` with the fork URL, branch names, patch rationale, and update commands.

- [x] **Step 2: Verify the submodule and package lane**

Run:

```bash
git submodule status third_party/iree
git -C third_party/iree status --short --branch
python3 -m py_compile scripts/validate_static_direct_package.py scripts/check_no_iree_vm_symbols.py scripts/run_miniresnet_patch_validation.py
python3 scripts/validate_static_direct_package.py --package logs/static_direct_packages/miniresnetv2_s1_64x50_tl_int8_fresh/static_direct.package.json
python3 scripts/check_no_iree_vm_symbols.py --map build-qemu-test/apps/mnist_app/miniresnet_patch_validation.map
cmake --build build-qemu-test --target miniresnet_patch_validation --parallel 5
python3 scripts/run_miniresnet_patch_validation.py --kernel build-qemu-test/apps/mnist_app/miniresnet_patch_validation --log logs/miniresnet_patch_validation_qemu_test.log --memory 64M
```

Expected: package validation, no-VM symbol gate, build, and QEMU canary pass.

- [x] **Step 3: Commit the parent repository update**

Run:

```bash
git add .gitmodules docs/IREE_FORK_MAINTENANCE.md docs/superpowers/plans/2026-05-08-iree-fork-v34-migration.md third_party/iree
git commit -m "chore(iree): pin forked v3.4 patch branch"
```

Expected: parent repository records the fork URL, branch hint, documentation, plan, and submodule commit.
