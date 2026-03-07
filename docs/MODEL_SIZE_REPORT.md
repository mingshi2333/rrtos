# Optimized Model Size Report

Date: 2026-01-22

This document provides a summary of the file sizes for the optimized AI models (IREE compiled `.vmfb` files) currently available in the project.

## Model Inventory

Location: `zoo/iree_static/`

| Model Name | Optimization Type | File Size | Description |
| :--- | :--- | :--- | :--- |
| **st_mnist_28.vmfb** | Static | **158 KB** | MNIST digit classification model (28x28 input). |
| **st_yolo_lc_v1_192_int_beta.vmfb** | Static, Int8 Quantization (Beta) | **535 KB** | YOLO Object Detection (Low Compute v1, 192x192), integer quantized. |
| **st_yolo_lc_v1_192.vmfb** | Static, FP32/Default | **802 KB** | YOLO Object Detection (Low Compute v1, 192x192), standard precision. |
| **yoloface_int8.vmfb** | Int8 Quantization | **1.1 MB** | YOLO Face Detection, integer quantized. |
| **test.vmfb** | - | **2.5 MB** | Test model artifact. |

## Analysis

1.  **Quantization Impact**:
    *   Comparing `st_yolo_lc_v1_192.vmfb` (802 KB) with `st_yolo_lc_v1_192_int_beta.vmfb` (535 KB), the integer quantization reduced the model size by approximately **33%**.

2.  **Smallest Model**:
    *   The `st_mnist_28.vmfb` is the most compact model at 158 KB, suitable for extremely constrained embedded scenarios.

3.  **Largest Production Model**:
    *   `yoloface_int8.vmfb` is the largest deployed model at 1.1 MB.
