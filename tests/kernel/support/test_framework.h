#ifndef KERNEL_TEST_FRAMEWORK_H
#define KERNEL_TEST_FRAMEWORK_H

#include <stdio.h>

#define OS_TEST_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "ASSERT FAILED: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
            return 1; \
        } \
    } while (0)

#endif
