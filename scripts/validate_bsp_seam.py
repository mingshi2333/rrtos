#!/usr/bin/env python3

from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent


def fail(message: str) -> None:
    print(f"ERROR: {message}")
    sys.exit(1)


def require(text: str, needle: str, context: str) -> None:
    if needle not in text:
        fail(f"missing '{needle}' in {context}")


def forbid(text: str, needle: str, context: str) -> None:
    if needle in text:
        fail(f"unexpected '{needle}' in {context}")


def main() -> None:
    beu_demo = (REPO_ROOT / "apps/be_u1000_demo/main.c").read_text(encoding="utf-8")
    mnist_main = (REPO_ROOT / "apps/mnist_app/src/main.c").read_text(encoding="utf-8")
    mnist_validation = (REPO_ROOT / "apps/mnist_app/src/validation_main.c").read_text(
        encoding="utf-8"
    )
    hal_board_h = (REPO_ROOT / "hal/include/hal_board.h").read_text(encoding="utf-8")
    hal_board_smp_h = (REPO_ROOT / "hal/include/hal_board_smp.h").read_text(
        encoding="utf-8"
    )
    hal_board_c = (REPO_ROOT / "hal/src/hal_board.c").read_text(encoding="utf-8")

    forbid(beu_demo, '"board_config.h"', "apps/be_u1000_demo/main.c")
    forbid(mnist_main, '"board_config.h"', "apps/mnist_app/src/main.c")
    forbid(mnist_validation, '"board_config.h"', "apps/mnist_app/src/validation_main.c")

    require(hal_board_h, "typedef enum {", "hal/include/hal_board.h")
    require(hal_board_h, "HAL_BOARD_PINMUX_GROUP_QSPI1", "hal/include/hal_board.h")
    require(hal_board_h, "const char *role_plan;", "hal/include/hal_board.h")
    require(
        hal_board_h,
        "const char *hal_board_task_map_description(bool smp_mode);",
        "hal/include/hal_board.h",
    )
    forbid(hal_board_h, '"os_types.h"', "hal/include/hal_board.h")
    require(
        hal_board_smp_h,
        "os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task);",
        "hal/include/hal_board_smp.h",
    )
    require(
        hal_board_smp_h,
        "bool hal_board_issue_reschedule_probe(void);",
        "hal/include/hal_board_smp.h",
    )
    require(
        hal_board_smp_h,
        "bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu);",
        "hal/include/hal_board_smp.h",
    )
    require(
        hal_board_smp_h,
        "uint32_t hal_board_balance_expected_mask(void);",
        "hal/include/hal_board_smp.h",
    )
    require(hal_board_smp_h, '#include "hal_board.h"', "hal/include/hal_board_smp.h")
    require(hal_board_smp_h, '#include "os_types.h"', "hal/include/hal_board_smp.h")
    forbid(hal_board_h, "uint32_t control_cpu;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t worker_cpu;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t reschedule_probe_cpu;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t balance_probe_cpu_a;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t balance_probe_cpu_b;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t single_core_cpu;", "hal/include/hal_board.h")
    forbid(hal_board_h, "hal_board_diag_config_t", "hal/include/hal_board.h")
    forbid(hal_board_h, "hal_board_gpio_resource_t", "hal/include/hal_board.h")
    forbid(hal_board_h, "hal_board_flash_profile_t", "hal/include/hal_board.h")
    forbid(hal_board_h, "hal_board_canfd_profile_t", "hal/include/hal_board.h")
    forbid(hal_board_h, "hal_board_selftest_profile_t", "hal/include/hal_board.h")
    forbid(hal_board_h, "HAL_BOARD_CANFD_CONTROLLER_COUNT", "hal/include/hal_board.h")
    forbid(
        hal_board_h,
        "void hal_board_get_diag_config(hal_board_diag_config_t *config);",
        "hal/include/hal_board.h",
    )
    forbid(
        hal_board_h,
        "void hal_board_get_gpio_resource(hal_board_gpio_role_t role,",
        "hal/include/hal_board.h",
    )
    forbid(
        hal_board_h,
        "void hal_board_get_flash_profile(hal_board_flash_profile_t *profile);",
        "hal/include/hal_board.h",
    )
    forbid(
        hal_board_h,
        "void hal_board_get_canfd_profile(uint32_t index, hal_board_canfd_profile_t *profile);",
        "hal/include/hal_board.h",
    )
    forbid(hal_board_h, "hal_board_get_selftest_profile", "hal/include/hal_board.h")
    require(
        hal_board_c,
        "typedef struct {\n    uintptr_t spi_base;",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c, "typedef struct {\n    uintptr_t ctrl_base;", "hal/src/hal_board.c"
    )
    require(
        hal_board_c,
        "typedef struct {\n    hal_board_pinmux_group_t console_pinmux_group;",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "typedef struct {\n    os_cpu_t control_cpu;",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "#define HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT 2u",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "static void hal_board_fill_diag_config(hal_board_diag_config_t *config)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "static void hal_board_fill_gpio_resource(hal_board_gpio_role_t role,",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "static void hal_board_fill_flash_profile(hal_board_flash_profile_t *profile)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "static void hal_board_fill_canfd_profile(uint32_t index, hal_board_canfd_profile_t *profile)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "static void hal_board_fill_selftest_profile(hal_board_selftest_profile_t *profile)",
        "hal/src/hal_board.c",
    )
    forbid(hal_board_c, "config->gpio_base", "hal/src/hal_board.c")
    forbid(hal_board_c, "config->gpio_pin", "hal/src/hal_board.c")
    require(
        hal_board_h,
        "int hal_board_run_selftest(void);",
        "hal/include/hal_board.h",
    )
    require(
        hal_board_c,
        "int hal_board_run_selftest(void)",
        "hal/src/hal_board.c",
    )
    require(hal_board_c, "static void run_gpio_selftest(", "hal/src/hal_board.c")
    require(hal_board_c, "static void run_serial_bus_selftest(", "hal/src/hal_board.c")
    require(hal_board_c, "static void run_flash_selftest(", "hal/src/hal_board.c")
    require(hal_board_c, "static void run_canfd_selftest(", "hal/src/hal_board.c")
    require(hal_board_c, "const char *window_capture_note;", "hal/src/hal_board.c")
    require(hal_board_c, "const char *identify_capture_note;", "hal/src/hal_board.c")
    require(hal_board_c, "uint8_t payload_seed;", "hal/src/hal_board.c")
    require(hal_board_c, "const char *route;", "hal/src/hal_board.c")
    require(
        hal_board_c,
        'selftest_label(flash_profile->window_capture_note, "sample captured")',
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        'selftest_label(flash_profile->identify_capture_note, "sample captured")',
        "hal/src/hal_board.c",
    )
    require(hal_board_c, "hal_gpio_init(button_gpio->base);", "hal/src/hal_board.c")
    require(hal_board_c, "profile->payload_seed + i", "hal/src/hal_board.c")
    require(hal_board_c, "[SELFTEST] %s route: %s", "hal/src/hal_board.c")
    require(
        hal_board_c, "HAL_BOARD_SELFTEST_CANFD_CONTROLLER_COUNT", "hal/src/hal_board.c"
    )
    require(hal_board_c, "CANFD_IRQ_SEEN(index)", "hal/src/hal_board.c")
    require(hal_board_c, "profile->canfd_count", "hal/src/hal_board.c")
    require(hal_board_c, "g_canfd_profile_count", "hal/src/hal_board.c")
    require(hal_board_c, "diag_config->spi_probe_tx", "hal/src/hal_board.c")
    require(hal_board_c, "flash_profile->window_sample_offset", "hal/src/hal_board.c")
    require(hal_board_c, "flash_profile->window_sample_words", "hal/src/hal_board.c")
    require(hal_board_c, "report_flash_sample(", "hal/src/hal_board.c")
    require(hal_board_c, "profile->internal_loopback", "hal/src/hal_board.c")
    require(
        hal_board_c,
        "static void hal_board_fill_demo_topology(hal_board_demo_topology_t *topology)",
        "hal/src/hal_board.c",
    )
    require(hal_board_c, "topology->control_cpu = 0u;", "hal/src/hal_board.c")
    require(hal_board_c, "topology->worker_cpu = 1u;", "hal/src/hal_board.c")
    require(hal_board_c, "topology->reschedule_probe_cpu = 1u;", "hal/src/hal_board.c")
    require(hal_board_c, "topology->balance_probe_cpu_a = 0u;", "hal/src/hal_board.c")
    require(hal_board_c, "topology->balance_probe_cpu_b = 1u;", "hal/src/hal_board.c")
    require(
        hal_board_c,
        "os_err_t hal_board_bind_demo_tasks(os_tcb_t *control_task, os_tcb_t *worker_task)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "bool hal_board_issue_reschedule_probe(void)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "bool hal_board_get_balance_peer(os_cpu_t current_cpu, os_cpu_t *peer_cpu)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "uint32_t hal_board_balance_expected_mask(void)",
        "hal/src/hal_board.c",
    )
    require(
        hal_board_c,
        "const char *hal_board_task_map_description(bool smp_mode)",
        "hal/src/hal_board.c",
    )
    require(beu_demo, "hal_board_run_selftest()", "apps/be_u1000_demo/main.c")
    require(beu_demo, '#include "hal_board_smp.h"', "apps/be_u1000_demo/main.c")
    require(
        beu_demo,
        "hal_board_bind_demo_tasks(&control_task_tcb, &worker_task_tcb)",
        "apps/be_u1000_demo/main.c",
    )
    require(beu_demo, "hal_board_issue_reschedule_probe()", "apps/be_u1000_demo/main.c")
    require(
        beu_demo,
        "hal_board_get_balance_peer(self->cpu_id, &peer)",
        "apps/be_u1000_demo/main.c",
    )
    require(beu_demo, "hal_board_balance_expected_mask()", "apps/be_u1000_demo/main.c")
    require(
        beu_demo, "hal_board_task_map_description(true)", "apps/be_u1000_demo/main.c"
    )
    require(
        beu_demo, "hal_board_task_map_description(false)", "apps/be_u1000_demo/main.c"
    )
    forbid(beu_demo, "CANFD0_IRQ_SEEN", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "CANFD1_IRQ_SEEN", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "g_canfd_profiles[0]", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "g_canfd_profiles[1]", "apps/be_u1000_demo/main.c")
    forbid(
        beu_demo,
        "for (index = 0; index < HAL_BOARD_CANFD_CONTROLLER_COUNT; ++index)",
        "apps/be_u1000_demo/main.c",
    )
    forbid(beu_demo, "hal_spi_transfer(0xA5u", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "hal_flash_read_u32(0u", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "window sample: 0x%x 0x%x 0x%x 0x%x", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "config.internal_loopback = true;", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "loopback=1)", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "lane_bias =", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "cpu_id == 0 ? 1 : 0", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "os_smp_cpu_online(1)", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "os_ipi_send(1, OS_IPI_RESCHEDULE)", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "g_execution_profile.control_cpu", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "g_execution_profile.worker_cpu", "apps/be_u1000_demo/main.c")
    forbid(
        beu_demo,
        "g_execution_profile.reschedule_probe_cpu",
        "apps/be_u1000_demo/main.c",
    )
    forbid(
        beu_demo, "g_execution_profile.balance_probe_cpu_a", "apps/be_u1000_demo/main.c"
    )
    forbid(
        beu_demo, "g_execution_profile.balance_probe_cpu_b", "apps/be_u1000_demo/main.c"
    )
    forbid(beu_demo, "g_execution_profile.task_map_smp", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "g_execution_profile.task_map_single", "apps/be_u1000_demo/main.c")
    forbid(
        beu_demo,
        "os_task_set_affinity(&control_task_tcb, 0)",
        "apps/be_u1000_demo/main.c",
    )
    forbid(
        beu_demo,
        "os_task_set_affinity(&worker_task_tcb, 1)",
        "apps/be_u1000_demo/main.c",
    )
    forbid(
        beu_demo,
        "hal_board_get_selftest_profile(&selftest_profile);",
        "apps/be_u1000_demo/main.c",
    )
    forbid(beu_demo, "static void run_gpio_selftest(", "apps/be_u1000_demo/main.c")
    forbid(
        beu_demo, "static void run_serial_bus_selftest(", "apps/be_u1000_demo/main.c"
    )
    forbid(beu_demo, "static void run_flash_selftest(", "apps/be_u1000_demo/main.c")
    forbid(beu_demo, "static void run_canfd_selftest(", "apps/be_u1000_demo/main.c")

    print("BSP seam checks passed.")


if __name__ == "__main__":
    main()
