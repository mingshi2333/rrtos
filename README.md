# RISC-V RTOS with IREE Runtime

A bare-metal RISC-V real-time operating system (RTOS) project featuring IREE (IREE Efficient Reliable Execution) runtime integration.

## Architecture

- **Target**: RISC-V 32/64-bit (RV32IMAC, RV64IMAC)
- **C Library**: Picolibc (optimized for embedded systems)
- **ML Runtime**: IREE Runtime (minimal embedded configuration)
- **Build System**: CMake + Pixi (cross-platform package manager)
- **Toolchain**: Clang/LLVM 18+

## Quick Start

### Prerequisites

- [Pixi](https://pixi.sh/) (package manager)
- Git

### Build Instructions

#### 1. Initial Setup

```bash
# Clone the repository
git clone <repo-url>
cd rtos

# Get Picolibc source code
pixi run get-picolibc

# Initialize IREE submodule
pixi run init-iree
```

#### 2. Build Picolibc (Source Build via Pixi)

The project uses **Pixi Source Build** to automatically build Picolibc from source:

```bash
# Build Picolibc for RV64 (default)
cd packages/picolibc-rv64
pixi build

# Or build for RV32
cd packages/picolibc-rv32
pixi build
```

This will:
- Compile Picolibc using Clang cross-compiler
- Install to `.pixi/envs/rv64/` (or `rv32`)
- Cache the build for incremental compilation

#### 3. Build the Main Project

```bash
# Build for RV64 (default)
pixi run -e rv64 build

# Or build for RV32
pixi run -e rv32 build
```

#### 4. Run Simulation

```bash
# QEMU simulation
pixi run sim

# Renode simulation
pixi run sim-renode
```

## Testing and Simulation

### Build and Run (RV32)

```bash
pixi run -e rv32 configure
pixi run -e rv32 build

# QEMU
./scripts/run_qemu.sh

# Renode
renode --disable-xwt --console --plain scripts/run_renode.resc
```

### Performance Data (Renode)

Measured on the latest RV32 build (no C/V extensions):

| Clock | Inference Time |
| --- | --- |
| 100 MHz | 5003 ms |
| 480 MHz | 1042 ms |

Raw log: `/tmp/renode_output.log`

### Model Size Analysis (YOLO)

| Artifact | Path | Size |
| --- | --- | --- |
| EmitC header | `zoo/iree_static/st_yolo_lc_v1_192_int_beta.h` | 1.6M |
| Static object | `zoo/iree_static/st_yolo_lc_v1_192_int_beta.o` | 289K |
| VM bytecode | `zoo/iree_static/st_yolo_lc_v1_192_int_beta.vmfb` | 535K |

### Model Regeneration

Use the model script to rebuild the YOLO artifacts with the current ISA config:

```bash
cd zoo/scripts
./tflite_to_iree_c.sh -a rv32 ../../../iree/zoo/st_yolo_lc_v1_192.tflite st_yolo_lc_v1_192_int_beta
```

### ISA Configuration

The project uses a centralized ISA config at `cmake/riscv_arch.cmake`. For Renode
compatibility, RV32 builds default to `rv32imafd` (no C/V). If any linked object
contains C instructions, Renode will fault with a compressed instruction decode error.

## Architecture Switching

The project supports both RV32 and RV64 through Pixi environments:

```bash
# Switch to RV32
pixi run -e rv32 configure
pixi run -e rv32 build

# Switch back to RV64
pixi run -e rv64 configure
pixi run -e rv64 build
```

## Project Structure

```
rtos/
├── pixi.toml                    # Pixi workspace configuration
├── packages/                    # Source-built packages
│   ├── picolibc-rv64/          # Picolibc for RV64
│   │   └── recipe.yaml
│   └── picolibc-rv32/          # Picolibc for RV32
│       └── recipe.yaml
├── third_party/
│   ├── picolibc_src/           # Picolibc source (git clone)
│   └── iree/                   # IREE runtime (submodule)
├── cmake/
│   ├── riscv64-pixi.cmake      # RV64 toolchain file
│   └── riscv32-pixi.cmake      # RV32 toolchain file
├── src/                        # Application source code
└── CMakeLists.txt              # Main build configuration
```

## Pixi Source Build

This project uses **Pixi Source Build** (preview feature) to manage Picolibc as a source dependency:

### Benefits

1. **Automatic Management**: No manual build scripts needed
2. **Reproducible**: `pixi.lock` ensures consistent dependencies
3. **Incremental Builds**: Pixi caches build artifacts
4. **Multi-Architecture**: Easy switching between RV32/RV64

### How It Works

1. `packages/picolibc-rv64/recipe.yaml` defines the build process
2. `pixi build` compiles Picolibc using Meson + Clang
3. Package is installed to `.pixi/envs/rv64/lib/libc.a`
4. CMake toolchain automatically detects and uses it

## Manual Build (Legacy)

If you prefer manual builds without Pixi Source Build:

```bash
# Build Picolibc manually
pixi run build-picolibc-legacy

# Then build the project
pixi run build
```

## Development

### Available Tasks

```bash
pixi task list                  # List all available tasks
pixi run configure              # Configure CMake
pixi run build                  # Build project
pixi run clean                  # Clean build artifacts
pixi run rebuild                # Clean + build
pixi run sim                    # Run QEMU simulation
pixi run verify                 # Build + simulate
```

### Debugging

```bash
# Build with debug symbols
pixi run configure -- -DCMAKE_BUILD_TYPE=Debug
pixi run build

# Run with GDB
qemu-system-riscv64 -machine virt -s -S -nographic -bios none -kernel build/iree_simple_test
# In another terminal:
gdb build/iree_simple_test
(gdb) target remote :1234
```

## Troubleshooting

### Picolibc Build Fails

```bash
# Clean build cache
rm -rf packages/picolibc-rv64/{build,output}
rm -rf .pixi/build

# Rebuild
cd packages/picolibc-rv64
pixi build
```

### CMake Can't Find Picolibc

Make sure you're running CMake within Pixi environment:

```bash
# Wrong (missing environment)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/riscv64-pixi.cmake

# Correct (Pixi sets $CONDA_PREFIX)
pixi run -e rv64 configure
```

### Environment Issues

```bash
# Reinstall environment
pixi install -e rv64

# Check installed packages
pixi list -e rv64
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test with both RV32 and RV64
5. Submit a pull request

## License

[Your License Here]

## References

- [Pixi Documentation](https://pixi.sh/)
- [Picolibc](https://github.com/picolibc/picolibc)
- [IREE](https://github.com/iree-org/iree)
- [RISC-V Specification](https://riscv.org/technical/specifications/)
