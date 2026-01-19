#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include "os_kernel.h"
#include "hal_uart.h"

// ============================================================================
// Required syscall implementations
// ============================================================================

// Write characters to stdout/stderr (for printf)
int _write(int file, const void *ptr, size_t len) {
    const char *buf = (const char *)ptr;
    
    if (file == 1 || file == 2) {  // stdout or stderr
        for (size_t i = 0; i < len; i++) {
            hal_uart_putc(buf[i]);
        }
        return len;
    }
    
    errno = EBADF;
    return -1;
}

// Read characters from stdin (for scanf)
int _read(int file, void *ptr, size_t len) {
    char *buf = (char *)ptr;
    
    if (file == 0) {  // stdin
        for (size_t i = 0; i < len; i++) {
            buf[i] = hal_uart_getc();
            if (buf[i] == '\r' || buf[i] == '\n') {
                return i + 1;
            }
        }
        return len;
    }
    
    errno = EBADF;
    return -1;
}

// Close file (stub)
int _close(int file) {
    (void)file;
    return 0;
}

// File seek (stub)
off_t _lseek(int file, off_t offset, int whence) {
    (void)file;
    (void)offset;
    (void)whence;
    return 0;
}

// File status (stub)
int _fstat(int file, struct stat *st) {
    (void)file;
    st->st_mode = S_IFCHR;  // Character device
    return 0;
}

// Check if is terminal
int _isatty(int file) {
    return (file >= 0 && file <= 2);  // stdin/stdout/stderr
}

// ============================================================================
// Memory management (connect to RTOS malloc)
// ============================================================================

extern void *os_malloc(size_t size);
extern void os_free(void *ptr);
extern void *os_realloc(void *ptr, size_t size);
extern void *os_calloc(size_t nmemb, size_t size);

__attribute__((weak))
void *malloc(size_t size) {
    return os_malloc(size);
}

__attribute__((weak))
void free(void *ptr) {
    os_free(ptr);
}

__attribute__((weak))
void *realloc(void *ptr, size_t size) {
    return os_realloc(ptr, size);
}

__attribute__((weak))
void *calloc(size_t nmemb, size_t size) {
    return os_calloc(nmemb, size);
}

// ============================================================================
// Time functions (connect to RTOS tick)
// ============================================================================

int _gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    
    if (tv) {
        os_tick_t ticks = os_tick_get();
        // Assume OS_TICK_RATE_HZ = 1000 (1ms per tick)
        tv->tv_sec = ticks / 1000;
        tv->tv_usec = (ticks % 1000) * 1000;
    }
    
    return 0;
}

// ============================================================================
// Process management (stub)
// ============================================================================

int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status) {
    // In bare-metal environment, enter infinite loop
    (void)status;
    while (1) {
        __asm__ volatile ("wfi");  // Wait For Interrupt
    }
}

// ============================================================================
// Heap management (sbrk)
// ============================================================================

extern char _heap_start __attribute__((weak));  // Defined by linker script
extern char _heap_end __attribute__((weak));    // Defined by linker script
// Heap pointer - initialized lazily to avoid .data dependency
static char *heap_ptr = NULL;

void *_sbrk(ptrdiff_t incr) {
    // Lazy initialization - handles Flash XIP where .data may not be copied
    if (heap_ptr == NULL) {
        heap_ptr = &_heap_start;
    }
    
    char *old_heap = heap_ptr;
    char *new_heap = heap_ptr + incr;
    
    // Check heap overflow
    if (new_heap > &_heap_end) {
        errno = ENOMEM;
        return (void *)-1;
    }
    
    heap_ptr = new_heap;
    return old_heap;
}
