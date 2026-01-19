#include "os_kernel.h"
#include "riscv_atomic.h"

os_err_t os_mem_pool_init(os_mem_pool_t *pool, const char *name,
                          void *start, os_size_t block_size, os_size_t block_count) {
    if (!pool || !start || block_size == 0 || block_count == 0) {
        return OS_EINVAL;
    }
    
    if (block_size < sizeof(os_mem_block_t)) {
        block_size = sizeof(os_mem_block_t);
    }
    block_size = OS_ALIGN_UP(block_size, sizeof(void *));
    
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

extern uint8_t _heap_start[];
extern uint8_t _heap_end[];

static heap_block_t *g_heap_free_list;
static os_spinlock_t g_heap_lock = OS_SPINLOCK_INIT;

void os_heap_init(void) {
    uint8_t *heap_start = _heap_start;
    uintptr_t aligned_start = OS_ALIGN_UP((uintptr_t)heap_start, 8);
    os_size_t heap_size = (os_size_t)(_heap_end - (uint8_t *)aligned_start);

    heap_block_t *first = (heap_block_t *)aligned_start;
    first->size = heap_size - sizeof(heap_block_t);
    first->used = 0;
    first->next = NULL;
    first->prev = NULL;
    g_heap_free_list = first;
}

void *os_malloc(os_size_t size) {
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
    
    os_spinlock_irq_restore(&g_heap_lock, flags);
    return (void *)((uint8_t *)best + sizeof(heap_block_t));
}

void os_free(void *ptr) {
    if (!ptr) {
        return;
    }
    
    heap_block_t *block = (heap_block_t *)((uint8_t *)ptr - sizeof(heap_block_t));
    
    os_reg_t flags = os_spinlock_irq_save(&g_heap_lock);
    
    block->used = 0;
    
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

void *os_calloc(os_size_t count, os_size_t size) {
    os_size_t total = count * size;
    void *ptr = os_malloc(total);
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
    
    void *new_ptr = os_malloc(size);
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
