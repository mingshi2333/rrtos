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

# Board Selection
# Immutable board facts live under boards/<board>/board_config.h.
# This top-level file owns selected build/runtime policy for the current lane.
set(CONFIG_BOARD "qemu_virt" CACHE STRING "Target board (qemu_virt, be_u1000)")
set_property(CACHE CONFIG_BOARD PROPERTY STRINGS qemu_virt be_u1000)

set(OS_AI_EN_DEFAULT ON)
if(CONFIG_BOARD STREQUAL "be_u1000")
    set(OS_AI_EN_DEFAULT OFF)
endif()

option(OS_SMP_EN "Enable SMP support" OFF)
option(OS_IPC_EN "Enable kernel IPC primitives" ON)
option(OS_TIMER_EN "Enable kernel software timers" ON)
option(OS_LIBC_SHIM_EN "Enable picolibc syscall shim in the kernel archive" ON)
option(OS_HEAP_EN "Enable RTOS heap allocator" ON)
option(OS_AI_EN "Enable AI runtime" ${OS_AI_EN_DEFAULT})
set(AI_IREE_STATUS_FEATURES "0" CACHE STRING "IREE status detail feature mask for AI runtime diagnostics")
set(AI_IREE_HAL_MODE "static_library" CACHE STRING "IREE HAL runtime path for generated AI models (static_library, inline, or both)")
set_property(CACHE AI_IREE_HAL_MODE PROPERTY STRINGS static_library inline both)
option(OS_FL_EN "Enable Federated Learning" OFF)
option(OS_DEBUG "Enable debug build" ON)
option(RRTOS_CXX_EN "Enable optional freestanding C++ support" OFF)
option(RRTOS_BUILD_EXPERIMENTAL_APPS "Build experimental apps and legacy AI demos" OFF)
set(RRTOS_HAL_FEATURES "auto" CACHE STRING "Semicolon-separated HAL features, or 'auto' for the selected board/app")

