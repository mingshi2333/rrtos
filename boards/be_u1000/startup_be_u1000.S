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
.equ CSR_MTVT,     0x307
.equ CSR_NMITVEC,  0x7C0

/* mtvec mode bits */
.equ MTVEC_MODE_CLIC, 0x3

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
_start:
    /* ---------------------------------------------------------------
     * Multi-core boot: only Core0 (mhartid=0) proceeds to boot
     * Core1 waits for a signal, Core2 is halted
     * --------------------------------------------------------------- */
    csrr    t0, mhartid
    andi    t0, t0, 0x3FF       /* Mask hart ID (BE-U1000 convention) */
    bnez    t0, _secondary_wait

    /* ---------------------------------------------------------------
     * Core0: Primary boot
     * --------------------------------------------------------------- */
_primary_boot:
    /* Disable interrupts during setup */
    csrw    mie, zero
    csrw    mstatus, zero

    /* Set up stack pointer (top of TCMB) */
    la      sp, _stack_top

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
    ori     t0, t0, MTVEC_MODE_CLIC
    csrw    mtvec, t0

    /* Set CLIC vector table (MTVT CSR) */
    la      t0, _clic_vector_table
    csrw    CSR_MTVT, t0

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
_secondary_wait:
    csrr    t0, mhartid
    andi    t0, t0, 0x3FF

    /* Core2 (hartid >= 2): halt forever */
    li      t1, 2
    bge     t0, t1, _halt

    /* Core1: Wait for primary to signal readiness */
    la      t1, _hart_ready
    slli    t2, t0, 2           /* word offset for this hart */
    add     t1, t1, t2
5:
    lw      t2, 0(t1)
    beqz    t2, 5b              /* Spin until non-zero */
    fence

    /* Set up stack for secondary hart (offset by 4KB per hart) */
    la      sp, _stack_top
    li      t2, 4096
    mul     t2, t0, t2
    sub     sp, sp, t2

    /* Set up GP */
    .option push
    .option norelax
    la      gp, __global_pointer$
    .option pop

    /* Configure CLIC for this hart */
    la      t1, _trap_entry
    ori     t1, t1, MTVEC_MODE_CLIC
    csrw    mtvec, t1
    la      t1, _clic_vector_table
    csrw    CSR_MTVT, t1

    /* Configure mstatus */
    li      t1, MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS
    csrw    mstatus, t1

    /* Enter kernel main on secondary hart (SMP init handled by kernel) */
    la      t1, os_kernel_main
    csrw    mepc, t1
    mret

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
 * In CLIC mode, each interrupt has a dedicated vector entry.
 * All entries jump to _trap_entry which dispatches via C code.
 * Specific high-frequency interrupts can be made direct for speed.
 * =================================================================== */
