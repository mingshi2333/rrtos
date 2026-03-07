#!/usr/bin/env python3

from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent


def fail(message: str) -> None:
    print(f"ERROR: {message}")
    sys.exit(1)


def main() -> None:
    ai_yaml = (REPO_ROOT / "ai_models.yaml").read_text(encoding="utf-8")
    if "/home/" in ai_yaml:
        fail("ai_models.yaml contains a machine-local absolute path")

    codegen = (REPO_ROOT / "scripts/ai_codegen.py").read_text(encoding="utf-8")
    if "mamba/envs/iree-toolchain" in codegen:
        fail("scripts/ai_codegen.py contains a hardcoded toolchain path")

    legacy_header = (REPO_ROOT / "ai/include/ai_runtime.h").read_text(encoding="utf-8")
    if "deprecated" not in legacy_header:
        fail("ai/include/ai_runtime.h is not marked deprecated")

    generated_models = (REPO_ROOT / "apps/mnist_app/generated/ai_models.c").read_text(
        encoding="utf-8"
    )
    if '"st_mnist_28"' not in generated_models:
        fail(
            "apps/mnist_app/generated/ai_models.c is missing the canonical st_mnist_28 model"
        )

    root_cmake = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    if "add_subdirectory(apps/mnist_app)" not in root_cmake:
        fail("supported mnist app is not built in the default non-board path")
    if "RRTOS_BUILD_EXPERIMENTAL_APPS" not in root_cmake:
        fail("experimental app gating is missing from CMakeLists.txt")

    validation_samples = (
        REPO_ROOT / "apps/mnist_app/src/mnist_validation_samples.h"
    ).read_text(encoding="utf-8")
    if "MNIST_VALIDATION_SAMPLE_COUNT" not in validation_samples:
        fail("mnist validation sample set header is missing")
    if ".expected_hash = 0u" in validation_samples:
        fail("mnist validation sample hashes are not fully baselined")

    validation_generator = (
        REPO_ROOT / "scripts/generate_mnist_validation_samples.py"
    ).read_text(encoding="utf-8")
    if "storage.googleapis.com/cvdf-datasets/mnist" not in validation_generator:
        fail("mnist validation sample generator is not pinned to the official dataset")

    print("Supported AI path checks passed.")


if __name__ == "__main__":
    main()
