# MiniResNet Static-Direct Real Inference QEMU

Date: 2026-05-07

Command:

```bash
pixi run build-miniresnet-static-direct-qemu-test
pixi run validate-miniresnet-static-direct-qemu-test
```

This is the current real static-direct path, not a patched-kernel runtime.

## Output

All three QEMU runs matched the current oracle.

| Run | Argmax | Hash | Top Score Q | Output Q Vector | Latency Ticks |
| ---: | ---: | ---: | ---: | --- | ---: |
| 0 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977885 |
| 1 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977885 |
| 2 | 3 | 3045847227 | 99609 | `[0,391,0,99609,0,0,0,0,0,0]` | 977884 |

Average latency: 977884.7 ticks. If interpreted at 250 MHz, this is about
3.912 ms.

## Memory And Size

Runtime static-direct app line:

```text
input_bytes=3200 arena_bytes=351104 arena_guard_bytes=128 output_bytes=40 io_scratch_bytes=354472 task_stack_bytes=65536
```

| Field | KiB |
| --- | ---: |
| Input | 3.125 |
| Arena | 342.875 |
| Arena guards | 0.125 |
| Output | 0.039 |
| Input + arena + guards + output | 346.164 |
| Task stack | 64.000 |
| Invoke heap peak | 0.000 |

Image sections from `llvm-size -A`:

| Section | KiB |
| --- | ---: |
| `.text` | 273.137 |
| `.data` | 0.160 |
| `.bss` | 414.371 |
| `.stack` | 16.004 |
| Total | 703.792 |

## Patch Status

The patch-based report remains a memory-model estimate. A true patched runtime
needs a real replacement for at least the first convolution plus requantization
chain; wrapping the existing IREE dispatches is not enough because the generated
dispatch constants encode offsets inside the original virtual 315264-byte slab.
