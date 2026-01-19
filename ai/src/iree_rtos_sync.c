#include <stdio.h>
#include <stdbool.h>
#include "iree/base/api.h"
#include "iree/base/internal/synchronization.h"
#include "os_types.h"
#include "os_kernel.h"

// Mutex pool configuration
#define IREE_RTOS_MUTEX_POOL_SIZE 32

typedef struct {
    os_mutex_t mutex;
    int used;
} mutex_slot_t;

static mutex_slot_t mutex_pool[IREE_RTOS_MUTEX_POOL_SIZE];

void iree_slim_mutex_initialize(iree_slim_mutex_t* mutex) {
    if (!mutex) return;
    
    // Find free slot
    int slot = -1;
    for (int i = 0; i < IREE_RTOS_MUTEX_POOL_SIZE; i++) {
        if (!mutex_pool[i].used) {
            mutex_pool[i].used = 1;
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        printf("❌ IREE RTOS Sync: Mutex pool exhausted!\n");
        mutex->reserved = -1;
        return;
    }
    
    // Initialize RTOS mutex
    os_err_t err = os_mutex_init(&mutex_pool[slot].mutex, "iree_mutex");
    if (err != OS_EOK) {
        printf("❌ IREE RTOS Sync: os_mutex_init failed: %d\n", err);
        mutex_pool[slot].used = 0;
        mutex->reserved = -1;
        return;
    }
    
    mutex->reserved = slot;
}

void iree_slim_mutex_deinitialize(iree_slim_mutex_t* mutex) {
    if (!mutex) return;
    int slot = mutex->reserved;
    if (slot >= 0 && slot < IREE_RTOS_MUTEX_POOL_SIZE) {
        // os_mutex_delete(&mutex_pool[slot].mutex); // If available
        mutex_pool[slot].used = 0;
    }
    mutex->reserved = -1;
}

void iree_slim_mutex_lock(iree_slim_mutex_t* mutex) {
    if (!mutex) return;
    int slot = mutex->reserved;
    if (slot >= 0 && slot < IREE_RTOS_MUTEX_POOL_SIZE) {
        os_mutex_lock(&mutex_pool[slot].mutex, OS_WAIT_FOREVER);
    }
}

void iree_slim_mutex_unlock(iree_slim_mutex_t* mutex) {
    if (!mutex) return;
    int slot = mutex->reserved;
    if (slot >= 0 && slot < IREE_RTOS_MUTEX_POOL_SIZE) {
        os_mutex_unlock(&mutex_pool[slot].mutex);
    }
}

// ============================================================================
// Notification (Stub Implementation for Bare Metal)
// ============================================================================

void iree_notification_initialize(iree_notification_t* out_notification) {
    // No-op
}

void iree_notification_deinitialize(iree_notification_t* notification) {
    // No-op
}

void iree_notification_post(iree_notification_t* notification, int32_t count) {
    // No-op
}

bool iree_notification_await(iree_notification_t* notification,
                             iree_condition_fn_t condition_fn, void* condition_arg,
                             iree_timeout_t timeout) {
    // Assuming synchronous execution, condition should be true.
    return condition_fn(condition_arg);
}

bool iree_slim_mutex_try_lock(iree_slim_mutex_t* mutex) {
    if (!mutex) return false;
    int slot = mutex->reserved;
    if (slot >= 0 && slot < IREE_RTOS_MUTEX_POOL_SIZE) {
        // OS_NO_WAIT usually means 0 timeout
        os_err_t err = os_mutex_lock(&mutex_pool[slot].mutex, OS_NO_WAIT);
        return (err == OS_EOK);
    }
    return false;
}

iree_time_t os_time_now(void) {
    return 0; // TODO: Hook to os_tick_get() * tick_period
}
