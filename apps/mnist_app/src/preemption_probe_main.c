#include <stdint.h>
#include <stdio.h>

#include "hal_board.h"
#include "os_kernel.h"

static os_tcb_t g_probe_high_tcb;
static os_tcb_t g_probe_low_tcb;
static uint64_t g_probe_high_stack[1024];
static uint64_t g_probe_low_stack[1024];

static volatile uint32_t g_probe_low_counter;
static volatile uint32_t g_probe_low_started;

static void probe_low_task(void *arg) {
    (void)arg;

    g_probe_low_started = 1;
    printf("PREEMPT_LOW_READY tick=%u\n", (unsigned)os_tick_get());

    while (1) {
        g_probe_low_counter++;
    }
}

static void probe_high_task(void *arg) {
    uint32_t counter_after_delay;
    os_tick_t wake_tick;

    (void)arg;

    printf("PREEMPT_HIGH_START tick=%u\n", (unsigned)os_tick_get());
    os_task_delay(3);

    wake_tick = os_tick_get();
    counter_after_delay = g_probe_low_counter;

    if (!g_probe_low_started || counter_after_delay == 0 || wake_tick < 3) {
        printf("PREEMPT_FAIL tick=%u low_started=%u counter=%u\n",
               (unsigned)wake_tick,
               (unsigned)g_probe_low_started,
               (unsigned)counter_after_delay);
    } else {
        printf("PREEMPT_PASS tick=%u counter=%u\n",
               (unsigned)wake_tick,
               (unsigned)counter_after_delay);
    }

    os_task_delete(NULL);

    while (1) {
    }
}

void os_kernel_main(void) {
    hal_board_init();

    printf("Booting preemption integration probe...\n");

    os_kernel_init();
    os_task_create(&g_probe_high_tcb, "probe_high", probe_high_task, NULL,
                   8, g_probe_high_stack, sizeof(g_probe_high_stack));
    os_task_create(&g_probe_low_tcb, "probe_low", probe_low_task, NULL,
                   12, g_probe_low_stack, sizeof(g_probe_low_stack));
    os_kernel_start();
}
