#!/usr/bin/env sh
set -eu

repo_root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
cache_path="${1:-"$repo_root/build-be_u1000/CMakeCache.txt"}"
build_ninja="${2:-"$(dirname "$cache_path")/build.ninja"}"
build_dir="$(dirname "$cache_path")"

expected_march="rv32imafc_zifencei"
expected_mabi="ilp32f"
expected_compile_march="rv32imafc_zifencei_zicsr"

fail() {
    printf 'ERROR: %s\n' "$1" >&2
    exit 1
}

require_cache_entry() {
    name="$1"
    expected="$2"
    if ! grep -Eq "^${name}:[A-Z]+=${expected}$" "$cache_path"; then
        actual="$(grep -E "^${name}:" "$cache_path" || true)"
        fail "expected ${name}=${expected} in ${cache_path}; found: ${actual:-<missing>}"
    fi
}

match_generated_flags() {
    pattern="$1"
    [ -d "$build_dir" ] || return 1
    find "$build_dir" \( -name flags.make -o -name link.txt \) \
        -exec grep -E "$pattern" {} + 2>/dev/null | grep -q .
}

require_flag() {
    flag="$1"
    if ! grep -Eq "^[^#].*${flag}" "$cache_path" && \
       { [ ! -f "$build_ninja" ] || ! grep -Eq "^[^#].*${flag}" "$build_ninja"; } && \
       ! match_generated_flags "^[^#].*${flag}"; then
        fail "expected ${flag} in configured CMake flags under ${build_dir}"
    fi
}

[ -f "$cache_path" ] || fail "CMake cache not found: ${cache_path}"

require_cache_entry "CONFIG_BOARD" "be_u1000"
require_cache_entry "RISCV_MARCH" "$expected_march"
require_cache_entry "RISCV_MABI" "$expected_mabi"
require_flag "-march=${expected_compile_march}"
require_flag "-mabi=${expected_mabi}"

if grep -Eq "^[^#].*(-march=rv32imafd|-mabi=ilp32d)" "$cache_path" || \
   { [ -f "$build_ninja" ] && grep -Eq "^[^#].*(-march=rv32imafd|-mabi=ilp32d)" "$build_ninja"; } || \
   match_generated_flags "^[^#].*(-march=rv32imafd|-mabi=ilp32d)"; then
    fail "BE-U1000 cache still contains unsupported rv32imafd/ilp32d flags"
fi

printf 'BE-U1000 ABI cache checks passed.\n'
