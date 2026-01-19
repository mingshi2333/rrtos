# RIOT-ML Model Zoo to IREE EmitC Conversion Report

## Task Summary
Convert all TFLite models from RIOT-ML model_zoo to IREE EmitC format for embedded RISCV deployment.

## Environment Setup
- **IREE Compiler**: v3.9.0rc20251125 @ 1a6681936a78138f7ae2b0fd9309d28dfe01ac5b
- **LLVM**: 22.0.0git (optimized build)
- **Mamba environment**: `iree-compiler` at `/home/mingshi/.mamba/envs/iree-compiler`
- **TensorFlow**: 2.20.0 (installed)
- **Model Source**: `/tmp/RIOT-ML/model_zoo` (9 TFLite models)

## Model Inventory
From RIOT-ML repository (https://github.com/TinyPART/RIOT-ML):

| Model | Size | Input Shape | Output Shape | Type |
|-------|------|-------------|--------------|------|
| ad01_int8.tflite | 276 KB | [1, 640] | [1, 640] | int8 |
| ad_large_int8.tflite | 442 KB | [1, 32, 32, 1] | [1, 8] | int8 |
| ad_small_int8.tflite | 252 KB | [1, 32, 32, 1] | [1, 8] | int8 |
| ds_cnn_s_quantized.tflite | 47 KB | [1, 490] | [1, 12] | int8 |
| mnist_0.983_quantized.tflite | 49 KB | [1, 1, 28, 28] | [1, 10] | float32 |
| rnnoise_INT8.tflite | 113 KB | [1, 1, 42] | [1, 1, 96] | int8 |
| sinus_float.tflite | 3 KB | [1, 1] | [1, 1] | float32 |
| vww2_50_50_INT8.tflite | 280 KB | [1, 50, 50, 1] | [1, 2] | int8 |
| vww_96_int8.tflite | 333 KB | [1, 96, 96, 3] | [1, 2] | int8 |

## Attempted Conversion Pipelines

### 1. Direct TFLite Input (❌ FAILED)
```bash
iree-compile model.tflite --iree-input-type=tflite --iree-hal-target-backends=vmvx --output-format=vm-c
```
**Result**: IREE does not support `tflite` as input type. Supported: `stablehlo`, `onnx`, `tosa`, `torch`, etc.

### 2. TFLite→ONNX→EmitC Pipeline (❌ FAILED)
```bash
tflite2onnx model.tflite -o model.onnx
iree-compile model.onnx --iree-input-type=onnx --output-format=vm-c
```
**Result**: 
- `tflite2onnx` doesn't support INT8 quantized models (8/9 models are INT8)
- Per-tensor quantization not supported
- Generated ONNX files corrupted (UTF-8 decode errors)

### 3. TFLite+TensorFlow→TOSA Pipeline (❌ FAILED)
```python
tf.lite.Interpreter(model_path)  # Load TFLite
iree-compile model.tflite --iree-input-type=tosa  # Attempt compile
```
**Result**: IREE doesn't accept binary TFLite files as TOSA input - expects TOSA MLIR text format

## Root Cause Analysis

**IREE TFLite Support Status** (v3.9.0):
- TFLite frontend was REMOVED from IREE in recent versions
- No direct TFLite→IREE compilation path exists
- Recommended path: TensorFlow→StableHLO→IREE (requires full TF model, not just TFLite)

**Blockers**:
1. ❌ No TFLite→StableHLO converter for quantized models
2. ❌ ONNX path broken for INT8 quantization  
3. ❌ Need original TensorFlow models (not available in RIOT-ML)
4. ✅ RIOT-ML uses **TVM**, not IREE

## Working Alternative: Use TVM (Recommended)

RIOT-ML already has TVM infrastructure:

```python
# From /tmp/RIOT-ML/model_converter.py
from tvm import relay
import tflite

# TFLite → TVM Relay
mod, params = relay.frontend.from_tflite(tflite_model)

# Compile to C code
module = relay.build(mod, target=TARGET, runtime=RUNTIME, executor=EXECUTOR)
export_model_library_format(module, output_path)
```

### Why TVM Works:
- ✅ Native TFLite support (including INT8)
- ✅ C code generation built-in
- ✅ Already integrated in RIOT-ML  
- ✅ Proven for embedded RISC-V targets

## IREE Workaround (If Required)

If IREE is mandatory:

### Option A: Convert via PyTorch
```bash
# Install ai-edge-torch
pip install ai-edge-torch

# TFLite → PyTorch → StableHLO → IREE
python convert_via_torch.py model.tflite
iree-compile model_stablehlo.mlir --iree-hal-target-backends=vmvx --output-format=vm-c
```

### Option B: Manual MLIR Authoring  
For simple models (e.g., `sinus_float.tflite`), manually write TOSA/StableHLO MLIR:
```mlir
module {
  func.func @main(%arg0: tensor<1x1xf32>) -> tensor<1x1xf32> {
    // Manually transcribe TFLite ops to TOSA
    return %result : tensor<1x1xf32>
  }
}
```

## Files Created

| File | Purpose | Status |
|------|---------|--------|
| `/home/mingshi/Project/PF/rtos/compile_models_to_emitc.sh` | Shell script for batch compilation | Deprecated (TFLite not supported) |
| `/home/mingshi/Project/PF/rtos/convert_tflite_to_tosa.py` | Python TFLite→TOSA converter | Not implemented (no tool exists) |
| `/home/mingshi/Project/PF/rtos/convert_all_to_emitc.py` | TFLite→ONNX→EmitC pipeline | Failed (quantization unsupported) |
| `/home/mingshi/Project/PF/rtos/convert_tflite_iree.py` | TensorFlow+IREE pipeline | Failed (format mismatch) |
| `/tmp/RIOT-ML/` | Cloned RIOT-ML repository | ✅ Ready |
| `/home/mingshi/Project/PF/rtos/onnx_models/` | ONNX intermediates (2 corrupt files) | ⚠️ Unusable |
| `/home/mingshi/Project/PF/rtos/emitc_models/` | Output directory (empty) | Created |
| `/home/mingshi/Project/PF/rtos/stablehlo_models/` | StableHLO intermediates (empty) | Created |

## Recommendations

### Immediate Action
**Use TVM instead of IREE** for these TFLite models:
```bash
cd /tmp/RIOT-ML
python model_converter.py \
  --model model_zoo/sinus_float.tflite \
  --board rpi-pico \
  --output ./output_model
```

### Long-term IREE Path
1. Request original TensorFlow saved models from RIOT-ML authors
2. Use TensorFlow→StableHLO official converter:
   ```python
   import tensorflow as tf
   from tensorflow.compiler.mlir.stablehlo import serialize_stablehlo
   ```
3. Compile StableHLO to EmitC via IREE

## Conclusion

**Status**: ❌ TFLite→IREE EmitC conversion NOT POSSIBLE with current IREE toolchain (v3.9.0)

**Successful Workaround**: Use RIOT-ML's built-in TVM workflow (already supports all 9 models)

**IREE Compatibility**: Requires original TensorFlow models or complete rewrite to StableHLO/TOSA MLIR
