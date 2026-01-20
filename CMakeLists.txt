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
# Picolibc Integration Configuration (Must be AFTER project() call)
# ============================================================================
# PICOLIBC_ROOT is set by toolchain file (cmake/riscv64-pixi.cmake or riscv32-pixi.cmake)
# Toolchain file is loaded during project() call.

# Handle Picolibc include layout (Native vs Cross)
message(STATUS "Checking Picolibc at: ${PICOLIBC_ROOT}")

if(EXISTS "${PICOLIBC_ROOT}/include/picolibc.h")
    message(STATUS "Found picolibc headers at: ${PICOLIBC_ROOT}/include")
    include_directories(BEFORE SYSTEM ${PICOLIBC_ROOT}/include)
else()
    # Cross-compiled layout often has includes under architecture tuple (e.g. riscv64-unknown-elf/include)
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
add_link_options(-nostdlib -Wl,--gc-sections -L${PICOLIBC_ROOT}/lib)

option(OS_SMP_EN "Enable SMP support" ON)
option(OS_AI_EN "Enable AI runtime" ON)
option(OS_FL_EN "Enable Federated Learning" ON)
option(OS_DEBUG "Enable debug build" ON)

# Architecture Selection
set(ARCH_BITS "64" CACHE STRING "Target architecture bits (32 or 64)")

if(ARCH_BITS STREQUAL "32")
    # RV32
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32 -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32")
    add_definitions(-DOS_CFG_ARCH_BITS=32)
    set(ARCH "riscv")
else()
    # RV64 (Default)
    # Enable Vector (+v), Float (+f/+d), Compressed (+c)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv64imafdcv_zicsr -mabi=lp64d -mcmodel=medany -Wall -Wextra -ffunction-sections -fdata-sections")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv64imafdcv_zicsr -mabi=lp64d -mcmodel=medany")
    add_definitions(-DOS_CFG_ARCH_BITS=64)
    set(ARCH_BITS "64")
    set(ARCH "riscv")
endif()

# ============================================================================
# IREE Configuration (Integrated)
# ============================================================================

