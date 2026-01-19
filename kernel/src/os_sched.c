#include "os_kernel.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"
#include "hal_clint.h"

extern void os_context_switch(void **from_sp, void **to_sp);
extern void os_context_switch_first(void **to_sp);
extern void os_heap_init(void);

static os_spinlock_t g_sched_lock = OS_SPINLOCK_INIT;
static os_list_t g_ready_list[OS_CFG_PRIO_MAX];
static uint8_t g_ready_bitmap[(OS_CFG_PRIO_MAX + 7) / 8];
static os_tcb_t *g_current_task[OS_CFG_CPU_MAX];
static volatile os_tick_t g_tick_count;
static os_tcb_t g_idle_tcb[OS_CFG_CPU_MAX];
static uint8_t g_idle_stack[OS_CFG_CPU_MAX][OS_CFG_IDLE_STACK_SIZE];
static volatile bool g_kernel_running = false;
static os_tcb_t *g_task_table[OS_CFG_TASK_MAX];
static uint32_t g_task_count;

#if OS_CFG_SMP_EN
static os_cpu_data_t g_cpu_data[OS_CFG_CPU_MAX];
#endif

static void os_list_init(os_list_t *list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

static void os_list_append(os_list_t *list, os_tcb_t *tcb) {
    tcb->next = NULL;
    tcb->prev = list->tail;
    if (list->tail) {
        list->tail->next = tcb;
    } else {
        list->head = tcb;
    }
    list->tail = tcb;
    list->count++;
}

static void os_list_remove(os_list_t *list, os_tcb_t *tcb) {
    if (tcb->prev) {
        tcb->prev->next = tcb->next;
    } else {
        list->head = tcb->next;
    }
    if (tcb->next) {
        tcb->next->prev = tcb->prev;
    } else {
        list->tail = tcb->prev;
    }
    tcb->next = NULL;
    tcb->prev = NULL;
    list->count--;
}

static void task_ready_add(os_tcb_t *tcb);

static void task_timeout_wake(os_tcb_t *task) {
    switch (task->pending_type) {
        case OS_PENDING_SEM: {
            os_sem_t *sem = (os_sem_t *)task->pending_ipc;
            if (sem) {
                os_list_remove(&sem->wait_list, task);
            }
            break;
        }
        case OS_PENDING_MUTEX: {
            os_mutex_t *mutex = (os_mutex_t *)task->pending_ipc;
            if (mutex) {
                os_list_remove(&mutex->wait_list, task);
            }
            break;
        }
        case OS_PENDING_QUEUE_RECV: {
            os_queue_t *q = (os_queue_t *)task->pending_ipc;
            if (q) {
                os_list_remove(&q->recv_wait, task);
            }
            break;
        }
        default:
            break;
    }

    task->pending_result = OS_ETIMEOUT;
    task->pending_obj = NULL;
    task->pending_ipc = NULL;
    task->pending_type = OS_PENDING_NONE;
    task->pending_deadline = 0;
    task_ready_add(task);
}

static void bitmap_set(uint8_t *bitmap, uint32_t bit) {
    bitmap[bit >> 3] |= (1 << (bit & 7));
}

static void bitmap_clear(uint8_t *bitmap, uint32_t bit) {
    bitmap[bit >> 3] &= ~(1 << (bit & 7));
}

static int bitmap_ffs(uint8_t *bitmap, uint32_t max) {
    for (uint32_t i = 0; i < (max + 7) / 8; i++) {
        if (bitmap[i]) {
            for (uint32_t j = 0; j < 8; j++) {
                if (bitmap[i] & (1 << j)) {
                    uint32_t bit = i * 8 + j;
                    return (bit < max) ? (int)bit : -1;
                }
            }
        }
    }
    return -1;
}

static void task_ready_add(os_tcb_t *tcb) {
    os_list_append(&g_ready_list[tcb->priority], tcb);
    bitmap_set(g_ready_bitmap, tcb->priority);
    tcb->state = OS_TASK_READY;
}

static void task_ready_remove(os_tcb_t *tcb) {
    os_list_remove(&g_ready_list[tcb->priority], tcb);
    if (g_ready_list[tcb->priority].count == 0) {
        bitmap_clear(g_ready_bitmap, tcb->priority);
    }
}

static os_tcb_t *sched_highest_ready(void) {
    int prio = bitmap_ffs(g_ready_bitmap, OS_CFG_PRIO_MAX);
    if (prio >= 0 && g_ready_list[prio].head) {
        return g_ready_list[prio].head;
    }
    os_cpu_t cpu = os_cpu_id();
    return &g_idle_tcb[cpu];
}

static void *stack_init(void *stack_base, os_size_t size, os_task_entry_t entry, void *arg) {
    (void)arg;
    uint8_t *sp = (uint8_t *)stack_base + size;
    
    sp = (uint8_t *)((os_ubase_t)sp & ~0xF);
    
    sp -= 128;
    
    os_reg_t *frame = (os_reg_t *)sp;
    
    for (int i = 0; i < 14; i++) {
        frame[i] = 0;
    }
    
    frame[0] = (os_reg_t)entry;
    frame[13] = MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_MIE | MSTATUS_FS_INITIAL;
    
    return sp;
}

static void idle_task_entry(void *arg) {
    (void)arg;
    while (1) {
        os_wfi();
    }
}

void os_kernel_init(void) {
    os_irq_disable();
    
    os_heap_init(); /* Initialize heap allocator */

    for (uint32_t i = 0; i < OS_CFG_PRIO_MAX; i++) {
        os_list_init(&g_ready_list[i]);
    }
    
    for (uint32_t i = 0; i < sizeof(g_ready_bitmap); i++) {
        g_ready_bitmap[i] = 0;
    }
    
    g_tick_count = 0;
    g_task_count = 0;
    for (uint32_t i = 0; i < OS_CFG_TASK_MAX; i++) {
        g_task_table[i] = NULL;
    }
    
#if OS_CFG_SMP_EN
    for (os_cpu_t i = 0; i < OS_CFG_CPU_MAX; i++) {
        g_cpu_data[i].current = NULL;
        g_cpu_data[i].idle_task = &g_idle_tcb[i];
        g_cpu_data[i].irq_nest = 0;
        g_cpu_data[i].sched_lock = 0;
        os_list_init(&g_cpu_data[i].local_ready);
        os_spinlock_init(&g_cpu_data[i].lock);
    }
#endif
    
    for (os_cpu_t i = 0; i < OS_CFG_CPU_COUNT; i++) {
        os_task_create(&g_idle_tcb[i], "idle", idle_task_entry, NULL,
                       OS_PRIO_IDLE, g_idle_stack[i], OS_CFG_IDLE_STACK_SIZE);
        task_ready_remove(&g_idle_tcb[i]);
#if OS_CFG_SMP_EN
        g_idle_tcb[i].affinity = i;
#endif
    }
}

void os_kernel_start(void) {
    os_cpu_t cpu = os_cpu_id();
    
    os_tcb_t *first = sched_highest_ready();
    if (first != &g_idle_tcb[cpu]) {
        task_ready_remove(first);
    }
    first->state = OS_TASK_RUNNING;
    g_current_task[cpu] = first;
    
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].current = first;
#endif
    
    g_kernel_running = true;
    
    uint64_t next_tick = hal_clint_mtime_get() + (OS_CFG_CPU_FREQ_HZ / OS_CFG_TICK_FREQ_HZ);
    hal_clint_mtimecmp_set(cpu, next_tick);
    
    csr_set(mie, MIE_MTIE | MIE_MSIE);
    csr_set(mstatus, MSTATUS_MIE);
    
    os_context_switch_first(&first->sp);
}

