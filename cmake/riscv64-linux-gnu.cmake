# RISC-V 64-bit Cross Compilation Toolchain File
# For use with riscv64-linux-gnu-gcc on Fedora

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Toolchain paths
set(CROSS_COMPILE riscv64-linux-gnu-)

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump)
set(CMAKE_SIZE ${CROSS_COMPILE}size)
set(CMAKE_AR ${CROSS_COMPILE}ar)

# Compiler flags for bare-metal
set(CMAKE_C_FLAGS_INIT "-ffreestanding -nostdlib -nostartfiles")
set(CMAKE_ASM_FLAGS_INIT "-ffreestanding -nostdlib -nostartfiles")

# Disable standard library linking
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostdlib -nostartfiles -static")

# Don't try to compile test programs (they will fail without runtime)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# RISC-V specific settings
set(RISCV_ARCH "rv64imac" CACHE STRING "RISC-V architecture")
set(RISCV_ABI "lp64" CACHE STRING "RISC-V ABI")
