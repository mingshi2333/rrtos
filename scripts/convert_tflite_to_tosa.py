#!/usr/bin/env python3
"""Convert TFLite models to TOSA MLIR format for IREE compilation."""

import os
import subprocess
import sys
from pathlib import Path

MODEL_ZOO_DIR = "/tmp/RIOT-ML/model_zoo"
OUTPUT_DIR = "/home/mingshi/Project/PF/rtos/tosa_models"


def convert_tflite_to_tosa(tflite_path: str, output_path: str) -> bool:
    """Convert a TFLite model to TOSA MLIR format using iree-import-tflite."""
    try:
        result = subprocess.run(
            [
                "mamba",
                "run",
                "-n",
                "iree-compiler",
                "python",
                "-m",
                "iree.compiler.tools.import_tflite",
                "--output-format=tosa",
                tflite_path,
                "-o",
                output_path,
            ],
            capture_output=True,
            text=True,
            timeout=60,
        )

        if result.returncode == 0:
            return True
        else:
            print(f"  Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"  Exception: {e}")
        return False


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    tflite_files = sorted(Path(MODEL_ZOO_DIR).glob("*.tflite"))

    print(f"Found {len(tflite_files)} TFLite models")
    print(f"Output directory: {OUTPUT_DIR}\n")

    success_count = 0
    failed_count = 0

    for idx, tflite_path in enumerate(tflite_files, 1):
        model_name = tflite_path.stem
        tosa_path = Path(OUTPUT_DIR) / f"{model_name}.mlir"

        print(f"[{idx}/{len(tflite_files)}] Converting {model_name}...")

        if convert_tflite_to_tosa(str(tflite_path), str(tosa_path)):
            print(f"  ✓ Success: {tosa_path}")
            success_count += 1
        else:
            print(f"  ✗ Failed")
            failed_count += 1

    print(f"\n===== Summary =====")
    print(f"Total: {len(tflite_files)}")
    print(f"Success: {success_count}")
    print(f"Failed: {failed_count}")

    return 0 if failed_count == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
