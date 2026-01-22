set(AI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ai_model_registry.c
)

add_library(rv_aios_ai STATIC ${AI_SOURCES})

target_link_libraries(rv_aios_ai PUBLIC
    rv_aios_rtos
    iree::base
    iree::vm
    iree::hal
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    iree::modules::hal
)

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
