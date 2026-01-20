#ifndef RISCV_CSR_H
#define RISCV_CSR_H

#include "os_types.h"

#define CSR_MSTATUS     0x300
#define CSR_MISA        0x301
#define CSR_MIE         0x304
#define CSR_MTVEC       0x305
#define CSR_MSCRATCH    0x340
#define CSR_MEPC        0x341
#define CSR_MCAUSE      0x342
#define CSR_MTVAL       0x343
#define CSR_MIP         0x344
#define CSR_MHARTID     0xF14

#define CSR_PMPCFG0     0x3A0
#define CSR_PMPCFG1     0x3A1
#define CSR_PMPCFG2     0x3A2
#define CSR_PMPCFG3     0x3A3
#define CSR_PMPADDR0    0x3B0
#define CSR_PMPADDR1    0x3B1
#define CSR_PMPADDR2    0x3B2
#define CSR_PMPADDR3    0x3B3

#define CSR_MCYCLE      0xB00
#define CSR_MINSTRET    0xB02
#define CSR_MCYCLEH     0xB80
#define CSR_MINSTRETH   0xB82

#define MSTATUS_MIE     (1UL << 3)
#define MSTATUS_MPIE    (1UL << 7)
#define MSTATUS_MPP     (3UL << 11)
#define MSTATUS_FS_INITIAL (1UL << 13)
#define MSTATUS_FS_CLEAN   (2UL << 13)
#define MSTATUS_FS_DIRTY   (3UL << 13)

#define MSTATUS_VS_INITIAL (1UL << 9)
#define MSTATUS_VS_CLEAN   (2UL << 9)
#define MSTATUS_VS_DIRTY   (3UL << 9)

#define MIE_MSIE        (1UL << 3)
#define MIE_MTIE        (1UL << 7)
#define MIE_MEIE        (1UL << 11)

#define MIP_MSIP        (1UL << 3)
#define MIP_MTIP        (1UL << 7)
#define MIP_MEIP        (1UL << 11)

#define PMP_R           (1 << 0)
#define PMP_W           (1 << 1)
#define PMP_X           (1 << 2)
#define PMP_A_OFF       (0 << 3)
#define PMP_A_TOR       (1 << 3)
#define PMP_A_NA4       (2 << 3)
#define PMP_A_NAPOT     (3 << 3)
#define PMP_L           (1 << 7)

#define csr_read(csr) ({ \
    os_reg_t __val; \
    __asm__ volatile("csrr %0, " #csr : "=r"(__val)); \
    __val; \
})

#define csr_write(csr, val) ({ \
    os_reg_t __val = (os_reg_t)(val); \
    __asm__ volatile("csrw " #csr ", %0" :: "r"(__val)); \
})

#define csr_set(csr, val) ({ \
    os_reg_t __val = (os_reg_t)(val); \
    __asm__ volatile("csrs " #csr ", %0" :: "r"(__val)); \
})

#define csr_clear(csr, val) ({ \
    os_reg_t __val = (os_reg_t)(val); \
    __asm__ volatile("csrc " #csr ", %0" :: "r"(__val)); \
})

#define csr_swap(csr, val) ({ \
    os_reg_t __val = (os_reg_t)(val); \
    os_reg_t __ret; \
    __asm__ volatile("csrrw %0, " #csr ", %1" : "=r"(__ret) : "r"(__val)); \
    __ret; \
})

OS_INLINE os_cpu_t os_cpu_id(void) {
    return (os_cpu_t)csr_read(mhartid);
}

OS_INLINE void os_irq_enable(void) {
    csr_set(mstatus, MSTATUS_MIE);
}

OS_INLINE void os_irq_disable(void) {
    csr_clear(mstatus, MSTATUS_MIE);
}

OS_INLINE os_reg_t os_irq_save(void) {
    os_reg_t mstatus;
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r"(mstatus) : "r"(MSTATUS_MIE));
    return mstatus;
}

OS_INLINE void os_irq_restore(os_reg_t flags) {
    csr_write(mstatus, flags);
}

OS_INLINE uint64_t os_cycle_get(void) {
#if OS_CFG_ARCH_BITS == 64
    return csr_read(mcycle);
#else
    uint32_t hi, lo, hi2;
    do {
        hi = csr_read(mcycleh);
        lo = csr_read(mcycle);
        hi2 = csr_read(mcycleh);
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | lo;
#endif
}

OS_INLINE void os_wfi(void) {
    __asm__ volatile("wfi");
}

OS_INLINE void os_fence(void) {
    __asm__ volatile("fence iorw, iorw" ::: "memory");
}

OS_INLINE void os_fence_i(void) {
    __asm__ volatile("fence.i" ::: "memory");
}

#endif
