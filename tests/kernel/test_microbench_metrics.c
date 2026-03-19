#include "os_kernel_test.h"
#include "riscv_csr.h"
#include "support/test_framework.h"

#include <inttypes.h>
#include <stdio.h>

#define OS_MICROBENCH_SAMPLES 64U
#define OS_MICROBENCH_WARMUP_SAMPLES 8U

typedef struct os_microbench_stats {
    uint64_t min;
    uint64_t median;
    uint64_t p95;
    uint64_t max;
    uint64_t stddev;
} os_microbench_stats_t;

static void noop_task(void *arg) {
    (void)arg;
}

static void print_metric(const char *name, uint64_t value, const char *units,
                         const char *source, const char *scenario,
                         uint32_t samples, const char *aggregation) {
    printf(
        "MICROBENCH_METRIC name=%s value=%llu units=%s source=%s scenario=%s samples=%u aggregation=%s\n",
        name, (unsigned long long)value, units, source, scenario, samples,
        aggregation);
}

static void sort_samples(uint64_t *samples, uint32_t count) {
    for (uint32_t i = 1; i < count; i++) {
        uint64_t value = samples[i];
        uint32_t j = i;

        while (j > 0 && samples[j - 1] > value) {
            samples[j] = samples[j - 1];
            j--;
        }

        samples[j] = value;
    }
}

static uint64_t isqrt_u128(unsigned __int128 value) {
    unsigned __int128 bit = (unsigned __int128)1 << 126;
    unsigned __int128 result = 0;

    while (bit > value) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (value >= result + bit) {
            value -= result + bit;
            result = (result >> 1) + bit;
        } else {
            result >>= 1;
        }
        bit >>= 2;
    }

    return (uint64_t)result;
}

static os_microbench_stats_t sample_stats(uint64_t (*fn)(void)) {
    uint64_t samples[OS_MICROBENCH_SAMPLES] = {0};
    uint64_t sorted[OS_MICROBENCH_SAMPLES] = {0};
    unsigned __int128 total = 0;
    os_microbench_stats_t stats = {0};
    uint32_t p95_rank;

    for (uint32_t i = 0; i < OS_MICROBENCH_WARMUP_SAMPLES; i++) {
        (void)fn();
    }

    for (uint32_t i = 0; i < OS_MICROBENCH_SAMPLES; i++) {
        samples[i] = fn();
        sorted[i] = samples[i];
        total += samples[i];
    }

    sort_samples(sorted, OS_MICROBENCH_SAMPLES);
    stats.min = sorted[0];
    stats.median =
        (sorted[(OS_MICROBENCH_SAMPLES / 2) - 1U] + sorted[OS_MICROBENCH_SAMPLES / 2]) /
        2U;
    p95_rank = ((95U * OS_MICROBENCH_SAMPLES) + 99U) / 100U;
    if (p95_rank == 0U) {
        p95_rank = 1U;
    }
    stats.p95 = sorted[p95_rank - 1U];
    stats.max = sorted[OS_MICROBENCH_SAMPLES - 1U];

    {
        uint64_t mean = (uint64_t)(total / OS_MICROBENCH_SAMPLES);
        unsigned __int128 variance_total = 0;

        for (uint32_t i = 0; i < OS_MICROBENCH_SAMPLES; i++) {
            uint64_t diff = (samples[i] > mean) ? (samples[i] - mean) : (mean - samples[i]);
            variance_total += (unsigned __int128)diff * (unsigned __int128)diff;
        }

        stats.stddev = isqrt_u128(variance_total / OS_MICROBENCH_SAMPLES);
    }

    return stats;
}

static void print_stats(const char *name, const char *units, const char *source,
                        const char *scenario, const os_microbench_stats_t *stats) {
    print_metric(name, stats->min, units, source, scenario, OS_MICROBENCH_SAMPLES,
                 "min");
    print_metric(name, stats->median, units, source, scenario,
                 OS_MICROBENCH_SAMPLES, "median");
    print_metric(name, stats->p95, units, source, scenario, OS_MICROBENCH_SAMPLES,
                 "p95");
    print_metric(name, stats->max, units, source, scenario, OS_MICROBENCH_SAMPLES,
                 "max");
    print_metric(name, stats->stddev, units, source, scenario,
                 OS_MICROBENCH_SAMPLES, "stddev");
}

