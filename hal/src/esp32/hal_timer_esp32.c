#include "hal_clint.h"
#include <stdint.h>

/* ESP32-C3 System Timer (SYSTIMER) */
/* Used to emulate CLINT timer behavior */
#define SYSTIMER_BASE           0x60023000
#define SYSTIMER_UNIT0_OP_OFFSET 0x00
#define SYSTIMER_UNIT0_LOAD_LO  0x04
#define SYSTIMER_UNIT0_LOAD_HI  0x08
#define SYSTIMER_UNIT0_VALUE_LO 0x0C
#define SYSTIMER_UNIT0_VALUE_HI 0x10
#define SYSTIMER_CONF_OFFSET    0x14
#define SYSTIMER_TARGET0_LO     0x40
#define SYSTIMER_TARGET0_HI     0x44
#define SYSTIMER_TARGET0_CONF   0x48

#define REG(base, off)          (*(volatile uint32_t *)((base) + (off)))

static volatile uint32_t g_systimer_base = SYSTIMER_BASE;

void hal_clint_init(os_ubase_t base) {
    if (base != 0) {
        g_systimer_base = base;
    }
    
    /* Enable SYSTIMER if needed, enable UNIT0 */
    REG(g_systimer_base, SYSTIMER_CONF_OFFSET) |= (1 << 24); // CLK_EN
}

uint64_t hal_clint_mtime_get(void) {
    /* Need to trigger update first in some modes, but C3 usually free-runs */
    /* Read update bit */
    REG(g_systimer_base, SYSTIMER_UNIT0_OP_OFFSET) = (1 << 30); // UPDATE
    
    /* Wait for valid? (Usually fast) */
    
    uint32_t lo = REG(g_systimer_base, SYSTIMER_UNIT0_VALUE_LO);
    uint32_t hi = REG(g_systimer_base, SYSTIMER_UNIT0_VALUE_HI);
    
    return ((uint64_t)hi << 32) | lo;
}

void hal_clint_mtimecmp_set(os_cpu_t hart, uint64_t time_cmp) {
    (void)hart;
    REG(g_systimer_base, SYSTIMER_TARGET0_LO) = (uint32_t)(time_cmp & 0xFFFFFFFF);
    REG(g_systimer_base, SYSTIMER_TARGET0_HI) = (uint32_t)(time_cmp >> 32);
    
    /* Enable Target 0 interrupt */
    REG(g_systimer_base, SYSTIMER_TARGET0_CONF) |= (1 << 31); // ENABLE
}
