#include "os_kernel_test.h"
#include "support/test_framework.h"

int os_test_suite_canary(void) {
    const os_test_trace_t *trace;

    os_test_kernel_reset();

    trace = os_test_trace_snapshot();
    OS_TEST_ASSERT(trace != NULL);
    OS_TEST_ASSERT(trace->tick_count == 0);
    OS_TEST_ASSERT(trace->tick_advances == 0);
    OS_TEST_ASSERT(trace->reschedule_requests == 0);

    os_test_tick_advance(3);

    trace = os_test_trace_snapshot();
    OS_TEST_ASSERT(trace->tick_count == 3);
    OS_TEST_ASSERT(trace->tick_advances == 3);

    os_test_trigger_reschedule();

    trace = os_test_trace_snapshot();
    OS_TEST_ASSERT(trace->reschedule_requests == 1);

    return 0;
}
