# MNIST Descriptor Runtime QEMU Comparison

Date: 2026-05-07

Scope: QEMU-only `build-qemu-test` lane with `arch/riscv/link_qemu_test.ld` and `--memory 64M`.

## Result

`mnist_descriptor_validation` passed on QEMU:

```text
MNIST_DESCRIPTOR_PROBE_PASS ops=3 tensors=7
MNIST_DESCRIPTOR_STATIC_RAM: input_bytes=3136 arena_bytes=512 output_bytes=40 io_scratch_bytes=3688 task_stack_bytes=65536
MNIST_DESCRIPTOR_MODEL_PEAK: phase=invoke heap_current_bytes=0 heap_peak_bytes=0 heap_alloc_count=0 heap_free_count=0
MNIST_DESCRIPTOR_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_hex=0x3f7f6fb2 latency_ticks=6135
MNIST_DESCRIPTOR_PASS count=1
```

`scripts/check_no_iree_vm_symbols.py --map build-qemu-test/apps/mnist_app/mnist_descriptor_validation.map`
reported `NO_IREE_VM_SYMBOLS_PASS`.

## Flash Size

| Path | ELF text | ELF data | ELF bss | binary flash |
| --- | ---: | ---: | ---: | ---: |
| Descriptor op runtime | 412.3 KiB | 0.0 KiB | 87.8 KiB | 412.3 KiB |
| Static direct dispatch | 416.6 KiB | 0.1 KiB | 87.9 KiB | 416.7 KiB |
| VMVX inline | 465.3 KiB | 1.4 KiB | 85.6 KiB | 466.7 KiB |
| Original static/native | 554.1 KiB | 3.0 KiB | 85.7 KiB | 557.1 KiB |

Descriptor binary delta:

| Baseline | Delta |
| --- | ---: |
| vs static direct dispatch | -4.5 KiB (-1.1%) |
| vs VMVX inline | -54.4 KiB (-11.7%) |
| vs original static/native | -144.8 KiB (-26.0%) |

## Runtime And Peak Memory

| Path | Invoke heap peak | Persistent heap after init/invoke | Static IO/scratch | First-sample latency |
| --- | ---: | ---: | ---: | ---: |
| Descriptor op runtime | 0 B | 0 B | 3.6 KiB | 6135 CLINT ticks |
| Static direct dispatch | 0 B | 0 B | 3.7 KiB | 2726 CLINT ticks |
| VMVX inline | 6416 B | grows to 23656 B after 5 samples | runtime-managed | 3158 us first sample |
| Original static/native | 5824 B | 67312 B after init | runtime-managed | 408 us first sample |

## Notes

- The descriptor path does not call generated IREE dispatch functions. It reuses only the generated constant blob.
- The IREE sample MNIST constant blob layout used by the static dispatch object is not MLIR global order:
  - `fc2_bias` at byte offset `0`
  - `fc2_weights` at byte offset `64`
  - `fc1_bias` at byte offset `5184`
  - `fc1_weights` at byte offset `5696`
- The descriptor path is slightly smaller than the current static-direct path, but slower. That is expected for the first prototype because the reusable FC kernel is generic and not unrolled like IREE's per-dispatch generated code.
