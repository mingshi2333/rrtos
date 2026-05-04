/**
 * @file os_smp.c
 * @brief SMP Multi-Core Implementation
 * 
 * Implements symmetric multi-processing including:
 * - Inter-Processor Interrupts via CLINT MSIP
 * - Load balancing with configurable strategy
 * - Task migration between cores
 */

#include "os_smp.h"
#include "os_kernel.h"
#include "os_types.h"
#include <stdint.h>
#include "riscv_csr.h"
#include "riscv_atomic.h"
#include "hal_clint.h"
#include "hal_uart.h"

#if defined(CONFIG_BOARD_BE_U1000)
#include "hal_corectrl.h"
extern void _start1(void);
#endif

extern volatile uint32_t _hart_ready[];

#if OS_CFG_SMP_EN

#if OS_CFG_CPU_COUNT < 2
#error "OS_CFG_CPU_COUNT must be at least 2 when OS_CFG_SMP_EN is enabled"
#endif

/*===========================================================================*/
/* Private Data                                                               */
/*===========================================================================*/

/** CPU online status bitmap */
static volatile uint32_t g_cpu_online_mask;

/** CPU ready to run bitmap */
static volatile uint32_t g_cpu_ready_mask;

/** Online secondary CPUs released into scheduler startup */
static volatile uint32_t g_scheduler_release_mask;

/** IPI pending flags indexed by compile-time CPU ceiling slots */
static volatile uint32_t g_ipi_pending[OS_CFG_CPU_MAX];

/** Load balance mode */
static os_lb_mode_t g_lb_mode = OS_LB_HYBRID;

/** Load balance lock */
static os_spinlock_t g_lb_lock = OS_SPINLOCK_INIT;

/** SMP statistics indexed by compile-time CPU ceiling slots */
static os_smp_stats_t g_smp_stats[OS_CFG_CPU_MAX];

/** Remote function call data indexed by compile-time CPU ceiling slots */
typedef struct {
    os_smp_call_func_t  func;
    void                *arg;
    volatile bool       done;
    os_spinlock_t       lock;
} os_smp_call_t;

static os_smp_call_t g_smp_call[OS_CFG_CPU_MAX];

/** Per-CPU load tracking indexed by compile-time CPU ceiling slots */
static volatile uint32_t g_cpu_load[OS_CFG_CPU_MAX];

#if OS_CFG_LOAD_BALANCE_EN
/** Load balance timer */
static os_timer_t g_lb_timer;
#endif

/*===========================================================================*/
/* Board Hooks                                                                */
/*===========================================================================*/

#if defined(CONFIG_BOARD_BE_U1000)
static void smp_board_reset_settle_delay(void) {
    for (volatile uint32_t i = 0; i < 10000u; ++i) {
        __asm__ volatile("nop");
    }
}

static bool smp_board_prepare_cpu(os_cpu_t cpu) {
    if (cpu != 1) {
        return false;
    }

    (void)hal_corectrl_force_reset_all(HAL_CORECTRL_CORE1);
    smp_board_reset_settle_delay();
    return true;
}

static bool smp_board_start_cpu(os_cpu_t cpu) {
    if (cpu != 1) {
        return false;
    }

    uintptr_t reset_vector = (uintptr_t)_start1;
    os_print("[SMP] CPU1 start-vector: 0x%x\n", (uint32_t)reset_vector);
    int rc = hal_corectrl_start(HAL_CORECTRL_CORE1, reset_vector);
    os_print("[SMP] CPU1 start-rc: %d\n", rc);
    return rc == 0;
}
#else
static bool smp_board_prepare_cpu(os_cpu_t cpu) {
    (void)cpu;
    return false;
}

static bool smp_board_start_cpu(os_cpu_t cpu) {
    (void)cpu;
    return false;
}
#endif

static bool smp_runtime_ipi_enabled(void) {
    return OS_CFG_SMP_RUNTIME_IPI_EN != 0;
}

