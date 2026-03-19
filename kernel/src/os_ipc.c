#include "os_kernel.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"

extern void os_sched(void);
extern os_tcb_t *os_task_self(void);
extern void os_sched_wake_task(os_tcb_t *task, os_err_t result);

static bool os_event_wait_satisfied(const os_event_t *event, uint32_t flags, uint32_t opt,
                                    uint32_t *actual) {
    uint32_t matched = event->flags & flags;

    if ((opt & OS_EVENT_WAIT_ALL) == OS_EVENT_WAIT_ALL) {
        if (matched != flags) {
            return false;
        }
        if (actual) {
            *actual = flags;
        }
        return true;
    }

    if (matched == 0) {
        return false;
    }

    if (actual) {
        *actual = matched;
    }
    return true;
}

static void os_ipc_wait_list_append(os_list_t *list, os_tcb_t *task) {
    if (list->tail) {
        list->tail->next = task;
    } else {
        list->head = task;
    }

    task->prev = list->tail;
    task->next = NULL;
    list->tail = task;
    list->count++;
}

static os_tcb_t *os_ipc_wait_list_pop_head(os_list_t *list) {
    os_tcb_t *task = list->head;

    if (!task) {
        return NULL;
    }

    list->head = task->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    task->next = NULL;
    task->prev = NULL;
    list->count--;
    return task;
}

static void os_ipc_wait_list_remove(os_list_t *list, os_tcb_t *task) {
    if (task->prev) {
        task->prev->next = task->next;
    } else {
        list->head = task->next;
    }

    if (task->next) {
        task->next->prev = task->prev;
    } else {
        list->tail = task->prev;
    }

    task->next = NULL;
    task->prev = NULL;
    list->count--;
}

static void os_queue_copy_message(void *dst_msg, const void *src_msg, os_size_t msg_size) {
    uint8_t *dst = (uint8_t *)dst_msg;
    const uint8_t *src = (const uint8_t *)src_msg;

    for (os_size_t i = 0; i < msg_size; i++) {
        dst[i] = src[i];
    }
}

static void os_queue_push_buffered(os_queue_t *q, const void *msg) {
    uint8_t *dst = (uint8_t *)q->buffer + (q->tail * q->msg_size);
    os_queue_copy_message(dst, msg, q->msg_size);
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
}

os_err_t os_sem_init(os_sem_t *sem, const char *name, int32_t init, int32_t max) {
    if (!sem || init < 0 || max <= 0 || init > max) {
        return OS_EINVAL;
    }
    
    sem->count = init;
    sem->max_count = max;
    sem->wait_list.head = NULL;
    sem->wait_list.tail = NULL;
    sem->wait_list.count = 0;
    os_spinlock_init(&sem->lock);
    sem->name = name;
    
    return OS_EOK;
}

os_err_t os_sem_take(os_sem_t *sem, os_tick_t timeout) {
    if (!sem) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&sem->lock);
    
    if (sem->count > 0) {
        sem->count--;
        os_spinlock_irq_restore(&sem->lock, flags);
        return OS_EOK;
    }
    
    if (timeout == OS_NO_WAIT) {
        os_spinlock_irq_restore(&sem->lock, flags);
        return OS_ETIMEOUT;
    }
    
    os_tcb_t *task = os_task_self();
    task->pending_obj = sem;
    task->pending_ipc = sem;
    task->pending_type = OS_PENDING_SEM;
    task->pending_deadline = (timeout == OS_WAIT_FOREVER) ? 0 : (os_tick_get() + timeout);
    task->pending_result = OS_ETIMEOUT;
    task->state = OS_TASK_BLOCKED;
    
    os_ipc_wait_list_append(&sem->wait_list, task);
    
    os_spinlock_irq_restore(&sem->lock, flags);
    os_sched();
    
    return task->pending_result;
}

os_err_t os_sem_give(os_sem_t *sem) {
    if (!sem) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&sem->lock);
    
    if (sem->wait_list.head) {
        os_tcb_t *task = os_ipc_wait_list_pop_head(&sem->wait_list);
        os_sched_wake_task(task, OS_EOK);
        
        os_spinlock_irq_restore(&sem->lock, flags);
        os_sched();
        return OS_EOK;
    }
    
    if (sem->count < sem->max_count) {
        sem->count++;
        os_spinlock_irq_restore(&sem->lock, flags);
        return OS_EOK;
    }
    
    os_spinlock_irq_restore(&sem->lock, flags);
    return OS_EFULL;
}

os_err_t os_mutex_init(os_mutex_t *mutex, const char *name) {
    if (!mutex) {
        return OS_EINVAL;
    }
    
    mutex->locked = 0;
    mutex->nest_count = 0;
    mutex->owner = NULL;
    mutex->original_prio = 0;
    mutex->wait_list.head = NULL;
    mutex->wait_list.tail = NULL;
    mutex->wait_list.count = 0;
    os_spinlock_init(&mutex->lock);
    mutex->name = name;
    
    return OS_EOK;
}

