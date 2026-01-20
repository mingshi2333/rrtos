#ifndef IREE_CONFIG_CUSTOM_H
#define IREE_CONFIG_CUSTOM_H

#include <stdint.h>

// Time
int64_t iree_rtos_time_now(void);
#define IREE_TIME_NOW_FN return iree_rtos_time_now();

// Wait Handle
// Used in wait_handle.c. iree_ok_status() is available in wait_handle.c scope.
#define IREE_WAIT_HANDLE_IMPL_FN return iree_ok_status();

#endif
