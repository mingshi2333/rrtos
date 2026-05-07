# Auto-generated CMakeLists.txt for AI Models
# This library encapsulates the generated C registry and compiled model object files.

add_library(rv_aios_miniresnet_models STATIC
    ai_models.c
    miniresnetv1_s1_64x50_tl_int8.o
)

target_include_directories(rv_aios_miniresnet_models PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Link against the core AI runtime (rv_aios_ai) to inherit IREE headers and configuration
target_link_libraries(rv_aios_miniresnet_models PUBLIC rv_aios_ai)

# Apply compilation flags required for IREE on bare-metal
target_compile_options(rv_aios_miniresnet_models PRIVATE
    -Wno-type-limits -Wno-unused-variable -Wno-unused-function
    -Wno-unused-parameter -Wno-cast-function-type
    -Wno-implicit-function-declaration -Wno-sign-compare -Wno-pointer-sign
    -include${CMAKE_SOURCE_DIR}/ai/iree_bare_metal_config.h
)
