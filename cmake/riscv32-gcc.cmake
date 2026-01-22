# RISC-V GCC Toolchain for RV32 (using riscv64-linux-gnu-gcc with multilib)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)

# Cross-compilation mode: skip link check
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Use system-installed GCC
set(CMAKE_C_COMPILER /usr/bin/riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/riscv64-linux-gnu-g++)
set(CMAKE_ASM_COMPILER /usr/bin/riscv64-linux-gnu-gcc)

# Target architecture configuration
set(RISCV_ARCH "rv32imac")
set(RISCV_ABI "ilp32")
set(ARCH_BITS "32")

# Compiler flags
set(CMAKE_C_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -mcmodel=medany -ffunction-sections -fdata-sections -g" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -mcmodel=medany -ffunction-sections -fdata-sections -g -fno-exceptions -fno-rtti" CACHE STRING "C++ flags")
set(CMAKE_ASM_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -mcmodel=medany -g" CACHE STRING "ASM flags")

# Linker flags - key is to use -nostdlib but explicitly link libgcc
set(CMAKE_EXE_LINKER_FLAGS "-march=${RISCV_ARCH} -mabi=${RISCV_ABI} -nostdlib -static -Wl,--gc-sections" CACHE STRING "Linker flags")

# Find tools
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