.align 8
.globl _clic_vector_table
_clic_vector_table:
    /* IRQ  0 */ j _trap_entry  /* Reserved */
    /* IRQ  1 */ j _trap_entry  /* Reserved */
    /* IRQ  2 */ j _trap_entry  /* Reserved */
    /* IRQ  3 */ j _trap_entry  /* Software IRQ (IPI) */
    /* IRQ  4 */ j _trap_entry  /* Reserved */
    /* IRQ  5 */ j _trap_entry  /* Reserved */
    /* IRQ  6 */ j _trap_entry  /* Reserved */
    /* IRQ  7 */ j _trap_entry  /* Timer IRQ (CLINT mtime) */
    /* IRQ  8 */ j _trap_entry  /* Reserved */
    /* IRQ  9 */ j _trap_entry  /* Reserved */
    /* IRQ 10 */ j _trap_entry  /* Reserved */
    /* IRQ 11 */ j _trap_entry  /* Reserved */
    /* IRQ 12 */ j _trap_entry  /* CLIC Software IRQ */
    /* IRQ 13 */ j _trap_entry  /* QSPI0 */
    /* IRQ 14 */ j _trap_entry  /* QSPI1 */
    /* IRQ 15 */ j _trap_entry  /* Reserved */
    /* IRQ 16 */ j _trap_entry  /* Reserved */
    /* IRQ 17 */ j _trap_entry  /* UART0 */
    /* IRQ 18 */ j _trap_entry  /* UART1 */
    /* IRQ 19 */ j _trap_entry  /* UART2 */
    /* IRQ 20 */ j _trap_entry  /* I2C0 */
    /* IRQ 21 */ j _trap_entry  /* I2C1 */
    /* IRQ 22 */ j _trap_entry  /* I2S0 */
    /* IRQ 23 */ j _trap_entry  /* TIM0_CH0 */
    /* IRQ 24 */ j _trap_entry  /* TIM0_CH1 */
    /* IRQ 25 */ j _trap_entry  /* TIM0_CH2 */
    /* IRQ 26 */ j _trap_entry  /* TIM0_CH3 */
    /* IRQ 27 */ j _trap_entry  /* WDT0 */
    /* IRQ 28 */ j _trap_entry  /* GPIO0 */
    /* IRQ 29 */ j _trap_entry  /* GPIO1 */
    /* IRQ 30 */ j _trap_entry  /* GPIO2 */
    /* IRQ 31 */ j _trap_entry  /* PWMG0 */
    /* IRQ 32 */ j _trap_entry  /* SPI0 */
    /* IRQ 33 */ j _trap_entry  /* SPI1 */
    /* IRQ 34 */ j _trap_entry  /* UART3 */
    /* IRQ 35 */ j _trap_entry  /* UART4 */
    /* IRQ 36 */ j _trap_entry  /* UART5 */
    /* IRQ 37 */ j _trap_entry  /* I2C2 */
    /* IRQ 38 */ j _trap_entry  /* I2C3 */
    /* IRQ 39 */ j _trap_entry  /* I2S1 */
    /* IRQ 40 */ j _trap_entry  /* TIM1_CH0 */
    /* IRQ 41 */ j _trap_entry  /* TIM1_CH1 */
    /* IRQ 42 */ j _trap_entry  /* TIM1_CH2 */
    /* IRQ 43 */ j _trap_entry  /* TIM1_CH3 */
    /* IRQ 44 */ j _trap_entry  /* WDT1 */
    /* IRQ 45 */ j _trap_entry  /* PWMG1 */
    /* IRQ 46 */ j _trap_entry  /* SPI2 */
    /* IRQ 47 */ j _trap_entry  /* SPI3 */
    /* IRQ 48 */ j _trap_entry  /* CANFD0 */
    /* IRQ 49 */ j _trap_entry  /* CANFD1 */
    /* IRQ 50 */ j _trap_entry  /* ADC0 */
    /* IRQ 51 */ j _trap_entry  /* ADC1 */
    /* IRQ 52 */ j _trap_entry  /* ADC2 */
    /* IRQ 53 */ j _trap_entry  /* PWMA0 */
    /* IRQ 54 */ j _trap_entry  /* PWMA1 */
    /* IRQ 55 */ j _trap_entry  /* PWMA2 */
    /* IRQ 56 */ j _trap_entry  /* PWMA3 */
    /* IRQ 57 */ j _trap_entry  /* QE0 */
    /* IRQ 58 */ j _trap_entry  /* QE1 */
    /* IRQ 59 */ j _trap_entry  /* QE2 */
    /* IRQ 60 */ j _trap_entry  /* QE3 */
    /* IRQ 61 */ j _trap_entry  /* USB */
    /* IRQ 62 */ j _trap_entry  /* UART6 */
    /* IRQ 63 */ j _trap_entry  /* UART7 */
    /* IRQ 64 */ j _trap_entry  /* DMA0 */
    /* IRQ 65 */ j _trap_entry  /* DMA1 */
    /* IRQ 66 */ j _trap_entry  /* USB_DMA */
    /* IRQ 67 */ j _trap_entry  /* CORE2 */
    /* IRQ 68 */ j _trap_entry  /* MAILBOX */
    /* IRQ 69 */ j _trap_entry  /* EXTI */
    /* IRQ 70 */ j _trap_entry  /* BUS_ERROR */
    /* IRQ 71 */ j _trap_entry  /* PLL_UNLOCK */
    /* IRQ 72-76: Reserved padding */
    /* IRQ 72 */ j _trap_entry
    /* IRQ 73 */ j _trap_entry
    /* IRQ 74 */ j _trap_entry
    /* IRQ 75 */ j _trap_entry
    /* IRQ 76 */ j _trap_entry

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
