#!/usr/bin/env python3
import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description="Check that the BE-U1000 AI demo binary fits in eFlash."
    )
    parser.add_argument("--binary", required=True, help="Path to rrtos_be_u1000.bin")
    parser.add_argument(
        "--max-bytes",
        type=int,
        default=262144,
        help="Maximum allowed binary size in bytes.",
    )
    args = parser.parse_args()

    binary = Path(args.binary)
    size = binary.stat().st_size

    if size > args.max_bytes:
        print(
            f"BE_U1000_AI_DEMO_SIZE_FAIL bytes={size} max={args.max_bytes} "
            f"over={size - args.max_bytes}"
        )
        return 1

    print(
        f"BE_U1000_AI_DEMO_SIZE_PASS bytes={size} max={args.max_bytes} "
        f"headroom={args.max_bytes - size}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
