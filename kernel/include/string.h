#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

int strncmp(const char* s1, const char* s2, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memchr(const void* s, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
size_t strlen(const char* s);

static inline long strtol(const char* s, char** e, int b) {
    (void)s; (void)b;
    if (e) *e = (char*)s;
    return 0;
}

static inline unsigned long strtoul(const char* s, char** e, int b) {
    (void)s; (void)b;
    if (e) *e = (char*)s;
    return 0;
}

static inline long long strtoll(const char* s, char** e, int b) {
    (void)s; (void)b;
    if (e) *e = (char*)s;
    return 0;
}

static inline unsigned long long strtoull(const char* s, char** e, int b) {
    (void)s; (void)b;
    if (e) *e = (char*)s;
    return 0;
}

static inline float strtof(const char* s, char** e) {
    (void)s;
    if (e) *e = (char*)s;
    return 0.0f;
}

static inline double strtod(const char* s, char** e) {
    (void)s;
    if (e) *e = (char*)s;
    return 0.0;
}

#endif
