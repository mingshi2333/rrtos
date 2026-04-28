#include "os_kernel.h"
#include "os_kernel_test.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"
#include "hal_clint.h"

#if OS_CFG_SMP_EN
#include "os_smp.h"
#define OS_SCHED_CPU_SLOTS OS_CFG_CPU_COUNT
#else
#define OS_SCHED_CPU_SLOTS 1
#endif

extern void os_context_switch(void **from_sp, void **to_sp);
extern void os_context_switch_first(void **to_sp);
extern void os_heap_init(void);

static os_spinlock_t g_sched_lock = OS_SPINLOCK_INIT;
static os_list_t g_ready_list[OS_CFG_PRIO_MAX];
static uint8_t g_ready_bitmap[(OS_CFG_PRIO_MAX + 7) / 8];
static os_tcb_t *g_current_task[OS_SCHED_CPU_SLOTS];
static volatile os_tick_t g_tick_count;
static os_tcb_t g_idle_tcb[OS_SCHED_CPU_SLOTS];
static uint8_t g_idle_stack[OS_SCHED_CPU_SLOTS][OS_CFG_IDLE_STACK_SIZE];
static volatile bool g_kernel_running = false;
static os_tcb_t *g_task_table[OS_CFG_TASK_MAX];
static uint32_t g_task_count;
static volatile bool g_sched_pending[OS_SCHED_CPU_SLOTS];

#if OS_CFG_SMP_EN
static os_cpu_data_t g_cpu_data[OS_CFG_CPU_MAX];
#else
static volatile uint32_t g_irq_nest[OS_SCHED_CPU_SLOTS];
static volatile uint32_t g_sched_lock_nest[OS_SCHED_CPU_SLOTS];
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
static void task_ready_remove(os_tcb_t *tcb);
void os_sched_wake_task(os_tcb_t *task, os_err_t result);

#if OS_CFG_SMP_EN
static bool task_affinity_matches_cpu(const os_tcb_t *tcb, os_cpu_t cpu) {
    return tcb->affinity == OS_CPU_ANY || tcb->affinity == cpu;
}

static bool task_is_schedulable_on_cpu(const os_tcb_t *tcb, os_cpu_t cpu) {
    if (!tcb) {
        return false;
    }

    if (!task_affinity_matches_cpu(tcb, cpu)) {
        return false;
    }

    if (tcb->state == OS_TASK_RUNNING && tcb->cpu_id != cpu) {
        return false;
    }

    return tcb->state == OS_TASK_READY ||
           (tcb->state == OS_TASK_RUNNING && tcb->cpu_id == cpu);
}

static bool ready_list_has_peer_for_cpu(os_prio_t prio, os_cpu_t cpu, const os_tcb_t *exclude) {
    os_tcb_t *iter = g_ready_list[prio].head;
    while (iter) {
        if (iter != exclude && task_is_schedulable_on_cpu(iter, cpu)) {
            return true;
        }
        iter = iter->next;
    }

    return false;
}
#endif

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
        case OS_PENDING_QUEUE_SEND: {
            os_queue_t *q = (os_queue_t *)task->pending_ipc;
            if (q) {
                os_list_remove(&q->send_wait, task);
            }
            break;
        }
        case OS_PENDING_EVENT: {
            os_event_t *event = (os_event_t *)task->pending_ipc;
            if (event) {
                os_list_remove(&event->wait_list, task);
            }
            break;
        }
        default:
            break;
    }

    os_sched_wake_task(task, OS_ETIMEOUT);
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

static uint32_t sched_irq_nest(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    return g_cpu_data[cpu].irq_nest;
#else
    return g_irq_nest[cpu];
#endif
}

static void sched_irq_inc(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].irq_nest++;
#else
    g_irq_nest[cpu]++;
#endif
}

static void sched_irq_dec(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].irq_nest--;
#else
    g_irq_nest[cpu]--;
#endif
}

static uint32_t sched_lock_nest(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    return g_cpu_data[cpu].sched_lock;
#else
    return g_sched_lock_nest[cpu];
#endif
}

static void sched_lock_inc(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].sched_lock++;
#else
    g_sched_lock_nest[cpu]++;
#endif
}

static void sched_lock_dec(os_cpu_t cpu) {
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].sched_lock--;
#else
    g_sched_lock_nest[cpu]--;
#endif
}

static bool sched_deferred(os_cpu_t cpu) {
    return sched_lock_nest(cpu) > 0 || sched_irq_nest(cpu) > 0;
}

#if OS_CFG_STACK_CHECK_EN
static void stack_guard_write(void *stack_base) {
    uint32_t pattern = (uint32_t)OS_CFG_STACK_GUARD_PATTERN;
    uint8_t *dst = (uint8_t *)stack_base;
    const uint8_t *src = (const uint8_t *)&pattern;

    for (uint32_t i = 0; i < sizeof(pattern); i++) {
        dst[i] = src[i];
    }
}

