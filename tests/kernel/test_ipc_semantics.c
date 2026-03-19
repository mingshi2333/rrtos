#include "os_kernel_test.h"
#include "support/test_framework.h"

static void noop_task(void *arg) {
    (void)arg;
}

static int test_event_wait_any_clear_on_exit_immediate(void) {
    os_event_t event;
    uint32_t actual = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_event_init(&event, "event-any") == OS_EOK);
    OS_TEST_ASSERT(os_event_set(&event, 0x6u) == OS_EOK);
    OS_TEST_ASSERT(os_event_wait(&event, 0x2u, OS_EVENT_WAIT_ANY | OS_EVENT_CLEAR_ON_EXIT,
                                 OS_NO_WAIT, &actual) == OS_EOK);
    OS_TEST_ASSERT(actual == 0x2u);
    OS_TEST_ASSERT(event.flags == 0x4u);

    return 0;
}

static int test_event_wait_all_blocks_then_wakes(void) {
    os_event_t event;
    os_tcb_t waiter = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t actual = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_event_init(&event, "event-all") == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&waiter, "event-waiter", noop_task, NULL, 4, stack, sizeof(stack)) == OS_EOK);
    os_test_bind_current_task(&waiter);

    (void)os_event_wait(&event, 0x3u, OS_EVENT_WAIT_ALL | OS_EVENT_CLEAR_ON_EXIT, 5, &actual);

    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(waiter.pending_type == OS_PENDING_EVENT);
    OS_TEST_ASSERT(waiter.pending_ipc == &event);
    OS_TEST_ASSERT(event.wait_list.count == 1);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&waiter));

    OS_TEST_ASSERT(os_event_set(&event, 0x1u) == OS_EOK);
    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(event.wait_list.count == 1);

    OS_TEST_ASSERT(os_event_set(&event, 0x2u) == OS_EOK);
    OS_TEST_ASSERT(waiter.pending_result == OS_EOK);
    OS_TEST_ASSERT(waiter.pending_ipc == NULL);
    OS_TEST_ASSERT(waiter.state == OS_TASK_READY);
    OS_TEST_ASSERT(actual == 0x3u);
    OS_TEST_ASSERT(event.flags == 0u);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&waiter));

    return 0;
}

static int test_semaphore_ready_path(void) {
    os_sem_t sem;
    os_tcb_t waiter = {0};
    uint8_t stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_sem_init(&sem, "sem-red", 0, 1) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&waiter, "waiter", noop_task, NULL, 3, stack, sizeof(stack)) == OS_EOK);
    os_test_bind_current_task(&waiter);

    (void)os_sem_take(&sem, 5);

    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(sem.wait_list.count == 1);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&waiter));

    OS_TEST_ASSERT(os_sem_give(&sem) == OS_EOK);
    OS_TEST_ASSERT(waiter.pending_result == OS_EOK);
    OS_TEST_ASSERT(waiter.state == OS_TASK_READY);
    OS_TEST_ASSERT(waiter.pending_ipc == NULL);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&waiter));
    return 0;
}

static int test_mutex_unlock_ready_handoff(void) {
    os_mutex_t mutex;
    os_tcb_t owner = {0};
    os_tcb_t waiter = {0};
    uint8_t owner_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t waiter_stack[OS_CFG_STACK_SIZE_MIN] = {0};

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_mutex_init(&mutex, "mutex-green") == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&owner, "owner", noop_task, NULL, 5, owner_stack, sizeof(owner_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&waiter, "waiter", noop_task, NULL, 3, waiter_stack, sizeof(waiter_stack)) == OS_EOK);

    os_test_bind_current_task(&owner);
    OS_TEST_ASSERT(os_mutex_lock(&mutex, OS_NO_WAIT) == OS_EOK);

    os_test_bind_current_task(&waiter);
    (void)os_mutex_lock(&mutex, 5);

    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(mutex.wait_list.count == 1);
    OS_TEST_ASSERT(owner.priority == waiter.priority);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&waiter));

    os_test_bind_current_task(&owner);
    OS_TEST_ASSERT(os_mutex_unlock(&mutex) == OS_EOK);

    OS_TEST_ASSERT(owner.priority == owner.base_prio);
    OS_TEST_ASSERT(mutex.owner == &waiter);
    OS_TEST_ASSERT(waiter.pending_result == OS_EOK);
    OS_TEST_ASSERT(waiter.pending_ipc == NULL);
    OS_TEST_ASSERT(waiter.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&waiter));
    return 0;
}

