option(AI_MODE_BYTECODE "Use IREE VM Bytecode mode" OFF)

if(AI_MODE_BYTECODE)
    add_definitions(-DAI_MODE_BYTECODE -DAI_MODEL_YOLO)
    add_executable(ai_demo main.c ai_model.c)
else()
    add_executable(ai_demo
        main.c
        ai_model.c
        model_impl.c
    )
endif()

target_include_directories(ai_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/zoo/iree_static
    ${CMAKE_SOURCE_DIR}/third_party/iree/third_party/flatcc/include
)

target_compile_options(ai_demo PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

if(ARCH_BITS STREQUAL "64")
    set(MODEL_OBJ "yoloface_int8_rv64.o")
else()
    set(MODEL_OBJ "yoloface_int8.o")
endif()

target_link_libraries(ai_demo
    rv_aios_ai
    rv_aios_libc_shim
    rv_aios_drivers
    ${CMAKE_SOURCE_DIR}/zoo/iree_static/${MODEL_OBJ}
    c m gcc gcc
)

set_target_properties(ai_demo PROPERTIES
    LINK_FLAGS "-T${LINKER_SCRIPT} -nostartfiles --ld-path=ld.lld -Wl,--gc-sections -Wl,--icf=all -Wl,--allow-multiple-definition"
)

add_custom_command(TARGET ai_demo POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ai_demo ai_demo.bin
    COMMAND ${CMAKE_SIZE} ai_demo
)
