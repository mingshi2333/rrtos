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
    -Wl,-Map,${CMAKE_BINARY_DIR}/rrtos_be_u1000.map
)

set_target_properties(rrtos_be_u1000 PROPERTIES
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_command(TARGET rrtos_be_u1000 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:rrtos_be_u1000>
            ${CMAKE_BINARY_DIR}/rrtos_be_u1000.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:rrtos_be_u1000>
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/report_build_footprint.py
            --label rrtos_be_u1000
            --image $<TARGET_FILE:rrtos_be_u1000>
            --kernel-archive ${CMAKE_BINARY_DIR}/kernel/librv_aios_kernel.a
            --cache ${CMAKE_BINARY_DIR}/CMakeCache.txt
            --linker-script ${LINKER_SCRIPT}
            --map ${CMAKE_BINARY_DIR}/rrtos_be_u1000.map
            --output ${CMAKE_BINARY_DIR}/rrtos_be_u1000_footprint.md
    COMMENT "Generating binary and size report"
)
