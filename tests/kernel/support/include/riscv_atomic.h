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

#define OS_ATOMIC_INIT(v) { .value = (v) }

OS_INLINE int32_t os_atomic_load(os_atomic_t *v) {
    return v->value;
}

OS_INLINE void os_atomic_store(os_atomic_t *v, int32_t val) {
    v->value = val;
}

OS_INLINE int32_t os_atomic_add(os_atomic_t *v, int32_t i) {
    v->value += i;
    return v->value;
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
    int32_t old = v->value;
    v->value = n;
    return old;
}

OS_INLINE int32_t os_atomic_cmpxchg(os_atomic_t *v, int32_t expected, int32_t desired) {
    int32_t old = v->value;
    if (old == expected) {
        v->value = desired;
    }
    return old;
}

OS_INLINE int32_t os_atomic_or(os_atomic_t *v, int32_t i) {
    int32_t old = v->value;
    v->value |= i;
    return old;
}

OS_INLINE int32_t os_atomic_and(os_atomic_t *v, int32_t i) {
    int32_t old = v->value;
    v->value &= i;
    return old;
}

OS_INLINE int32_t os_atomic_xor(os_atomic_t *v, int32_t i) {
    int32_t old = v->value;
    v->value ^= i;
    return old;
}

OS_INLINE bool os_atomic_test_and_set(os_atomic_t *v, int32_t bit) {
    int32_t mask = 1 << bit;
    bool was_set = (v->value & mask) != 0;
    v->value |= mask;
    return was_set;
}

OS_INLINE void os_spinlock_init(os_spinlock_t *lock) {
    lock->lock = 0;
#if OS_CFG_DEBUG_EN
    lock->owner = (os_cpu_t)-1;
    lock->name = NULL;
#endif
}

OS_INLINE void os_spinlock_lock(os_spinlock_t *lock) {
    lock->lock = 1;
#if OS_CFG_DEBUG_EN
    lock->owner = os_cpu_id();
#endif
}

OS_INLINE bool os_spinlock_trylock(os_spinlock_t *lock) {
    if (lock->lock != 0) {
        return false;
    }
    os_spinlock_lock(lock);
    return true;
}

OS_INLINE void os_spinlock_unlock(os_spinlock_t *lock) {
#if OS_CFG_DEBUG_EN
    lock->owner = (os_cpu_t)-1;
#endif
    lock->lock = 0;
}

OS_INLINE os_reg_t os_spinlock_irq_save(os_spinlock_t *lock) {
    os_reg_t flags = os_irq_save();
    os_spinlock_lock(lock);
    return flags;
}

OS_INLINE void os_spinlock_irq_restore(os_spinlock_t *lock, os_reg_t flags) {
    os_spinlock_unlock(lock);
    os_irq_restore(flags);
}

#endif