# IREE Build Options
set(IREE_BUILD_COMPILER OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_MLIR OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_THREADING OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(IREE_BUILD_BINDINGS_TFLITE OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_LOADER_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_PLUGIN_DEFAULTS OFF CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_PLUGIN_EMBEDDED_ELF ON CACHE BOOL "" FORCE)

# Enable specific drivers/loaders
set(IREE_HAL_DRIVER_LOCAL_SYNC ON CACHE BOOL "" FORCE)
set(IREE_HAL_EXECUTABLE_LOADER_STATIC_LIBRARY ON CACHE BOOL "" FORCE)

set(IREE_BUILD_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(IREE_ENABLE_WERROR_FLAG OFF CACHE BOOL "" FORCE)

# Global compiler options
add_compile_options(-Wno-error)

# Skip submodule checks
set(IREE_ERROR_ON_MISSING_SUBMODULES OFF CACHE BOOL "" FORCE)

# Explicitly disable external drivers to avoid dependency checks
set(IREE_HAL_DRIVER_CUDA OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_HIP OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_VULKAN OFF CACHE BOOL "" FORCE)
set(IREE_HAL_DRIVER_METAL OFF CACHE BOOL "" FORCE)
set(IREE_EXTERNAL_HAL_DRIVERS "" CACHE STRING "" FORCE)

# Global Definitions for IREE
add_compile_definitions(IREE_PLATFORM_GENERIC)
add_compile_definitions(IREE_SYNCHRONIZATION_DISABLE_UNSAFE=1)
add_compile_definitions(IREE_STATUS_FEATURES=0)
add_compile_definitions(IREE_FILE_IO_ENABLE=0)
add_compile_definitions(IREE_HAVE_POSIX_MEMALIGN=0)
add_compile_definitions(IREE_HAVE_STDIO=0)
add_compile_definitions(_POSIX_C_SOURCE=0)
add_compile_definitions(_ISOC11_SOURCE=0)

# Custom Config Header
include_directories(${CMAKE_SOURCE_DIR}/ai/include)
add_definitions(-DIREE_USER_CONFIG_H=\"iree_config_custom.h\")

# Add IREE Subdirectory
add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)

# ============================================================================
# AI Library
# ============================================================================
set(AI_SOURCES
    ai/src/ai_runtime.c
    ai/src/ai_backend.c
    ai/mobilenet_test_module_c.c
)

add_library(rv_aios_ai STATIC ${AI_SOURCES})

# Link against IREE targets
target_link_libraries(rv_aios_ai PUBLIC
    iree::base
    iree::vm
    iree::hal
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    iree::modules::hal
)

target_compile_definitions(rv_aios_ai PUBLIC
    EMITC_IMPLEMENTATION
)

target_compile_options(rv_aios_ai PRIVATE
    -Wno-error
    -Wno-type-limits
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-cast-function-type
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

target_include_directories(rv_aios_ai PRIVATE
    ${CMAKE_SOURCE_DIR}/ai/include
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

# ============================================================================
# Memory Library
# ============================================================================
file(GLOB_RECURSE MEMORY_SOURCES memory/src/*.c)
add_library(rv_aios_memory STATIC ${MEMORY_SOURCES})
target_include_directories(rv_aios_memory PRIVATE
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/hal/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
)

# Kernel Library
file(GLOB_RECURSE KERNEL_SOURCES
    kernel/src/*.c
    arch/${ARCH}/src/*.c
    arch/${ARCH}/src/*.S
)
add_library(rv_aios_kernel STATIC ${KERNEL_SOURCES})
target_include_directories(rv_aios_kernel PRIVATE
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

# HAL (Drivers)
file(GLOB_RECURSE HAL_SOURCES hal/src/*.c)
add_library(rv_aios_drivers STATIC ${HAL_SOURCES})
target_include_directories(rv_aios_drivers PRIVATE
    ${CMAKE_SOURCE_DIR}/hal/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
)

if(NOT DEFINED LINKER_SCRIPT)
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/arch/${ARCH}/link.ld")
else()
    if(NOT IS_ABSOLUTE "${LINKER_SCRIPT}")
        set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}")
    endif()
endif()

# ============================================================================
# LibC Shim Library (Wrappers for malloc/stdio + RTOS impl)
# Compiled as OBJECT library to ensure symbols are linked
# ============================================================================
add_library(rv_aios_libc_shim OBJECT
    ai/src/malloc_wrapper.c
    ai/src/stdio_wrapper.c
    ai/src/iree_rtos_sync.c
)
target_include_directories(rv_aios_libc_shim PRIVATE
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/hal/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/ai/include # For iree_config_custom.h
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src # For iree/base/api.h
)
# Note: No -include iree_bare_metal_config.h here!

# ============================================================================
# AI Demo - STM32CubeAI Style Template
# ============================================================================
option(AI_MODE_BYTECODE "Use IREE VM Bytecode mode" OFF)

if(AI_MODE_BYTECODE)
    add_definitions(-DAI_MODE_BYTECODE)
    add_definitions(-DAI_MODEL_YOLO) # Hardcode for test

    add_executable(ai_demo
        apps/ai_demo/main.c
        apps/ai_demo/ai_model.c
    )
    target_include_directories(ai_demo PRIVATE ${CMAKE_SOURCE_DIR}/zoo/iree_static)
else()
    # EmitC mode (Legacy)
    add_executable(ai_demo
        apps/ai_demo/main.c
        apps/ai_demo/ai_model.c
        apps/ai_demo/model_impl.c
        zoo/iree_static/st_yolo_lc_v1_192.h
        # Shim sources handled by object lib
    )
endif()

target_include_directories(ai_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/apps/ai_demo
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/hal/include
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/ai/include
    ${CMAKE_SOURCE_DIR}/zoo/iree_static
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/iree/third_party/flatcc/include
)
target_compile_options(ai_demo PRIVATE
    -Wno-type-limits
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-cast-function-type
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

if(AI_MODE_BYTECODE)
    target_link_libraries(ai_demo
        rv_aios_ai rv_aios_kernel rv_aios_drivers rv_aios_memory
        rv_aios_drivers
        c m gcc gcc
    )
else()
    if(ARCH_BITS STREQUAL "64")
        set(MODEL_OBJ "st_yolo_lc_v1_192_rv64.o")
    else()
        set(MODEL_OBJ "st_yolo_lc_v1_192.o")
    endif()

    target_link_libraries(ai_demo
        ${CMAKE_SOURCE_DIR}/zoo/iree_static/${MODEL_OBJ}
        rv_aios_ai rv_aios_kernel rv_aios_drivers rv_aios_memory rv_aios_libc_shim
        rv_aios_drivers
        c m gcc gcc
    )
endif()

set_target_properties(ai_demo PROPERTIES
    LINK_FLAGS "-T${LINKER_SCRIPT} -nostartfiles -Wl,--gc-sections"
)
add_custom_command(TARGET ai_demo POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ai_demo ai_demo.bin
    COMMAND ${CMAKE_SIZE} ai_demo
)
