#include "os_mem.h"
#include "riscv_atomic.h"

#include <string.h>

static bool os_mem_pool_ptr_valid(const os_mem_pool_t *pool, const void *ptr) {
    uintptr_t start = (uintptr_t)pool->start;
    uintptr_t addr = (uintptr_t)ptr;
    os_size_t total_size;

    if (pool->block_count != 0 &&
        pool->block_size > ((os_size_t)-1) / pool->block_count) {
        return false;
    }

    total_size = pool->block_size * pool->block_count;
    if (addr < start || addr >= start + total_size) {
        return false;
    }

    return ((addr - start) % pool->block_size) == 0;
}

static bool os_mem_pool_free_list_contains(const os_mem_pool_t *pool, const void *ptr) {
    const os_mem_block_t *block = pool->free_list;

    while (block) {
        if ((const void *)block == ptr) {
            return true;
        }
        block = block->next;
    }

    return false;
}

os_err_t os_mem_pool_init(os_mem_pool_t *pool, const char *name,
                          void *start, os_size_t block_size, os_size_t block_count) {
    if (!pool || !start || block_size == 0 || block_count == 0) {
        return OS_EINVAL;
    }
    
    if (block_size < sizeof(os_mem_block_t)) {
        block_size = sizeof(os_mem_block_t);
    }
    block_size = OS_ALIGN_UP(block_size, sizeof(void *));
    if (block_size > ((os_size_t)-1) / block_count) {
        return OS_EINVAL;
    }
    
    pool->start = start;
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->free_count = block_count;
    pool->name = name;
    os_spinlock_init(&pool->lock);
    
#if OS_CFG_STATS_EN
    pool->alloc_count = 0;
    pool->peak_used = 0;
#endif
    
    pool->free_list = (os_mem_block_t *)start;
    os_mem_block_t *block = pool->free_list;
    
    for (os_size_t i = 0; i < block_count - 1; i++) {
        block->next = (os_mem_block_t *)((uint8_t *)block + block_size);
        block = block->next;
    }
    block->next = NULL;
    
    return OS_EOK;
}

void *os_mem_pool_alloc(os_mem_pool_t *pool) {
    if (!pool) {
        return NULL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&pool->lock);
    
    if (!pool->free_list) {
        os_spinlock_irq_restore(&pool->lock, flags);
        return NULL;
    }
    
    os_mem_block_t *block = pool->free_list;
    pool->free_list = block->next;
    pool->free_count--;
    
#if OS_CFG_STATS_EN
    pool->alloc_count++;
    os_size_t used = pool->block_count - pool->free_count;
    if (used > pool->peak_used) {
        pool->peak_used = used;
    }
#endif
    
    os_spinlock_irq_restore(&pool->lock, flags);
    return (void *)block;
}

os_err_t os_mem_pool_free(os_mem_pool_t *pool, void *ptr) {
    if (!pool || !ptr) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&pool->lock);

    if (!os_mem_pool_ptr_valid(pool, ptr)) {
        os_spinlock_irq_restore(&pool->lock, flags);
        return OS_EINVAL;
    }

    if (pool->free_count >= pool->block_count ||
        os_mem_pool_free_list_contains(pool, ptr)) {
        os_spinlock_irq_restore(&pool->lock, flags);
        return OS_EBUSY;
    }
    
    os_mem_block_t *block = (os_mem_block_t *)ptr;
    block->next = pool->free_list;
    pool->free_list = block;
    pool->free_count++;
    
    os_spinlock_irq_restore(&pool->lock, flags);
    return OS_EOK;
}

os_size_t os_mem_pool_free_count(os_mem_pool_t *pool) {
    if (!pool) {
        return 0;
    }
    return pool->free_count;
}

#if OS_CFG_HEAP_EN

typedef struct heap_block {
    os_size_t size;
    uint8_t used;
    struct heap_block *next;
    struct heap_block *prev;
} heap_block_t;

#if defined(OS_TEST_HARNESS)
extern uint8_t os_test_heap[];
extern uint8_t *os_test_heap_end;
#else
extern uint8_t _heap_start[];
extern uint8_t _heap_end[];
#endif

