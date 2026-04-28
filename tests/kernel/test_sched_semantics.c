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

static int test_task_create_seeds_entry_argument_in_initial_context(void) {
    os_tcb_t task = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uintptr_t expected_arg = 0x12345678UL;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&task, "arg-task", noop_task, (void *)expected_arg,
                                  4, stack, sizeof(stack)) == OS_EOK);

    os_reg_t *frame = (os_reg_t *)task.sp;
    OS_TEST_ASSERT(frame[14] == (os_reg_t)expected_arg);

    return 0;
}

static int test_time_slice_rotation_preserves_ready_peer(void) {
    os_tcb_t current = {0};
    os_tcb_t peer = {0};
    uint8_t current_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t peer_stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&current, "slice-current", noop_task, NULL,
                                  4, current_stack, sizeof(current_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&peer, "slice-peer", noop_task, NULL,
                                  4, peer_stack, sizeof(peer_stack)) == OS_EOK);
    os_test_bind_current_task(&current);
    current.time_slice = 1;

    OS_TEST_ASSERT(os_test_ready_count(4) == 1);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&peer));

    os_test_tick_advance(1);

    OS_TEST_ASSERT(os_test_ready_count(4) == 2);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&peer));
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&current));

    return 0;
}

static int test_scheduler_defers_while_irq_nested(void) {
    os_tcb_t low = {0};
    os_tcb_t high = {0};
    uint8_t low_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t high_stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&low, "irq-low", noop_task, NULL,
                                  5, low_stack, sizeof(low_stack)) == OS_EOK);
    os_kernel_start();
    os_test_bind_current_task(&low);
    OS_TEST_ASSERT(os_task_self() == &low);

    os_irq_enter();
    OS_TEST_ASSERT(os_task_create(&high, "irq-high", noop_task, NULL,
                                  3, high_stack, sizeof(high_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_self() == &low);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&high));

    os_irq_exit();
    OS_TEST_ASSERT(os_task_self() == &high);
    OS_TEST_ASSERT(high.state == OS_TASK_RUNNING);
    OS_TEST_ASSERT(low.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&low));

    return 0;
}

static int test_task_yield_without_peer_keeps_current_running_out_of_ready_queue(void) {
    os_tcb_t task = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&task, "yield-solo", noop_task, NULL,
                                  4, stack, sizeof(stack)) == OS_EOK);
    os_kernel_start();
    os_test_bind_current_task(&task);

    OS_TEST_ASSERT(os_task_self() == &task);
    OS_TEST_ASSERT(task.state == OS_TASK_RUNNING);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&task));

    os_task_yield();

    OS_TEST_ASSERT(os_task_self() == &task);
    OS_TEST_ASSERT(task.state == OS_TASK_RUNNING);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&task));

    return 0;
}

static void corrupt_stack_guard(uint32_t *stack) {
    stack[0] = (uint32_t)~(uint32_t)OS_CFG_STACK_GUARD_PATTERN;
}

static int test_scheduler_marks_current_task_failed_when_stack_guard_is_corrupt(void) {
    os_tcb_t low = {0};
    os_tcb_t high = {0};
    uint32_t low_stack[OS_CFG_STACK_SIZE_MIN / sizeof(uint32_t)] = {0};
    uint32_t high_stack[OS_CFG_STACK_SIZE_MIN / sizeof(uint32_t)] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&low, "stack-low", noop_task, NULL,
                                  5, low_stack, sizeof(low_stack)) == OS_EOK);
    os_kernel_start();
    os_test_bind_current_task(&low);

    os_sched_lock();
    OS_TEST_ASSERT(os_task_create(&high, "stack-high", noop_task, NULL,
                                  3, high_stack, sizeof(high_stack)) == OS_EOK);
    corrupt_stack_guard(low_stack);
    os_sched_unlock();

    OS_TEST_ASSERT(low.state == OS_TASK_TERMINATED);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&low));
    OS_TEST_ASSERT(os_task_self() == &high);
    OS_TEST_ASSERT(high.state == OS_TASK_RUNNING);

    return 0;
}

static int test_scheduler_skips_ready_task_when_stack_guard_is_corrupt(void) {
    os_tcb_t low = {0};
    os_tcb_t high = {0};
    uint32_t low_stack[OS_CFG_STACK_SIZE_MIN / sizeof(uint32_t)] = {0};
    uint32_t high_stack[OS_CFG_STACK_SIZE_MIN / sizeof(uint32_t)] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_task_create(&low, "stack-ready-low", noop_task, NULL,
                                  5, low_stack, sizeof(low_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&high, "stack-ready-high", noop_task, NULL,
                                  1, high_stack, sizeof(high_stack)) == OS_EOK);
    corrupt_stack_guard(high_stack);

    os_kernel_start();

    OS_TEST_ASSERT(high.state == OS_TASK_TERMINATED);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&high));
    OS_TEST_ASSERT(os_task_self() != &high);

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
    OS_TEST_ASSERT(test_task_create_seeds_entry_argument_in_initial_context() == 0);
    OS_TEST_ASSERT(test_time_slice_rotation_preserves_ready_peer() == 0);
    OS_TEST_ASSERT(test_scheduler_defers_while_irq_nested() == 0);
    OS_TEST_ASSERT(test_task_yield_without_peer_keeps_current_running_out_of_ready_queue() == 0);
    OS_TEST_ASSERT(test_scheduler_marks_current_task_failed_when_stack_guard_is_corrupt() == 0);
    OS_TEST_ASSERT(test_scheduler_skips_ready_task_when_stack_guard_is_corrupt() == 0);

    return 0;
}
