cmake_minimum_required(VERSION 3.20)
project(mnist_app C)

# Add the generated AI models library
add_subdirectory(generated)
add_subdirectory(generated_mnist_vmvx)
add_subdirectory(generated_quant)
add_subdirectory(generated_miniresnet)
add_subdirectory(generated_miniresnet_minpeak)
add_subdirectory(generated_miniresnet_stream_debug)

set(MNIST_RUNTIME_LIBS
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

function(configure_mnist_target_with_models target_name model_library)
    target_link_libraries(${target_name}
        ${model_library}
        ${MNIST_RUNTIME_LIBS}
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

function(configure_mnist_target target_name)
    configure_mnist_target_with_models(${target_name} rv_aios_models)
endfunction()

set(MNIST_STATIC_DIRECT_MODEL_OBJECT
    "${CMAKE_SOURCE_DIR}/apps/mnist_app/generated/st_mnist_28.o"
    CACHE FILEPATH "Generated IREE MNIST static executable object for QEMU direct validation")

set(MINIRESNET_STATIC_DIRECT_MODEL_OBJECT
    "${CMAKE_SOURCE_DIR}/apps/mnist_app/generated_miniresnet/miniresnetv1_s1_64x50_tl_int8.o"
    CACHE FILEPATH "Generated IREE MiniResNet static executable object for QEMU direct validation")

if(NOT EXISTS "${MNIST_STATIC_DIRECT_MODEL_OBJECT}")
    message(FATAL_ERROR "Missing MNIST static-direct model object: ${MNIST_STATIC_DIRECT_MODEL_OBJECT}")
endif()

if(NOT EXISTS "${MINIRESNET_STATIC_DIRECT_MODEL_OBJECT}")
    message(FATAL_ERROR "Missing MiniResNet static-direct model object: ${MINIRESNET_STATIC_DIRECT_MODEL_OBJECT}")
endif()

set_source_files_properties(
    "${MNIST_STATIC_DIRECT_MODEL_OBJECT}"
    PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE)

set_source_files_properties(
    "${MINIRESNET_STATIC_DIRECT_MODEL_OBJECT}"
    PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE)

add_library(mnist_st_mnist_28_const OBJECT
    src/static_direct_model_const.c
)

target_include_directories(mnist_st_mnist_28_const PRIVATE
    ${MNIST_COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/apps/mnist_app
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
)

target_compile_definitions(mnist_st_mnist_28_const PRIVATE
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

target_compile_options(mnist_st_mnist_28_const PRIVATE
    ${MNIST_COMMON_COMPILE_OPTIONS}
)

add_library(miniresnet_static_direct_const OBJECT
    src/miniresnet_static_direct_model_const.c
)

target_include_directories(miniresnet_static_direct_const PRIVATE
    ${MNIST_COMMON_INCLUDES}
    ${CMAKE_SOURCE_DIR}/apps/mnist_app
    ${CMAKE_SOURCE_DIR}/config
    ${CMAKE_SOURCE_DIR}/memory/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
)

target_compile_definitions(miniresnet_static_direct_const PRIVATE
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

target_compile_options(miniresnet_static_direct_const PRIVATE
    ${MNIST_COMMON_COMPILE_OPTIONS}
)

function(configure_mnist_static_direct_target target_name)
    target_link_libraries(${target_name}
        rv_aios_ai_static_direct
        ${MNIST_RUNTIME_LIBS}
    )

    target_include_directories(${target_name} PRIVATE
        ${MNIST_COMMON_INCLUDES}
        ${CMAKE_SOURCE_DIR}/apps/mnist_app
        ${CMAKE_SOURCE_DIR}/apps/mnist_app/src
        ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
    )

    target_compile_definitions(${target_name} PRIVATE
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

function(configure_mnist_descriptor_target target_name)
    target_link_libraries(${target_name}
        rv_aios_ai_descriptor
        ${MNIST_RUNTIME_LIBS}
    )

    target_include_directories(${target_name} PRIVATE
        ${MNIST_COMMON_INCLUDES}
        ${CMAKE_SOURCE_DIR}/apps/mnist_app/src
    )

    target_compile_options(${target_name} PRIVATE
        -Wno-type-limits -Wno-unused-variable -Wno-unused-function
        -Wno-unused-parameter -Wno-cast-function-type
        -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
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

add_executable(mnist_vmvx_validation src/validation_main.c src/validation.c)
configure_mnist_target_with_models(mnist_vmvx_validation rv_aios_mnist_vmvx_models)

add_executable(mnist_static_direct_validation
    src/static_direct_validation_main.c
    $<TARGET_OBJECTS:mnist_st_mnist_28_const>
    "${MNIST_STATIC_DIRECT_MODEL_OBJECT}"
)
configure_mnist_static_direct_target(mnist_static_direct_validation)

add_executable(mnist_descriptor_validation
    src/descriptor_validation_main.c
    src/descriptor_mnist_model.c
    $<TARGET_OBJECTS:mnist_st_mnist_28_const>
)
configure_mnist_descriptor_target(mnist_descriptor_validation)

add_executable(mnist_quant_validation src/quant_validation_main.c)
configure_mnist_target_with_models(mnist_quant_validation rv_aios_mnist_quant_models)

add_executable(miniresnet_validation src/miniresnet_validation_main.c)
configure_mnist_target_with_models(miniresnet_validation rv_aios_miniresnet_models)

add_executable(miniresnet_static_direct_validation
    src/miniresnet_static_direct_validation_main.c
    src/miniresnet_static_direct_desc.c
    $<TARGET_OBJECTS:miniresnet_static_direct_const>
    "${MINIRESNET_STATIC_DIRECT_MODEL_OBJECT}"
)
configure_mnist_static_direct_target(miniresnet_static_direct_validation)

add_executable(miniresnet_patch_validation
    src/miniresnet_patch_validation_main.c
    src/miniresnet_patch_arena_remap.c
    src/miniresnet_patch_first_layer.c
    src/miniresnet_static_direct_desc.c
    $<TARGET_OBJECTS:miniresnet_static_direct_const>
    "${MINIRESNET_STATIC_DIRECT_MODEL_OBJECT}"
)
configure_mnist_static_direct_target(miniresnet_patch_validation)
target_link_libraries(miniresnet_patch_validation rv_aios_ai_ukernel)

add_executable(miniresnet_minpeak_validation src/miniresnet_validation_main.c)
configure_mnist_target_with_models(miniresnet_minpeak_validation rv_aios_miniresnet_minpeak_models)

add_executable(miniresnet_stream_debug_validation src/miniresnet_validation_main.c)
configure_mnist_target_with_models(miniresnet_stream_debug_validation rv_aios_miniresnet_stream_debug_models)

add_executable(mnist_preemption_probe src/preemption_probe_main.c)
configure_mnist_target(mnist_preemption_probe)
