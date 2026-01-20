# Memory Analysis Report: AI-RTOS System
**Date:** January 20, 2026  
**Target Application:** `ai_demo` (YOLOv1-Tiny INT8)  
**Platform:** RISC-V 64-bit (Custom RTOS)

---

## 1. Executive Summary

The system demonstrates a highly efficient, lightweight RTOS implementation hosting a significant AI workload. The custom RTOS kernel is minimal, allowing the majority of system resources to be dedicated to the IREE runtime and the AI model.

| Resource | Total Usage | Utilization Focus |
| :--- | :--- | :--- |
| **Flash (ROM)** | **~453 KB** | **64%** used by AI Model (Weights) |
| **RAM (Memory)** | **~162 KB** | **86%** used by Model Inputs & Stack |

---

## 2. Memory Footprint Breakdown

### 2.1 Overall Distribution

| Component | Flash (Code/RO) | RAM (Data/Stack) | Description |
| :--- | :--- | :--- | :--- |
| **AI Model (YOLO)** | **290.2 KB** | **140.0 KB** | Neural network weights and runtime buffers |
| **IREE Runtime** | **112.0 KB** | **3.0 KB** | Inference engine core (VM, HAL) |
| **RTOS Kernel** | **12.4 KB** | **3.4 KB** | Task scheduling, IPC, memory management |
| **System / Libc** | **~32.0 KB** | **16.0 KB** | Standard library, startup code, system stack |
| **AI Wrapper** | **4.9 KB** | **0.1 KB** | Application logic and helper functions |
| **TOTAL** | **~453 KB** | **~162.5 KB** | |

---

## 3. Component Deep Dive

### 3.1 AI Model (st_yolo_lc_v1_192_int_beta)
The application runs a quantized (INT8) version of a lightweight YOLO model. This is the dominant resource consumer.

*   **Flash Usage (~290 KB):**
    *   `module__const` stores the model topology and quantized weights.
    *   Deployed via **Static Library Linking** to minimize loading overhead.
*   **RAM Usage (~140 KB):**
    *   **Input Buffer (`input_buffer`):** **108 KB**.
        *   Format: 192 x 192 x 3 channels (RGB) x 1 byte (INT8).
    *   **Task Stack (`ai_stack`):** **32 KB**.
        *   Dedicated stack for the IREE VM execution.

### 3.2 IREE Runtime
The Google IREE runtime provides the execution environment for the model.

*   **Flash Usage (~112 KB):**
    *   **HAL (Hardware Abstraction Layer):** ~81 KB. Handles buffers and device synchronization.
    *   **VM (Virtual Machine):** ~21 KB. Executes the bytecode.
    *   **Utils:** ~10 KB. String handling, synchronization primitives.
*   **RAM Usage:** Minimal (< 3 KB). The runtime relies mostly on the stack and the pre-allocated arena in the model buffers.

### 3.3 Custom RTOS (rv_aios_rtos)
The operating system core is extremely lightweight, proving effectively "invisible" compared to the AI workload.

*   **Total Kernel Size:** **< 13 KB** (Flash)
*   **Key Modules:**
    *   **Scheduler (`os_sched.c`):** 3.2 KB
    *   **IPC (`os_ipc.c`):** 2.8 KB
    *   **Timer (`os_timer.c`):** 1.6 KB
    *   **Memory (`os_mem.c`):** 1.2 KB
    *   **Drivers (UART/PLIC):** ~1.8 KB

---

## 4. Optimization Opportunities

### 4.1 RAM Optimization (Critical)
RAM usage is the primary constraint. 
1.  **Reduce Input Resolution:** 
    *   Current: 192x192 RGB (~108 KB)
    *   Target: 160x160 RGB (~75 KB) -> **Saves 33 KB**
2.  **Grayscale Input:**
    *   Target: 192x192 Grayscale (~36 KB) -> **Saves 72 KB** (Requires model retraining)
3.  **Stack Tuning:**
    *   Reduce `ai_stack` from 32 KB to 16-20 KB. (Requires stress testing to verify safety).

### 4.2 Flash Optimization
1.  **IREE Configuration:** Disable unused IREE HAL drivers or debugging features if present.
2.  **Model Compression:** Apply stricter pruning or quantization (INT4) if supported by the toolchain.

---

## 5. Conclusion
The `rv_aios` project successfully integrates a modern AI runtime into a minimal footprint. The custom RTOS incurs negligible overhead (~3% of Flash), validating the architecture for resource-constrained Edge AI applications.
