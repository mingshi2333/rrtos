#include "hal_plic.h"

static volatile os_ubase_t g_plic_base;

void hal_plic_init(os_ubase_t base) {
    g_plic_base = base;
}

void hal_plic_set_priority(uint32_t irq, uint32_t priority) {
    volatile uint32_t *prio = (volatile uint32_t *)(g_plic_base + PLIC_PRIORITY_OFFSET + irq * 4);
    *prio = priority;
}

void hal_plic_enable(os_cpu_t hart, uint32_t irq) {
    os_ubase_t addr = g_plic_base + PLIC_ENABLE_OFFSET + hart * PLIC_ENABLE_STRIDE + (irq / 32) * 4;
    volatile uint32_t *enable = (volatile uint32_t *)addr;
    *enable |= (1U << (irq % 32));
}

void hal_plic_disable(os_cpu_t hart, uint32_t irq) {
    os_ubase_t addr = g_plic_base + PLIC_ENABLE_OFFSET + hart * PLIC_ENABLE_STRIDE + (irq / 32) * 4;
    volatile uint32_t *enable = (volatile uint32_t *)addr;
    *enable &= ~(1U << (irq % 32));
}

void hal_plic_set_threshold(os_cpu_t hart, uint32_t threshold) {
    volatile uint32_t *thr = (volatile uint32_t *)(g_plic_base + PLIC_THRESHOLD_OFFSET + hart * PLIC_CONTEXT_STRIDE);
    *thr = threshold;
}

uint32_t hal_plic_claim(os_cpu_t hart) {
    volatile uint32_t *claim = (volatile uint32_t *)(g_plic_base + PLIC_CLAIM_OFFSET + hart * PLIC_CONTEXT_STRIDE);
    return *claim;
}

void hal_plic_complete(os_cpu_t hart, uint32_t irq) {
    volatile uint32_t *complete = (volatile uint32_t *)(g_plic_base + PLIC_CLAIM_OFFSET + hart * PLIC_CONTEXT_STRIDE);
    *complete = irq;
}
