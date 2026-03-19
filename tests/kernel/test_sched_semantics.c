#include "os_kernel_test.h"
#include "support/test_framework.h"

static void noop_task(void *arg) {
    (void)arg;
}

static int test_task_set_prio_updates_ready_queue(void) {
    os_tcb_t task = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&task, "prio-task", noop_task, NULL, 6, stack, sizeof(stack)) == OS_EOK);
    OS_TEST_ASSERT(os_test_ready_count(6) == 1);
    OS_TEST_ASSERT(os_task_set_prio(&task, 3) == OS_EOK);
    OS_TEST_ASSERT(task.priority == 3);
    OS_TEST_ASSERT(task.base_prio == 3);
    OS_TEST_ASSERT(task.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_ready_count(6) == 0);
    OS_TEST_ASSERT(os_test_ready_count(3) == 1);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&task));

    return 0;
}

int os_test_suite_sched_semantics(void) {
    os_tcb_t task = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&task, "sched-red", noop_task, NULL, 4, stack, sizeof(stack)) == OS_EOK);
    os_test_bind_current_task(&task);

    OS_TEST_ASSERT(os_task_delay(2) == OS_EOK);
    OS_TEST_ASSERT(task.state == OS_TASK_SLEEPING);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&task));

    os_test_tick_advance(1);
    OS_TEST_ASSERT(task.state == OS_TASK_SLEEPING);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&task));

    os_test_tick_advance(1);
    OS_TEST_ASSERT(task.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&task));
    OS_TEST_ASSERT(test_task_set_prio_updates_ready_queue() == 0);

    return 0;
}
