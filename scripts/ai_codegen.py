#!/usr/bin/env python3
import os
import sys
import yaml
import subprocess
import re
import shutil
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent.resolve()
LOCK_FILE = PROJECT_ROOT / "iree-version.lock"
TOOLCHECK_CACHE = {}


def run_command(cmd, cwd=None, env=None):
    """Runs a shell command and raises exception on failure."""
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(
        cmd, cwd=cwd, env=env, check=True, capture_output=True, text=True
    )
    return result.stdout


def tool_is_usable(tool_path):
    tool_path = str(tool_path)
    if tool_path in TOOLCHECK_CACHE:
        return TOOLCHECK_CACHE[tool_path]

    try:
        result = subprocess.run(
            [tool_path, "--help"],
            check=False,
            capture_output=True,
            text=True,
        )
        usable = result.returncode == 0
    except OSError:
        usable = False

    TOOLCHECK_CACHE[tool_path] = usable
    return usable


def read_locked_toolchain_env():
    if not LOCK_FILE.exists():
        return ""

    for line in LOCK_FILE.read_text(encoding="utf-8").splitlines():
        if line.startswith("TOOLCHAIN_ENV="):
            return line.split("=", 1)[1].strip()
    return ""


def candidate_toolchain_roots(config):
    toolchain_cfg = config.get("toolchain", {})
    root_env = toolchain_cfg.get("root_env", "IREE_TOOLCHAIN_ROOT")
    root_value = os.environ.get(root_env, "")
    candidates = []
    if root_value:
        candidates.append(Path(root_value).expanduser())

    locked_env = read_locked_toolchain_env()
    if locked_env:
        candidates.append(Path.home() / ".mamba" / "envs" / locked_env / "bin")

    return candidates


def resolve_iree_tools(config, required_tools):
    resolved = {}

    for root in candidate_toolchain_roots(config):
        missing = []
        for tool_name in required_tools:
            candidate = root / tool_name
            if not candidate.exists() or not tool_is_usable(candidate):
                missing.append(tool_name)
            else:
                resolved[tool_name] = str(candidate)
        if not missing:
            return resolved

    path_resolved = {}
    missing = []
    for tool_name in required_tools:
        candidate = shutil.which(tool_name)
        if not candidate or not tool_is_usable(candidate):
            missing.append(tool_name)
        else:
            path_resolved[tool_name] = candidate
    if not missing:
        return path_resolved

    if missing:
        print("Error: Missing usable IREE toolchain components: " + ", ".join(missing))
        print(
            "Set IREE_TOOLCHAIN_ROOT or install the locked toolchain env from iree-version.lock."
        )
        sys.exit(1)

    return path_resolved


def resolve_model_path(config, model, config_path):
    model_file = Path(model["file"]).expanduser()
    if model_file.is_absolute():
        return model_file

    root_env = config.get("model_sources", {}).get("root_env")
    if root_env and os.environ.get(root_env):
        return Path(os.environ[root_env]).expanduser() / model_file

    return config_path.parent / model_file


def parse_mlir_info(mlir_text):
    """Extracts input and output metadata from MLIR text."""
    export_match = None
    export_pattern = re.compile(
        r"func\.func\s+@(?P<name>[A-Za-z0-9_.$-]+)\((?P<inputs>.*?)\)\s*->\s*(?P<outputs>.*?)\s*(?P<suffix>attributes\s*\{.*?\})?\s*\{",
        re.DOTALL,
    )
    for match in export_pattern.finditer(mlir_text):
        suffix = match.group("suffix") or ""
        if "iree.module.export" in suffix or match.group("name") == "main":
            export_match = match
            break

    if not export_match:
        return None

    entry_function = export_match.group("name")
    inputs_raw = export_match.group("inputs")
    outputs_raw = export_match.group("outputs")

    def parse_tensors(raw):
        tensors = []
        # Find all tensor definitions: tensor<shape x type>
        matches = re.finditer(r"tensor<([0-9x\?]+)x(\w+)>", raw)
        for i, m in enumerate(matches):
            shape_str, dtype_mlir = m.groups()
            # Replace ? with 1 for batch size, and convert to list of ints
            shape = [int(d) if d != "?" else 1 for d in shape_str.split("x")]

            # Map MLIR types to C types and IREE descriptors
            type_map = {
                "f32": {"c": "float", "dtype": "AI_DTYPE_FP32"},
                "ui8": {"c": "uint8_t", "dtype": "AI_DTYPE_UINT8"},
                "si8": {"c": "int8_t", "dtype": "AI_DTYPE_INT8"},
                "i16": {"c": "int16_t", "dtype": "AI_DTYPE_INT16"},
                "i32": {"c": "int32_t", "dtype": "AI_DTYPE_INT32"},
            }
            info = type_map.get(dtype_mlir, {"c": "uint8_t", "dtype": "AI_DTYPE_UINT8"})

            tensors.append(
                {
                    "name": f"tensor_{i}",
                    "shape": shape,
                    "type": info["c"],
                    "dtype": info["dtype"],
                }
            )
        return tensors

    return {
        "entry_function": entry_function,
        "inputs": parse_tensors(inputs_raw),
        "outputs": parse_tensors(outputs_raw),
    }


