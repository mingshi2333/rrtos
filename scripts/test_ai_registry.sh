#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd -- "$(dirname "$0")" && pwd)
REPO_ROOT=$(cd -- "$SCRIPT_DIR/.." && pwd)

cd "$REPO_ROOT"

echo "=========================================="
echo "  Supported AI Runtime Validation"
echo "=========================================="
echo

echo "[1/1] Running supported rv32 AI validation..."
pixi run -e rv32 validate-supported-rv32

echo
echo "Supported AI runtime validation complete."
