#ifndef IREE_BARE_METAL_CONFIG_H_
#define IREE_BARE_METAL_CONFIG_H_

// ============================================================================
// CRITICAL: Must disable POSIX/C11 aligned allocation before any system headers
// ============================================================================
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#define _POSIX_C_SOURCE 0

#ifdef _ISOC11_SOURCE
#undef _ISOC11_SOURCE
#endif
#define _ISOC11_SOURCE 0

// IREE Bare-Metal Configuration
#define IREE_HAVE_POSIX_MEMALIGN 0
#define IREE_HAVE_STDIO 0
#define IREE_PLATFORM_GENERIC 1

// Enable per-allocation tracing for IREE heap buffers.
#define AI_IREE_ALLOC_TRACE 1

// ============================================================================
// System headers can now be safely included
// ============================================================================
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Bare-Metal Compatibility Definitions
// ============================================================================
#ifndef abort
#define abort() do { while (1) {} } while (0)
#endif

#ifndef abs
#define abs(x) __builtin_abs(x)
#endif
#ifndef llabs
#define llabs(x) __builtin_llabs(x)
#endif

#ifndef fflush
#define fflush(stream) (0)
#endif

// ============================================================================
// IREE Time Interface
// ============================================================================
// #include "iree/base/api.h"
// #include "iree/base/time.h"

// extern iree_time_t os_time_now(void);
// #define IREE_TIME_NOW_FN return os_time_now();

#define IREE_WAIT_UNTIL_FN(deadline_ns) \
  (((void)(deadline_ns)), false)

#endif // IREE_BARE_METAL_CONFIG_H_