def compile_model(config, config_path, model, defaults, output_dir):
    name = model["name"]
    model_path = resolve_model_path(config, model, config_path)
    source_suffix = model_path.suffix.lower()
    required_tools = ["iree-compile"]
    if source_suffix == ".tflite":
        required_tools = ["iree-import-tflite", "iree-opt", "iree-compile"]
    iree_tools = resolve_iree_tools(config, required_tools)

    if not model_path.exists():
        print(f"Error: Model file not found: {model_path}")
        sys.exit(1)

    print(f"=== Compiling Model: {name} ===")

    # Create temp dir
    temp_dir = Path("/tmp/iree_build_temp")
    if temp_dir.exists():
        shutil.rmtree(temp_dir)
    temp_dir.mkdir(parents=True)

    # 1. Resolve model source into MLIR
    mlir_path = temp_dir / f"{name}.mlir"
    if source_suffix == ".tflite":
        run_command(
            [
                iree_tools["iree-import-tflite"],
                str(model_path),
                "-o",
                str(mlir_path),
            ]
        )
    elif source_suffix == ".mlir":
        shutil.copyfile(model_path, mlir_path)
    else:
        print(f"Error: Unsupported model source type: {model_path}")
        sys.exit(1)

    opt_mlir_path = temp_dir / f"{name}_opt.mlir"
    if source_suffix == ".tflite":
        run_command(
            [
                iree_tools["iree-opt"],
                "--pass-pipeline=builtin.module(func.func(iree-tosa-strip-signedness))",
                str(mlir_path),
                "-o",
                str(opt_mlir_path),
            ]
        )
    else:
        shutil.copyfile(mlir_path, opt_mlir_path)

    with open(opt_mlir_path, "r") as f:
        mlir_content = f.read()

    metadata = parse_mlir_info(mlir_content)
    if not metadata:
        print(f"Error: Could not parse metadata from MLIR for {name}")
        sys.exit(1)

    # Ensure module naming
    if "module @" not in mlir_content:
        mlir_content = re.sub(
            r"^module\s", f"module @{name} ", mlir_content, count=1, flags=re.MULTILINE
        )
        with open(opt_mlir_path, "w") as f:
            f.write(mlir_content)

    # 3. Compile to EmitC Header (.h) & Object (.o)
    arch = defaults.get("target_arch", "rv32")
    if arch == "rv64":
        target_triple = "riscv64-unknown-elf"
        target_abi = "lp64d"
        vm_index_bits = "64"
        output_obj = f"{name}_rv64.o"
    else:
        target_triple = "riscv32-unknown-elf"
        target_abi = "ilp32d"
        vm_index_bits = "32"
        output_obj = f"{name}.o"

    cpu_features = "+m,+a,+f,+d,+zicsr"

    run_command(
        [
            iree_tools["iree-compile"],
            "--iree-hal-target-device=local",
            "--iree-hal-local-target-device-backends=llvm-cpu",
            f"--iree-llvmcpu-target-triple={target_triple}",
            f"--iree-llvmcpu-target-cpu-features={cpu_features}",
            f"--iree-llvmcpu-target-abi={target_abi}",
            "--output-format=vm-c",
            f"--iree-vm-target-index-bits={vm_index_bits}",
            "--iree-llvmcpu-link-embedded=false",
            "--iree-llvmcpu-link-static",
            f"--iree-llvmcpu-static-library-output-path={output_dir}/{output_obj}",
            "--iree-llvmcpu-loop-unrolling=false",
            "--iree-llvmcpu-enable-ukernels=all",
            "--iree-opt-data-tiling",
            "--iree-stream-partitioning-favor=min-peak-memory",
            "--iree-stream-resource-alias-mutable-bindings",
            "--iree-stream-resource-index-bits=32",
            "--iree-stream-resource-memory-model=unified",
            "--iree-stream-resource-max-allocation-size=1048576",
            "--iree-flow-inline-constants-max-byte-length=0",
            "--iree-llvmcpu-debug-symbols=false",
            str(opt_mlir_path),
            "-o",
            f"{output_dir}/{name}.h",
        ]
    )

    return output_obj, metadata


