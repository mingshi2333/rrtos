#ifndef OS_MEM_H
#define OS_MEM_H

#include "os_types.h"
#include "os_config.h"

#ifdef __cplusplus
extern "C" {
#endif

os_err_t os_mem_pool_init(os_mem_pool_t *pool, const char *name,
                          void *start, os_size_t block_size, os_size_t block_count);
void *os_mem_pool_alloc(os_mem_pool_t *pool);
os_err_t os_mem_pool_free(os_mem_pool_t *pool, void *ptr);
os_size_t os_mem_pool_free_count(os_mem_pool_t *pool);

#if OS_CFG_HEAP_EN
typedef struct os_heap_stats {
    os_size_t current_used_bytes;
    os_size_t peak_used_bytes;
    os_size_t allocation_count;
    os_size_t free_count;
} os_heap_stats_t;

enum {
    OS_HEAP_TRACE_TOP_CAPACITY = 8,
};

typedef struct os_heap_trace_record {
    os_size_t size;
    os_size_t current_used_bytes;
    uintptr_t ptr;
    uintptr_t caller;
    os_size_t sequence;
} os_heap_trace_record_t;

typedef struct os_heap_trace {
    os_heap_trace_record_t largest_allocations[OS_HEAP_TRACE_TOP_CAPACITY];
    os_heap_trace_record_t largest_live_block;
    os_heap_trace_record_t peak_event;
    os_size_t sequence;
} os_heap_trace_t;

void os_heap_init(void);
void *os_malloc(os_size_t size);
void os_free(void *ptr);
void *os_calloc(os_size_t count, os_size_t size);
void *os_realloc(void *ptr, os_size_t size);
os_heap_stats_t os_heap_stats_get(void);
void os_heap_stats_reset_peak(void);
void os_heap_trace_reset(void);
os_heap_trace_t os_heap_trace_get(void);

#define os_mem_alloc(size)  os_malloc(size)
#define os_mem_free(ptr)    os_free(ptr)
#endif

#ifdef __cplusplus
}
#endif

#endif
