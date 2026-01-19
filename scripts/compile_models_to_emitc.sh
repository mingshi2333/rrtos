#!/bin/bash

# Script to compile all TFLite models from RIOT-ML model_zoo to EmitC format using IREE
# Uses local IREE mamba environment: iree-compiler

set -e

MODEL_ZOO_DIR="/tmp/RIOT-ML/model_zoo"
OUTPUT_DIR="/home/mingshi/Project/PF/rtos/emitc_models"
LOG_FILE="$OUTPUT_DIR/compilation.log"

# Ensure output directory exists
mkdir -p "$OUTPUT_DIR"

# Initialize log
echo "===== IREE TFLite to EmitC Compilation Log =====" > "$LOG_FILE"
echo "Start time: $(date)" >> "$LOG_FILE"
echo "" >> "$LOG_FILE"

# Get all TFLite models
TFLITE_MODELS=$(find "$MODEL_ZOO_DIR" -name "*.tflite" | sort)

if [ -z "$TFLITE_MODELS" ]; then
    echo "ERROR: No TFLite models found in $MODEL_ZOO_DIR" | tee -a "$LOG_FILE"
    exit 1
fi

echo "Found models to compile:" | tee -a "$LOG_FILE"
echo "$TFLITE_MODELS" | tee -a "$LOG_FILE"
echo "" >> "$LOG_FILE"

# Compile each model
TOTAL=0
SUCCESS=0
FAILED=0

for MODEL_PATH in $TFLITE_MODELS; do
    TOTAL=$((TOTAL + 1))
    MODEL_NAME=$(basename "$MODEL_PATH" .tflite)
    
    echo "[$TOTAL] Compiling: $MODEL_NAME" | tee -a "$LOG_FILE"
    
    # Output files
    VMFB_OUTPUT="$OUTPUT_DIR/${MODEL_NAME}.vmfb"
    C_OUTPUT="$OUTPUT_DIR/${MODEL_NAME}.c"
    H_OUTPUT="$OUTPUT_DIR/${MODEL_NAME}.h"
    
    # Step 1: Compile TFLite to VMFB with vmvx backend
    echo "  -> Compiling to VMFB (vmvx backend)..." | tee -a "$LOG_FILE"
    if mamba run -n iree-compiler iree-compile \
        --iree-input-type=tflite \
        --iree-hal-target-backends=vmvx \
        "$MODEL_PATH" \
        -o "$VMFB_OUTPUT" 2>&1 | tee -a "$LOG_FILE"; then
        echo "  ✓ VMFB compilation successful" | tee -a "$LOG_FILE"
    else
        echo "  ✗ VMFB compilation failed for $MODEL_NAME" | tee -a "$LOG_FILE"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Step 2: Convert VMFB to vm-c (EmitC output)
    echo "  -> Converting to vm-c (EmitC)..." | tee -a "$LOG_FILE"
    if mamba run -n iree-compiler iree-compile \
        --iree-input-type=tflite \
        --iree-hal-target-backends=vmvx \
        --output-format=vm-c \
        --iree-vm-c-module-output-format=code \
        "$MODEL_PATH" \
        -o "$C_OUTPUT" 2>&1 | tee -a "$LOG_FILE"; then
        echo "  ✓ EmitC (vm-c) compilation successful" | tee -a "$LOG_FILE"
        SUCCESS=$((SUCCESS + 1))
        
        # Check file sizes
        VMFB_SIZE=$(stat -c%s "$VMFB_OUTPUT" 2>/dev/null || echo "0")
        C_SIZE=$(stat -c%s "$C_OUTPUT" 2>/dev/null || echo "0")
        echo "  - VMFB size: $VMFB_SIZE bytes" >> "$LOG_FILE"
        echo "  - C code size: $C_SIZE bytes" >> "$LOG_FILE"
    else
        echo "  ✗ EmitC (vm-c) compilation failed for $MODEL_NAME" | tee -a "$LOG_FILE"
        FAILED=$((FAILED + 1))
    fi
    
    echo "" >> "$LOG_FILE"
done

# Summary
echo "===== Compilation Summary =====" | tee -a "$LOG_FILE"
echo "Total models: $TOTAL" | tee -a "$LOG_FILE"
echo "Successful: $SUCCESS" | tee -a "$LOG_FILE"
echo "Failed: $FAILED" | tee -a "$LOG_FILE"
echo "End time: $(date)" | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
echo "Output directory: $OUTPUT_DIR" | tee -a "$LOG_FILE"
echo "Log file: $LOG_FILE" | tee -a "$LOG_FILE"

# List generated files
echo "" | tee -a "$LOG_FILE"
echo "Generated files:" | tee -a "$LOG_FILE"
ls -lh "$OUTPUT_DIR"/*.{c,vmfb} 2>/dev/null | tee -a "$LOG_FILE" || echo "No files generated" | tee -a "$LOG_FILE"

exit $FAILED