/*===========================================================================*/
/* External Functions                                                         */
/*===========================================================================*/

extern void os_sched(void);
extern os_tcb_t *os_task_self(void);
extern os_cpu_data_t *os_cpu_data(os_cpu_t cpu);

/*===========================================================================*/
/* Private Functions                                                          */
/*===========================================================================*/

/**
 * @brief Get least loaded CPU
 */
static os_cpu_t find_least_loaded_cpu(os_cpu_t exclude) {
    os_cpu_t best = 0;
    uint32_t min_load = 0xFFFFFFFF;
    
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (i == exclude) continue;
        if (!(g_cpu_online_mask & (1U << i))) continue;
        
        if (g_cpu_load[i] < min_load) {
            min_load = g_cpu_load[i];
            best = i;
        }
    }
    
    return best;
}

/**
 * @brief Get most loaded CPU
 */
static os_cpu_t find_most_loaded_cpu(os_cpu_t exclude) {
    os_cpu_t best = 0;
    uint32_t max_load = 0;
    
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (i == exclude) continue;
        if (!(g_cpu_online_mask & (1U << i))) continue;
        
        if (g_cpu_load[i] > max_load) {
            max_load = g_cpu_load[i];
            best = i;
        }
    }
    
    return best;
}

/**
 * @brief Calculate CPU load from ready queue
 */
static uint32_t calculate_cpu_load(os_cpu_t cpu) {
    os_cpu_data_t *data = os_cpu_data(cpu);
    if (!data) return 0;
    
    /* Load = number of ready tasks + running (not idle) */
    uint32_t count = data->local_ready.count;
    if (data->current && data->current != data->idle_task) {
        count++;
    }
    
    /* Scale to percentage (assume 10 tasks = 100% load) */
    uint32_t load = count * 10;
    if (load > 100) load = 100;
    
    return load;
}

/**
 * @brief Update load tracking for all CPUs
 */
static void update_load_stats(void) {
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (g_cpu_online_mask & (1U << i)) {
            uint32_t current = calculate_cpu_load(i);
            /* Exponential moving average */
            g_cpu_load[i] = (g_cpu_load[i] * 3 + current) / 4;
        }
    }
}

#if OS_CFG_LOAD_BALANCE_EN
/**
 * @brief Load balance timer callback
 */
static void lb_timer_callback(void *arg) {
    (void)arg;
    os_smp_load_balance();
}
#endif

/**
 * @brief Push a task from busy CPU to idle CPU
 */
static void lb_push(os_cpu_t from, os_cpu_t to) {
    if (!smp_runtime_ipi_enabled()) {
        return;
    }

    os_cpu_data_t *from_data = os_cpu_data(from);
    os_cpu_data_t *to_data = os_cpu_data(to);
    
    if (!from_data || !to_data) return;
    
    /* Find a migratable task */
    os_spinlock_lock(&from_data->lock);
    
    os_tcb_t *tcb = from_data->local_ready.head;
    while (tcb) {
        /* Skip tasks pinned to this CPU */
        if (tcb->affinity == OS_CPU_ANY || tcb->affinity == to) {
            /* Remove from source CPU */
            if (tcb->prev) {
                tcb->prev->next = tcb->next;
            } else {
                from_data->local_ready.head = tcb->next;
            }
            if (tcb->next) {
                tcb->next->prev = tcb->prev;
            } else {
                from_data->local_ready.tail = tcb->prev;
            }
            from_data->local_ready.count--;
            
            os_spinlock_unlock(&from_data->lock);
            
            /* Add to target CPU */
            os_spinlock_lock(&to_data->lock);
            
            tcb->next = NULL;
            tcb->prev = to_data->local_ready.tail;
            if (to_data->local_ready.tail) {
                to_data->local_ready.tail->next = tcb;
            } else {
                to_data->local_ready.head = tcb;
            }
            to_data->local_ready.tail = tcb;
            to_data->local_ready.count++;
            
            os_spinlock_unlock(&to_data->lock);
            
            /* Update stats */
            g_smp_stats[from].migrations++;
            
            /* Notify target CPU */
            os_ipi_send(to, OS_IPI_RESCHEDULE);
            
            return;
        }
        tcb = tcb->next;
    }
    
    os_spinlock_unlock(&from_data->lock);
}

