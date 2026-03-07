/**
 * @file hal_irq.h
 * @brief Unified interrupt controller abstraction layer
 *
 * Abstracts the differences between PLIC (standard RISC-V) and
 * CLIC (Baikal BE-U1000) interrupt controllers, allowing the kernel
 * trap handler to work with either model.
 */

#ifndef HAL_IRQ_H
#define HAL_IRQ_H

#include "os_types.h"

/**
 * Interrupt controller type
 */
typedef enum {
    HAL_IRQ_TYPE_PLIC = 0,  /**< Standard RISC-V PLIC */
    HAL_IRQ_TYPE_CLIC = 1,  /**< Core-Local Interrupt Controller (Baikal) */
} hal_irq_type_t;

/**
 * External interrupt handler callback
 */
typedef void (*hal_irq_handler_t)(uint32_t irq_num);

/**
 * Interrupt controller operations interface
 *
 * This vtable allows the kernel to interact with different interrupt
 * controllers (PLIC or CLIC) through a unified API.
 */
typedef struct hal_irq_ops {
    /** Initialize the interrupt controller */
    void (*init)(os_ubase_t base);

    /** Enable a specific interrupt source */
    void (*enable)(uint32_t irq_num);

    /** Disable a specific interrupt source */
    void (*disable)(uint32_t irq_num);

    /** Set interrupt priority (0 = disabled, higher = higher priority) */
    void (*set_priority)(uint32_t irq_num, uint32_t priority);

    /**
     * Claim the highest-priority pending interrupt.
     * PLIC: reads claim register
     * CLIC: returns irq_num extracted from mcause by trap handler
     */
    uint32_t (*claim)(uint32_t hart);

    /**
     * Complete/acknowledge an interrupt.
     * PLIC: writes to complete register
     * CLIC: typically a no-op (auto-cleared)
     */
    void (*complete)(uint32_t hart, uint32_t irq_num);

    /** Register a handler for a specific interrupt source */
    void (*register_handler)(uint32_t irq_num, hal_irq_handler_t handler);

    /** Get the interrupt controller type */
    hal_irq_type_t type;
} hal_irq_ops_t;

/* ============================================================================
 * Global interrupt controller instance
 * ========================================================================= */

/** Active interrupt controller (set during board init) */
extern const hal_irq_ops_t *g_irq_ops;

/* ============================================================================
 * Convenience API (delegates to g_irq_ops)
 * ========================================================================= */

static inline void hal_irq_init(os_ubase_t base) {
    if (g_irq_ops && g_irq_ops->init)
        g_irq_ops->init(base);
}

static inline void hal_irq_enable(uint32_t irq_num) {
    if (g_irq_ops && g_irq_ops->enable)
        g_irq_ops->enable(irq_num);
}

static inline void hal_irq_disable(uint32_t irq_num) {
    if (g_irq_ops && g_irq_ops->disable)
        g_irq_ops->disable(irq_num);
}

static inline void hal_irq_set_priority(uint32_t irq_num, uint32_t prio) {
    if (g_irq_ops && g_irq_ops->set_priority)
        g_irq_ops->set_priority(irq_num, prio);
}

static inline uint32_t hal_irq_claim(uint32_t hart) {
    if (g_irq_ops && g_irq_ops->claim)
        return g_irq_ops->claim(hart);
    return 0;
}

static inline void hal_irq_complete(uint32_t hart, uint32_t irq_num) {
    if (g_irq_ops && g_irq_ops->complete)
        g_irq_ops->complete(hart, irq_num);
}

static inline void hal_irq_register_handler(uint32_t irq_num, hal_irq_handler_t handler) {
    if (g_irq_ops && g_irq_ops->register_handler)
        g_irq_ops->register_handler(irq_num, handler);
}

/* ============================================================================
 * Backend initialization (called by board-specific code)
 * ========================================================================= */

/** Initialize PLIC backend and set g_irq_ops (standard RISC-V) */
void hal_irq_init_plic(os_ubase_t base);

/** Initialize CLIC backend and set g_irq_ops (BE-U1000) */
void hal_irq_init_clic(os_ubase_t base);

#endif /* HAL_IRQ_H */