os_tick_t os_tick_get(void) {
    return g_tick_count;
}

void os_tick_handler(void) {
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    g_tick_count++;
    
    for (uint32_t i = 0; i < g_task_count; i++) {
        os_tcb_t *task = g_task_table[i];
        if (!task) {
            continue;
        }
        if (task->state == OS_TASK_SLEEPING) {
            if ((os_tick_t)(g_tick_count - task->wake_tick) < 0x80000000U) {
                task->state = OS_TASK_READY;
                task_ready_add(task);
            }
            continue;
        }
        if (task->state == OS_TASK_BLOCKED && task->pending_deadline != 0) {
            if ((os_tick_t)(g_tick_count - task->pending_deadline) < 0x80000000U) {
                task_timeout_wake(task);
            }
        }
    }
    
    os_cpu_t cpu = os_cpu_id();
    os_tcb_t *current = g_current_task[cpu];
    
#if OS_CFG_TIME_SLICE_EN
    if (current && current->time_slice > 0) {
        current->time_slice--;
        if (current->time_slice == 0) {
            current->time_slice = OS_CFG_TIME_SLICE_TICKS;
            if (g_ready_list[current->priority].count > 0) {
                task_ready_remove(current);
                task_ready_add(current);
            }
        }
    }
#endif
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    os_sched();
}

os_err_t os_task_create(os_tcb_t *tcb, const char *name, os_task_entry_t entry,
                        void *arg, os_prio_t prio, void *stack, os_size_t stack_size) {
    if (!tcb || !entry || !stack || stack_size < OS_CFG_STACK_SIZE_MIN) {
        return OS_EINVAL;
    }
    
    if ((uint32_t)prio >= OS_CFG_PRIO_MAX) {
        return OS_EINVAL;
    }
    
    tcb->sp = stack_init(stack, stack_size, entry, arg);
    tcb->stack_base = stack;
    tcb->stack_size = stack_size;
    tcb->name = name;
    tcb->entry = entry;
    tcb->arg = arg;
    tcb->priority = prio;
    tcb->base_prio = prio;
    tcb->state = OS_TASK_INIT;
    tcb->time_slice = OS_CFG_TIME_SLICE_TICKS;
    tcb->next = NULL;
    tcb->prev = NULL;
    tcb->pending_obj = NULL;
    tcb->pending_ipc = NULL;
    tcb->pending_type = OS_PENDING_NONE;
    tcb->pending_deadline = 0;
    
    if (g_task_count < OS_CFG_TASK_MAX) {
        g_task_table[g_task_count++] = tcb;
    }
    
#if OS_CFG_SMP_EN
    tcb->affinity = OS_CPU_ANY;
    tcb->cpu_lock = 0;
#endif
    
#if OS_CFG_STATS_EN
    tcb->run_count = 0;
    tcb->run_ticks = 0;
    tcb->stack_peak = 0;
#endif
    
#if OS_CFG_STACK_CHECK_EN
    uint32_t *guard = (uint32_t *)stack;
    *guard = OS_CFG_STACK_GUARD_PATTERN;
#endif
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    task_ready_add(tcb);
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    if (g_kernel_running) {
        os_sched();
    }
    
    return OS_EOK;
}

