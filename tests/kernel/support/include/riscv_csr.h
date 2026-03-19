#ifndef RISCV_CSR_H
#define RISCV_CSR_H

#include "os_types.h"

#define MSTATUS_MIE     (1UL << 3)
#define MSTATUS_MPIE    (1UL << 7)
#define MSTATUS_MPP     (3UL << 11)
#define MSTATUS_FS_INITIAL (1UL << 13)
#define MSTATUS_VS_INITIAL (1UL << 9)

#define MIE_MSIE        (1UL << 3)
#define MIE_MTIE        (1UL << 7)

extern os_reg_t os_test_fake_mstatus;
extern os_reg_t os_test_fake_mie;
extern os_cpu_t os_test_fake_cpu_id;
uint64_t os_test_cycle_get(void);

OS_INLINE os_reg_t os_test_csr_read_mhartid(void) {
    return os_test_fake_cpu_id;
}

OS_INLINE os_reg_t os_test_csr_read_mstatus(void) {
    return os_test_fake_mstatus;
}

OS_INLINE os_reg_t os_test_csr_read_mie(void) {
    return os_test_fake_mie;
}

OS_INLINE void os_test_csr_write_mstatus(os_reg_t value) {
    os_test_fake_mstatus = value;
}

OS_INLINE void os_test_csr_write_mie(os_reg_t value) {
    os_test_fake_mie = value;
}

OS_INLINE void os_test_csr_set_mstatus(os_reg_t value) {
    os_test_fake_mstatus |= value;
}

OS_INLINE void os_test_csr_set_mie(os_reg_t value) {
    os_test_fake_mie |= value;
}

OS_INLINE void os_test_csr_clear_mstatus(os_reg_t value) {
    os_test_fake_mstatus &= ~value;
}

OS_INLINE void os_test_csr_clear_mie(os_reg_t value) {
    os_test_fake_mie &= ~value;
}

#define csr_read(csr) os_test_csr_read_##csr()
#define csr_write(csr, val) os_test_csr_write_##csr((os_reg_t)(val))
#define csr_set(csr, val) os_test_csr_set_##csr((os_reg_t)(val))
#define csr_clear(csr, val) os_test_csr_clear_##csr((os_reg_t)(val))

OS_INLINE os_cpu_t os_cpu_id(void) {
    return os_test_fake_cpu_id;
}

OS_INLINE void os_irq_enable(void) {
    os_test_fake_mstatus |= MSTATUS_MIE;
}

OS_INLINE void os_irq_disable(void) {
    os_test_fake_mstatus &= ~MSTATUS_MIE;
}

OS_INLINE os_reg_t os_irq_save(void) {
    os_reg_t flags = os_test_fake_mstatus;
    os_irq_disable();
    return flags;
}

OS_INLINE void os_irq_restore(os_reg_t flags) {
    os_test_fake_mstatus = flags;
}

OS_INLINE uint64_t os_cycle_get(void) {
    return os_test_cycle_get();
}

OS_INLINE void os_wfi(void) {
}

OS_INLINE void os_fence(void) {
}

OS_INLINE void os_fence_i(void) {
}

#endif
