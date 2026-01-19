/**
 * @file os_smp.h
 * @brief SMP Multi-Core Support Header
 * 
 * Provides symmetric multi-processing support including:
 * - Inter-Processor Interrupts (IPI)
 * - Load balancing
 * - CPU affinity management
 * - Core-local data structures
 */

#ifndef OS_SMP_H
#define OS_SMP_H

#include "os_types.h"
#include "os_kernel.h"

#if OS_CFG_SMP_EN

/*===========================================================================*/
/* IPI Reasons                                                                */
/*===========================================================================*/

/** IPI reason codes */
typedef enum {
    OS_IPI_RESCHEDULE   = 0,    /**< Request reschedule on target CPU */
    OS_IPI_CALL_FUNC    = 1,    /**< Execute function on target CPU */
    OS_IPI_STOP_CPU     = 2,    /**< Stop the target CPU */
    OS_IPI_TLB_FLUSH    = 3,    /**< Flush TLB (if MMU enabled) */
    OS_IPI_CACHE_FLUSH  = 4,    /**< Flush cache */
    OS_IPI_HALT         = 5,    /**< Halt for debug */
} os_ipi_reason_t;

/*===========================================================================*/
/* Load Balance Modes                                                         */
/*===========================================================================*/

/** Load balance strategy */
typedef enum {
    OS_LB_NONE          = 0,    /**< No load balancing */
    OS_LB_PUSH          = 1,    /**< Push excess tasks to idle CPUs */
    OS_LB_PULL          = 2,    /**< Idle CPUs pull from busy CPUs */
    OS_LB_HYBRID        = 3,    /**< Combined push/pull */
} os_lb_mode_t;

/*===========================================================================*/
/* SMP Statistics                                                             */
/*===========================================================================*/

/** Per-CPU statistics */
typedef struct os_smp_stats {
    uint64_t    task_runs;      /**< Tasks scheduled on this CPU */
    uint64_t    ipi_sent;       /**< IPIs sent from this CPU */
    uint64_t    ipi_recv;       /**< IPIs received by this CPU */
    uint64_t    migrations;     /**< Tasks migrated from this CPU */
    uint64_t    lb_pulls;       /**< Tasks pulled to this CPU */
    uint64_t    idle_time;      /**< Ticks spent idle */
} os_smp_stats_t;

/*===========================================================================*/
/* Function Prototypes                                                        */
/*===========================================================================*/

/**
 * @brief Initialize SMP subsystem
 * 
 * Sets up per-CPU data structures and IPI handlers.
 * Call after kernel_init on boot CPU.
 */
void os_smp_init(void);

/**
 * @brief Start secondary CPUs
 * 
 * Brings up additional cores and starts their idle tasks.
 * Call after smp_init on boot CPU.
 */
void os_smp_start_cpus(void);

/**
 * @brief Secondary CPU entry point
 * 
 * Called by startup code on secondary CPUs.
 */
void os_smp_secondary_start(void);

/**
 * @brief Send Inter-Processor Interrupt
 * 
 * @param cpu       Target CPU (OS_CPU_ANY for all)
 * @param reason    IPI reason code
 */
void os_ipi_send(os_cpu_t cpu, uint32_t reason);

/**
 * @brief Broadcast IPI to all CPUs except self
 * 
 * @param reason    IPI reason code
 */
void os_ipi_broadcast(uint32_t reason);

/**
 * @brief Handle incoming IPI
 * 
 * Called from IPI interrupt handler.
 */
void os_ipi_handler(void);

/**
 * @brief Set load balancing mode
 * 
 * @param mode      Load balance strategy
 */
void os_smp_set_lb_mode(os_lb_mode_t mode);

/**
 * @brief Trigger load balance check
 * 
 * Called periodically or when CPU becomes idle.
 */
void os_smp_load_balance(void);

/**
 * @brief Migrate task to another CPU
 * 
 * @param tcb       Task to migrate
 * @param target    Target CPU
 * @return OS_EOK on success
 */
os_err_t os_smp_migrate_task(os_tcb_t *tcb, os_cpu_t target);

/**
 * @brief Get CPU load (0-100)
 * 
 * @param cpu       CPU to query
 * @return Load percentage
 */
uint32_t os_smp_get_cpu_load(os_cpu_t cpu);

/**
 * @brief Get SMP statistics for a CPU
 * 
 * @param cpu       CPU to query
 * @param stats     Output statistics
 * @return OS_EOK on success
 */
os_err_t os_smp_get_stats(os_cpu_t cpu, os_smp_stats_t *stats);

/**
 * @brief Check if CPU is online
 * 
 * @param cpu       CPU to check
 * @return true if online
 */
bool os_smp_cpu_online(os_cpu_t cpu);

/**
 * @brief Get number of online CPUs
 * 
 * @return Number of active CPUs
 */
os_cpu_t os_smp_online_count(void);

/**
 * @brief Pin current task to current CPU
 * 
 * Prevents task migration during critical sections.
 */
void os_smp_cpu_pin(void);

/**
 * @brief Unpin current task from CPU
 */
void os_smp_cpu_unpin(void);

/**
 * @brief Execute function on target CPU
 * 
 * @param cpu       Target CPU
 * @param func      Function to execute
 * @param arg       Function argument
 * @param wait      Wait for completion
 * @return OS_EOK on success
 */
typedef void (*os_smp_call_func_t)(void *arg);
os_err_t os_smp_call_on_cpu(os_cpu_t cpu, os_smp_call_func_t func, 
                            void *arg, bool wait);

/**
 * @brief Memory barrier for SMP synchronization
 */
static inline void os_smp_mb(void) {
    __asm__ volatile("fence iorw, iorw" ::: "memory");
}

/**
 * @brief Read memory barrier
 */
static inline void os_smp_rmb(void) {
    __asm__ volatile("fence ir, ir" ::: "memory");
}

/**
 * @brief Write memory barrier
 */
static inline void os_smp_wmb(void) {
    __asm__ volatile("fence ow, ow" ::: "memory");
}

#endif /* OS_CFG_SMP_EN */

#endif /* OS_SMP_H */
