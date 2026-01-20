file(GLOB_RECURSE HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(rv_aios_drivers STATIC ${HAL_SOURCES})

target_include_directories(rv_aios_drivers PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(rv_aios_drivers PUBLIC rv_aios_kernel)
