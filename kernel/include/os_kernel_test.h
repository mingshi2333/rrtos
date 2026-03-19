#ifndef OS_KERNEL_TEST_H
#define OS_KERNEL_TEST_H

#include <stdbool.h>

#include "os_kernel.h"

#ifdef OS_TEST_HARNESS
typedef struct os_test_trace {
    os_tick_t tick_count;
    uint32_t tick_advances;
    uint32_t reschedule_requests;
} os_test_trace_t;

void os_test_kernel_reset(void);
void os_test_tick_advance(os_tick_t ticks);
void os_test_trigger_reschedule(void);
void os_test_bind_current_task(os_tcb_t *task);
bool os_test_task_in_ready_queue(const os_tcb_t *task);
uint32_t os_test_ready_count(os_prio_t prio);
const os_test_trace_t *os_test_trace_snapshot(void);
uint64_t os_test_cycle_get(void);
const char *os_test_cycle_source(void);
#endif

#endif
