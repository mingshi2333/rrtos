#ifndef OS_MEM_H
#define OS_MEM_H

#include "os_types.h"
#include "os_config.h"

os_err_t os_mem_pool_init(os_mem_pool_t *pool, const char *name,
                          void *start, os_size_t block_size, os_size_t block_count);
void *os_mem_pool_alloc(os_mem_pool_t *pool);
os_err_t os_mem_pool_free(os_mem_pool_t *pool, void *ptr);
os_size_t os_mem_pool_free_count(os_mem_pool_t *pool);

#if OS_CFG_HEAP_EN
void os_heap_init(void);
void *os_malloc(os_size_t size);
void os_free(void *ptr);
void *os_calloc(os_size_t count, os_size_t size);
void *os_realloc(void *ptr, os_size_t size);

#define os_mem_alloc(size)  os_malloc(size)
#define os_mem_free(ptr)    os_free(ptr)
#endif

#endif
