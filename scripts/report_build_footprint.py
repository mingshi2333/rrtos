#!/usr/bin/env python3

import argparse
import subprocess
from datetime import datetime, timezone
from pathlib import Path


def run(cmd):
    return subprocess.run(cmd, check=True, capture_output=True, text=True)


def parse_size_table(output):
    sections = []
    total = None
    for raw in output.splitlines():
        line = raw.strip()
        if not line or line.startswith("section"):
            continue
        parts = line.split()
        if len(parts) < 2:
            continue
        name = parts[0]
        if name == "Total" and len(parts) >= 2:
            try:
                total = int(parts[1])
            except ValueError:
                pass
            continue
        try:
            size = int(parts[1])
        except ValueError:
            continue
        sections.append((name, size))
    return sections, total


def parse_archive_totals(output):
    text = data = bss = dec = 0
    for raw in output.splitlines():
        line = raw.strip()
        if not line or line.startswith("text"):
            continue
        parts = line.split()
        if len(parts) < 5:
            continue
        try:
            text += int(parts[0])
            data += int(parts[1])
            bss += int(parts[2])
            dec += int(parts[3])
        except ValueError:
            continue
    return text, data, bss, dec


def parse_cache(cache_path):
    keys = {
        "CONFIG_BOARD": None,
        "ARCH_BITS": None,
        "OS_SMP_EN": None,
        "CMAKE_BUILD_TYPE": None,
        "BE_U1000_MEMORY_MODEL": None,
    }
    if not cache_path.exists():
        return keys
    for raw in cache_path.read_text(encoding="utf-8").splitlines():
        if raw.startswith("//") or raw.startswith("#") or "=" not in raw:
            continue
        left, value = raw.split("=", 1)
        key = left.split(":", 1)[0]
        if key in keys:
            keys[key] = value
    return keys


def format_rows(rows):
    return "\n".join(f"| {name} | {size} |" for name, size in rows)


def main():
    parser = argparse.ArgumentParser(
        description="Generate a footprint markdown summary for a build artifact."
    )
    parser.add_argument("--label", required=True)
    parser.add_argument("--image", required=True)
    parser.add_argument("--kernel-archive", required=True)
    parser.add_argument("--cache", required=True)
    parser.add_argument("--linker-script", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--map", required=False)
    parser.add_argument("--max-image-bytes", type=int, required=False)
    parser.add_argument("--max-kernel-dec-bytes", type=int, required=False)
    args = parser.parse_args()

    image = Path(args.image)
    kernel_archive = Path(args.kernel_archive)
    cache_path = Path(args.cache)
    output_path = Path(args.output)
    map_path = Path(args.map) if args.map else None

    image_size = run(["llvm-size", "--format=sysv", str(image)]).stdout
    image_rows, image_total = parse_size_table(image_size)

    kernel_size = run(["llvm-size", str(kernel_archive)]).stdout
    kernel_totals = parse_archive_totals(kernel_size)
    budget_failures = []

    if args.max_image_bytes is not None and (image_total or 0) > args.max_image_bytes:
        budget_failures.append(
            f"image total {image_total or 0} exceeds max {args.max_image_bytes}"
        )

    if (
        args.max_kernel_dec_bytes is not None
        and kernel_totals[3] > args.max_kernel_dec_bytes
    ):
        budget_failures.append(
            f"kernel dec {kernel_totals[3]} exceeds max {args.max_kernel_dec_bytes}"
        )

    cache_values = parse_cache(cache_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    lines = [
        f"# Footprint Report: {args.label}",
        "",
        f"Generated: {datetime.now(timezone.utc).isoformat()}",
        "",
        "## Inputs",
        "",
        f"- Image: `{image}`",
        f"- Kernel archive: `{kernel_archive}`",
        f"- Cache: `{cache_path}`",
        f"- Linker script: `{args.linker_script}`",
        f"- Map file: `{map_path}` ({'present' if map_path and map_path.exists() else 'missing'})",
        "",
        "## Build configuration",
        "",
    ]

    for key, value in cache_values.items():
        if value is not None:
            lines.append(f"- {key}: `{value}`")

    lines.extend(
        [
            "",
            "## Image sections",
            "",
            "| Section | Bytes |",
            "| --- | ---: |",
            format_rows(image_rows),
            f"| Total | {image_total or 0} |",
            "",
            "## Kernel archive aggregate",
            "",
            "| Field | Bytes |",
            "| --- | ---: |",
            f"| text | {kernel_totals[0]} |",
            f"| data | {kernel_totals[1]} |",
            f"| bss | {kernel_totals[2]} |",
            f"| dec | {kernel_totals[3]} |",
            "",
            "## Budgets",
            "",
            "| Budget | Limit | Actual | Status |",
            "| --- | ---: | ---: | --- |",
            (
                f"| image total | {args.max_image_bytes} | {image_total or 0} | "
                f"{'PASS' if args.max_image_bytes is None or (image_total or 0) <= args.max_image_bytes else 'FAIL'} |"
            ),
            (
                f"| kernel dec | {args.max_kernel_dec_bytes} | {kernel_totals[3]} | "
                f"{'PASS' if args.max_kernel_dec_bytes is None or kernel_totals[3] <= args.max_kernel_dec_bytes else 'FAIL'} |"
            ),
            "",
            "## Regeneration",
            "",
            f"```bash\nllvm-size --format=sysv {image}\nllvm-size {kernel_archive}\n```",
            "",
        ]
    )

    output_path.write_text("\n".join(lines), encoding="utf-8")

    if budget_failures:
        raise SystemExit("Footprint budget failed: " + "; ".join(budget_failures))


if __name__ == "__main__":
    main()
