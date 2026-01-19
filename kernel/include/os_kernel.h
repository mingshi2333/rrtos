#ifndef OS_KERNEL_H
#define OS_KERNEL_H

#include "os_types.h"

void os_kernel_init(void);
void os_kernel_start(void);
os_tick_t os_tick_get(void);
void os_tick_handler(void);

os_err_t os_task_create(os_tcb_t *tcb, const char *name, os_task_entry_t entry,
                        void *arg, os_prio_t prio, void *stack, os_size_t stack_size);
os_err_t os_task_delete(os_tcb_t *tcb);
os_err_t os_task_suspend(os_tcb_t *tcb);
os_err_t os_task_resume(os_tcb_t *tcb);
void os_task_yield(void);
os_err_t os_task_delay(os_tick_t ticks);
os_err_t os_task_set_prio(os_tcb_t *tcb, os_prio_t prio);
os_tcb_t *os_task_self(void);

void os_sched_lock(void);
void os_sched_unlock(void);
void os_sched(void);

os_err_t os_sem_init(os_sem_t *sem, const char *name, int32_t init, int32_t max);
os_err_t os_sem_take(os_sem_t *sem, os_tick_t timeout);
os_err_t os_sem_give(os_sem_t *sem);
os_err_t os_sem_reset(os_sem_t *sem, int32_t count);

os_err_t os_mutex_init(os_mutex_t *mutex, const char *name);
os_err_t os_mutex_lock(os_mutex_t *mutex, os_tick_t timeout);
os_err_t os_mutex_unlock(os_mutex_t *mutex);

os_err_t os_queue_init(os_queue_t *q, const char *name, void *buf,
                       os_size_t msg_size, os_size_t capacity);
os_err_t os_queue_send(os_queue_t *q, const void *msg, os_tick_t timeout);
os_err_t os_queue_recv(os_queue_t *q, void *msg, os_tick_t timeout);

os_err_t os_event_init(os_event_t *event, const char *name);
os_err_t os_event_set(os_event_t *event, uint32_t flags);
os_err_t os_event_clear(os_event_t *event, uint32_t flags);
os_err_t os_event_wait(os_event_t *event, uint32_t flags, uint32_t opt,
                       os_tick_t timeout, uint32_t *actual);

#define OS_EVENT_WAIT_ALL       0x01
#define OS_EVENT_WAIT_ANY       0x00
#define OS_EVENT_CLEAR_ON_EXIT  0x02

os_err_t os_timer_init(os_timer_t *timer, const char *name, os_timer_cb_t cb,
                       void *arg, os_tick_t period, uint8_t periodic);
os_err_t os_timer_start(os_timer_t *timer);
os_err_t os_timer_stop(os_timer_t *timer);

#if OS_CFG_SMP_EN
os_cpu_t os_cpu_count(void);
os_cpu_data_t *os_cpu_data(os_cpu_t cpu);
void os_smp_init(void);
void os_ipi_send(os_cpu_t cpu, uint32_t reason);
os_err_t os_task_set_affinity(os_tcb_t *tcb, os_cpu_t affinity);
#endif

#endif
