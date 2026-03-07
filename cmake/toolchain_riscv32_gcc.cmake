# CMake toolchain file for RISC-V 32-bit GCC (BE-U1000 SDK)
#
# This toolchain file configures CMake to use the riscv32-none-elf GCC
# compiler from the BE-U1000 SDK. Set BE_U1000_SDK_PATH to the SDK root.
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_riscv32_gcc.cmake \
#         -DBE_U1000_SDK_PATH=/path/to/mcu-sdk-linux_2.1.1 ...

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# SDK toolchain path
if(NOT DEFINED BE_U1000_SDK_PATH)
    # Default: check if extracted to /tmp or user home
    if(EXISTS "$ENV{HOME}/Downloads/mcu-sdk-linux_2.1.1/Tools/toolchain")
        set(BE_U1000_SDK_PATH "$ENV{HOME}/Downloads/mcu-sdk-linux_2.1.1")
    elseif(EXISTS "/tmp/mcu-sdk-extract/Tools/toolchain")
        set(BE_U1000_SDK_PATH "/tmp/mcu-sdk-extract")
    else()
        message(WARNING "BE_U1000_SDK_PATH not set. Using system riscv32 GCC.")
    endif()
endif()

# Find the GCC toolchain
if(DEFINED BE_U1000_SDK_PATH)
    # The SDK ships multiple ISA variants; pick the base IMC one
    file(GLOB _TC_DIRS "${BE_U1000_SDK_PATH}/Tools/toolchain/riscv32-none-elf-gcc-14.2.0-*")
    if(_TC_DIRS)
        list(GET _TC_DIRS 0 _TC_DIR)
        set(CROSS_PREFIX "${_TC_DIR}/bin/riscv32-none-elf-")
    endif()
endif()

if(NOT DEFINED CROSS_PREFIX)
    set(CROSS_PREFIX "riscv32-none-elf-")
endif()

set(CMAKE_C_COMPILER ${CROSS_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CROSS_PREFIX}gcc)
set(CMAKE_OBJCOPY ${CROSS_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${CROSS_PREFIX}objdump)
set(CMAKE_SIZE ${CROSS_PREFIX}size)
set(CMAKE_AR ${CROSS_PREFIX}ar)

# RISC-V architecture flags for BE-U1000
# Base: rv32imafc (Integer, Multiply, Atomic, Float, Compressed)
set(RISCV_MARCH "rv32imafc_zicsr_zifencei" CACHE STRING "RISC-V march for BE-U1000")
set(RISCV_MABI "ilp32f" CACHE STRING "RISC-V mabi for BE-U1000")

set(CMAKE_C_FLAGS_INIT
    "-march=${RISCV_MARCH} -mabi=${RISCV_MABI} -mcmodel=medlow \
     -ffunction-sections -fdata-sections -fno-common \
     -nostdlib -ffreestanding"
)
set(CMAKE_ASM_FLAGS_INIT
    "-march=${RISCV_MARCH} -mabi=${RISCV_MABI} -mcmodel=medlow"
)
set(CMAKE_EXE_LINKER_FLAGS_INIT
    "-nostdlib -Wl,--gc-sections -Wl,--print-memory-usage"
)

# Don't try to compile test programs (cross-compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
