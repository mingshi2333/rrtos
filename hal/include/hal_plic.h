#ifndef HAL_PLIC_H
#define HAL_PLIC_H

#include "os_types.h"

#define PLIC_PRIORITY_OFFSET        0x000000
#define PLIC_PENDING_OFFSET         0x001000
#define PLIC_ENABLE_OFFSET          0x002000
#define PLIC_THRESHOLD_OFFSET       0x200000
#define PLIC_CLAIM_OFFSET           0x200004

#define PLIC_ENABLE_STRIDE          0x80
#define PLIC_CONTEXT_STRIDE         0x1000

void hal_plic_init(os_ubase_t base);
void hal_plic_set_priority(uint32_t irq, uint32_t priority);
void hal_plic_enable(os_cpu_t hart, uint32_t irq);
void hal_plic_disable(os_cpu_t hart, uint32_t irq);
void hal_plic_set_threshold(os_cpu_t hart, uint32_t threshold);
uint32_t hal_plic_claim(os_cpu_t hart);
void hal_plic_complete(os_cpu_t hart, uint32_t irq);

#endif
