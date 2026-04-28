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

# === Picolibc path configuration ===
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

# Linker flags.
# Probe a GCC toolchain that can supply RV32 libgcc. Different distributions
# use different multilib layouts: linux-gnu commonly uses lib32/<abi>, while
# unknown-elf commonly uses ISA/ABI directory names. Accept only probes whose
# selected multilib clearly names an RV32/ILP32 variant.
set(LIBGCC_PATH "")
set(LIBGCC_PROBE "")
foreach(LIBGCC_CANDIDATE riscv64-unknown-elf-gcc riscv64-linux-gnu-gcc)
  execute_process(
    COMMAND ${LIBGCC_CANDIDATE} -march=${RISCV_MARCH} -mabi=${RISCV_MABI} -print-libgcc-file-name
    OUTPUT_VARIABLE LIBGCC_CANDIDATE_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE LIBGCC_CANDIDATE_RC
  )
  execute_process(
    COMMAND ${LIBGCC_CANDIDATE} -march=${RISCV_MARCH} -mabi=${RISCV_MABI} -print-multi-directory
    OUTPUT_VARIABLE LIBGCC_CANDIDATE_MULTILIB
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )
  if(LIBGCC_CANDIDATE_RC EQUAL 0 AND LIBGCC_CANDIDATE_PATH AND EXISTS "${LIBGCC_CANDIDATE_PATH}" AND
      (LIBGCC_CANDIDATE_PATH MATCHES "/lib32/" OR LIBGCC_CANDIDATE_MULTILIB MATCHES "(rv32|ilp32)"))
    set(LIBGCC_PATH "${LIBGCC_CANDIDATE_PATH}")
    set(LIBGCC_PROBE "${LIBGCC_CANDIDATE}")
    break()
  endif()
endforeach()

if(NOT LIBGCC_PATH AND RISCV_MABI STREQUAL "ilp32f")
  foreach(LIBGCC_CANDIDATE riscv64-unknown-elf-gcc riscv64-linux-gnu-gcc)
    execute_process(
      COMMAND ${LIBGCC_CANDIDATE} -march=rv32imac -mabi=ilp32 -print-libgcc-file-name
      OUTPUT_VARIABLE LIBGCC_CANDIDATE_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
      RESULT_VARIABLE LIBGCC_CANDIDATE_RC
    )
    execute_process(
      COMMAND ${LIBGCC_CANDIDATE} -march=rv32imac -mabi=ilp32 -print-multi-directory
      OUTPUT_VARIABLE LIBGCC_CANDIDATE_MULTILIB
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(LIBGCC_CANDIDATE_RC EQUAL 0 AND LIBGCC_CANDIDATE_PATH AND EXISTS "${LIBGCC_CANDIDATE_PATH}" AND
        (LIBGCC_CANDIDATE_PATH MATCHES "/lib32/" OR LIBGCC_CANDIDATE_MULTILIB MATCHES "(rv32|ilp32)"))
      set(LIBGCC_PATH "${LIBGCC_CANDIDATE_PATH}")
      set(LIBGCC_PROBE "${LIBGCC_CANDIDATE} ilp32 multilib fallback")
      break()
    endif()
  endforeach()
endif()

if(LIBGCC_PATH)
  get_filename_component(LIBGCC_DIR ${LIBGCC_PATH} DIRECTORY)
  message(STATUS "Found RV32 libgcc via ${LIBGCC_PROBE}: ${LIBGCC_PATH}")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -Wl,--gc-sections -Wl,--no-relax -L${LIBGCC_DIR} ${LIBGCC_PATH}")
else()
  message(WARNING "Could not find RV32 hard-float libgcc. Link errors may occur.")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "${RISCV_FLAGS} -nostdlib -nostartfiles -static -fuse-ld=lld -Wl,--gc-sections -Wl,--no-relax")
endif()

if(DEFINED CONFIG_BOARD AND CONFIG_BOARD STREQUAL "be_u1000")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_INIT}" CACHE STRING "C compiler flags" FORCE)
  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS_INIT}" CACHE STRING "ASM compiler flags" FORCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT}" CACHE STRING "CXX compiler flags" FORCE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT}" CACHE STRING "Executable linker flags" FORCE)
endif()

# Don't try to compile test programs
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# RISC-V specific settings
set(RISCV_ARCH "${RISCV_MARCH}" CACHE STRING "RISC-V architecture")
set(RISCV_ABI "${RISCV_MABI}" CACHE STRING "RISC-V ABI")
set(ARCH_BITS "32" CACHE STRING "Target architecture bits (32 or 64)")
