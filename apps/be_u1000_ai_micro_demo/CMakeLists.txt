add_subdirectory(generated)

add_executable(rrtos_be_u1000 main.c)

target_link_libraries(rrtos_be_u1000 PRIVATE
    rv_aios_models
    rv_aios_libc_shim
    rv_aios_rtos
    c m gcc gcc
)

target_include_directories(rrtos_be_u1000 PRIVATE
    ${CMAKE_SOURCE_DIR}/ai/include
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

target_compile_options(rrtos_be_u1000 PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wno-type-limits>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-variable>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-function>
    $<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:C>:-Wno-cast-function-type>
    $<$<COMPILE_LANGUAGE:C>:-Wno-implicit-function-declaration>
    $<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>
    $<$<COMPILE_LANGUAGE:C>:-Wno-pointer-sign>
    $<$<COMPILE_LANGUAGE:C>:-include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h>
)

if(DEFINED BOARD_SOURCES)
    target_sources(rrtos_be_u1000 PRIVATE ${BOARD_SOURCES})
endif()

target_link_options(rrtos_be_u1000 PRIVATE
    -T${LINKER_SCRIPT}
    -nostdlib
    --ld-path=ld.lld
    -Wl,--gc-sections
    -Wl,--icf=all
    -Wl,--allow-multiple-definition
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
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/check_be_u1000_ai_demo_size.py
            --binary ${CMAKE_BINARY_DIR}/rrtos_be_u1000.bin
            --max-bytes 262144
    COMMENT "Generating BE-U1000 AI micro demo binary and size report"
)