/**
 * @brief Pull a task from busy CPU to idle CPU
 */
static void lb_pull(os_cpu_t to) {
    os_cpu_t from = find_most_loaded_cpu(to);
    
    if (g_cpu_load[from] > g_cpu_load[to] + 20) {
        lb_push(from, to);
        g_smp_stats[to].lb_pulls++;
    }
}

/*===========================================================================*/
/* Public API                                                                 */
/*===========================================================================*/

void os_smp_init(void) {
    os_cpu_t boot_cpu = os_cpu_id();
    
    /* Initialize data structures */
    g_cpu_online_mask = 1U << boot_cpu;
    g_cpu_ready_mask = 0;
    g_scheduler_release_mask = 0;
    
    for (os_cpu_t i = 0; i < OS_CFG_CPU_MAX; i++) {
        g_ipi_pending[i] = 0;
        g_cpu_load[i] = 0;
        
        /* Clear stats */
        g_smp_stats[i].task_runs = 0;
        g_smp_stats[i].ipi_sent = 0;
        g_smp_stats[i].ipi_recv = 0;
        g_smp_stats[i].migrations = 0;
        g_smp_stats[i].lb_pulls = 0;
        g_smp_stats[i].idle_time = 0;
        
        /* Init call structures */
        g_smp_call[i].func = NULL;
        g_smp_call[i].done = true;
        os_spinlock_init(&g_smp_call[i].lock);
    }
    
    os_spinlock_init(&g_lb_lock);
    
#if OS_CFG_LOAD_BALANCE_EN
    /* Initialize load balance timer */
    os_timer_init(&g_lb_timer, "lb_timer", lb_timer_callback, NULL,
                  OS_CFG_LOAD_BALANCE_TICKS, 1);
    os_timer_start(&g_lb_timer);
#endif
}

void os_smp_start_cpus(void) {
    os_cpu_t boot_cpu = os_cpu_id();
    
    /* Signal secondary CPUs to start */
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (i != boot_cpu) {
            bool board_started;

            (void)smp_board_prepare_cpu(i);

            /* Set ready flag for secondary CPU */
            g_cpu_ready_mask |= (1U << i);

            /* Release secondary from startup wait */
            _hart_ready[i] = 1;
            OS_MEMORY_BARRIER();

            board_started = smp_board_start_cpu(i);

            if (!board_started) {
                /* Send IPI to wake secondary CPU */
                hal_clint_ipi_send(i);
            }
        }
    }
    
    /* Wait for all CPUs to come online */
    uint32_t timeout = 1000000;
    uint32_t expected = (1U << OS_CFG_CPU_COUNT) - 1;
    
    while ((g_cpu_online_mask & expected) != expected && timeout > 0) {
        timeout--;
    }

    if ((g_cpu_online_mask & expected) == expected) {
        os_print("[SMP] online-count: %u\n", os_smp_online_count());
    } else {
        os_print("[SMP] online-timeout mask=0x%x expected=0x%x\n", g_cpu_online_mask, expected);
    }
}

void os_smp_release_cpus(void) {
    os_cpu_t boot_cpu = os_cpu_id();

    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (i == boot_cpu) {
            continue;
        }

        g_scheduler_release_mask |= (1U << i);
        OS_MEMORY_BARRIER();
#if OS_CFG_SMP_RELEASE_IPI_EN
        hal_clint_ipi_send(i);
#endif
    }
}