static bool stack_guard_matches(const void *stack_base) {
    uint32_t pattern = (uint32_t)OS_CFG_STACK_GUARD_PATTERN;
    uint32_t observed = 0;
    uint8_t *dst = (uint8_t *)&observed;
    const uint8_t *src = (const uint8_t *)stack_base;

    for (uint32_t i = 0; i < sizeof(observed); i++) {
        dst[i] = src[i];
    }

    return observed == pattern;
}

static bool task_stack_guard_ok(const os_tcb_t *tcb) {
    if (!tcb || !tcb->stack_base || tcb->stack_size < sizeof(uint32_t)) {
        return false;
    }

    return stack_guard_matches(tcb->stack_base);
}

static void task_stack_guard_fail(os_tcb_t *tcb) {
    if (!tcb) {
        return;
    }

    tcb->state = OS_TASK_TERMINATED;
    tcb->next = NULL;
    tcb->prev = NULL;
}
#endif

static void task_ready_add(os_tcb_t *tcb) {
    os_list_append(&g_ready_list[tcb->priority], tcb);
    bitmap_set(g_ready_bitmap, tcb->priority);
    tcb->state = OS_TASK_READY;
}

void os_sched_wake_task(os_tcb_t *task, os_err_t result) {
    if (!task) {
        return;
    }

    task->pending_result = result;
    task->pending_obj = NULL;
    task->pending_ipc = NULL;
    task->pending_type = OS_PENDING_NONE;
    task->pending_flags = 0;
    task->pending_options = 0;
    task->pending_deadline = 0;
    task_ready_add(task);
}

#ifdef OS_TEST_HARNESS
static bool task_in_ready_queue(const os_tcb_t *tcb) {
    if (!tcb) {
        return false;
    }

    for (uint32_t prio = 0; prio < OS_CFG_PRIO_MAX; prio++) {
        os_tcb_t *iter = g_ready_list[prio].head;
        while (iter) {
            if (iter == tcb) {
                return true;
            }
            iter = iter->next;
        }
    }

    return false;
}
#endif

static void task_ready_remove(os_tcb_t *tcb) {
    os_list_remove(&g_ready_list[tcb->priority], tcb);
    if (g_ready_list[tcb->priority].count == 0) {
        bitmap_clear(g_ready_bitmap, tcb->priority);
    }
}

static os_tcb_t *sched_highest_ready(os_cpu_t cpu) {
    int prio = bitmap_ffs(g_ready_bitmap, OS_CFG_PRIO_MAX);

    while (prio >= 0) {
        os_tcb_t *iter = g_ready_list[prio].head;
        while (iter) {
#if OS_CFG_SMP_EN
            if (task_is_schedulable_on_cpu(iter, cpu)) {
#if OS_CFG_STACK_CHECK_EN
                if (!task_stack_guard_ok(iter)) {
                    os_tcb_t *failed = iter;
                    iter = iter->next;
                    task_ready_remove(failed);
                    task_stack_guard_fail(failed);
                    continue;
                }
#endif
                return iter;
            }
#else
#if OS_CFG_STACK_CHECK_EN
            if (!task_stack_guard_ok(iter)) {
                os_tcb_t *failed = iter;
                iter = iter->next;
                task_ready_remove(failed);
                task_stack_guard_fail(failed);
                continue;
            }
#endif
            return iter;
#endif
            iter = iter->next;
        }

        prio++;
        while ((uint32_t)prio < OS_CFG_PRIO_MAX) {
            if (g_ready_bitmap[prio >> 3] & (1U << (prio & 7))) {
                break;
            }
            prio++;
        }
        if ((uint32_t)prio >= OS_CFG_PRIO_MAX) {
            break;
        }
    }

    return &g_idle_tcb[cpu];
}

static void *stack_init(void *stack_base, os_size_t size, os_task_entry_t entry, void *arg) {
    uint8_t *sp = (uint8_t *)stack_base + size;
    
    sp = (uint8_t *)((os_ubase_t)sp & ~0xF);
    
    #if OS_CFG_ARCH_BITS == 64
    sp -= 128;
#else
    sp -= 64;
#endif
    
    os_reg_t *frame = (os_reg_t *)sp;
    
    for (int i = 0; i < 15; i++) {
        frame[i] = 0;
    }
    
    frame[0] = (os_reg_t)entry;
    frame[13] = MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_MIE | MSTATUS_FS_INITIAL | MSTATUS_VS_INITIAL;
    frame[14] = (os_reg_t)arg;
    
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

    g_kernel_running = false;
    for (os_cpu_t i = 0; i < OS_SCHED_CPU_SLOTS; i++) {
        g_current_task[i] = NULL;
        g_sched_pending[i] = false;
#if !OS_CFG_SMP_EN
        g_irq_nest[i] = 0;
        g_sched_lock_nest[i] = 0;
#endif
    }
    
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

    os_timer_subsys_init();
    
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
    
    os_tcb_t *first = sched_highest_ready(cpu);
    if (first != &g_idle_tcb[cpu]) {
        task_ready_remove(first);
    }
    first->state = OS_TASK_RUNNING;
    g_current_task[cpu] = first;
    
#if OS_CFG_SMP_EN
    g_cpu_data[cpu].current = first;
    first->cpu_id = cpu;
#endif
    
    g_kernel_running = true;
    
    uint64_t next_tick = hal_clint_mtime_get() + (OS_CFG_TIMER_FREQ_HZ / OS_CFG_TICK_FREQ_HZ);
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
            if (g_ready_list[current->priority].count > 0
#if OS_CFG_SMP_EN
                && ready_list_has_peer_for_cpu(current->priority, cpu, current)
#endif
            ) {
                current->state = OS_TASK_READY;
                task_ready_add(current);
            }
        }
    }
