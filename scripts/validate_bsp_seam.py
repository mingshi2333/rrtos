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
    hal_board_c = (REPO_ROOT / "hal/src/hal_board.c").read_text(encoding="utf-8")

    forbid(beu_demo, '"board_config.h"', "apps/be_u1000_demo/main.c")
    forbid(mnist_main, '"board_config.h"', "apps/mnist_app/src/main.c")
    forbid(mnist_validation, '"board_config.h"', "apps/mnist_app/src/validation_main.c")

    require(
        hal_board_h,
        "typedef struct {\n    hal_board_pinmux_group_t console_pinmux_group;",
        "hal/include/hal_board.h",
    )
    require(hal_board_h, "uint32_t canfd_count;", "hal/include/hal_board.h")
    require(hal_board_h, "uint8_t spi_probe_tx;", "hal/include/hal_board.h")
    require(hal_board_h, "uint32_t window_sample_offset;", "hal/include/hal_board.h")
    require(hal_board_h, "uint32_t window_sample_words;", "hal/include/hal_board.h")
    require(hal_board_h, "bool internal_loopback;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uintptr_t gpio_base;", "hal/include/hal_board.h")
    forbid(hal_board_h, "uint32_t gpio_pin;", "hal/include/hal_board.h")
    forbid(hal_board_h, "} hal_board_gpio_role_t;", "hal/include/hal_board.h")
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
    require(
        hal_board_h,
        "void hal_board_get_selftest_profile(hal_board_selftest_profile_t *profile);",
        "hal/include/hal_board.h",
    )
    require(
        hal_board_c,
        "void hal_board_get_selftest_profile(hal_board_selftest_profile_t *profile)",
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
    forbid(hal_board_c, "config->gpio_base", "hal/src/hal_board.c")
    forbid(hal_board_c, "config->gpio_pin", "hal/src/hal_board.c")
    require(
        beu_demo,
        "hal_board_get_selftest_profile(&selftest_profile);",
        "apps/be_u1000_demo/main.c",
    )
    require(beu_demo, "static void run_gpio_selftest(", "apps/be_u1000_demo/main.c")
    require(
        beu_demo, "static void run_serial_bus_selftest(", "apps/be_u1000_demo/main.c"
    )
    require(beu_demo, "static void run_flash_selftest(", "apps/be_u1000_demo/main.c")
    require(beu_demo, "static void run_canfd_selftest(", "apps/be_u1000_demo/main.c")
    require(
        hal_board_h,
        "const char *window_capture_note;",
        "hal/include/hal_board.h",
    )
    require(
        hal_board_h,
        "const char *identify_capture_note;",
        "hal/include/hal_board.h",
    )
    require(hal_board_h, "uint8_t payload_seed;", "hal/include/hal_board.h")
    require(hal_board_h, "const char *route;", "hal/include/hal_board.h")
    require(
        beu_demo,
        'selftest_label(flash_profile->window_capture_note, "sample captured")',
        "apps/be_u1000_demo/main.c",
    )
    require(
        beu_demo,
        'selftest_label(flash_profile->identify_capture_note, "sample captured")',
        "apps/be_u1000_demo/main.c",
    )
    require(beu_demo, "hal_gpio_init(button_gpio->base);", "apps/be_u1000_demo/main.c")
    require(beu_demo, "profile->payload_seed + i", "apps/be_u1000_demo/main.c")
    require(beu_demo, "[SELFTEST] %s route: %s", "apps/be_u1000_demo/main.c")
    require(beu_demo, "HAL_BOARD_CANFD_CONTROLLER_COUNT", "apps/be_u1000_demo/main.c")
    require(beu_demo, "CANFD_IRQ_SEEN(index)", "apps/be_u1000_demo/main.c")
    require(beu_demo, "profile->canfd_count", "apps/be_u1000_demo/main.c")
    require(beu_demo, "g_canfd_profile_count", "apps/be_u1000_demo/main.c")
    require(beu_demo, "diag_config->spi_probe_tx", "apps/be_u1000_demo/main.c")
    require(
        beu_demo, "flash_profile->window_sample_offset", "apps/be_u1000_demo/main.c"
    )
    require(beu_demo, "flash_profile->window_sample_words", "apps/be_u1000_demo/main.c")
    require(beu_demo, "report_flash_sample(", "apps/be_u1000_demo/main.c")
    require(beu_demo, "profile->internal_loopback", "apps/be_u1000_demo/main.c")
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

    print("BSP seam checks passed.")


if __name__ == "__main__":
    main()
