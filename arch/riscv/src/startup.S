#include "os_config.h"

.section .text.entry
.global _start
.global _trap_entry
.extern os_trap_handler
.extern os_kernel_main
#if OS_CFG_SMP_EN
.extern os_smp_secondary_start
#endif

.option norvc

#if __riscv_xlen == 64
    #define STORE    sd
    #define LOAD     ld
    #define REGBYTES 8
#else
    #define STORE    sw
    #define LOAD     lw
    #define REGBYTES 4
#endif

_start:
    csrr    t0, mhartid
#if OS_CFG_SMP_EN
    bnez    t0, _secondary_wait
#else
    bnez    t0, _halt
#endif

    la      sp, _stack_top

    la      t0, _bss_start
    la      t1, _bss_end
    bgeu    t0, t1, 2f
1:
    STORE   zero, 0(t0)
    addi    t0, t0, REGBYTES
    bltu    t0, t1, 1b
2:
    la      t0, _data_lma
    la      t1, _data_start
    la      t2, _data_end
    bgeu    t1, t2, 4f
3:
    LOAD    t3, 0(t0)
    STORE   t3, 0(t1)
    addi    t0, t0, REGBYTES
    addi    t1, t1, REGBYTES
    bltu    t1, t2, 3b
4:
    la      t0, _trap_entry
    csrw    mtvec, t0

    # Set mstatus: MPP=Machine(11) | FS=Initial(01)
    li      t0, 0x00003800
    csrw    mstatus, t0

    la      t0, os_kernel_main
    csrw    mepc, t0

    mret

#if OS_CFG_SMP_EN
_secondary_wait:
    la      t0, _hart_ready
    csrr    t1, mhartid
#if __riscv_xlen == 64
    slli    t1, t1, 3
#else
    slli    t1, t1, 2
#endif
    add     t0, t0, t1
5:
    LOAD    t2, 0(t0)
    beqz    t2, 5b

    la      sp, _stack_top
    csrr    t1, mhartid
    li      t2, 8192
    mul     t1, t1, t2
    sub     sp, sp, t1

    la      t0, _trap_entry
    csrw    mtvec, t0

    # Set mstatus: MPP=Machine(11) | FS=Initial(01)
    li      t0, 0x00003800
    csrw    mstatus, t0

    la      t0, os_smp_secondary_start
    csrw    mepc, t0
    mret
#endif

_halt:
    wfi
    j _halt

.align 4
_trap_entry:
    addi    sp, sp, -32 * REGBYTES

    STORE   ra, 0*REGBYTES(sp)
    STORE   t0, 1*REGBYTES(sp)
    STORE   t1, 2*REGBYTES(sp)
    STORE   t2, 3*REGBYTES(sp)
    STORE   t3, 4*REGBYTES(sp)
    STORE   t4, 5*REGBYTES(sp)
    STORE   t5, 6*REGBYTES(sp)
    STORE   t6, 7*REGBYTES(sp)
    STORE   a0, 8*REGBYTES(sp)
    STORE   a1, 9*REGBYTES(sp)
    STORE   a2, 10*REGBYTES(sp)
    STORE   a3, 11*REGBYTES(sp)
    STORE   a4, 12*REGBYTES(sp)
    STORE   a5, 13*REGBYTES(sp)
    STORE   a6, 14*REGBYTES(sp)
    STORE   a7, 15*REGBYTES(sp)
    STORE   s0, 16*REGBYTES(sp)
    STORE   s1, 17*REGBYTES(sp)
    STORE   s2, 18*REGBYTES(sp)
    STORE   s3, 19*REGBYTES(sp)
    STORE   s4, 20*REGBYTES(sp)
    STORE   s5, 21*REGBYTES(sp)
    STORE   s6, 22*REGBYTES(sp)
    STORE   s7, 23*REGBYTES(sp)
    STORE   s8, 24*REGBYTES(sp)
    STORE   s9, 25*REGBYTES(sp)
    STORE   s10, 26*REGBYTES(sp)
    STORE   s11, 27*REGBYTES(sp)

    csrr    t0, mepc
    csrr    t1, mcause
    csrr    t2, mtval
    STORE   t0, 28*REGBYTES(sp)
    STORE   t1, 29*REGBYTES(sp)
    STORE   t2, 30*REGBYTES(sp)

    mv      a0, sp
    la      t0, os_trap_handler
    jalr    ra, t0, 0

    LOAD    t0, 28*REGBYTES(sp)
    csrw    mepc, t0

    LOAD    ra, 0*REGBYTES(sp)
    LOAD    t0, 1*REGBYTES(sp)
    LOAD    t1, 2*REGBYTES(sp)
    LOAD    t2, 3*REGBYTES(sp)
    LOAD    t3, 4*REGBYTES(sp)
    LOAD    t4, 5*REGBYTES(sp)
    LOAD    t5, 6*REGBYTES(sp)
    LOAD    t6, 7*REGBYTES(sp)
    LOAD    a0, 8*REGBYTES(sp)
    LOAD    a1, 9*REGBYTES(sp)
    LOAD    a2, 10*REGBYTES(sp)
    LOAD    a3, 11*REGBYTES(sp)
    LOAD    a4, 12*REGBYTES(sp)
    LOAD    a5, 13*REGBYTES(sp)
    LOAD    a6, 14*REGBYTES(sp)
    LOAD    a7, 15*REGBYTES(sp)
    LOAD    s0, 16*REGBYTES(sp)
    LOAD    s1, 17*REGBYTES(sp)
    LOAD    s2, 18*REGBYTES(sp)
    LOAD    s3, 19*REGBYTES(sp)
    LOAD    s4, 20*REGBYTES(sp)
    LOAD    s5, 21*REGBYTES(sp)
    LOAD    s6, 22*REGBYTES(sp)
    LOAD    s7, 23*REGBYTES(sp)
    LOAD    s8, 24*REGBYTES(sp)
    LOAD    s9, 25*REGBYTES(sp)
    LOAD    s10, 26*REGBYTES(sp)
    LOAD    s11, 27*REGBYTES(sp)

    addi    sp, sp, 32 * REGBYTES

    mret

.section .data
.global _hart_ready
_hart_ready:
    .dword 0, 0, 0, 0, 0, 0, 0, 0
