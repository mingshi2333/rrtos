# RISC-V 32-bit Hard-Float Cross Compilation Toolchain File (Pixi - Clang)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Central RISC-V ISA configuration
set(ARCH_BITS "32")
include(${CMAKE_CURRENT_LIST_DIR}/riscv_arch.cmake)

# Toolchain binaries
if(DEFINED ENV{CONDA_PREFIX})
  set(TOOLCHAIN_BIN "$ENV{CONDA_PREFIX}/bin")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32/bin/clang")
  set(TOOLCHAIN_BIN "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32/bin")
else()
  set(TOOLCHAIN_BIN "")
endif()

if(TOOLCHAIN_BIN STREQUAL "")
  set(CMAKE_C_COMPILER clang)
  set(CMAKE_ASM_COMPILER clang)
  set(CMAKE_CXX_COMPILER clang++)
else()
  set(CMAKE_C_COMPILER "${TOOLCHAIN_BIN}/clang")
  set(CMAKE_ASM_COMPILER "${TOOLCHAIN_BIN}/clang")
  if(EXISTS "${TOOLCHAIN_BIN}/clang++")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN}/clang++")
  elseif(EXISTS "${TOOLCHAIN_BIN}/clang++-21")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN}/clang++-21")
  else()
    set(CMAKE_CXX_COMPILER clang++)
  endif()
endif()

# Use LLVM tools
set(CMAKE_AR llvm-ar)
set(CMAKE_RANLIB llvm-ranlib)
set(CMAKE_OBJCOPY llvm-objcopy)
set(CMAKE_OBJDUMP llvm-objdump)
set(CMAKE_SIZE llvm-size)

# Target flags (RV32 hard-float ABI)
set(RISCV_FLAGS "--target=riscv32-unknown-elf -march=${RISCV_MARCH} -mabi=${RISCV_MABI} -mcmodel=medany")

# === Picolibc 路径配置 ===
if(NOT DEFINED PICOLIBC_ROOT)
  if(DEFINED ENV{CONDA_PREFIX})
    set(PICOLIBC_ROOT "$ENV{CONDA_PREFIX}")
    message(STATUS "Using Picolibc from Pixi environment: ${PICOLIBC_ROOT}")
  elseif(EXISTS "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32/include/picolibc.h")
    set(PICOLIBC_ROOT "${CMAKE_SOURCE_DIR}/.pixi/envs/rv32")
    message(STATUS "Using Picolibc from .pixi/envs/rv32: ${PICOLIBC_ROOT}")
  else()
    message(STATUS "Using legacy Picolibc path: ${CMAKE_SOURCE_DIR}/third_party/picolibc_pixi/riscv32-unknown-elf")
  endif()
else()
  message(STATUS "Using explicitly specified Picolibc: ${PICOLIBC_ROOT}")
endif()

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt -ffunction-sections -fdata-sections -U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=0")
set(CMAKE_ASM_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt")
set(CMAKE_CXX_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-plt -ffunction-sections -fdata-sections -U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=0")

# Linker flags (use lld for ilp32d)
# 查找系统 GCC 的硬浮点库 (libgcc)
execute_process(COMMAND /usr/bin/riscv64-linux-gnu-gcc -march=${RISCV_MARCH} -mabi=${RISCV_MABI} -print-libgcc-file-name OUTPUT_VARIABLE LIBGCC_PATH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if(LIBGCC_PATH AND EXISTS "${LIBGCC_PATH}")
  get_filename_component(LIBGCC_DIR ${LIBGCC_PATH} DIRECTORY)
  message(STATUS "Found libgcc for hard-float: ${LIBGCC_PATH}")
  # 添加 -L${LIBGCC_DIR} -lgcc 来显式链接
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -Wl,--gc-sections -Wl,--no-relax -L${LIBGCC_DIR} -lgcc")
else()
  message(WARNING "Could not find libgcc for hard-float. Link errors may occur.")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -Wl,--gc-sections -Wl,--no-relax")
endif()

# Don't try to compile test programs
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# RISC-V specific settings
set(RISCV_ARCH "${RISCV_MARCH}" CACHE STRING "RISC-V architecture")
set(RISCV_ABI "${RISCV_MABI}" CACHE STRING "RISC-V ABI")
set(ARCH_BITS "32" CACHE STRING "Target architecture bits (32 or 64)")
