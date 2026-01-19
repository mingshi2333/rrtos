# RISC-V 32-bit Cross Compilation Toolchain File (Pixi - Clang)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Toolchain binaries (Clang is native, so just use clang)
set(CMAKE_C_COMPILER clang)
set(CMAKE_ASM_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Use LLVM tools
set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJCOPY llvm-objcopy)
set(CMAKE_OBJDUMP llvm-objdump)
set(CMAKE_SIZE llvm-size)

# Target flags (RV32 specific)
set(RISCV_FLAGS "--target=riscv32-unknown-elf -march=rv32imac -mabi=ilp32 -mcmodel=medany")

# === Picolibc 路径配置 ===
if(NOT DEFINED PICOLIBC_ROOT)
  if(DEFINED ENV{CONDA_PREFIX})
    set(PICOLIBC_ROOT "$ENV{CONDA_PREFIX}")
    message(STATUS "Using Picolibc from Pixi environment: ${PICOLIBC_ROOT}")
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32/include/picolibc.h")
    set(PICOLIBC_ROOT "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32")
    message(STATUS "Using Picolibc from .pixi/envs/rv32: ${PICOLIBC_ROOT}")
  else()
    set(PICOLIBC_ROOT "${CMAKE_SOURCE_DIR}/third_party/picolibc_pixi/riscv32-unknown-elf")
    message(STATUS "Using legacy Picolibc path: ${PICOLIBC_ROOT}")
  endif()
else()
  message(STATUS "Using explicitly specified Picolibc: ${PICOLIBC_ROOT}")
endif()

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt -ffunction-sections -fdata-sections -U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=0")
set(CMAKE_ASM_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt")
set(CMAKE_CXX_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt -ffunction-sections -fdata-sections -U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=0")

# Linker flags (Must use lld)
# 查找系统 GCC 的软浮点库 (libgcc)
execute_process(COMMAND /usr/bin/riscv64-linux-gnu-gcc -march=rv32imac -mabi=ilp32 -print-libgcc-file-name OUTPUT_VARIABLE LIBGCC_PATH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if(LIBGCC_PATH AND EXISTS "${LIBGCC_PATH}")
  get_filename_component(LIBGCC_DIR ${LIBGCC_PATH} DIRECTORY)
  message(STATUS "Found libgcc for soft-float: ${LIBGCC_PATH}")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=/usr/bin/riscv64-linux-gnu-ld -Wl,-m,elf32lriscv,--gc-sections,--no-relax -L${LIBGCC_DIR} -lgcc")
else()
  message(WARNING "Could not find libgcc for soft-float. Link errors may occur.")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -Wl,--gc-sections")
endif()

# Don't try to compile test programs
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# RISC-V specific settings
set(RISCV_ARCH "rv32imac" CACHE STRING "RISC-V architecture")
set(RISCV_ABI "ilp32" CACHE STRING "RISC-V ABI")
set(ARCH_BITS "32" CACHE STRING "Target architecture bits (32 or 64)")
