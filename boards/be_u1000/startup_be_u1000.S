/**
 * @file startup_be_u1000.S
 * @brief Startup code for BE-U1000 RISC-V MCU (Core0/Core1)
 *
 * Key differences from generic RRTOS startup:
 * 1. CLIC interrupt mode (mtvec mode = 0x3, MTVT CSR for vector table)
 * 2. 3-core asymmetric: Core0 boots, Core1 waits, Core2 halted
 * 3. Memory layout: TCMA/TCMB/EFLASH instead of flat RAM
 * 4. DesignWare UART (word-addressed) instead of 16550
 */

/* CLIC CSR addresses */
.equ BEU1000_ASM_CSR_MTVT,     0x307
.equ BEU1000_ASM_CSR_NMITVEC,  0x7C0

/* mtvec mode bits */
.equ BEU1000_ASM_MTVEC_MODE_CLIC, 0x3

/* mstatus bits */
.equ MSTATUS_MIE,   0x00000008  /* Machine Interrupt Enable */
.equ MSTATUS_MPIE,  0x00000080  /* Machine Previous IE */
.equ MSTATUS_MPP,   0x00001800  /* Machine Previous Privilege (M-mode) */
.equ MSTATUS_FS,    0x00002000  /* FPU State: Initial */

/* mie bits */
.equ MIE_MSIE,      0x00000008  /* Machine Software Interrupt Enable */
.equ MIE_MTIE,      0x00000080  /* Machine Timer Interrupt Enable */

.section .text.init, "ax", @progbits
.globl _start
.type _start, @function
#if OS_CFG_SMP_EN
.globl _start1
.type _start1, @function
.extern os_smp_secondary_start
#endif

_start:
    /* ---------------------------------------------------------------
     * Multi-core boot: only Core0 (mhartid=0) proceeds to boot
     * Core1 waits for a signal, Core2 is halted
     * --------------------------------------------------------------- */
    csrr    t0, mhartid
    andi    t0, t0, 0x3FF       /* Mask hart ID (BE-U1000 convention) */
#if !OS_CFG_SMP_EN
    bnez    t0, _halt
#else
    bnez    t0, _secondary_wait
#endif
    j       _primary_boot

#if OS_CFG_SMP_EN
.align 2
_start1:
    li      t0, 1
    j       _secondary_boot
#endif

    /* ---------------------------------------------------------------
     * Core0: Primary boot
     * --------------------------------------------------------------- */
_primary_boot:
    /* Disable interrupts during setup */
    csrw    mie, zero
    csrw    mstatus, zero

    /* Set up stack pointer (hart0 top of the BE-U1000 stack bank) */
    la      sp, __hart0_stack_top

    /* Set up global pointer */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* ---------------------------------------------------------------
     * Copy .data from Flash (LMA) to RAM (VMA)
     * --------------------------------------------------------------- */
    la      t0, _data_start     /* VMA destination */
    la      t1, _data_lma       /* LMA source (in flash) */
    la      t2, _data_end
    beq     t0, t1, 2f          /* Skip if VMA == LMA (RAM-only boot) */
1:
    bge     t0, t2, 2f
    lw      t3, 0(t1)
    sw      t3, 0(t0)
    addi    t0, t0, 4
    addi    t1, t1, 4
    j       1b
2:

    /* ---------------------------------------------------------------
     * Clear .bss
     * --------------------------------------------------------------- */
    la      t0, _bss_start
    la      t1, _bss_end
3:
    bge     t0, t1, 4f
    sw      zero, 0(t0)
    addi    t0, t0, 4
    j       3b
4:

    /* ---------------------------------------------------------------
     * Configure CLIC interrupt controller
     *
     * Unlike standard RISC-V (mtvec = trap_entry, direct/vectored mode),
     * CLIC uses:
     *   mtvec = _trap_entry | 0x3 (CLIC mode bits)
     *   MTVT  = _clic_vector_table (base of vector table)
     * --------------------------------------------------------------- */
    la      t0, _trap_entry
    ori     t0, t0, BEU1000_ASM_MTVEC_MODE_CLIC
    csrw    mtvec, t0

    /* Set CLIC vector table (MTVT CSR) */
    la      t0, _clic_vector_table
    csrw    BEU1000_ASM_CSR_MTVT, t0

    /* ---------------------------------------------------------------
     * Configure mstatus for kernel entry
     * MPP = M-mode, MPIE = 1, FS = Initial (for FPU)
     * --------------------------------------------------------------- */
    li      t0, MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS
    csrw    mstatus, t0

    /* ---------------------------------------------------------------
     * Jump to RTOS kernel via mret
     * --------------------------------------------------------------- */
    la      t0, os_kernel_main
    csrw    mepc, t0
    mret

/* ===================================================================
 * Secondary hart (Core1) wait loop
 * =================================================================== */