void os_smp_secondary_start(void) {
    os_cpu_t cpu = os_cpu_id();
    os_print("[CPU%d] Secondary core starting\n", cpu);
    
    /* Wait for boot CPU to signal ready */
    while (!(g_cpu_ready_mask & (1U << cpu))) {
        os_wfi();
    }
    
    /* Initialize per-CPU data */
    os_cpu_data_t *data = os_cpu_data(cpu);
    data->irq_nest = 0;
    data->sched_lock = 0;

    csr_set(mie, MIE_MSIE);
    csr_set(mstatus, MSTATUS_MIE);

    /* Mark CPU as online (atomic OR) */
    __asm__ volatile("amoor.w zero, %0, (%1)" 
                     : 
                     : "r"(1U << cpu), "r"(&g_cpu_online_mask) 
                     : "memory");

    os_print("[SMP] CPU%d secondary online\n", cpu);

    while (!(g_scheduler_release_mask & (1U << cpu))) {
        __asm__ volatile("nop");
    }
    
    os_kernel_start();
    
    /* Should never return */
    while (1) {
        os_wfi();
    }
}

void os_ipi_send(os_cpu_t cpu, uint32_t reason) {
    if (!smp_runtime_ipi_enabled()) {
        return;
    }

    if (cpu == OS_CPU_ANY) {
        os_ipi_broadcast(reason);
        return;
    }
    
    if (cpu >= OS_CFG_CPU_COUNT) {
        return;
    }
    
    /* Set pending flag */
    __asm__ volatile("amoor.w zero, %0, (%1)" 
                     : 
                     : "r"(1U << reason), "r"(&g_ipi_pending[cpu]) 
                     : "memory");
    
    /* Send software interrupt via CLINT */
    hal_clint_ipi_send(cpu);
    
    /* Update stats */
    g_smp_stats[os_cpu_id()].ipi_sent++;
}

void os_ipi_broadcast(uint32_t reason) {
    if (!smp_runtime_ipi_enabled()) {
        return;
    }

    os_cpu_t self = os_cpu_id();
    
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        if (i != self && (g_cpu_online_mask & (1U << i))) {
            os_ipi_send(i, reason);
        }
    }
}

void os_ipi_handler(void) {
    os_cpu_t cpu = os_cpu_id();
    
    /* Clear IPI */
    hal_clint_ipi_clear(cpu);
    
    /* Get and clear pending flags (atomic exchange) */
    uint32_t pending;
    __asm__ volatile("amoswap.w %0, zero, (%1)" 
                     : "=r"(pending) 
                     : "r"(&g_ipi_pending[cpu]) 
                     : "memory");
    
    /* Update stats */
    g_smp_stats[cpu].ipi_recv++;
    
    /* Process each pending reason */
    if (pending & (1U << OS_IPI_RESCHEDULE)) {
        os_print("[SMP] CPU%d IPI reschedule\n", cpu);
        /* Just return - scheduler will run on ISR exit */
    }
    
    if (pending & (1U << OS_IPI_CALL_FUNC)) {
        /* Execute remote function */
        os_smp_call_t *call = &g_smp_call[cpu];
        if (call->func) {
            call->func(call->arg);
            call->done = true;
        }
    }
    
    if (pending & (1U << OS_IPI_STOP_CPU)) {
        /* Stop this CPU */
        os_irq_disable();
        while (1) {
            os_wfi();
        }
    }
    
    if (pending & (1U << OS_IPI_HALT)) {
        /* Debug halt */
        __asm__ volatile("ebreak");
    }
}

void os_smp_set_lb_mode(os_lb_mode_t mode) {
    g_lb_mode = mode;
}

void os_smp_load_balance(void) {
    if (!smp_runtime_ipi_enabled()) {
        return;
    }

    if (g_lb_mode == OS_LB_NONE) {
        return;
    }
    
    os_spinlock_lock(&g_lb_lock);
    
    /* Update load statistics */
    update_load_stats();
    
    os_cpu_t self = os_cpu_id();
    
    if (g_lb_mode == OS_LB_PUSH || g_lb_mode == OS_LB_HYBRID) {
        /* Check if we're overloaded */
        if (g_cpu_load[self] > 60) {
            os_cpu_t target = find_least_loaded_cpu(self);
            if (g_cpu_load[target] < g_cpu_load[self] - 20) {
                lb_push(self, target);
            }
        }
    }
    
    if (g_lb_mode == OS_LB_PULL || g_lb_mode == OS_LB_HYBRID) {
        /* Check if we're idle */
        if (g_cpu_load[self] < 20) {
            lb_pull(self);
        }
    }
    
    os_spinlock_unlock(&g_lb_lock);
}

