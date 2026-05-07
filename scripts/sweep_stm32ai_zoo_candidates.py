#!/usr/bin/env python3
"""Download and compile selected STM32AI Model Zoo candidates.

This is a measurement tool, not a production app integration path. It keeps each
candidate in its own build directory so generated wrappers and objects do not
overwrite the canonical MNIST validation artifacts.
"""

from __future__ import annotations

import argparse
import csv
import shutil
import subprocess
import time
import urllib.parse
import urllib.request
from dataclasses import dataclass
from pathlib import Path

import yaml


PROJECT_ROOT = Path(__file__).resolve().parent.parent
ST_ZOO_BASE_URL = (
    "https://github.com/STMicroelectronics/stm32ai-modelzoo/raw/main"
)
SUPPORTED_SOURCE_TYPES = {".tflite", ".mlir"}


@dataclass(frozen=True)
class Candidate:
    name: str
    upstream_path: str
    license_hint: str
    note: str

    @property
    def filename(self) -> str:
        return self.upstream_path.rsplit("/", 1)[-1]

    @property
    def suffix(self) -> str:
        return Path(self.filename).suffix.lower()

    @property
    def url(self) -> str:
        quoted_path = urllib.parse.quote(self.upstream_path, safe="/")
        return f"{ST_ZOO_BASE_URL}/{quoted_path}"


