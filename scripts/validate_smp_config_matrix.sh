#!/usr/bin/env sh
set -eu

repo_root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cc_bin="${CC:-cc}"
be_u1000_smp_build_dir="$repo_root/build/validate_smp_config_matrix/be_u1000_smp_$$"

preprocess_config() {
    printf '#include "config/os_config.h"\n' | "$cc_bin" \
        -E -P -x c - \
        -I"$repo_root" \
        -I"$repo_root/config" \
        -I"$repo_root/boards/be_u1000" \
        "$@" >/dev/null
}

expect_success() {
    name="$1"
    shift
    if preprocess_config "$@"; then
        printf '[PASS] %s\n' "$name"
    else
        printf '[FAIL] %s: expected config to preprocess successfully\n' "$name" >&2
        exit 1
    fi
}

expect_failure() {
    name="$1"
    shift
    if preprocess_config "$@" 2>/dev/null; then
        printf '[FAIL] %s: expected config validation to fail\n' "$name" >&2
        exit 1
    else
        printf '[PASS] %s\n' "$name"
    fi
}

expect_build_success() {
    name="$1"
    shift
    if "$@"; then
        printf '[PASS] %s\n' "$name"
    else
        printf '[FAIL] %s: expected build-only lane to succeed\n' "$name" >&2
        exit 1
    fi
}

expect_success "qemu single-core fallback" \
    -DOS_CFG_SMP_EN=0 \
    -DOS_CFG_CPU_MAX=1 \
    -DOS_CFG_CPU_COUNT=1

expect_success "be-u1000 single-core supported lane" \
    -DCONFIG_BOARD_BE_U1000=1 \
    -DOS_CFG_SMP_EN=0 \
    -DOS_CFG_CPU_MAX=3 \
    -DOS_CFG_CPU_COUNT=1

expect_success "be-u1000 experimental dual-core lane" \
    -DCONFIG_BOARD_BE_U1000=1 \
    -DOS_CFG_SMP_EN=1 \
    -DOS_CFG_CPU_MAX=3 \
    -DOS_CFG_CPU_COUNT=2

expect_failure "generic cpu count must be at least one" \
    -DOS_CFG_SMP_EN=0 \
    -DOS_CFG_CPU_MAX=1 \
    -DOS_CFG_CPU_COUNT=0

expect_failure "generic cpu count cannot exceed max" \
    -DOS_CFG_SMP_EN=1 \
    -DOS_CFG_CPU_MAX=1 \
    -DOS_CFG_CPU_COUNT=2

expect_failure "single-core lane cannot advertise multiple active cpus" \
    -DOS_CFG_SMP_EN=0 \
    -DOS_CFG_CPU_MAX=3 \
    -DOS_CFG_CPU_COUNT=2

expect_failure "smp lane requires at least two active cpus" \
    -DOS_CFG_SMP_EN=1 \
    -DOS_CFG_CPU_MAX=2 \
    -DOS_CFG_CPU_COUNT=1

expect_failure "be-u1000 max cannot exceed physical cores" \
    -DCONFIG_BOARD_BE_U1000=1 \
    -DOS_CFG_SMP_EN=1 \
    -DOS_CFG_CPU_MAX=4 \
    -DOS_CFG_CPU_COUNT=2

expect_failure "be-u1000 active cpus cannot exceed main scheduling cores" \
    -DCONFIG_BOARD_BE_U1000=1 \
    -DOS_CFG_SMP_EN=1 \
    -DOS_CFG_CPU_MAX=3 \
    -DOS_CFG_CPU_COUNT=3

cmake -E rm -rf "$be_u1000_smp_build_dir"
trap 'cmake -E rm -rf "$be_u1000_smp_build_dir"' EXIT INT TERM

expect_build_success "be-u1000 experimental smp build-only lane configures" \
    cmake -S "$repo_root" -B "$be_u1000_smp_build_dir" \
        -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-pixi.cmake \
        -DARCH_BITS=32 \
        -DCONFIG_BOARD=be_u1000 \
        -DRISCV_MARCH=rv32imafc_zifencei \
        -DRISCV_MABI=ilp32f \
        -DRISCV_ABI=ilp32d \
        -DCMAKE_BUILD_TYPE=MinSizeRel \
        -DOS_SMP_EN=ON \
        -DOS_AI_EN=OFF

expect_build_success "be-u1000 experimental smp build-only lane builds" \
    cmake --build "$be_u1000_smp_build_dir"

printf 'SMP config matrix checks passed.\n'
