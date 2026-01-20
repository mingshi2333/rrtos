#include <stddef.h>
#include <stdarg.h>

// Stub for vsnprintf if missing from libc
int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
    // Minimal implementation: empty string
    if (size > 0 && str) *str = '\0';
    return 0;
}