static heap_block_t *g_heap_free_list;
static os_spinlock_t g_heap_lock = OS_SPINLOCK_INIT;
static os_heap_stats_t g_heap_stats;
static os_heap_trace_t g_heap_trace;

static uint8_t *os_heap_start_addr(void) {
#if defined(OS_TEST_HARNESS)
    return os_test_heap;
#else
    return _heap_start;
#endif
}

static uint8_t *os_heap_end_addr(void) {
#if defined(OS_TEST_HARNESS)
    return os_test_heap_end;
#else
    return _heap_end;
#endif
}

static void os_heap_stats_record_alloc(os_size_t size) {
    g_heap_stats.current_used_bytes += size;
    if (g_heap_stats.current_used_bytes > g_heap_stats.peak_used_bytes) {
        g_heap_stats.peak_used_bytes = g_heap_stats.current_used_bytes;
    }
    g_heap_stats.allocation_count++;
}

static void os_heap_stats_record_free(os_size_t size) {
    if (g_heap_stats.current_used_bytes >= size) {
        g_heap_stats.current_used_bytes -= size;
    } else {
        g_heap_stats.current_used_bytes = 0;
    }
    g_heap_stats.free_count++;
}

static void os_heap_trace_insert_largest(const os_heap_trace_record_t *record) {
    size_t insert_at = OS_HEAP_TRACE_TOP_CAPACITY;

    if (record->size == 0) {
        return;
    }

    for (size_t i = 0; i < OS_HEAP_TRACE_TOP_CAPACITY; ++i) {
        if (record->size > g_heap_trace.largest_allocations[i].size) {
            insert_at = i;
            break;
        }
    }

    if (insert_at == OS_HEAP_TRACE_TOP_CAPACITY) {
        return;
    }

    for (size_t i = OS_HEAP_TRACE_TOP_CAPACITY - 1; i > insert_at; --i) {
        g_heap_trace.largest_allocations[i] =
            g_heap_trace.largest_allocations[i - 1];
    }
    g_heap_trace.largest_allocations[insert_at] = *record;
}

static void os_heap_trace_record_alloc(void *ptr, os_size_t size, uintptr_t caller) {
    os_heap_trace_record_t record;

    g_heap_trace.sequence++;
    record.size = size;
    record.current_used_bytes = g_heap_stats.current_used_bytes;
    record.ptr = (uintptr_t)ptr;
    record.caller = caller;
    record.sequence = g_heap_trace.sequence;

    os_heap_trace_insert_largest(&record);

    if (record.size >= g_heap_trace.largest_live_block.size) {
        g_heap_trace.largest_live_block = record;
    }

    if (record.current_used_bytes >= g_heap_trace.peak_event.current_used_bytes) {
        g_heap_trace.peak_event = record;
    }
}

void os_heap_init(void) {
    uint8_t *heap_start = os_heap_start_addr();
    uint8_t *heap_end = os_heap_end_addr();
    uintptr_t aligned_start = OS_ALIGN_UP((uintptr_t)heap_start, 8);
    os_size_t heap_size = (os_size_t)(heap_end - (uint8_t *)aligned_start);

    heap_block_t *first = (heap_block_t *)aligned_start;
    first->size = heap_size - sizeof(heap_block_t);
    first->used = 0;
    first->next = NULL;
    first->prev = NULL;
    g_heap_free_list = first;
    g_heap_stats.current_used_bytes = 0;
    g_heap_stats.peak_used_bytes = 0;
    g_heap_stats.allocation_count = 0;
    g_heap_stats.free_count = 0;
    memset(&g_heap_trace, 0, sizeof(g_heap_trace));
}

