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

# IREE Configuration
set(IREE_RUNTIME_ROOT ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src)
set(IREE_INCLUDE_DIRS
    ${IREE_RUNTIME_ROOT}
    ${CMAKE_SOURCE_DIR}/third_party/iree/third_party/flatcc/include
)

# Base
file(GLOB_RECURSE IREE_BASE_SOURCES ${IREE_RUNTIME_ROOT}/iree/base/*.c)
list(FILTER IREE_BASE_SOURCES EXCLUDE REGEX ".*synchronization.*")
list(FILTER IREE_BASE_SOURCES EXCLUDE REGEX ".*threading.*")

# VM (Core only, exclude bytecode/dynamic if using EmitC)
file(GLOB IREE_VM_SOURCES ${IREE_RUNTIME_ROOT}/iree/vm/*.c)

if(AI_MODE_BYTECODE)
    file(GLOB IREE_VM_BYTECODE_SOURCES ${IREE_RUNTIME_ROOT}/iree/vm/bytecode/*.c)
else()
    set(IREE_VM_BYTECODE_SOURCES "")
endif()

# HAL (Core only)
file(GLOB IREE_HAL_CORE_SOURCES ${IREE_RUNTIME_ROOT}/iree/hal/*.c)
file(GLOB IREE_HAL_UTILS_SOURCES ${IREE_RUNTIME_ROOT}/iree/hal/utils/*.c)

# HAL Local (CPU execution)
file(GLOB IREE_HAL_LOCAL_SOURCES ${IREE_RUNTIME_ROOT}/iree/hal/local/*.c)
list(FILTER IREE_HAL_LOCAL_SOURCES EXCLUDE REGEX ".*benchmark.*")
list(FILTER IREE_HAL_LOCAL_SOURCES EXCLUDE REGEX ".*test.*")
list(FILTER IREE_HAL_LOCAL_SOURCES EXCLUDE REGEX ".*demo.*")

# HAL Local Loaders
set(IREE_HAL_STATIC_LIBRARY_LOADER_SOURCES
    ${IREE_RUNTIME_ROOT}/iree/hal/local/loaders/static_library_loader.c
)

# HAL Drivers (Sync)
file(GLOB IREE_HAL_DRIVER_SYNC_SOURCES
    ${IREE_RUNTIME_ROOT}/iree/hal/drivers/local_sync/*.c
    ${IREE_RUNTIME_ROOT}/iree/hal/drivers/local_sync/registration/*.c
)

# HAL Modules
file(GLOB IREE_MODULES_HAL_SOURCES ${IREE_RUNTIME_ROOT}/iree/modules/hal/*.c)
file(GLOB IREE_MODULES_HAL_UTILS_SOURCES ${IREE_RUNTIME_ROOT}/iree/modules/hal/utils/*.c)

# IO sources (exclude stdio_stream which requires file system)
set(IREE_IO_SOURCES
    ${IREE_RUNTIME_ROOT}/iree/io/file_handle.c
    ${IREE_RUNTIME_ROOT}/iree/io/memory_stream.c
    ${IREE_RUNTIME_ROOT}/iree/io/stream.c
    ${IREE_RUNTIME_ROOT}/iree/io/vec_stream.c
)

set(IREE_RUNTIME_SOURCES
    ${IREE_BASE_SOURCES}
    ${IREE_VM_SOURCES}
    ${IREE_VM_BYTECODE_SOURCES}
    ${IREE_HAL_CORE_SOURCES}
    ${IREE_HAL_UTILS_SOURCES}
    ${IREE_HAL_LOCAL_SOURCES}
    ${IREE_HAL_STATIC_LIBRARY_LOADER_SOURCES}
    ${IREE_HAL_DRIVER_SYNC_SOURCES}
    ${IREE_MODULES_HAL_SOURCES}
    ${IREE_MODULES_HAL_UTILS_SOURCES}
    ${IREE_IO_SOURCES}
)

list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*module_impl_emitc.c")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*executable_debug_info.c")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*executable_header.c")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*libmpi.c")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*mpi_channel_provider.c")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*mimalloc.*")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*flags.c.*")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*flags_demo.c.*")

# list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*file_io.*")
# list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*file_path.*")
list(FILTER IREE_RUNTIME_SOURCES EXCLUDE REGEX ".*wait_scope.c.*")

# AI Library
set(AI_SOURCES
    ai/src/ai_runtime.c
    ai/src/ai_backend.c
    ai/src/iree_rtos_sync.c

    # Small models (Pure EmitC - no .o needed):
    ai/mobilenet_test_module_c.c

    # Large models (EmitC + .o) - disabled due to llvm.round.f32 issue:
    # zoo/iree_static/efficientnet_lc_128_c_module.c
    ${IREE_RUNTIME_SOURCES}
)

# Large models require linked .o files (disabled for st_mnist test)
# if(ARCH_BITS EQUAL 64)
# message(STATUS "Linking RV64 model object: efficientnet_lc_128.o")
# list(APPEND AI_SOURCES ${CMAKE_SOURCE_DIR}/zoo/iree_static/efficientnet_lc_128.o)
# else()
# message(STATUS "Linking RV32 model object: efficientnet_lc_128.o")
# list(APPEND AI_SOURCES ${CMAKE_SOURCE_DIR}/zoo/iree_static/efficientnet_lc_128.o)
# endif()
add_library(rv_aios_ai STATIC ${AI_SOURCES})
target_compile_definitions(rv_aios_ai PUBLIC
    IREE_STATUS_FEATURES=0
    IREE_FILE_IO_ENABLE=0
    IREE_HAVE_POSIX_MEMALIGN=0
    IREE_HAVE_STDIO=0
    IREE_PLATFORM_GENERIC=1
    IREE_SYNCHRONIZATION_DISABLE_UNSAFE=1
    IREE_HAL_HAVE_STATIC_LIBRARY_LOADER=1
    _POSIX_C_SOURCE=0
    _ISOC11_SOURCE=0
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
    ${IREE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

# Memory Library
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
    ${IREE_INCLUDE_DIRS}
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
        rv_aios_ai rv_aios_kernel rv_aios_drivers rv_aios_memory
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
