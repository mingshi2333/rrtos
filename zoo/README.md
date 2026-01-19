# IREE Model Zoo for Bare-Metal RISC-V

This directory contains TFLite models used for embedded testing, along with the toolchain for converting them into IREE C modules.

## Directory Structure

- `models/`: Original TFLite model files (categorized by size)
- `iree_static/`: Converted C code (.h) and static libraries (.o)
- `scripts/`: Automated conversion scripts
- `benchmarks/`: Memory footprint test results

## Automated Conversion Tools

We provide a script `scripts/tflite_to_iree_c.sh` to automatically convert TFLite models into IREE C modules.

**Features**:
1. Automatic INT8 quantization handling (ui8 -> i8)
2. Generation of `.h` (C interface) and `.o` (RISC-V object file)
3. Resolution of bare-metal soft-float linking issues (via Deferred Linking)

### Usage

```bash
cd zoo/scripts
./tflite_to_iree_c.sh <path/to/model.tflite> <output_name>
```

### Example

```bash
cd zoo/scripts
./tflite_to_iree_c.sh ../models/tiny/st_mnist_28_int8.tflite st_mnist_28
```

This will generate:
- `zoo/iree_static/st_mnist_28.h`
- `zoo/iree_static/st_mnist_28.o`

## Model Scale and Status Overview

| Model | TFLite Size | Binary (text/data/bss) | Flash Est. | RAM Est. | Status |
| --- | --- | --- | --- | --- | --- |
| MiniResNet INT8 | 141KB | 21832 / 65964 / 27256 | ~87KB | ~93KB | ✅ Passed (QEMU/Renode) |
| ST MNIST INT8 | 20KB | 54072 / 65964 / 27256 | ~119KB | ~92KB | ✅ Renode Inference Success |
| EfficientNet LC INT8 | 790KB | 770340 / 66076 / 27264 | ~817KB | ~93KB | ✅ Verified (Flash XIP) |

> Note: RAM estimation represents data+bss in the current simulation layout; moving weights to Flash/XIP can significantly reduce RAM usage.

## Integration into CMake

1. Create a `.c` file to include the generated header:
   ```c
   // my_model_impl.c
   #define EMITC_IMPLEMENTATION
   #include "st_mnist_28.h"
   ```

2. Add the source and object files to `CMakeLists.txt`:
   ```cmake
   add_executable(my_app main.c my_model_impl.c)
   target_link_libraries(my_app PRIVATE ${CMAKE_SOURCE_DIR}/zoo/iree_static/st_mnist_28.o)
   ```
   ```
