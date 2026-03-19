#ifndef KERNEL_TEST_STDIO_H
#define KERNEL_TEST_STDIO_H

typedef struct kernel_test_file FILE;

extern FILE *stderr;

int printf(const char *fmt, ...);
int fprintf(FILE *stream, const char *fmt, ...);

#endif