static int test_queue_recv_wake_path(void) {
    os_queue_t queue;
    os_tcb_t receiver = {0};
    os_tcb_t sender = {0};
    uint8_t receiver_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t sender_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t storage[2] = {0};
    uint32_t received = 0;
    uint32_t message = 0x12345678u;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_queue_init(&queue, "queue-recv", storage, sizeof(uint32_t), 2) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&receiver, "receiver", noop_task, NULL, 4, receiver_stack, sizeof(receiver_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&sender, "sender", noop_task, NULL, 5, sender_stack, sizeof(sender_stack)) == OS_EOK);

    os_test_bind_current_task(&receiver);
    (void)os_queue_recv(&queue, &received, 5);

    OS_TEST_ASSERT(receiver.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(queue.recv_wait.count == 1);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&receiver));

    os_test_bind_current_task(&sender);
    OS_TEST_ASSERT(os_queue_send(&queue, &message, OS_NO_WAIT) == OS_EOK);

    OS_TEST_ASSERT(received == message);
    OS_TEST_ASSERT(receiver.pending_result == OS_EOK);
    OS_TEST_ASSERT(receiver.pending_ipc == NULL);
    OS_TEST_ASSERT(receiver.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&receiver));
    return 0;
}

static int test_queue_send_block_and_timeout(void) {
    os_queue_t queue;
    os_tcb_t sender = {0};
    os_tcb_t receiver = {0};
    uint8_t sender_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t receiver_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t storage[1] = {0};
    uint32_t initial = 0xAA55AA55u;
    uint32_t pending = 0xCAFEBABEu;
    uint32_t first = 0;
    uint32_t second = 0;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_queue_init(&queue, "queue-send", storage, sizeof(uint32_t), 1) == OS_EOK);
    OS_TEST_ASSERT(os_queue_send(&queue, &initial, OS_NO_WAIT) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&sender, "sender", noop_task, NULL, 4, sender_stack, sizeof(sender_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&receiver, "receiver", noop_task, NULL, 5, receiver_stack, sizeof(receiver_stack)) == OS_EOK);

    os_test_bind_current_task(&sender);
    (void)os_queue_send(&queue, &pending, 3);

    OS_TEST_ASSERT(sender.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(queue.send_wait.count == 1);
    OS_TEST_ASSERT(!os_test_task_in_ready_queue(&sender));

    os_test_bind_current_task(&receiver);
    OS_TEST_ASSERT(os_queue_recv(&queue, &first, OS_NO_WAIT) == OS_EOK);

    OS_TEST_ASSERT(first == initial);
    OS_TEST_ASSERT(sender.pending_result == OS_EOK);
    OS_TEST_ASSERT(sender.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&sender));
    OS_TEST_ASSERT(os_queue_recv(&queue, &second, OS_NO_WAIT) == OS_EOK);
    OS_TEST_ASSERT(second == pending);

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_queue_init(&queue, "queue-send-timeout", storage, sizeof(uint32_t), 1) == OS_EOK);
    OS_TEST_ASSERT(os_queue_send(&queue, &initial, OS_NO_WAIT) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&sender, "sender-timeout", noop_task, NULL, 4, sender_stack, sizeof(sender_stack)) == OS_EOK);

    os_test_bind_current_task(&sender);
    (void)os_queue_send(&queue, &pending, 2);

    OS_TEST_ASSERT(sender.state == OS_TASK_BLOCKED);
    OS_TEST_ASSERT(queue.send_wait.count == 1);

    os_test_tick_advance(2);

    OS_TEST_ASSERT(queue.send_wait.count == 0);
    OS_TEST_ASSERT(sender.pending_result == OS_ETIMEOUT);
    OS_TEST_ASSERT(sender.pending_ipc == NULL);
    OS_TEST_ASSERT(sender.state == OS_TASK_READY);
    OS_TEST_ASSERT(os_test_task_in_ready_queue(&sender));
    return 0;
}

int os_test_suite_ipc_semantics(void) {
    OS_TEST_ASSERT(test_semaphore_ready_path() == 0);
    OS_TEST_ASSERT(test_mutex_unlock_ready_handoff() == 0);
    OS_TEST_ASSERT(test_queue_recv_wake_path() == 0);
    OS_TEST_ASSERT(test_queue_send_block_and_timeout() == 0);
    OS_TEST_ASSERT(test_event_wait_any_clear_on_exit_immediate() == 0);
    OS_TEST_ASSERT(test_event_wait_all_blocks_then_wakes() == 0);

    return 0;
}
