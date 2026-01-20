file(GLOB_RECURSE MEMORY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(rv_aios_memory STATIC ${MEMORY_SOURCES})

target_include_directories(rv_aios_memory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(rv_aios_memory PRIVATE
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/hal/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
)