if(RRTOS_CXX_EN)
    enable_language(CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(OS_TIMER_EN AND NOT OS_IPC_EN)
    message(FATAL_ERROR "OS_TIMER_EN requires OS_IPC_EN because the software timer task is semaphore-backed")
endif()

if(OS_LIBC_SHIM_EN AND NOT OS_HEAP_EN)
    message(FATAL_ERROR "OS_LIBC_SHIM_EN requires OS_HEAP_EN because picolibc malloc/free bridge to os_malloc/os_free")
endif()

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
set(OS_CFG_IRQ_EXIT_SCHED_EN_VALUE 1)
set(OS_CFG_LOAD_BALANCE_EN_VALUE 1)
set(OS_CFG_SMP_TICK_OWNER_CPU_VALUE 0xFFFFFFFFUL)
set(OS_CFG_SMP_IDLE_POLL_MASK_VALUE 0UL)
set(OS_CFG_SMP_RUNTIME_IPI_EN_VALUE 1)
set(OS_CFG_SMP_RELEASE_IPI_EN_VALUE 1)

if(CONFIG_BOARD_BE_U1000)
    # BE-U1000 board facts currently imply a 3-core physical maximum and a
    # 2-core main-cluster lane, but the build still selects the effective
    # RTOS-schedulable-core policy for the current configuration.
    set(OS_CFG_CPU_FREQ_HZ_VALUE 250000000UL)
    set(OS_CFG_TIMER_FREQ_HZ_VALUE 1000000UL)
    set(OS_CFG_CPU_MAX_VALUE 3)
    set(OS_CFG_IRQ_EXIT_SCHED_EN_VALUE 0)
    set(OS_CFG_LOAD_BALANCE_EN_VALUE 0)
    set(OS_CFG_SMP_TICK_OWNER_CPU_VALUE 0UL)
    set(OS_CFG_SMP_RUNTIME_IPI_EN_VALUE 0)
    set(OS_CFG_SMP_RELEASE_IPI_EN_VALUE 0)
    if(OS_SMP_EN)
        set(OS_CFG_CPU_COUNT_VALUE 2)
        set(OS_CFG_SMP_IDLE_POLL_MASK_VALUE 0x2UL)
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

if(OS_IPC_EN)
    set(OS_CFG_IPC_EN_VALUE 1)
else()
    set(OS_CFG_IPC_EN_VALUE 0)
endif()

if(OS_TIMER_EN)
    set(OS_CFG_TIMER_EN_VALUE 1)
else()
    set(OS_CFG_TIMER_EN_VALUE 0)
endif()

if(OS_LIBC_SHIM_EN)
    set(OS_CFG_LIBC_SHIM_EN_VALUE 1)
else()
    set(OS_CFG_LIBC_SHIM_EN_VALUE 0)
endif()

if(OS_HEAP_EN)
    set(OS_CFG_HEAP_EN_VALUE 1)
else()
    set(OS_CFG_HEAP_EN_VALUE 0)
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
    OS_CFG_IPC_EN=${OS_CFG_IPC_EN_VALUE}
    OS_CFG_TIMER_EN=${OS_CFG_TIMER_EN_VALUE}
    OS_CFG_LIBC_SHIM_EN=${OS_CFG_LIBC_SHIM_EN_VALUE}
    OS_CFG_HEAP_EN=${OS_CFG_HEAP_EN_VALUE}
    OS_CFG_AI_EN=${OS_CFG_AI_EN_VALUE}
    OS_CFG_FL_EN=${OS_CFG_FL_EN_VALUE}
    OS_CFG_CPU_FREQ_HZ=${OS_CFG_CPU_FREQ_HZ_VALUE}
    OS_CFG_TIMER_FREQ_HZ=${OS_CFG_TIMER_FREQ_HZ_VALUE}
    OS_CFG_CPU_MAX=${OS_CFG_CPU_MAX_VALUE}
    OS_CFG_CPU_COUNT=${OS_CFG_CPU_COUNT_VALUE}
    OS_CFG_IRQ_EXIT_SCHED_EN=${OS_CFG_IRQ_EXIT_SCHED_EN_VALUE}
    OS_CFG_LOAD_BALANCE_EN=${OS_CFG_LOAD_BALANCE_EN_VALUE}
    OS_CFG_SMP_TICK_OWNER_CPU=${OS_CFG_SMP_TICK_OWNER_CPU_VALUE}
    OS_CFG_SMP_IDLE_POLL_MASK=${OS_CFG_SMP_IDLE_POLL_MASK_VALUE}
    OS_CFG_SMP_RUNTIME_IPI_EN=${OS_CFG_SMP_RUNTIME_IPI_EN_VALUE}
    OS_CFG_SMP_RELEASE_IPI_EN=${OS_CFG_SMP_RELEASE_IPI_EN_VALUE}
)
add_compile_definitions(${OS_SELECTED_LANE_DEFINITIONS})

# Architecture Selection
set(ARCH_BITS "64" CACHE STRING "Target architecture bits (32 or 64)")

# Central RISC-V ISA configuration
include(${CMAKE_SOURCE_DIR}/cmake/riscv_arch.cmake)

