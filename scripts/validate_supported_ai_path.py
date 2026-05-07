#!/usr/bin/env python3

from pathlib import Path
import sys


REPO_ROOT = Path(__file__).resolve().parent.parent


def fail(message: str) -> None:
    print(f"ERROR: {message}")
    sys.exit(1)


def main() -> None:
    ai_canonical = (REPO_ROOT / "docs/AI_CANONICAL_PATH.md").read_text(encoding="utf-8")
    mnist_readme = (REPO_ROOT / "apps/mnist_app/README.md").read_text(encoding="utf-8")
    pixi = (REPO_ROOT / "pixi.toml").read_text(encoding="utf-8")
    validation_main = (REPO_ROOT / "apps/mnist_app/src/validation_main.c").read_text(
        encoding="utf-8"
    )
    ai_yaml = (REPO_ROOT / "ai_models.yaml").read_text(encoding="utf-8")
    if "/home/" in ai_yaml:
        fail("ai_models.yaml contains a machine-local absolute path")

    codegen = (REPO_ROOT / "scripts/ai_codegen.py").read_text(encoding="utf-8")
    if "mamba/envs/iree-toolchain" in codegen:
        fail("scripts/ai_codegen.py contains a hardcoded toolchain path")

    legacy_header = (REPO_ROOT / "ai/include/ai_runtime.h").read_text(encoding="utf-8")
    if "deprecated" not in legacy_header:
        fail("ai/include/ai_runtime.h is not marked deprecated")

    if "ai/include/ai_runtime.h" in ai_canonical:
        fail("docs/AI_CANONICAL_PATH.md still treats ai_runtime.h as supported")

    if "ai_runtime.h" in validation_main:
        fail("apps/mnist_app/src/validation_main.c includes legacy ai_runtime.h")

    required_validation_markers = [
        'ai_model_find_by_name("st_mnist_28")',
        "ai_model_get_input_info(handle, 0u, &input_spec)",
        "ai_model_get_output_info(handle, 0u, &output_spec)",
        "ai_infer_sync(handle, &input_tensor, 1u, &output_tensor, 1u, 0u)",
    ]
    for marker in required_validation_markers:
        if marker not in validation_main:
            fail(
                f"apps/mnist_app/src/validation_main.c is missing supported registry marker: {marker}"
            )

    if (
        "The supported validation entrypoint is `apps/mnist_app/src/validation_main.c`"
        not in ai_canonical
    ):
        fail(
            "docs/AI_CANONICAL_PATH.md does not name validation_main.c as the supported validation entrypoint"
        )

    if (
        "Generated wrappers such as `ai_st_mnist_28_run()` remain convenience glue"
        not in ai_canonical
    ):
        fail(
            "docs/AI_CANONICAL_PATH.md does not demote generated wrappers to convenience glue"
        )

    if (
        "The supported proof path uses `ai_model_find_by_name()`, `ai_model_get_input_info()`, `ai_model_get_output_info()`, and `ai_infer_sync()` directly"
        not in mnist_readme
    ):
        fail(
            "apps/mnist_app/README.md does not document the supported registry proof path"
        )

    maintained_task_surface = (
        "The maintained AI task surface intentionally stays small: "
        "`validate-supported-ai`, `validate-mnist-runtime`, `validate-mnist-quant-runtime`, `observe-mnist-runtime-renode`, and `compare-mnist-runtime-platforms`."
    )
    if maintained_task_surface not in mnist_readme:
        fail("apps/mnist_app/README.md does not lock the maintained AI task surface")

    if (
        "The maintained pixi AI task surface is `validate-supported-ai`, `validate-mnist-runtime`, `validate-mnist-quant-runtime`, `observe-mnist-runtime-renode`, and `compare-mnist-runtime-platforms`; redundant composite aliases should not be reintroduced."
        not in ai_canonical
    ):
        fail(
            "docs/AI_CANONICAL_PATH.md does not lock the maintained AI pixi task surface"
        )

    required_pixi_markers = [
        'validate-supported-ai = "python scripts/validate_supported_ai_path.py"',
        'validate-mnist-runtime = "python scripts/run_mnist_validation.py --kernel build/apps/mnist_app/mnist_validation --log logs/mnist_validation_qemu.log"',
        'validate-mnist-quant-runtime = "python scripts/run_mnist_quant_validation.py --kernel build/apps/mnist_app/mnist_quant_validation --log logs/mnist_quant_validation_qemu.log"',
        'observe-mnist-runtime-renode = "python scripts/run_mnist_validation_renode.py --kernel build/apps/mnist_app/mnist_validation --log logs/mnist_validation_renode.log"',
        'compare-mnist-runtime-platforms = "python scripts/compare_mnist_validation_logs.py --qemu-log logs/mnist_validation_qemu.log --renode-log logs/mnist_validation_renode.log"',
    ]
    for marker in required_pixi_markers:
        if marker not in pixi:
            fail(f"pixi.toml is missing maintained AI task marker: {marker}")

    forbidden_pixi_markers = ["validate-mnist-runtime-dual ="]
    for marker in forbidden_pixi_markers:
        if marker in pixi:
            fail(f"pixi.toml still exposes stale AI task marker: {marker}")

    if "sim-ai =" in pixi or "sim-renode-ai =" in pixi:
        fail(
            "pixi.toml still exposes legacy ai_demo simulation tasks on the maintained task surface"
        )

    generated_models = (REPO_ROOT / "apps/mnist_app/generated/ai_models.c").read_text(
        encoding="utf-8"
    )
    if '"st_mnist_28"' not in generated_models:
        fail(
            "apps/mnist_app/generated/ai_models.c is missing the canonical st_mnist_28 model"
        )

    generated_quant_models = (
        REPO_ROOT / "apps/mnist_app/generated_quant/ai_models.c"
    ).read_text(encoding="utf-8")
    if '"st_mnistv1_28_tfs_int8"' not in generated_quant_models:
        fail(
            "apps/mnist_app/generated_quant/ai_models.c is missing the quantized st_mnistv1_28_tfs_int8 model"
        )

    root_cmake = (REPO_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
    if "add_subdirectory(apps/mnist_app)" not in root_cmake:
        fail("supported mnist app is not built in the default non-board path")
    if "RRTOS_BUILD_EXPERIMENTAL_APPS" not in root_cmake:
        fail("experimental app gating is missing from CMakeLists.txt")

    mnist_cmake = (REPO_ROOT / "apps/mnist_app/CMakeLists.txt").read_text(
        encoding="utf-8"
    )
    if "mnist_quant_validation" not in mnist_cmake:
        fail("apps/mnist_app/CMakeLists.txt is missing mnist_quant_validation")
    if "rv_aios_mnist_quant_models" not in mnist_cmake:
        fail("apps/mnist_app/CMakeLists.txt is missing the separated quantized model library")

    validation_samples = (
        REPO_ROOT / "apps/mnist_app/src/mnist_validation_samples.h"
    ).read_text(encoding="utf-8")
    if "MNIST_VALIDATION_SAMPLE_COUNT" not in validation_samples:
        fail("mnist validation sample set header is missing")
    if "dataset_index" not in validation_samples or "label" not in validation_samples:
        fail("mnist validation sample set is missing dataset metadata")

    validation_generator = (
        REPO_ROOT / "scripts/generate_mnist_validation_samples.py"
    ).read_text(encoding="utf-8")
    if "storage.googleapis.com/cvdf-datasets/mnist" not in validation_generator:
        fail("mnist validation sample generator is not pinned to the official dataset")

    print("Supported AI path checks passed.")


if __name__ == "__main__":
    main()
