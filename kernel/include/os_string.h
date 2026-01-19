#ifndef OS_STRING_H
#define OS_STRING_H

#include "os_types.h"

OS_INLINE void *os_memset(void *s, int c, os_size_t n) {
    uint8_t *p = (uint8_t *)s;
    while (n--) {
        *p++ = (uint8_t)c;
    }
    return s;
}

OS_INLINE void *os_memcpy(void *dest, const void *src, os_size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

OS_INLINE void *os_memmove(void *dest, const void *src, os_size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dest;
}

OS_INLINE int os_memcmp(const void *s1, const void *s2, os_size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

OS_INLINE os_size_t os_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return (os_size_t)(p - s);
}

#define memset  os_memset
#define memcpy  os_memcpy
#define memmove os_memmove
#define memcmp  os_memcmp
#define strlen  os_strlen

#endif
