cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(rrtos
    VERSION 1.0.0
    DESCRIPTION "RISC-V AI-RTOS with Federated Learning Support"
    LANGUAGES C ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

link_libraries(gcc gcc)

# ============================================================================
# Picolibc Integration Configuration
# ============================================================================
message(STATUS "Checking Picolibc at: ${PICOLIBC_ROOT}")

if(EXISTS "${PICOLIBC_ROOT}/include/picolibc.h")
    message(STATUS "Found picolibc headers at: ${PICOLIBC_ROOT}/include")
    include_directories(BEFORE SYSTEM ${PICOLIBC_ROOT}/include)
else()
    file(GLOB_RECURSE PICOLIBC_INCLUDE_DIRS "${PICOLIBC_ROOT}/*/include")
    if(PICOLIBC_INCLUDE_DIRS)
        list(GET PICOLIBC_INCLUDE_DIRS 0 FOUND_INC)
        message(STATUS "Found cross-compiled picolibc headers at: ${FOUND_INC}")
        include_directories(BEFORE SYSTEM ${FOUND_INC})
    else()
        message(STATUS "Fallback to: ${PICOLIBC_ROOT}/include")
        include_directories(BEFORE SYSTEM ${PICOLIBC_ROOT}/include)
    endif()
endif()

link_directories(${PICOLIBC_ROOT}/lib)
add_compile_options(-nostdlib -ffunction-sections -fdata-sections)
add_link_options(-nostdlib -Wl,--gc-sections -Wl,--strip-all -L${PICOLIBC_ROOT}/lib)

option(OS_SMP_EN "Enable SMP support" ON)
option(OS_AI_EN "Enable AI runtime" ON)
option(OS_FL_EN "Enable Federated Learning" ON)
option(OS_DEBUG "Enable debug build" ON)

# Architecture Selection
set(ARCH_BITS "64" CACHE STRING "Target architecture bits (32 or 64)")

# Central RISC-V ISA configuration
include(${CMAKE_SOURCE_DIR}/cmake/riscv_arch.cmake)

if(ARCH_BITS STREQUAL "32")
    if(RISCV_ABI STREQUAL "ilp32d")
        # Hard-Float (rv32imafdcv)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections -Oz")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI}")
    else()
        # Soft-Float (rv32imacv) - Default
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32 -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections")
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32")
    endif()
    add_definitions(-DOS_CFG_ARCH_BITS=32)
    set(ARCH "riscv")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -Wall -Wextra -ffunction-sections -fdata-sections")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany")
    add_definitions(-DOS_CFG_ARCH_BITS=64)
    set(ARCH_BITS "64")
    set(ARCH "riscv")
endif()

# Linker Script
if(NOT DEFINED LINKER_SCRIPT)
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/arch/${ARCH}/link.ld")
else()
    if(NOT IS_ABSOLUTE "${LINKER_SCRIPT}")
        set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}")
    endif()
endif()

# ============================================================================
# IREE Configuration (Integrated)
# ============================================================================
set(IREE_BUILD_COMPILER OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_MLIR OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_THREADING OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_BINDINGS_TFLITE OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_LOADER_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_PLUGIN_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_PLUGIN_EMBEDDED_ELF OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_LOCAL_SYNC ON CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_LOADER_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(IREE_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_WERROR_FLAG OFF CACHE BOOL "" FORCE)
add_compile_options(-Wno-error)
set(IREE_ERROR_ON_MISSING_SUBMODULES OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_CUDA OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_HIP OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_VULKAN OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_METAL OFF CACHE BOOL "" FORCE)
set(IREE_EXTERNAL_HAL_DRIVERS "" CACHE STRING "" FORCE)

# Optimization Flags
set(IREE_ENABLE_CPUINFO OFF CACHE BOOL "" FORCE)
set(IREE_TRACING_MODE 0 CACHE STRING "" FORCE)

add_compile_definitions(IREE_PLATFORM_GENERIC)
add_compile_definitions(IREE_SYNCHRONIZATION_DISABLE_UNSAFE=1)
add_compile_definitions(IREE_STATUS_FEATURES=0)
add_compile_definitions(IREE_FILE_IO_ENABLE=0)
add_compile_definitions(IREE_HAVE_POSIX_MEMALIGN=0)
add_compile_definitions(IREE_HAVE_STDIO=0)
add_compile_definitions(AI_IREE_ALLOC_TRACE=1)
add_compile_definitions(_POSIX_C_SOURCE=0)
add_compile_definitions(_ISOC11_SOURCE=0)

include_directories(${CMAKE_SOURCE_DIR}/ai/include)
add_definitions(-DIREE_USER_CONFIG_H=\"iree_config_custom.h\")

add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)

# ============================================================================
# RTOS Core Modules
# ============================================================================
add_subdirectory(memory)
add_subdirectory(kernel)
add_subdirectory(hal)

add_library(rv_aios_rtos INTERFACE)
target_link_libraries(rv_aios_rtos INTERFACE
    rv_aios_memory
    rv_aios_kernel
    rv_aios_drivers
)

# ============================================================================
# AI Module
# ============================================================================
add_subdirectory(ai)

# ============================================================================
# Applications
# ============================================================================
add_subdirectory(apps/ai_demo)
