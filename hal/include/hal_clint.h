#ifndef HAL_CLINT_H
#define HAL_CLINT_H

#include "os_types.h"

#define CLINT_MSIP_OFFSET       0x0000
#define CLINT_MTIMECMP_OFFSET   0x4000
#define CLINT_MTIME_OFFSET      0xBFF8

void hal_clint_init(os_ubase_t base);
uint64_t hal_clint_mtime_get(void);
void hal_clint_mtimecmp_set(os_cpu_t hart, uint64_t value);
void hal_clint_ipi_send(os_cpu_t hart);
void hal_clint_ipi_clear(os_cpu_t hart);

#endif