os_err_t os_mutex_lock(os_mutex_t *mutex, os_tick_t timeout) {
    if (!mutex) {
        return OS_EINVAL;
    }
    
    os_tcb_t *task = os_task_self();
    os_reg_t flags = os_spinlock_irq_save(&mutex->lock);
    
    if (!mutex->locked) {
        mutex->locked = 1;
        mutex->owner = task;
        mutex->nest_count = 1;
        mutex->original_prio = task->priority;
        os_spinlock_irq_restore(&mutex->lock, flags);
        return OS_EOK;
    }
    
    if (mutex->owner == task) {
        mutex->nest_count++;
        os_spinlock_irq_restore(&mutex->lock, flags);
        return OS_EOK;
    }
    
    if (timeout == OS_NO_WAIT) {
        os_spinlock_irq_restore(&mutex->lock, flags);
        return OS_EBUSY;
    }
    
#if OS_CFG_MUTEX_PRIO_INHERIT
    if (task->priority < mutex->owner->priority) {
        mutex->owner->priority = task->priority;
    }
#endif
    
    task->pending_obj = mutex;
    task->pending_ipc = mutex;
    task->pending_type = OS_PENDING_MUTEX;
    task->pending_deadline = (timeout == OS_WAIT_FOREVER) ? 0 : (os_tick_get() + timeout);
    task->pending_result = OS_ETIMEOUT;
    task->state = OS_TASK_BLOCKED;
    
    os_ipc_wait_list_append(&mutex->wait_list, task);
    
    os_spinlock_irq_restore(&mutex->lock, flags);
    os_sched();
    
    return task->pending_result;
}

os_err_t os_mutex_unlock(os_mutex_t *mutex) {
    if (!mutex) {
        return OS_EINVAL;
    }
    
    os_tcb_t *task = os_task_self();
    os_reg_t flags = os_spinlock_irq_save(&mutex->lock);
    
    if (mutex->owner != task) {
        os_spinlock_irq_restore(&mutex->lock, flags);
        return OS_EPERM;
    }
    
    mutex->nest_count--;
    if (mutex->nest_count > 0) {
        os_spinlock_irq_restore(&mutex->lock, flags);
        return OS_EOK;
    }
    
#if OS_CFG_MUTEX_PRIO_INHERIT
    task->priority = mutex->original_prio;
#endif
    
    if (mutex->wait_list.head) {
        os_tcb_t *waiter = os_ipc_wait_list_pop_head(&mutex->wait_list);
        
        mutex->owner = waiter;
        mutex->locked = 1;
        mutex->nest_count = 1;
        mutex->original_prio = waiter->priority;
        
        os_sched_wake_task(waiter, OS_EOK);
        
        os_spinlock_irq_restore(&mutex->lock, flags);
        os_sched();
        return OS_EOK;
    }
    
    mutex->locked = 0;
    mutex->owner = NULL;
    
    os_spinlock_irq_restore(&mutex->lock, flags);
    return OS_EOK;
}

os_err_t os_queue_init(os_queue_t *q, const char *name, void *buf,
                       os_size_t msg_size, os_size_t capacity) {
    if (!q || !buf || msg_size == 0 || capacity == 0) {
        return OS_EINVAL;
    }
    
    q->buffer = buf;
    q->msg_size = msg_size;
    q->capacity = capacity;
    q->count = 0;
    q->head = 0;
    q->tail = 0;
    q->send_wait.head = NULL;
    q->send_wait.tail = NULL;
    q->send_wait.count = 0;
    q->recv_wait.head = NULL;
    q->recv_wait.tail = NULL;
    q->recv_wait.count = 0;
    os_spinlock_init(&q->lock);
    q->name = name;
    
    return OS_EOK;
}

os_err_t os_queue_send(os_queue_t *q, const void *msg, os_tick_t timeout) {
    if (!q || !msg) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&q->lock);
    
    if (q->recv_wait.head) {
        os_tcb_t *waiter = os_ipc_wait_list_pop_head(&q->recv_wait);
        os_queue_copy_message(waiter->pending_obj, msg, q->msg_size);
        os_sched_wake_task(waiter, OS_EOK);
        
        os_spinlock_irq_restore(&q->lock, flags);
        os_sched();
        return OS_EOK;
    }
    
    if (q->count < q->capacity) {
        os_queue_push_buffered(q, msg);
        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EOK;
    }
    
    if (timeout == OS_NO_WAIT) {
        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EFULL;
    }
    
    os_tcb_t *task = os_task_self();
    task->pending_obj = (void *)msg;
    task->pending_ipc = q;
    task->pending_type = OS_PENDING_QUEUE_SEND;
    task->pending_deadline = (timeout == OS_WAIT_FOREVER) ? 0 : (os_tick_get() + timeout);
    task->pending_result = OS_ETIMEOUT;
    task->state = OS_TASK_BLOCKED;

    os_ipc_wait_list_append(&q->send_wait, task);

    os_spinlock_irq_restore(&q->lock, flags);
    os_sched();

    return task->pending_result;
}