#endif

    os_spinlock_irq_restore(&g_sched_lock, flags);

    os_timer_tick();

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
    tcb->pending_flags = 0;
    tcb->pending_options = 0;
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
    stack_guard_write(stack);
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
    bool should_resched = false;
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    if (current && current->state == OS_TASK_RUNNING
        && g_ready_list[current->priority].count > 0
#if OS_CFG_SMP_EN
        && ready_list_has_peer_for_cpu(current->priority, cpu, current)
#endif
    ) {
        current->state = OS_TASK_READY;
        task_ready_add(current);
        should_resched = true;
    }
    
    os_spinlock_irq_restore(&g_sched_lock, flags);
    
    if (should_resched) {
        os_sched();
    }
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

os_err_t os_task_set_prio(os_tcb_t *tcb, os_prio_t prio) {
    bool need_resched = false;
    os_prio_t old_prio;

    if (!tcb || (uint32_t)prio >= OS_CFG_PRIO_MAX) {
        return OS_EINVAL;
    }

    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);

    old_prio = tcb->priority;
    if (old_prio == prio && tcb->base_prio == prio) {
        os_spinlock_irq_restore(&g_sched_lock, flags);
        return OS_EOK;
    }

    if (tcb->state == OS_TASK_READY) {
        task_ready_remove(tcb);
    }

    if (tcb->priority == tcb->base_prio || prio < tcb->priority) {
        tcb->priority = prio;
    }
    tcb->base_prio = prio;

    if (tcb->state == OS_TASK_READY) {
        task_ready_add(tcb);
    }

    need_resched = g_kernel_running;
    os_spinlock_irq_restore(&g_sched_lock, flags);

    if (need_resched) {
        os_sched();
    }

    return OS_EOK;
}

os_tcb_t *os_task_self(void) {
    return g_current_task[os_cpu_id()];
}

void os_sched_lock(void) {
    os_cpu_t cpu = os_cpu_id();
    sched_lock_inc(cpu);
}

void os_sched_unlock(void) {
    os_cpu_t cpu = os_cpu_id();
    if (sched_lock_nest(cpu) > 0) {
        sched_lock_dec(cpu);
        if (sched_lock_nest(cpu) == 0 && g_sched_pending[cpu]) {
            os_sched();
        }
    }
}

void os_irq_enter(void) {
    os_cpu_t cpu = os_cpu_id();
    sched_irq_inc(cpu);
}

void os_irq_exit(void) {
    os_cpu_t cpu = os_cpu_id();

    if (sched_irq_nest(cpu) == 0) {
        return;
    }

    sched_irq_dec(cpu);
    if (!sched_deferred(cpu) && g_sched_pending[cpu]) {
        os_sched();
    }
}

void os_sched(void) {
    if (!g_kernel_running) {
        return;
    }
    
    os_cpu_t cpu = os_cpu_id();
    
    if (sched_deferred(cpu)) {
        g_sched_pending[cpu] = true;
        return;
    }
    g_sched_pending[cpu] = false;
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    
    os_tcb_t *current = g_current_task[cpu];
#if OS_CFG_STACK_CHECK_EN
    if (current && current->state == OS_TASK_RUNNING && !task_stack_guard_ok(current)) {
        task_stack_guard_fail(current);
    }
#endif
    os_tcb_t *next = sched_highest_ready(cpu);
    
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

#ifdef OS_TEST_HARNESS
void os_test_bind_current_task(os_tcb_t *task) {
    os_cpu_t cpu = os_cpu_id();

    g_current_task[cpu] = task;
    if (!task) {
        return;
    }

    if (task->state == OS_TASK_READY && task_in_ready_queue(task)) {
        task_ready_remove(task);
    }
    task->state = OS_TASK_RUNNING;
}

bool os_test_task_in_ready_queue(const os_tcb_t *task) {
    return task_in_ready_queue(task);
}

uint32_t os_test_ready_count(os_prio_t prio) {
    if ((uint32_t)prio >= OS_CFG_PRIO_MAX) {
        return 0;
    }

    return g_ready_list[prio].count;
}
#endif

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

    if (affinity != OS_CPU_ANY && affinity >= OS_CFG_CPU_COUNT) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&g_sched_lock);
    tcb->affinity = affinity;
    os_spinlock_irq_restore(&g_sched_lock, flags);

    if (g_kernel_running) {
        if (tcb->state == OS_TASK_RUNNING && affinity != OS_CPU_ANY && tcb->cpu_id != affinity) {
            os_ipi_send(tcb->cpu_id, OS_IPI_RESCHEDULE);
        }
        os_sched();
    }
    
    return OS_EOK;
}
#endif
