#define _POSIX_C_SOURCE 200809L

#include "os_kernel_test.h"
#include "hal_clint.h"

#include <string.h>
#include <time.h>

os_reg_t os_test_fake_mstatus;
os_reg_t os_test_fake_mie;
os_cpu_t os_test_fake_cpu_id;

static os_test_trace_t g_trace;
static uint64_t g_fake_mtime;
static uint64_t g_fake_mtimecmp;
static uint64_t g_cycle_fallback;
uint8_t os_test_heap[64 * 1024];
uint8_t *_heap_start = os_test_heap;
uint8_t *_heap_end = os_test_heap + sizeof(os_test_heap);

void os_context_switch(void **from_sp, void **to_sp) {
    (void)from_sp;
    (void)to_sp;
}

void os_context_switch_first(void **to_sp) {
    (void)to_sp;
}

void hal_clint_init(os_ubase_t base) {
    (void)base;
}

uint64_t hal_clint_mtime_get(void) {
    return g_fake_mtime;
}

void hal_clint_mtimecmp_set(os_cpu_t hart, uint64_t value) {
    (void)hart;
    g_fake_mtimecmp = value;
}

void hal_clint_ipi_send(os_cpu_t hart) {
    (void)hart;
}

void hal_clint_ipi_clear(os_cpu_t hart) {
    (void)hart;
}

void os_print(const char *fmt, ...) {
    (void)fmt;
}

void os_test_kernel_reset(void) {
    memset(&g_trace, 0, sizeof(g_trace));
    os_test_fake_mstatus = 0;
    os_test_fake_mie = 0;
    os_test_fake_cpu_id = 0;
    g_fake_mtime = 0;
    g_fake_mtimecmp = 0;
    g_cycle_fallback = 0;
    os_kernel_init();
    g_trace.tick_count = os_tick_get();
}

void os_test_tick_advance(os_tick_t ticks) {
    for (os_tick_t i = 0; i < ticks; i++) {
        g_fake_mtime++;
        g_trace.tick_advances++;
        os_tick_handler();
    }
    g_trace.tick_count = os_tick_get();
}

void os_test_trigger_reschedule(void) {
    g_trace.reschedule_requests++;
    os_sched();
}

const os_test_trace_t *os_test_trace_snapshot(void) {
    g_trace.tick_count = os_tick_get();
    return &g_trace;
}

uint64_t os_test_cycle_get(void) {
#if defined(CLOCK_MONOTONIC_RAW) || defined(CLOCK_MONOTONIC)
    struct timespec ts;
#ifdef CLOCK_MONOTONIC_RAW
    const clockid_t clock_id = CLOCK_MONOTONIC_RAW;
#else
    const clockid_t clock_id = CLOCK_MONOTONIC;
#endif

    if (clock_gettime(clock_id, &ts) == 0) {
        return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
    }
#endif

    g_cycle_fallback++;
    return g_cycle_fallback;
}

const char *os_test_cycle_source(void) {
#ifdef CLOCK_MONOTONIC_RAW
    return "host-monotonic-raw-ns-proxy";
#else
    return "host-monotonic-ns-proxy";
#endif
}
