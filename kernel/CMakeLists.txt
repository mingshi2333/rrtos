file(GLOB_RECURSE KERNEL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/*.c
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/*.S
)

if(CONFIG_BOARD_BE_U1000)
    list(REMOVE_ITEM KERNEL_SOURCES
        ${CMAKE_SOURCE_DIR}/arch/${ARCH}/src/startup.S
    )
endif()

if(OS_SMP_EN)
    list(APPEND KERNEL_SOURCES
        ${CMAKE_SOURCE_DIR}/multicore/src/os_smp.c
    )
endif()

add_library(rv_aios_kernel STATIC ${KERNEL_SOURCES})

target_include_directories(rv_aios_kernel PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/config>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/arch/${ARCH}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/hal/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/multicore/include>
)

target_include_directories(rv_aios_kernel PRIVATE
    ${CMAKE_SOURCE_DIR}/memory/include
)

target_link_libraries(rv_aios_kernel PUBLIC rv_aios_memory)
