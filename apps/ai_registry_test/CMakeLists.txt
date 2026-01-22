add_executable(ai_registry_test main.c)

target_include_directories(ai_registry_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/zoo/iree_static
    ${CMAKE_SOURCE_DIR}/third_party/iree/third_party/flatcc/include
)

target_compile_options(ai_registry_test PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

# Link model object files (MNIST + YOLO with correct ilp32d ABI)
if(ARCH_BITS STREQUAL "64")
    set(MNIST_OBJ "st_mnist_28_rv64.o")
    set(YOLO_OBJ "yoloface_int8_rv64.o")
else()
    set(MNIST_OBJ "st_mnist_28.o")
    set(YOLO_OBJ "yoloface_int8.o")
endif()

target_link_libraries(ai_registry_test
    rv_aios_ai
    rv_aios_libc_shim
    rv_aios_drivers
    ${CMAKE_SOURCE_DIR}/zoo/iree_static/${MNIST_OBJ}
    ${CMAKE_SOURCE_DIR}/zoo/iree_static/${YOLO_OBJ}
    c m gcc gcc
)

set_target_properties(ai_registry_test PROPERTIES
    LINK_FLAGS "-T${LINKER_SCRIPT} -nostartfiles --ld-path=ld.lld -Wl,--gc-sections -Wl,--icf=all -Wl,--allow-multiple-definition"
)

add_custom_command(TARGET ai_registry_test POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ai_registry_test ai_registry_test.bin
    COMMAND ${CMAKE_SIZE} ai_registry_test
)
