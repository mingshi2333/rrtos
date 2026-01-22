set(HAL_SOURCES)

if(CONFIG_PLATFORM_ESP32C3)
    file(GLOB HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/esp32/*.c)
else()
    # Default/Generic RISC-V HAL
    file(GLOB HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
endif()

add_library(rv_aios_drivers STATIC ${HAL_SOURCES})

target_include_directories(rv_aios_drivers PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(rv_aios_drivers PUBLIC rv_aios_kernel)
