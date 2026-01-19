#include "hal_clint.h"

static volatile os_ubase_t g_clint_base;

void hal_clint_init(os_ubase_t base) {
    g_clint_base = base;
}

uint64_t hal_clint_mtime_get(void) {
    volatile uint64_t *mtime = (volatile uint64_t *)(g_clint_base + CLINT_MTIME_OFFSET);
    return *mtime;
}

void hal_clint_mtimecmp_set(os_cpu_t hart, uint64_t value) {
    volatile uint64_t *mtimecmp = (volatile uint64_t *)(g_clint_base + CLINT_MTIMECMP_OFFSET + hart * 8);
    *mtimecmp = value;
}

void hal_clint_ipi_send(os_cpu_t hart) {
    volatile uint32_t *msip = (volatile uint32_t *)(g_clint_base + CLINT_MSIP_OFFSET + hart * 4);
    *msip = 1;
}

void hal_clint_ipi_clear(os_cpu_t hart) {
    volatile uint32_t *msip = (volatile uint32_t *)(g_clint_base + CLINT_MSIP_OFFSET + hart * 4);
    *msip = 0;
}
