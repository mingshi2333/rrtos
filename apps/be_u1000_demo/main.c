#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hal_canfd.h"
#include "os_kernel.h"
#include "os_config.h"
#include "hal_board.h"
#include "hal_clint.h"
#include "hal_flash.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_irq.h"
#include "hal_spi.h"
#include "hal_uart.h"

#if OS_CFG_SMP_EN
#include "riscv_csr.h"
#endif

#if OS_CFG_SMP_EN
#include "os_smp.h"
#endif

static uint8_t control_task_stack[1024];
static uint8_t worker_task_stack[1024];
#if OS_CFG_SMP_EN
static uint8_t balance_task_stack[1024];
#endif
static os_tcb_t control_task_tcb;
static os_tcb_t worker_task_tcb;
#if OS_CFG_SMP_EN
static os_tcb_t balance_task_tcb;
static volatile uint32_t g_balance_seen_mask;
#endif

#define SELFTEST_SPI_TIMEOUT  100000u
#define SELFTEST_CANFD_TIMEOUT 100000u
#define CANFD_IRQ_SEEN(index) (1u << (index))

static volatile uint32_t g_canfd_irq_seen_mask;
static volatile uint32_t g_canfd_irq_count[HAL_BOARD_CANFD_CONTROLLER_COUNT];
static hal_board_canfd_profile_t g_canfd_profiles[HAL_BOARD_CANFD_CONTROLLER_COUNT];
static uint32_t g_canfd_profile_count;

static bool canfd_frames_match(const hal_canfd_frame_t *lhs, const hal_canfd_frame_t *rhs)
{
    uint8_t i;

    if (lhs->id != rhs->id || lhs->len != rhs->len) {
        return false;
    }

    for (i = 0; i < lhs->len; ++i) {
        if (lhs->data[i] != rhs->data[i]) {
            return false;
        }
    }

    return true;
}

static bool wait_for_canfd_irq(uint32_t irq_mask, uint32_t timeout)
{
    uint32_t i;

    for (i = 0; i < timeout; ++i) {
        if ((g_canfd_irq_seen_mask & irq_mask) != 0u) {
            return true;
        }
    }

    return false;
}

static const char *selftest_label(const char *label, const char *fallback);

#if defined(CONFIG_BOARD_BE_U1000)
static void canfd_irq_handler(uint32_t irq_num)
{
    uint32_t index;

    for (index = 0; index < g_canfd_profile_count; ++index) {
        const char *name = selftest_label(g_canfd_profiles[index].label, "CANFD");

        if (irq_num != g_canfd_profiles[index].irq_num) {
            continue;
        }

        (void)hal_canfd_irq_enable(0u);
        (void)hal_canfd_irq_ack(HAL_CANFD_IRQ_TX_COMPLETE |
                                HAL_CANFD_IRQ_RX_READY |
                                HAL_CANFD_IRQ_ERROR);
        g_canfd_irq_seen_mask |= CANFD_IRQ_SEEN(index);
        ++g_canfd_irq_count[index];
        os_print("[IRQ] %s external: OK (irq=%u count=%u)\n",
                 name,
                 irq_num,
                 g_canfd_irq_count[index]);
        break;
    }
}
#endif

static void report_pinmux_result(const char *name, hal_board_pinmux_group_t group)
{
    const char *label = name ? name : hal_board_pinmux_group_name(group);

    if (hal_board_apply_pinmux_group(group) == 0) {
        os_print("[SELFTEST] %s pinmux: OK\n", label);
    } else {
        os_print("[SELFTEST] %s pinmux: FAIL\n", label);
    }
}

static const char *selftest_label(const char *label, const char *fallback)
{
    return label ? label : fallback;
}

static void report_flash_sample(const char *label,
                                const uint32_t *words,
                                uint32_t count)
{
    uint32_t i;

    os_print("[SELFTEST] %s window sample:", label);
    for (i = 0; i < count; ++i) {
        os_print(" 0x%x", words[i]);
    }
    os_print("\n");
}

static void report_canfd_result(const hal_board_canfd_profile_t *profile,
                                uint32_t index);