if(ARCH_BITS STREQUAL "32")
    if(RISCV_ABI STREQUAL "ilp32d" OR RISCV_MABI STREQUAL "ilp32f")
        # RV32 hard-float ABI. RISCV_MABI selects single- or double-float.
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections -Oz")
        if(RRTOS_CXX_EN)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections -Oz")
        endif()
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI}")
    else()
        # Soft-Float (rv32imacv) - Default
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32 -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections")
        if(RRTOS_CXX_EN)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32 -mcmodel=medany -mno-relax -Wall -Wextra -ffunction-sections -fdata-sections")
        endif()
        set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -march=rv32imacv_zicsr -mabi=ilp32")
    endif()
    add_definitions(-DOS_CFG_ARCH_BITS=32)
    set(ARCH "riscv")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -Wall -Wextra -ffunction-sections -fdata-sections")
    if(RRTOS_CXX_EN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${RISCV_MARCH}_zicsr -mabi=${RISCV_MABI} -mcmodel=medany -Wall -Wextra -ffunction-sections -fdata-sections")
    endif()
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

if(CONFIG_BOARD_BE_U1000)
    set(BE_U1000_APP "demo" CACHE STRING "BE-U1000 application lane (demo, gpio_ledblink, gpio_inputpolling, uart_printf, canfd_polling, qspi_flash, i2c_polling, tim_timebase, adc_single, wdt_heartbeat, pwmg_outputcompare, dma_mem2mem, i2s_tx, pwma_timebase, usb_runtime, ai_micro_demo, ai_micro_demo_cpp, ai_static_direct_probe, ai_static_direct_mnist_probe, or etl_smoke)")
    set_property(CACHE BE_U1000_APP PROPERTY STRINGS demo gpio_ledblink gpio_inputpolling uart_printf canfd_polling qspi_flash i2c_polling tim_timebase adc_single wdt_heartbeat pwmg_outputcompare dma_mem2mem i2s_tx pwma_timebase usb_runtime ai_micro_demo ai_micro_demo_cpp ai_static_direct_probe ai_static_direct_mnist_probe etl_smoke)

    set(RRTOS_BE_U1000_BASE_HAL_FEATURES board uart clint irq_clic)
    if(RRTOS_HAL_FEATURES STREQUAL "auto")
        set(RRTOS_EFFECTIVE_HAL_FEATURES ${RRTOS_BE_U1000_BASE_HAL_FEATURES})
        if(BE_U1000_APP STREQUAL "demo")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES board_selftest gpio i2c spi flash canfd)
        elseif(BE_U1000_APP STREQUAL "gpio_ledblink")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES gpio)
        elseif(BE_U1000_APP STREQUAL "gpio_inputpolling")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES gpio)
        elseif(BE_U1000_APP STREQUAL "uart_printf")
            # Base HAL features are enough.
        elseif(BE_U1000_APP STREQUAL "canfd_polling")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES board_selftest gpio i2c spi flash canfd)
        elseif(BE_U1000_APP STREQUAL "qspi_flash")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES flash)
        elseif(BE_U1000_APP STREQUAL "i2c_polling")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES i2c)
        elseif(BE_U1000_APP STREQUAL "tim_timebase")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES gpio)
        elseif(BE_U1000_APP STREQUAL "adc_single")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES adc)
        elseif(BE_U1000_APP STREQUAL "wdt_heartbeat")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES wdt)
        elseif(BE_U1000_APP STREQUAL "pwmg_outputcompare")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES pwmg)
        elseif(BE_U1000_APP STREQUAL "dma_mem2mem")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES dma)
        elseif(BE_U1000_APP STREQUAL "i2s_tx")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES i2s)
        elseif(BE_U1000_APP STREQUAL "pwma_timebase")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES pwma)
        elseif(BE_U1000_APP STREQUAL "usb_runtime")
            list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES usb)
        elseif(BE_U1000_APP STREQUAL "ai_micro_demo")
            # Base HAL features are enough for the UART/CLINT AI smoke demo.
        elseif(BE_U1000_APP STREQUAL "ai_micro_demo_cpp")
            # Base HAL features are enough for the UART/CLINT C++ AI smoke demo.
        elseif(BE_U1000_APP STREQUAL "ai_static_direct_probe")
            # Base HAL features are enough for the UART/CLINT static executable probe.
        elseif(BE_U1000_APP STREQUAL "ai_static_direct_mnist_probe")
            # Base HAL features are enough for the UART/CLINT static MNIST executable probe.
        elseif(BE_U1000_APP STREQUAL "etl_smoke")
            # Base HAL features are enough for the UART/CLINT C++/ETL smoke demo.
        else()
            message(FATAL_ERROR "Unsupported BE_U1000_APP='${BE_U1000_APP}'. Expected one of: demo, gpio_ledblink, gpio_inputpolling, uart_printf, canfd_polling, qspi_flash, i2c_polling, tim_timebase, adc_single, wdt_heartbeat, pwmg_outputcompare, dma_mem2mem, i2s_tx, pwma_timebase, usb_runtime, ai_micro_demo, ai_micro_demo_cpp, ai_static_direct_probe, ai_static_direct_mnist_probe, etl_smoke")
        endif()
    else()
        set(RRTOS_EFFECTIVE_HAL_FEATURES ${RRTOS_HAL_FEATURES})
    endif()
    if(OS_SMP_EN)
        list(APPEND RRTOS_EFFECTIVE_HAL_FEATURES corectrl)
    endif()
    list(REMOVE_DUPLICATES RRTOS_EFFECTIVE_HAL_FEATURES)
    message(STATUS "BE-U1000 HAL features: ${RRTOS_EFFECTIVE_HAL_FEATURES}")
endif()

