# MobileNet Model Generation Guide for IREE EmitC

## Overview

This guide describes how to generate MobileNet models in IREE EmitC format for bare-metal RISC-V RTOS environments.

---

## Method 1: Using TensorFlow Lite Models (Recommended)

### Step 1: Download Pre-trained MobileNet Models

#### MobileNet V1 (Lightweight, Recommended for Embedded)
```bash
# Download MobileNet V1 (1.0, 224) - approx. 17MB
wget https://storage.googleapis.com/tensorflow/keras-applications/mobilenet/mobilenet_1_0_224_tf.h5

# Or download the TFLite version (Smaller)
wget https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz
tar -xzf mobilenet_v1_1.0_224_quant.tgz
```

#### MobileNet V2 (Better Accuracy)
```bash
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite_11_05_08/mobilenet_v2_1.0_224.tgz
tar -xzf mobilenet_v2_1.0_224.tgz
```

#### MobileNet V3 (Latest, Optimized)
```bash
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/model_zoo/upload_20180427/mobilenet_v3_small_100_224_uint8.tgz
tar -xzf mobilenet_v3_small_100_224_uint8.tgz
```

---

### Step 2: Install IREE Compiler

**Using Mamba Environment** (Recommended):
```bash
# Activate the iree-compiler environment
mamba activate iree-compiler

# If the environment does not exist, create and install it
mamba create -n iree-compiler python=3.11 -y
mamba activate iree-compiler
pip install iree-compiler iree-runtime
```

**Or Build from Source** (Optional, Advanced Users):
```bash
git clone https://github.com/iree-org/iree.git
cd iree
python ./build_tools/python/configure_ci.py
cmake --build build/
```

---

### Step 3: Compile TFLite Model to IREE EmitC

#### Basic Compilation Command

```bash
iree-compile \
  --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64 \
  --iree-llvmcpu-target-cpu=generic-rv64 \
  --iree-llvmcpu-target-abi=lp64 \
  --iree-vm-bytecode-module-output-format=c \
  mobilenet_v1_1.0_224.tflite \
  -o mobilenet_v1_emitc.c
```

#### Optimized Compilation for RISC-V

```bash
iree-compile \
  --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64-unknown-elf \
  --iree-llvmcpu-target-cpu=generic-rv64 \
  --iree-llvmcpu-target-cpu-features=+m,+a,+c,+zicsr \
  --iree-llvmcpu-target-abi=lp64 \
  --iree-llvmcpu-link-embedded=true \
  --iree-llvmcpu-link-static=true \
  --iree-vm-bytecode-module-output-format=c \
  --iree-vm-emit-polyglot-zip=false \
  mobilenet_v1_1.0_224_quant.tflite \
  -o mobilenet_v1_quant_emitc.c
```

#### INT8 Quantized Version (Smaller)

```bash
iree-compile \
  --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64 \
  --iree-llvmcpu-target-abi=lp64 \
  --iree-llvmcpu-enable-microkernels \
  --iree-vm-bytecode-module-output-format=c \
  mobilenet_v1_1.0_224_quant.tflite \
  -o mobilenet_v1_int8_emitc.c
```

---

### Step 4: Integrate into RTOS Project

```bash
# Copy the generated EmitC file to the project
cp mobilenet_v1_emitc.c /home/mingshi/Project/PF/rtos/ai/

# Update ai/iree_sources.cmake
cat >> /home/mingshi/Project/PF/rtos/ai/iree_sources.cmake << 'EOFCMAKE'

# MobileNet EmitC Module
set(AI_SOURCES
    ${AI_SOURCES}
    ${CMAKE_SOURCE_DIR}/ai/mobilenet_v1_emitc.c
)
EOFCMAKE

# Recompile
cd /home/mingshi/Project/PF/rtos/build
cmake ..
make rv_aios_ai
```

---

## Method 2: Using Kenning Toolchain (Automated)

### Install Kenning

```bash
pip install git+https://github.com/antmicro/kenning.git
```

