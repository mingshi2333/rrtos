#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", required=True)
    parser.add_argument("--min-lines", type=int, default=3)
    args = parser.parse_args()

    log_path = Path(args.log)
    if not log_path.exists():
        print(f"USB_RUNTIME_CHECK_ERROR: log not found: {log_path}")
        return 2

    text = log_path.read_text(encoding="utf-8", errors="ignore")
    required = [
        "RV-AIOS v1.0.0 on BE-U1000",
        "[USB_APP] RUN_MARKER:",
        "[USB_APP] usb_runtime ready",
        "[USB_APP] Initializing kernel...",
        "[USB_APP] Starting scheduler...",
    ]

    failures: list[str] = []
    for token in required:
        if token not in text:
            failures.append(f"missing token: {token}")

    lines = [
        line
        for line in text.splitlines()
        if "[USB_APP] tick " in line
        and "intr=0x" in line
        and "intrtx=0x" in line
        and "idx=" in line
        and "txmaxp=0x" in line
        and "rxmaxp=0x" in line
        and "csr0=0x" in line
        and "count0=" in line
        and "txcsr=0x" in line
        and "rxcsr=0x" in line
        and "rxcount=" in line
        and "frame=" in line
        and "view=" in line
        and "role=" in line
        and "maxp=0x" in line
        and "csrsel=" in line
        and "csr-valid=" in line
        and "countsel=" in line
        and "count-valid=" in line
        and "ready=" in line
    ]
    if len(lines) < args.min_lines:
        failures.append(
            f"expected at least {args.min_lines} usb lines, found {len(lines)}"
        )

    if "cfg=0x00000000" in text:
        failures.append(
            "usb configdata stayed zero; semantic stub did not surface controller capabilities"
        )

    if lines and not any(
        "intr=0x00000010" in line or "intr=0x00000018" in line for line in lines
    ):
        failures.append("usb lane never reported connection/sof interrupt semantics")

    if lines and not any("power=0x00000060" in line for line in lines):
        failures.append("usb lane never reported soft-connect / high-power state")

    if lines and not any(
        "idx=0" in line and ("intrtx=0x0000000c" in line or "intrtx=0x000c" in line)
        for line in lines
    ):
        failures.append(
            "usb lane never showed EP0/EP2 interrupt cadence on idx0 snapshots"
        )

    if lines and not any(
        "idx=1" in line and ("intrtx=0x00000006" in line or "intrtx=0x0006" in line)
        for line in lines
    ):
        failures.append("usb lane never showed idx1 INTRTX cadence")

    if lines and not any(
        "idx=2" in line and ("intrtx=0x00000002" in line or "intrtx=0x0002" in line)
        for line in lines
    ):
        failures.append("usb lane never showed idx2 INTRTX cadence")

    if lines and not any(
        "idx=3" in line and ("intrtx=0x00000008" in line or "intrtx=0x0008" in line)
        for line in lines
    ):
        failures.append("usb lane never showed idx3 INTRTX cadence")

    if lines and not any(
        "idx=1" in line and ("txmaxp=0x00000040" in line or "txmaxp=0x0040" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced idx1 max-packet sizing")

    if lines and not any(
        "idx=2" in line and ("txmaxp=0x00000020" in line or "txmaxp=0x0020" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced idx2 max-packet sizing")

    if lines and not any(
        "idx=3" in line and ("txmaxp=0x00000010" in line or "txmaxp=0x0010" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced idx3 max-packet sizing")

    if lines and not any(
        "idx=0" in line and ("count0=8" in line or "count0=0x8" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced EP0 count0 data-ready semantics")

    if lines and not any(
        "idx=0" in line
        and ("txmaxp=0x0008" in line or "txmaxp=0x00000008" in line)
        and ("csr0=0x0009" in line or "csr0=0x00000009" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced indexed EP0 CSR-style state")

    if lines and not any(
        "idx=2" in line and ("rxcsr=0x0003" in line or "rxcsr=0x00000003" in line)
        for line in lines
    ):
        failures.append(
            "usb lane never surfaced indexed RX CSR packet-ready progression"
        )

    if lines and not any(
        ("idx=1" in line or "idx=3" in line)
        and ("txcsr=0x0003" in line or "txcsr=0x00000003" in line)
        for line in lines
    ):
        failures.append("usb lane never surfaced indexed endpoint CSR progression")

    if lines and not any(
        "idx=2" in line
        and (
            ("rxcsr=0x0003" in line or "rxcsr=0x00000003" in line)
            and ("rxcount=24" in line or "rxcount=0x18" in line)
        )
        for line in lines
    ):
        failures.append(
            "usb lane never linked indexed RX packet-ready state to count progression"
        )

    if lines and not any(
        "idx=0" in line
        and "view=ep0" in line
        and "role=control" in line
        and "csrsel=csr0" in line
        and "countsel=count0" in line
        and "csr-valid=1" in line
        and "count-valid=1" in line
        and "ready=1" in line
        for line in lines
    ):
        failures.append("usb lane never exposed semantic EP0 control-window readiness")

    if lines and not any(
        "idx=1" in line
        and "view=txrx" in line
        and "role=tx" in line
        and ("maxp=0x00000040" in line or "maxp=0x40" in line)
        and "csrsel=txcsr" in line
        and "csr-valid=1" in line
        and "count-valid=0" in line
        and "ready=1" in line
        for line in lines
    ):
        failures.append("usb lane never exposed semantic TX endpoint readiness")

    if lines and not any(
        "idx=2" in line
        and "view=txrx" in line
        and "role=rx" in line
        and ("maxp=0x00000020" in line or "maxp=0x20" in line)
        and "csrsel=rxcsr" in line
        and "countsel=rxcount" in line
        and "csr-valid=1" in line
        and "count-valid=1" in line
        and "ready=1" in line
        and ("count=24" in line or "count=0x18" in line)
        for line in lines
    ):
        failures.append(
            "usb lane never exposed semantic RX endpoint packet-ready state"
        )

    if "FAIL" in text:
        failures.append("found FAIL marker in log")

    if failures:
        for failure in failures:
            print(f"USB_RUNTIME_CHECK_FAIL: {failure}")
        return 1

    print(f"USB_RUNTIME_CHECK_OK: found {len(lines)} USB markers in {log_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
