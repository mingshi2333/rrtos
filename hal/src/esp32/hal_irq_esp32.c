#include "hal_plic.h"
#include <stdint.h>

/* ESP32-C3 Interrupt Controller */
/* It's not a standard PLIC, but a custom interrupt matrix + CLIC-like controller */
/* For bare-metal simplistic support, we just map interrupts directly */

#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG 0x600C2000

void hal_plic_init(os_ubase_t base) {
    (void)base;
    /* Enable global interrupts in MSTATUS (done in start.S usually) */
    /* Configure interrupt matrix here if needed */
}

void hal_plic_enable(os_cpu_t hart, uint32_t irq) {
    (void)hart;
    /* Enable specific IRQ source */
    /* ESP32-C3 has 32 IRQ lines mapped to CPU */
    if (irq < 32) {
        volatile uint32_t *reg = (volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_ENABLE_REG;
        *reg |= (1 << irq);
    }
}

void hal_plic_disable(os_cpu_t hart, uint32_t irq) {
    (void)hart;
    if (irq < 32) {
        volatile uint32_t *reg = (volatile uint32_t *)INTERRUPT_CORE0_CPU_INT_ENABLE_REG;
        *reg &= ~(1 << irq);
    }
}

void hal_plic_set_priority(uint32_t irq, uint32_t priority) {
    /* ESP32-C3 priorities are set in interrupt map registers */
    (void)irq;
    (void)priority;
}

uint32_t hal_plic_claim(os_cpu_t hart) {
    (void)hart;
    /* On ESP32-C3, the MCAUSE register holds the exception cause */
    uint32_t mcause;
    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    
    if (mcause & 0x80000000) {
        /* Interrupt */
        return mcause & 0x1F; /* Exception code */
    }
    return 0;
}

void hal_plic_complete(os_cpu_t hart, uint32_t irq) {
    (void)hart;
    /* Nothing specific for ESP32-C3 CLIC-like controller completion usually,
       interrupt pending bit might need clearing depending on source */
    (void)irq;
}
