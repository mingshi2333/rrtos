/**
 * @file os_types.h
 * @brief Core OS Type Definitions
 */

#ifndef OS_TYPES_H
#define OS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "os_config.h"

/*===========================================================================*/
/* Basic Types                                                                */
/*===========================================================================*/

#if OS_CFG_ARCH_BITS == 64
    typedef uint64_t    os_ubase_t;     /**< Architecture-dependent unsigned */
    typedef int64_t     os_sbase_t;     /**< Architecture-dependent signed */
    typedef uint64_t    os_reg_t;       /**< Register size type */
#else
    typedef uint32_t    os_ubase_t;
    typedef int32_t     os_sbase_t;
    typedef uint32_t    os_reg_t;
#endif

typedef uint8_t         os_prio_t;      /**< Priority type */
typedef uint32_t        os_tick_t;      /**< System tick type */
typedef uint32_t        os_cpu_t;       /**< CPU ID type */
typedef os_ubase_t      os_size_t;      /**< Size type */

/*===========================================================================*/
/* Error Codes                                                                */
/*===========================================================================*/

typedef enum {
    OS_EOK          = 0,        /**< Success */
    OS_ERROR        = -1,       /**< Generic error */
    OS_ETIMEOUT     = -2,       /**< Operation timed out */
    OS_ENOMEM       = -3,       /**< Out of memory */
    OS_EBUSY        = -4,       /**< Resource busy */
    OS_EINVAL       = -5,       /**< Invalid argument */
    OS_ENOENT       = -6,       /**< No such entry */
    OS_ENOSYS       = -7,       /**< Function not implemented */
    OS_EFULL        = -8,       /**< Resource full */
    OS_EEMPTY       = -9,       /**< Resource empty */
    OS_EPERM        = -10,      /**< Permission denied */
    OS_ENODEV       = -11,      /**< No such device */
    OS_EAGAIN       = -12,      /**< Try again */
    OS_EIO          = -13,      /**< I/O error */
} os_err_t;

/*===========================================================================*/
/* Task States                                                                */
/*===========================================================================*/

typedef enum {
    OS_TASK_INIT        = 0,    /**< Task initialized but not started */
    OS_TASK_READY       = 1,    /**< Ready to run */
    OS_TASK_RUNNING     = 2,    /**< Currently running */
    OS_TASK_SUSPENDED   = 3,    /**< Suspended */
    OS_TASK_BLOCKED     = 4,    /**< Blocked on IPC */
    OS_TASK_SLEEPING    = 5,    /**< Sleeping (delay) */
    OS_TASK_TERMINATED  = 6,    /**< Terminated */
} os_task_state_t;

/**
 * @brief Pending object type for timeouts
 */
typedef enum {
    OS_PENDING_NONE = 0,
    OS_PENDING_SEM,
    OS_PENDING_MUTEX,
    OS_PENDING_QUEUE_RECV,
} os_pending_type_t;

/*===========================================================================*/
/* Task Control Block                                                         */
/*===========================================================================*/

/** Forward declarations */
struct os_tcb;
struct os_sem;
struct os_mutex;
struct os_queue;

/**
 * @brief Task entry function type
 */
typedef void (*os_task_entry_t)(void *arg);

/**
 * @brief Task Control Block (TCB)
 */
typedef struct os_tcb {
    /* Stack management - must be first for context switch */
    void            *sp;            /**< Current stack pointer */
    void            *stack_base;    /**< Stack base address */
    os_size_t       stack_size;     /**< Stack size in bytes */
    
    /* Task identification */
    uint16_t        id;             /**< Task ID */
    const char      *name;          /**< Task name (debug) */
    
    /* Scheduling info */
    os_prio_t       priority;       /**< Current priority */
    os_prio_t       base_prio;      /**< Base priority (before inheritance) */
    os_task_state_t state;          /**< Current state */
    
    /* Time management */
    os_tick_t       time_slice;     /**< Remaining time slice */
    os_tick_t       wake_tick;      /**< Tick to wake from sleep */
    
    /* Multi-core support */
#if OS_CFG_SMP_EN
    os_cpu_t        cpu_id;         /**< Currently running on CPU */
    os_cpu_t        affinity;       /**< CPU affinity mask */
    uint8_t         cpu_lock;       /**< CPU lock count */
#endif
    
    /* Entry point */
    os_task_entry_t entry;          /**< Task entry function */
    void            *arg;           /**< Entry function argument */
    
    /* List linkage */
    struct os_tcb   *next;          /**< Next in ready/wait list */
    struct os_tcb   *prev;          /**< Previous in list */
    
    /* Blocking info */
    void            *pending_obj;   /**< Object task is blocked on */
    void            *pending_ipc;   /**< IPC object for timeout handling */
    os_pending_type_t pending_type;/**< Pending object type */
    os_tick_t       pending_deadline; /**< Timeout deadline tick */
    os_err_t        pending_result; /**< Result of pending operation */
    
    /* Statistics */
#if OS_CFG_STATS_EN
    uint32_t        run_count;      /**< Number of times scheduled */
    uint64_t        run_ticks;      /**< Total ticks running */
    os_size_t       stack_peak;     /**< Peak stack usage */
#endif
    
} os_tcb_t;