os_err_t os_task_delete(os_tcb_t *tcb) {
    if (!tcb) {
        tcb = g_current_task[os_cpu_id()];
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (tcb->state == OS_TASK_READY) {
        task_ready_remove(tcb);
    }
    tcb->state = OS_TASK_TERMINATED;
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    if (tcb == g_current_task[os_cpu_id()]) {
        os_sched();
    }
    
    return OS_EOK;
}

os_err_t os_task_suspend(os_tcb_t *tcb) {
    if (!tcb) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (tcb->state == OS_TASK_READY) {
        task_ready_remove(tcb);
    }
    tcb->state = OS_TASK_SUSPENDED;
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    if (tcb == g_current_task[os_cpu_id()]) {
        os_sched();
    }
    
    return OS_EOK;
}

os_err_t os_task_resume(os_tcb_t *tcb) {
    if (!tcb) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (tcb->state == OS_TASK_SUSPENDED) {
        task_ready_add(tcb);
    }
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    os_sched();
    
    return OS_EOK;
}

void os_task_yield(void) {
    os_cpu_t cpu = os_cpu_id();
    os_tcb_t *current = g_current_task[cpu];
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (current && current->state == OS_TASK_RUNNING) {
        current->state = OS_TASK_READY;
        task_ready_add(current);
    }
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    os_sched();
}

os_err_t os_task_delay(os_tick_t ticks) {
    if (ticks == 0) {
        os_task_yield();
        return OS_EOK;
    }
    
    os_cpu_t cpu = os_cpu_id();
    os_tcb_t *current = g_current_task[cpu];
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (current->state == OS_TASK_READY) {
        task_ready_remove(current);
    }
    
    current->wake_tick = g_tick_count + ticks;
    current->state = OS_TASK_SLEEPING;
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    os_sched();
    
    return OS_EOK;
}

os_tcb_t *os_task_self(void) {
    return g_current_task[os_cpu_id()];
}

void os_sched_lock(void) {
#if OS_CFG_SMP_EN
    os_cpu_t cpu = os_cpu_id();
    g_cpu_data[cpu].sched_lock++;
#endif
}

void os_sched_unlock(void) {
#if OS_CFG_SMP_EN
    os_cpu_t cpu = os_cpu_id();
    if (g_cpu_data[cpu].sched_lock > 0) {
        g_cpu_data[cpu].sched_lock--;
        if (g_cpu_data[cpu].sched_lock == 0) {
            os_sched();
        }
    }
#endif
}

void os_sched(void) {
    if (!g_kernel_running) {
        return;
    }
    
    os_cpu_t cpu = os_cpu_id();
    
#if OS_CFG_SMP_EN
    if (g_cpu_data[cpu].sched_lock > 0 || g_cpu_data[cpu].irq_nest > 0) {
        return;
    }
#endif
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    os_tcb_t *current = g_current_task[cpu];
    os_tcb_t *next = sched_highest_ready();
    
    if (next != current && next != &g_idle_tcb[cpu]) {
        task_ready_remove(next);
    }
    
    if (current == next) {
        os_spinlock_irq_restore(&g_sched_lock, flags);
        return;
    }
    
    if (current && current->state == OS_TASK_RUNNING) {
        current->state = OS_TASK_READY;
        task_ready_add(current);
    }
    
    next->state = OS_TASK_RUNNING;
    g_current_task[cpu] = next;
    
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].current = next;
    next->cpu_id = cpu;
#endif
    
#if OS_CFG_STATS_EN
    next->run_count++;
#endif
    
    os_spinlock_unlock(&g_sched_lock);
    
    os_context_switch(&current->sp, &next->sp);
    
    os_irq_restore(flags);
}

#if OS_CFG_SMP_EN
os_cpu_t os_cpu_count(void) {
    return OS_CFG_CPU_COUNT;
}

os_cpu_data_t *os_cpu_data(os_cpu_t cpu) {
    if (cpu >= OS_CFG_CPU_MAX) {
        return NULL;
    }
    return &g_cpu_data[cpu];
}

os_err_t os_task_set_affinity(os_tcb_t *tcb, os_cpu_t affinity) {
    if (!tcb) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    tcb->affinity = affinity;
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    return OS_EOK;
}
#endif
