set(AI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_model_registry.c
)

add_library(rv_aios_ai STATIC ${AI_SOURCES})

target_link_libraries(rv_aios_ai PUBLIC
    rv_aios_rtos
    iree::base
    iree::vm
    iree::hal
)

if(NOT DEFINED AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE)
    set(AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE 1)
endif()

if(NOT DEFINED AI_IREE_ENABLE_INLINE_HAL_VALUE)
    set(AI_IREE_ENABLE_INLINE_HAL_VALUE 0)
endif()

if(AI_IREE_ENABLE_STATIC_LIBRARY_HAL_VALUE)
    target_link_libraries(rv_aios_ai PUBLIC
        iree::hal::drivers::local_sync::sync_driver
        iree::hal::local::loaders::static_library_loader
        iree::modules::hal
    )
endif()

if(AI_IREE_ENABLE_INLINE_HAL_VALUE)
    target_link_libraries(rv_aios_ai PUBLIC
        iree::modules::hal::inline
    )
endif()

target_compile_definitions(rv_aios_ai PUBLIC EMITC_IMPLEMENTATION)

target_compile_options(rv_aios_ai PRIVATE
    -Wno-error -Wno-type-limits -Wno-unused-variable
    -Wno-unused-function -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

target_include_directories(rv_aios_ai PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(rv_aios_ai_descriptor STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_descriptor_runtime.c
)

target_include_directories(rv_aios_ai_descriptor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(rv_aios_ai_static_direct STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_static_direct.c
)

target_include_directories(rv_aios_ai_static_direct PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
)

add_library(rv_aios_ai_ukernel STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ukernel/rrtos_ai_ukernel.c
)

target_include_directories(rv_aios_ai_ukernel PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ukernel>
)

target_compile_definitions(rv_aios_ai_static_direct PUBLIC
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

target_compile_options(rv_aios_ai_static_direct PRIVATE
    -Wno-error -Wno-type-limits -Wno-unused-variable
    -Wno-unused-function -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)

add_library(rv_aios_libc_shim OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/src/malloc_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stdio_wrapper.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/iree_rtos_sync.c
)

target_link_libraries(rv_aios_libc_shim PRIVATE rv_aios_rtos)

target_include_directories(rv_aios_libc_shim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
)
