#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re


METRICS_RE = re.compile(
    r"AI_VALIDATION_METRICS: sample=(?P<sample>\S+) idx=(?P<idx>\d+) label=(?P<label>\d+) argmax=(?P<argmax>\d+) hash=(?P<hash>\d+) top_score_q=(?P<top_score_q>-?\d+) latency_us=(?P<latency_us>\d+) total=(?P<total>\d+) arena_peak=(?P<arena_peak>\d+)"
)


def parse_log(path: Path) -> list[dict[str, int | str]]:
    text = path.read_text(encoding="utf-8", errors="ignore")
    if "AI_VALIDATION_PASS" not in text:
        raise ValueError(f"missing pass token in {path}")

    records: list[dict[str, int | str]] = []
    for match in METRICS_RE.finditer(text):
        record: dict[str, int | str] = {"sample": match.group("sample")}
        for key in ("idx", "label", "argmax", "hash", "top_score_q", "latency_us", "total", "arena_peak"):
            record[key] = int(match.group(key))
        records.append(record)

    if not records:
        raise ValueError(f"missing metrics lines in {path}")

    return records


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--qemu-log", default="logs/mnist_validation_qemu.log")
    parser.add_argument("--renode-log", default="logs/mnist_validation_renode.log")
    args = parser.parse_args()

    repo_root = Path(__file__).resolve().parent.parent
    qemu_log = (repo_root / args.qemu_log).resolve()
    renode_log = (repo_root / args.renode_log).resolve()

    if not qemu_log.exists() or not renode_log.exists():
        print("AI validation cross-check error: one or more logs are missing")
        return 2

    try:
        qemu_records = parse_log(qemu_log)
        renode_records = parse_log(renode_log)
    except ValueError as exc:
        print(f"AI validation cross-check error: {exc}")
        return 1

    if len(qemu_records) != len(renode_records):
        print(
            "AI validation cross-check mismatch: sample count "
            f"qemu={len(qemu_records)} renode={len(renode_records)}"
        )
        return 1

    for qemu_record, renode_record in zip(qemu_records, renode_records):
        for field in ("sample", "idx", "label", "argmax", "hash", "top_score_q", "total"):
            if qemu_record[field] != renode_record[field]:
                print(
                    "AI validation cross-check mismatch: "
                    f"sample={qemu_record['sample']} field={field} "
                    f"qemu={qemu_record[field]} renode={renode_record[field]}"
                )
                return 1

    print(
        "AI_VALIDATION_CROSSCHECK_OK "
        f"samples={len(qemu_records)} first_sample={qemu_records[0]['sample']} "
        f"qemu_latency_us={qemu_records[0]['latency_us']} "
        f"renode_latency_us={renode_records[0]['latency_us']}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
