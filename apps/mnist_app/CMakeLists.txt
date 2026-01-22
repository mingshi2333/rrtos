cmake_minimum_required(VERSION 3.20)
project(mnist_app C)

# Add the generated AI models library
add_subdirectory(generated)

add_executable(mnist_app src/main.c src/hooks.c)

target_link_libraries(mnist_app
    rv_aios_models
    rv_aios_libc_shim
    rv_aios_drivers
    rv_aios_kernel
    c m gcc gcc
)

target_include_directories(mnist_app PRIVATE
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

target_compile_options(mnist_app PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

set_target_properties(mnist_app PROPERTIES
    LINK_FLAGS "-T${LINKER_SCRIPT} -nostartfiles --ld-path=ld.lld -Wl,--gc-sections -Wl,--icf=all -Wl,--allow-multiple-definition"
)

add_custom_command(TARGET mnist_app POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary mnist_app mnist_app.bin
    COMMAND ${CMAKE_SIZE} mnist_app
)
