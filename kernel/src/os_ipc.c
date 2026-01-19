#include "os_kernel.h"
#include "riscv_csr.h"
#include "riscv_atomic.h"

extern void os_sched(void);
extern os_tcb_t *os_task_self(void);

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
    
    if (sem->wait_list.tail) {
        sem->wait_list.tail->next = task;
    } else {
        sem->wait_list.head = task;
    }
    task->prev = sem->wait_list.tail;
    task->next = NULL;
    sem->wait_list.tail = task;
    sem->wait_list.count++;
    
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
        os_tcb_t *task = sem->wait_list.head;
        sem->wait_list.head = task->next;
        if (sem->wait_list.head) {
            sem->wait_list.head->prev = NULL;
        } else {
            sem->wait_list.tail = NULL;
        }
        sem->wait_list.count--;
        
        task->pending_result = OS_EOK;
        task->pending_obj = NULL;
        task->pending_ipc = NULL;
        task->pending_type = OS_PENDING_NONE;
        task->pending_deadline = 0;
        task->state = OS_TASK_READY;
        
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
    
    if (mutex->wait_list.tail) {
        mutex->wait_list.tail->next = task;
    } else {
        mutex->wait_list.head = task;
    }
    task->prev = mutex->wait_list.tail;
    task->next = NULL;
    mutex->wait_list.tail = task;
    mutex->wait_list.count++;
    
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
        os_tcb_t *waiter = mutex->wait_list.head;
        mutex->wait_list.head = waiter->next;
        if (mutex->wait_list.head) {
            mutex->wait_list.head->prev = NULL;
        } else {
            mutex->wait_list.tail = NULL;
        }
        mutex->wait_list.count--;
        
        mutex->owner = waiter;
        mutex->nest_count = 1;
        mutex->original_prio = waiter->priority;
        
        waiter->pending_result = OS_EOK;
        waiter->pending_obj = NULL;
        waiter->state = OS_TASK_READY;
        
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
        os_tcb_t *waiter = q->recv_wait.head;
        q->recv_wait.head = waiter->next;
        if (q->recv_wait.head) {
            q->recv_wait.head->prev = NULL;
        } else {
            q->recv_wait.tail = NULL;
        }
        q->recv_wait.count--;
        
        uint8_t *dst = (uint8_t *)waiter->pending_obj;
        const uint8_t *src = (const uint8_t *)msg;
        for (os_size_t i = 0; i < q->msg_size; i++) {
            dst[i] = src[i];
        }
        
        waiter->pending_result = OS_EOK;
        waiter->pending_obj = NULL;
        waiter->state = OS_TASK_READY;
        
        os_spinlock_irq_restore(&q->lock, flags);
        os_sched();
        return OS_EOK;
    }
    
    if (q->count < q->capacity) {
        uint8_t *dst = (uint8_t *)q->buffer + (q->tail * q->msg_size);
        const uint8_t *src = (const uint8_t *)msg;
        for (os_size_t i = 0; i < q->msg_size; i++) {
            dst[i] = src[i];
        }
        q->tail = (q->tail + 1) % q->capacity;
        q->count++;
        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EOK;
    }
    
    if (timeout == OS_NO_WAIT) {
        os_spinlock_irq_restore(&q->lock, flags);
        return OS_EFULL;
    }
    
    os_spinlock_irq_restore(&q->lock, flags);
    return OS_EFULL;
}

os_err_t os_queue_recv(os_queue_t *q, void *msg, os_tick_t timeout) {
    if (!q || !msg) {
        return OS_EINVAL;
    }
    
    os_reg_t flags = os_spinlock_irq_save(&q->lock);
    
    if (q->count > 0) {
        uint8_t *dst = (uint8_t *)msg;
        uint8_t *src = (uint8_t *)q->buffer + (q->head * q->msg_size);
        for (os_size_t i = 0; i < q->msg_size; i++) {
            dst[i] = src[i];
        }
        q->head = (q->head + 1) % q->capacity;
        q->count--;
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
    
    if (q->recv_wait.tail) {
        q->recv_wait.tail->next = task;
    } else {
        q->recv_wait.head = task;
    }
    task->prev = q->recv_wait.tail;
    task->next = NULL;
    q->recv_wait.tail = task;
    q->recv_wait.count++;
    
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
    if (!event) {
        return OS_EINVAL;
    }
    
    os_reg_t irq_flags = os_spinlock_irq_save(&event->lock);
    event->flags |= flags;
    os_spinlock_irq_restore(&event->lock, irq_flags);
    
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
