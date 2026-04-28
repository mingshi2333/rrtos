# Central RISC-V ISA configuration

if(NOT DEFINED RISCV_MARCH)
  if(DEFINED ARCH_BITS AND ARCH_BITS STREQUAL "64")
    set(RISCV_MARCH "rv64imafdc" CACHE STRING "RISC-V march")
  elseif(DEFINED CONFIG_BOARD AND CONFIG_BOARD STREQUAL "be_u1000")
    set(RISCV_MARCH "rv32imafc_zifencei" CACHE STRING "RISC-V march")
  else()
    set(RISCV_MARCH "rv32imafd" CACHE STRING "RISC-V march")
  endif()
endif()

if(NOT DEFINED RISCV_MABI)
  if(DEFINED ARCH_BITS AND ARCH_BITS STREQUAL "64")
    set(RISCV_MABI "lp64d" CACHE STRING "RISC-V mabi")
  elseif(DEFINED CONFIG_BOARD AND CONFIG_BOARD STREQUAL "be_u1000")
    set(RISCV_MABI "ilp32f" CACHE STRING "RISC-V mabi")
  else()
    set(RISCV_MABI "ilp32d" CACHE STRING "RISC-V mabi")
  endif()
endif()

if(NOT DEFINED RISCV_ABI)
  if(DEFINED CONFIG_BOARD AND CONFIG_BOARD STREQUAL "be_u1000")
    # The top-level RV32 flag path still uses RISCV_ABI as its hard-float
    # selector. RISCV_MABI remains the actual compiler ABI.
    set(RISCV_ABI "ilp32d" CACHE STRING "RISC-V ABI selector")
  else()
    set(RISCV_ABI "${RISCV_MABI}" CACHE STRING "RISC-V ABI selector")
  endif()
endif()
