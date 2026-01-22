#!/bin/bash
# tflite_to_iree_c.sh
# Automates the conversion of TFLite models to IREE Bare-Metal C modules AND VM Bytecode modules

set -e

# ================= Configuration =================
IREE_TOOLCHAIN_ROOT="/home/mingshi/.mamba/envs/iree-toolchain310/bin"
SCRIPT_DIR=$(cd -- "$(dirname "$0")" && pwd)
OUTPUT_DIR="$SCRIPT_DIR/../iree_static"
TEMP_DIR="/tmp/iree_build_temp"

ARCH="rv32"
# =================================================

while getopts "a:" opt; do
  case $opt in
    a) ARCH=$OPTARG ;;
    *) echo "Usage: $0 [-a rv32|rv64] <input.tflite> <output_name>"; exit 1 ;;
  esac
done
shift $((OPTIND-1))

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 [-a rv32|rv64] <input.tflite> <output_name>"
    exit 1
fi

INPUT_TFLITE=$1
OUTPUT_NAME=$2

# Create necessary directories based on OUTPUT_NAME structure
OUTPUT_SUBDIR=$(dirname "$OUTPUT_NAME")
if [ "$OUTPUT_SUBDIR" != "." ]; then
    mkdir -p "$TEMP_DIR/$OUTPUT_SUBDIR"
    mkdir -p "$OUTPUT_DIR/$OUTPUT_SUBDIR"
fi

if [ "$ARCH" = "rv64" ]; then
    echo "Configuring for RV64..."
    TARGET_TRIPLE="riscv64-unknown-elf"
    TARGET_ABI="lp64d"
    CPU_FEATURES="+m,+a,+f,+d,+zicsr"
    VM_INDEX_BITS=64
    OUTPUT_OBJ_NAME="${OUTPUT_NAME}_rv64.o"
    BYTECODE_OBJ_NAME="${OUTPUT_NAME}_bytecode_rv64.o"
else
    echo "Configuring for RV32..."
    TARGET_TRIPLE="riscv32-unknown-elf"
    TARGET_ABI="ilp32d"
    CPU_FEATURES="+m,+a,+f,+d,+zicsr"
    VM_INDEX_BITS=32
    OUTPUT_OBJ_NAME="${OUTPUT_NAME}.o"
    BYTECODE_OBJ_NAME="${OUTPUT_NAME}_bytecode.o"
fi

# Ensure base directories exist
mkdir -p "$OUTPUT_DIR"
mkdir -p "$TEMP_DIR"

echo "=== Starting Conversion: $INPUT_TFLITE -> $OUTPUT_NAME ==="

echo "[1/3] Importing TFLite to MLIR..."
"$IREE_TOOLCHAIN_ROOT/iree-import-tflite" \
    "$INPUT_TFLITE" \
    -o "$TEMP_DIR/$OUTPUT_NAME.mlir"

echo "[2/3] Optimizing & Stripping Signedness (ui8 -> i8)..."
"$IREE_TOOLCHAIN_ROOT/iree-opt" \
    --pass-pipeline='builtin.module(func.func(iree-tosa-strip-signedness))' \
    "$TEMP_DIR/$OUTPUT_NAME.mlir" \
    -o "$TEMP_DIR/${OUTPUT_NAME}_opt.mlir"

# Rename module to avoid collision
sed -i "s/^module /module @${OUTPUT_NAME} /" "$TEMP_DIR/${OUTPUT_NAME}_opt.mlir"

