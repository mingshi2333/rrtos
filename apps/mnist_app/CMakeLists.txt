cmake_minimum_required(VERSION 3.20)
project(mnist_app C)

# Add the generated AI models library
add_subdirectory(generated)

set(MNIST_COMMON_LIBS
    rv_aios_models
    rv_aios_libc_shim
    rv_aios_drivers
    rv_aios_kernel
    c m gcc gcc
)

set(MNIST_COMMON_INCLUDES
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/hal/include
)

set(MNIST_COMMON_COMPILE_OPTIONS
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

function(configure_mnist_target target_name)
    target_link_libraries(${target_name}
        ${MNIST_COMMON_LIBS}
    )

    target_include_directories(${target_name} PRIVATE
        ${MNIST_COMMON_INCLUDES}
    )

    target_compile_options(${target_name} PRIVATE
        ${MNIST_COMMON_COMPILE_OPTIONS}
    )

    target_link_options(${target_name} PRIVATE
        -T${LINKER_SCRIPT}
        -nostartfiles
        --ld-path=ld.lld
        -Wl,--gc-sections
        -Wl,--icf=all
        -Wl,--allow-multiple-definition
        -Wl,-Map,$<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>.map
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${target_name} ${target_name}.bin
        COMMAND ${CMAKE_SIZE} ${target_name}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/report_build_footprint.py
                --label ${target_name}
                --image $<TARGET_FILE:${target_name}>
                --kernel-archive ${CMAKE_BINARY_DIR}/kernel/librv_aios_kernel.a
                --cache ${CMAKE_BINARY_DIR}/CMakeCache.txt
                --linker-script ${LINKER_SCRIPT}
                --map $<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>.map
                --output $<TARGET_FILE_DIR:${target_name}>/$<TARGET_FILE_BASE_NAME:${target_name}>_footprint.md
    )
endfunction()

add_executable(mnist_app src/main.c src/hooks.c)
configure_mnist_target(mnist_app)

add_executable(mnist_validation src/validation_main.c src/validation.c)
configure_mnist_target(mnist_validation)

add_executable(mnist_preemption_probe src/preemption_probe_main.c)
configure_mnist_target(mnist_preemption_probe)