os_err_t os_queue_recv(os_queue_t *q, void *msg, os_tick_t timeout) {
    if (!q || !msg) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&q->lock);
    
    if (q->count > 0) {
        os_queue_copy_message(msg, (uint8_t *)q->buffer + (q->head * q->msg_size), q->msg_size);
        q->head = (q->head + 1) % q->capacity;
        q->count--;

        if (q->send_wait.head) {
            os_tcb_t *waiter = os_ipc_wait_list_pop_head(&q->send_wait);
            os_queue_push_buffered(q, waiter->pending_obj);
            os_sched_wake_task(waiter, OS_EOK);
        }

        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EOK;
    }
    
    if (timeout == OS_NO_WAIT) {
        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EEMPTY;
    }
    
    os_tcb_t *task = os_task_self();
    task->pending_obj = msg;
    task->pending_ipc = q;
    task->pending_type = OS_PENDING_QUEUE_RECV;
    task->pending_deadline = (timeout == OS_WAIT_FOREVER) ? 0 : (os_tick_get() + timeout);
    task->pending_result = OS_ETIMEOUT;
    task->state = OS_TASK_BLOCKED;
    
    os_ipc_wait_list_append(&q->recv_wait, task);
    
    os_spinlock_irq_restore(&q->lock, flags);
    os_sched();
    
    return task->pending_result;
}

os_err_t os_event_init(os_event_t *event, const char *name) {
    if (!event) {
        return OS_EINVAL;
    }
    
    event->flags = 0;
    event->wait_list.head = NULL;
    event->wait_list.tail = NULL;
    event->wait_list.count = 0;
    os_spinlock_init(&event->lock);
    event->name = name;
    
    return OS_EOK;
}

os_err_t os_event_set(os_event_t *event, uint32_t flags) {
    bool woke_any = false;

    if (!event) {
        return OS_EINVAL;
    }
    
    os_reg_t irq_flags = os_spinlock_irq_save(&event->lock);
    event->flags |= flags;

    os_tcb_t *task = event->wait_list.head;
    while (task) {
        os_tcb_t *next = task->next;
        uint32_t actual = 0;

        if (os_event_wait_satisfied(event, task->pending_flags, task->pending_options, &actual)) {
            os_ipc_wait_list_remove(&event->wait_list, task);
            if (task->pending_obj) {
                *(uint32_t *)task->pending_obj = actual;
            }
            if (task->pending_options & OS_EVENT_CLEAR_ON_EXIT) {
                event->flags &= ~actual;
            }
            os_sched_wake_task(task, OS_EOK);
            woke_any = true;
        }

        task = next;
    }

    os_spinlock_irq_restore(&event->lock, irq_flags);

    if (woke_any) {
        os_sched();
    }
    
    return OS_EOK;
}

os_err_t os_event_clear(os_event_t *event, uint32_t flags) {
    if (!event) {
        return OS_EINVAL;
    }
    
    os_reg_t irq_flags = os_spinlock_irq_save(&event->lock);
    event->flags &= ~flags;
    os_spinlock_irq_restore(&event->lock, irq_flags);
    
    return OS_EOK;
}

os_err_t os_event_wait(os_event_t *event, uint32_t flags, uint32_t opt,
                       os_tick_t timeout, uint32_t *actual) {
    os_tcb_t *task;
    os_reg_t irq_flags;
    uint32_t matched = 0;

    if (!event || flags == 0) {
        return OS_EINVAL;
    }

    irq_flags = os_spinlock_irq_save(&event->lock);

    if (os_event_wait_satisfied(event, flags, opt, &matched)) {
        if (actual) {
            *actual = matched;
        }
        if (opt & OS_EVENT_CLEAR_ON_EXIT) {
            event->flags &= ~matched;
        }
        os_spinlock_irq_restore(&event->lock, irq_flags);
        return OS_EOK;
    }

    if (timeout == OS_NO_WAIT) {
        if (actual) {
            *actual = 0;
        }
        os_spinlock_irq_restore(&event->lock, irq_flags);
        return OS_ETIMEOUT;
    }

    task = os_task_self();
    task->pending_obj = actual;
    task->pending_ipc = event;
    task->pending_type = OS_PENDING_EVENT;
    task->pending_flags = flags;
    task->pending_options = opt;
    task->pending_deadline = (timeout == OS_WAIT_FOREVER) ? 0 : (os_tick_get() + timeout);
    task->pending_result = OS_ETIMEOUT;
    task->state = OS_TASK_BLOCKED;

    os_ipc_wait_list_append(&event->wait_list, task);

    os_spinlock_irq_restore(&event->lock, irq_flags);
    os_sched();

    return task->pending_result;
}