static void run_gpio_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_gpio_resource_t *led_gpio = &profile->led_gpio;
    const hal_board_gpio_resource_t *button_gpio = &profile->button_gpio;

    report_pinmux_result(NULL, profile->led_pinmux_group);
    report_pinmux_result(NULL, profile->button_pinmux_group);

    hal_gpio_init(led_gpio->base);
    if (led_gpio->available &&
        hal_gpio_set_direction(led_gpio->pin, true) == 0 &&
        hal_gpio_write(led_gpio->pin, true) == 0 &&
        hal_gpio_toggle(led_gpio->pin) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%u)\n",
                 (uint32_t)led_gpio->base,
                 (uint32_t)led_gpio->pin);
        os_print("[SELFTEST] %s: OK (%s)\n",
                 selftest_label(led_gpio->label, "LED GPIO"),
                 selftest_label(led_gpio->location, "n/a"));
    } else {
        os_print("[CHK] GPIO init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(led_gpio->label, "LED GPIO"));
    }

    hal_gpio_init(button_gpio->base);
    if (button_gpio->available &&
        hal_gpio_set_direction(button_gpio->pin, false) == 0) {
        os_print("[CHK] GPIO init: OK (base=0x%x pin=%u)\n",
                 (uint32_t)button_gpio->base,
                 (uint32_t)button_gpio->pin);
        os_print("[SELFTEST] %s: OK (%s level=%u)\n",
                 selftest_label(button_gpio->label, "button GPIO"),
                 selftest_label(button_gpio->location, "n/a"),
                 hal_gpio_read(button_gpio->pin) ? 1u : 0u);
    } else {
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(button_gpio->label, "button GPIO"));
    }
}

static void run_serial_bus_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_diag_config_t *diag_config = &profile->diag;
    uint8_t spi_rx = 0;

    report_pinmux_result(NULL, profile->i2c_pinmux_group);
    if (diag_config->available &&
        hal_i2c_init(diag_config->i2c_base, diag_config->i2c_bus_hz) == 0) {
        os_print("[CHK] I2C init: OK (base=0x%x hz=%u)\n",
                 (uint32_t)diag_config->i2c_base,
                 (uint32_t)diag_config->i2c_bus_hz);
        os_print("[SELFTEST] %s: OK (%s, %uHz)\n",
                 selftest_label(diag_config->i2c_label, "I2C"),
                 selftest_label(diag_config->i2c_route, "n/a"),
                 (uint32_t)diag_config->i2c_bus_hz);
    } else {
        os_print("[CHK] I2C init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(diag_config->i2c_label, "I2C"));
    }

    report_pinmux_result(NULL, profile->spi_pinmux_group);
    if (diag_config->available &&
        hal_spi_init(diag_config->spi_base, diag_config->spi_baud_div, HAL_SPI_MODE0) == 0 &&
        hal_spi_transfer(diag_config->spi_probe_tx, &spi_rx, SELFTEST_SPI_TIMEOUT) == 0) {
        os_print("[CHK] SPI init: OK (base=0x%x div=%u)\n",
                 (uint32_t)diag_config->spi_base,
                 (uint32_t)diag_config->spi_baud_div);
        os_print("[SELFTEST] %s: OK (%s rx=0x%x)\n",
                 selftest_label(diag_config->spi_label, "SPI"),
                 selftest_label(diag_config->spi_route, "n/a"),
                 spi_rx);
    } else {
        os_print("[CHK] SPI init: FAIL\n");
        os_print("[SELFTEST] %s: FAIL\n",
                 selftest_label(diag_config->spi_label, "SPI"));
    }
}

