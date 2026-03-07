#!/usr/bin/env python3

from __future__ import annotations

import gzip
from pathlib import Path
import struct
import urllib.request


IMAGE_URL = (
    "https://storage.googleapis.com/cvdf-datasets/mnist/t10k-images-idx3-ubyte.gz"
)
LABEL_URL = (
    "https://storage.googleapis.com/cvdf-datasets/mnist/t10k-labels-idx1-ubyte.gz"
)
OUTPUT = Path("apps/mnist_app/src/mnist_validation_samples.h")

SAMPLES = [
    ("mnist_t10k_00000", 0, 7),
    ("mnist_t10k_00001", 1, 2),
    ("mnist_t10k_00002", 2, 1),
    ("mnist_t10k_00003", 3, 0),
    ("mnist_t10k_00004", 4, 4),
]


def fetch(url: str) -> bytes:
    with urllib.request.urlopen(url, timeout=30) as response:
        return response.read()


def load_dataset() -> tuple[bytes, bytes]:
    labels_gz = fetch(LABEL_URL)
    images_gz = fetch(IMAGE_URL)

    labels_raw = gzip.decompress(labels_gz)
    images_raw = gzip.decompress(images_gz)

    label_magic, label_count = struct.unpack(">II", labels_raw[:8])
    image_magic, image_count, rows, cols = struct.unpack(">IIII", images_raw[:16])

    if label_magic != 2049 or image_magic != 2051:
        raise ValueError("Unexpected MNIST magic header")
    if label_count != image_count or rows != 28 or cols != 28:
        raise ValueError("Unexpected MNIST dimensions")

    return labels_raw[8:], images_raw[16:]


def format_pixels(pixels: bytes) -> str:
    lines = []
    for offset in range(0, len(pixels), 16):
        chunk = ", ".join(str(value) for value in pixels[offset : offset + 16])
        suffix = "," if offset + 16 < len(pixels) else ""
        lines.append(f"            {chunk}{suffix}")
    return "\n".join(lines)


def build_header(labels: bytes, images: bytes) -> str:
    entries = []
    for sample_id, dataset_index, label in SAMPLES:
        actual_label = labels[dataset_index]
        if actual_label != label:
            raise ValueError(
                f"Label mismatch for {sample_id}: expected {label}, got {actual_label}"
            )

        start = dataset_index * 784
        pixels = images[start : start + 784]
        if len(pixels) != 784:
            raise ValueError(f"Sample {sample_id} is incomplete")

        entries.append(
            "    {\n"
            f'        .id = "{sample_id}",\n'
            f"        .dataset_index = {dataset_index}u,\n"
            f"        .label = {label}u,\n"
            "        .pixels = {\n"
            f"{format_pixels(pixels)}\n"
            "        }\n"
            "    }"
        )

    entries_text = ",\n".join(entries)

    return f'''#ifndef MNIST_VALIDATION_SAMPLES_H
#define MNIST_VALIDATION_SAMPLES_H

#include <stddef.h>
#include <stdint.h>

#define MNIST_VALIDATION_SAMPLESET_NAME "mnist_t10k_official"
#define MNIST_VALIDATION_SAMPLESET_IMAGE_URL "{IMAGE_URL}"
#define MNIST_VALIDATION_SAMPLESET_LABEL_URL "{LABEL_URL}"

typedef struct {{
    const char *id;
    uint32_t dataset_index;
    uint32_t label;
    uint8_t pixels[784];
}} mnist_validation_sample_t;

static const mnist_validation_sample_t k_mnist_validation_samples[] = {{
{entries_text}
}};

#define MNIST_VALIDATION_SAMPLE_COUNT \
    (sizeof(k_mnist_validation_samples) / sizeof(k_mnist_validation_samples[0]))

#endif
'''


def main() -> None:
    labels, images = load_dataset()
    OUTPUT.write_text(build_header(labels, images), encoding="utf-8")
    print(f"Wrote {OUTPUT}")


if __name__ == "__main__":
    main()