def get_symbol_name(obj_path, pattern="library_query"):
    """Finds the exact symbol name in the object file using nm."""
    try:
        # Use nm from toolchain or system
        # Assuming riscv32-unknown-elf-nm is in path or just nm
        output = run_command(["nm", str(obj_path)])
        for line in output.splitlines():
            if pattern in line and " U " not in line:  # Must be defined (not Undefined)
                # Line format: address T symbol_name
                parts = line.split()
                if len(parts) >= 3:
                    return parts[2]
    except Exception as e:
        print(f"Warning: Could not run nm on {obj_path}: {e}")
        return None
    return None


def generate_code(config, models_metadata, output_dir):

    # 1. ai_models.h
    h_content = """#ifndef AI_MODELS_GEN_H
#define AI_MODELS_GEN_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h> // For abort, abs, etc.
#include <math.h>   // For fabs, etc.

/* --- Wrappers and Descriptors --- */
"""

    for meta in models_metadata:
        name = meta["name"]
        metadata = meta["metadata"]
        h_content += f"/* Model: {name} */\n"

        # Input Struct
        h_content += f"typedef struct {{\n"
        for inp in metadata["inputs"]:
            size = 1
            for dim in inp["shape"]:
                size *= dim
            h_content += (
                f"    {inp['type']} {inp['name']}[{size}]; // Shape: {inp['shape']}\n"
            )
        h_content += f"}} ai_{name}_input_t;\n\n"

        # Output Struct
        h_content += f"typedef struct {{\n"
        for out in metadata["outputs"]:
            size = 1
            for dim in out["shape"]:
                size *= dim
            h_content += (
                f"    {out['type']} {out['name']}[{size}]; // Shape: {out['shape']}\n"
            )
        h_content += f"}} ai_{name}_output_t;\n\n"

        # Run Function
        h_content += f"/**\n * Run inference for {name}\n"
        h_content += f" * Returns 0 on success, non-zero on failure.\n */\n"
        h_content += f"int ai_{name}_run(const ai_{name}_input_t *input, ai_{name}_output_t *output);\n\n"

    h_content += "#endif // AI_MODELS_GEN_H\n"

    with open(output_dir / "ai_models.h", "w") as f:
        f.write(h_content)

    # 2. ai_models_impl.c
    c_content = """#include "ai_models.h"
#include "ai_model_registry.h"
#include "iree/vm/api.h"
#include "iree/hal/local/executable_library.h"
#include <string.h>

/* --- Wrappers and Descriptors --- */
"""

    for meta in models_metadata:
        name = meta["name"]
        c_content += f'#include "{name}.h"\n'

        c_content += f"""
static iree_status_t {name}_create_wrapper(
    iree_vm_instance_t* instance,
    iree_allocator_t allocator,
    iree_vm_module_t** out_module) {{
    return {name}_create(instance, allocator, out_module);
}}
"""
        symbol = meta["symbol"]
        c_content += f"""
extern const iree_hal_executable_library_header_t** {symbol}(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t* environment);
"""

    for meta in models_metadata:
        name = meta["name"]
        symbol = meta["symbol"]
        metadata = meta["metadata"]

        c_content += (
            f"\nstatic const ai_emitc_model_descriptor_t {name}_descriptor = {{\n"
        )
        c_content += f'    .name = "{name}",\n'
        c_content += f'    .entry_function = "{metadata["entry_function"]}",\n'
        c_content += f"    .module_create_fn = {name}_create_wrapper,\n"
        c_content += f"    .library_query_fn = (const void*){symbol},\n"
        c_content += "    .inputs = {\n"
        for inp in metadata["inputs"]:
            dims = ", ".join(map(str, inp["shape"]))
            c_content += f"        {{ .dims = {{{dims}}}, .ndim = {len(inp['shape'])}, .dtype = {inp['dtype']} }},\n"
        c_content += "    },\n"
        c_content += "    .outputs = {\n"
        for out in metadata["outputs"]:
            dims = ", ".join(map(str, out["shape"]))
            c_content += f"        {{ .dims = {{{dims}}}, .ndim = {len(out['shape'])}, .dtype = {out['dtype']} }},\n"
        c_content += "    },\n"
        c_content += f"    .num_inputs = {len(metadata['inputs'])},\n"
        c_content += f"    .num_outputs = {len(metadata['outputs'])}\n"
        c_content += "};\n"

    c_content += "\nconst ai_emitc_model_descriptor_t *g_emitc_models[] = {\n"
    for meta in models_metadata:
        c_content += f"    &{meta['name']}_descriptor,\n"
    c_content += "    NULL\n};\n\n"

    c_content += "/* --- Helper Functions --- */\n"
    for meta in models_metadata:
        name = meta["name"]
        metadata = meta["metadata"]
        num_in = len(metadata["inputs"])
        num_out = len(metadata["outputs"])

        c_content += f"""
int ai_{name}_run(const ai_{name}_input_t *input, ai_{name}_output_t *output) {{
    ai_model_handle_t handle = ai_model_find_by_name("{name}");
    if (!handle) return -1;

    ai_tensor_t in_tensors[{num_in}];
"""
        for i, inp in enumerate(metadata["inputs"]):
            shape_init = ", ".join(map(str, inp["shape"]))
            c_content += f"""    in_tensors[{i}].data = (void*)input->{inp["name"]};
    {{ uint32_t s[] = {{{shape_init}}}; memcpy(in_tensors[{i}].shape, s, sizeof(s)); }}
    in_tensors[{i}].ndim = {len(inp["shape"])};
    in_tensors[{i}].dtype = {inp["dtype"]};
    in_tensors[{i}].size = sizeof(input->{inp["name"]});
"""

        c_content += f"""
    ai_tensor_t out_tensors[{num_out}];
"""
        for i, out in enumerate(metadata["outputs"]):
            shape_init = ", ".join(map(str, out["shape"]))
            c_content += f"""    out_tensors[{i}].data = (void*)output->{out["name"]};
    {{ uint32_t s[] = {{{shape_init}}}; memcpy(out_tensors[{i}].shape, s, sizeof(s)); }}
    out_tensors[{i}].ndim = {len(out["shape"])};
    out_tensors[{i}].dtype = {out["dtype"]};
    out_tensors[{i}].size = sizeof(output->{out["name"]});
"""

        c_content += f"""
    return ai_infer_sync(handle, in_tensors, {num_in}, out_tensors, {num_out}, 1000);
}}
"""

    with open(output_dir / "ai_models.c", "w") as f:
        f.write(c_content)


