#include "hal_clint.h"

static volatile os_ubase_t g_clint_base;

void hal_clint_init(os_ubase_t base) {
    g_clint_base = base;
}

uint64_t hal_clint_mtime_get(void) {
    volatile uint32_t *mtime = (volatile uint32_t *)(g_clint_base + CLINT_MTIME_OFFSET);
    uint32_t hi;
    uint32_t lo;

    do {
        hi = mtime[1];
        lo = mtime[0];
    } while (hi != mtime[1]);

    return ((uint64_t)hi << 32) | lo;
}

void hal_clint_mtimecmp_set(os_cpu_t hart, uint64_t value) {
    volatile uint32_t *mtimecmp =
        (volatile uint32_t *)(g_clint_base + CLINT_MTIMECMP_OFFSET + hart * 8);

    mtimecmp[0] = 0xFFFFFFFFu;
    mtimecmp[1] = (uint32_t)(value >> 32);
    mtimecmp[0] = (uint32_t)value;
}

void hal_clint_ipi_send(os_cpu_t hart) {
    volatile uint32_t *msip = (volatile uint32_t *)(g_clint_base + CLINT_MSIP_OFFSET + hart * 4);
    *msip = 1;
}

void hal_clint_ipi_clear(os_cpu_t hart) {
    volatile uint32_t *msip = (volatile uint32_t *)(g_clint_base + CLINT_MSIP_OFFSET + hart * 4);
    *msip = 0;
}
