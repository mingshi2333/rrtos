# AI Model Performance Benchmarks

## Test Environment

| Item | Configuration |
| --- | --- |
| Platform | RISC-V 32-bit (RV32IMAC) |
| Simulator | Renode |
| Compiler | Clang (LLVM) |
| Runtime | IREE (EmitC Static Compilation) |
| Quantization | INT8 |

---

## 1. ST-YOLO-LC (v1_192)
Lightweight object detection model, optimized for MCUs.

### Model Info

| Property | Value |
| --- | --- |
| Model Name | st_yolo_lc_v1_192 |
| Type | Object Detection (Person Detection) |
| Input | 192×192×3 (INT8) |
| Output | 1470 values (Bounding boxes + Confidence) |

### Performance Benchmarks (Renode Simulation)

**1. Scalar Mode (Scalar Baseline)**
- **Config**: RV32IMAC, Soft-Float
- **Inference (480MHz)**: ~1000 ms

**2. Vector Mode (Vector Optimized)** 🚀
- **Config**: RV32IMAFDCV (Vector 1.0), Soft-Float ABI
- **Optimization**: LLVM Auto-Vectorization (Loop + SLP)
- **Inference (480MHz)**: **127 ms** (↑7.8x improvement)

### Memory Footprint (Vector Mode)

| Segment | Size | Description |
| --- | --- | --- |
| .text | 472 KB | Code + Weights (297KB) |
| .data | 1 KB | Initialized data |
| .bss | 164 KB | Work buffers |
| **Flash Total** | **~473 KB** | Highly compact |
| **RAM Total** | **~165 KB** | Suitable for most MCUs |

### Hardware Recommendations

| Model | Minimum Config | Recommended Config |
| --- | --- | --- |
| YOLO-LC | Flash 512KB, RAM 192KB | RV32 with Vector Extension |
| MobileNetV2 | Flash 4MB, RAM 512KB | Requires medany code model |

*Last Updated: Jan 19, 2025*
