# Local IREE Compiler Lane Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build and use a repository-local `iree-compile`/`iree-opt` lane so future IREE compiler changes can be tested against MiniResNet instead of the locked Python wheel compiler.

**Architecture:** Keep `iree-import-tflite` from the existing `iree-toolchain310` environment, but allow `scripts/ai_codegen.py` to override compiler tools from a local build root through `toolchain.compiler_root_env`. Add a Pixi task that initializes the required IREE compiler submodule and builds only the host compiler tools needed for this project.

**Tech Stack:** CMake, Ninja, Pixi, IREE v3.1.0 source in `third_party/iree`, Python unittest.

---

### Task 1: Add Compiler Tool Override Support

**Files:**
- Modify: `scripts/ai_codegen.py`
- Create: `tests/test_iree_local_compiler_toolchain.py`

- [ ] **Step 1: Write tests**

Create tests that verify:

```python
config = {"toolchain": {"compiler_root_env": "IREE_LOCAL_COMPILER_ROOT"}}
```

causes `resolve_iree_tools(config, ["iree-import-tflite", "iree-opt", "iree-compile"])`
to choose `iree-opt` and `iree-compile` from the fake local compiler root while
leaving `iree-import-tflite` to normal path resolution.

- [ ] **Step 2: Implement override**

Add `toolchain.compiler_root_env` support for `iree-compile` and `iree-opt`.
If the env var is set and either tool is missing/unusable, fail loudly instead
of silently falling back to the wheel compiler.

- [ ] **Step 3: Verify**

Run:

```bash
python3 -m unittest tests.test_iree_local_compiler_toolchain
python3 -m py_compile scripts/ai_codegen.py
```

Expected: both commands exit 0.

### Task 2: Add Local Compiler Build Script

**Files:**
- Create: `scripts/build_local_iree_compiler.sh`
- Modify: `pixi.toml`
- Modify: `tests/test_iree_local_compiler_toolchain.py`

- [ ] **Step 1: Add script/pixi contract tests**

Assert that:

```text
scripts/build_local_iree_compiler.sh exists
pixi.toml contains build-local-iree-compiler
script initializes third_party/llvm-project
script configures IREE_BUILD_COMPILER=ON
script disables unused heavy backends with IREE_TARGET_BACKEND_DEFAULTS=OFF
script builds iree-compile and iree-opt
```

- [ ] **Step 2: Implement script**

The script should:

```bash
git -C third_party/iree submodule update --init --depth=1 third_party/llvm-project
cmake -S third_party/iree -B build/iree-compiler-local -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DIREE_BUILD_COMPILER=ON \
  -DIREE_BUILD_TESTS=OFF \
  -DIREE_BUILD_SAMPLES=OFF \
  -DIREE_BUILD_PYTHON_BINDINGS=OFF \
  -DIREE_DEFAULT_CPU_LLVM_TARGETS="X86;RISCV" \
  -DIREE_TARGET_BACKEND_DEFAULTS=OFF \
  -DIREE_TARGET_BACKEND_LLVM_CPU=ON \
  -DIREE_INPUT_STABLEHLO=OFF \
  -DIREE_INPUT_TORCH=OFF \
  -DIREE_INPUT_TOSA=ON \
  -DIREE_OUTPUT_FORMAT_C=ON \
  -DIREE_ERROR_ON_MISSING_SUBMODULES=OFF
cmake --build build/iree-compiler-local --target iree-compile iree-opt
```

- [ ] **Step 3: Verify**

Run:

```bash
python3 -m unittest tests.test_iree_local_compiler_toolchain
git diff --check
```

Expected: both commands exit 0.

### Task 3: Build And Smoke Test Local Compiler

**Files:**
- Generated only under: `build/iree-compiler-local/`

- [ ] **Step 1: Build**

Run:

```bash
pixi run build-local-iree-compiler
```

Expected: `build/iree-compiler-local/tools/iree-compile` and
`build/iree-compiler-local/tools/iree-opt` exist and answer `--version`.
Keep both `X86` and `RISCV` in the local compiler build: `X86` is required by
the compiler's default host const-eval/JIT path, while `RISCV` is required for
the BE-U1000 output target.

- [ ] **Step 2: Use local compiler on MiniResNet**

Run:

```bash
IREE_LOCAL_COMPILER_ROOT=$PWD/build/iree-compiler-local/tools \
python3 scripts/ai_codegen.py \
  --config ai_models_stm32ai_miniresnet_stream_debug.yaml \
  --dump-compile-phases-to build/iree_dumps/miniresnet_stream_debug_local_compiler/phases
```

Expected: codegen succeeds and the phase dump still reports a 300928-byte HAL
peak for the stream-debug lane.

- [ ] **Step 3: Runtime regression**

Run:

```bash
pixi run -e rv32 build
python3 scripts/run_miniresnet_validation.py \
  --kernel build/apps/mnist_app/miniresnet_stream_debug_validation \
  --log logs/miniresnet_stream_debug_local_compiler_qemu.log \
  --timeout-seconds 30
```

Expected: `MINIRESNET_PASS count=1`, output hash remains `3045847227`, and HAL
peak remains `300928`.
