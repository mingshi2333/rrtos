#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUT_DIR="${1:-${PROJECT_ROOT}/build/rrtos_ai_ukernel}"

CLANG="${CLANG:-clang}"
RRTOS_AI_UKERNEL_RISCV_MARCH="${RRTOS_AI_UKERNEL_RISCV_MARCH:-rv32imafc_zifencei_zicsr}"
RRTOS_AI_UKERNEL_RISCV_ABI="${RRTOS_AI_UKERNEL_RISCV_ABI:-ilp32f}"
SRC="${PROJECT_ROOT}/ai/ukernel/rrtos_ai_ukernel.c"

mkdir -p "${OUT_DIR}"

COMMON_FLAGS=(
  --target=riscv32-unknown-elf
  -march=${RRTOS_AI_UKERNEL_RISCV_MARCH}
  -mabi=${RRTOS_AI_UKERNEL_RISCV_ABI}
  -Oz
  -ffreestanding
  -fno-builtin
  -Wall
  -Wextra
  -Werror
  -I"${PROJECT_ROOT}/ai/ukernel"
)

"${CLANG}" "${COMMON_FLAGS[@]}" -emit-llvm -c "${SRC}" \
  -o "${OUT_DIR}/rrtos_ai_ukernel_riscv32.bc"

"${CLANG}" "${COMMON_FLAGS[@]}" -c "${SRC}" \
  -o "${OUT_DIR}/rrtos_ai_ukernel_riscv32.o"

echo "Wrote ${OUT_DIR}/rrtos_ai_ukernel_riscv32.bc"
echo "Wrote ${OUT_DIR}/rrtos_ai_ukernel_riscv32.o"