static uint64_t measure_queue_send_to_blocked_recv_handoff_once(void) {
    os_queue_t queue;
    os_tcb_t receiver = {0};
    os_tcb_t sender = {0};
    uint8_t receiver_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t sender_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint32_t storage[2] = {0};
    uint32_t received = 0;
    uint32_t message = 0x12345678u;
    uint64_t begin;
    uint64_t end;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_queue_init(&queue, "bench-queue", storage,
                                 sizeof(uint32_t), 2) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&receiver, "bench-receiver", noop_task, NULL, 4,
                                  receiver_stack, sizeof(receiver_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&sender, "bench-sender", noop_task, NULL, 5,
                                  sender_stack, sizeof(sender_stack)) == OS_EOK);

    os_test_bind_current_task(&receiver);
    (void)os_queue_recv(&queue, &received, 5);
    OS_TEST_ASSERT(receiver.state == OS_TASK_BLOCKED);

    os_test_bind_current_task(&sender);
    begin = os_cycle_get();
    OS_TEST_ASSERT(os_queue_send(&queue, &message, OS_NO_WAIT) == OS_EOK);
    end = os_cycle_get();

    OS_TEST_ASSERT(received == message);
    OS_TEST_ASSERT(receiver.state == OS_TASK_READY);
    return end - begin;
}

static uint64_t measure_mutex_unlock_to_waiter_handoff_once(void) {
    os_mutex_t mutex;
    os_tcb_t owner = {0};
    os_tcb_t waiter = {0};
    uint8_t owner_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t waiter_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint64_t begin;
    uint64_t end;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_mutex_init(&mutex, "bench-mutex") == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&owner, "bench-owner", noop_task, NULL, 5,
                                  owner_stack, sizeof(owner_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&waiter, "bench-waiter", noop_task, NULL, 3,
                                  waiter_stack, sizeof(waiter_stack)) == OS_EOK);

    os_test_bind_current_task(&owner);
    OS_TEST_ASSERT(os_mutex_lock(&mutex, OS_NO_WAIT) == OS_EOK);

    os_test_bind_current_task(&waiter);
    (void)os_mutex_lock(&mutex, 5);
    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);

    os_test_bind_current_task(&owner);
    begin = os_cycle_get();
    OS_TEST_ASSERT(os_mutex_unlock(&mutex) == OS_EOK);
    end = os_cycle_get();

    OS_TEST_ASSERT(mutex.owner == &waiter);
    OS_TEST_ASSERT(waiter.state == OS_TASK_READY);
    return end - begin;
}

static uint64_t measure_sem_give_to_blocked_waiter_handoff_once(void) {
    os_sem_t sem;
    os_tcb_t waiter = {0};
    os_tcb_t giver = {0};
    uint8_t waiter_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint8_t giver_stack[OS_CFG_STACK_SIZE_MIN] = {0};
    uint64_t begin;
    uint64_t end;

    os_test_kernel_reset();

    OS_TEST_ASSERT(os_sem_init(&sem, "bench-sem", 0, 1) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&waiter, "bench-sem-waiter", noop_task, NULL, 3,
                                  waiter_stack, sizeof(waiter_stack)) == OS_EOK);
    OS_TEST_ASSERT(os_task_create(&giver, "bench-sem-giver", noop_task, NULL, 5,
                                  giver_stack, sizeof(giver_stack)) == OS_EOK);

    os_test_bind_current_task(&waiter);
    (void)os_sem_take(&sem, 5);
    OS_TEST_ASSERT(waiter.state == OS_TASK_BLOCKED);

    os_test_bind_current_task(&giver);
    begin = os_cycle_get();
    OS_TEST_ASSERT(os_sem_give(&sem) == OS_EOK);
    end = os_cycle_get();

    OS_TEST_ASSERT(waiter.state == OS_TASK_READY);
    return end - begin;
}

int os_test_suite_microbench_metrics(void) {
    const char *source = os_test_cycle_source();
    const char *units = "host-monotonic-ns";
    os_microbench_stats_t queue_stats =
        sample_stats(measure_queue_send_to_blocked_recv_handoff_once);
    os_microbench_stats_t mutex_stats =
        sample_stats(measure_mutex_unlock_to_waiter_handoff_once);
    os_microbench_stats_t sem_stats =
        sample_stats(measure_sem_give_to_blocked_waiter_handoff_once);

    print_stats("queue_send_to_blocked_recv_handoff", units, source,
                "queue-send-to-blocked-recv-handoff", &queue_stats);
    print_stats("mutex_unlock_to_waiter_handoff", units, source,
                "mutex-unlock-to-waiter-handoff", &mutex_stats);
    print_stats("sem_give_to_blocked_waiter_handoff", units, source,
                "sem-give-to-blocked-waiter-handoff", &sem_stats);
    return 0;
}