os_err_t os_smp_migrate_task(os_tcb_t *tcb, os_cpu_t target) {
    if (!tcb || target >= OS_CFG_CPU_COUNT) {
        return OS_EINVAL;
    }

    if (!smp_runtime_ipi_enabled() && target != os_cpu_id()) {
        return OS_ENOSYS;
    }
    
    if (!(g_cpu_online_mask & (1U << target))) {
        return OS_ENODEV;
    }
    
    /* Set affinity to target CPU */
    tcb->affinity = target;
    
    /* If task is not currently running, migrate immediately */
    if (tcb->state == OS_TASK_READY) {
        os_cpu_t current_cpu = tcb->cpu_id;
        if (current_cpu != target) {
            lb_push(current_cpu, target);
        }
    }
    
    return OS_EOK;
}

uint32_t os_smp_get_cpu_load(os_cpu_t cpu) {
    if (cpu >= OS_CFG_CPU_COUNT) {
        return 0;
    }
    return g_cpu_load[cpu];
}

os_err_t os_smp_get_stats(os_cpu_t cpu, os_smp_stats_t *stats) {
    if (cpu >= OS_CFG_CPU_COUNT || !stats) {
        return OS_EINVAL;
    }
    
    *stats = g_smp_stats[cpu];
    return OS_EOK;
}

bool os_smp_cpu_online(os_cpu_t cpu) {
    if (cpu >= OS_CFG_CPU_COUNT) {
        return false;
    }
    return (g_cpu_online_mask & (1U << cpu)) != 0;
}

os_cpu_t os_smp_online_count(void) {
    os_cpu_t count = 0;
    uint32_t mask = g_cpu_online_mask;
    
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    
    return count;
}

void os_smp_cpu_pin(void) {
    os_tcb_t *tcb = os_task_self();
    if (tcb) {
        tcb->cpu_lock++;
    }
}

void os_smp_cpu_unpin(void) {
    os_tcb_t *tcb = os_task_self();
    if (tcb && tcb->cpu_lock > 0) {
        tcb->cpu_lock--;
    }
}

os_err_t os_smp_call_on_cpu(os_cpu_t cpu, os_smp_call_func_t func, 
                            void *arg, bool wait) {
    if (cpu >= OS_CFG_CPU_COUNT || !func) {
        return OS_EINVAL;
    }
    
    if (!(g_cpu_online_mask & (1U << cpu))) {
        return OS_ENODEV;
    }
    
    /* If target is self, just call directly */
    if (cpu == os_cpu_id()) {
        func(arg);
        return OS_EOK;
    }

    if (!smp_runtime_ipi_enabled()) {
        return OS_ENOSYS;
    }
    
    os_smp_call_t *call = &g_smp_call[cpu];
    
    os_spinlock_lock(&call->lock);
    
    /* Wait for previous call to complete */
    while (!call->done) {
        os_spinlock_unlock(&call->lock);
        os_task_yield();
        os_spinlock_lock(&call->lock);
    }
    
    /* Setup call */
    call->func = func;
    call->arg = arg;
    call->done = false;
    
    os_spinlock_unlock(&call->lock);
    
    /* Send IPI */
    os_ipi_send(cpu, OS_IPI_CALL_FUNC);
    
    /* Wait for completion if requested */
    if (wait) {
        while (!call->done) {
            os_task_yield();
        }
    }
    
    return OS_EOK;
}

#endif /* OS_CFG_SMP_EN */
