# MobileNet Quick Start Guide

## 🎯 Goal

Generate a functional MobileNet model for your RISC-V RTOS project.

---

## ⚡ Quick Start (5 Minutes)

### Recommended: MobileNet V3 Small INT8

**Why choose this one**:
- ✅ Smallest footprint (2.5MB)
- ✅ Fastest speed (150-250ms @ 320MHz)
- ✅ Ideal for embedded systems
- ✅ INT8 quantized, memory-friendly

---

## 📋 Complete Steps

### 1. Install IREE Compiler

**Using Mamba Environment** (Recommended):
```bash
# Activate the iree-compiler environment
mamba activate iree-compiler

# If the environment does not exist, create and install it
mamba create -n iree-compiler python=3.11 -y
mamba activate iree-compiler
pip install iree-compiler iree-runtime
```

Verify the installation:
```bash
iree-compile --version
```

---

### 2. Download MobileNet Model

#### Option A: MobileNet V3 Small INT8 (Recommended)
```bash
cd /tmp
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/model_zoo/upload_20180427/mobilenet_v3_small_100_224_uint8.tgz
tar -xzf mobilenet_v3_small_100_224_uint8.tgz
```

#### Option B: MobileNet V1 INT8 (Classic, Stable)
```bash
wget https://storage.googleapis.com/download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz
tar -xzf mobilenet_v1_1.0_224_quant.tgz
```

---

### 3. Compile to IREE EmitC Format

```bash
# Compile MobileNet V3 Small
iree-compile \
  --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64-unknown-elf \
  --iree-llvmcpu-target-cpu=generic-rv64 \
  --iree-llvmcpu-target-cpu-features=+m,+a,+c,+zicsr \
  --iree-llvmcpu-target-abi=lp64 \
  --iree-llvmcpu-link-embedded=true \
  --iree-llvmcpu-link-static=true \
  --iree-llvmcpu-enable-microkernels \
  --iree-vm-bytecode-module-output-format=c \
  --iree-vm-emit-polyglot-zip=false \
  mobilenet_v3_small_100_224_uint8.tflite \
  -o mobilenet_v3_small_emitc.c
```

**Expected Output**:
- A `mobilenet_v3_small_emitc.c` file is generated (approx. 2-5MB).
- Contains model weights and IREE runtime code.

---

### 4. Integrate into RTOS Project

```bash
# Copy to project
cp mobilenet_v3_small_emitc.c /home/mingshi/Project/PF/rtos/ai/

# Update CMakeLists
cd /home/mingshi/Project/PF/rtos

# Edit ai/CMakeLists.txt or add directly to AI_SOURCES
echo 'list(APPEND AI_SOURCES ${CMAKE_SOURCE_DIR}/ai/mobilenet_v3_small_emitc.c)' \
  >> ai/iree_sources.cmake

# Rebuild
cd build
cmake ..
make rv_aios_ai -j$(nproc)
```

---

### 5. Verify Compilation

```bash
# Check library size (should increase by approx. 2-3MB)
ls -lh build/librv_aios_ai.a

# Check symbols
nm build/librv_aios_ai.a | grep mobilenet
```

---

## 📊 Model Specification Comparison

| Model | Input | Output | Size | RAM | Inference Time |
|-------|-------|--------|------|-----|----------------|
| **Vec Add (Current)** | float32[4] | float32[4] | 118KB | 11KB | <1ms |
| **MobileNet V3 Small** | uint8[1,224,224,3] | uint8[1,1001] | ~2.5MB | ~300KB | 150-250ms |
| **MobileNet V1 INT8** | uint8[1,224,224,3] | uint8[1,1001] | ~4.3MB | ~500KB | 300-500ms |

---

## 🔧 Example Usage Code

```c
#include "iree/vm/api.h"

// Example inference function
int mobilenet_inference(uint8_t* input_image, uint8_t* output_classes) {
    iree_allocator_t allocator = iree_allocator_system();
    iree_vm_instance_t* instance = NULL;
    iree_vm_module_t* module = NULL;
    
    // Create VM instance
    iree_vm_instance_create(IREE_VM_TYPE_CAPACITY_DEFAULT, 
                           allocator, &instance);
    
    // Load MobileNet module
    module_create(instance, allocator, &module);
    
    // TODO: Set input tensors (input_image)
    // TODO: Execute inference
    // TODO: Read output tensors (output_classes)
    
    // Cleanup
    iree_vm_module_release(module);
    iree_vm_instance_release(instance);
    
    return 0;
}
```

---

## ⚠️ Important Notes

### 1. Memory Requirements
- **Flash**: Increases by approx. 2.5-4.5MB (depending on model).
- **RAM**: Approx. 300-500KB required at runtime.
- Ensure the linker script configures sufficient heap space.

### 2. Performance Estimation
- RISC-V @ 320MHz: 150-500ms per inference.
- Approx. 4x slower than ARM Cortex-M7.
- Recommended to use hardware accelerators (e.g., Vector Extensions).

### 3. Compilation Time
- EmitC compilation may take 5-30 minutes.
- Generated C files can be large (2-10MB).
- Project build time will increase accordingly.

---

## 🚀 Next Steps

1. ✅ Generate MobileNet EmitC file.
2. ✅ Integrate into RTOS project.
3. ⏸️ Write inference interface code.
4. ⏸️ Test on QEMU.
5. ⏸️ Verify on actual hardware.

---

## 📚 Reference Resources

- [Full Guide](docs/MOBILENET_GENERATION_GUIDE.md)
- [IREE Official Documentation](https://iree.dev)
- [TFLite Model Zoo](https://www.tensorflow.org/lite/models)

---

**Quick Command Summary**:
```bash
# 0. Activate mamba environment
mamba activate iree-compiler

# 1. Install tools (if not already installed)
pip install iree-compiler iree-runtime

# 2. Download model
wget https://storage.googleapis.com/download.tensorflow.org/models/tflite/model_zoo/upload_20180427/mobilenet_v3_small_100_224_uint8.tgz
tar -xzf mobilenet_v3_small_100_224_uint8.tgz

# 3. Compile
iree-compile --iree-input-type=tflite \
  --iree-hal-target-backends=llvm-cpu \
  --iree-llvmcpu-target-triple=riscv64 \
  --iree-vm-bytecode-module-output-format=c \
  mobilenet_v3_small_100_224_uint8.tflite \
  -o mobilenet_v3_emitc.c

# 4. Integrate
cp mobilenet_v3_emitc.c /home/mingshi/Project/PF/rtos/ai/
```