def generate_cmake(config, models_metadata, output_dir):
    cmake_content = f"""# Auto-generated CMakeLists.txt for AI Models
# This library encapsulates the generated C registry and compiled model object files.

add_library(rv_aios_models STATIC
    ai_models.c
"""

    for meta in models_metadata:
        cmake_content += f"    {meta['obj_file']}\n"

    cmake_content += ")\n\n"

    cmake_content += """target_include_directories(rv_aios_models PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Link against the core AI runtime (rv_aios_ai) to inherit IREE headers and configuration
target_link_libraries(rv_aios_models PUBLIC rv_aios_ai)

# Apply compilation flags required for IREE on bare-metal
target_compile_options(rv_aios_models PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)
"""

    with open(output_dir / "CMakeLists.txt", "w") as f:
        f.write(cmake_content)


def generate_app_scaffolding(config, app_dir):
    app_name = config.get("app_name", "ai_app")
    src_dir = app_dir / "src"
    src_dir.mkdir(parents=True, exist_ok=True)

    # 1. src/hooks.c
    hooks_c = """#include <stdio.h>
#include <stdint.h>
#include "ai_models.h"

/**
 * User-defined pre-processing.
 * e.g., image normalization, resizing, or sensor data scaling.
 */
void ai_app_pre_process(void *input_data, size_t size) {
    // printf("[App] Pre-processing input data...\\n");
    // Default: identity
}

/**
 * User-defined post-processing.
 * e.g., finding max index (argmax), applying thresholds, or formatting strings.
 */
void ai_app_post_process(void *output_data, size_t size) {
    // printf("[App] Post-processing output data...\\n");
    // Example: find argmax for classification
}
"""
    with open(src_dir / "hooks.c", "w") as f:
        f.write(hooks_c)

    # 2. src/main.c
    # Pick the first model as default for main.c template
    model_name = config["models"][0]["name"]
    main_c = f"""#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "ai_models.h"
#include "ai_model_registry.h"
#include "os_config.h"
#include "os_kernel.h"
#include "hal_board.h"
#include "hal_uart.h"
#include "hal_clint.h"

// Task configuration
static os_tcb_t ai_tcb;
static uint64_t ai_stack[8192]; // AI tasks need more stack

// Defined in hooks.c
extern void ai_app_pre_process(void *input_data, size_t size);
extern void ai_app_post_process(void *output_data, size_t size);

void ai_main_task(void *arg) {{
    (void)arg;
    printf("=== AI App Task Started: {app_name} ===\\n");

    // Initialize AI Runtime
    if (ai_runtime_init() != 0) {{
        printf("AI Runtime initialization failed!\\n");
        return;
    }}

    // Static buffers to save stack space
    static ai_{model_name}_input_t input;
    static ai_{model_name}_output_t output;

    while(1) {{
        // 1. Pre-process
        ai_app_pre_process(&input, sizeof(input));

        // 2. Run Inference
        printf("Running inference for {model_name}...\\n");
        int ret = ai_{model_name}_run(&input, &output);
        if (ret != 0) {{
            printf("Inference failed with code: %d\\n", ret);
        }} else {{
            // 3. Post-process
            ai_app_post_process(&output, sizeof(output));
        }}

        printf("Inference cycle complete.\\n");
        os_task_delay(100); // Delay for 100 ticks
    }}
}}

void os_kernel_main(void) {{
    // 1. Hardware Init
    hal_board_init();
    
    printf("Booting AI App: {app_name}...\\n");

    // 2. Kernel Init
    os_kernel_init();
    
    // 3. Create AI Task
    os_task_create(&ai_tcb, "{app_name}_task", ai_main_task, NULL, 
                   10, ai_stack, sizeof(ai_stack));
    
    // 4. Start Kernel
    os_kernel_start();
}}
"""
    with open(src_dir / "main.c", "w") as f:
        f.write(main_c)

    # 3. top-level CMakeLists.txt
    root_cmake = f"""cmake_minimum_required(VERSION 3.20)
project({app_name} C)

# Add the generated AI models library
add_subdirectory(generated)

add_executable({app_name} src/main.c src/hooks.c)

target_link_libraries({app_name}
    rv_aios_models
    rv_aios_libc_shim
    rv_aios_drivers
    rv_aios_kernel
    c m gcc gcc
)

target_include_directories({app_name} PRIVATE
    ${{CMAKE_SOURCE_DIR}}/kernel/include
    ${{CMAKE_SOURCE_DIR}}/hal/include
)

target_compile_options({app_name} PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${{CMAKE_SOURCE_DIR}}/ai/iree_bare_metal_config.h
)

set_target_properties({app_name} PROPERTIES
    LINK_FLAGS "-T${{LINKER_SCRIPT}} -nostartfiles --ld-path=ld.lld -Wl,--gc-sections -Wl,--icf=all -Wl,--allow-multiple-definition"
)

add_custom_command(TARGET {app_name} POST_BUILD
    COMMAND ${{CMAKE_OBJCOPY}} -O binary {app_name} {app_name}.bin
    COMMAND ${{CMAKE_SIZE}} {app_name}
)
"""
    with open(app_dir / "CMakeLists.txt", "w") as f:
        f.write(root_cmake)


