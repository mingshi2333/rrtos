/**
 * @file hal_clic.c
 * @brief CLIC (Core-Local Interrupt Controller) driver for BE-U1000
 *
 * The BE-U1000 uses a CLIC interrupt controller instead of the standard
 * RISC-V PLIC. Key differences:
 * - mtvec mode = 0x3 (CLIC vectored mode)
 * - MTVT CSR (0x307) holds the vector table base
 * - Each interrupt has its own vector entry
 * - No claim/complete cycle — interrupts are auto-claimed on entry
 *
 * Timer (IRQ7) and software (IRQ3) interrupts still go through CLINT
 * as on standard RISC-V — only external interrupts use CLIC.
 */

#include "hal_irq.h"
#include "hal_uart.h"
#include <stddef.h>

/* Maximum number of CLIC interrupt sources */
#define CLIC_MAX_IRQS   80

/* CLIC CSR definitions */
#define CSR_MTVT        0x307

/* Interrupt handler table */
static hal_irq_handler_t clic_handlers[CLIC_MAX_IRQS];

/* CLIC base address (for memory-mapped control registers, if any) */
static volatile uint32_t *clic_base = NULL;

/* ============================================================================
 * CLIC Operations Implementation
 * ========================================================================= */

static void clic_init(os_ubase_t base)
{
    clic_base = (volatile uint32_t *)base;

    /* Clear all handler registrations */
    for (int i = 0; i < CLIC_MAX_IRQS; i++) {
        clic_handlers[i] = NULL;
    }

    /*
     * CLIC mode initialization is done in startup assembly:
     * - mtvec = vector_table | 0x3 (CLIC mode)
     * - MTVT CSR set to vector table base
     *
     * We handle the software side here (handler registration).
     */
}

static void clic_enable(uint32_t irq_num)
{
    if (irq_num >= CLIC_MAX_IRQS) return;

    /*
     * On CLIC, individual interrupt enable is controlled via
     * the clicintie[i] register. For now, interrupts are enabled
     * through the global MIE bit and per-source enable in the
     * CLIC memory-mapped registers.
     *
     * Simplified: rely on mie CSR bits for standard interrupts
     * (timer=7, sw=3) and CLIC IE registers for external.
     */
    if (clic_base) {
        /*
         * CLIC memory-mapped register layout (per interrupt):
         * Offset 0x1000 + irq_num*4: clicintctl[i] (control)
         * Bit 0: IE (interrupt enable)
         * Bit 1: IP (interrupt pending, read-only)
         * Bits 4-7: Priority level
         *
         * Note: Exact register layout depends on CLIC implementation.
         * This follows the RISC-V CLIC specification draft.
         */
        volatile uint8_t *clicintie = (volatile uint8_t *)((uintptr_t)clic_base + 0x1001 + irq_num * 4);
        *clicintie = 1;
    }
}

static void clic_disable(uint32_t irq_num)
{
    if (irq_num >= CLIC_MAX_IRQS) return;

    if (clic_base) {
        volatile uint8_t *clicintie = (volatile uint8_t *)((uintptr_t)clic_base + 0x1001 + irq_num * 4);
        *clicintie = 0;
    }
}

static void clic_set_priority(uint32_t irq_num, uint32_t priority)
{
    if (irq_num >= CLIC_MAX_IRQS) return;

    if (clic_base) {
        /* clicintctl register: bits 4-7 hold the priority level */
        volatile uint8_t *clicintctl = (volatile uint8_t *)((uintptr_t)clic_base + 0x1003 + irq_num * 4);
        *clicintctl = (uint8_t)((priority & 0xF) << 4);
    }
}

static uint32_t clic_claim(uint32_t hart)
{
    (void)hart;
    /*
     * In CLIC mode, the interrupt number is automatically determined
     * by the hardware and delivered via the vector table. The trap
     * handler reads mcause to get the IRQ number.
     *
     * This function is called from trap.c with the mcause-derived
     * IRQ number already known. We return 0 here as a no-op.
     * The actual dispatch is done via clic_dispatch().
     */
    return 0;
}

static void clic_complete(uint32_t hart, uint32_t irq_num)
{
    (void)hart;
    (void)irq_num;
    /*
     * CLIC auto-completes interrupts. No acknowledge cycle needed
     * (unlike PLIC claim/complete). This is a no-op.
     */
}

static void clic_register_handler(uint32_t irq_num, hal_irq_handler_t handler)
{
    if (irq_num < CLIC_MAX_IRQS) {
        clic_handlers[irq_num] = handler;
    }
}

/* ============================================================================
 * CLIC dispatch — called from trap handler for external interrupts
 * ========================================================================= */

/**
 * Dispatch a CLIC interrupt to the registered handler.
 * Called from os_trap_handler() when an external interrupt arrives.
 *
 * @param irq_num The interrupt number from mcause[9:0]
 */
void hal_clic_dispatch(uint32_t irq_num)
{
    if (irq_num < CLIC_MAX_IRQS && clic_handlers[irq_num]) {
        clic_handlers[irq_num](irq_num);
    } else {
        os_print("[CLIC] Unhandled IRQ %d\n", irq_num);
    }
}

/* ============================================================================
 * CLIC ops vtable
 * ========================================================================= */

static const hal_irq_ops_t clic_ops = {
    .init           = clic_init,
    .enable         = clic_enable,
    .disable        = clic_disable,
    .set_priority   = clic_set_priority,
    .claim          = clic_claim,
    .complete       = clic_complete,
    .register_handler = clic_register_handler,
    .type           = HAL_IRQ_TYPE_CLIC,
};

/* ============================================================================
 * Public initialization
 * ========================================================================= */

/* g_irq_ops defined here for BE-U1000 (hal_plic_irq.c is excluded from this build) */
const hal_irq_ops_t *g_irq_ops = (void *)0;

void hal_irq_init_clic(os_ubase_t base)
{
    g_irq_ops = &clic_ops;
    clic_init(base);
}
