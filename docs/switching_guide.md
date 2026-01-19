# 架构切换指南 (Switching Guide)

本项目的 `ai_demo` 支持两种主要的 RISC-V 架构配置。

---

## 1. 架构配置对比

| 配置 | RV32 (默认) | RV64 (性能) |
|---|---|---|
| **CPU 架构** | `rv32imac` | `rv64imafdcv` |
| **ABI** | `ilp32` (Soft-Float) | `lp64d` (Hard-Float) |
| **向量化** | ❌ 无 (Scalar) | ✅ 有 (Vector 1.0) |
| **YOLO 性能** | ~4800ms (@100MHz) | ~615ms (@100MHz) |
| **依赖库** | host libgcc (32-bit compat) | host libgcc (64-bit native) |
| **稳定性** | ⭐⭐⭐⭐⭐ (高兼容性) | ⭐⭐⭐⭐ (依赖 Host 环境) |

---

## 2. 切换步骤

### 切换到 RV32 (32-bit Scalar)

这是默认的稳定模式，适用于大多数低成本 MCU (如 ESP32-C3, BL602)。

```bash
# 1. 重新转换模型 (使用 RV32 参数)
cd zoo/scripts
./tflite_to_iree_c.sh -a rv32 ../models/medium/st_yolo_lc_v1_192_int8.tflite st_yolo_lc_v1_192

# 2. 清理并编译
cd ../..
pixi run clean
pixi run -e rv32 configure
cmake --build build --target ai_demo

# 3. 运行模拟 (RV32)
renode --console --disable-xwt scripts/simulation/ai_demo_rv32.resc
```

### 切换到 RV64 (64-bit Vector)

这是高性能模式，适用于带 Vector 扩展的高端核心 (如 C906, Ara)。

```bash
# 1. 重新转换模型 (使用 RV64 + Vector 参数)
cd zoo/scripts
./tflite_to_iree_c.sh -a rv64 ../models/medium/st_yolo_lc_v1_192_int8.tflite st_yolo_lc_v1_192

# 2. 清理并编译
cd ../..
pixi run clean
pixi run -e rv64 configure
cmake --build build --target ai_demo

# 3. 运行模拟 (RV64)
renode --console --disable-xwt scripts/simulation/ai_demo_rv64.resc
```

---

## 3. 配置文件修改点 (参考)

如果你需要手动修改配置，主要涉及以下文件：

1.  **`CMakeLists.txt`**:
    *   RV32: `-march=rv32imac -mabi=ilp32`
    *   RV64: `-march=rv64imafdcv_zicsr -mabi=lp64d`

2.  **`cmake/riscvXX-pixi.cmake`**:
    *   定义了交叉编译工具链和 libgcc 查找路径。
    *   RV64 需要正确的 `execute_process` 来找到 Host GCC 的库。

3.  **`zoo/scripts/tflite_to_iree_c.sh`**:
    *   RV32: `CPU_FEATURES="+m,+a,+c"`
    *   RV64: `CPU_FEATURES="+m,+a,+f,+d,+c,+v"` (开启 Vector)

4.  **Renode 脚本 (`.resc` / `.repl`)**:
    *   RV32: `cpuType: "rv32imac"`
    *   RV64: `cpuType: "rv64imafdcv"`

---

*最后更新: 2025-01-19*
