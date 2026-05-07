#include "os_mem.h"
#include "os_kernel.h"
#include "hal_uart.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#undef malloc
#undef free
#undef calloc
#undef realloc

void *malloc(size_t size) {
    void *ptr = os_malloc(size);
#if defined(AI_IREE_ALLOC_TRACE) && AI_IREE_ALLOC_TRACE
    if (ptr == NULL && size != 0u) {
        os_print("[AI_ALLOC] malloc_fail size=%u caller=0x%x\n",
                 (uint32_t)size,
                 (uint32_t)(uintptr_t)__builtin_return_address(0));
    }
#endif
    return ptr;
}

void free(void *ptr) {
    os_free(ptr);
}

void *calloc(size_t nmemb, size_t size) {
    void *ptr = os_calloc(nmemb, size);
#if defined(AI_IREE_ALLOC_TRACE) && AI_IREE_ALLOC_TRACE
    if (ptr == NULL && nmemb != 0u && size != 0u) {
        os_print("[AI_ALLOC] calloc_fail count=%u size=%u caller=0x%x\n",
                 (uint32_t)nmemb,
                 (uint32_t)size,
                 (uint32_t)(uintptr_t)__builtin_return_address(0));
    }
#endif
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    void *new_ptr = os_realloc(ptr, size);
#if defined(AI_IREE_ALLOC_TRACE) && AI_IREE_ALLOC_TRACE
    if (new_ptr == NULL && size != 0u) {
        os_print("[AI_ALLOC] realloc_fail size=%u caller=0x%x\n",
                 (uint32_t)size,
                 (uint32_t)(uintptr_t)__builtin_return_address(0));
    }
#endif
    return new_ptr;
}

void abort(void) {
    while (1);
}
