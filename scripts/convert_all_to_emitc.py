#!/usr/bin/env python3
"""Convert all TFLite models from RIOT-ML to ONNX, then compile to EmitC with IREE."""

import os
import sys
from pathlib import Path
import subprocess

MODEL_ZOO_DIR = "/tmp/RIOT-ML/model_zoo"
ONNX_DIR = "/home/mingshi/Project/PF/rtos/onnx_models"
EMITC_DIR = "/home/mingshi/Project/PF/rtos/emitc_models"


def tflite_to_onnx(tflite_path: Path) -> Path:
    """Convert TFLite model to ONNX using tflite2onnx."""
    import tflite2onnx

    onnx_path = Path(ONNX_DIR) / f"{tflite_path.stem}.onnx"

    try:
        tflite2onnx.convert(str(tflite_path), str(onnx_path))
        return onnx_path
    except Exception as e:
        print(f"  Error converting to ONNX: {e}")
        return None


def onnx_to_emitc(onnx_path: Path) -> bool:
    """Compile ONNX model to EmitC using IREE."""
    c_path = Path(EMITC_DIR) / f"{onnx_path.stem}.c"
    vmfb_path = Path(EMITC_DIR) / f"{onnx_path.stem}.vmfb"

    try:
        result = subprocess.run(
            [
                "mamba",
                "run",
                "-n",
                "iree-compiler",
                "iree-compile",
                str(onnx_path),
                "--iree-input-type=onnx",
                "--iree-hal-target-backends=vmvx",
                "--output-format=vm-c",
                "-o",
                str(c_path),
            ],
            capture_output=True,
            text=True,
            timeout=120,
        )

        if result.returncode == 0:
            print(f"  ✓ EmitC output: {c_path.name}")
            return True
        else:
            print(f"  ✗ IREE compilation failed: {result.stderr[:200]}")
            return False

    except subprocess.TimeoutExpired:
        print(f"  ✗ Compilation timeout")
        return False
    except Exception as e:
        print(f"  ✗ Exception: {e}")
        return False


def main():
    os.makedirs(ONNX_DIR, exist_ok=True)
    os.makedirs(EMITC_DIR, exist_ok=True)

    tflite_files = sorted(Path(MODEL_ZOO_DIR).glob("*.tflite"))

    print(f"=== TFLite → ONNX → EmitC Compilation ===")
    print(f"Found {len(tflite_files)} TFLite models\n")

    stats = {"total": len(tflite_files), "onnx_ok": 0, "emitc_ok": 0, "failed": 0}

    for idx, tflite_path in enumerate(tflite_files, 1):
        print(f"[{idx}/{len(tflite_files)}] {tflite_path.name}")

        onnx_path = tflite_to_onnx(tflite_path)
        if not onnx_path:
            stats["failed"] += 1
            continue

        print(f"  → ONNX: {onnx_path.name}")
        stats["onnx_ok"] += 1

        if onnx_to_emitc(onnx_path):
            stats["emitc_ok"] += 1
        else:
            stats["failed"] += 1

        print()

    print("=== Summary ===")
    print(f"Total models: {stats['total']}")
    print(f"ONNX converted: {stats['onnx_ok']}")
    print(f"EmitC compiled: {stats['emitc_ok']}")
    print(f"Failed: {stats['failed']}")
    print(f"\nOutput: {EMITC_DIR}")

    return 0 if stats["failed"] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
