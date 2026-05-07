set(BE_U1000_STATIC_DIRECT_MODEL_OBJECT
    "${CMAKE_SOURCE_DIR}/apps/be_u1000_ai_micro_demo/generated/be_u1000_hello_world_float.o"
    CACHE FILEPATH "Generated IREE static executable object for the direct probe")

if(NOT EXISTS "${BE_U1000_STATIC_DIRECT_MODEL_OBJECT}")
    message(FATAL_ERROR "Missing BE-U1000 static direct model object: ${BE_U1000_STATIC_DIRECT_MODEL_OBJECT}")
endif()

set_source_files_properties(
    "${BE_U1000_STATIC_DIRECT_MODEL_OBJECT}"
    PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE)

add_executable(rrtos_be_u1000
    main.c
    model_const.c
    "${BE_U1000_STATIC_DIRECT_MODEL_OBJECT}"
)

target_link_libraries(rrtos_be_u1000 PRIVATE
    rv_aios_rtos
    c m gcc gcc
)

target_include_directories(rrtos_be_u1000 PRIVATE
    ${CMAKE_SOURCE_DIR}/ai/include
    ${CMAKE_SOURCE_DIR}/apps/be_u1000_ai_micro_demo
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

target_compile_definitions(rrtos_be_u1000 PRIVATE
    IREE_PLATFORM_GENERIC
    IREE_SYNCHRONIZATION_DISABLE_UNSAFE=1
    IREE_STATUS_FEATURES=0
    IREE_FILE_IO_ENABLE=0
    IREE_HAVE_POSIX_MEMALIGN=0
    IREE_HAVE_STDIO=0
    _POSIX_C_SOURCE=0
    _ISOC11_SOURCE=0
    IREE_USER_CONFIG_H=\"iree_config_custom.h\"
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
            --label rrtos_be_u1000_ai_static_direct_probe
            --image $<TARGET_FILE:rrtos_be_u1000>
            --kernel-archive ${CMAKE_BINARY_DIR}/kernel/librv_aios_kernel.a
            --cache ${CMAKE_BINARY_DIR}/CMakeCache.txt
            --linker-script ${LINKER_SCRIPT}
            --map ${CMAKE_BINARY_DIR}/rrtos_be_u1000.map
            --output ${CMAKE_BINARY_DIR}/rrtos_be_u1000_footprint.md
    COMMENT "Generating BE-U1000 IREE static direct probe binary and size report"
)
