#!/bin/bash
set -e

# Configuration
QEMU_BIN="qemu-system-riscv32"
BINARY="build/apps/ai_demo/ai_demo"
MACHINE="virt"
CPU="rv32"
MEMORY="128M" # QEMU memory size (larger than physical to ensure it runs)

# Check for QEMU
if ! command -v $QEMU_BIN &> /dev/null; then
    echo "Error: $QEMU_BIN not found in PATH."
    echo "Please install QEMU or check your environment."
    exit 1
fi

# Check for Binary
if [ ! -f "$BINARY" ]; then
    echo "Error: Binary $BINARY not found."
    echo "Please run 'pixi run build' first."
    exit 1
fi

echo "=============================================="
echo " Starting AI Demo Simulation (QEMU)"
echo " Binary: $BINARY"
echo " Machine: $MACHINE"
echo " CPU: $CPU"
echo "=============================================="

# Run QEMU
# -nographic: No GUI, serial output to console
# -bios none: Bare metal mode
# -kernel: Load our ELF binary directly
# -machine virt: Generic RISC-V Virtual Machine
# -m: Memory size
$QEMU_BIN \
    -nographic \
    -machine $MACHINE \
    -cpu $CPU \
    -m $MEMORY \
    -bios none \
    -kernel "$BINARY"

echo ""
echo "Simulation Finished."
