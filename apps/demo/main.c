#include "os_kernel.h"
#include "riscv_csr.h"
#include "hal_uart.h"
#include "hal_clint.h"
#include "hal_plic.h"
#include "ai_runtime.h"
#include "os_smp.h"

static uint8_t task1_stack[1024];
static uint8_t task2_stack[1024];
static uint8_t task3_stack[1024];
static os_tcb_t task1_tcb;
static os_tcb_t task2_tcb;
static os_tcb_t task3_tcb;

static os_sem_t demo_sem;
static os_mutex_t demo_mutex;
static os_queue_t demo_queue;
static uint32_t queue_buf[8];

static uint8_t ai_arena[OS_CFG_AI_ARENA_SIZE];
static uint8_t ai_fake_model[16] = { 'T', 'F', 'L', '3' };

static void ai_demo_run(void) {
    ai_model_t *model = NULL;
    ai_err_t err = ai_runtime_init(ai_arena, sizeof(ai_arena));
    if (err != AI_ERR_OK) {
        os_print("[AI] runtime init failed: %d\n", err);
        return;
    }

    err = ai_model_load(&model, ai_fake_model, sizeof(ai_fake_model));
    if (err != AI_ERR_OK) {
        os_print("[AI] model load failed: %d\n", err);
        ai_runtime_deinit();
        return;
    }

    err = ai_model_invoke(model);
    if (err != AI_ERR_OK) {
        os_print("[AI] model invoke failed: %d\n", err);
    } else {
        os_print("[AI] model invoke OK\n");
    }

    ai_model_unload(model);
    ai_runtime_deinit();
}

void task1_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;
    static bool ai_ran = false;
    
    while (1) {
        if (!ai_ran) {
            ai_demo_run();
            ai_ran = true;
        }
        os_print("[Task1] Running, count=%d\n", count++);
        os_sem_give(&demo_sem);
        os_task_delay(100);
    }
}

void task2_entry(void *arg) {
    (void)arg;
    
    while (1) {
        os_sem_take(&demo_sem, OS_WAIT_FOREVER);
        os_print("[Task2] Got semaphore!\n");
        
        os_mutex_lock(&demo_mutex, OS_WAIT_FOREVER);
        os_print("[Task2] Mutex locked\n");
        os_task_delay(50);
        os_print("[Task2] Mutex unlocking\n");
        os_mutex_unlock(&demo_mutex);
    }
}

void task3_entry(void *arg) {
    (void)arg;
    uint32_t msg;
    
    while (1) {
        if (os_queue_recv(&demo_queue, &msg, 200) == OS_EOK) {
            os_print("[Task3] Received message: %d\n", msg);
        } else {
            os_print("[Task3] Queue timeout, sending message\n");
            msg = os_tick_get();
            os_queue_send(&demo_queue, &msg, OS_NO_WAIT);
        }
    }
}

void os_kernel_main(void) {
    hal_uart_init(OS_CFG_UART_BASE, 115200);
    hal_clint_init(OS_CFG_CLINT_BASE);
    hal_plic_init(OS_CFG_PLIC_BASE);
    
    os_print("\n");
    os_print("========================================\n");
    os_print("  RV-AIOS: RISC-V AI Operating System\n");
    os_print("  Version %d.%d.%d\n", OS_VERSION_MAJOR, OS_VERSION_MINOR, OS_VERSION_PATCH);
    os_print("========================================\n");
    os_print("\n");
    
    os_print("[BOOT] Initializing kernel...\n");
    os_kernel_init();

#if OS_CFG_SMP_EN
    os_smp_init();
    os_smp_start_cpus();
#endif
    
    os_print("[BOOT] Initializing IPC objects...\n");
    os_sem_init(&demo_sem, "demo_sem", 0, 10);
    os_mutex_init(&demo_mutex, "demo_mutex");
    os_queue_init(&demo_queue, "demo_queue", queue_buf, sizeof(uint32_t), 8);

    os_print("[BOOT] Creating tasks...\n");
    os_task_create(&task1_tcb, "task1", task1_entry, NULL, 10, task1_stack, sizeof(task1_stack));
    os_task_create(&task2_tcb, "task2", task2_entry, NULL, 20, task2_stack, sizeof(task2_stack));
    os_task_create(&task3_tcb, "task3", task3_entry, NULL, 30, task3_stack, sizeof(task3_stack));
    
    os_print("[BOOT] Starting scheduler...\n");
    os_print("\n");
    
    os_kernel_start();
    
    while (1) {
    }
}

void os_secondary_main(void) {
    os_cpu_t cpu = os_cpu_id();
    os_print("[CPU%d] Secondary core started\n", cpu);
    
    while (1) {
        os_wfi();
    }
}
