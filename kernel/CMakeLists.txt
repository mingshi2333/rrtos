set(RRTOS_KERNEL_PUBLIC_INCLUDES
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/config>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/arch/${ARCH}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/hal/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/multicore/include>
)

set(RRTOS_KERNEL_PRIVATE_INCLUDES
    ${CMAKE_SOURCE_DIR}/memory/include
)

function(rrtos_kernel_object_module module_name)
    add_library(${module_name} OBJECT ${ARGN})
    target_include_directories(${module_name} PUBLIC ${RRTOS_KERNEL_PUBLIC_INCLUDES})
    target_include_directories(${module_name} PRIVATE ${RRTOS_KERNEL_PRIVATE_INCLUDES})
    target_link_libraries(${module_name} PUBLIC rv_aios_memory)
endfunction()

set(RRTOS_KERNEL_ARCH_SOURCES
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/trap.c
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/context.S
)

if(NOT CONFIG_BOARD_BE_U1000)
    list(APPEND RRTOS_KERNEL_ARCH_SOURCES
        ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/startup.S
    )
endif()

rrtos_kernel_object_module(rv_aios_kernel_arch
    ${RRTOS_KERNEL_ARCH_SOURCES}
)

rrtos_kernel_object_module(rv_aios_kernel_core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_sched.c
)

set(RRTOS_KERNEL_OBJECTS
    $<TARGET_OBJECTS:rv_aios_kernel_arch>
    $<TARGET_OBJECTS:rv_aios_kernel_core>
)

if(OS_IPC_EN)
    rrtos_kernel_object_module(rv_aios_kernel_ipc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/os_ipc.c
    )
    list(APPEND RRTOS_KERNEL_OBJECTS $<TARGET_OBJECTS:rv_aios_kernel_ipc>)
endif()

if(OS_TIMER_EN)
    rrtos_kernel_object_module(rv_aios_kernel_timer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/os_timer.c
    )
    list(APPEND RRTOS_KERNEL_OBJECTS $<TARGET_OBJECTS:rv_aios_kernel_timer>)
endif()

set(RRTOS_KERNEL_LIBC_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/os_string.c
)

if(OS_LIBC_SHIM_EN)
    list(APPEND RRTOS_KERNEL_LIBC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/picolibc_syscalls.c
    )
endif()

rrtos_kernel_object_module(rv_aios_kernel_libc
    ${RRTOS_KERNEL_LIBC_SOURCES}
)
list(APPEND RRTOS_KERNEL_OBJECTS $<TARGET_OBJECTS:rv_aios_kernel_libc>)

if(OS_SMP_EN)
    rrtos_kernel_object_module(rv_aios_kernel_smp
        ${CMAKE_SOURCE_DIR}/multicore/src/os_smp.c
    )
    list(APPEND RRTOS_KERNEL_OBJECTS $<TARGET_OBJECTS:rv_aios_kernel_smp>)
endif()

add_library(rv_aios_kernel STATIC
    ${RRTOS_KERNEL_OBJECTS}
)

target_include_directories(rv_aios_kernel PUBLIC
    ${RRTOS_KERNEL_PUBLIC_INCLUDES}
)

target_include_directories(rv_aios_kernel PRIVATE
    ${RRTOS_KERNEL_PRIVATE_INCLUDES}
)

target_link_libraries(rv_aios_kernel PUBLIC rv_aios_memory)