# ============================================================================
# IREE Configuration (Integrated)
# ============================================================================
if(OS_AI_EN)
    if(AI_IREE_HAL_MODE STREQUAL "inline")
        set(AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE 0)
        set(AI_IREE_ENABLE_INLINE_HAL_VALUE 1)
    elseif(AI_IREE_HAL_MODE STREQUAL "both")
        set(AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE 1)
        set(AI_IREE_ENABLE_INLINE_HAL_VALUE 1)
    elseif(AI_IREE_HAL_MODE STREQUAL "static_library")
        set(AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE 1)
        set(AI_IREE_ENABLE_INLINE_HAL_VALUE 0)
    else()
        message(FATAL_ERROR "Unsupported AI_IREE_HAL_MODE='${AI_IREE_HAL_MODE}'. Expected static_library, inline, or both")
    endif()

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
    set(IREE_HAL_DRIVER_LOCAL_SYNC ${AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE} CACHE BOOL "" FORCE)
    set(IREE_HAL_EXECUTABLE_LOADER_STATIC_LIBRARY ${AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE} CACHE BOOL "" FORCE)
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
    add_compile_definitions(IREE_STATUS_FEATURES=${AI_IREE_STATUS_FEATURES})
    add_compile_definitions(IREE_HAL_COMMAND_BUFFER_VALIDATION_ENABLE=0)
    add_compile_definitions(IREE_FILE_IO_ENABLE=0)
    add_compile_definitions(IREE_HAVE_POSIX_MEMALIGN=0)
    add_compile_definitions(IREE_HAVE_STDIO=0)
    add_compile_definitions(AI_IREE_ALLOC_TRACE=1)
    add_compile_definitions(AI_IREE_ENABLE_STATIC_LIBRARY_HAL=${AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE})
    add_compile_definitions(AI_IREE_ENABLE_INLINE_HAL=${AI_IREE_ENABLE_INLINE_HAL_VALUE})
    add_compile_definitions(_POSIX_C_SOURCE=0)
    add_compile_definitions(_ISOC11_SOURCE=0)

    include_directories(${CMAKE_SOURCE_DIR}/ai/include)
    add_definitions(-DIREE_USER_CONFIG_H=\"iree_config_custom.h\")

    add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)
endif()

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

if(RRTOS_CXX_EN)
    include(${CMAKE_SOURCE_DIR}/cmake/rrtos_cxx.cmake)
endif()

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
    elseif(BE_U1000_APP STREQUAL "ai_micro_demo")
        if(NOT OS_AI_EN)
            message(FATAL_ERROR "ai_micro_demo requires OS_AI_EN=ON")
        endif()
        add_subdirectory(apps/be_u1000_ai_micro_demo)
    elseif(BE_U1000_APP STREQUAL "ai_micro_demo_cpp")
        if(NOT OS_AI_EN)
            message(FATAL_ERROR "ai_micro_demo_cpp requires OS_AI_EN=ON")
        endif()
        if(NOT RRTOS_CXX_EN)
            message(FATAL_ERROR "ai_micro_demo_cpp requires RRTOS_CXX_EN=ON")
        endif()
        add_subdirectory(apps/be_u1000_ai_micro_demo_cpp)
    elseif(BE_U1000_APP STREQUAL "ai_static_direct_probe")
        add_subdirectory(apps/be_u1000_ai_static_direct_probe)
    elseif(BE_U1000_APP STREQUAL "ai_static_direct_mnist_probe")
        add_subdirectory(apps/be_u1000_ai_static_direct_mnist_probe)
    elseif(BE_U1000_APP STREQUAL "etl_smoke")
        if(NOT RRTOS_CXX_EN)
            message(FATAL_ERROR "etl_smoke requires RRTOS_CXX_EN=ON")
        endif()
        add_subdirectory(apps/be_u1000_etl_smoke)
    else()
        message(FATAL_ERROR "Unsupported BE_U1000_APP='${BE_U1000_APP}'. Expected one of: demo, gpio_ledblink, gpio_inputpolling, uart_printf, canfd_polling, qspi_flash, i2c_polling, tim_timebase, adc_single, wdt_heartbeat, pwmg_outputcompare, dma_mem2mem, i2s_tx, pwma_timebase, usb_runtime, ai_micro_demo, ai_micro_demo_cpp, ai_static_direct_probe, ai_static_direct_mnist_probe, etl_smoke")
    endif()
else()
    if(OS_AI_EN)
        add_subdirectory(apps/mnist_app)
    endif()

    if(RRTOS_BUILD_EXPERIMENTAL_APPS)
        add_subdirectory(apps/ai_demo)
    endif()
endif()
