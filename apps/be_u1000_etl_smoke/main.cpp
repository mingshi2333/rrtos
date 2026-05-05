extern "C" {
#include "hal_board.h"
#include "hal_uart.h"
#include "os_kernel.h"
}

#include "rrtos/cxx/static_queue.hpp"

struct SmokeEvent {
    uint32_t code;
    uint32_t value;
};

static os_tcb_t g_etl_task_tcb;
static uint8_t g_etl_task_stack[768];

static bool run_static_queue_smoke(void)
{
    rrtos::StaticQueue<SmokeEvent, 4> queue;
    SmokeEvent out = {0u, 0u};

    if (!queue.empty() || queue.full() || queue.size() != 0u) {
        return false;
    }
    if (!queue.push({0x45544C31u, 7u})) {
        return false;
    }
    if (queue.empty() || queue.size() != 1u) {
        return false;
    }
    if (!queue.pop(out)) {
        return false;
    }
    return out.code == 0x45544C31u && out.value == 7u && queue.empty();
}

static void etl_smoke_task(void *arg)
{
    uint32_t tick = 0;
    (void)arg;

    if (run_static_queue_smoke()) {
        os_print("[ETL_SMOKE] static_queue: PASS\n");
    } else {
        os_print("[ETL_SMOKE] static_queue: FAIL\n");
    }

    while (1) {
        os_print("[ETL_SMOKE] tick %u\n", tick++);
        os_task_delay(1000u);
    }
}

extern "C" void os_kernel_main(void)
{
    os_err_t rc;

    hal_board_init();
    hal_board_print_banner();
    os_print("[ETL_SMOKE] C++/ETL lane ready\n");

    os_kernel_init();
    rc = os_task_create(&g_etl_task_tcb,
                        "etl_smoke",
                        etl_smoke_task,
                        0,
                        10,
                        g_etl_task_stack,
                        sizeof(g_etl_task_stack));
    if (rc != OS_EOK) {
        os_print("[ETL_SMOKE] task create failed rc=%d\n", (int)rc);
        return;
    }

    os_kernel_start();
}
