#include "os_mem.h"
#include <stddef.h>
#include <stdarg.h>

#undef malloc
#undef free
#undef calloc
#undef realloc

void *malloc(size_t size) {
    return os_malloc(size);
}

void free(void *ptr) {
    os_free(ptr);
}

void *calloc(size_t nmemb, size_t size) {
    return os_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size) {
    return os_realloc(ptr, size);
}

void abort(void) {
    while (1);
}
