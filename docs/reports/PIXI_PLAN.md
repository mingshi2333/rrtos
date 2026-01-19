# Pixi Implementation Plan

## 1. Toolchain Management
Pixi will manage the entire development environment, replacing manual apt/brew installs.

### Dependencies (pixi.toml)
- **Core Build**: `cmake`, `make`, `ninja`
- **Compiler (Host)**: `clang`, `lld`, `llvm` (for tools/verification)
- **Compiler (Target)**: `gcc-riscv64-linux-gnu`, `binutils-riscv64-linux-gnu` (from conda-forge)
- **Simulation**: `qemu`, `renode`
- **Scripting**: `python`

## 2. Task Automation
We will define standard tasks to standardize the workflow:

| Task | Command | Description |
|------|---------|-------------|
| `init` | `git submodule ...` | Initialize third-party deps |
| `configure` | `cmake -B build ...` | Configure CMake with toolchain |
| `build` | `cmake --build build` | Compile the RTOS |
| `clean` | `rm -rf build` | Clean build artifacts |
| `sim` | `qemu ...` | Run basic verification |

## 3. Third-Party Strategy
We will NOT use pixi to install IREE/Flatbuffers libraries directly, because:
1. They are embedded dependencies needed as source.
2. We already have them as git submodules.
3. We need precise control over their compilation flags (e.g., no stdlib).

**Strategy**: Use `pixi` to provide the *tools* (cmake, compilers) to build the *existing* submodules.

## 4. Execution
1. Initialize `pixi` project.
2. Add dependencies.
3. Define tasks.
4. Verify build inside `pixi run build`.
