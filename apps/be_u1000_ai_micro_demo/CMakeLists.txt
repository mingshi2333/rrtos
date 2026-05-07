set(BE_U1000_AI_MODELS_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/generated"
    CACHE PATH "Generated BE-U1000 AI model directory")
set(BE_U1000_AI_MODELS_TARGET
    rv_aios_models
    CACHE STRING "Generated BE-U1000 AI model CMake target")
set(BE_U1000_AI_MODEL_NAME
    "be_u1000_hello_world_float"
    CACHE STRING "AI model name to run from the generated registry")
set(BE_U1000_AI_INPUT_BUFFER_BYTES
    4096
    CACHE STRING "Static input buffer capacity for the AI micro probe")
set(BE_U1000_AI_OUTPUT_BUFFER_BYTES
    4096
    CACHE STRING "Static output buffer capacity for the AI micro probe")
set(BE_U1000_AI_SIZE_CHECK_MAX_BYTES
    262144
    CACHE STRING "Maximum accepted BE-U1000 AI micro binary size")
option(BE_U1000_AI_ALLOW_OVERSIZE_LINK
    "Allow oversize BE-U1000 AI images to link for measurement only"
    OFF)

add_subdirectory("${BE_U1000_AI_MODELS_DIR}" generated)

add_executable(rrtos_be_u1000 main.c)

target_link_libraries(rrtos_be_u1000 PRIVATE
    ${BE_U1000_AI_MODELS_TARGET}
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

target_compile_definitions(rrtos_be_u1000 PRIVATE
    AI_MICRO_MODEL_NAME="${BE_U1000_AI_MODEL_NAME}"
    AI_MICRO_INPUT_BUFFER_BYTES=${BE_U1000_AI_INPUT_BUFFER_BYTES}
    AI_MICRO_OUTPUT_BUFFER_BYTES=${BE_U1000_AI_OUTPUT_BUFFER_BYTES}
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

if(BE_U1000_AI_ALLOW_OVERSIZE_LINK)
    target_link_options(rrtos_be_u1000 PRIVATE
        -Wl,--no-check-sections
    )
endif()

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
            --max-bytes ${BE_U1000_AI_SIZE_CHECK_MAX_BYTES}
    COMMENT "Generating BE-U1000 AI micro demo binary and size report"
)
