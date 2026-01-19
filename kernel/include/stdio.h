#ifndef _STDIO_H
#define _STDIO_H
#include "hal_uart.h"

typedef int FILE;

#define printf os_print
#define fprintf(fp, ...) os_print(__VA_ARGS__)

static inline int snprintf(char* buf, unsigned long len, const char* fmt, ...) {
    (void)buf; (void)len; (void)fmt;
    return 0;
}

static inline int vsnprintf(char* buf, unsigned long len, const char* fmt, __builtin_va_list ap) {
    (void)buf; (void)len; (void)fmt; (void)ap;
    return 0;
}

#define stderr ((FILE*)0)
#define stdout ((FILE*)1)
#define stdin  ((FILE*)2)

#endif
