.section .text
.global os_context_switch
.global os_context_switch_first

#if __riscv_xlen == 64
    #define STORE    sd
    #define LOAD     ld
    #define REGBYTES 8
#else
    #define STORE    sw
    #define LOAD     lw
    #define REGBYTES 4
#endif

/* Stack frame size: 16 registers (ra, s0-s11, mstatus, 2 padding? or just enough)
   Saved: ra, s0-s11 (13 regs), mstatus (1 reg). Total 14.
   Original had 128 bytes. 128/8 = 16 words.
   Offset 104 is 13*8. 
   So we save 14 items. 14*REGBYTES.
   Stack alignment 16 bytes.
   Size = (14 * REGBYTES + 15) & ~15
*/

#if __riscv_xlen == 64
    #define STACK_SIZE 128
#else
    #define STACK_SIZE 64  /* 14*4 = 56. Aligned to 16 -> 64 */
#endif

os_context_switch:
    addi    sp, sp, -STACK_SIZE

    STORE   ra, 0*REGBYTES(sp)
    STORE   s0, 1*REGBYTES(sp)
    STORE   s1, 2*REGBYTES(sp)
    STORE   s2, 3*REGBYTES(sp)
    STORE   s3, 4*REGBYTES(sp)
    STORE   s4, 5*REGBYTES(sp)
    STORE   s5, 6*REGBYTES(sp)
    STORE   s6, 7*REGBYTES(sp)
    STORE   s7, 8*REGBYTES(sp)
    STORE   s8, 9*REGBYTES(sp)
    STORE   s9, 10*REGBYTES(sp)
    STORE   s10, 11*REGBYTES(sp)
    STORE   s11, 12*REGBYTES(sp)

    csrr    t0, mstatus
    STORE   t0, 13*REGBYTES(sp)

    STORE   sp, 0(a0)

    LOAD    sp, 0(a1)

    LOAD    t0, 13*REGBYTES(sp)
    csrw    mstatus, t0

    LOAD    ra, 0*REGBYTES(sp)
    LOAD    s0, 1*REGBYTES(sp)
    LOAD    s1, 2*REGBYTES(sp)
    LOAD    s2, 3*REGBYTES(sp)
    LOAD    s3, 4*REGBYTES(sp)
    LOAD    s4, 5*REGBYTES(sp)
    LOAD    s5, 6*REGBYTES(sp)
    LOAD    s6, 7*REGBYTES(sp)
    LOAD    s7, 8*REGBYTES(sp)
    LOAD    s8, 9*REGBYTES(sp)
    LOAD    s9, 10*REGBYTES(sp)
    LOAD    s10, 11*REGBYTES(sp)
    LOAD    s11, 12*REGBYTES(sp)

    addi    sp, sp, STACK_SIZE

    ret

os_context_switch_first:
    LOAD    sp, 0(a0)

    LOAD    t0, 13*REGBYTES(sp)
    csrw    mstatus, t0

    LOAD    ra, 0*REGBYTES(sp)
    LOAD    s0, 1*REGBYTES(sp)
    LOAD    s1, 2*REGBYTES(sp)
    LOAD    s2, 3*REGBYTES(sp)
    LOAD    s3, 4*REGBYTES(sp)
    LOAD    s4, 5*REGBYTES(sp)
    LOAD    s5, 6*REGBYTES(sp)
    LOAD    s6, 7*REGBYTES(sp)
    LOAD    s7, 8*REGBYTES(sp)
    LOAD    s8, 9*REGBYTES(sp)
    LOAD    s9, 10*REGBYTES(sp)
    LOAD    s10, 11*REGBYTES(sp)
    LOAD    s11, 12*REGBYTES(sp)

    addi    sp, sp, STACK_SIZE

    ret
