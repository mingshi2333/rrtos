/**
 * @file hal_plic_irq.c
 * @brief PLIC adapter implementing the hal_irq abstraction
 *
 * Wraps the existing hal_plic functions into the hal_irq_ops_t interface
 * so the kernel trap handler can use a unified interrupt controller API.
 * Used on QEMU virt and other standard RISC-V platforms with PLIC.
 */

#include "hal_irq.h"
#include "hal_plic.h"

/* Global interrupt controller pointer */
const hal_irq_ops_t *g_irq_ops = (void *)0;

/* ============================================================================
 * PLIC ops wrappers
 * ========================================================================= */

static void plic_irq_init(os_ubase_t base)
{
    hal_plic_init(base);
}

static void plic_irq_enable(uint32_t irq_num)
{
    /* Default to hart 0 for single-core or primary core */
    hal_plic_enable(0, irq_num);
}

static void plic_irq_disable(uint32_t irq_num)
{
    hal_plic_disable(0, irq_num);
}

static void plic_irq_set_priority(uint32_t irq_num, uint32_t priority)
{
    hal_plic_set_priority(irq_num, priority);
}

static uint32_t plic_irq_claim(uint32_t hart)
{
    return hal_plic_claim(hart);
}

static void plic_irq_complete(uint32_t hart, uint32_t irq_num)
{
    hal_plic_complete(hart, irq_num);
}

/* Handler table for PLIC (for consistency with CLIC, though
 * typically handled in trap.c directly) */
#define PLIC_MAX_IRQS 128
static hal_irq_handler_t plic_handlers[PLIC_MAX_IRQS];

static void plic_irq_register_handler(uint32_t irq_num, hal_irq_handler_t handler)
{
    if (irq_num < PLIC_MAX_IRQS) {
        plic_handlers[irq_num] = handler;
    }
}

/**
 * Dispatch a PLIC interrupt to the registered handler.
 * Called from trap.c after claim.
 */
void hal_plic_dispatch(uint32_t irq_num)
{
    if (irq_num < PLIC_MAX_IRQS && plic_handlers[irq_num]) {
        plic_handlers[irq_num](irq_num);
    }
}

/* ============================================================================
 * PLIC ops vtable
 * ========================================================================= */

static const hal_irq_ops_t plic_irq_ops = {
    .init           = plic_irq_init,
    .enable         = plic_irq_enable,
    .disable        = plic_irq_disable,
    .set_priority   = plic_irq_set_priority,
    .claim          = plic_irq_claim,
    .complete       = plic_irq_complete,
    .register_handler = plic_irq_register_handler,
    .type           = HAL_IRQ_TYPE_PLIC,
};

void hal_irq_init_plic(os_ubase_t base)
{
    g_irq_ops = &plic_irq_ops;
    plic_irq_init(base);
}
