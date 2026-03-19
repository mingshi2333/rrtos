cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(rv_aios
    VERSION 1.0.0
    DESCRIPTION "Bare-metal RISC-V RTOS with supported RV32 and BE-U1000 firmware lanes"
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
add_compile_options(-nostdlib -ffunction-sections -fdata-sections -Qunused-arguments -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers -Wno-pointer-sign -Wno-sign-compare -Wno-cast-function-type -Wno-cast-function-type-mismatch)
add_link_options(-nostdlib -Wl,--gc-sections -Wl,--strip-all -L${PICOLIBC_ROOT}/lib)

option(OS_SMP_EN "Enable SMP support" OFF)
option(OS_AI_EN "Enable AI runtime" ON)
option(OS_FL_EN "Enable Federated Learning" OFF)
option(OS_DEBUG "Enable debug build" ON)
option(RRTOS_BUILD_EXPERIMENTAL_APPS "Build experimental apps and legacy AI demos" OFF)

# Board Selection
# Immutable board facts live under boards/<board>/board_config.h.
# This top-level file owns selected build/runtime policy for the current lane.
set(CONFIG_BOARD "qemu_virt" CACHE STRING "Target board (qemu_virt, be_u1000)")
set_property(CACHE CONFIG_BOARD PROPERTY STRINGS qemu_virt be_u1000)
if(CONFIG_BOARD STREQUAL "be_u1000")
    set(CONFIG_BOARD_BE_U1000 ON)
    add_definitions(-DCONFIG_BOARD_BE_U1000=1)
    include_directories(${CMAKE_SOURCE_DIR}/boards/be_u1000)
    add_subdirectory(boards/be_u1000)
    if(DEFINED BOARD_INCLUDES)
        include_directories(${BOARD_INCLUDES})
    endif()
endif()

# Effective runtime lane policy for the selected build.
# - Board headers publish immutable hardware facts.
# - This top-level file selects the lane policy exposed through OS_CFG_* defines.
# - OS_CFG_CPU_MAX is the compile-time execution ceiling for the chosen lane.
# - OS_CFG_CPU_COUNT is the active RTOS-schedulable core count for the chosen lane.
# - OS_CFG_CPU_FREQ_HZ / OS_CFG_TIMER_FREQ_HZ are the effective runtime clocks.
set(OS_CFG_CPU_FREQ_HZ_VALUE 100000000UL)
set(OS_CFG_TIMER_FREQ_HZ_VALUE 10000000UL)
set(OS_CFG_CPU_MAX_VALUE 1)
set(OS_CFG_CPU_COUNT_VALUE 1)

if(CONFIG_BOARD_BE_U1000)
    # BE-U1000 board facts currently imply a 3-core physical maximum and a
    # 2-core main-cluster lane, but the build still selects the effective
    # RTOS-schedulable-core policy for the current configuration.
    set(OS_CFG_CPU_FREQ_HZ_VALUE 200000000UL)
    set(OS_CFG_TIMER_FREQ_HZ_VALUE 1000000UL)
    set(OS_CFG_CPU_MAX_VALUE 3)
    if(OS_SMP_EN)
        set(OS_CFG_CPU_COUNT_VALUE 2)
    else()
        set(OS_CFG_CPU_COUNT_VALUE 1)
    endif()
endif()

if(OS_SMP_EN)
    set(OS_CFG_SMP_EN_VALUE 1)
else()
    set(OS_CFG_SMP_EN_VALUE 0)
endif()

if(OS_AI_EN)
    set(OS_CFG_AI_EN_VALUE 1)
else()
    set(OS_CFG_AI_EN_VALUE 0)
endif()

if(OS_FL_EN)
    set(OS_CFG_FL_EN_VALUE 1)
else()
    set(OS_CFG_FL_EN_VALUE 0)
endif()

# Export the selected build/runtime lane policy as compile definitions.
# config/os_config.h provides fallback defaults only when these are absent.
set(OS_SELECTED_LANE_DEFINITIONS
    OS_CFG_SMP_EN=${OS_CFG_SMP_EN_VALUE}
    OS_CFG_AI_EN=${OS_CFG_AI_EN_VALUE}
    OS_CFG_FL_EN=${OS_CFG_FL_EN_VALUE}
    OS_CFG_CPU_FREQ_HZ=${OS_CFG_CPU_FREQ_HZ_VALUE}
    OS_CFG_TIMER_FREQ_HZ=${OS_CFG_TIMER_FREQ_HZ_VALUE}
    OS_CFG_CPU_MAX=${OS_CFG_CPU_MAX_VALUE}
    OS_CFG_CPU_COUNT=${OS_CFG_CPU_COUNT_VALUE}
)
add_compile_definitions(${OS_SELECTED_LANE_DEFINITIONS})

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
if(CONFIG_BOARD_BE_U1000)
    if(DEFINED BOARD_LINKER_SCRIPT)
        set(LINKER_SCRIPT "${BOARD_LINKER_SCRIPT}" CACHE FILEPATH "Linker script" FORCE)
    else()
        message(FATAL_ERROR "BOARD_LINKER_SCRIPT must be exported by boards/be_u1000/CMakeLists.txt")
    endif()
elseif(NOT DEFINED LINKER_SCRIPT)
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
if(OS_AI_EN)
    add_subdirectory(ai)
endif()

# ============================================================================
# Applications
# ============================================================================
if(CONFIG_BOARD_BE_U1000)
    set(BE_U1000_APP "demo" CACHE STRING "BE-U1000 application lane (demo, gpio_ledblink, gpio_inputpolling, uart_printf, canfd_polling, qspi_flash, i2c_polling, tim_timebase, adc_single, wdt_heartbeat, pwmg_outputcompare, dma_mem2mem, i2s_tx, pwma_timebase, or usb_runtime)")
    set_property(CACHE BE_U1000_APP PROPERTY STRINGS demo gpio_ledblink gpio_inputpolling uart_printf canfd_polling qspi_flash i2c_polling tim_timebase adc_single wdt_heartbeat pwmg_outputcompare dma_mem2mem i2s_tx pwma_timebase usb_runtime)

    if(BE_U1000_APP STREQUAL "demo")
        add_subdirectory(apps/be_u1000_demo)
    elseif(BE_U1000_APP STREQUAL "gpio_ledblink")
        add_subdirectory(apps/be_u1000_gpio_ledblink)
    elseif(BE_U1000_APP STREQUAL "gpio_inputpolling")
        add_subdirectory(apps/be_u1000_gpio_inputpolling)
    elseif(BE_U1000_APP STREQUAL "uart_printf")
        add_subdirectory(apps/be_u1000_uart_printf)
    elseif(BE_U1000_APP STREQUAL "canfd_polling")
        add_subdirectory(apps/be_u1000_canfd_polling)
    elseif(BE_U1000_APP STREQUAL "qspi_flash")
        add_subdirectory(apps/be_u1000_qspi_flash)
    elseif(BE_U1000_APP STREQUAL "i2c_polling")
        add_subdirectory(apps/be_u1000_i2c_polling)
    elseif(BE_U1000_APP STREQUAL "tim_timebase")
        add_subdirectory(apps/be_u1000_tim_timebase)
    elseif(BE_U1000_APP STREQUAL "adc_single")
        add_subdirectory(apps/be_u1000_adc_single)
    elseif(BE_U1000_APP STREQUAL "wdt_heartbeat")
        add_subdirectory(apps/be_u1000_wdt_heartbeat)
    elseif(BE_U1000_APP STREQUAL "pwmg_outputcompare")
        add_subdirectory(apps/be_u1000_pwmg_outputcompare)
    elseif(BE_U1000_APP STREQUAL "dma_mem2mem")
        add_subdirectory(apps/be_u1000_dma_mem2mem)
    elseif(BE_U1000_APP STREQUAL "i2s_tx")
        add_subdirectory(apps/be_u1000_i2s_tx)
    elseif(BE_U1000_APP STREQUAL "pwma_timebase")
        add_subdirectory(apps/be_u1000_pwma_timebase)
    elseif(BE_U1000_APP STREQUAL "usb_runtime")
        add_subdirectory(apps/be_u1000_usb_runtime)
    else()
        message(FATAL_ERROR "Unsupported BE_U1000_APP='${BE_U1000_APP}'. Expected one of: demo, gpio_ledblink, gpio_inputpolling, uart_printf, canfd_polling, qspi_flash, i2c_polling, tim_timebase, adc_single, wdt_heartbeat, pwmg_outputcompare, dma_mem2mem, i2s_tx, pwma_timebase, usb_runtime")
    endif()
else()
    if(OS_AI_EN)
        add_subdirectory(apps/mnist_app)
    endif()

    if(RRTOS_BUILD_EXPERIMENTAL_APPS)
        add_subdirectory(apps/ai_demo)
    endif()
endif()