def main():
    config_path = PROJECT_ROOT / "ai_models.yaml"
    if not config_path.exists():
        print("Config file not found: ai_models.yaml")
        sys.exit(1)

    with open(config_path, "r") as f:
        config = yaml.safe_load(f)

    app_name = config.get("app_name")
    if app_name:
        app_dir = PROJECT_ROOT / "apps" / app_name
        output_dir = app_dir / "generated"
    else:
        output_dir = PROJECT_ROOT / config.get("output_dir", "generated")
        app_dir = None

    output_dir.mkdir(parents=True, exist_ok=True)

    models_metadata = []

    for model in config["models"]:
        # Compile
        obj_file, metadata = compile_model(
            config, config_path, model, config["defaults"], output_dir
        )

        # Find Symbol
        full_obj_path = output_dir / obj_file
        symbol = get_symbol_name(full_obj_path)
        if not symbol:
            print(f"Error: Could not find library_query symbol in {obj_file}")
            sys.exit(1)

        print(f"  -> Symbol found: {symbol}")

        models_metadata.append(
            {
                "name": model["name"],
                "symbol": symbol,
                "obj_file": obj_file,
                "metadata": metadata,
            }
        )

    # Generate Code
    print("=== Generating Registry Code ===")
    generate_code(config, models_metadata, output_dir)
    generate_cmake(config, models_metadata, output_dir)

    scaffold_targets = (
        [
            app_dir / "CMakeLists.txt",
            app_dir / "src" / "main.c",
            app_dir / "src" / "hooks.c",
        ]
        if app_dir
        else []
    )

    if app_dir and not all(path.exists() for path in scaffold_targets):
        print("=== Generating App Scaffolding ===")
        generate_app_scaffolding(config, app_dir)
    elif app_dir:
        print("=== Skipping App Scaffolding (existing files preserved) ===")

    print("Done! Generated files in:", app_dir if app_dir else output_dir)


if __name__ == "__main__":
    main()
