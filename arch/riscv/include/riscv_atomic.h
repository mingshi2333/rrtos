#ifndef RISCV_ATOMIC_H
#define RISCV_ATOMIC_H

#include "os_types.h"
#include "riscv_csr.h"

typedef struct {
    volatile int32_t value;
} os_atomic_t;

typedef struct {
    volatile int64_t value;
} os_atomic64_t;

#define OS_ATOMIC_INIT(v)   { .value = (v) }

OS_INLINE int32_t os_atomic_load(os_atomic_t *v) {
    int32_t ret;
    __asm__ volatile("lw %0, 0(%1)" : "=r"(ret) : "r"(&v->value) : "memory");
    return ret;
}

OS_INLINE void os_atomic_store(os_atomic_t *v, int32_t val) {
    __asm__ volatile("sw %0, 0(%1)" :: "r"(val), "r"(&v->value) : "memory");
}

OS_INLINE int32_t os_atomic_add(os_atomic_t *v, int32_t i) {
    int32_t ret;
    __asm__ volatile("amoadd.w %0, %2, (%1)"
        : "=r"(ret)
        : "r"(&v->value), "r"(i)
        : "memory");
    return ret + i;
}

OS_INLINE int32_t os_atomic_sub(os_atomic_t *v, int32_t i) {
    return os_atomic_add(v, -i);
}

OS_INLINE int32_t os_atomic_inc(os_atomic_t *v) {
    return os_atomic_add(v, 1);
}

OS_INLINE int32_t os_atomic_dec(os_atomic_t *v) {
    return os_atomic_add(v, -1);
}

OS_INLINE int32_t os_atomic_xchg(os_atomic_t *v, int32_t n) {
    int32_t ret;
    __asm__ volatile("amoswap.w %0, %2, (%1)"
        : "=r"(ret)
        : "r"(&v->value), "r"(n)
        : "memory");
    return ret;
}

OS_INLINE int32_t os_atomic_cmpxchg(os_atomic_t *v, int32_t expected, int32_t desired) {
    int32_t ret, tmp;
    __asm__ volatile(
        "1: lr.w %0, (%2)\n"
        "   bne %0, %3, 2f\n"
        "   sc.w %1, %4, (%2)\n"
        "   bnez %1, 1b\n"
        "2:"
        : "=&r"(ret), "=&r"(tmp)
        : "r"(&v->value), "r"(expected), "r"(desired)
        : "memory");
    return ret;
}

OS_INLINE int32_t os_atomic_or(os_atomic_t *v, int32_t i) {
    int32_t ret;
    __asm__ volatile("amoor.w %0, %2, (%1)"
        : "=r"(ret)
        : "r"(&v->value), "r"(i)
        : "memory");
    return ret | i;
}

OS_INLINE int32_t os_atomic_and(os_atomic_t *v, int32_t i) {
    int32_t ret;
    __asm__ volatile("amoand.w %0, %2, (%1)"
        : "=r"(ret)
        : "r"(&v->value), "r"(i)
        : "memory");
    return ret & i;
}

OS_INLINE int32_t os_atomic_xor(os_atomic_t *v, int32_t i) {
    int32_t ret;
    __asm__ volatile("amoxor.w %0, %2, (%1)"
        : "=r"(ret)
        : "r"(&v->value), "r"(i)
        : "memory");
    return ret ^ i;
}

OS_INLINE bool os_atomic_test_and_set(os_atomic_t *v, int32_t bit) {
    int32_t mask = 1 << bit;
    int32_t old = os_atomic_or(v, mask);
    return (old & mask) != 0;
}

OS_INLINE void os_spinlock_init(os_spinlock_t *lock) {
    lock->lock = 0;
#if OS_CFG_DEBUG_EN
    lock->owner = (os_cpu_t)-1;
    lock->name = NULL;
#endif
}

OS_INLINE void os_spinlock_lock(os_spinlock_t *lock) {
    while (1) {
        while (lock->lock != 0) {
            __asm__ volatile("nop");
        }
        int32_t tmp, ret;
        __asm__ volatile(
            "li %1, 1\n"
            "1: lr.w %0, (%2)\n"
            "   bnez %0, 1b\n"
            "   sc.w %0, %1, (%2)\n"
            "   bnez %0, 1b\n"
            "   fence rw, rw\n"
            : "=&r"(ret), "=&r"(tmp)
            : "r"(&lock->lock)
            : "memory");
        if (ret == 0) break;
    }
#if OS_CFG_DEBUG_EN
    lock->owner = os_cpu_id();
#endif
}

OS_INLINE bool os_spinlock_trylock(os_spinlock_t *lock) {
    int32_t tmp, ret;
    __asm__ volatile(
        "li %1, 1\n"
        "lr.w %0, (%2)\n"
        "bnez %0, 1f\n"
        "sc.w %0, %1, (%2)\n"
        "1: fence rw, rw\n"
        : "=&r"(ret), "=&r"(tmp)
        : "r"(&lock->lock)
        : "memory");
    if (ret == 0) {
#if OS_CFG_DEBUG_EN
        lock->owner = os_cpu_id();
#endif
        return true;
    }
    return false;
}

OS_INLINE void os_spinlock_unlock(os_spinlock_t *lock) {
#if OS_CFG_DEBUG_EN
    lock->owner = (os_cpu_t)-1;
#endif
    __asm__ volatile("fence rw, w" ::: "memory");
    lock->lock = 0;
}

OS_INLINE os_reg_t os_spinlock_irq_save(os_spinlock_t *lock) {
    os_reg_t flags;
    __asm__ volatile("csrrc %0, mstatus, %1" : "=r"(flags) : "r"(MSTATUS_MIE));
    os_spinlock_lock(lock);
    return flags;
}

OS_INLINE void os_spinlock_irq_restore(os_spinlock_t *lock, os_reg_t flags) {
    os_spinlock_unlock(lock);
    csr_write(mstatus, flags);
}

#endif
