add_library(rv_aios_cxx STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cxx_runtime.cpp
)

target_compile_features(rv_aios_cxx PUBLIC cxx_std_17)

target_compile_options(rv_aios_cxx PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-asynchronous-unwind-tables>
)

target_include_directories(rv_aios_cxx PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/kernel/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/hal/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/config>
)

target_link_libraries(rv_aios_cxx PUBLIC
    rv_aios_rtos
    etl::etl
)
