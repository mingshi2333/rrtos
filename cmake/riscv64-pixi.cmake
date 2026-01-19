# RISC-V 64-bit Cross Compilation Toolchain File (Pixi - Clang)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

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

# Target flags
set(RISCV_FLAGS "--target=riscv64-unknown-elf -march=rv64imafdcv -mabi=lp64d -mcmodel=medany")

# === Picolibc 路径配置（优先使用 CMake -D 参数，否则尝试环境变量）===
if(NOT DEFINED PICOLIBC_ROOT)
  if(DEFINED ENV{CONDA_PREFIX})
    # Pixi build 将 Picolibc 直接安装到 $PREFIX/ 而不是 $PREFIX/riscv64-unknown-elf/
    set(PICOLIBC_ROOT "$ENV{CONDA_PREFIX}")
    message(STATUS "Using Picolibc from Pixi environment: ${PICOLIBC_ROOT}")
  else()
    # Fallback：向后兼容旧的手动构建路径
    set(PICOLIBC_ROOT "${CMAKE_SOURCE_DIR}/third_party/picolibc_pixi/riscv64-unknown-elf")
    message(STATUS "Using legacy Picolibc path: ${PICOLIBC_ROOT}")
  endif()
else()
  message(STATUS "Using explicitly specified Picolibc: ${PICOLIBC_ROOT}")
endif()

# Compiler flags
set(CMAKE_C_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles")
set(CMAKE_ASM_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles")
set(CMAKE_CXX_FLAGS_INIT "${RISCV_FLAGS} -ffreestanding -nostdlib -nostartfiles")

# Find libgcc
execute_process(
    COMMAND /usr/bin/riscv64-linux-gnu-gcc -march=rv64imafdcv -mabi=lp64d -print-libgcc-file-name
    OUTPUT_VARIABLE LIBGCC_FILE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
get_filename_component(LIBGCC_DIR ${LIBGCC_FILE} DIRECTORY)
message(STATUS "Found libgcc at: ${LIBGCC_DIR}")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -L${LIBGCC_DIR} -lgcc")

# Don't try to compile test programs
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# RISC-V specific settings
set(RISCV_ARCH "rv64imafdcv" CACHE STRING "RISC-V architecture")
set(RISCV_ABI "lp64d" CACHE STRING "RISC-V ABI")
set(ARCH_BITS "64" CACHE STRING "Target architecture bits (32 or 64)")