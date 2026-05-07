#include "os_mem.h"
#include "os_kernel_test.h"
#include "support/test_framework.h"

static int test_calloc_rejects_multiplication_overflow(void) {
    void *ptr = os_calloc((os_size_t)-1, 2);

    OS_TEST_ASSERT(ptr == NULL);
    return 0;
}

static int test_memory_pool_rejects_invalid_and_double_free(void) {
    os_mem_pool_t pool;
    uint8_t storage[4][16] = {0};
    void *first;
    uint8_t foreign[16] = {0};

    OS_TEST_ASSERT(os_mem_pool_init(&pool, "pool", storage, sizeof(storage[0]), 4) == OS_EOK);

    first = os_mem_pool_alloc(&pool);
    OS_TEST_ASSERT(first != NULL);
    OS_TEST_ASSERT(os_mem_pool_free_count(&pool) == 3);

    OS_TEST_ASSERT(os_mem_pool_free(&pool, (uint8_t *)first + 1) == OS_EINVAL);
    OS_TEST_ASSERT(os_mem_pool_free(&pool, foreign) == OS_EINVAL);
    OS_TEST_ASSERT(os_mem_pool_free(&pool, first) == OS_EOK);
    OS_TEST_ASSERT(os_mem_pool_free_count(&pool) == 4);
    OS_TEST_ASSERT(os_mem_pool_free(&pool, first) == OS_EBUSY);
    OS_TEST_ASSERT(os_mem_pool_free_count(&pool) == 4);

    return 0;
}

static int test_heap_stats_track_current_peak_and_reset(void) {
    const os_size_t first_size = OS_ALIGN_UP(13, 8);
    const os_size_t second_size = OS_ALIGN_UP(24, 8);
    os_heap_stats_t stats;
    void *first;
    void *second;

    os_test_kernel_reset();

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == 0);
    OS_TEST_ASSERT(stats.peak_used_bytes == 0);
    OS_TEST_ASSERT(stats.allocation_count == 0);
    OS_TEST_ASSERT(stats.free_count == 0);

    first = os_malloc(13);
    OS_TEST_ASSERT(first != NULL);

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == first_size);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size);
    OS_TEST_ASSERT(stats.allocation_count == 1);
    OS_TEST_ASSERT(stats.free_count == 0);

    os_heap_stats_reset_peak();

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == first_size);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size);

    second = os_malloc(24);
    OS_TEST_ASSERT(second != NULL);

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == first_size + second_size);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size + second_size);
    OS_TEST_ASSERT(stats.allocation_count == 2);
    OS_TEST_ASSERT(stats.free_count == 0);

    os_free(second);

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == first_size);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size + second_size);
    OS_TEST_ASSERT(stats.allocation_count == 2);
    OS_TEST_ASSERT(stats.free_count == 1);

    os_heap_stats_reset_peak();

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == first_size);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size);

    os_free(first);

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == 0);
    OS_TEST_ASSERT(stats.peak_used_bytes == first_size);
    OS_TEST_ASSERT(stats.free_count == 2);

    os_heap_stats_reset_peak();

    stats = os_heap_stats_get();
    OS_TEST_ASSERT(stats.current_used_bytes == 0);
    OS_TEST_ASSERT(stats.peak_used_bytes == 0);

    return 0;
}

static int test_heap_trace_tracks_largest_allocations_and_peak_event(void) {
    os_heap_trace_t trace;
    void *small;
    void *large;
    void *middle;

    os_test_kernel_reset();
    os_heap_trace_reset();

    small = os_malloc(8);
    OS_TEST_ASSERT(small != NULL);
    large = os_malloc(64);
    OS_TEST_ASSERT(large != NULL);
    middle = os_malloc(24);
    OS_TEST_ASSERT(middle != NULL);

    trace = os_heap_trace_get();
    OS_TEST_ASSERT(trace.sequence == 3);
    OS_TEST_ASSERT(trace.largest_allocations[0].size == OS_ALIGN_UP(64, 8));
    OS_TEST_ASSERT(trace.largest_allocations[1].size == OS_ALIGN_UP(24, 8));
    OS_TEST_ASSERT(trace.largest_allocations[2].size == OS_ALIGN_UP(8, 8));
    OS_TEST_ASSERT(trace.largest_live_block.size == OS_ALIGN_UP(64, 8));
    OS_TEST_ASSERT(trace.peak_event.size == OS_ALIGN_UP(24, 8));
    OS_TEST_ASSERT(trace.peak_event.current_used_bytes ==
                   OS_ALIGN_UP(8, 8) + OS_ALIGN_UP(64, 8) + OS_ALIGN_UP(24, 8));

    os_heap_trace_reset();
    trace = os_heap_trace_get();
    OS_TEST_ASSERT(trace.sequence == 0);
    OS_TEST_ASSERT(trace.largest_allocations[0].size == 0);
    OS_TEST_ASSERT(trace.peak_event.current_used_bytes == 0);

    os_free(middle);
    os_free(large);
    os_free(small);

    return 0;
}

int os_test_suite_mem_semantics(void) {
    OS_TEST_ASSERT(test_calloc_rejects_multiplication_overflow() == 0);
    OS_TEST_ASSERT(test_memory_pool_rejects_invalid_and_double_free() == 0);
    OS_TEST_ASSERT(test_heap_stats_track_current_peak_and_reset() == 0);
    OS_TEST_ASSERT(test_heap_trace_tracks_largest_allocations_and_peak_event() == 0);

    return 0;
}
