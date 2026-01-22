# Architecture Switching Guide

The project `ai_demo` supports two primary RISC-V architecture configurations.

## 1. Architecture Configuration Comparison

| Configuration | RV32 (Default) | RV64 (Performance) |
| --- | --- | --- |
| **CPU Arch** | `rv32imac` | `rv64imafdcv` |
| **Vectorization** | ❌ No (Scalar) | ✅ Yes (Vector 1.0) |
| **YOLO Performance** | ~4800ms (@100MHz) | ~615ms (@100MHz) |
| **Dependencies** | host libgcc (32-bit compat) | host libgcc (64-bit native) |
| **Stability** | ⭐⭐⭐⭐⭐ (High Compatibility) | ⭐⭐⭐⭐ (Host Dependency) |

## 2. Switching Steps

### Switch to RV32 (32-bit Scalar)
This is the default stable mode, suitable for low-cost MCUs (e.g., ESP32-C3, BL602).

```bash
# 1. Re-convert model (using RV32 parameters)
pixi run convert-model zoo/models/tiny/st_mnist_28_int8.tflite st_mnist_28

# 2. Clean and compile
pixi run -e rv32 configure
pixi run -e rv32 build

# 3. Run simulation (RV32)
pixi run sim
```

### Switch to RV64 (64-bit Vector)
This is the high-performance mode, suitable for high-end cores with Vector extensions (e.g., C906, Ara).

```bash
# 1. Re-convert model (using RV64 + Vector parameters)
# (Currently manual adjustment in scripts/tflite_to_iree_c.sh may be needed)

# 2. Clean and compile
pixi run -e rv64 configure
pixi run -e rv64 build

# 3. Run simulation (RV64)
pixi run sim
```

## 3. Configuration Modification Points (Reference)

If you need to manually modify the configuration, it mainly involves the following files:

1.  **`cmake/riscv[32/64]-pixi.cmake`**:
    *   Defines the cross-compilation toolchain and libgcc search paths.
    *   RV64 requires proper `execute_process` to find Host GCC libraries.
2.  **`scripts/tflite_to_iree_c.sh`**:
    *   RV32: `CPU_FEATURES="+m,+a,+f,+d,+c"`
    *   RV64: `CPU_FEATURES="+m,+a,+f,+d,+c,+v"` (Enable Vector)
3.  **Renode Script (`.resc` / `.repl`)**:
    *   Ensure the CPU type and extensions in the `.repl` file match the target architecture.

*Last Updated: Jan 19, 2025*
