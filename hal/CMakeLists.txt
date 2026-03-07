set(HAL_SOURCES)

if(CONFIG_PLATFORM_ESP32C3)
    file(GLOB HAL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/esp32/*.c)
elseif(CONFIG_BOARD_BE_U1000)
    file(GLOB HAL_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_clint.c
    )
else()
    file(GLOB HAL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
endif()

list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_board.c)

add_library(rv_aios_drivers STATIC ${HAL_SOURCES})

target_include_directories(rv_aios_drivers PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/kernel/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/config>
)

if(CONFIG_BOARD_BE_U1000)
    target_include_directories(rv_aios_drivers PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/boards/be_u1000>
    )
endif()
