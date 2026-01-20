# Central RISC-V ISA configuration

if(NOT DEFINED RISCV_MARCH)
  if(DEFINED ARCH_BITS AND ARCH_BITS STREQUAL "64")
    set(RISCV_MARCH "rv64imafdc" CACHE STRING "RISC-V march")
  else()
    set(RISCV_MARCH "rv32imafd" CACHE STRING "RISC-V march")
  endif()
endif()

if(NOT DEFINED RISCV_MABI)
  if(DEFINED ARCH_BITS AND ARCH_BITS STREQUAL "64")
    set(RISCV_MABI "lp64d" CACHE STRING "RISC-V mabi")
  else()
    set(RISCV_MABI "ilp32d" CACHE STRING "RISC-V mabi")
  endif()
endif()
