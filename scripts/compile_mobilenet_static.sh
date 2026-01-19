#!/bin/bash
set -e

# Configuration
MODEL_URL="https://storage.googleapis.com/download.tensorflow.org/models/tflite/mobilenet_v1_1.0_224_quant.tflite"
MODEL_PATH="/tmp/mobilenet_quant.tflite"
OUTPUT_DIR="zoo/iree_static"
OUTPUT_NAME="mobilenet_static"
IREE_COMPILE="/home/mingshi/.mamba/envs/iree-toolchain310/bin/iree-compile"

mkdir -p "$OUTPUT_DIR"

# Download model
if [ ! -f "$MODEL_PATH" ]; then
    echo "Downloading MobileNet..."
    wget -q "$MODEL_URL" -O "$MODEL_PATH"
fi

# Compile
echo "Compiling MobileNet for RV32..."
"$IREE_COMPILE" "$MODEL_PATH" \
    --iree-input-type=tosa \
    --iree-hal-target-backends=llvm-cpu \
    --iree-llvmcpu-target-triple=riscv32-unknown-unknown-elf \
    --iree-llvmcpu-target-cpu=generic-rv32 \
    --iree-llvmcpu-target-abi=ilp32 \
    --iree-llvmcpu-target-cpu-features="+m,+a,+c" \
    --output-format=vm-c \
    --iree-vm-c-module-name="$OUTPUT_NAME" \
    --iree-vm-c-module-optimize=false \
    -o "$OUTPUT_DIR/${OUTPUT_NAME}_c_module.c"

echo "Compilation complete: $OUTPUT_DIR/${OUTPUT_NAME}_c_module.c"