echo "[3/3] Compiling to EmitC + Object ($ARCH) [Optimized]..."
"$IREE_TOOLCHAIN_ROOT/iree-compile" \
    --iree-hal-target-backends=llvm-cpu \
    --iree-llvmcpu-target-triple="$TARGET_TRIPLE" \
    --iree-llvmcpu-target-cpu-features="$CPU_FEATURES" \
    --iree-llvmcpu-target-abi="$TARGET_ABI" \
    --output-format=vm-c \
    --iree-vm-target-index-bits=$VM_INDEX_BITS \
    --iree-llvmcpu-link-embedded=false \
    --iree-llvmcpu-link-static \
    --iree-llvmcpu-static-library-output-path="$OUTPUT_DIR/$OUTPUT_OBJ_NAME" \
    --iree-llvmcpu-loop-unrolling=false \
    --iree-llvmcpu-enable-ukernels=all \
    --iree-stream-partitioning-favor=min-peak-memory \
    --iree-stream-resource-alias-mutable-bindings \
    --iree-stream-resource-index-bits=32 \
    --iree-stream-resource-memory-model=unified \
    --iree-stream-resource-max-allocation-size=1048576 \
    --iree-flow-inline-constants-max-byte-length=0 \
    --iree-llvmcpu-debug-symbols=false \
    "$TEMP_DIR/${OUTPUT_NAME}_opt.mlir" \
    -o "$OUTPUT_DIR/$OUTPUT_NAME.h"

echo "[4/4] Compiling to VM Bytecode + Static Library..."
"$IREE_TOOLCHAIN_ROOT/iree-compile" \
    --iree-hal-target-backends=llvm-cpu \
    --iree-llvmcpu-target-triple="$TARGET_TRIPLE" \
    --iree-llvmcpu-target-cpu-features="$CPU_FEATURES" \
    --iree-llvmcpu-target-abi="$TARGET_ABI" \
    --output-format=vm-bytecode \
    --iree-vm-target-index-bits=$VM_INDEX_BITS \
    --iree-llvmcpu-link-embedded=false \
    --iree-llvmcpu-static-library-output-path="$OUTPUT_DIR/$BYTECODE_OBJ_NAME" \
    --iree-llvmcpu-loop-unrolling=false \
    --iree-llvmcpu-enable-ukernels=all \
    --iree-stream-partitioning-favor=min-peak-memory \
    --iree-stream-resource-alias-mutable-bindings \
    --iree-stream-resource-index-bits=32 \
    --iree-stream-resource-memory-model=unified \
    --iree-stream-resource-max-allocation-size=1048576 \
    --iree-llvmcpu-debug-symbols=false \
    "$TEMP_DIR/${OUTPUT_NAME}_opt.mlir" \
    -o "$OUTPUT_DIR/${OUTPUT_NAME}.vmfb"

echo "Converting .vmfb to C array..."
xxd -i "$OUTPUT_DIR/${OUTPUT_NAME}.vmfb" > "$OUTPUT_DIR/${OUTPUT_NAME}_vmfb.h"

# Sanitize variable name: replace any non-alphanumeric chars (like /) with _
SAFE_NAME=$(echo "$OUTPUT_NAME" | sed 's/[^a-zA-Z0-9_]/_/g')

sed -i "s/^unsigned char.*\[\]/const unsigned char ${SAFE_NAME}_vmfb[]/" "$OUTPUT_DIR/${OUTPUT_NAME}_vmfb.h"
sed -i "s/^unsigned int.*len/unsigned int ${SAFE_NAME}_vmfb_len/" "$OUTPUT_DIR/${OUTPUT_NAME}_vmfb.h"

rm -rf "$TEMP_DIR"

echo "=== Conversion Successful! ==="
echo "--------------------------------------------------------"
echo "MODE 1: EmitC (C Code Generation)"
echo "  Header: $OUTPUT_DIR/$OUTPUT_NAME.h"
echo "  Object: $OUTPUT_DIR/$OUTPUT_OBJ_NAME"
echo "--------------------------------------------------------"
echo "MODE 2: VM Bytecode (Runtime Interpretation)"
echo "  VMFB:   $OUTPUT_DIR/${OUTPUT_NAME}.vmfb"
echo "  Header: $OUTPUT_DIR/${OUTPUT_NAME}_vmfb.h"
echo "  Object: $OUTPUT_DIR/$BYTECODE_OBJ_NAME"
echo "--------------------------------------------------------"
echo "Next Steps:"
echo "1. Ensure your linker script handles the .o files correctly."
echo "2. Link against the matching ukernel bitcode (compiled earlier)."
echo "3. Use -Wl,--icf=all to deduplicate code."