CANDIDATES = (
    Candidate(
        name="st_mnistv1_28_tfs_int8",
        upstream_path=(
            "image_classification/st_mnistv1/ST_pretrainedmodel_public_dataset/"
            "emnist_byclass/st_mnistv1_28_tfs/st_mnistv1_28_tfs_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Already integrated quant MNIST baseline.",
    ),
    Candidate(
        name="st_dense_freq_1channel_512_int8",
        upstream_path=(
            "arc_fault_detection/st_dense/ST_pretrainedmodel_custom_dataset/"
            "afd_test_bench_dataset/st_dense_freq_1channel_512/"
            "st_dense_freq_1channel_512_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Tiny dense arc-fault candidate.",
    ),
    Candidate(
        name="st_conv_freq_1channel_512_int8",
        upstream_path=(
            "arc_fault_detection/st_conv/ST_pretrainedmodel_custom_dataset/"
            "afd_test_bench_dataset/st_conv_freq_1channel_512/"
            "st_conv_freq_1channel_512_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Small convolutional arc-fault candidate.",
    ),
    Candidate(
        name="miniresnetv1_s1_64x50_tl_int8",
        upstream_path=(
            "audio_event_detection/miniresnetv1/ST_pretrainedmodel_public_dataset/"
            "esc10/miniresnetv1_s1_64x50_tl/"
            "miniresnetv1_s1_64x50_tl_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Small audio event model.",
    ),
    Candidate(
        name="miniresnetv2_s1_64x50_tl_int8",
        upstream_path=(
            "audio_event_detection/miniresnetv2/ST_pretrainedmodel_public_dataset/"
            "esc10/miniresnetv2_s1_64x50_tl/"
            "miniresnetv2_s1_64x50_tl_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Second MiniResNet audio-event architecture.",
    ),
    Candidate(
        name="yamnet_e256_64x96_tl_int8",
        upstream_path=(
            "audio_event_detection/yamnet/ST_pretrainedmodel_public_dataset/"
            "esc10/yamnet_e256_64x96_tl/yamnet_e256_64x96_tl_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Audio event YAMNet transfer-learning model.",
    ),
    Candidate(
        name="blazeface_front_128_int8",
        upstream_path=(
            "face_detection/facedetect_front/Public_pretrainedmodel_public_dataset/"
            "widerface/blazeface_front_128/blazeface_front_128_int8.tflite"
        ),
        license_hint="Apache-2.0/public-domain",
        note="Front-face detection candidate.",
    ),
    Candidate(
        name="fdmobilenet_a025_128_tfs_int8",
        upstream_path=(
            "image_classification/fdmobilenet/ST_pretrainedmodel_public_dataset/"
            "tf_flowers/fdmobilenet_a025_128_tfs/"
            "fdmobilenet_a025_128_tfs_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Small image-classification candidate.",
    ),
    Candidate(
        name="mobilenetv1_a025_96_int8",
        upstream_path=(
            "image_classification/mobilenetv1/Public_pretrainedmodel_public_dataset/"
            "visual_wake_word/mobilenetv1_a025_96/mobilenetv1_a025_96_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Visual Wake Words TinyML benchmark candidate.",
    ),
    Candidate(
        name="mobilenetv2_a035_128_int8",
        upstream_path=(
            "image_classification/mobilenetv2/Public_pretrainedmodel_public_dataset/"
            "ImageNet/mobilenetv2_a035_128/"
            "mobilenetv2_a035_128_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="ImageNet MobileNetV2 width 0.35 at 128px.",
    ),
    Candidate(
        name="resnet8_32_tfs_int8",
        upstream_path=(
            "image_classification/resnet/ST_pretrainedmodel_public_dataset/"
            "cifar10/resnet8_32_tfs/resnet8_32_tfs_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Small CIFAR-10 ResNet image-classification candidate.",
    ),
    Candidate(
        name="squeezenetv11_128_tfs_int8",
        upstream_path=(
            "image_classification/squeezenetv11/ST_pretrainedmodel_public_dataset/"
            "tf_flowers/squeezenetv11_128_tfs/"
            "squeezenetv11_128_tfs_int8.tflite"
        ),
        license_hint="SLA0044",
        note="SqueezeNet image-classification candidate.",
    ),
    Candidate(
        name="fastdepth_224_int8",
        upstream_path=(
            "depth_estimation/fastdepth/Public_pretrainedmodel_public_dataset/"
            "nyu_depthv2/fastdepth_224/fastdepth_224_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Depth-estimation candidate with a different output shape profile.",
    ),
    Candidate(
        name="st_yololcv1_192_int8",
        upstream_path=(
            "object_detection/st_yololcv1/ST_pretrainedmodel_public_dataset/"
            "coco_2017_person/st_yololcv1_192/st_yololcv1_192_int8.tflite"
        ),
        license_hint="SLA0044",
        note="Small person-detection candidate.",
    ),
    Candidate(
        name="handlandmarks_full_224_int8",
        upstream_path=(
            "pose_estimation/handlandmarks/Public_pretrainedmodel_custom_dataset/"
            "custom_dataset_hands_21kpts/handlandmarks_full_224_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Hand landmark pose-estimation candidate.",
    ),
    Candidate(
        name="st_movenet_lightning_a100_heatmaps_192_int8",
        upstream_path=(
            "pose_estimation/movenet/ST_pretrainedmodel_public_dataset/"
            "coco_person_17kpts/st_movenet_lightning_a100_heatmaps_192/"
            "st_movenet_lightning_a100_heatmaps_192_int8.tflite"
        ),
        license_hint="Apache-2.0",
        note="Person pose-estimation heatmap candidate.",
    ),
    Candidate(
        name="mobilenetv2_a035_128_qdq_w4w8a8",
        upstream_path=(
            "image_classification/mobilenetv2/Public_pretrainedmodel_public_dataset/"
            "ImageNet/mobilenetv2_a035_128/"
            "mobilenetv2_a035_128_qdq_w4_85.64%_w8_14.36%_a8_100%_acc_43.53.onnx"
        ),
        license_hint="Apache-2.0",
        note="Mixed-precision ONNX candidate; current codegen does not import ONNX.",
    ),
)


def candidate_by_name(name: str) -> Candidate:
    for candidate in CANDIDATES:
        if candidate.name == name:
            return candidate
    names = ", ".join(candidate.name for candidate in CANDIDATES)
    raise ValueError(f"Unknown candidate {name!r}; available: {names}")


def classify_candidate(candidate: Candidate) -> str:
    if candidate.suffix not in SUPPORTED_SOURCE_TYPES:
        return "unsupported_source_type"
    return "supported"


def local_model_path(candidate: Candidate, models_dir: Path) -> Path:
    return models_dir / candidate.filename


def path_for_config(path: Path) -> str:
    try:
        return str(path.relative_to(PROJECT_ROOT))
    except ValueError:
        return str(path)


def model_file_for_config(path: Path) -> str:
    return str(path)


def download_candidate(candidate: Candidate, models_dir: Path, force: bool = False) -> Path:
    models_dir.mkdir(parents=True, exist_ok=True)
    destination = local_model_path(candidate, models_dir)
    if destination.exists() and not force:
        return destination

    temporary = destination.with_suffix(destination.suffix + ".download")
    if temporary.exists():
        temporary.unlink()

    with urllib.request.urlopen(candidate.url, timeout=120) as response:
        with temporary.open("wb") as output:
            shutil.copyfileobj(response, output)

    temporary.replace(destination)
    return destination


def generated_config(
    candidate: Candidate,
    model_path: Path,
    output_dir: Path,
    iree_backend: str | None = None,
) -> dict:
    defaults = {
        "target_arch": "rv32",
        "target_abi": "ilp32f",
        "cpu_features": "+m,+a,+f,+zicsr",
        "enable_llvmcpu_microkernels": False,
        "enable_data_tiling": False,
        "enable_stream_memory_flags": False,
        "optimization": "size",
        "memory_strategy": "static",
    }
    if iree_backend:
        defaults["iree_backend"] = iree_backend

    return {
        "project_name": f"rrtos_stm32ai_zoo_{candidate.name}",
        "output_dir": path_for_config(output_dir),
        "model_library_name": f"rv_aios_st_zoo_{candidate.name}_models",
        "defaults": defaults,
        "toolchain": {
            "root_env": "IREE_TOOLCHAIN_ROOT",
            "tflite_toolchain_env": "iree-toolchain310",
        },
        "models": [
            {
                "name": candidate.name,
                "file": model_file_for_config(model_path),
            }
        ],
    }


def write_config(
    candidate: Candidate,
    model_path: Path,
    candidate_root: Path,
    iree_backend: str | None = None,
) -> Path:
    output_dir = candidate_root / "generated"
    output_dir.mkdir(parents=True, exist_ok=True)
    config_path = candidate_root / "config.yaml"
    config = generated_config(candidate, model_path, output_dir, iree_backend=iree_backend)
    with config_path.open("w", encoding="utf-8") as handle:
        yaml.safe_dump(config, handle, sort_keys=False)
    return config_path


def run_command(command: list[str], log_path: Path) -> tuple[int, float]:
    start = time.monotonic()
    result = subprocess.run(
        command,
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    elapsed = time.monotonic() - start
    log_path.write_text(
        "$ " + " ".join(command) + "\n\n"
        + "=== stdout ===\n"
        + result.stdout
        + "\n=== stderr ===\n"
        + result.stderr,
        encoding="utf-8",
    )
    return result.returncode, elapsed


def parse_size_output(output: str) -> dict[str, int]:
    lines = [line.split() for line in output.splitlines() if line.strip()]
    for columns in lines:
        if len(columns) >= 6 and columns[0] == "text":
            continue
        if len(columns) >= 6 and all(value.isdigit() for value in columns[:4]):
            return {
                "object_text_bytes": int(columns[0]),
                "object_data_bytes": int(columns[1]),
                "object_bss_bytes": int(columns[2]),
                "object_total_bytes": int(columns[3]),
            }
    return {
        "object_text_bytes": 0,
        "object_data_bytes": 0,
        "object_bss_bytes": 0,
        "object_total_bytes": 0,
    }


def collect_object_size(object_path: Path) -> dict[str, int]:
    size_tool = shutil.which("llvm-size") or shutil.which("size")
    if not size_tool or not object_path.exists():
        return {
            "object_text_bytes": 0,
            "object_data_bytes": 0,
            "object_bss_bytes": 0,
            "object_total_bytes": 0,
        }

    result = subprocess.run(
        [size_tool, str(object_path)],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        return {
            "object_text_bytes": 0,
            "object_data_bytes": 0,
            "object_bss_bytes": 0,
            "object_total_bytes": 0,
        }
    return parse_size_output(result.stdout)


def empty_result(candidate: Candidate) -> dict[str, str | int | float]:
    return {
        "name": candidate.name,
        "status": "",
        "source_type": candidate.suffix,
        "source_bytes": 0,
        "object_text_bytes": 0,
        "object_data_bytes": 0,
        "object_bss_bytes": 0,
        "object_total_bytes": 0,
        "header_bytes": 0,
        "wrapper_bytes": 0,
        "compile_seconds": 0.0,
        "license_hint": candidate.license_hint,
        "note": candidate.note,
    }


def run_candidate(
    candidate: Candidate,
    output_root: Path,
    models_dir: Path,
    iree_backend: str | None = None,
    force_download: bool = False,
    skip_compile: bool = False,
) -> dict[str, str | int | float]:
    result = empty_result(candidate)
    candidate_root = output_root / candidate.name
    candidate_root.mkdir(parents=True, exist_ok=True)

    try:
        model_path = download_candidate(candidate, models_dir, force=force_download)
        result["source_bytes"] = model_path.stat().st_size
    except Exception as exc:  # pragma: no cover - exercised during real network failures.
        result["status"] = "download_failed"
        result["note"] = f"{candidate.note}; download error: {exc}"
        return result

    classification = classify_candidate(candidate)
    if classification != "supported":
        result["status"] = classification
        result["note"] = (
            f"{candidate.note}; scripts/ai_codegen.py currently supports "
            ".tflite and .mlir only"
        )
        return result

    config_path = write_config(
        candidate,
        model_path,
        candidate_root,
        iree_backend=iree_backend,
    )
    if skip_compile:
        result["status"] = "skipped_compile"
        return result

    log_path = candidate_root / "codegen.log"
    returncode, elapsed = run_command(
        ["python3", "scripts/ai_codegen.py", "--config", str(config_path)],
        log_path,
    )
    result["compile_seconds"] = round(elapsed, 3)
    if returncode != 0:
        result["status"] = "compile_failed"
        result["note"] = f"{candidate.note}; see {log_path.relative_to(PROJECT_ROOT)}"
        return result

    generated_dir = candidate_root / "generated"
    object_path = generated_dir / f"{candidate.name}.o"
    header_path = generated_dir / f"{candidate.name}.h"
    wrapper_path = generated_dir / "ai_models.c"
    result.update(collect_object_size(object_path))
    result["header_bytes"] = header_path.stat().st_size if header_path.exists() else 0
    result["wrapper_bytes"] = wrapper_path.stat().st_size if wrapper_path.exists() else 0
    result["status"] = "compiled"
    return result


def write_csv(results: list[dict[str, str | int | float]], output_root: Path) -> Path:
    csv_path = output_root / "stm32ai_zoo_sweep.csv"
    fields = list(empty_result(CANDIDATES[0]).keys())
    with csv_path.open("w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        writer.writerows(results)
    return csv_path


def write_markdown(results: list[dict[str, str | int | float]], output_root: Path) -> Path:
    markdown_path = output_root / "stm32ai_zoo_sweep.md"
    lines = [
        "# STM32AI Zoo Sweep",
        "",
        "| Candidate | Status | Source | Object total | Text | Data | BSS | Compile s | Note |",
        "|---|---:|---:|---:|---:|---:|---:|---:|---|",
    ]
    for result in results:
        lines.append(
            "| {name} | {status} | {source_bytes} | {object_total_bytes} | "
            "{object_text_bytes} | {object_data_bytes} | {object_bss_bytes} | "
            "{compile_seconds} | {note} |".format(**result)
        )
    lines.append("")
    lines.append("Source repository: https://github.com/STMicroelectronics/stm32ai-modelzoo")
    markdown_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return markdown_path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Sweep selected STM32AI Model Zoo candidates through RRTOS codegen."
    )
    parser.add_argument(
        "--output-root",
        default=str(PROJECT_ROOT / "build" / "stm32ai_zoo_sweep"),
        help="Directory for generated configs, codegen output, logs, and reports.",
    )
    parser.add_argument(
        "--models-dir",
        default=str(PROJECT_ROOT / "models" / "stm32ai_zoo"),
        help="Directory used to cache downloaded upstream model artifacts.",
    )
    parser.add_argument(
        "--candidate",
        action="append",
        help="Candidate name to run. May be passed more than once. Defaults to all.",
    )
    parser.add_argument(
        "--force-download",
        action="store_true",
        help="Redownload model artifacts even when cached files already exist.",
    )
    parser.add_argument(
        "--skip-compile",
        action="store_true",
        help="Download and report source files without invoking ai_codegen.py.",
    )
    parser.add_argument(
        "--iree-backend",
        choices=("llvmcpu_static", "vmvx_inline"),
        help="Override defaults.iree_backend in generated candidate configs.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    output_root = Path(args.output_root).expanduser().resolve()
    models_dir = Path(args.models_dir).expanduser().resolve()
    output_root.mkdir(parents=True, exist_ok=True)

    selected = (
        [candidate_by_name(name) for name in args.candidate]
        if args.candidate
        else list(CANDIDATES)
    )

    results = []
    for candidate in selected:
        print(f"=== {candidate.name} ===")
        result = run_candidate(
            candidate,
            output_root=output_root,
            models_dir=models_dir,
            iree_backend=args.iree_backend,
            force_download=args.force_download,
            skip_compile=args.skip_compile,
        )
        results.append(result)
        print(
            f"{result['status']}: source={result['source_bytes']} "
            f"object={result['object_total_bytes']}"
        )

    csv_path = write_csv(results, output_root)
    markdown_path = write_markdown(results, output_root)
    print(f"Wrote {csv_path}")
    print(f"Wrote {markdown_path}")
    failed = [result for result in results if result["status"] == "download_failed"]
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
