#include "os_kernel_test.h"
#include "support/test_framework.h"

static uint32_t g_timer_fire_count;

static void timer_cb(void *arg) {
    uint32_t *counter = (uint32_t *)arg;
    (*counter)++;
    g_timer_fire_count++;
}

static int test_timer_one_shot_expiry(void) {
    os_timer_t timer;
    uint32_t cb_count = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_timer_init(&timer, "oneshot", timer_cb, &cb_count, 3, 0) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);
    OS_TEST_ASSERT(timer.active == 1);

    os_test_tick_advance(2);
    OS_TEST_ASSERT(cb_count == 0);
    OS_TEST_ASSERT(timer.active == 1);

    os_test_tick_advance(1);
    OS_TEST_ASSERT(cb_count == 1);
    OS_TEST_ASSERT(g_timer_fire_count == 1);
    OS_TEST_ASSERT(timer.active == 0);

    return 0;
}

static int test_timer_periodic_rearm(void) {
    os_timer_t timer;
    uint32_t cb_count = 0;

    os_test_kernel_reset();
    g_timer_fire_count = 0;

    OS_TEST_ASSERT(os_timer_init(&timer, "periodic", timer_cb, &cb_count, 2, 1) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);

    os_test_tick_advance(5);

    OS_TEST_ASSERT(cb_count == 2);
    OS_TEST_ASSERT(g_timer_fire_count == 2);
    OS_TEST_ASSERT(timer.active == 1);

    return 0;
}

static int test_timer_long_delay_boundary(void) {
    os_timer_t timer;
    uint32_t cb_count = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_timer_init(&timer, "long-delay", timer_cb, &cb_count, 260, 0) == OS_EOK);
    OS_TEST_ASSERT(os_timer_start(&timer) == OS_EOK);

    os_test_tick_advance(259);
    OS_TEST_ASSERT(cb_count == 0);
    OS_TEST_ASSERT(timer.active == 1);

    os_test_tick_advance(1);
    OS_TEST_ASSERT(cb_count == 1);
    OS_TEST_ASSERT(timer.active == 0);

    return 0;
}

int os_test_suite_timer_semantics(void) {
    g_timer_fire_count = 0;
    OS_TEST_ASSERT(test_timer_one_shot_expiry() == 0);
    OS_TEST_ASSERT(test_timer_periodic_rearm() == 0);
    OS_TEST_ASSERT(test_timer_long_delay_boundary() == 0);

    return 0;
}
