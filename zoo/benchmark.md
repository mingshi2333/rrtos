# AI 模型性能基准测试

## 测试环境

| 项目 | 配置 |
|------|------|
| 平台 | RISC-V 32-bit (RV32IMAC) |
| 模拟器 | Renode |
| 编译器 | Clang (LLVM) |
| 运行时 | IREE (EmitC 静态编译) |
| 量化 | INT8 |

---

## ST YOLO LC v1 192

轻量级目标检测模型，专为 MCU 优化。

### 模型信息

| 属性 | 值 |
|------|-----|
| 模型名 | st_yolo_lc_v1_192 |
| 类型 | 目标检测 (人体检测) |
| 输入 | 192×192×3 (INT8) |
| 输出 | 1470 个值 (检测框+置信度) |
| TFLite | 332 KB |

### 性能基准 (Renode 模拟)

**1. 标量模式 (Scalar Baseline)**
- **配置**: RV32IMAC, Soft-Float
- **Flash**: 715 KB
- **推理 (480MHz)**: ~1000 ms

**2. 向量模式 (Vector Optimized)** 🚀
- **配置**: RV32IMAFDCV (Vector 1.0), Soft-Float ABI
- **优化**: LLVM Auto-Vectorization (Loop + SLP)
- **Flash**: **472 KB** (↓34%)
- **推理 (480MHz)**: **127 ms** (↑7.8x)

### 内存占用 (Vector Mode)

| 段 | 大小 | 说明 |
|----|------|------|
| .text | 472 KB | 代码 + 权重(297KB) |
| .data | 1 KB | 初始化数据 |
| .bss | 164 KB | 工作缓冲区 |
| **Flash 总计** | **~473 KB** | 极度紧凑 |
| **RAM 总计** | **~165 KB** | 适合大多数 MCU |

### 编译配置

```bash
# Vector Enabled Script
--iree-llvmcpu-target-cpu-features="+m,+a,+c,+f,+d,+v,+zicsr"
--iree-llvmcpu-loop-vectorization
--iree-llvmcpu-slp-vectorization
```

---

## 硬件推荐

| 模型 | 最低配置 | 推荐配置 |
|------|----------|----------|
| ST YOLO LC v1 | Flash 1MB, RAM 256KB, 100MHz | Flash 1MB, RAM 256KB, 480MHz |
| MobileNetV2 | Flash 4MB, RAM 512KB | 需要 medany 代码模型 |

---

*最后更新: 2025-01-19*
