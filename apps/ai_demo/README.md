# AI Demo - 轻量化 AI 推理模板

`ai_demo` 提供了一个类似于 STM32CubeAI 风格的轻量化推理框架，旨在为嵌入式开发者提供最简化的 API 来在 AIOS 上部署 IREE 编译的模型。

## 📁 文件结构

- `ai_model.h`: **用户配置区**。定义模型名称、输入输出维度及数据类型。
- `ai_model.c`: **模型包装层**。负责与 IREE 运行时交互，通常不需要修改（除非更换库查询函数名）。
- `main.c`: **应用逻辑**。演示如何初始化运行时、准备数据并执行推理。
- `CMakeLists.txt`: **构建配置**。用于链接模型静态库（.o 文件）。

## 🚀 快速开始

### 1. 配置模型 (`ai_model.h`)
根据你的模型参数修改以下宏：

```c
/* 模型显示名称 */
#define AI_MODEL_NAME           "my_model"

/* 输入张量配置 (NHWC) */
#define AI_INPUT_HEIGHT         28
#define AI_INPUT_WIDTH          28
#define AI_INPUT_CHANNELS       1

/* 输出类别数 */
#define AI_OUTPUT_CLASSES       10

/* 数据类型: 支持 int8_t, uint8_t, float */
typedef int8_t  ai_input_t;
typedef int8_t  ai_output_t;
```

### 2. 链接模型静态库
将 IREE 编译生成的 `.o` 文件放入工程中，并修改 `ai_model.c` 中的外部符号：

```c
/* 1. 确保 EmitC 模块创建函数存在 (通常由 IREE 生成) */
extern iree_status_t module_create(...);

/* 2. 更新静态库查询函数名 (需与模型名称匹配) */
extern const iree_hal_executable_library_header_t**
    your_model_library_query(...);
```

### 3. 应用集成 (`main.c`)
只需调用三个核心 API：

```c
// 1. 初始化
ai_init();

// 2. 执行推理
ai_run(input_data);

// 3. 获取预测
int result = ai_get_prediction(&confidence);
```

## 🔄 更换模型流程

1. **导出模型**: 使用 IREE 将模型编译为 EmitC 静态库（产生 `.h` 和 `.o`）。
2. **更新头文件**: 在 `ai_model.h` 中更新维度参数。
3. **关联符号**: 在 `ai_model.c` 中更新 `*_library_query` 函数的声明和调用。
4. **CMake 链接**: 在 `CMakeLists.txt` 中添加新的 `.o` 文件到链接列表。

## 📝 开发者注意事项

### 符号依赖
- **fmaximumf/fminimumf**: IREE 生成的代码可能依赖 C23 标准的 `fmaximumf`。在 `picolibc` 环境下，这些符号由 `hal/src/math_compat.c` 提供，请确保该文件已包含在构建中。
- **Library Query**: IREE 生成的每个模型都有一个唯一的 `_library_query` 入口点，必须在 `ai_model.c` 中正确注册到 `libraries` 数组中。

### 性能与内存
- 建议使用 `int8_t` 量化模型以获得最佳性能。
- 推理任务通常需要较大的栈空间（建议 `ai_stack` 大于 32KB）。

## 🛠️ 构建与运行

```bash
# 进入构建目录并配置 (使用预设环境)
pixi run -e rv32 cmake -B build -G Ninja

# 编译 demo
pixi run -e rv32 ninja -C build ai_demo

# 启动 Renode 仿真运行
pixi run -e rv32 renode --disable-xwt -e \
    "$bin=@build/apps/ai_demo/ai_demo.elf; s @scripts/simulation/mobilenet_test_rv32.resc; start"
```
```
