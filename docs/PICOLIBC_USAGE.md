# Picolibc Usage Guide

## How to use Picolibc in your code

Picolibc is fully integrated, and you can use standard C library functions directly.

### stdio functions
```c
#include <stdio.h>

printf("Hello, World!\n");
printf("Integer: %d, Float: %.2f\n", 42, 3.14);
```

### math functions
```c
#include <math.h>

double x = sin(1.0);
double y = sqrt(4.0);
```

### time functions
```c
#include <sys/time.h>

struct timeval tv;
_gettimeofday(&tv, NULL);
```

### memory allocation
```c
#include <stdlib.h>

int *arr = malloc(10 * sizeof(int));
free(arr);
```

## Syscall Implementation

Picolibc is connected to the RTOS via `kernel/src/picolibc_syscalls.c`:
- `_write` → hal_uart_putc
- `_read` → hal_uart_getc
- `malloc/free` → os_malloc/os_free
- `_gettimeofday` → os_tick_get

## Important Notes

1. File I/O is currently implemented as stubs.
2. Time precision depends on the RTOS tick frequency.
3. The current configuration is single-threaded.
