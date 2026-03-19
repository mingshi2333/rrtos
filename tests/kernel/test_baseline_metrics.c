#include "os_kernel_test.h"
#include "support/test_framework.h"

#include <inttypes.h>
#include <stdio.h>

static void noop_task(void *arg) {
    (void)arg;
}

static uint32_t g_timer_callbacks;
static os_tick_t *g_periodic_fire_ticks;

static void timer_cb(void *arg) {
    uint32_t *counter = (uint32_t *)arg;
    (*counter)++;
    g_timer_callbacks++;
}

static void periodic_timer_cb(void *arg) {
    uint32_t *counter = (uint32_t *)arg;

    if (*counter < 3 && g_periodic_fire_ticks != NULL) {
        g_periodic_fire_ticks[*counter] = os_tick_get();
    }

    (*counter)++;
    g_timer_callbacks++;
}

static void print_metric(const char *name, uint32_t value, const char *units,
                         const char *source, const char *scenario) {
    printf(
        "BASELINE_METRIC name=%s value=%" PRIu32 " units=%s source=%s scenario=%s\n",
        name, value, units, source, scenario);
}

static int measure_scheduler_wake_latency(void) {
    os_tcb_t task = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t ticks = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&task, "baseline-sched", noop_task, NULL, 4,
                                  stack, sizeof(stack)) == OS_EOK);
    os_test_bind_current_task(&task);
    OS_TEST_ASSERT(os_task_delay(2) == OS_EOK);

    while (task.state != OS_TASK_READY && ticks < 8) {
        os_test_tick_advance(1);
        ticks++;
    }

    OS_TEST_ASSERT(task.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&task));
    print_metric("scheduler_wake_latency", ticks, "ticks", "semantic-harness",
                 "task-delay-timeout");
    return 0;
}

static int measure_ipc_handoff_latency(void) {
    os_queue_t queue;
    os_tcb_t receiver = {0};
    os_tcb_t sender = {0};
    uint8_t receiver_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t sender_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t storage[2] = {0};
    uint32_t received = 0;
    uint32_t message = 0x12345678u;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_queue_init(&queue, "baseline-queue", storage,
                                 sizeof(uint32_t), 2) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&receiver, "baseline-receiver", noop_task, NULL,
                                  4, receiver_stack, sizeof(receiver_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&sender, "baseline-sender", noop_task, NULL, 5,
                                  sender_stack, sizeof(sender_stack)) == OS_EOK);

    os_test_bind_current_task(&receiver);
    (void)os_queue_recv(&queue, &received, 5);

    OS_TEST_ASSERT(receiver.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&receiver));

    os_test_bind_current_task(&sender);
    OS_TEST_ASSERT(os_queue_send(&queue, &message, OS_NO_WAIT) == OS_EOK);

    OS_TEST_ASSERT(received == message);
    OS_TEST_ASSERT(receiver.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&receiver));
    print_metric("ipc_handoff_latency", 0, "ticks", "semantic-harness",
                 "queue-send-to-blocked-recv");
    return 0;
}

static int measure_timer_latency_and_jitter(void) {
    os_timer_t timer;
    uint32_t callback_count = 0;
    uint32_t ticks = 0;
    uint32_t configured_period = 3;

    os_test_kernel_reset();
    g_timer_callbacks = 0;

    OS_TEST_ASSERT(os_timer_init(&timer, "baseline-timer", timer_cb,
                                 &callback_count, configured_period, 0) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);

    while (callback_count == 0 && ticks < 16) {
        os_test_tick_advance(1);
        ticks++;
    }

    OS_TEST_ASSERT(callback_count == 1);
    OS_TEST_ASSERT(g_timer_callbacks == 1);
    print_metric("timer_latency", ticks, "ticks", "semantic-harness",
                 "one-shot-expiry");
    print_metric("timer_jitter", ticks - configured_period, "ticks",
                 "semantic-harness", "one-shot-expiry");
    return 0;
}

static int measure_timer_periodic_jitter(void) {
    os_timer_t timer;
    os_tick_t fire_ticks[3] = {0};
    uint32_t callback_count = 0;
    uint32_t ticks = 0;
    uint32_t configured_period = 2;
    uint32_t max_jitter = 0;

    os_test_kernel_reset();
    g_timer_callbacks = 0;
    g_periodic_fire_ticks = fire_ticks;

    OS_TEST_ASSERT(os_timer_init(&timer, "baseline-periodic", periodic_timer_cb,
                                 &callback_count, configured_period, 1) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);

    while (callback_count < 3 && ticks < 16) {
        os_test_tick_advance(1);
        ticks++;
    }

    OS_TEST_ASSERT(callback_count == 3);
    OS_TEST_ASSERT(g_timer_callbacks == 3);

    for (uint32_t i = 1; i < 3; i++) {
        uint32_t interval = fire_ticks[i] - fire_ticks[i - 1];
        uint32_t jitter = (interval > configured_period) ? (interval - configured_period)
                                                         : (configured_period - interval);
        if (jitter > max_jitter) {
            max_jitter = jitter;
        }
    }

    OS_TEST_ASSERT(os_timer_stop(&timer) == OS_EOK);
    g_periodic_fire_ticks = NULL;
    print_metric("timer_periodic_jitter", max_jitter, "ticks", "semantic-harness",
                 "periodic-rearm-stability");
    return 0;
}

static int measure_timer_long_delay_boundary(void) {
    os_timer_t timer;
    uint32_t callback_count = 0;
    uint32_t ticks = 0;
    const uint32_t configured_period = 260;

    os_test_kernel_reset();
    g_timer_callbacks = 0;

    OS_TEST_ASSERT(os_timer_init(&timer, "baseline-long-delay", timer_cb,
                                 &callback_count, configured_period, 0) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);

    while (callback_count == 0 && ticks <= configured_period) {
        os_test_tick_advance(1);
        ticks++;
    }

    OS_TEST_ASSERT(callback_count == 1);
    OS_TEST_ASSERT(g_timer_callbacks == 1);
    print_metric("timer_long_delay_boundary", ticks, "ticks", "semantic-harness",
                 "timer-wheel-wrap-boundary");
    return 0;
}

int os_test_suite_baseline_metrics(void) {
    OS_TEST_ASSERT(measure_scheduler_wake_latency() == 0);
    OS_TEST_ASSERT(measure_ipc_handoff_latency() == 0);
    OS_TEST_ASSERT(measure_timer_latency_and_jitter() == 0);
    OS_TEST_ASSERT(measure_timer_periodic_jitter() == 0);
    OS_TEST_ASSERT(measure_timer_long_delay_boundary() == 0);
    return 0;
}