static void *os_malloc_with_caller(os_size_t size, uintptr_t caller) {
    if (size == 0) {
        return NULL;
    }
    
    size = OS_ALIGN_UP(size, 8);
    
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);
    
    heap_block_t *block = g_heap_free_list;
    heap_block_t *best = NULL;
    
    while (block) {
        if (!block->used && block->size >= size) {
            if (!best || block->size < best->size) {
                best = block;
                if (block->size == size) break;
            }
        }
        block = block->next;
    }
    
    if (!best) {
        os_spinlock_irq_restore(&g_heap_lock, flags);
        return NULL;
    }
    
    if (best->size > size + sizeof(heap_block_t) + 8) {
        heap_block_t *new_block = (heap_block_t *)((uint8_t *)best + sizeof(heap_block_t) + size);
        new_block->size = best->size - size - sizeof(heap_block_t);
        new_block->used = 0;
        new_block->next = best->next;
        new_block->prev = best;
        if (best->next) {
            best->next->prev = new_block;
        }
        best->next = new_block;
        best->size = size;
    }
    
    best->used = 1;
    os_heap_stats_record_alloc(best->size);
    os_heap_trace_record_alloc((uint8_t *)best + sizeof(heap_block_t),
                               best->size,
                               caller);
    
    os_spinlock_irq_restore(&g_heap_lock, flags);
    return (void *)((uint8_t *)best + sizeof(heap_block_t));
}

void *os_malloc(os_size_t size) {
    return os_malloc_with_caller(size, (uintptr_t)__builtin_return_address(0));
}

void os_free(void *ptr) {
    if (!ptr) {
        return;
    }
    
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);

    if (!block->used) {
        os_spinlock_irq_restore(&g_heap_lock, flags);
        return;
    }
    
    block->used = 0;
    os_heap_stats_record_free(block->size);
    
    if (block->next && !block->next->used) {
        heap_block_t *next = block->next;
        block->size += sizeof(heap_block_t) + next->size;
        block->next = next->next;
        if (next->next) {
            next->next->prev = block;
        }
    }
    
    if (block->prev && !block->prev->used) {
        heap_block_t *prev = block->prev;
        prev->size += sizeof(heap_block_t) + block->size;
        prev->next = block->next;
        if (block->next) {
            block->next->prev = prev;
        }
    }
    
    os_spinlock_irq_restore(&g_heap_lock, flags);
}

void os_heap_trace_reset(void) {
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);

    memset(&g_heap_trace, 0, sizeof(g_heap_trace));

    os_spinlock_irq_restore(&g_heap_lock, flags);
}

os_heap_trace_t os_heap_trace_get(void) {
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);
    os_heap_trace_t trace = g_heap_trace;

    os_spinlock_irq_restore(&g_heap_lock, flags);
    return trace;
}

os_heap_stats_t os_heap_stats_get(void) {
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);
    os_heap_stats_t stats = g_heap_stats;

    os_spinlock_irq_restore(&g_heap_lock, flags);
    return stats;
}

void os_heap_stats_reset_peak(void) {
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);

    g_heap_stats.peak_used_bytes = g_heap_stats.current_used_bytes;

    os_spinlock_irq_restore(&g_heap_lock, flags);
}

void *os_calloc(os_size_t count, os_size_t size) {
    if (count != 0 && size > ((os_size_t)-1) / count) {
        return NULL;
    }

    os_size_t total = count * size;
    void *ptr = os_malloc_with_caller(total, (uintptr_t)__builtin_return_address(0));
    if (ptr) {
        uint8_t *p = (uint8_t *)ptr;
        for (os_size_t i = 0; i < total; i++) {
            p[i] = 0;
        }
    }
    return ptr;
}

void *os_realloc(void *ptr, os_size_t size) {
    if (!ptr) {
        return os_malloc(size);
    }
    if (size == 0) {
        os_free(ptr);
        return NULL;
    }
    
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    if (block->size >= size) {
        return ptr;
    }
    
    void *new_ptr = os_malloc_with_caller(size, (uintptr_t)__builtin_return_address(0));
    if (new_ptr) {
        uint8_t *dst = (uint8_t *)new_ptr;
        uint8_t *src = (uint8_t *)ptr;
        for (os_size_t i = 0; i < block->size; i++) {
            dst[i] = src[i];
        }
        os_free(ptr);
    }
    return new_ptr;
}

#endif
