#include <stdint.h>

#include "../../boards/be_u1000/board_config.h"
#include "../../hal/include/hal_board.h"
#include "../../hal/include/hal_clint.h"
#include "../../hal/include/hal_uart.h"
#include "../../hal/include/hal_usb.h"
#include "../../kernel/include/os_kernel.h"

static os_tcb_t g_usb_task_tcb;
static uint8_t g_usb_task_stack[768];

static const char *usb_role_name(hal_usb_endpoint_role_t role)
{
    switch (role) {
    case HAL_USB_EP_ROLE_CONTROL:
        return "control";
    case HAL_USB_EP_ROLE_TX:
        return "tx";
    case HAL_USB_EP_ROLE_RX:
        return "rx";
    default:
        return "bidir";
    }
}

static const char *usb_view_name(hal_usb_endpoint_view_t view)
{
    return (view == HAL_USB_EP_VIEW_EP0) ? "ep0" : "txrx";
}

static const char *usb_csr_name(hal_usb_endpoint_csr_kind_t kind)
{
    switch (kind) {
    case HAL_USB_CSR_KIND_CSR0:
        return "csr0";
    case HAL_USB_CSR_KIND_TXCSR:
        return "txcsr";
    default:
        return "rxcsr";
    }
}

static const char *usb_count_name(hal_usb_endpoint_count_kind_t kind)
{
    return (kind == HAL_USB_COUNT_KIND_COUNT0) ? "count0" : "rxcount";
}

static void usb_runtime_task(void *arg)
{
    hal_usb_runtime_state_t state = {0};
    os_err_t delay_rc;
    int rc;
    uint32_t tick = 0;

    (void)arg;

    while (1) {
        for (uint32_t sample = 0; sample < 8u; sample++) {
            uint32_t index = tick & 0x3u;

            rc = hal_usb_set_index((uint8_t)index);
            if (rc != 0) {
                os_print("[USB_APP] set index failed rc=%d idx=%u\n", rc, index);
                break;
            }

            rc = hal_usb_get_runtime_state(&state);
            if (rc != 0) {
                os_print("[USB_APP] runtime state failed rc=%d idx=%u\n", rc, index);
                break;
            }

            os_print("[USB_APP] tick %u intr=0x%x intrtx=0x%x idx=%u cfg=0x%x power=0x%x txmaxp=0x%x rxmaxp=0x%x csr0=0x%x count0=%u txcsr=0x%x rxcsr=0x%x rxcount=%u frame=%u view=%s role=%s maxp=0x%x csrsel=%s csr=0x%x csr-valid=%u countsel=%s count=%u count-valid=%u ready=%u\n",
                     tick++,
                     (uint32_t)state.controller.intrusb,
                     (uint32_t)state.controller.intrtx,
                     (uint32_t)state.controller.index,
                     (uint32_t)state.controller.configdata,
                     (uint32_t)state.controller.power,
                     (uint32_t)state.endpoint.txmaxp,
                     (uint32_t)state.endpoint.rxmaxp,
                     (uint32_t)state.endpoint.csr0,
                     (uint32_t)state.endpoint.count0,
                     (uint32_t)state.endpoint.txcsr,
                     (uint32_t)state.endpoint.rxcsr,
                     (uint32_t)state.endpoint.rxcount,
                     (uint32_t)state.controller.frame,
                     usb_view_name(state.endpoint.register_view),
                     usb_role_name(state.endpoint.role),
                     (uint32_t)state.endpoint.max_packet,
                     usb_csr_name(state.endpoint.csr_kind),
                     (uint32_t)state.endpoint.csr_value,
                     state.endpoint.csr_valid ? 1u : 0u,
                     usb_count_name(state.endpoint.count_kind),
                     (uint32_t)state.endpoint.count_value,
                     state.endpoint.count_valid ? 1u : 0u,
                     state.endpoint.ready ? 1u : 0u);
        }
        delay_rc = os_task_delay(100);
        if (delay_rc != OS_EOK) {
            os_print("[USB_APP] task delay failed rc=%d\n", (int)delay_rc);
            while (1) {
            }
        }
    }
}

void os_kernel_main(void)
{
    hal_usb_config_t cfg;
    uint8_t configdata = 0;
    int init_rc;
    int rc;

    hal_board_init();
    hal_board_print_banner();

    os_print("[USB_APP] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());

    cfg.function_address = 1u;
    cfg.soft_connect = true;
    cfg.high_speed_enable = true;
    cfg.intrusb_enable = 0x1Fu;
    init_rc = hal_usb_init(BE_U1000_USB_BASE, &cfg);
    if (init_rc != 0) {
        os_print("[USB_APP] init failed rc=%d\n", init_rc);
        return;
    }

    rc = hal_usb_set_index(1u);
    if (rc != 0) {
        os_print("[USB_APP] set index failed rc=%d idx=1\n", rc);
        return;
    }

    rc = hal_usb_get_configdata(&configdata);
    if (rc != 0) {
        os_print("[USB_APP] get configdata failed rc=%d\n", rc);
        return;
    }

    os_print("[USB_APP] usb_runtime ready base=0x%x faddr=%u cfg=0x%x init-rc=%d\n",
             (uint32_t)BE_U1000_USB_BASE,
             (uint32_t)cfg.function_address,
             (uint32_t)configdata,
             init_rc);
    os_print("[USB_APP] Initializing kernel...\n");

    os_kernel_init();
    rc = os_task_create(&g_usb_task_tcb,
                        "usb_runtime",
                        usb_runtime_task,
                        0,
                        10,
                        g_usb_task_stack,
                        sizeof(g_usb_task_stack));
    if (rc != OS_EOK) {
        os_print("[USB_APP] task create failed rc=%d\n", rc);
        return;
    }

    os_print("[USB_APP] Starting scheduler...\n");
    os_kernel_start();
}
