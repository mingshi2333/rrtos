# IREE Bare-Metal Workflow for RISC-V (RV32/RV64)

This document details how to deploy TFLite models into a bare-metal RISC-V RTOS environment, supporting both RV32 and RV64 architectures.

## 1. Environment Preparation

### 1.1 Pixi (Build System)
Used for managing the RISC-V toolchain and Picolibc.
```bash
# Activate the environment
pixi shell -e rv32  # or rv64
```

### 1.2 IREE Toolchain (Compiler)
Used for compiling models into C code.
- Path: `/home/mingshi/.mamba/envs/iree-toolchain310/`
- Includes: `iree-import-tflite`, `iree-opt`, `iree-compile`

## 2. Model Zoo Structure

```
zoo/
├── models/                  # Original TFLite models
│   ├── tiny/
│   │   └── st_mnist_28_int8.tflite
│   └── small/
│       └── miniresnet_int8.tflite
├── iree_static/             # Converted C code
│   ├── st_mnist_28.h        # Interface definition (C wrapper)
│   ├── st_mnist_28.o        # RV32 object file
│   └── st_mnist_28_rv64.o   # RV64 object file
└── scripts/
    └── tflite_to_iree_c.sh  # Automated conversion script
```

## 3. Conversion Workflow (TFLite -> C)

Use our automated script `tflite_to_iree_c.sh`. This script handles:
- **INT8 Quantization**: Automatically converts `ui8` to IREE-supported `i8`.
- **Soft-Float Linking**: Generates unlinked object files, deferring linking to the CMake stage.
- **Multi-Architecture**: Supports `-a rv32` or `-a rv64`.

### Usage

```bash
cd zoo/scripts

# Generate RV32 version (.o)
./tflite_to_iree_c.sh -a rv32 ../models/tiny/model.tflite model_name

# Generate RV64 version (_rv64.o)
./tflite_to_iree_c.sh -a rv64 ../models/tiny/model.tflite model_name
```

**Output**:
- `zoo/iree_static/model_name.h` (Common header)
- `zoo/iree_static/model_name.o` (RV32)
- `zoo/iree_static/model_name_rv64.o` (RV64)

## 4. Integration into CMake

In `CMakeLists.txt`, you need to:
1.  Add a C Wrapper (`.c` including the `.h`).
2.  Dynamically select which object file to link based on `ARCH_BITS`.

### Example Configuration

```cmake
# 1. Define source files
set(AI_SOURCES
    # ... other source files
    zoo/iree_static/my_model_c_module.c  # C wrapper including the header
    ${IREE_RUNTIME_SOURCES}
)

# 2. Dynamically link object files
if(ARCH_BITS EQUAL 64)
    message(STATUS "Linking RV64 model object")
    list(APPEND AI_SOURCES ${CMAKE_SOURCE_DIR}/zoo/iree_static/my_model_rv64.o)
else()
    message(STATUS "Linking RV32 model object")
    list(APPEND AI_SOURCES ${CMAKE_SOURCE_DIR}/zoo/iree_static/my_model.o)
endif()

add_library(rv_aios_ai STATIC ${AI_SOURCES})
```

## 5. Compilation and Execution

Use Pixi for one-click building and simulation.

### RV32
```bash
pixi run -e rv32 rebuild       # Clean and build
pixi run -e rv32 sim-mobilenet # QEMU simulation
```

### RV64
```bash
pixi run -e rv64 rebuild
pixi run -e rv64 sim-mobilenet
```

## 6. Technical Principles

- **EmitC + Embedded Object**: We use the `vm-c` format for generated C control flow code and the `llvm-cpu` backend for optimized binary operators.
- **Deferred Linking**: Using `--iree-llvmcpu-link-embedded=false` tells the IREE compiler not to attempt linking into an ELF. We hand over the generated `.o` file to the main project's GCC/Clang for final linking, automatically resolving `libgcc` (soft-float) dependencies.