#if OS_CFG_SMP_EN
_secondary_wait:
    csrr    t0, mhartid
    andi    t0, t0, 0x3FF

    /* Core2 (hartid >= 2): halt forever */
    li      t1, 2
    bge     t0, t1, _halt

_secondary_boot:
    /* Core1: Wait for primary to signal readiness */
    la      t1, _hart_ready
    slli    t2, t0, 2           /* word offset for this hart */
    add     t1, t1, t2
5:
    lw      t2, 0(t1)
    beqz    t2, 5b              /* Spin until non-zero */
    fence

    /* Set up stack for the supported experimental secondary hart. */
    la      sp, __hart1_stack_top

    /* Set up GP */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* Configure CLIC for this hart */
    la      t1, _trap_entry
    ori     t1, t1, BEU1000_ASM_MTVEC_MODE_CLIC
    csrw    mtvec, t1
    la      t1, _clic_vector_table
    csrw    BEU1000_ASM_CSR_MTVT, t1

    /* Configure mstatus */
    li      t1, MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS
    csrw    mstatus, t1

    /* Enter the secondary CPU path. */
    la      t1, os_smp_secondary_start
    csrw    mepc, t1
    mret
#endif

_halt:
    wfi
    j       _halt

/* ===================================================================
 * Trap entry (shared for all interrupts/exceptions)
 *
 * In CLIC mode, mtvec still points here for synchronous exceptions.
 * Vectored CLIC interrupts go through the vector table (MTVT),
 * but the default trap handler catches anything not vectored.
 * =================================================================== */
.align 6
.globl _trap_entry
_trap_entry:
    /* Save all 32 registers + mepc/mcause/mtval */
    addi    sp, sp, -128

    sw      ra,   0(sp)
    sw      t0,   4(sp)
    sw      t1,   8(sp)
    sw      t2,  12(sp)
    sw      s0,  16(sp)
    sw      s1,  20(sp)
    sw      a0,  24(sp)
    sw      a1,  28(sp)
    sw      a2,  32(sp)
    sw      a3,  36(sp)
    sw      a4,  40(sp)
    sw      a5,  44(sp)
    sw      a6,  48(sp)
    sw      a7,  52(sp)
    sw      s2,  56(sp)
    sw      s3,  60(sp)
    sw      s4,  64(sp)
    sw      s5,  68(sp)
    sw      s6,  72(sp)
    sw      s7,  76(sp)
    sw      s8,  80(sp)
    sw      s9,  84(sp)
    sw      s10, 88(sp)
    sw      s11, 92(sp)
    sw      t3,  96(sp)
    sw      t4, 100(sp)
    sw      t5, 104(sp)
    sw      t6, 108(sp)

    csrr    t0, mepc
    sw      t0, 112(sp)
    csrr    t0, mcause
    sw      t0, 116(sp)
    csrr    t0, mtval
    sw      t0, 120(sp)
    sw      gp, 124(sp)

    /* Call C trap handler with stack pointer as argument */
    mv      a0, sp
    call    os_trap_handler

    /* Restore registers */
    lw      t0, 112(sp)
    csrw    mepc, t0

    lw      ra,   0(sp)
    lw      t0,   4(sp)
    lw      t1,   8(sp)
    lw      t2,  12(sp)
    lw      s0,  16(sp)
    lw      s1,  20(sp)
    lw      a0,  24(sp)
    lw      a1,  28(sp)
    lw      a2,  32(sp)
    lw      a3,  36(sp)
    lw      a4,  40(sp)
    lw      a5,  44(sp)
    lw      a6,  48(sp)
    lw      a7,  52(sp)
    lw      s2,  56(sp)
    lw      s3,  60(sp)
    lw      s4,  64(sp)
    lw      s5,  68(sp)
    lw      s6,  72(sp)
    lw      s7,  76(sp)
    lw      s8,  80(sp)
    lw      s9,  84(sp)
    lw      s10, 88(sp)
    lw      s11, 92(sp)
    lw      t3,  96(sp)
    lw      t4, 100(sp)
    lw      t5, 104(sp)
    lw      t6, 108(sp)
    lw      gp, 124(sp)

    addi    sp, sp, 128
    mret

/* ===================================================================
 * CLIC Vector Table
 *
 * In BE-U1000 CLIC mode, MTVT points at a table of handler addresses, not
 * executable jump instructions.  Keep every interrupt routed through the
 * shared trap frame path so the C handler can dispatch by mcause.
 * =================================================================== */
.align 8
.globl _clic_vector_table
.type _clic_vector_table, @object
_clic_vector_table:
    .rept 78
    .word _trap_entry
    .endr
.size _clic_vector_table, . - _clic_vector_table

/* ===================================================================
 * Data
 * =================================================================== */
.section .data
.align 2
.globl _hart_ready
_hart_ready:
    .word 0     /* Hart 0 — not used (primary) */
    .word 0     /* Hart 1 — set by primary to wake secondary */
    .word 0     /* Hart 2 — not used (halted) */