### Compile MobileNet using Kenning

```bash
kenning optimize \
  --model-path mobilenet_v1_1.0_224.tflite \
  --target iree-llvm \
  --target-device riscv64 \
  --output-path mobilenet_kenning.c
```

---

## Method 3: Converting from PyTorch Models

### Step 1: Export PyTorch Model to ONNX

```python
import torch
import torchvision.models as models

# Load pre-trained MobileNet
model = models.mobilenet_v2(pretrained=True)
model.eval()

# Export to ONNX
dummy_input = torch.randn(1, 3, 224, 224)
torch.onnx.export(
    model,
    dummy_input,
    "mobilenet_v2.onnx",
    export_params=True,
    opset_version=13,
    do_constant_folding=True,
    input_names=['input'],
    output_names=['output']
)
```

### Step 2: ONNX to IREE

```bash
iree-import-onnx mobilenet_v2.onnx -o mobilenet_v2.mlir

iree-compile \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64 \
  --iree-vm-bytecode-module-output-format=c \
  mobilenet_v2.mlir \
  -o mobilenet_v2_emitc.c
```

---

## Model Comparison

| Model | Size | Accuracy | Inference Speed (Est.) | Recommended Scenario |
|------|------|------|-----------------|---------|
| MobileNet V1 FP32 | ~17MB | 71% | Medium | Development & Testing |
| MobileNet V1 INT8 | ~4.3MB | 70% | Fast | **Production Recommended** |
| MobileNet V2 FP32 | ~14MB | 72% | Slow | High Accuracy Needs |
| MobileNet V2 INT8 | ~3.4MB | 71% | Fast | Production Environments |
| MobileNet V3 Small INT8 | ~2.5MB | 68% | Fastest | **Embedded Preferred** |

**Recommendation**: For bare-metal RISC-V environments, use **MobileNet V1 INT8** or **MobileNet V3 Small INT8**.

---

## Performance Estimation

Based on RISC-V hifive1b (320MHz) reference data:

| Model | RAM | Flash | Latency (Est.) |
|------|-----|-------|------------|
| Vec Add (Current) | 11KB | 61KB | < 1ms |
| MobileNet V1 INT8 | ~500KB | ~4.3MB | ~300-500ms |
| MobileNet V3 Small | ~300KB | ~2.5MB | ~150-250ms |

---

## Troubleshooting

### Issue 1: Compilation error "unsupported target"

**Solution**: Ensure IREE version >= 20231220
```bash
pip install --upgrade iree-compiler
```

### Issue 2: Generated C file is too large

**Solution**: Use INT8 quantized version
```bash
# Add quantization flag
--iree-llvmcpu-enable-microkernels
```

### Issue 3: Linker error "undefined reference"

**Solution**: Ensure all IREE runtime source files are included in `iree_sources.cmake`.

---

## Next Steps

1. Select the appropriate MobileNet version.
2. Use the above commands to generate the EmitC file.
3. Integrate into the RTOS project.
4. Compile and test.
5. Verify on QEMU or actual hardware.

---

**Quick Start Commands** (Recommended):

```bash
# Activate mamba environment
mamba activate iree-compiler

# Download MobileNet V3 Small INT8 (Best for embedded)
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/model_zoo/upload_20180427/mobilenet_v3_small_100_224_uint8.tgz
tar -xzf mobilenet_v3_small_100_224_uint8.tgz

# Compile to EmitC
iree-compile \
  --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64 \
  --iree-vm-bytecode-module-output-format=c \
  mobilenet_v3_small_100_224_uint8.tflite \
  -o mobilenet_v3_emitc.c

# Integrate into the project
cp mobilenet_v3_emitc.c /home/mingshi/Project/PF/rtos/ai/
```

---

**Reference Resources**:
- IREE Official Documentation: https://iree.dev
- TFLite Model Zoo: https://www.tensorflow.org/lite/models
- Kenning Tool: https://github.com/antmicro/kenning
