#!/bin/bash
# Quick test script for AI Model Registry
# Tests multiple EmitC models (MNIST + YOLO)

set -e

cd "$(dirname "$0")/.."

echo "=========================================="
echo "  AI Model Registry Test"
echo "=========================================="
echo ""

# Step 1: Build
echo "[1/2] Building..."
pixi run -e rv32 build

# Step 2: Test
echo "[2/2] Running test in QEMU..."
echo ""
echo "=========================================="
pixi run -e rv32 test-registry 2>&1 | tee /tmp/ai_registry_test.log
echo "=========================================="
echo ""
echo "✓ Test complete!"
echo "Log saved to: /tmp/ai_registry_test.log"
