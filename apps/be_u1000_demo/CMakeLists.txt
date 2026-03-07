add_executable(rrtos_be_u1000 main.c)

target_link_libraries(rrtos_be_u1000 PRIVATE
    rv_aios_rtos
)

if(DEFINED BOARD_SOURCES)
    target_sources(rrtos_be_u1000 PRIVATE ${BOARD_SOURCES})
endif()

target_link_options(rrtos_be_u1000 PRIVATE
    -T${LINKER_SCRIPT}
    -nostdlib
    -Wl,--gc-sections
)

set_target_properties(rrtos_be_u1000 PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_command(TARGET rrtos_be_u1000 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:rrtos_be_u1000>
            ${CMAKE_BINARY_DIR}/rrtos_be_u1000.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:rrtos_be_u1000>
    COMMENT "Generating binary and size report"
)
