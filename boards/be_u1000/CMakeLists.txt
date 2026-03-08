# Board support package for Baikal Electronics BE-U1000
#
# Usage: cmake -DCONFIG_BOARD=be_u1000 ...

message(STATUS "Board: BE-U1000 (Baikal Electronics RISC-V RV32IMAFC)")

# Board-specific sources
set(BOARD_BE_U1000_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_be_u1000.S
)

# Board include directories
set(BOARD_BE_U1000_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linker script selection
if(NOT DEFINED BE_U1000_MEMORY_MODEL)
    set(BE_U1000_MEMORY_MODEL "tcm" CACHE STRING "Memory model: tcm or flash")
endif()

if(BE_U1000_MEMORY_MODEL STREQUAL "flash")
    set(BOARD_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/link_be_u1000_flash.ld)
else()
    set(BOARD_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/link_be_u1000_tcm.ld)
endif()

# This file owns board-local sources and linker / memory-model selection only.
# Effective runtime policy such as OS_CFG_CPU_COUNT, OS_CFG_TIMER_FREQ_HZ,
# OS_CFG_SMP_EN, and feature toggles is selected in the top-level CMakeLists.txt.
# Historical duplicate board-local compile definitions are intentionally not
# exported from here so they do not act as a second source of runtime truth.

# Export to parent scope
set(BOARD_SOURCES ${BOARD_BE_U1000_SOURCES} PARENT_SCOPE)
set(BOARD_INCLUDES ${BOARD_BE_U1000_INCLUDES} PARENT_SCOPE)
set(BOARD_LINKER_SCRIPT ${BOARD_LINKER_SCRIPT} PARENT_SCOPE)
