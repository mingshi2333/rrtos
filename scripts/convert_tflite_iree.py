#!/usr/bin/env python3
"""Convert TFLite models to EmitC using TensorFlow + IREE pipeline."""

import os
import sys
import subprocess
from pathlib import Path
import tensorflow as tf

MODEL_ZOO_DIR = "/tmp/RIOT-ML/model_zoo"
STABLEHLO_DIR = "/home/mingshi/Project/PF/rtos/stablehlo_models"
EMITC_DIR = "/home/mingshi/Project/PF/rtos/emitc_models"


def tflite_to_saved_model(tflite_path: Path, output_dir: Path) -> bool:
    """Convert TFLite to SavedModel format."""
    try:
        interpreter = tf.lite.Interpreter(model_path=str(tflite_path))
        interpreter.allocate_tensors()

        input_details = interpreter.get_input_details()
        output_details = interpreter.get_output_details()

        print(f"  Input: {input_details[0]['shape']} {input_details[0]['dtype']}")
        print(f"  Output: {output_details[0]['shape']} {output_details[0]['dtype']}")

        return True

    except Exception as e:
        print(f"  Error loading TFLite: {e}")
        return False


def compile_to_emitc(tflite_path: Path) -> bool:
    """Compile TFLite directly to EmitC using iree-compile with experimental TFLite support."""
    c_path = Path(EMITC_DIR) / f"{tflite_path.stem}.c"
    mlir_path = Path(STABLEHLO_DIR) / f"{tflite_path.stem}.mlir"

    try:
        result = subprocess.run(
            [
                "mamba",
                "run",
                "-n",
                "iree-compiler",
                "iree-compile",
                str(tflite_path),
                "--iree-input-type=tosa",
                "--iree-hal-target-backends=vmvx",
                "--output-format=vm-c",
                "-o",
                str(c_path),
            ],
            capture_output=True,
            text=True,
            timeout=120,
        )

        if result.returncode == 0 and c_path.exists():
            size = c_path.stat().st_size
            print(f"  ✓ EmitC: {c_path.name} ({size} bytes)")
            return True
        else:
            stderr = result.stderr[:300] if result.stderr else "No error output"
            print(f"  ✗ Compilation failed: {stderr}")
            return False

    except subprocess.TimeoutExpired:
        print(f"  ✗ Timeout")
        return False
    except Exception as e:
        print(f"  ✗ Exception: {e}")
        return False


def main():
    os.makedirs(STABLEHLO_DIR, exist_ok=True)
    os.makedirs(EMITC_DIR, exist_ok=True)

    print(f"TensorFlow version: {tf.__version__}")
    print(f"=== TFLite → EmitC Compilation ===\n")

    tflite_files = sorted(Path(MODEL_ZOO_DIR).glob("*.tflite"))

    stats = {"total": len(tflite_files), "success": 0, "failed": 0}

    for idx, tflite_path in enumerate(tflite_files, 1):
        print(f"[{idx}/{len(tflite_files)}] {tflite_path.name}")

        if not tflite_to_saved_model(
            tflite_path, Path(STABLEHLO_DIR) / tflite_path.stem
        ):
            stats["failed"] += 1
            continue

        if compile_to_emitc(tflite_path):
            stats["success"] += 1
        else:
            stats["failed"] += 1

        print()

    print("=== Summary ===")
    print(f"Total: {stats['total']}")
    print(f"Success: {stats['success']}")
    print(f"Failed: {stats['failed']}")
    print(f"\nOutput: {EMITC_DIR}")

    if stats["success"] > 0:
        print("\nGenerated files:")
        for c_file in sorted(Path(EMITC_DIR).glob("*.c")):
            size = c_file.stat().st_size
            print(f"  {c_file.name}: {size:,} bytes")

    return 0 if stats["failed"] == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