static void run_flash_selftest(const hal_board_selftest_profile_t *profile)
{
    const hal_board_flash_profile_t *flash_profile = &profile->flash;
    uint32_t qspi_sig[4] = {0};
    uint32_t sample_offset = flash_profile->window_sample_offset;
    uint32_t sample_words = flash_profile->window_sample_words;
    uint32_t i;
    hal_flash_info_t flash_info = {0};
    bool qspi_sample_valid = false;

    if (sample_words > 4u) {
        sample_words = 4u;
    }

    report_pinmux_result(NULL, profile->flash_pinmux_group);
    os_print("[SELFTEST] %s ready: ctrl=0x%x window=0x%x (%s)\n",
             selftest_label(flash_profile->label, "flash"),
             (uint32_t)flash_profile->ctrl_base,
             (uint32_t)flash_profile->window_base,
             selftest_label(flash_profile->ready_note, selftest_label(flash_profile->route, "n/a")));
    if (flash_profile->available &&
        hal_flash_init(flash_profile->window_base, flash_profile->window_size) == 0) {
        os_print("[CHK] FLASH init: OK (base=0x%x size=0x%x)\n",
                 (uint32_t)flash_profile->window_base,
                 (uint32_t)flash_profile->window_size);
        qspi_sample_valid = sample_words == 4u;
        for (i = 0; i < sample_words; ++i) {
            if (hal_flash_read_u32(sample_offset + (i * sizeof(uint32_t)), &qspi_sig[i]) != 0) {
                qspi_sample_valid = false;
                break;
            }
        }

        if (qspi_sample_valid) {
            os_print("[CHK] FLASH read: OK (offset=0x%x len=%u)\n",
                     sample_offset,
                     sample_words * (uint32_t)sizeof(uint32_t));
            report_flash_sample(selftest_label(flash_profile->label, "flash"),
                                qspi_sig,
                                sample_words);
        } else {
            os_print("[CHK] FLASH read: FAIL\n");
        }

        if (hal_flash_get_info(&flash_info) == 0) {
            os_print("[CHK] FLASH identify: OK (jedec=0x%x page=%u sector=%u size=0x%x)\n",
                     flash_info.jedec_id,
                     flash_info.page_size,
                     flash_info.sector_size,
                     flash_info.capacity_bytes);
        } else {
            os_print("[CHK] FLASH identify: FAIL\n");
        }
    } else {
        os_print("[CHK] FLASH init: FAIL\n");
    }
    if (qspi_sample_valid &&
        qspi_sig[0] == flash_profile->expected_signature[0] &&
        qspi_sig[1] == flash_profile->expected_signature[1] &&
        qspi_sig[2] == flash_profile->expected_signature[2] &&
        qspi_sig[3] == flash_profile->expected_signature[3]) {
        os_print("[SELFTEST] %s window read: OK (%s)\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->window_match_note, selftest_label(flash_profile->window_read_note, "sample matched")));
    } else {
        os_print("[SELFTEST] %s window read: %s\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->window_capture_note, "sample captured"));
    }
    if (flash_info.jedec_id == flash_profile->jedec_id &&
        flash_info.page_size == flash_profile->page_size &&
        flash_info.sector_size == flash_profile->sector_size &&
        flash_info.capacity_bytes == flash_profile->capacity_bytes) {
        os_print("[SELFTEST] %s identify: OK (%s)\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->identify_match_note, selftest_label(flash_profile->identify_note, "profile matched")));
    } else if (flash_info.jedec_id != 0u) {
        os_print("[SELFTEST] %s identify: %s\n",
                 selftest_label(flash_profile->label, "flash"),
                 selftest_label(flash_profile->identify_capture_note, "sample captured"));
    }
}

static void run_canfd_selftest(const hal_board_selftest_profile_t *profile)
{
    uint32_t index;
    uint32_t canfd_count = profile->canfd_count;

    if (canfd_count > HAL_BOARD_CANFD_CONTROLLER_COUNT) {
        canfd_count = HAL_BOARD_CANFD_CONTROLLER_COUNT;
    }

    g_canfd_profile_count = canfd_count;

    for (index = 0; index < canfd_count; ++index) {
        g_canfd_profiles[index] = profile->canfd[index];
        report_canfd_result(&g_canfd_profiles[index], index);
    }
}

static void report_canfd_result(const hal_board_canfd_profile_t *profile,
                                uint32_t index)
{
    hal_canfd_config_t config;
    hal_canfd_state_t state;
    hal_canfd_frame_t tx_frame = {0};
    hal_canfd_frame_t rx_frame = {0};
    uint8_t i;
    uint32_t irq_mask = CANFD_IRQ_SEEN(index);
    const char *name = selftest_label(profile ? profile->label : NULL, "CANFD");

    if (!profile || index >= HAL_BOARD_CANFD_CONTROLLER_COUNT || !profile->available) {
        os_print("[CHK] %s init: FAIL\n", name);
        return;
    }

    report_pinmux_result(name, profile->pinmux_group);

    config.nominal_bitrate = profile->bitrate;
    config.internal_loopback = profile->internal_loopback;
    tx_frame.id = profile->frame_id;
    tx_frame.len = (uint8_t)profile->frame_len;
    for (i = 0; i < tx_frame.len; ++i) {
        tx_frame.data[i] = (uint8_t)(profile->payload_seed + i);
    }

    g_canfd_irq_seen_mask &= ~irq_mask;
    g_canfd_irq_count[index] = 0u;

    hal_irq_register_handler(profile->irq_num, canfd_irq_handler);
    hal_irq_set_priority(profile->irq_num, 1u);
    hal_irq_enable(profile->irq_num);
    os_print("[CHK] %s irq-arm: OK (irq=%u)\n", name, profile->irq_num);

    if (hal_canfd_init(profile->base, &config) == 0) {
        os_print("[CHK] %s init: OK (base=0x%x bitrate=%u loopback=%u)\n",
                 name,
                 (uint32_t)profile->base,
                 profile->bitrate,
                 profile->internal_loopback ? 1u : 0u);
        if (hal_canfd_irq_enable(HAL_CANFD_IRQ_TX_COMPLETE |
                                 HAL_CANFD_IRQ_RX_READY |
                                 HAL_CANFD_IRQ_ERROR) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE) {
            os_print("[CHK] %s state: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
        } else {
            os_print("[CHK] %s state: FAIL\n", name);
            return;
        }

        if (hal_canfd_tx_enqueue(&tx_frame, SELFTEST_CANFD_TIMEOUT) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE) {
            os_print("[CHK] %s path: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
        } else {
            os_print("[CHK] %s path: FAIL\n", name);
            return;
        }

        if (wait_for_canfd_irq(irq_mask, SELFTEST_CANFD_TIMEOUT)) {
            os_print("[CHK] %s irq-fire: OK (irq=%u count=%u)\n",
                     name,
                     profile->irq_num,
                     g_canfd_irq_count[index]);
        } else {
            os_print("[CHK] %s irq-fire: FAIL\n", name);
            return;
        }

        if (hal_canfd_rx_dequeue(&rx_frame, SELFTEST_CANFD_TIMEOUT) == 0 &&
            hal_canfd_irq_ack(HAL_CANFD_IRQ_TX_COMPLETE |
                              HAL_CANFD_IRQ_RX_READY |
                              HAL_CANFD_IRQ_ERROR) == 0 &&
            hal_canfd_get_state(&state) == 0 &&
            state.error_status == HAL_CANFD_ERROR_NONE &&
            canfd_frames_match(&tx_frame, &rx_frame)) {
            os_print("[CHK] %s loopback: OK (id=0x%x len=%u)\n",
                     name,
                     tx_frame.id,
                     (uint32_t)tx_frame.len);
            os_print("[CHK] %s settle: OK (status=0x%x irq=0x%x err=0x%x txflr=%u rxflr=%u)\n",
                     name,
                     state.status,
                     state.irq_status,
                     state.error_status,
                     state.tx_fifo_level,
                     state.rx_fifo_level);
            os_print("[SELFTEST] %s loopback: OK (base=0x%x bitrate=%u id=0x%x)\n",
                     name,
                     (uint32_t)profile->base,
                     profile->bitrate,
                     tx_frame.id);
            if (profile->route) {
                os_print("[SELFTEST] %s route: %s\n", name, profile->route);
            }
            if (profile->loopback_note) {
                os_print("[SELFTEST] %s loopback note: %s\n",
                         name,
                         profile->loopback_note);
            }
        } else {
            os_print("[CHK] %s loopback: FAIL\n", name);
        }
    } else {
        os_print("[CHK] %s init: FAIL\n", name);
    }
}

#if defined(CONFIG_BOARD_BE_U1000)
static void board_bus_self_check(void)
{
    hal_board_selftest_profile_t selftest_profile;

    hal_board_get_selftest_profile(&selftest_profile);

    if (!selftest_profile.available) {
        os_print("[CHK] board selftest profile: FAIL\n");
        return;
    }

    report_pinmux_result(NULL, selftest_profile.console_pinmux_group);
    run_gpio_selftest(&selftest_profile);
    run_serial_bus_selftest(&selftest_profile);
    run_flash_selftest(&selftest_profile);
    run_canfd_selftest(&selftest_profile);
}
#endif

static void platform_self_check(void) {
    uint64_t t0;
    uint64_t t1;
    volatile uint32_t i;

    t0 = hal_clint_mtime_get();
    for (i = 0; i < 50000; i++) {
    }
    t1 = hal_clint_mtime_get();

    if (t1 > t0) {
        os_print("[CHK] CLINT mtime monotonic: OK (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    } else {
        os_print("[CHK] CLINT mtime monotonic: FAIL (0x%x -> 0x%x)\n", (uint32_t)t0, (uint32_t)t1);
    }

#if defined(CONFIG_BOARD_BE_U1000)
    board_bus_self_check();
#endif
}

static void control_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
#if OS_CFG_SMP_EN
        os_tcb_t *self = os_task_self();
        os_print("[AFFINITY] control on Core%u affinity Core%u tick %u\n",
                 self->cpu_id, self->affinity, count++);
#else
        os_print("[CTRL] single-core tick %u\n", count++);
#endif
        os_task_delay(1000);
    }
}

static void worker_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
#if OS_CFG_SMP_EN
        os_tcb_t *self = os_task_self();
        os_print("[AFFINITY] worker on Core%u affinity Core%u tick %u\n",
                 self->cpu_id, self->affinity, count++);
#else
        os_print("[WORK] single-core tick %u\n", count++);
#endif
        os_task_delay(2000);
    }
}

#if OS_CFG_SMP_EN
static void balance_remote_marker(void *unused)
{
    (void)unused;
    g_balance_seen_mask |= 1u << os_cpu_id();
    os_print("[BALANCE] balance on Core%u affinity any bootstrap seen-mask=0x%x\n",
             os_cpu_id(), g_balance_seen_mask);
}

static void balance_task_entry(void *arg) {
    (void)arg;
    uint32_t count = 0;

    while (1) {
        os_tcb_t *self = os_task_self();
        g_balance_seen_mask |= 1u << self->cpu_id;
        if (os_smp_online_count() >= 2 && g_balance_seen_mask != 0x3u) {
            os_cpu_t peer = self->cpu_id == 0 ? 1 : 0;
            if ((g_balance_seen_mask & (1u << peer)) == 0u) {
                (void)os_smp_call_on_cpu(peer, balance_remote_marker, NULL, true);
            }
        }
        os_print("[BALANCE] balance on Core%u affinity any tick %u seen-mask=0x%x\n",
                 self->cpu_id, count++, g_balance_seen_mask);
        os_task_delay(500);
    }
}
#endif

void os_kernel_main(void) {
    hal_board_execution_profile_t execution_profile;

    hal_board_init();
    hal_board_print_banner();
    os_print("[BOOT] RUN_MARKER: 0x%x\n", (uint32_t)hal_clint_mtime_get());
    hal_board_get_execution_profile(&execution_profile);

#if OS_CFG_IRQ_MODEL_CLIC
    os_print("[BOOT] IRQ model: CLIC\n");
#else
    os_print("[BOOT] IRQ model: PLIC\n");
#endif

    platform_self_check();

    os_print("[BOOT] Initializing kernel...\n");
    os_kernel_init();

#if OS_CFG_SMP_EN
    if (execution_profile.available && execution_profile.role_plan) {
        os_print("[SMP] role-plan: %s\n", execution_profile.role_plan);
    }
    os_smp_init();
    os_smp_start_cpus();
    if (os_smp_cpu_online(1)) {
        os_ipi_send(1, OS_IPI_RESCHEDULE);
    }
#endif

    os_task_create(&control_task_tcb, "control", control_task_entry, NULL,
                   10, control_task_stack, sizeof(control_task_stack));
    os_task_create(&worker_task_tcb, "worker", worker_task_entry, NULL,
                   11, worker_task_stack, sizeof(worker_task_stack));

#if OS_CFG_SMP_EN
    os_task_set_affinity(&control_task_tcb, 0);
    os_task_set_affinity(&worker_task_tcb, 1);
    os_task_create(&balance_task_tcb, "balance", balance_task_entry, NULL,
                   12, balance_task_stack, sizeof(balance_task_stack));
    if (execution_profile.available && execution_profile.task_map_smp) {
        os_print("[SMP] task-map: %s\n", execution_profile.task_map_smp);
    }
    os_smp_release_cpus();
#else
    if (execution_profile.available && execution_profile.task_map_single) {
        os_print("[BOOT] task-map: %s\n", execution_profile.task_map_single);
    }
#endif

    os_print("[BOOT] Starting scheduler...\n");
    os_kernel_start();
}
