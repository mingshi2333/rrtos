# IREE Compiler Variant Sweep Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a repeatable MiniResNet compiler sweep that tests whether existing IREE flags can reduce the 315264-byte transient activation slab.

**Architecture:** `scripts/ai_codegen.py` gains a generic YAML-controlled escape hatch for extra `iree-compile` flags. A new sweep script writes temporary MiniResNet configs, runs codegen with phase dumps, and summarizes max Stream/HAL allocation sizes through the existing compile-dump analyzer.

**Tech Stack:** Python standard library, PyYAML, existing RRTOS AI codegen, IREE `iree-compile` phase dumps.

---

### Task 1: Add Extra IREE Compile Flags To Codegen

**Files:**
- Modify: `scripts/ai_codegen.py`
- Modify: `tests/test_be_u1000_ai_micro_demo.py`

- [ ] **Step 1: Add a failing contract test**

Add assertions to `test_codegen_targets_be_u1000_single_float_abi`:

```python
self.assertIn("extra_iree_compile_flags", codegen_text)
self.assertIn("collect_extra_iree_compile_flags", codegen_text)
```

- [ ] **Step 2: Run test to verify it fails before implementation**

Run: `python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_codegen_targets_be_u1000_single_float_abi`

Expected: FAIL until `collect_extra_iree_compile_flags` exists.

- [ ] **Step 3: Implement config collection**

Add a helper near `compile_model`:

```python
def collect_extra_iree_compile_flags(defaults, model):
    flags = []
    for source in (defaults, model):
        value = source.get("extra_iree_compile_flags", [])
        if value is None:
            continue
        if isinstance(value, str):
            flags.append(value)
        else:
            flags.extend(str(flag) for flag in value)
    return flags
```

Append the result to both `llvmcpu_static` and `vmvx_inline` `compile_cmd` before the input MLIR path and output arguments.

- [ ] **Step 4: Run focused tests**

Run:

```bash
python3 -m unittest tests.test_be_u1000_ai_micro_demo.BeU1000AiMicroDemoTest.test_codegen_targets_be_u1000_single_float_abi
python3 -m py_compile scripts/ai_codegen.py
```

Expected: both commands exit 0.

### Task 2: Add MiniResNet Compiler Variant Sweep

**Files:**
- Create: `scripts/sweep_iree_miniresnet_compile_variants.py`
- Create: `tests/test_iree_miniresnet_compile_variant_sweep.py`

- [ ] **Step 1: Write unit tests for variant config generation and dump summarization**

Create tests that import the script and assert:

```python
self.assertIn("baseline", [variant.name for variant in sweep.VARIANTS])
self.assertEqual(config["models"][0]["name"], sweep.MODEL_NAME)
self.assertEqual(config["defaults"]["extra_iree_compile_flags"], variant.extra_flags)
self.assertEqual(summary["max_stream_allocation"], 315264)
```

The dump-summary test should write a tiny `.7.stream.mlir` fixture with one `stream.resource.alloca` using the same arithmetic expression already covered by `tests/test_iree_compile_dump_analyzer.py`.

- [ ] **Step 2: Run tests to verify they fail before implementation**

Run: `python3 -m unittest tests.test_iree_miniresnet_compile_variant_sweep`

Expected: FAIL until the new script exists.

- [ ] **Step 3: Implement the sweep script**

The script must:

```text
1. Define variants: baseline, minpeak_alias, aggressive_fusion, data_tiling, experimental_data_tiling, stream_debug.
2. Write YAML configs under build/iree_miniresnet_variants/<variant>/config.yaml.
3. Run scripts/ai_codegen.py --config <config> --dump-compile-phases-to <variant>/phases.
4. Continue after compile failures and record status/error.
5. Summarize max .7.stream allocation, max .11.hal allocation, and whether a 315264-byte allocation remains.
6. Print a compact table.
```

- [ ] **Step 4: Run focused tests**

Run:

```bash
python3 -m py_compile scripts/sweep_iree_miniresnet_compile_variants.py
python3 -m unittest tests.test_iree_miniresnet_compile_variant_sweep
```

Expected: both commands exit 0.

### Task 3: Execute Real Compiler Sweep And Report Result

**Files:**
- Generated only under: `build/iree_miniresnet_variants/`

- [ ] **Step 1: Run the sweep**

Run:

```bash
python3 scripts/sweep_iree_miniresnet_compile_variants.py --clean
```

Expected: each variant either compiles and prints allocation summaries, or records a compiler failure without stopping the whole sweep.

- [ ] **Step 2: Run regression tests**

Run:

```bash
python3 -m py_compile scripts/ai_codegen.py scripts/analyze_iree_compile_dump.py scripts/sweep_iree_miniresnet_compile_variants.py
python3 -m unittest tests.test_be_u1000_ai_micro_demo tests.test_iree_compile_dump_analyzer tests.test_iree_miniresnet_compile_variant_sweep
git diff --check
```

Expected: all commands exit 0.

- [ ] **Step 3: Interpret the table**

If all successful variants still report `has_315264=yes`, conclude that public IREE flags are not enough for this model. The next optimization must be a compiler/model transformation that prevents the first `tensor<?x32x25x64xi32>` accumulator from being materialized as one full 204800-byte transient region.