/*===========================================================================*/
/* List Management                                                            */
/*===========================================================================*/

/**
 * @brief Doubly-linked list head
 */
typedef struct os_list {
    struct os_tcb   *head;          /**< First item */
    struct os_tcb   *tail;          /**< Last item */
    uint32_t        count;          /**< Number of items */
} os_list_t;

/*===========================================================================*/
/* Synchronization Primitives                                                 */
/*===========================================================================*/

/**
 * @brief Spinlock for SMP
 */
typedef struct os_spinlock {
    volatile uint32_t   lock;       /**< Lock state */
#if OS_CFG_DEBUG_EN
    os_cpu_t            owner;      /**< Owning CPU */
    const char          *name;      /**< Lock name (debug) */
#endif
} os_spinlock_t;

/**
 * @brief Semaphore
 */
typedef struct os_sem {
    volatile int32_t    count;      /**< Current count */
    int32_t             max_count;  /**< Maximum count */
    os_list_t           wait_list;  /**< Waiting tasks */
    os_spinlock_t       lock;       /**< SMP lock */
    const char          *name;      /**< Name (debug) */
} os_sem_t;

/**
 * @brief Mutex
 */
typedef struct os_mutex {
    volatile uint8_t    locked;     /**< Lock state */
    uint8_t             nest_count; /**< Recursive lock count */
    os_tcb_t            *owner;     /**< Current owner */
    os_prio_t           original_prio; /**< Owner's original priority */
    os_list_t           wait_list;  /**< Waiting tasks */
    os_spinlock_t       lock;       /**< SMP lock */
    const char          *name;      /**< Name (debug) */
} os_mutex_t;

/**
 * @brief Message queue
 */
typedef struct os_queue {
    void                *buffer;    /**< Message buffer */
    os_size_t           msg_size;   /**< Size of each message */
    os_size_t           capacity;   /**< Maximum messages */
    volatile os_size_t  count;      /**< Current message count */
    os_size_t           head;       /**< Read index */
    os_size_t           tail;       /**< Write index */
    os_list_t           send_wait;  /**< Tasks waiting to send */
    os_list_t           recv_wait;  /**< Tasks waiting to receive */
    os_spinlock_t       lock;       /**< SMP lock */
    const char          *name;      /**< Name (debug) */
} os_queue_t;

/**
 * @brief Event flags
 */
typedef struct os_event {
    volatile uint32_t   flags;      /**< Current flags */
    os_list_t           wait_list;  /**< Waiting tasks */
    os_spinlock_t       lock;       /**< SMP lock */
    const char          *name;      /**< Name (debug) */
} os_event_t;

/*===========================================================================*/
/* Memory Management                                                          */
/*===========================================================================*/

/**
 * @brief Memory pool block header
 */
typedef struct os_mem_block {
    struct os_mem_block *next;      /**< Next free block */
} os_mem_block_t;

/**
 * @brief Memory pool
 */
typedef struct os_mem_pool {
    void                *start;     /**< Pool memory start */
    os_size_t           block_size; /**< Block size */
    os_size_t           block_count;/**< Total blocks */
    os_mem_block_t      *free_list; /**< Free block list */
    volatile os_size_t  free_count; /**< Free blocks available */
    os_spinlock_t       lock;       /**< SMP lock */
    const char          *name;      /**< Name (debug) */
#if OS_CFG_STATS_EN
    os_size_t           alloc_count;/**< Total allocations */
    os_size_t           peak_used;  /**< Peak usage */
#endif
} os_mem_pool_t;

