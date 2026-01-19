/**
 * @file os_timer.c
 * @brief Software Timer Subsystem
 * 
 * Implements software timers with:
 * - One-shot and periodic modes
 * - Timer wheel for O(1) timer management
 * - Dedicated timer task for callback execution
 */

#include "os_kernel.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"

/*===========================================================================*/
/* Configuration                                                              */
/*===========================================================================*/

/** Timer wheel slots (power of 2 for fast modulo) */
#define TIMER_WHEEL_SLOTS       256
#define TIMER_WHEEL_MASK        (TIMER_WHEEL_SLOTS - 1)

/*===========================================================================*/
/* Private Data                                                               */
/*===========================================================================*/

/** Timer wheel - array of timer lists indexed by tick */
static os_timer_t *g_timer_wheel[TIMER_WHEEL_SLOTS];

/** Active timer count */
static volatile uint32_t g_timer_count;

/** Timer subsystem lock */
static os_spinlock_t g_timer_lock = OS_SPINLOCK_INIT;

/** Timer task TCB */
static os_tcb_t g_timer_tcb;
static uint8_t g_timer_stack[OS_CFG_TIMER_STACK_SIZE] OS_ALIGNED(16);

/** Semaphore to wake timer task */
static os_sem_t g_timer_sem;

/** Current wheel position */
static volatile os_tick_t g_timer_wheel_pos;

/*===========================================================================*/
/* Private Functions                                                          */
/*===========================================================================*/

/**
 * @brief Calculate wheel slot for a given tick
 */
static inline uint32_t timer_wheel_slot(os_tick_t tick) {
    return tick & TIMER_WHEEL_MASK;
}

/**
 * @brief Insert timer into wheel at specified slot
 */
static void timer_wheel_insert(os_timer_t *timer, os_tick_t expire_tick) {
    uint32_t slot = timer_wheel_slot(expire_tick);
    
    timer->remaining = expire_tick;
    timer->next = g_timer_wheel[slot];
    g_timer_wheel[slot] = timer;
}

/**
 * @brief Remove timer from wheel
 */
static bool timer_wheel_remove(os_timer_t *timer) {
    /* Search all slots - could optimize with slot tracking */
    for (uint32_t slot = 0; slot < TIMER_WHEEL_SLOTS; slot++) {
        os_timer_t **pp = &g_timer_wheel[slot];
        while (*pp) {
            if (*pp == timer) {
                *pp = timer->next;
                timer->next = NULL;
                return true;
            }
            pp = &(*pp)->next;
        }
    }
    return false;
}

/**
 * @brief Process expired timers in current slot
 */
static void timer_process_slot(os_tick_t current_tick) {
    uint32_t slot = timer_wheel_slot(current_tick);
    os_timer_t *timer = g_timer_wheel[slot];
    
    g_timer_wheel[slot] = NULL;
    
    while (timer) {
        os_timer_t *next = timer->next;
        
        /* Check if this timer actually expired (handle wrap-around) */
        os_tick_t diff = current_tick - timer->remaining;
        
        if (diff < 0x80000000UL) {
            /* Timer expired - execute callback */
            timer->active = 0;
            timer->next = NULL;
            
            /* Execute callback outside of lock */
            os_spinlock_unlock(&g_timer_lock);
            
            if (timer->callback) {
                timer->callback(timer->arg);
            }
            
            os_spinlock_lock(&g_timer_lock);
            
            /* Re-arm periodic timers */
            if (timer->periodic) {
                timer->active = 1;
                os_tick_t next_expire = current_tick + timer->period;
                timer_wheel_insert(timer, next_expire);
            } else {
                g_timer_count--;
            }
        } else {
            /* Timer hasn't expired yet, re-insert */
            timer->next = g_timer_wheel[slot];
            g_timer_wheel[slot] = timer;
        }
        
        timer = next;
    }
}

/**
 * @brief Timer task entry
 * 
 * Processes expired timers and executes callbacks.
 * Runs at configurable priority.
 */
static void timer_task_entry(void *arg) {
    (void)arg;
    
    while (1) {
        /* Wait for timer tick notification */
        os_sem_take(&g_timer_sem, OS_WAIT_FOREVER);
        
        os_spinlock_lock(&g_timer_lock);
        
        /* Get current tick */
        os_tick_t current_tick = os_tick_get();
        
        /* Process all slots from last position to current */
        while (g_timer_wheel_pos != current_tick) {
            g_timer_wheel_pos++;
            timer_process_slot(g_timer_wheel_pos);
        }
        
        os_spinlock_unlock(&g_timer_lock);
    }
}

/*===========================================================================*/
/* Public API                                                                 */
/*===========================================================================*/

/**
 * @brief Initialize software timer subsystem
 * 
 * Creates timer task and initializes timer wheel.
 * Call this after kernel init but before kernel start.
 */
