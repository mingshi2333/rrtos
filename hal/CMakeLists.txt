set(HAL_SOURCES)

if(CONFIG_PLATFORM_ESP32C3)
    file(GLOB HAL_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/esp32/*.c)
elseif(CONFIG_BOARD_BE_U1000)
    set(RRTOS_BE_U1000_HAL_FEATURES ${RRTOS_EFFECTIVE_HAL_FEATURES})
    set(RRTOS_BE_U1000_BOARD_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_board.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_board_be_u1000_pinmux.c
    )
    if(NOT RRTOS_BE_U1000_HAL_FEATURES)
        set(RRTOS_BE_U1000_HAL_FEATURES
            board
            board_selftest
            uart
            clint
            irq_clic
            gpio
            i2c
            spi
            flash
            canfd
            adc
            dma
            i2s
            pwma
            pwmg
            usb
            wdt
            corectrl
            mailbox
        )
    endif()

    foreach(feature IN LISTS RRTOS_BE_U1000_HAL_FEATURES)
        if(feature STREQUAL "board")
            list(APPEND HAL_SOURCES ${RRTOS_BE_U1000_BOARD_SOURCES})
        elseif(feature STREQUAL "board_selftest")
            list(APPEND HAL_SOURCES
                ${RRTOS_BE_U1000_BOARD_SOURCES}
                ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_board_selftest.c
                ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_gpio_be_u1000.c
                ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_i2c_be_u1000.c
                ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_spi_be_u1000.c
                ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_flash_be_u1000.c
                ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_canfd_be_u1000.c
            )
        elseif(feature STREQUAL "uart")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_uart_be_u1000.c)
        elseif(feature STREQUAL "clint")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_clint.c)
        elseif(feature STREQUAL "irq_clic")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_clic.c)
        elseif(feature STREQUAL "gpio")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_gpio_be_u1000.c)
        elseif(feature STREQUAL "i2c")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_i2c_be_u1000.c)
        elseif(feature STREQUAL "spi")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_spi_be_u1000.c)
        elseif(feature STREQUAL "flash")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_flash_be_u1000.c)
        elseif(feature STREQUAL "canfd")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_canfd_be_u1000.c)
        elseif(feature STREQUAL "adc")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_adc_be_u1000.c)
        elseif(feature STREQUAL "dma")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_dma_be_u1000.c)
        elseif(feature STREQUAL "i2s")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_i2s_be_u1000.c)
        elseif(feature STREQUAL "pwma")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_pwma_be_u1000.c)
        elseif(feature STREQUAL "pwmg")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_pwmg_be_u1000.c)
        elseif(feature STREQUAL "usb")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_usb_be_u1000.c)
        elseif(feature STREQUAL "wdt")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_wdt_be_u1000.c)
        elseif(feature STREQUAL "corectrl")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_corectrl_be_u1000.c)
        elseif(feature STREQUAL "mailbox")
            list(APPEND HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/be_u1000/hal_mailbox_be_u1000.c)
        else()
            message(FATAL_ERROR "Unknown BE-U1000 HAL feature '${feature}'")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES HAL_SOURCES)
else()
    set(HAL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_board.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_clint.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_plic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_plic_irq.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/hal_uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/math_compat.c
    )
endif()

add_library(rv_aios_drivers STATIC ${HAL_SOURCES})

if(CONFIG_BOARD_BE_U1000)
    list(FIND RRTOS_BE_U1000_HAL_FEATURES "board_selftest" RRTOS_HAL_BOARD_SELFTEST_INDEX)
    if(RRTOS_HAL_BOARD_SELFTEST_INDEX EQUAL -1)
        target_compile_definitions(rv_aios_drivers PRIVATE RRTOS_HAL_HAS_BOARD_SELFTEST=0)
    else()
        target_compile_definitions(rv_aios_drivers PRIVATE RRTOS_HAL_HAS_BOARD_SELFTEST=1)
    endif()
endif()

target_include_directories(rv_aios_drivers PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/kernel/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/config>
)

if(OS_SMP_EN)
    target_include_directories(rv_aios_drivers PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/multicore/include>
    )
endif()

if(CONFIG_BOARD_BE_U1000)
    target_include_directories(rv_aios_drivers PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/boards/be_u1000>
    )
endif()