/*===========================================================================*/
/* Timer                                                                      */
/*===========================================================================*/

/** Timer callback type */
typedef void (*os_timer_cb_t)(void *arg);

/**
 * @brief Software timer
 */
typedef struct os_timer {
    os_tick_t           period;     /**< Timer period */
    os_tick_t           remaining;  /**< Ticks remaining */
    os_timer_cb_t       callback;   /**< Callback function */
    void                *arg;       /**< Callback argument */
    uint8_t             periodic;   /**< Periodic flag */
    uint8_t             active;     /**< Active flag */
    struct os_timer     *next;      /**< Next timer in list */
    const char          *name;      /**< Name (debug) */
} os_timer_t;

/*===========================================================================*/
/* CPU Context                                                                */
/*===========================================================================*/

/**
 * @brief Per-CPU data structure
 */
typedef struct os_cpu_data {
    os_tcb_t        *current;       /**< Currently running task */
    os_tcb_t        *idle_task;     /**< CPU's idle task */
    os_list_t       local_ready;    /**< CPU-bound ready tasks */
    os_spinlock_t   lock;           /**< CPU data lock */
    volatile uint32_t irq_nest;     /**< Interrupt nesting depth */
    volatile uint32_t sched_lock;   /**< Scheduler lock count */
    os_tick_t       tick_count;     /**< Local tick count */
#if OS_CFG_STATS_EN
    uint64_t        idle_ticks;     /**< Ticks spent idle */
    uint64_t        busy_ticks;     /**< Ticks spent busy */
#endif
} os_cpu_data_t;

/*===========================================================================*/
/* Kernel Object Base                                                         */
/*===========================================================================*/

/** Kernel object types */
typedef enum {
    OS_OBJ_TASK     = 0,
    OS_OBJ_SEM      = 1,
    OS_OBJ_MUTEX    = 2,
    OS_OBJ_QUEUE    = 3,
    OS_OBJ_EVENT    = 4,
    OS_OBJ_TIMER    = 5,
    OS_OBJ_POOL     = 6,
} os_obj_type_t;

/*===========================================================================*/
/* Utility Macros                                                             */
/*===========================================================================*/

/** Timeout values */
#define OS_WAIT_FOREVER     ((os_tick_t)-1)
#define OS_NO_WAIT          ((os_tick_t)0)

/** CPU affinity */
#define OS_CPU_ANY          ((os_cpu_t)-1)

/** Priority values */
#define OS_PRIO_HIGHEST     0
#define OS_PRIO_LOWEST      (OS_CFG_PRIO_MAX - 1)
#define OS_PRIO_IDLE        OS_PRIO_LOWEST

/** Static initializers */
#define OS_SPINLOCK_INIT    { .lock = 0 }
#define OS_LIST_INIT        { .head = NULL, .tail = NULL, .count = 0 }

/** Alignment macros */
#define OS_ALIGN_UP(x, a)   (((x) + ((a) - 1)) & ~((a) - 1))
#define OS_ALIGN_DOWN(x, a) ((x) & ~((a) - 1))

/** Container macro */
#define OS_CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

/** Array size */
#define OS_ARRAY_SIZE(a)    (sizeof(a) / sizeof((a)[0]))

/** Min/Max */
#define OS_MIN(a, b)        ((a) < (b) ? (a) : (b))
#define OS_MAX(a, b)        ((a) > (b) ? (a) : (b))

/** Compiler hints */
#define OS_INLINE           static inline __attribute__((always_inline))
#define OS_NOINLINE         __attribute__((noinline))
#define OS_NORETURN         __attribute__((noreturn))
#define OS_WEAK             __attribute__((weak))
#define OS_ALIGNED(n)       __attribute__((aligned(n)))
#define OS_PACKED           __attribute__((packed))
#define OS_SECTION(s)       __attribute__((section(s)))
#define OS_USED             __attribute__((used))

/** Memory barriers */
#define OS_COMPILER_BARRIER()   __asm__ volatile("" ::: "memory")
#define OS_MEMORY_BARRIER()     __asm__ volatile("fence iorw, iorw" ::: "memory")
#define OS_READ_BARRIER()       __asm__ volatile("fence ir, ir" ::: "memory")
#define OS_WRITE_BARRIER()      __asm__ volatile("fence ow, ow" ::: "memory")

#endif /* OS_TYPES_H */