void os_timer_subsys_init(void) {
    /* Initialize timer wheel */
    for (uint32_t i = 0; i < TIMER_WHEEL_SLOTS; i++) {
        g_timer_wheel[i] = NULL;
    }
    
    g_timer_count = 0;
    g_timer_wheel_pos = 0;
    os_spinlock_init(&g_timer_lock);
    
    /* Initialize timer semaphore */
    os_sem_init(&g_timer_sem, "timer_sem", 0, 0x7FFFFFFF);
    
    /* Create timer task */
    os_task_create(&g_timer_tcb, "timer", timer_task_entry, NULL,
                   OS_CFG_TIMER_TASK_PRIO, g_timer_stack, 
                   OS_CFG_TIMER_STACK_SIZE);
}

/**
 * @brief Timer tick handler - call from system tick ISR
 * 
 * Signals timer task if any timers may have expired.
 */
void os_timer_tick(void) {
    if (g_timer_count > 0) {
        os_sem_give(&g_timer_sem);
    }
}

/**
 * @brief Initialize a software timer
 * 
 * @param timer     Timer control block
 * @param name      Timer name (debug)
 * @param cb        Callback function
 * @param arg       Callback argument
 * @param period    Timer period in ticks
 * @param periodic  Non-zero for periodic, zero for one-shot
 * @return OS_EOK on success
 */
os_err_t os_timer_init(os_timer_t *timer, const char *name, os_timer_cb_t cb,
                       void *arg, os_tick_t period, uint8_t periodic) {
    if (!timer || !cb || period == 0) {
        return OS_EINVAL;
    }
    
    timer->name = name;
    timer->callback = cb;
    timer->arg = arg;
    timer->period = period;
    timer->remaining = 0;
    timer->periodic = periodic ? 1 : 0;
    timer->active = 0;
    timer->next = NULL;
    
    return OS_EOK;
}

/**
 * @brief Start a timer
 * 
 * @param timer     Timer to start
 * @return OS_EOK on success
 */
os_err_t os_timer_start(os_timer_t *timer) {
    if (!timer) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_timer_lock);
    
    /* Remove if already active */
    if (timer->active) {
        timer_wheel_remove(timer);
    } else {
        g_timer_count++;
    }
    
    /* Calculate expiration tick */
    os_tick_t expire = os_tick_get() + timer->period;
    
    /* Insert into wheel */
    timer->active = 1;
    timer_wheel_insert(timer, expire);
    
    os_spinlock_irq_restore(&g_timer_lock, flags);
    
    return OS_EOK;
}

/**
 * @brief Stop a timer
 * 
 * @param timer     Timer to stop
 * @return OS_EOK on success
 */
os_err_t os_timer_stop(os_timer_t *timer) {
    if (!timer) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_timer_lock);
    
    if (timer->active) {
        timer_wheel_remove(timer);
        timer->active = 0;
        g_timer_count--;
    }
    
    os_spinlock_irq_restore(&g_timer_lock, flags);
    
    return OS_EOK;
}

/**
 * @brief Reset and restart a timer with new period
 * 
 * @param timer     Timer to reset
 * @param period    New period (0 = keep current)
 * @return OS_EOK on success
 */
os_err_t os_timer_reset(os_timer_t *timer, os_tick_t period) {
    if (!timer) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_timer_lock);
    
    if (period > 0) {
        timer->period = period;
    }
    
    /* Remove if active */
    if (timer->active) {
        timer_wheel_remove(timer);
    } else {
        g_timer_count++;
    }
    
    /* Restart */
    os_tick_t expire = os_tick_get() + timer->period;
    timer->active = 1;
    timer_wheel_insert(timer, expire);
    
    os_spinlock_irq_restore(&g_timer_lock, flags);
    
    return OS_EOK;
}

/**
 * @brief Check if timer is currently active
 * 
 * @param timer     Timer to check
 * @return true if active
 */
bool os_timer_is_active(os_timer_t *timer) {
    if (!timer) {
        return false;
    }
    return timer->active != 0;
}

/**
 * @brief Get remaining time until timer expires
 * 
 * @param timer     Timer to query
 * @return Remaining ticks, or 0 if not active
 */
os_tick_t os_timer_remaining(os_timer_t *timer) {
    if (!timer || !timer->active) {
        return 0;
    }
    
    os_tick_t current = os_tick_get();
    os_tick_t diff = timer->remaining - current;
    
    /* Check for wrap-around / already expired */
    if (diff > 0x80000000UL) {
        return 0;
    }
    
    return diff;
}

/**
 * @brief Get timer statistics
 * 
 * @param active_count  Output: number of active timers
 */
void os_timer_stats(uint32_t *active_count) {
    if (active_count) {
        *active_count = g_timer_count;
    }
}
