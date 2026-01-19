set(TARGET_NAME ai_demo)

# 源文件
set(SOURCES
    main.c
    ai_model.c
)

add_executable(${TARGET_NAME}.elf ${SOURCES})

target_include_directories(${TARGET_NAME}.elf PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/kernel/include
    ${CMAKE_SOURCE_DIR}/arch/${ARCH}/include
    ${CMAKE_SOURCE_DIR}/drivers/include
    ${CMAKE_SOURCE_DIR}/ai/include
    ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src
)

target_link_libraries(${TARGET_NAME}.elf
    rv_aios_kernel
    rv_aios_drivers
    rv_aios_ai
)

set_target_properties(${TARGET_NAME}.elf PROPERTIES
    LINK_DEPENDS ${CMAKE_SOURCE_DIR}/arch/${ARCH}/link.ld
)

target_link_options(${TARGET_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/arch/${ARCH}/link.ld
    -nostartfiles
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.map
)

add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_NAME}.elf ${TARGET_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -D ${TARGET_NAME}.elf > ${TARGET_NAME}.asm
    COMMAND ${CMAKE_SIZE} ${TARGET_NAME}.elf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating ${TARGET_NAME}.bin and ${TARGET_NAME}.asm"
)
