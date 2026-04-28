#include "os_mem.h"
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

int os_test_suite_mem_semantics(void) {
    OS_TEST_ASSERT(test_calloc_rejects_multiplication_overflow() == 0);
    OS_TEST_ASSERT(test_memory_pool_rejects_invalid_and_double_free() == 0);

    return 0;
}
