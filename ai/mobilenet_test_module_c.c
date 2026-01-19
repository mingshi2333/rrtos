#ifndef MODULE_H_
#define MODULE_H_
#include "iree/vm/api.h"
#ifdef __cplusplus
extern "C" {
#endif //  __cplusplus
iree_status_t mobilenet_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3);
#ifdef __cplusplus
}  // extern "C"
#endif //  __cplusplus
#endif //  MODULE_H_
#if defined(EMITC_IMPLEMENTATION)
#include "iree/vm/ops.h"
#include "iree/vm/ops_emitc.h"
#include "iree/vm/shims_emitc.h"
iree_alignas(1) static const uint8_t module__utf8_hal_device_id_C6650FF277232B5A[] = {104, 97, 108, 46, 100, 101, 118, 105, 99, 101, 46, 105, 100};
iree_alignas(1) static const uint8_t module__utf8_local_1A8FF0278D7661D8[] = {108, 111, 99, 97, 108, 42};
iree_alignas(1) static const uint8_t module__utf8_hal_executable_format_E03EECB63A2AAF52[] = {104, 97, 108, 46, 101, 120, 101, 99, 117, 116, 97, 98, 108, 101, 46, 102, 111, 114, 109, 97, 116};
iree_alignas(1) static const uint8_t module__utf8_embedded_elf_riscv_64_C3BA011E86B77EF5[] = {101, 109, 98, 101, 100, 100, 101, 100, 45, 101, 108, 102, 45, 114, 105, 115, 99, 118, 95, 54, 52};
iree_alignas(16) static const uint8_t module_main_dispatch_0_embedded_elf_riscv_64[] = {127, 69, 76, 70, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 243, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 96, 9, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 64, 0, 56, 0, 8, 0, 64, 0, 22, 0, 20, 0, 6, 0, 0, 0, 4, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 192, 1, 0, 0, 0, 0, 0, 0, 192, 1, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 4, 0, 0, 0, 0, 0, 0, 108, 4, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 0, 0, 0, 108, 4, 0, 0, 0, 0, 0, 0, 108, 20, 0, 0, 0, 0, 0, 0, 108, 20, 0, 0, 0, 0, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 0, 0, 0, 240, 4, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 160, 1, 0, 0, 0, 0, 0, 0, 16, 11, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 6, 0, 0, 0, 208, 5, 0, 0, 0, 0, 0, 0, 208, 37, 0, 0, 0, 0, 0, 0, 208, 37, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 82, 229, 116, 100, 4, 0, 0, 0, 240, 4, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 160, 1, 0, 0, 0, 0, 0, 0, 16, 11, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 81, 229, 116, 100, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 112, 4, 0, 0, 0, 145, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 18, 0, 7, 0, 204, 20, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 114, 101, 101, 95, 104, 97, 108, 95, 101, 120, 101, 99, 117, 116, 97, 98, 108, 101, 95, 108, 105, 98, 114, 97, 114, 121, 95, 113, 117, 101, 114, 121, 0, 0, 0, 0, 0, 0, 248, 36, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 144, 3, 0, 0, 0, 0, 0, 0, 8, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 108, 20, 0, 0, 0, 0, 0, 0, 16, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 224, 3, 0, 0, 0, 0, 0, 0, 32, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 4, 0, 0, 0, 0, 0, 0, 56, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 4, 0, 0, 0, 0, 0, 0, 64, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 4, 0, 0, 0, 0, 0, 0, 80, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 112, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 8, 37, 0, 0, 0, 0, 0, 0, 120, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 160, 3, 0, 0, 0, 0, 0, 0, 144, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 16, 37, 0, 0, 0, 0, 0, 0, 168, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 24, 37, 0, 0, 0, 0, 0, 0, 176, 37, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 48, 37, 0, 0, 0, 0, 0, 0, 109, 97, 105, 110, 95, 100, 105, 115, 112, 97, 116, 99, 104, 95, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 109, 97, 105, 110, 95, 100, 105, 115, 112, 97, 116, 99, 104, 95, 48, 95, 101, 108, 101, 109, 101, 110, 116, 119, 105, 115, 101, 95, 52, 95, 102, 51, 50, 0, 105, 114, 101, 101, 95, 118, 101, 99, 95, 97, 100, 100, 46, 109, 108, 105, 114, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 1, 122, 82, 0, 1, 120, 1, 1, 27, 12, 2, 0, 36, 0, 0, 0, 24, 0, 0, 0, 56, 16, 0, 0, 96, 0, 0, 0, 0, 68, 14, 16, 72, 129, 1, 136, 2, 68, 12, 8, 0, 2, 64, 12, 2, 16, 72, 193, 200, 68, 14, 0, 16, 0, 0, 0, 64, 0, 0, 0, 112, 16, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 1, 1, 255, 35, 52, 17, 0, 35, 48, 129, 0, 19, 4, 1, 1, 3, 181, 5, 2, 131, 53, 5, 0, 3, 53, 133, 0, 135, 167, 5, 0, 7, 167, 69, 0, 135, 166, 133, 0, 7, 166, 197, 0, 211, 247, 247, 0, 83, 119, 231, 0, 211, 246, 214, 0, 83, 118, 198, 0, 39, 32, 245, 0, 39, 34, 229, 0, 39, 36, 213, 0, 39, 38, 197, 0, 19, 5, 0, 0, 131, 48, 129, 0, 3, 52, 1, 0, 19, 1, 1, 1, 103, 128, 0, 0, 27, 5, 165, 255, 151, 21, 0, 0, 51, 53, 160, 0, 147, 133, 5, 8, 19, 5, 245, 255, 51, 117, 181, 0, 103, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 251, 255, 255, 111, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 112, 2, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 249, 255, 255, 111, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 72, 2, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 17, 1, 37, 14, 19, 5, 3, 14, 16, 23, 180, 66, 25, 17, 1, 18, 6, 0, 0, 2, 46, 0, 17, 1, 18, 6, 64, 24, 110, 14, 3, 14, 58, 11, 59, 11, 73, 19, 63, 25, 0, 0, 3, 36, 0, 3, 14, 62, 11, 11, 11, 0, 0, 0, 71, 0, 0, 0, 4, 0, 0, 0, 0, 0, 8, 1, 40, 0, 0, 0, 44, 0, 38, 0, 0, 0, 0, 0, 0, 0, 108, 20, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 2, 108, 20, 0, 0, 0, 0, 0, 0, 96, 0, 0, 0, 1, 88, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 67, 0, 0, 0, 3, 34, 0, 0, 0, 5, 4, 0, 109, 97, 105, 110, 95, 100, 105, 115, 112, 97, 116, 99, 104, 95, 48, 95, 101, 108, 101, 109, 101, 110, 116, 119, 105, 115, 101, 95, 52, 95, 102, 51, 50, 0, 105, 110, 116, 0, 45, 0, 73, 82, 69, 69, 0, 52, 0, 0, 0, 2, 0, 0, 0, 0, 0, 75, 0, 0, 0, 38, 0, 0, 0, 109, 97, 105, 110, 95, 100, 105, 115, 112, 97, 116, 99, 104, 95, 48, 95, 101, 108, 101, 109, 101, 110, 116, 119, 105, 115, 101, 95, 52, 95, 102, 51, 50, 0, 0, 0, 0, 0, 22, 0, 0, 0, 2, 0, 0, 0, 0, 0, 75, 0, 0, 0, 67, 0, 0, 0, 105, 110, 116, 0, 0, 0, 0, 0, 65, 46, 0, 0, 0, 114, 105, 115, 99, 118, 0, 1, 36, 0, 0, 0, 4, 16, 5, 114, 118, 54, 52, 105, 50, 112, 49, 95, 102, 50, 112, 50, 95, 100, 50, 112, 50, 95, 122, 105, 99, 115, 114, 50, 112, 48, 0, 57, 0, 0, 0, 4, 0, 25, 0, 0, 0, 1, 1, 1, 251, 14, 13, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 45, 0, 0, 0, 0, 0, 0, 9, 2, 108, 20, 0, 0, 0, 0, 0, 0, 1, 5, 3, 10, 243, 6, 11, 2, 64, 18, 2, 16, 0, 1, 1, 73, 82, 69, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 2, 9, 0, 208, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 18, 0, 7, 0, 204, 20, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 46, 100, 121, 110, 115, 121, 109, 0, 46, 104, 97, 115, 104, 0, 46, 100, 121, 110, 115, 116, 114, 0, 46, 114, 101, 108, 97, 46, 100, 121, 110, 0, 46, 114, 111, 100, 97, 116, 97, 0, 46, 101, 104, 95, 102, 114, 97, 109, 101, 0, 46, 116, 101, 120, 116, 0, 46, 100, 97, 116, 97, 46, 114, 101, 108, 46, 114, 111, 0, 46, 100, 121, 110, 97, 109, 105, 99, 0, 46, 114, 101, 108, 114, 111, 95, 112, 97, 100, 100, 105, 110, 103, 0, 46, 100, 101, 98, 117, 103, 95, 97, 98, 98, 114, 101, 118, 0, 46, 100, 101, 98, 117, 103, 95, 105, 110, 102, 111, 0, 46, 100, 101, 98, 117, 103, 95, 115, 116, 114, 0, 46, 100, 101, 98, 117, 103, 95, 112, 117, 98, 110, 97, 109, 101, 115, 0, 46, 100, 101, 98, 117, 103, 95, 112, 117, 98, 116, 121, 112, 101, 115, 0, 46, 114, 105, 115, 99, 118, 46, 97, 116, 116, 114, 105, 98, 117, 116, 101, 115, 0, 46, 100, 101, 98, 117, 103, 95, 108, 105, 110, 101, 0, 46, 99, 111, 109, 109, 101, 110, 116, 0, 46, 115, 121, 109, 116, 97, 98, 0, 46, 115, 104, 115, 116, 114, 116, 97, 98, 0, 46, 115, 116, 114, 116, 97, 98, 0, 0, 105, 114, 101, 101, 95, 104, 97, 108, 95, 101, 120, 101, 99, 117, 116, 97, 98, 108, 101, 95, 108, 105, 98, 114, 97, 114, 121, 95, 113, 117, 101, 114, 121, 0, 95, 68, 89, 78, 65, 77, 73, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 11, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 5, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 72, 2, 0, 0, 0, 0, 0, 0, 72, 2, 0, 0, 0, 0, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 112, 2, 0, 0, 0, 0, 0, 0, 112, 2, 0, 0, 0, 0, 0, 0, 32, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 144, 3, 0, 0, 0, 0, 0, 0, 144, 3, 0, 0, 0, 0, 0, 0, 136, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 24, 4, 0, 0, 0, 0, 0, 0, 24, 4, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 0, 0, 0, 1, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 108, 20, 0, 0, 0, 0, 0, 0, 108, 4, 0, 0, 0, 0, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 57, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 240, 36, 0, 0, 0, 0, 0, 0, 240, 4, 0, 0, 0, 0, 0, 0, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, 0, 0, 0, 6, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 208, 37, 0, 0, 0, 0, 0, 0, 208, 5, 0, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 79, 0, 0, 0, 8, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 144, 38, 0, 0, 0, 0, 0, 0, 144, 6, 0, 0, 0, 0, 0, 0, 112, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 94, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 144, 6, 0, 0, 0, 0, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 199, 6, 0, 0, 0, 0, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 120, 0, 0, 0, 1, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 7, 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 131, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 7, 0, 0, 0, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 147, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 119, 7, 0, 0, 0, 0, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 0, 0, 0, 3, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 7, 0, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 181, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 7, 0, 0, 0, 0, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 193, 0, 0, 0, 1, 0, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 253, 7, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 202, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 210, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 8, 0, 0, 0, 0, 0, 0, 228, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 220, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 9, 0, 0, 0, 0, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
iree_alignas(1) static const uint8_t module__utf8_input0_DCE99660CEB3F6B[] = {105, 110, 112, 117, 116, 48};
iree_alignas(1) static const uint8_t module__utf8_tensor_FC1814BC4A58F22A[] = {116, 101, 110, 115, 111, 114};
struct module_t {iree_allocator_t allocator;iree_vm_ref_type_t types[10];};
struct module_state_t {iree_allocator_t allocator;uint8_t rwdata[1];iree_vm_ref_t refs[3];iree_vm_buffer_t rodata_buffers[7];iree_vm_function_t imports[17];};
typedef iree_status_t(*begin_call_t)(void*, iree_vm_stack_t*, iree_vm_function_call_t);
static iree_status_t module___main_memoize_apply(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3, iree_vm_ref_t* v4);
static iree_status_t module_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8);
static iree_status_t module_call_0rIIiiCID_r_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, iree_vm_ref_t* v10);
static iree_status_t module_call_0rriiCID_v_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8);
static iree_status_t module_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4);
static iree_status_t module_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8);
static iree_status_t module_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3);
static iree_status_t module_call_0riiI_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6);
static iree_status_t module_call_0rriiiiICiDCiirIID_v_0_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, iree_vm_ref_t* v14, int64_t v15, int64_t v16, int32_t v17, int32_t v18, iree_vm_ref_t* v19, int64_t v20, int64_t v21);
static iree_status_t module_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7);
static iree_status_t module_call_0rIrrIiiII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int64_t v7, int32_t v8, int32_t v9, int64_t v10, int64_t v11, iree_vm_ref_t* v12);
static iree_status_t module_call_0rIrrrICrIID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10, int64_t v11, int64_t v12, iree_vm_ref_t* v13, int64_t v14, int64_t v15);
static iree_status_t module_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3);
static iree_status_t module_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4);
static iree_status_t module_call_0rIrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, iree_vm_ref_t* v8);
static iree_status_t module_call_0rI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5);
static iree_status_t module_call_0iICrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int32_t* v7);
static iree_status_t module_main(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5);
static iree_status_t module_main_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t module___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t module___init(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3);
static void module_destroy(void* v1);
static iree_status_t module_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3);
static void module_free_state(void* v1, iree_vm_module_state_t* v2);
static iree_status_t module_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4);
static iree_status_t module_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5);
static const iree_vm_module_dependency_t module_dependencies_[] = {{{"hal", 3}, 6, IREE_VM_MODULE_DEPENDENCY_FLAG_REQUIRED},};
static const iree_vm_native_import_descriptor_t module_imports_[] = {{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer.assert", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.create", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.assert", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.buffer", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.create", 25}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.finalize", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.execution_barrier", 36}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.dispatch", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.allocator", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.query.i64", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.alloca", 23}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.execute.indirect", 33}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.count", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.get", 15}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.executable.create", 21}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.create", 16}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.await", 15}},};
static const iree_vm_native_export_descriptor_t module_exports_[] = {{{"__init", 6}, {"0v_v", 4}, 0, NULL},{{"main", 4}, {"0r_r", 4}, 0, NULL},};
static const iree_vm_native_function_ptr_t module_funcs_[] = {{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)module___init_export_shim},{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)module_main_export_shim},};
static const iree_vm_native_module_descriptor_t module_descriptor_ = {{"module", 6},0,0,NULL,1,module_dependencies_,17,module_imports_,2,module_exports_,2,module_funcs_,};
static iree_status_t module___main_memoize_apply(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3, iree_vm_ref_t* v4) {
  int64_t v5;
  int64_t v6;
  int64_t v7;
  int32_t v8;
  int32_t v9;
  int32_t v10;
  int32_t v11;
  int32_t v12;
  int32_t v13;
  iree_vm_ref_t v14;
  iree_vm_ref_t* v15;
  iree_host_size_t v16;
  iree_vm_ref_t v17;
  iree_vm_ref_t* v18;
  iree_host_size_t v19;
  iree_vm_ref_t v20;
  iree_vm_ref_t* v21;
  iree_host_size_t v22;
  struct module_state_t* v23;
  iree_vm_ref_t* v24;
  iree_vm_ref_t* v25;
  struct module_t* v26;
  iree_vm_ref_type_t* v27;
  iree_vm_ref_type_t v28;
  iree_vm_type_def_t v29;
  iree_vm_ref_type_t v30;
  iree_status_t v31;
  bool v32;
  struct module_state_t* v33;
  iree_vm_ref_t* v34;
  iree_vm_ref_t* v35;
  struct module_t* v36;
  iree_vm_ref_type_t* v37;
  iree_vm_ref_type_t v38;
  iree_vm_type_def_t v39;
  iree_vm_ref_type_t v40;
  iree_status_t v41;
  bool v42;
  struct module_state_t* v43;
  iree_vm_function_t* v44;
  iree_vm_function_t* v45;
  iree_vm_ref_t v46;
  iree_vm_ref_t* v47;
  iree_host_size_t v48;
  iree_status_t v49;
  bool v50;
  struct module_state_t* v51;
  iree_vm_function_t* v52;
  iree_vm_function_t* v53;
  iree_vm_ref_t v54;
  iree_vm_ref_t* v55;
  iree_host_size_t v56;
  iree_vm_ref_t v57;
  iree_vm_ref_t* v58;
  iree_host_size_t v59;
  iree_vm_ref_t v60;
  iree_vm_ref_t* v61;
  iree_host_size_t v62;
  iree_vm_ref_t v63;
  iree_vm_ref_t* v64;
  iree_host_size_t v65;
  iree_status_t v66;
  bool v67;
  struct module_state_t* v68;
  iree_vm_function_t* v69;
  iree_vm_function_t* v70;
  iree_vm_ref_t v71;
  iree_vm_ref_t* v72;
  iree_host_size_t v73;
  iree_status_t v74;
  bool v75;
  struct module_state_t* v76;
  iree_vm_function_t* v77;
  iree_vm_function_t* v78;
  iree_vm_ref_t v79;
  iree_vm_ref_t* v80;
  iree_host_size_t v81;
  iree_status_t v82;
  bool v83;
  iree_vm_ref_t v84;
  iree_vm_ref_t* v85;
  iree_host_size_t v86;
  iree_status_t v87;
  v5 = -1;
  v6 = 0;
  v7 = 16;
  v8 = 0;
  v9 = 3;
  v10 = 2;
  v11 = 1;
  v12 = 28;
  v13 = 13;
  ;
  v15 = &v14;
  v16 = sizeof(iree_vm_ref_t);
  memset(v15, 0, v16);
  ;
  v18 = &v17;
  v19 = sizeof(iree_vm_ref_t);
  memset(v18, 0, v19);
  ;
  v21 = &v20;
  v22 = sizeof(iree_vm_ref_t);
  memset(v21, 0, v22);
  iree_vm_ref_release(v4);
  ;
  v23 = v3;
  v24 = v23->refs;
  v25 = &v24[0];
  ;
  v26 = v2;
  v27 = v26->types;
  v28 = v27[6];
  v29 = iree_vm_make_ref_type_def(v28);
  v30 = iree_vm_type_def_as_ref(v29);
  v31 = iree_vm_ref_retain_or_move_checked(false, v25, v30, v18);
  v32 = (bool) v31;
  if (v32) {
    goto label8;
  } else {
    goto label2;
  }
label2:
  ;
  v33 = v3;
  v34 = v33->refs;
  v35 = &v34[1];
  ;
  v36 = v2;
  v37 = v36->types;
  v38 = v37[7];
  v39 = iree_vm_make_ref_type_def(v38);
  v40 = iree_vm_type_def_as_ref(v39);
  v41 = iree_vm_ref_retain_or_move_checked(false, v35, v40, v21);
  v42 = (bool) v41;
  if (v42) {
    goto label9;
  } else {
    goto label3;
  }
label3:
  ;
  v43 = v3;
  v44 = v43->imports;
  v45 = &v44[4];
  ;
  v47 = &v46;
  v48 = sizeof(iree_vm_ref_t);
  memset(v47, 0, v48);
  iree_vm_ref_assign(v18, v47);
  v49 = module_call_0riiIi_r_import_shim(v1, v45, v47, v8, v9, v5, v10, v18);
  v50 = (bool) v49;
  if (v50) {
    goto label10;
  } else {
    goto label4;
  }
label4:
  ;
  v51 = v3;
  v52 = v51->imports;
  v53 = &v52[7];
  ;
  v55 = &v54;
  v56 = sizeof(iree_vm_ref_t);
  memset(v55, 0, v56);
  iree_vm_ref_assign(v18, v55);
  ;
  v58 = &v57;
  v59 = sizeof(iree_vm_ref_t);
  memset(v58, 0, v59);
  iree_vm_ref_assign(v21, v58);
  ;
  v61 = &v60;
  v62 = sizeof(iree_vm_ref_t);
  memset(v61, 0, v62);
  iree_vm_ref_assign(v4, v61);
  ;
  v64 = &v63;
  v65 = sizeof(iree_vm_ref_t);
  memset(v64, 0, v65);
  iree_vm_ref_assign(v4, v64);
  v66 = module_call_0rriiiiICiDCiirIID_v_0_2_import_shim(v1, v53, v55, v58, v8, v11, v11, v11, v6, v8, v10, v8, v8, v61, v6, v7, v8, v11, v64, v6, v7);
  v67 = (bool) v66;
  if (v67) {
    goto label11;
  } else {
    goto label5;
  }
label5:
  ;
  v68 = v3;
  v69 = v68->imports;
  v70 = &v69[6];
  ;
  v72 = &v71;
  v73 = sizeof(iree_vm_ref_t);
  memset(v72, 0, v73);
  iree_vm_ref_assign(v18, v72);
  v74 = module_call_0riiI_v_import_shim(v1, v70, v72, v12, v13, v6);
  v75 = (bool) v74;
  if (v75) {
    goto label12;
  } else {
    goto label6;
  }
label6:
  ;
  v76 = v3;
  v77 = v76->imports;
  v78 = &v77[5];
  ;
  v80 = &v79;
  v81 = sizeof(iree_vm_ref_t);
  memset(v80, 0, v81);
  iree_vm_ref_assign(v18, v80);
  v82 = module_call_0r_v_import_shim(v1, v78, v80);
  v83 = (bool) v82;
  if (v83) {
    goto label13;
  } else {
    goto label7;
  }
label7:
  ;
  v85 = &v84;
  v86 = sizeof(iree_vm_ref_t);
  memset(v85, 0, v86);
  iree_vm_ref_move(v18, v85);
  iree_vm_ref_move(v85, v4);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  v87 = iree_ok_status();
  return v87;
label8:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v31;
label9:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v41;
label10:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v49;
label11:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v66;
label12:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v74;
label13:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v18);
  return v82;
}
static iree_status_t module_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8) {
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_host_size_t v22;
  iree_vm_function_t* v23;
  iree_vm_module_t* v24;
  bool v25;
  iree_vm_function_call_t v26;
  iree_vm_function_t v27;
  void* v28;
  uint8_t* v29;
  void* v30;
  uint8_t* v31;
  uint8_t* v32;
  iree_vm_ref_t* v33;
  iree_host_size_t v34;
  uint8_t* v35;
  iree_vm_ref_t* v36;
  iree_host_size_t v37;
  uint8_t* v38;
  iree_vm_ref_t* v39;
  iree_host_size_t v40;
  uint8_t* v41;
  int64_t v42;
  iree_host_size_t v43;
  int64_t* v44;
  iree_host_size_t v45;
  uint8_t* v46;
  int32_t v47;
  iree_host_size_t v48;
  int32_t* v49;
  iree_host_size_t v50;
  uint8_t* v51;
  int32_t v52;
  iree_host_size_t v53;
  int32_t* v54;
  begin_call_t v55;
  iree_vm_module_t* v56;
  iree_vm_function_call_t v57;
  iree_status_t v58;
  bool v59;
  iree_status_t v60;
  iree_status_t v61;
  v9 = 1;
  v10 = 0;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v10 + v11;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(iree_vm_ref_t);
  v16 = v14 + v15;
  v17 = sizeof(int64_t);
  v18 = v16 + v17;
  v19 = sizeof(int32_t);
  v20 = v18 + v19;
  v21 = sizeof(int32_t);
  v22 = v20 + v21;
  ;
  v23 = v2;
  v24 = v23->module;
  v25 = !v24;
  if (v25) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v27 = *v2;
  v26.function = v27;
  v28 = iree_alloca(v22);
  v29 = (uint8_t*) v28;
  v26.arguments.data_length = v22;
  v26.arguments.data = v29;
  memset(v29, 0, v22);
  v30 = iree_alloca(v9);
  v31 = (uint8_t*) v30;
  v26.results.data_length = v10;
  v26.results.data = v31;
  memset(v31, 0, v9);
  v32 = v26.arguments.data;
  v33 = (iree_vm_ref_t*) v32;
  iree_vm_ref_assign(v3, v33);
  v34 = sizeof(iree_vm_ref_t);
  v35 = v32 + v34;
  v36 = (iree_vm_ref_t*) v35;
  iree_vm_ref_assign(v4, v36);
  v37 = sizeof(iree_vm_ref_t);
  v38 = v35 + v37;
  v39 = (iree_vm_ref_t*) v38;
  iree_vm_ref_assign(v5, v39);
  v40 = sizeof(iree_vm_ref_t);
  v41 = v38 + v40;
  ;
  v42 = v6;
  v43 = sizeof(int64_t);
  v44 = &v42;
  memcpy(v41, v44, v43);
  v45 = sizeof(int64_t);
  v46 = v41 + v45;
  ;
  v47 = v7;
  v48 = sizeof(int32_t);
  v49 = &v47;
  memcpy(v46, v49, v48);
  v50 = sizeof(int32_t);
  v51 = v46 + v50;
  ;
  v52 = v8;
  v53 = sizeof(int32_t);
  v54 = &v52;
  memcpy(v51, v54, v53);
  v55 = v23->module->begin_call;
  v56 = v23->module;
  v57 = v26;
  v58 = EMITC_CALL_INDIRECT(v55, v56, v1, v57);
  v59 = (bool) v58;
  if (v59) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v60 = iree_ok_status();
  return v60;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  v61 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v61;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  return v58;
}
static iree_status_t module_call_0rIIiiCID_r_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, iree_vm_ref_t* v10) {
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_host_size_t v22;
  iree_host_size_t v23;
  iree_host_size_t v24;
  iree_host_size_t v25;
  iree_host_size_t v26;
  iree_host_size_t v27;
  iree_vm_function_t* v28;
  iree_vm_module_t* v29;
  bool v30;
  iree_vm_function_call_t v31;
  iree_vm_function_t v32;
  void* v33;
  uint8_t* v34;
  void* v35;
  uint8_t* v36;
  uint8_t* v37;
  iree_vm_ref_t* v38;
  iree_host_size_t v39;
  uint8_t* v40;
  int64_t v41;
  iree_host_size_t v42;
  int64_t* v43;
  iree_host_size_t v44;
  uint8_t* v45;
  int64_t v46;
  iree_host_size_t v47;
  int64_t* v48;
  iree_host_size_t v49;
  uint8_t* v50;
  int32_t v51;
  iree_host_size_t v52;
  int32_t* v53;
  iree_host_size_t v54;
  uint8_t* v55;
  int32_t v56;
  iree_host_size_t v57;
  int32_t* v58;
  iree_host_size_t v59;
  uint8_t* v60;
  int32_t v61;
  iree_host_size_t v62;
  int32_t* v63;
  iree_host_size_t v64;
  uint8_t* v65;
  int64_t v66;
  iree_host_size_t v67;
  int64_t* v68;
  begin_call_t v69;
  iree_vm_module_t* v70;
  iree_vm_function_call_t v71;
  iree_status_t v72;
  bool v73;
  uint8_t* v74;
  iree_vm_ref_t* v75;
  iree_status_t v76;
  iree_status_t v77;
  v11 = 0;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v11 + v12;
  v14 = sizeof(int64_t);
  v15 = v13 + v14;
  v16 = sizeof(int64_t);
  v17 = v15 + v16;
  v18 = sizeof(int32_t);
  v19 = v17 + v18;
  v20 = sizeof(int32_t);
  v21 = v19 + v20;
  v22 = sizeof(int32_t);
  v23 = v21 + v22;
  v24 = sizeof(int64_t);
  v25 = v23 + v24;
  v26 = sizeof(iree_vm_ref_t);
  v27 = v11 + v26;
  ;
  v28 = v2;
  v29 = v28->module;
  v30 = !v29;
  if (v30) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v32 = *v2;
  v31.function = v32;
  v33 = iree_alloca(v25);
  v34 = (uint8_t*) v33;
  v31.arguments.data_length = v25;
  v31.arguments.data = v34;
  memset(v34, 0, v25);
  v35 = iree_alloca(v27);
  v36 = (uint8_t*) v35;
  v31.results.data_length = v27;
  v31.results.data = v36;
  memset(v36, 0, v27);
  v37 = v31.arguments.data;
  v38 = (iree_vm_ref_t*) v37;
  iree_vm_ref_assign(v3, v38);
  v39 = sizeof(iree_vm_ref_t);
  v40 = v37 + v39;
  ;
  v41 = v4;
  v42 = sizeof(int64_t);
  v43 = &v41;
  memcpy(v40, v43, v42);
  v44 = sizeof(int64_t);
  v45 = v40 + v44;
  ;
  v46 = v5;
  v47 = sizeof(int64_t);
  v48 = &v46;
  memcpy(v45, v48, v47);
  v49 = sizeof(int64_t);
  v50 = v45 + v49;
  ;
  v51 = v6;
  v52 = sizeof(int32_t);
  v53 = &v51;
  memcpy(v50, v53, v52);
  v54 = sizeof(int32_t);
  v55 = v50 + v54;
  ;
  v56 = v7;
  v57 = sizeof(int32_t);
  v58 = &v56;
  memcpy(v55, v58, v57);
  v59 = sizeof(int32_t);
  v60 = v55 + v59;
  ;
  v61 = v8;
  v62 = sizeof(int32_t);
  v63 = &v61;
  memcpy(v60, v63, v62);
  v64 = sizeof(int32_t);
  v65 = v60 + v64;
  ;
  v66 = v9;
  v67 = sizeof(int64_t);
  v68 = &v66;
  memcpy(v65, v68, v67);
  v69 = v28->module->begin_call;
  v70 = v28->module;
  v71 = v31;
  v72 = EMITC_CALL_INDIRECT(v69, v70, v1, v71);
  v73 = (bool) v72;
  if (v73) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v74 = v31.results.data;
  v75 = (iree_vm_ref_t*) v74;
  iree_vm_ref_move(v75, v10);
  v76 = iree_ok_status();
  return v76;
label4:
  iree_vm_ref_release(v3);
  v77 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v77;
label5:
  iree_vm_ref_release(v3);
  return v72;
}
static iree_status_t module_call_0rriiCID_v_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8) {
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_host_size_t v22;
  iree_vm_function_t* v23;
  iree_vm_module_t* v24;
  bool v25;
  iree_vm_function_call_t v26;
  iree_vm_function_t v27;
  void* v28;
  uint8_t* v29;
  void* v30;
  uint8_t* v31;
  uint8_t* v32;
  iree_vm_ref_t* v33;
  iree_host_size_t v34;
  uint8_t* v35;
  iree_vm_ref_t* v36;
  iree_host_size_t v37;
  uint8_t* v38;
  int32_t v39;
  iree_host_size_t v40;
  int32_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  int32_t v44;
  iree_host_size_t v45;
  int32_t* v46;
  iree_host_size_t v47;
  uint8_t* v48;
  int32_t v49;
  iree_host_size_t v50;
  int32_t* v51;
  iree_host_size_t v52;
  uint8_t* v53;
  int64_t v54;
  iree_host_size_t v55;
  int64_t* v56;
  begin_call_t v57;
  iree_vm_module_t* v58;
  iree_vm_function_call_t v59;
  iree_status_t v60;
  bool v61;
  iree_status_t v62;
  iree_status_t v63;
  v9 = 1;
  v10 = 0;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v10 + v11;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(int32_t);
  v16 = v14 + v15;
  v17 = sizeof(int32_t);
  v18 = v16 + v17;
  v19 = sizeof(int32_t);
  v20 = v18 + v19;
  v21 = sizeof(int64_t);
  v22 = v20 + v21;
  ;
  v23 = v2;
  v24 = v23->module;
  v25 = !v24;
  if (v25) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v27 = *v2;
  v26.function = v27;
  v28 = iree_alloca(v22);
  v29 = (uint8_t*) v28;
  v26.arguments.data_length = v22;
  v26.arguments.data = v29;
  memset(v29, 0, v22);
  v30 = iree_alloca(v9);
  v31 = (uint8_t*) v30;
  v26.results.data_length = v10;
  v26.results.data = v31;
  memset(v31, 0, v9);
  v32 = v26.arguments.data;
  v33 = (iree_vm_ref_t*) v32;
  iree_vm_ref_assign(v3, v33);
  v34 = sizeof(iree_vm_ref_t);
  v35 = v32 + v34;
  v36 = (iree_vm_ref_t*) v35;
  iree_vm_ref_assign(v4, v36);
  v37 = sizeof(iree_vm_ref_t);
  v38 = v35 + v37;
  ;
  v39 = v5;
  v40 = sizeof(int32_t);
  v41 = &v39;
  memcpy(v38, v41, v40);
  v42 = sizeof(int32_t);
  v43 = v38 + v42;
  ;
  v44 = v6;
  v45 = sizeof(int32_t);
  v46 = &v44;
  memcpy(v43, v46, v45);
  v47 = sizeof(int32_t);
  v48 = v43 + v47;
  ;
  v49 = v7;
  v50 = sizeof(int32_t);
  v51 = &v49;
  memcpy(v48, v51, v50);
  v52 = sizeof(int32_t);
  v53 = v48 + v52;
  ;
  v54 = v8;
  v55 = sizeof(int64_t);
  v56 = &v54;
  memcpy(v53, v56, v55);
  v57 = v23->module->begin_call;
  v58 = v23->module;
  v59 = v26;
  v60 = EMITC_CALL_INDIRECT(v57, v58, v1, v59);
  v61 = (bool) v60;
  if (v61) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v62 = iree_ok_status();
  return v62;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v63 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v63;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v60;
}
static iree_status_t module_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4) {
  iree_host_size_t v5;
  iree_host_size_t v6;
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_vm_function_t* v10;
  iree_vm_module_t* v11;
  bool v12;
  iree_vm_function_call_t v13;
  iree_vm_function_t v14;
  void* v15;
  uint8_t* v16;
  void* v17;
  uint8_t* v18;
  uint8_t* v19;
  iree_vm_ref_t* v20;
  begin_call_t v21;
  iree_vm_module_t* v22;
  iree_vm_function_call_t v23;
  iree_status_t v24;
  bool v25;
  uint8_t* v26;
  iree_vm_ref_t* v27;
  iree_status_t v28;
  iree_status_t v29;
  v5 = 0;
  v6 = sizeof(iree_vm_ref_t);
  v7 = v5 + v6;
  v8 = sizeof(iree_vm_ref_t);
  v9 = v5 + v8;
  ;
  v10 = v2;
  v11 = v10->module;
  v12 = !v11;
  if (v12) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v14 = *v2;
  v13.function = v14;
  v15 = iree_alloca(v7);
  v16 = (uint8_t*) v15;
  v13.arguments.data_length = v7;
  v13.arguments.data = v16;
  memset(v16, 0, v7);
  v17 = iree_alloca(v9);
  v18 = (uint8_t*) v17;
  v13.results.data_length = v9;
  v13.results.data = v18;
  memset(v18, 0, v9);
  v19 = v13.arguments.data;
  v20 = (iree_vm_ref_t*) v19;
  iree_vm_ref_assign(v3, v20);
  v21 = v10->module->begin_call;
  v22 = v10->module;
  v23 = v13;
  v24 = EMITC_CALL_INDIRECT(v21, v22, v1, v23);
  v25 = (bool) v24;
  if (v25) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v26 = v13.results.data;
  v27 = (iree_vm_ref_t*) v26;
  iree_vm_ref_move(v27, v4);
  v28 = iree_ok_status();
  return v28;
label4:
  iree_vm_ref_release(v3);
  v29 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v29;
label5:
  iree_vm_ref_release(v3);
  return v24;
}
static iree_status_t module_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8) {
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_vm_function_t* v22;
  iree_vm_module_t* v23;
  bool v24;
  iree_vm_function_call_t v25;
  iree_vm_function_t v26;
  void* v27;
  uint8_t* v28;
  void* v29;
  uint8_t* v30;
  uint8_t* v31;
  iree_vm_ref_t* v32;
  iree_host_size_t v33;
  uint8_t* v34;
  int32_t v35;
  iree_host_size_t v36;
  int32_t* v37;
  iree_host_size_t v38;
  uint8_t* v39;
  int32_t v40;
  iree_host_size_t v41;
  int32_t* v42;
  iree_host_size_t v43;
  uint8_t* v44;
  int64_t v45;
  iree_host_size_t v46;
  int64_t* v47;
  iree_host_size_t v48;
  uint8_t* v49;
  int32_t v50;
  iree_host_size_t v51;
  int32_t* v52;
  begin_call_t v53;
  iree_vm_module_t* v54;
  iree_vm_function_call_t v55;
  iree_status_t v56;
  bool v57;
  uint8_t* v58;
  iree_vm_ref_t* v59;
  iree_status_t v60;
  iree_status_t v61;
  v9 = 0;
  v10 = sizeof(iree_vm_ref_t);
  v11 = v9 + v10;
  v12 = sizeof(int32_t);
  v13 = v11 + v12;
  v14 = sizeof(int32_t);
  v15 = v13 + v14;
  v16 = sizeof(int64_t);
  v17 = v15 + v16;
  v18 = sizeof(int32_t);
  v19 = v17 + v18;
  v20 = sizeof(iree_vm_ref_t);
  v21 = v9 + v20;
  ;
  v22 = v2;
  v23 = v22->module;
  v24 = !v23;
  if (v24) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v26 = *v2;
  v25.function = v26;
  v27 = iree_alloca(v19);
  v28 = (uint8_t*) v27;
  v25.arguments.data_length = v19;
  v25.arguments.data = v28;
  memset(v28, 0, v19);
  v29 = iree_alloca(v21);
  v30 = (uint8_t*) v29;
  v25.results.data_length = v21;
  v25.results.data = v30;
  memset(v30, 0, v21);
  v31 = v25.arguments.data;
  v32 = (iree_vm_ref_t*) v31;
  iree_vm_ref_assign(v3, v32);
  v33 = sizeof(iree_vm_ref_t);
  v34 = v31 + v33;
  ;
  v35 = v4;
  v36 = sizeof(int32_t);
  v37 = &v35;
  memcpy(v34, v37, v36);
  v38 = sizeof(int32_t);
  v39 = v34 + v38;
  ;
  v40 = v5;
  v41 = sizeof(int32_t);
  v42 = &v40;
  memcpy(v39, v42, v41);
  v43 = sizeof(int32_t);
  v44 = v39 + v43;
  ;
  v45 = v6;
  v46 = sizeof(int64_t);
  v47 = &v45;
  memcpy(v44, v47, v46);
  v48 = sizeof(int64_t);
  v49 = v44 + v48;
  ;
  v50 = v7;
  v51 = sizeof(int32_t);
  v52 = &v50;
  memcpy(v49, v52, v51);
  v53 = v22->module->begin_call;
  v54 = v22->module;
  v55 = v25;
  v56 = EMITC_CALL_INDIRECT(v53, v54, v1, v55);
  v57 = (bool) v56;
  if (v57) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v58 = v25.results.data;
  v59 = (iree_vm_ref_t*) v58;
  iree_vm_ref_move(v59, v8);
  v60 = iree_ok_status();
  return v60;
label4:
  iree_vm_ref_release(v3);
  v61 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v61;
label5:
  iree_vm_ref_release(v3);
  return v56;
}
static iree_status_t module_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3) {
  iree_host_size_t v4;
  iree_host_size_t v5;
  iree_host_size_t v6;
  iree_host_size_t v7;
  iree_vm_function_t* v8;
  iree_vm_module_t* v9;
  bool v10;
  iree_vm_function_call_t v11;
  iree_vm_function_t v12;
  void* v13;
  uint8_t* v14;
  void* v15;
  uint8_t* v16;
  uint8_t* v17;
  iree_vm_ref_t* v18;
  begin_call_t v19;
  iree_vm_module_t* v20;
  iree_vm_function_call_t v21;
  iree_status_t v22;
  bool v23;
  iree_status_t v24;
  iree_status_t v25;
  v4 = 1;
  v5 = 0;
  v6 = sizeof(iree_vm_ref_t);
  v7 = v5 + v6;
  ;
  v8 = v2;
  v9 = v8->module;
  v10 = !v9;
  if (v10) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v12 = *v2;
  v11.function = v12;
  v13 = iree_alloca(v7);
  v14 = (uint8_t*) v13;
  v11.arguments.data_length = v7;
  v11.arguments.data = v14;
  memset(v14, 0, v7);
  v15 = iree_alloca(v4);
  v16 = (uint8_t*) v15;
  v11.results.data_length = v5;
  v11.results.data = v16;
  memset(v16, 0, v4);
  v17 = v11.arguments.data;
  v18 = (iree_vm_ref_t*) v17;
  iree_vm_ref_assign(v3, v18);
  v19 = v8->module->begin_call;
  v20 = v8->module;
  v21 = v11;
  v22 = EMITC_CALL_INDIRECT(v19, v20, v1, v21);
  v23 = (bool) v22;
  if (v23) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v24 = iree_ok_status();
  return v24;
label4:
  iree_vm_ref_release(v3);
  v25 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v25;
label5:
  iree_vm_ref_release(v3);
  return v22;
}
static iree_status_t module_call_0riiI_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6) {
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_vm_function_t* v17;
  iree_vm_module_t* v18;
  bool v19;
  iree_vm_function_call_t v20;
  iree_vm_function_t v21;
  void* v22;
  uint8_t* v23;
  void* v24;
  uint8_t* v25;
  uint8_t* v26;
  iree_vm_ref_t* v27;
  iree_host_size_t v28;
  uint8_t* v29;
  int32_t v30;
  iree_host_size_t v31;
  int32_t* v32;
  iree_host_size_t v33;
  uint8_t* v34;
  int32_t v35;
  iree_host_size_t v36;
  int32_t* v37;
  iree_host_size_t v38;
  uint8_t* v39;
  int64_t v40;
  iree_host_size_t v41;
  int64_t* v42;
  begin_call_t v43;
  iree_vm_module_t* v44;
  iree_vm_function_call_t v45;
  iree_status_t v46;
  bool v47;
  iree_status_t v48;
  iree_status_t v49;
  v7 = 1;
  v8 = 0;
  v9 = sizeof(iree_vm_ref_t);
  v10 = v8 + v9;
  v11 = sizeof(int32_t);
  v12 = v10 + v11;
  v13 = sizeof(int32_t);
  v14 = v12 + v13;
  v15 = sizeof(int64_t);
  v16 = v14 + v15;
  ;
  v17 = v2;
  v18 = v17->module;
  v19 = !v18;
  if (v19) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v21 = *v2;
  v20.function = v21;
  v22 = iree_alloca(v16);
  v23 = (uint8_t*) v22;
  v20.arguments.data_length = v16;
  v20.arguments.data = v23;
  memset(v23, 0, v16);
  v24 = iree_alloca(v7);
  v25 = (uint8_t*) v24;
  v20.results.data_length = v8;
  v20.results.data = v25;
  memset(v25, 0, v7);
  v26 = v20.arguments.data;
  v27 = (iree_vm_ref_t*) v26;
  iree_vm_ref_assign(v3, v27);
  v28 = sizeof(iree_vm_ref_t);
  v29 = v26 + v28;
  ;
  v30 = v4;
  v31 = sizeof(int32_t);
  v32 = &v30;
  memcpy(v29, v32, v31);
  v33 = sizeof(int32_t);
  v34 = v29 + v33;
  ;
  v35 = v5;
  v36 = sizeof(int32_t);
  v37 = &v35;
  memcpy(v34, v37, v36);
  v38 = sizeof(int32_t);
  v39 = v34 + v38;
  ;
  v40 = v6;
  v41 = sizeof(int64_t);
  v42 = &v40;
  memcpy(v39, v42, v41);
  v43 = v17->module->begin_call;
  v44 = v17->module;
  v45 = v20;
  v46 = EMITC_CALL_INDIRECT(v43, v44, v1, v45);
  v47 = (bool) v46;
  if (v47) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v48 = iree_ok_status();
  return v48;
label4:
  iree_vm_ref_release(v3);
  v49 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v49;
label5:
  iree_vm_ref_release(v3);
  return v46;
}
static iree_status_t module_call_0rriiiiICiDCiirIID_v_0_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, iree_vm_ref_t* v14, int64_t v15, int64_t v16, int32_t v17, int32_t v18, iree_vm_ref_t* v19, int64_t v20, int64_t v21) {
  iree_host_size_t v22;
  iree_host_size_t v23;
  iree_host_size_t v24;
  iree_host_size_t v25;
  iree_host_size_t v26;
  iree_host_size_t v27;
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_host_size_t v31;
  iree_host_size_t v32;
  iree_host_size_t v33;
  iree_host_size_t v34;
  iree_host_size_t v35;
  iree_host_size_t v36;
  iree_host_size_t v37;
  iree_host_size_t v38;
  iree_host_size_t v39;
  iree_host_size_t v40;
  iree_host_size_t v41;
  iree_host_size_t v42;
  iree_host_size_t v43;
  iree_host_size_t v44;
  iree_host_size_t v45;
  iree_host_size_t v46;
  iree_host_size_t v47;
  iree_host_size_t v48;
  iree_host_size_t v49;
  iree_host_size_t v50;
  iree_host_size_t v51;
  iree_host_size_t v52;
  iree_host_size_t v53;
  iree_host_size_t v54;
  iree_host_size_t v55;
  iree_host_size_t v56;
  iree_host_size_t v57;
  iree_host_size_t v58;
  iree_host_size_t v59;
  iree_host_size_t v60;
  iree_host_size_t v61;
  iree_vm_function_t* v62;
  iree_vm_module_t* v63;
  bool v64;
  iree_vm_function_call_t v65;
  iree_vm_function_t v66;
  void* v67;
  uint8_t* v68;
  void* v69;
  uint8_t* v70;
  uint8_t* v71;
  iree_vm_ref_t* v72;
  iree_host_size_t v73;
  uint8_t* v74;
  iree_vm_ref_t* v75;
  iree_host_size_t v76;
  uint8_t* v77;
  int32_t v78;
  iree_host_size_t v79;
  int32_t* v80;
  iree_host_size_t v81;
  uint8_t* v82;
  int32_t v83;
  iree_host_size_t v84;
  int32_t* v85;
  iree_host_size_t v86;
  uint8_t* v87;
  int32_t v88;
  iree_host_size_t v89;
  int32_t* v90;
  iree_host_size_t v91;
  uint8_t* v92;
  int32_t v93;
  iree_host_size_t v94;
  int32_t* v95;
  iree_host_size_t v96;
  uint8_t* v97;
  int64_t v98;
  iree_host_size_t v99;
  int64_t* v100;
  iree_host_size_t v101;
  uint8_t* v102;
  int32_t v103;
  iree_host_size_t v104;
  int32_t* v105;
  iree_host_size_t v106;
  uint8_t* v107;
  int32_t v108;
  iree_host_size_t v109;
  int32_t* v110;
  iree_host_size_t v111;
  uint8_t* v112;
  int32_t v113;
  iree_host_size_t v114;
  int32_t* v115;
  iree_host_size_t v116;
  uint8_t* v117;
  int32_t v118;
  iree_host_size_t v119;
  int32_t* v120;
  iree_host_size_t v121;
  uint8_t* v122;
  iree_vm_ref_t* v123;
  iree_host_size_t v124;
  uint8_t* v125;
  int64_t v126;
  iree_host_size_t v127;
  int64_t* v128;
  iree_host_size_t v129;
  uint8_t* v130;
  int64_t v131;
  iree_host_size_t v132;
  int64_t* v133;
  iree_host_size_t v134;
  uint8_t* v135;
  int32_t v136;
  iree_host_size_t v137;
  int32_t* v138;
  iree_host_size_t v139;
  uint8_t* v140;
  int32_t v141;
  iree_host_size_t v142;
  int32_t* v143;
  iree_host_size_t v144;
  uint8_t* v145;
  iree_vm_ref_t* v146;
  iree_host_size_t v147;
  uint8_t* v148;
  int64_t v149;
  iree_host_size_t v150;
  int64_t* v151;
  iree_host_size_t v152;
  uint8_t* v153;
  int64_t v154;
  iree_host_size_t v155;
  int64_t* v156;
  begin_call_t v157;
  iree_vm_module_t* v158;
  iree_vm_function_call_t v159;
  iree_status_t v160;
  bool v161;
  iree_status_t v162;
  iree_status_t v163;
  v22 = 1;
  v23 = 0;
  v24 = sizeof(iree_vm_ref_t);
  v25 = v23 + v24;
  v26 = sizeof(iree_vm_ref_t);
  v27 = v25 + v26;
  v28 = sizeof(int32_t);
  v29 = v27 + v28;
  v30 = sizeof(int32_t);
  v31 = v29 + v30;
  v32 = sizeof(int32_t);
  v33 = v31 + v32;
  v34 = sizeof(int32_t);
  v35 = v33 + v34;
  v36 = sizeof(int64_t);
  v37 = v35 + v36;
  v38 = sizeof(int32_t);
  v39 = v37 + v38;
  v40 = sizeof(int32_t);
  v41 = v39 + v40;
  v42 = sizeof(int32_t);
  v43 = v41 + v42;
  v44 = sizeof(int32_t);
  v45 = v43 + v44;
  v46 = sizeof(iree_vm_ref_t);
  v47 = v45 + v46;
  v48 = sizeof(int64_t);
  v49 = v47 + v48;
  v50 = sizeof(int64_t);
  v51 = v49 + v50;
  v52 = sizeof(int32_t);
  v53 = v51 + v52;
  v54 = sizeof(int32_t);
  v55 = v53 + v54;
  v56 = sizeof(iree_vm_ref_t);
  v57 = v55 + v56;
  v58 = sizeof(int64_t);
  v59 = v57 + v58;
  v60 = sizeof(int64_t);
  v61 = v59 + v60;
  ;
  v62 = v2;
  v63 = v62->module;
  v64 = !v63;
  if (v64) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v66 = *v2;
  v65.function = v66;
  v67 = iree_alloca(v61);
  v68 = (uint8_t*) v67;
  v65.arguments.data_length = v61;
  v65.arguments.data = v68;
  memset(v68, 0, v61);
  v69 = iree_alloca(v22);
  v70 = (uint8_t*) v69;
  v65.results.data_length = v23;
  v65.results.data = v70;
  memset(v70, 0, v22);
  v71 = v65.arguments.data;
  v72 = (iree_vm_ref_t*) v71;
  iree_vm_ref_assign(v3, v72);
  v73 = sizeof(iree_vm_ref_t);
  v74 = v71 + v73;
  v75 = (iree_vm_ref_t*) v74;
  iree_vm_ref_assign(v4, v75);
  v76 = sizeof(iree_vm_ref_t);
  v77 = v74 + v76;
  ;
  v78 = v5;
  v79 = sizeof(int32_t);
  v80 = &v78;
  memcpy(v77, v80, v79);
  v81 = sizeof(int32_t);
  v82 = v77 + v81;
  ;
  v83 = v6;
  v84 = sizeof(int32_t);
  v85 = &v83;
  memcpy(v82, v85, v84);
  v86 = sizeof(int32_t);
  v87 = v82 + v86;
  ;
  v88 = v7;
  v89 = sizeof(int32_t);
  v90 = &v88;
  memcpy(v87, v90, v89);
  v91 = sizeof(int32_t);
  v92 = v87 + v91;
  ;
  v93 = v8;
  v94 = sizeof(int32_t);
  v95 = &v93;
  memcpy(v92, v95, v94);
  v96 = sizeof(int32_t);
  v97 = v92 + v96;
  ;
  v98 = v9;
  v99 = sizeof(int64_t);
  v100 = &v98;
  memcpy(v97, v100, v99);
  v101 = sizeof(int64_t);
  v102 = v97 + v101;
  ;
  v103 = v10;
  v104 = sizeof(int32_t);
  v105 = &v103;
  memcpy(v102, v105, v104);
  v106 = sizeof(int32_t);
  v107 = v102 + v106;
  ;
  v108 = v11;
  v109 = sizeof(int32_t);
  v110 = &v108;
  memcpy(v107, v110, v109);
  v111 = sizeof(int32_t);
  v112 = v107 + v111;
  ;
  v113 = v12;
  v114 = sizeof(int32_t);
  v115 = &v113;
  memcpy(v112, v115, v114);
  v116 = sizeof(int32_t);
  v117 = v112 + v116;
  ;
  v118 = v13;
  v119 = sizeof(int32_t);
  v120 = &v118;
  memcpy(v117, v120, v119);
  v121 = sizeof(int32_t);
  v122 = v117 + v121;
  v123 = (iree_vm_ref_t*) v122;
  iree_vm_ref_assign(v14, v123);
  v124 = sizeof(iree_vm_ref_t);
  v125 = v122 + v124;
  ;
  v126 = v15;
  v127 = sizeof(int64_t);
  v128 = &v126;
  memcpy(v125, v128, v127);
  v129 = sizeof(int64_t);
  v130 = v125 + v129;
  ;
  v131 = v16;
  v132 = sizeof(int64_t);
  v133 = &v131;
  memcpy(v130, v133, v132);
  v134 = sizeof(int64_t);
  v135 = v130 + v134;
  ;
  v136 = v17;
  v137 = sizeof(int32_t);
  v138 = &v136;
  memcpy(v135, v138, v137);
  v139 = sizeof(int32_t);
  v140 = v135 + v139;
  ;
  v141 = v18;
  v142 = sizeof(int32_t);
  v143 = &v141;
  memcpy(v140, v143, v142);
  v144 = sizeof(int32_t);
  v145 = v140 + v144;
  v146 = (iree_vm_ref_t*) v145;
  iree_vm_ref_assign(v19, v146);
  v147 = sizeof(iree_vm_ref_t);
  v148 = v145 + v147;
  ;
  v149 = v20;
  v150 = sizeof(int64_t);
  v151 = &v149;
  memcpy(v148, v151, v150);
  v152 = sizeof(int64_t);
  v153 = v148 + v152;
  ;
  v154 = v21;
  v155 = sizeof(int64_t);
  v156 = &v154;
  memcpy(v153, v156, v155);
  v157 = v62->module->begin_call;
  v158 = v62->module;
  v159 = v65;
  v160 = EMITC_CALL_INDIRECT(v157, v158, v1, v159);
  v161 = (bool) v160;
  if (v161) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v162 = iree_ok_status();
  return v162;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v163 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v163;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v160;
}
static iree_status_t module_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7) {
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_vm_function_t* v19;
  iree_vm_module_t* v20;
  bool v21;
  iree_vm_function_call_t v22;
  iree_vm_function_t v23;
  void* v24;
  uint8_t* v25;
  void* v26;
  uint8_t* v27;
  uint8_t* v28;
  iree_vm_ref_t* v29;
  iree_host_size_t v30;
  uint8_t* v31;
  iree_vm_ref_t* v32;
  iree_host_size_t v33;
  uint8_t* v34;
  iree_vm_ref_t* v35;
  begin_call_t v36;
  iree_vm_module_t* v37;
  iree_vm_function_call_t v38;
  iree_status_t v39;
  bool v40;
  uint8_t* v41;
  iree_host_size_t v42;
  iree_host_size_t v43;
  uint8_t* v44;
  iree_host_size_t v45;
  iree_status_t v46;
  iree_status_t v47;
  v8 = 0;
  v9 = sizeof(iree_vm_ref_t);
  v10 = v8 + v9;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v10 + v11;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(int32_t);
  v16 = v8 + v15;
  v17 = sizeof(int64_t);
  v18 = v16 + v17;
  ;
  v19 = v2;
  v20 = v19->module;
  v21 = !v20;
  if (v21) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v23 = *v2;
  v22.function = v23;
  v24 = iree_alloca(v14);
  v25 = (uint8_t*) v24;
  v22.arguments.data_length = v14;
  v22.arguments.data = v25;
  memset(v25, 0, v14);
  v26 = iree_alloca(v18);
  v27 = (uint8_t*) v26;
  v22.results.data_length = v18;
  v22.results.data = v27;
  memset(v27, 0, v18);
  v28 = v22.arguments.data;
  v29 = (iree_vm_ref_t*) v28;
  iree_vm_ref_assign(v3, v29);
  v30 = sizeof(iree_vm_ref_t);
  v31 = v28 + v30;
  v32 = (iree_vm_ref_t*) v31;
  iree_vm_ref_assign(v4, v32);
  v33 = sizeof(iree_vm_ref_t);
  v34 = v31 + v33;
  v35 = (iree_vm_ref_t*) v34;
  iree_vm_ref_assign(v5, v35);
  v36 = v19->module->begin_call;
  v37 = v19->module;
  v38 = v22;
  v39 = EMITC_CALL_INDIRECT(v36, v37, v1, v38);
  v40 = (bool) v39;
  if (v40) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v41 = v22.results.data;
  v42 = sizeof(int32_t);
  memcpy(v6, v41, v42);
  v43 = sizeof(int32_t);
  v44 = v41 + v43;
  v45 = sizeof(int64_t);
  memcpy(v7, v44, v45);
  v46 = iree_ok_status();
  return v46;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  v47 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v47;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  return v39;
}
static iree_status_t module_call_0rIrrIiiII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int64_t v7, int32_t v8, int32_t v9, int64_t v10, int64_t v11, iree_vm_ref_t* v12) {
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_host_size_t v22;
  iree_host_size_t v23;
  iree_host_size_t v24;
  iree_host_size_t v25;
  iree_host_size_t v26;
  iree_host_size_t v27;
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_host_size_t v31;
  iree_host_size_t v32;
  iree_host_size_t v33;
  iree_vm_function_t* v34;
  iree_vm_module_t* v35;
  bool v36;
  iree_vm_function_call_t v37;
  iree_vm_function_t v38;
  void* v39;
  uint8_t* v40;
  void* v41;
  uint8_t* v42;
  uint8_t* v43;
  iree_vm_ref_t* v44;
  iree_host_size_t v45;
  uint8_t* v46;
  int64_t v47;
  iree_host_size_t v48;
  int64_t* v49;
  iree_host_size_t v50;
  uint8_t* v51;
  iree_vm_ref_t* v52;
  iree_host_size_t v53;
  uint8_t* v54;
  iree_vm_ref_t* v55;
  iree_host_size_t v56;
  uint8_t* v57;
  int64_t v58;
  iree_host_size_t v59;
  int64_t* v60;
  iree_host_size_t v61;
  uint8_t* v62;
  int32_t v63;
  iree_host_size_t v64;
  int32_t* v65;
  iree_host_size_t v66;
  uint8_t* v67;
  int32_t v68;
  iree_host_size_t v69;
  int32_t* v70;
  iree_host_size_t v71;
  uint8_t* v72;
  int64_t v73;
  iree_host_size_t v74;
  int64_t* v75;
  iree_host_size_t v76;
  uint8_t* v77;
  int64_t v78;
  iree_host_size_t v79;
  int64_t* v80;
  begin_call_t v81;
  iree_vm_module_t* v82;
  iree_vm_function_call_t v83;
  iree_status_t v84;
  bool v85;
  uint8_t* v86;
  iree_vm_ref_t* v87;
  iree_status_t v88;
  iree_status_t v89;
  v13 = 0;
  v14 = sizeof(iree_vm_ref_t);
  v15 = v13 + v14;
  v16 = sizeof(int64_t);
  v17 = v15 + v16;
  v18 = sizeof(iree_vm_ref_t);
  v19 = v17 + v18;
  v20 = sizeof(iree_vm_ref_t);
  v21 = v19 + v20;
  v22 = sizeof(int64_t);
  v23 = v21 + v22;
  v24 = sizeof(int32_t);
  v25 = v23 + v24;
  v26 = sizeof(int32_t);
  v27 = v25 + v26;
  v28 = sizeof(int64_t);
  v29 = v27 + v28;
  v30 = sizeof(int64_t);
  v31 = v29 + v30;
  v32 = sizeof(iree_vm_ref_t);
  v33 = v13 + v32;
  ;
  v34 = v2;
  v35 = v34->module;
  v36 = !v35;
  if (v36) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v38 = *v2;
  v37.function = v38;
  v39 = iree_alloca(v31);
  v40 = (uint8_t*) v39;
  v37.arguments.data_length = v31;
  v37.arguments.data = v40;
  memset(v40, 0, v31);
  v41 = iree_alloca(v33);
  v42 = (uint8_t*) v41;
  v37.results.data_length = v33;
  v37.results.data = v42;
  memset(v42, 0, v33);
  v43 = v37.arguments.data;
  v44 = (iree_vm_ref_t*) v43;
  iree_vm_ref_assign(v3, v44);
  v45 = sizeof(iree_vm_ref_t);
  v46 = v43 + v45;
  ;
  v47 = v4;
  v48 = sizeof(int64_t);
  v49 = &v47;
  memcpy(v46, v49, v48);
  v50 = sizeof(int64_t);
  v51 = v46 + v50;
  v52 = (iree_vm_ref_t*) v51;
  iree_vm_ref_assign(v5, v52);
  v53 = sizeof(iree_vm_ref_t);
  v54 = v51 + v53;
  v55 = (iree_vm_ref_t*) v54;
  iree_vm_ref_assign(v6, v55);
  v56 = sizeof(iree_vm_ref_t);
  v57 = v54 + v56;
  ;
  v58 = v7;
  v59 = sizeof(int64_t);
  v60 = &v58;
  memcpy(v57, v60, v59);
  v61 = sizeof(int64_t);
  v62 = v57 + v61;
  ;
  v63 = v8;
  v64 = sizeof(int32_t);
  v65 = &v63;
  memcpy(v62, v65, v64);
  v66 = sizeof(int32_t);
  v67 = v62 + v66;
  ;
  v68 = v9;
  v69 = sizeof(int32_t);
  v70 = &v68;
  memcpy(v67, v70, v69);
  v71 = sizeof(int32_t);
  v72 = v67 + v71;
  ;
  v73 = v10;
  v74 = sizeof(int64_t);
  v75 = &v73;
  memcpy(v72, v75, v74);
  v76 = sizeof(int64_t);
  v77 = v72 + v76;
  ;
  v78 = v11;
  v79 = sizeof(int64_t);
  v80 = &v78;
  memcpy(v77, v80, v79);
  v81 = v34->module->begin_call;
  v82 = v34->module;
  v83 = v37;
  v84 = EMITC_CALL_INDIRECT(v81, v82, v1, v83);
  v85 = (bool) v84;
  if (v85) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v86 = v37.results.data;
  v87 = (iree_vm_ref_t*) v86;
  iree_vm_ref_move(v87, v12);
  v88 = iree_ok_status();
  return v88;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  v89 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v89;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  return v84;
}
static iree_status_t module_call_0rIrrrICrIID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10, int64_t v11, int64_t v12, iree_vm_ref_t* v13, int64_t v14, int64_t v15) {
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_host_size_t v22;
  iree_host_size_t v23;
  iree_host_size_t v24;
  iree_host_size_t v25;
  iree_host_size_t v26;
  iree_host_size_t v27;
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_host_size_t v31;
  iree_host_size_t v32;
  iree_host_size_t v33;
  iree_host_size_t v34;
  iree_host_size_t v35;
  iree_host_size_t v36;
  iree_host_size_t v37;
  iree_host_size_t v38;
  iree_host_size_t v39;
  iree_host_size_t v40;
  iree_host_size_t v41;
  iree_host_size_t v42;
  iree_host_size_t v43;
  iree_vm_function_t* v44;
  iree_vm_module_t* v45;
  bool v46;
  iree_vm_function_call_t v47;
  iree_vm_function_t v48;
  void* v49;
  uint8_t* v50;
  void* v51;
  uint8_t* v52;
  uint8_t* v53;
  iree_vm_ref_t* v54;
  iree_host_size_t v55;
  uint8_t* v56;
  int64_t v57;
  iree_host_size_t v58;
  int64_t* v59;
  iree_host_size_t v60;
  uint8_t* v61;
  iree_vm_ref_t* v62;
  iree_host_size_t v63;
  uint8_t* v64;
  iree_vm_ref_t* v65;
  iree_host_size_t v66;
  uint8_t* v67;
  iree_vm_ref_t* v68;
  iree_host_size_t v69;
  uint8_t* v70;
  int64_t v71;
  iree_host_size_t v72;
  int64_t* v73;
  iree_host_size_t v74;
  uint8_t* v75;
  int32_t v76;
  iree_host_size_t v77;
  int32_t* v78;
  iree_host_size_t v79;
  uint8_t* v80;
  iree_vm_ref_t* v81;
  iree_host_size_t v82;
  uint8_t* v83;
  int64_t v84;
  iree_host_size_t v85;
  int64_t* v86;
  iree_host_size_t v87;
  uint8_t* v88;
  int64_t v89;
  iree_host_size_t v90;
  int64_t* v91;
  iree_host_size_t v92;
  uint8_t* v93;
  iree_vm_ref_t* v94;
  iree_host_size_t v95;
  uint8_t* v96;
  int64_t v97;
  iree_host_size_t v98;
  int64_t* v99;
  iree_host_size_t v100;
  uint8_t* v101;
  int64_t v102;
  iree_host_size_t v103;
  int64_t* v104;
  begin_call_t v105;
  iree_vm_module_t* v106;
  iree_vm_function_call_t v107;
  iree_status_t v108;
  bool v109;
  iree_status_t v110;
  iree_status_t v111;
  v16 = 1;
  v17 = 0;
  v18 = sizeof(iree_vm_ref_t);
  v19 = v17 + v18;
  v20 = sizeof(int64_t);
  v21 = v19 + v20;
  v22 = sizeof(iree_vm_ref_t);
  v23 = v21 + v22;
  v24 = sizeof(iree_vm_ref_t);
  v25 = v23 + v24;
  v26 = sizeof(iree_vm_ref_t);
  v27 = v25 + v26;
  v28 = sizeof(int64_t);
  v29 = v27 + v28;
  v30 = sizeof(int32_t);
  v31 = v29 + v30;
  v32 = sizeof(iree_vm_ref_t);
  v33 = v31 + v32;
  v34 = sizeof(int64_t);
  v35 = v33 + v34;
  v36 = sizeof(int64_t);
  v37 = v35 + v36;
  v38 = sizeof(iree_vm_ref_t);
  v39 = v37 + v38;
  v40 = sizeof(int64_t);
  v41 = v39 + v40;
  v42 = sizeof(int64_t);
  v43 = v41 + v42;
  ;
  v44 = v2;
  v45 = v44->module;
  v46 = !v45;
  if (v46) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v48 = *v2;
  v47.function = v48;
  v49 = iree_alloca(v43);
  v50 = (uint8_t*) v49;
  v47.arguments.data_length = v43;
  v47.arguments.data = v50;
  memset(v50, 0, v43);
  v51 = iree_alloca(v16);
  v52 = (uint8_t*) v51;
  v47.results.data_length = v17;
  v47.results.data = v52;
  memset(v52, 0, v16);
  v53 = v47.arguments.data;
  v54 = (iree_vm_ref_t*) v53;
  iree_vm_ref_assign(v3, v54);
  v55 = sizeof(iree_vm_ref_t);
  v56 = v53 + v55;
  ;
  v57 = v4;
  v58 = sizeof(int64_t);
  v59 = &v57;
  memcpy(v56, v59, v58);
  v60 = sizeof(int64_t);
  v61 = v56 + v60;
  v62 = (iree_vm_ref_t*) v61;
  iree_vm_ref_assign(v5, v62);
  v63 = sizeof(iree_vm_ref_t);
  v64 = v61 + v63;
  v65 = (iree_vm_ref_t*) v64;
  iree_vm_ref_assign(v6, v65);
  v66 = sizeof(iree_vm_ref_t);
  v67 = v64 + v66;
  v68 = (iree_vm_ref_t*) v67;
  iree_vm_ref_assign(v7, v68);
  v69 = sizeof(iree_vm_ref_t);
  v70 = v67 + v69;
  ;
  v71 = v8;
  v72 = sizeof(int64_t);
  v73 = &v71;
  memcpy(v70, v73, v72);
  v74 = sizeof(int64_t);
  v75 = v70 + v74;
  ;
  v76 = v9;
  v77 = sizeof(int32_t);
  v78 = &v76;
  memcpy(v75, v78, v77);
  v79 = sizeof(int32_t);
  v80 = v75 + v79;
  v81 = (iree_vm_ref_t*) v80;
  iree_vm_ref_assign(v10, v81);
  v82 = sizeof(iree_vm_ref_t);
  v83 = v80 + v82;
  ;
  v84 = v11;
  v85 = sizeof(int64_t);
  v86 = &v84;
  memcpy(v83, v86, v85);
  v87 = sizeof(int64_t);
  v88 = v83 + v87;
  ;
  v89 = v12;
  v90 = sizeof(int64_t);
  v91 = &v89;
  memcpy(v88, v91, v90);
  v92 = sizeof(int64_t);
  v93 = v88 + v92;
  v94 = (iree_vm_ref_t*) v93;
  iree_vm_ref_assign(v13, v94);
  v95 = sizeof(iree_vm_ref_t);
  v96 = v93 + v95;
  ;
  v97 = v14;
  v98 = sizeof(int64_t);
  v99 = &v97;
  memcpy(v96, v99, v98);
  v100 = sizeof(int64_t);
  v101 = v96 + v100;
  ;
  v102 = v15;
  v103 = sizeof(int64_t);
  v104 = &v102;
  memcpy(v101, v104, v103);
  v105 = v44->module->begin_call;
  v106 = v44->module;
  v107 = v47;
  v108 = EMITC_CALL_INDIRECT(v105, v106, v1, v107);
  v109 = (bool) v108;
  if (v109) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v110 = iree_ok_status();
  return v110;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  v111 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v111;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  return v108;
}
static iree_status_t module_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3) {
  iree_host_size_t v4;
  iree_host_size_t v5;
  iree_host_size_t v6;
  iree_host_size_t v7;
  iree_vm_function_t* v8;
  iree_vm_module_t* v9;
  bool v10;
  iree_vm_function_call_t v11;
  iree_vm_function_t v12;
  void* v13;
  uint8_t* v14;
  void* v15;
  uint8_t* v16;
  begin_call_t v17;
  iree_vm_module_t* v18;
  iree_vm_function_call_t v19;
  iree_status_t v20;
  bool v21;
  uint8_t* v22;
  iree_host_size_t v23;
  iree_status_t v24;
  iree_status_t v25;
  v4 = 1;
  v5 = 0;
  v6 = sizeof(int32_t);
  v7 = v5 + v6;
  ;
  v8 = v2;
  v9 = v8->module;
  v10 = !v9;
  if (v10) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v12 = *v2;
  v11.function = v12;
  v13 = iree_alloca(v4);
  v14 = (uint8_t*) v13;
  v11.arguments.data_length = v5;
  v11.arguments.data = v14;
  memset(v14, 0, v4);
  v15 = iree_alloca(v7);
  v16 = (uint8_t*) v15;
  v11.results.data_length = v7;
  v11.results.data = v16;
  memset(v16, 0, v7);
  v17 = v8->module->begin_call;
  v18 = v8->module;
  v19 = v11;
  v20 = EMITC_CALL_INDIRECT(v17, v18, v1, v19);
  v21 = (bool) v20;
  if (v21) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v22 = v11.results.data;
  v23 = sizeof(int32_t);
  memcpy(v3, v22, v23);
  v24 = iree_ok_status();
  return v24;
label4:
  v25 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v25;
label5:
  return v20;
}
static iree_status_t module_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4) {
  iree_host_size_t v5;
  iree_host_size_t v6;
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_vm_function_t* v10;
  iree_vm_module_t* v11;
  bool v12;
  iree_vm_function_call_t v13;
  iree_vm_function_t v14;
  void* v15;
  uint8_t* v16;
  void* v17;
  uint8_t* v18;
  uint8_t* v19;
  int32_t v20;
  iree_host_size_t v21;
  int32_t* v22;
  begin_call_t v23;
  iree_vm_module_t* v24;
  iree_vm_function_call_t v25;
  iree_status_t v26;
  bool v27;
  uint8_t* v28;
  iree_vm_ref_t* v29;
  iree_status_t v30;
  iree_status_t v31;
  v5 = 0;
  v6 = sizeof(int32_t);
  v7 = v5 + v6;
  v8 = sizeof(iree_vm_ref_t);
  v9 = v5 + v8;
  ;
  v10 = v2;
  v11 = v10->module;
  v12 = !v11;
  if (v12) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v14 = *v2;
  v13.function = v14;
  v15 = iree_alloca(v7);
  v16 = (uint8_t*) v15;
  v13.arguments.data_length = v7;
  v13.arguments.data = v16;
  memset(v16, 0, v7);
  v17 = iree_alloca(v9);
  v18 = (uint8_t*) v17;
  v13.results.data_length = v9;
  v13.results.data = v18;
  memset(v18, 0, v9);
  v19 = v13.arguments.data;
  ;
  v20 = v3;
  v21 = sizeof(int32_t);
  v22 = &v20;
  memcpy(v19, v22, v21);
  v23 = v10->module->begin_call;
  v24 = v10->module;
  v25 = v13;
  v26 = EMITC_CALL_INDIRECT(v23, v24, v1, v25);
  v27 = (bool) v26;
  if (v27) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v28 = v13.results.data;
  v29 = (iree_vm_ref_t*) v28;
  iree_vm_ref_move(v29, v4);
  v30 = iree_ok_status();
  return v30;
label4:
  v31 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v31;
label5:
  return v26;
}
static iree_status_t module_call_0rIrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, iree_vm_ref_t* v8) {
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_host_size_t v19;
  iree_host_size_t v20;
  iree_host_size_t v21;
  iree_vm_function_t* v22;
  iree_vm_module_t* v23;
  bool v24;
  iree_vm_function_call_t v25;
  iree_vm_function_t v26;
  void* v27;
  uint8_t* v28;
  void* v29;
  uint8_t* v30;
  uint8_t* v31;
  iree_vm_ref_t* v32;
  iree_host_size_t v33;
  uint8_t* v34;
  int64_t v35;
  iree_host_size_t v36;
  int64_t* v37;
  iree_host_size_t v38;
  uint8_t* v39;
  iree_vm_ref_t* v40;
  iree_host_size_t v41;
  uint8_t* v42;
  iree_vm_ref_t* v43;
  iree_host_size_t v44;
  uint8_t* v45;
  iree_vm_ref_t* v46;
  begin_call_t v47;
  iree_vm_module_t* v48;
  iree_vm_function_call_t v49;
  iree_status_t v50;
  bool v51;
  uint8_t* v52;
  iree_vm_ref_t* v53;
  iree_status_t v54;
  iree_status_t v55;
  v9 = 0;
  v10 = sizeof(iree_vm_ref_t);
  v11 = v9 + v10;
  v12 = sizeof(int64_t);
  v13 = v11 + v12;
  v14 = sizeof(iree_vm_ref_t);
  v15 = v13 + v14;
  v16 = sizeof(iree_vm_ref_t);
  v17 = v15 + v16;
  v18 = sizeof(iree_vm_ref_t);
  v19 = v17 + v18;
  v20 = sizeof(iree_vm_ref_t);
  v21 = v9 + v20;
  ;
  v22 = v2;
  v23 = v22->module;
  v24 = !v23;
  if (v24) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v26 = *v2;
  v25.function = v26;
  v27 = iree_alloca(v19);
  v28 = (uint8_t*) v27;
  v25.arguments.data_length = v19;
  v25.arguments.data = v28;
  memset(v28, 0, v19);
  v29 = iree_alloca(v21);
  v30 = (uint8_t*) v29;
  v25.results.data_length = v21;
  v25.results.data = v30;
  memset(v30, 0, v21);
  v31 = v25.arguments.data;
  v32 = (iree_vm_ref_t*) v31;
  iree_vm_ref_assign(v3, v32);
  v33 = sizeof(iree_vm_ref_t);
  v34 = v31 + v33;
  ;
  v35 = v4;
  v36 = sizeof(int64_t);
  v37 = &v35;
  memcpy(v34, v37, v36);
  v38 = sizeof(int64_t);
  v39 = v34 + v38;
  v40 = (iree_vm_ref_t*) v39;
  iree_vm_ref_assign(v5, v40);
  v41 = sizeof(iree_vm_ref_t);
  v42 = v39 + v41;
  v43 = (iree_vm_ref_t*) v42;
  iree_vm_ref_assign(v6, v43);
  v44 = sizeof(iree_vm_ref_t);
  v45 = v42 + v44;
  v46 = (iree_vm_ref_t*) v45;
  iree_vm_ref_assign(v7, v46);
  v47 = v22->module->begin_call;
  v48 = v22->module;
  v49 = v25;
  v50 = EMITC_CALL_INDIRECT(v47, v48, v1, v49);
  v51 = (bool) v50;
  if (v51) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v52 = v25.results.data;
  v53 = (iree_vm_ref_t*) v52;
  iree_vm_ref_move(v53, v8);
  v54 = iree_ok_status();
  return v54;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  v55 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v55;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  return v50;
}
static iree_status_t module_call_0rI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5) {
  iree_host_size_t v6;
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_vm_function_t* v13;
  iree_vm_module_t* v14;
  bool v15;
  iree_vm_function_call_t v16;
  iree_vm_function_t v17;
  void* v18;
  uint8_t* v19;
  void* v20;
  uint8_t* v21;
  uint8_t* v22;
  iree_vm_ref_t* v23;
  iree_host_size_t v24;
  uint8_t* v25;
  int64_t v26;
  iree_host_size_t v27;
  int64_t* v28;
  begin_call_t v29;
  iree_vm_module_t* v30;
  iree_vm_function_call_t v31;
  iree_status_t v32;
  bool v33;
  uint8_t* v34;
  iree_vm_ref_t* v35;
  iree_status_t v36;
  iree_status_t v37;
  v6 = 0;
  v7 = sizeof(iree_vm_ref_t);
  v8 = v6 + v7;
  v9 = sizeof(int64_t);
  v10 = v8 + v9;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v6 + v11;
  ;
  v13 = v2;
  v14 = v13->module;
  v15 = !v14;
  if (v15) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v17 = *v2;
  v16.function = v17;
  v18 = iree_alloca(v10);
  v19 = (uint8_t*) v18;
  v16.arguments.data_length = v10;
  v16.arguments.data = v19;
  memset(v19, 0, v10);
  v20 = iree_alloca(v12);
  v21 = (uint8_t*) v20;
  v16.results.data_length = v12;
  v16.results.data = v21;
  memset(v21, 0, v12);
  v22 = v16.arguments.data;
  v23 = (iree_vm_ref_t*) v22;
  iree_vm_ref_assign(v3, v23);
  v24 = sizeof(iree_vm_ref_t);
  v25 = v22 + v24;
  ;
  v26 = v4;
  v27 = sizeof(int64_t);
  v28 = &v26;
  memcpy(v25, v28, v27);
  v29 = v13->module->begin_call;
  v30 = v13->module;
  v31 = v16;
  v32 = EMITC_CALL_INDIRECT(v29, v30, v1, v31);
  v33 = (bool) v32;
  if (v33) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v34 = v16.results.data;
  v35 = (iree_vm_ref_t*) v34;
  iree_vm_ref_move(v35, v5);
  v36 = iree_ok_status();
  return v36;
label4:
  iree_vm_ref_release(v3);
  v37 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v37;
label5:
  iree_vm_ref_release(v3);
  return v32;
}
static iree_status_t module_call_0iICrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int32_t* v7) {
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_host_size_t v16;
  iree_host_size_t v17;
  iree_host_size_t v18;
  iree_vm_function_t* v19;
  iree_vm_module_t* v20;
  bool v21;
  iree_vm_function_call_t v22;
  iree_vm_function_t v23;
  void* v24;
  uint8_t* v25;
  void* v26;
  uint8_t* v27;
  uint8_t* v28;
  int32_t v29;
  iree_host_size_t v30;
  int32_t* v31;
  iree_host_size_t v32;
  uint8_t* v33;
  int64_t v34;
  iree_host_size_t v35;
  int64_t* v36;
  iree_host_size_t v37;
  uint8_t* v38;
  int32_t v39;
  iree_host_size_t v40;
  int32_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  iree_vm_ref_t* v44;
  begin_call_t v45;
  iree_vm_module_t* v46;
  iree_vm_function_call_t v47;
  iree_status_t v48;
  bool v49;
  uint8_t* v50;
  iree_host_size_t v51;
  iree_status_t v52;
  iree_status_t v53;
  v8 = 0;
  v9 = sizeof(int32_t);
  v10 = v8 + v9;
  v11 = sizeof(int64_t);
  v12 = v10 + v11;
  v13 = sizeof(int32_t);
  v14 = v12 + v13;
  v15 = sizeof(iree_vm_ref_t);
  v16 = v14 + v15;
  v17 = sizeof(int32_t);
  v18 = v8 + v17;
  ;
  v19 = v2;
  v20 = v19->module;
  v21 = !v20;
  if (v21) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v23 = *v2;
  v22.function = v23;
  v24 = iree_alloca(v16);
  v25 = (uint8_t*) v24;
  v22.arguments.data_length = v16;
  v22.arguments.data = v25;
  memset(v25, 0, v16);
  v26 = iree_alloca(v18);
  v27 = (uint8_t*) v26;
  v22.results.data_length = v18;
  v22.results.data = v27;
  memset(v27, 0, v18);
  v28 = v22.arguments.data;
  ;
  v29 = v3;
  v30 = sizeof(int32_t);
  v31 = &v29;
  memcpy(v28, v31, v30);
  v32 = sizeof(int32_t);
  v33 = v28 + v32;
  ;
  v34 = v4;
  v35 = sizeof(int64_t);
  v36 = &v34;
  memcpy(v33, v36, v35);
  v37 = sizeof(int64_t);
  v38 = v33 + v37;
  ;
  v39 = v5;
  v40 = sizeof(int32_t);
  v41 = &v39;
  memcpy(v38, v41, v40);
  v42 = sizeof(int32_t);
  v43 = v38 + v42;
  v44 = (iree_vm_ref_t*) v43;
  iree_vm_ref_assign(v6, v44);
  v45 = v19->module->begin_call;
  v46 = v19->module;
  v47 = v22;
  v48 = EMITC_CALL_INDIRECT(v45, v46, v1, v47);
  v49 = (bool) v48;
  if (v49) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v50 = v22.results.data;
  v51 = sizeof(int32_t);
  memcpy(v7, v50, v51);
  v52 = iree_ok_status();
  return v52;
label4:
  iree_vm_ref_release(v6);
  v53 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v53;
label5:
  iree_vm_ref_release(v6);
  return v48;
}
static iree_status_t module_main(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5) {
  int32_t v6;
  int32_t v7;
  int64_t v8;
  int64_t v9;
  int64_t v10;
  int64_t v11;
  int32_t v12;
  int32_t v13;
  int32_t v14;
  int32_t v15;
  int32_t v16;
  iree_vm_ref_t v17;
  iree_vm_ref_t* v18;
  iree_host_size_t v19;
  iree_vm_ref_t v20;
  iree_vm_ref_t* v21;
  iree_host_size_t v22;
  iree_vm_ref_t v23;
  iree_vm_ref_t* v24;
  iree_host_size_t v25;
  iree_vm_ref_t v26;
  iree_vm_ref_t* v27;
  iree_host_size_t v28;
  iree_vm_ref_t v29;
  iree_vm_ref_t* v30;
  iree_host_size_t v31;
  struct module_state_t* v32;
  iree_vm_ref_t* v33;
  iree_vm_ref_t* v34;
  struct module_t* v35;
  iree_vm_ref_type_t* v36;
  iree_vm_ref_type_t v37;
  iree_vm_type_def_t v38;
  iree_vm_ref_type_t v39;
  iree_status_t v40;
  bool v41;
  struct module_state_t* v42;
  iree_vm_ref_t* v43;
  iree_vm_ref_t* v44;
  struct module_t* v45;
  iree_vm_ref_type_t* v46;
  iree_vm_ref_type_t v47;
  iree_vm_type_def_t v48;
  iree_vm_ref_type_t v49;
  iree_status_t v50;
  bool v51;
  struct module_state_t* v52;
  iree_vm_buffer_t* v53;
  iree_vm_buffer_t* v54;
  iree_vm_ref_type_t v55;
  iree_status_t v56;
  bool v57;
  struct module_state_t* v58;
  iree_vm_function_t* v59;
  iree_vm_function_t* v60;
  iree_vm_ref_t v61;
  iree_vm_ref_t* v62;
  iree_host_size_t v63;
  iree_vm_ref_t v64;
  iree_vm_ref_t* v65;
  iree_host_size_t v66;
  iree_status_t v67;
  bool v68;
  struct module_state_t* v69;
  iree_vm_function_t* v70;
  iree_vm_function_t* v71;
  iree_vm_ref_t v72;
  iree_vm_ref_t* v73;
  iree_host_size_t v74;
  iree_status_t v75;
  bool v76;
  struct module_state_t* v77;
  iree_vm_function_t* v78;
  iree_vm_function_t* v79;
  iree_vm_ref_t v80;
  iree_vm_ref_t* v81;
  iree_host_size_t v82;
  iree_status_t v83;
  bool v84;
  struct module_state_t* v85;
  iree_vm_buffer_t* v86;
  iree_vm_buffer_t* v87;
  iree_vm_ref_type_t v88;
  iree_status_t v89;
  bool v90;
  struct module_state_t* v91;
  iree_vm_function_t* v92;
  iree_vm_function_t* v93;
  iree_vm_ref_t v94;
  iree_vm_ref_t* v95;
  iree_host_size_t v96;
  iree_vm_ref_t v97;
  iree_vm_ref_t* v98;
  iree_host_size_t v99;
  iree_vm_ref_t v100;
  iree_vm_ref_t* v101;
  iree_host_size_t v102;
  iree_status_t v103;
  bool v104;
  struct module_state_t* v105;
  iree_vm_function_t* v106;
  iree_vm_function_t* v107;
  iree_vm_ref_t v108;
  iree_vm_ref_t* v109;
  iree_host_size_t v110;
  iree_status_t v111;
  bool v112;
  struct module_state_t* v113;
  iree_vm_function_t* v114;
  iree_vm_function_t* v115;
  iree_vm_ref_t v116;
  iree_vm_ref_t* v117;
  iree_host_size_t v118;
  iree_vm_ref_t v119;
  iree_vm_ref_t* v120;
  iree_host_size_t v121;
  iree_vm_ref_t v122;
  iree_vm_ref_t* v123;
  iree_host_size_t v124;
  iree_status_t v125;
  bool v126;
  struct module_state_t* v127;
  iree_vm_function_t* v128;
  iree_vm_function_t* v129;
  iree_vm_ref_t v130;
  iree_vm_ref_t* v131;
  iree_host_size_t v132;
  iree_status_t v133;
  bool v134;
  struct module_state_t* v135;
  iree_vm_function_t* v136;
  iree_vm_function_t* v137;
  iree_vm_ref_t v138;
  iree_vm_ref_t* v139;
  iree_host_size_t v140;
  iree_vm_ref_t v141;
  iree_vm_ref_t* v142;
  iree_host_size_t v143;
  iree_vm_ref_t v144;
  iree_vm_ref_t* v145;
  iree_host_size_t v146;
  iree_vm_ref_t v147;
  iree_vm_ref_t* v148;
  iree_host_size_t v149;
  iree_vm_ref_t v150;
  iree_vm_ref_t* v151;
  iree_host_size_t v152;
  iree_vm_ref_t v153;
  iree_vm_ref_t* v154;
  iree_host_size_t v155;
  iree_status_t v156;
  bool v157;
  struct module_state_t* v158;
  iree_vm_function_t* v159;
  iree_vm_function_t* v160;
  iree_vm_ref_t v161;
  iree_vm_ref_t* v162;
  iree_host_size_t v163;
  int32_t v164;
  int32_t* v165;
  iree_status_t v166;
  bool v167;
  int32_t v168;
  int32_t v169;
  bool v170;
  struct module_state_t* v171;
  iree_vm_function_t* v172;
  iree_vm_function_t* v173;
  iree_vm_ref_t v174;
  iree_vm_ref_t* v175;
  iree_host_size_t v176;
  iree_status_t v177;
  bool v178;
  iree_vm_ref_t v179;
  iree_vm_ref_t* v180;
  iree_host_size_t v181;
  iree_status_t v182;
  bool v183;
  iree_status_t v184;
  iree_string_view_t v185;
  iree_string_view_t v186;
  iree_host_size_t v187;
  int v188;
  const char* v189;
  iree_status_t v190;
  v6 = 2;
  v7 = -1;
  v8 = -1;
  v9 = 0;
  v10 = 16;
  v11 = 4;
  v12 = 48;
  v13 = 3075;
  v14 = 553648160;
  v15 = 1;
  v16 = 16;
  ;
  v18 = &v17;
  v19 = sizeof(iree_vm_ref_t);
  memset(v18, 0, v19);
  ;
  v21 = &v20;
  v22 = sizeof(iree_vm_ref_t);
  memset(v21, 0, v22);
  ;
  v24 = &v23;
  v25 = sizeof(iree_vm_ref_t);
  memset(v24, 0, v25);
  ;
  v27 = &v26;
  v28 = sizeof(iree_vm_ref_t);
  memset(v27, 0, v28);
  ;
  v30 = &v29;
  v31 = sizeof(iree_vm_ref_t);
  memset(v30, 0, v31);
  iree_vm_ref_release(v5);
  ;
  v32 = v3;
  v33 = v32->refs;
  v34 = &v33[0];
  ;
  v35 = v2;
  v36 = v35->types;
  v37 = v36[6];
  v38 = iree_vm_make_ref_type_def(v37);
  v39 = iree_vm_type_def_as_ref(v38);
  v40 = iree_vm_ref_retain_or_move_checked(false, v34, v39, v21);
  v41 = (bool) v40;
  if (v41) {
    goto label18;
  } else {
    goto label2;
  }
label2:
  ;
  v42 = v3;
  v43 = v42->refs;
  v44 = &v43[2];
  ;
  v45 = v2;
  v46 = v45->types;
  v47 = v46[5];
  v48 = iree_vm_make_ref_type_def(v47);
  v49 = iree_vm_type_def_as_ref(v48);
  v50 = iree_vm_ref_retain_or_move_checked(false, v44, v49, v24);
  v51 = (bool) v50;
  if (v51) {
    goto label19;
  } else {
    goto label3;
  }
label3:
  ;
  v52 = v3;
  v53 = v52->rodata_buffers;
  v54 = &v53[5];
  v55 = iree_vm_buffer_type();
  v56 = iree_vm_ref_wrap_retain(v54, v55, v27);
  v57 = (bool) v56;
  if (v57) {
    goto label20;
  } else {
    goto label4;
  }
label4:
  ;
  v58 = v3;
  v59 = v58->imports;
  v60 = &v59[2];
  ;
  v62 = &v61;
  v63 = sizeof(iree_vm_ref_t);
  memset(v62, 0, v63);
  iree_vm_ref_assign(v4, v62);
  ;
  v65 = &v64;
  v66 = sizeof(iree_vm_ref_t);
  memset(v65, 0, v66);
  iree_vm_ref_assign(v27, v65);
  v67 = module_call_0rriiCID_v_1_import_shim(v1, v60, v62, v65, v14, v15, v15, v11);
  v68 = (bool) v67;
  if (v68) {
    goto label21;
  } else {
    goto label5;
  }
label5:
  ;
  v69 = v3;
  v70 = v69->imports;
  v71 = &v70[3];
  ;
  v73 = &v72;
  v74 = sizeof(iree_vm_ref_t);
  memset(v73, 0, v74);
  iree_vm_ref_assign(v4, v73);
  v75 = module_call_0r_r_import_shim(v1, v71, v73, v4);
  v76 = (bool) v75;
  if (v76) {
    goto label22;
  } else {
    goto label6;
  }
label6:
  ;
  v77 = v3;
  v78 = v77->imports;
  v79 = &v78[8];
  ;
  v81 = &v80;
  v82 = sizeof(iree_vm_ref_t);
  memset(v81, 0, v82);
  iree_vm_ref_assign(v21, v81);
  v83 = module_call_0r_r_import_shim(v1, v79, v81, v27);
  v84 = (bool) v83;
  if (v84) {
    goto label23;
  } else {
    goto label7;
  }
label7:
  ;
  v85 = v3;
  v86 = v85->rodata_buffers;
  v87 = &v86[6];
  v88 = iree_vm_buffer_type();
  v89 = iree_vm_ref_wrap_retain(v87, v88, v30);
  v90 = (bool) v89;
  if (v90) {
    goto label24;
  } else {
    goto label8;
  }
label8:
  ;
  v91 = v3;
  v92 = v91->imports;
  v93 = &v92[0];
  ;
  v95 = &v94;
  v96 = sizeof(iree_vm_ref_t);
  memset(v95, 0, v96);
  iree_vm_ref_assign(v4, v95);
  ;
  v98 = &v97;
  v99 = sizeof(iree_vm_ref_t);
  memset(v98, 0, v99);
  iree_vm_ref_assign(v30, v98);
  ;
  v101 = &v100;
  v102 = sizeof(iree_vm_ref_t);
  memset(v101, 0, v102);
  iree_vm_ref_assign(v27, v101);
  v103 = module_call_0rrrIii_v_import_shim(v1, v93, v95, v98, v101, v10, v16, v13);
  v104 = (bool) v103;
  if (v104) {
    goto label25;
  } else {
    goto label9;
  }
label9:
  ;
  v105 = v3;
  v106 = v105->imports;
  v107 = &v106[15];
  ;
  v109 = &v108;
  v110 = sizeof(iree_vm_ref_t);
  memset(v109, 0, v110);
  iree_vm_ref_assign(v21, v109);
  v111 = module_call_0rI_r_import_shim(v1, v107, v109, v9, v27);
  v112 = (bool) v111;
  if (v112) {
    goto label26;
  } else {
    goto label10;
  }
label10:
  ;
  v113 = v3;
  v114 = v113->imports;
  v115 = &v114[10];
  ;
  v117 = &v116;
  v118 = sizeof(iree_vm_ref_t);
  memset(v117, 0, v118);
  iree_vm_ref_assign(v21, v117);
  ;
  v120 = &v119;
  v121 = sizeof(iree_vm_ref_t);
  memset(v120, 0, v121);
  iree_vm_ref_assign(v5, v120);
  ;
  v123 = &v122;
  v124 = sizeof(iree_vm_ref_t);
  memset(v123, 0, v124);
  iree_vm_ref_assign(v27, v123);
  v125 = module_call_0rIrrIiiII_r_import_shim(v1, v115, v117, v8, v120, v123, v9, v12, v13, v10, v9, v5);
  v126 = (bool) v125;
  if (v126) {
    goto label27;
  } else {
    goto label11;
  }
label11:
  ;
  v127 = v3;
  v128 = v127->imports;
  v129 = &v128[15];
  ;
  v131 = &v130;
  v132 = sizeof(iree_vm_ref_t);
  memset(v131, 0, v132);
  iree_vm_ref_assign(v21, v131);
  v133 = module_call_0rI_r_import_shim(v1, v129, v131, v9, v30);
  v134 = (bool) v133;
  if (v134) {
    goto label28;
  } else {
    goto label12;
  }
label12:
  ;
  v135 = v3;
  v136 = v135->imports;
  v137 = &v136[11];
  ;
  v139 = &v138;
  v140 = sizeof(iree_vm_ref_t);
  memset(v139, 0, v140);
  iree_vm_ref_assign(v21, v139);
  ;
  v142 = &v141;
  v143 = sizeof(iree_vm_ref_t);
  memset(v142, 0, v143);
  iree_vm_ref_assign(v27, v142);
  ;
  v145 = &v144;
  v146 = sizeof(iree_vm_ref_t);
  memset(v145, 0, v146);
  iree_vm_ref_assign(v30, v145);
  ;
  v148 = &v147;
  v149 = sizeof(iree_vm_ref_t);
  memset(v148, 0, v149);
  iree_vm_ref_assign(v24, v148);
  ;
  v151 = &v150;
  v152 = sizeof(iree_vm_ref_t);
  memset(v151, 0, v152);
  iree_vm_ref_assign(v4, v151);
  ;
  v154 = &v153;
  v155 = sizeof(iree_vm_ref_t);
  memset(v154, 0, v155);
  iree_vm_ref_assign(v5, v154);
  v156 = module_call_0rIrrrICrIID_v_2_import_shim(v1, v137, v139, v8, v142, v145, v148, v9, v6, v151, v9, v10, v154, v9, v10);
  v157 = (bool) v156;
  if (v157) {
    goto label29;
  } else {
    goto label13;
  }
label13:
  ;
  v158 = v3;
  v159 = v158->imports;
  v160 = &v159[16];
  ;
  v162 = &v161;
  v163 = sizeof(iree_vm_ref_t);
  memset(v162, 0, v163);
  iree_vm_ref_assign(v30, v162);
  ;
  v165 = &v164;
  v166 = module_call_0iICrD_i_1_import_shim(v1, v160, v7, v9, v15, v162, v165);
  v167 = (bool) v166;
  if (v167) {
    goto label30;
  } else {
    goto label14;
  }
label14:
  v168 = v164;
  v169 = vm_cmp_nz_i32(v168);
  v170 = (bool) v169;
  if (v170) {
    goto label17;
  } else {
    goto label15;
  }
label15:
  ;
  v171 = v3;
  v172 = v171->imports;
  v173 = &v172[1];
  ;
  v175 = &v174;
  v176 = sizeof(iree_vm_ref_t);
  memset(v175, 0, v176);
  iree_vm_ref_assign(v5, v175);
  v177 = module_call_0rIIiiCID_r_1_import_shim(v1, v173, v175, v9, v10, v14, v15, v15, v11, v4);
  v178 = (bool) v177;
  if (v178) {
    goto label33;
  } else {
    goto label16;
  }
label16:
  ;
  v180 = &v179;
  v181 = sizeof(iree_vm_ref_t);
  memset(v180, 0, v181);
  iree_vm_ref_move(v4, v180);
  iree_vm_ref_move(v180, v5);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  v182 = iree_ok_status();
  return v182;
label17:
  v183 = (bool) v168;
  if (v183) {
    goto label32;
  } else {
    goto label31;
  }
label18:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v40;
label19:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v50;
label20:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v56;
label21:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v67;
label22:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v75;
label23:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v83;
label24:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v89;
label25:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v103;
label26:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v111;
label27:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v125;
label28:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v133;
label29:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v156;
label30:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v166;
label31:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  v184 = iree_ok_status();
  return v184;
label32:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  v185 = iree_make_cstring_view("failed to wait on timepoint");
  ;
  v186 = v185;
  v187 = v186.size;
  v188 = (int) v187;
  v189 = v186.data;
  v190 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v188, v189);
  return v190;
label33:
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v4);
  return v177;
}
struct module_main_args_t {iree_vm_ref_t arg0;};
struct module_main_result_t {iree_vm_ref_t res0;};
static iree_status_t module_main_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct module_t* v7;
  struct module_state_t* v8;
  iree_byte_span_t v9;
  uint8_t* v10;
  struct module_main_args_t* v11;
  struct module_main_args_t* v12;
  iree_byte_span_t v13;
  uint8_t* v14;
  struct module_main_result_t* v15;
  struct module_main_result_t* v16;
  iree_vm_ref_t* v17;
  iree_vm_ref_t* v18;
  iree_status_t v19;
  bool v20;
  iree_status_t v21;
  v7 = (struct module_t*) v5;
  v8 = (struct module_state_t*) v6;
  ;
  v9 = v3;
  v10 = v9.data;
  v11 = (struct module_main_args_t*) v10;
  ;
  v12 = v11;
  ;
  v13 = v4;
  v14 = v13.data;
  v15 = (struct module_main_result_t*) v14;
  ;
  v16 = v15;
  v17 = &v12->arg0;
  iree_vm_ref_retain_inplace(v17);
  v18 = &v16->res0;
  v19 = module_main(v1, v7, v8, v17, v18);
  v20 = (bool) v19;
  if (v20) {
    goto label3;
  } else {
    goto label2;
  }
label2:
  v21 = iree_ok_status();
  return v21;
label3:
  return v19;
}
static iree_status_t module___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct module_t* v7;
  struct module_state_t* v8;
  iree_status_t v9;
  bool v10;
  iree_status_t v11;
  v7 = (struct module_t*) v5;
  v8 = (struct module_state_t*) v6;
  v9 = module___init(v1, v7, v8);
  v10 = (bool) v9;
  if (v10) {
    goto label3;
  } else {
    goto label2;
  }
label2:
  v11 = iree_ok_status();
  return v11;
label3:
  return v9;
}
static iree_status_t module___init(iree_vm_stack_t* v1, struct module_t* v2, struct module_state_t* v3) {
  int64_t v4;
  int64_t v5;
  int32_t v6;
  int32_t v7;
  int64_t v8;
  int32_t v9;
  int32_t v10;
  iree_vm_ref_t v11;
  iree_vm_ref_t* v12;
  iree_host_size_t v13;
  iree_vm_ref_t v14;
  iree_vm_ref_t* v15;
  iree_host_size_t v16;
  iree_vm_ref_t v17;
  iree_vm_ref_t* v18;
  iree_host_size_t v19;
  iree_vm_ref_t v20;
  iree_vm_ref_t* v21;
  iree_host_size_t v22;
  iree_vm_ref_t v23;
  iree_vm_ref_t* v24;
  iree_host_size_t v25;
  struct module_state_t* v26;
  iree_vm_function_t* v27;
  iree_vm_function_t* v28;
  int32_t v29;
  int32_t* v30;
  iree_status_t v31;
  bool v32;
  int32_t v33;
  int64_t v34;
  iree_vm_ref_t v35;
  iree_vm_ref_t* v36;
  iree_host_size_t v37;
  int32_t v38;
  int32_t v39;
  int32_t v40;
  int32_t v41;
  int32_t v42;
  bool v43;
  int32_t v44;
  struct module_state_t* v45;
  iree_vm_function_t* v46;
  iree_vm_function_t* v47;
  iree_status_t v48;
  bool v49;
  struct module_state_t* v50;
  iree_vm_buffer_t* v51;
  iree_vm_buffer_t* v52;
  iree_vm_ref_type_t v53;
  iree_status_t v54;
  bool v55;
  struct module_state_t* v56;
  iree_vm_buffer_t* v57;
  iree_vm_buffer_t* v58;
  iree_vm_ref_type_t v59;
  iree_status_t v60;
  bool v61;
  struct module_state_t* v62;
  iree_vm_function_t* v63;
  iree_vm_function_t* v64;
  iree_vm_ref_t v65;
  iree_vm_ref_t* v66;
  iree_host_size_t v67;
  iree_vm_ref_t v68;
  iree_vm_ref_t* v69;
  iree_host_size_t v70;
  iree_vm_ref_t v71;
  iree_vm_ref_t* v72;
  iree_host_size_t v73;
  int32_t v74;
  int32_t* v75;
  int64_t v76;
  int64_t* v77;
  iree_status_t v78;
  bool v79;
  int32_t v80;
  int64_t v81;
  int32_t v82;
  int32_t v83;
  int32_t v84;
  bool v85;
  struct module_state_t* v86;
  iree_vm_buffer_t* v87;
  iree_vm_buffer_t* v88;
  iree_vm_ref_type_t v89;
  iree_status_t v90;
  bool v91;
  struct module_state_t* v92;
  iree_vm_buffer_t* v93;
  iree_vm_buffer_t* v94;
  iree_vm_ref_type_t v95;
  iree_status_t v96;
  bool v97;
  struct module_state_t* v98;
  iree_vm_function_t* v99;
  iree_vm_function_t* v100;
  iree_vm_ref_t v101;
  iree_vm_ref_t* v102;
  iree_host_size_t v103;
  iree_vm_ref_t v104;
  iree_vm_ref_t* v105;
  iree_host_size_t v106;
  iree_vm_ref_t v107;
  iree_vm_ref_t* v108;
  iree_host_size_t v109;
  int32_t v110;
  int32_t* v111;
  int64_t v112;
  int64_t* v113;
  iree_status_t v114;
  bool v115;
  int32_t v116;
  int64_t v117;
  int32_t v118;
  int32_t v119;
  int32_t v120;
  int64_t v121;
  int64_t v122;
  int32_t v123;
  struct module_t* v124;
  iree_vm_ref_type_t* v125;
  iree_vm_ref_type_t v126;
  iree_vm_type_def_t v127;
  iree_vm_ref_type_t v128;
  int32_t v129;
  bool v130;
  iree_status_t v131;
  bool v132;
  iree_status_t v133;
  bool v134;
  int64_t v135;
  iree_vm_ref_t v136;
  iree_vm_ref_t* v137;
  iree_host_size_t v138;
  int32_t v139;
  bool v140;
  bool v141;
  struct module_state_t* v142;
  iree_vm_buffer_t* v143;
  iree_vm_buffer_t* v144;
  iree_vm_ref_type_t v145;
  iree_status_t v146;
  bool v147;
  struct module_state_t* v148;
  iree_vm_buffer_t* v149;
  iree_vm_buffer_t* v150;
  iree_vm_ref_type_t v151;
  iree_status_t v152;
  bool v153;
  struct module_state_t* v154;
  iree_vm_function_t* v155;
  iree_vm_function_t* v156;
  iree_vm_ref_t v157;
  iree_vm_ref_t* v158;
  iree_host_size_t v159;
  iree_vm_ref_t v160;
  iree_vm_ref_t* v161;
  iree_host_size_t v162;
  iree_vm_ref_t v163;
  iree_vm_ref_t* v164;
  iree_host_size_t v165;
  int32_t v166;
  int32_t* v167;
  int64_t v168;
  int64_t* v169;
  iree_status_t v170;
  bool v171;
  int32_t v172;
  int64_t v173;
  int32_t v174;
  int32_t v175;
  int64_t v176;
  int32_t v177;
  struct module_state_t* v178;
  iree_vm_ref_t* v179;
  iree_vm_ref_t* v180;
  struct module_t* v181;
  iree_vm_ref_type_t* v182;
  iree_vm_ref_type_t v183;
  iree_vm_type_def_t v184;
  iree_vm_ref_type_t v185;
  iree_status_t v186;
  bool v187;
  int32_t v188;
  bool v189;
  struct module_state_t* v190;
  iree_vm_buffer_t* v191;
  iree_vm_buffer_t* v192;
  iree_vm_ref_type_t v193;
  iree_status_t v194;
  bool v195;
  struct module_state_t* v196;
  iree_vm_function_t* v197;
  iree_vm_function_t* v198;
  iree_vm_ref_t v199;
  iree_vm_ref_t* v200;
  iree_host_size_t v201;
  iree_vm_ref_t v202;
  iree_vm_ref_t* v203;
  iree_host_size_t v204;
  iree_vm_ref_t v205;
  iree_vm_ref_t* v206;
  iree_host_size_t v207;
  iree_vm_ref_t v208;
  iree_vm_ref_t* v209;
  iree_host_size_t v210;
  iree_status_t v211;
  bool v212;
  struct module_state_t* v213;
  iree_vm_ref_t* v214;
  iree_vm_ref_t* v215;
  struct module_t* v216;
  iree_vm_ref_type_t* v217;
  iree_vm_ref_type_t v218;
  iree_vm_type_def_t v219;
  iree_vm_ref_type_t v220;
  iree_status_t v221;
  bool v222;
  iree_status_t v223;
  bool v224;
  struct module_state_t* v225;
  iree_vm_ref_t* v226;
  iree_vm_ref_t* v227;
  struct module_t* v228;
  iree_vm_ref_type_t* v229;
  iree_vm_ref_type_t v230;
  iree_vm_type_def_t v231;
  iree_vm_ref_type_t v232;
  iree_status_t v233;
  bool v234;
  iree_status_t v235;
  bool v236;
  iree_status_t v237;
  iree_string_view_t v238;
  iree_string_view_t v239;
  iree_host_size_t v240;
  int v241;
  const char* v242;
  iree_status_t v243;
  iree_status_t v244;
  iree_string_view_t v245;
  iree_string_view_t v246;
  iree_host_size_t v247;
  int v248;
  const char* v249;
  iree_status_t v250;
  int64_t v251;
  int64_t v252;
  int32_t v253;
  v4 = 1;
  v5 = 0;
  v6 = 0;
  v7 = 18;
  v8 = -1;
  v9 = 14;
  v10 = 1;
  ;
  v12 = &v11;
  v13 = sizeof(iree_vm_ref_t);
  memset(v12, 0, v13);
  ;
  v15 = &v14;
  v16 = sizeof(iree_vm_ref_t);
  memset(v15, 0, v16);
  ;
  v18 = &v17;
  v19 = sizeof(iree_vm_ref_t);
  memset(v18, 0, v19);
  ;
  v21 = &v20;
  v22 = sizeof(iree_vm_ref_t);
  memset(v21, 0, v22);
  ;
  v24 = &v23;
  v25 = sizeof(iree_vm_ref_t);
  memset(v24, 0, v25);
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v15);
  ;
  v26 = v3;
  v27 = v26->imports;
  v28 = &v27[12];
  ;
  v30 = &v29;
  v31 = module_call_0v_i_import_shim(v1, v28, v30);
  v32 = (bool) v31;
  if (v32) {
    goto label31;
  } else {
    goto label2;
  }
label2:
  v33 = v29;
  v34 = vm_ext_i32i64s(v33);
  ;
  v36 = &v35;
  v37 = sizeof(iree_vm_ref_t);
  memset(v36, 0, v37);
  iree_vm_ref_retain(v15, v36);
  iree_vm_ref_assign(v36, v18);
  v251 = v5;
  v252 = v5;
  goto label3;
label3:
  v38 = vm_cmp_nz_ref(v18);
  v39 = vm_xor_i32(v38, v10);
  v40 = vm_cmp_lt_i64s(v251, v34);
  v41 = vm_and_i32(v39, v40);
  v42 = vm_cmp_nz_i32(v41);
  v43 = (bool) v42;
  if (v43) {
    goto label4;
  } else {
    goto label17;
  }
label4:
  v44 = vm_trunc_i64i32(v251);
  ;
  v45 = v3;
  v46 = v45->imports;
  v47 = &v46[13];
  v48 = module_call_0i_r_import_shim(v1, v47, v44, v18);
  v49 = (bool) v48;
  if (v49) {
    goto label32;
  } else {
    goto label5;
  }
label5:
  ;
  v50 = v3;
  v51 = v50->rodata_buffers;
  v52 = &v51[0];
  v53 = iree_vm_buffer_type();
  v54 = iree_vm_ref_wrap_retain(v52, v53, v21);
  v55 = (bool) v54;
  if (v55) {
    goto label33;
  } else {
    goto label6;
  }
label6:
  ;
  v56 = v3;
  v57 = v56->rodata_buffers;
  v58 = &v57[1];
  v59 = iree_vm_buffer_type();
  v60 = iree_vm_ref_wrap_retain(v58, v59, v24);
  v61 = (bool) v60;
  if (v61) {
    goto label34;
  } else {
    goto label7;
  }
label7:
  ;
  v62 = v3;
  v63 = v62->imports;
  v64 = &v63[9];
  ;
  v66 = &v65;
  v67 = sizeof(iree_vm_ref_t);
  memset(v66, 0, v67);
  iree_vm_ref_assign(v18, v66);
  ;
  v69 = &v68;
  v70 = sizeof(iree_vm_ref_t);
  memset(v69, 0, v70);
  iree_vm_ref_assign(v21, v69);
  ;
  v72 = &v71;
  v73 = sizeof(iree_vm_ref_t);
  memset(v72, 0, v73);
  iree_vm_ref_assign(v24, v72);
  ;
  v75 = &v74;
  ;
  v77 = &v76;
  v78 = module_call_0rrr_iI_import_shim(v1, v64, v66, v69, v72, v75, v77);
  v79 = (bool) v78;
  if (v79) {
    goto label35;
  } else {
    goto label8;
  }
label8:
  v80 = v74;
  v81 = v76;
  v82 = vm_cmp_nz_i64(v81);
  v83 = vm_select_i32(v80, v82, v6);
  v84 = vm_cmp_nz_i32(v83);
  v85 = (bool) v84;
  if (v85) {
    goto label9;
  } else {
    v253 = v6;
    goto label13;
  }
label9:
  ;
  v86 = v3;
  v87 = v86->rodata_buffers;
  v88 = &v87[2];
  v89 = iree_vm_buffer_type();
  v90 = iree_vm_ref_wrap_retain(v88, v89, v21);
  v91 = (bool) v90;
  if (v91) {
    goto label36;
  } else {
    goto label10;
  }
label10:
  ;
  v92 = v3;
  v93 = v92->rodata_buffers;
  v94 = &v93[3];
  v95 = iree_vm_buffer_type();
  v96 = iree_vm_ref_wrap_retain(v94, v95, v24);
  v97 = (bool) v96;
  if (v97) {
    goto label37;
  } else {
    goto label11;
  }
label11:
  ;
  v98 = v3;
  v99 = v98->imports;
  v100 = &v99[9];
  ;
  v102 = &v101;
  v103 = sizeof(iree_vm_ref_t);
  memset(v102, 0, v103);
  iree_vm_ref_assign(v18, v102);
  ;
  v105 = &v104;
  v106 = sizeof(iree_vm_ref_t);
  memset(v105, 0, v106);
  iree_vm_ref_assign(v21, v105);
  ;
  v108 = &v107;
  v109 = sizeof(iree_vm_ref_t);
  memset(v108, 0, v109);
  iree_vm_ref_assign(v24, v108);
  ;
  v111 = &v110;
  ;
  v113 = &v112;
  v114 = module_call_0rrr_iI_import_shim(v1, v100, v102, v105, v108, v111, v113);
  v115 = (bool) v114;
  if (v115) {
    goto label38;
  } else {
    goto label12;
  }
label12:
  v116 = v110;
  v117 = v112;
  v118 = vm_cmp_nz_i64(v117);
  v119 = vm_select_i32(v116, v118, v6);
  v253 = v119;
  goto label13;
label13:
  v120 = vm_cmp_eq_i64(v252, v5);
  v121 = vm_select_i64(v253, v4, v5);
  v122 = vm_add_i64(v252, v121);
  v123 = vm_and_i32(v253, v120);
  ;
  v124 = v2;
  v125 = v124->types;
  v126 = v125[6];
  v127 = iree_vm_make_ref_type_def(v126);
  v128 = iree_vm_type_def_as_ref(v127);
  v129 = vm_cmp_nz_i32(v123);
  v130 = (bool) v129;
  if (v130) {
    goto label14;
  } else {
    goto label15;
  }
label14:
  v131 = iree_vm_ref_retain_or_move_checked(false, v18, v128, v18);
  v132 = (bool) v131;
  if (v132) {
    goto label39;
  } else {
    goto label16;
  }
label15:
  v133 = iree_vm_ref_retain_or_move_checked(false, v15, v128, v18);
  v134 = (bool) v133;
  if (v134) {
    goto label40;
  } else {
    goto label16;
  }
label16:
  v135 = vm_add_i64(v251, v4);
  ;
  v137 = &v136;
  v138 = sizeof(iree_vm_ref_t);
  memset(v137, 0, v138);
  iree_vm_ref_retain(v18, v137);
  iree_vm_ref_assign(v137, v18);
  v251 = v135;
  v252 = v122;
  goto label3;
label17:
  v139 = vm_cmp_nz_i32(v39);
  v140 = (bool) v139;
  if (v140) {
    goto label18;
  } else {
    goto label19;
  }
label18:
  v141 = (bool) v7;
  if (v141) {
    goto label42;
  } else {
    goto label41;
  }
label19:
  ;
  v142 = v3;
  v143 = v142->rodata_buffers;
  v144 = &v143[2];
  v145 = iree_vm_buffer_type();
  v146 = iree_vm_ref_wrap_retain(v144, v145, v15);
  v147 = (bool) v146;
  if (v147) {
    goto label43;
  } else {
    goto label20;
  }
label20:
  ;
  v148 = v3;
  v149 = v148->rodata_buffers;
  v150 = &v149[3];
  v151 = iree_vm_buffer_type();
  v152 = iree_vm_ref_wrap_retain(v150, v151, v21);
  v153 = (bool) v152;
  if (v153) {
    goto label44;
  } else {
    goto label21;
  }
label21:
  ;
  v154 = v3;
  v155 = v154->imports;
  v156 = &v155[9];
  ;
  v158 = &v157;
  v159 = sizeof(iree_vm_ref_t);
  memset(v158, 0, v159);
  iree_vm_ref_assign(v18, v158);
  ;
  v161 = &v160;
  v162 = sizeof(iree_vm_ref_t);
  memset(v161, 0, v162);
  iree_vm_ref_assign(v15, v161);
  ;
  v164 = &v163;
  v165 = sizeof(iree_vm_ref_t);
  memset(v164, 0, v165);
  iree_vm_ref_assign(v21, v164);
  ;
  v167 = &v166;
  ;
  v169 = &v168;
  v170 = module_call_0rrr_iI_import_shim(v1, v156, v158, v161, v164, v167, v169);
  v171 = (bool) v170;
  if (v171) {
    goto label45;
  } else {
    goto label22;
  }
label22:
  v172 = v166;
  v173 = v168;
  v174 = vm_cmp_nz_i64(v173);
  v175 = vm_select_i32(v172, v174, v6);
  v176 = vm_select_i64(v175, v5, v8);
  v177 = vm_cmp_eq_i64(v176, v5);
  ;
  v178 = v3;
  v179 = v178->refs;
  v180 = &v179[0];
  ;
  v181 = v2;
  v182 = v181->types;
  v183 = v182[6];
  v184 = iree_vm_make_ref_type_def(v183);
  v185 = iree_vm_type_def_as_ref(v184);
  v186 = iree_vm_ref_retain_or_move_checked(false, v18, v185, v180);
  v187 = (bool) v186;
  if (v187) {
    goto label46;
  } else {
    goto label23;
  }
label23:
  v188 = vm_cmp_nz_i32(v177);
  v189 = (bool) v188;
  if (v189) {
    goto label24;
  } else {
    goto label30;
  }
label24:
  ;
  v190 = v3;
  v191 = v190->rodata_buffers;
  v192 = &v191[4];
  v193 = iree_vm_buffer_type();
  v194 = iree_vm_ref_wrap_retain(v192, v193, v15);
  v195 = (bool) v194;
  if (v195) {
    goto label47;
  } else {
    goto label25;
  }
label25:
  ;
  v196 = v3;
  v197 = v196->imports;
  v198 = &v197[14];
  ;
  v200 = &v199;
  v201 = sizeof(iree_vm_ref_t);
  memset(v200, 0, v201);
  iree_vm_ref_assign(v18, v200);
  ;
  v203 = &v202;
  v204 = sizeof(iree_vm_ref_t);
  memset(v203, 0, v204);
  iree_vm_ref_assign(v21, v203);
  ;
  v206 = &v205;
  v207 = sizeof(iree_vm_ref_t);
  memset(v206, 0, v207);
  iree_vm_ref_assign(v15, v206);
  ;
  v209 = &v208;
  v210 = sizeof(iree_vm_ref_t);
  memset(v209, 0, v210);
  iree_vm_ref_assign(v12, v209);
  v211 = module_call_0rIrrr_r_import_shim(v1, v198, v200, v8, v203, v206, v209, v12);
  v212 = (bool) v211;
  if (v212) {
    goto label48;
  } else {
    goto label26;
  }
label26:
  ;
  v213 = v3;
  v214 = v213->refs;
  v215 = &v214[1];
  ;
  v216 = v2;
  v217 = v216->types;
  v218 = v217[7];
  v219 = iree_vm_make_ref_type_def(v218);
  v220 = iree_vm_type_def_as_ref(v219);
  v221 = iree_vm_ref_retain_or_move_checked(false, v12, v220, v215);
  v222 = (bool) v221;
  if (v222) {
    goto label49;
  } else {
    goto label27;
  }
label27:
  v223 = module___main_memoize_apply(v1, v2, v3, v12);
  v224 = (bool) v223;
  if (v224) {
    goto label50;
  } else {
    goto label28;
  }
label28:
  ;
  v225 = v3;
  v226 = v225->refs;
  v227 = &v226[2];
  ;
  v228 = v2;
  v229 = v228->types;
  v230 = v229[5];
  v231 = iree_vm_make_ref_type_def(v230);
  v232 = iree_vm_type_def_as_ref(v231);
  v233 = iree_vm_ref_retain_or_move_checked(false, v12, v232, v227);
  v234 = (bool) v233;
  if (v234) {
    goto label51;
  } else {
    goto label29;
  }
label29:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  v235 = iree_ok_status();
  return v235;
label30:
  v236 = (bool) v9;
  if (v236) {
    goto label53;
  } else {
    goto label52;
  }
label31:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v31;
label32:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v48;
label33:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v54;
label34:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v60;
label35:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v78;
label36:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v90;
label37:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v96;
label38:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v114;
label39:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v131;
label40:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v133;
label41:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  v237 = iree_ok_status();
  return v237;
label42:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  v238 = iree_make_cstring_view("HAL device `__device_0` not found or unavailable: #hal.device.target<\"local\", [#hal.executable.target<\"llvm-cpu\", \"embedded-elf-riscv_64\", {cpu = \"generic-rv64\", cpu_features = \"+f,+d\", data_layout = \"e-m:e-p:64:64-i64:64-i128:128-n32:64-S128\", iree.encoding.resolver = #iree_cpu.cpu_encoding_resolver<>, max_stack_allocation_size = 32768 : i64, native_vector_size = 16 : i64, target_triple = \"riscv64-unknown-unknown-eabi-elf\"}>]>");
  ;
  v239 = v238;
  v240 = v239.size;
  v241 = (int) v240;
  v242 = v239.data;
  v243 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v241, v242);
  return v243;
label43:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v146;
label44:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v152;
label45:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v170;
label46:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v186;
label47:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v194;
label48:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v211;
label49:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v221;
label50:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v223;
label51:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  return v233;
label52:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  v244 = iree_ok_status();
  return v244;
label53:
  iree_vm_ref_release(v12);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  v245 = iree_make_cstring_view("HAL device `__device_0` does not support any variant of executable `main_dispatch_0`; available formats: [embedded-elf-riscv_64]");
  ;
  v246 = v245;
  v247 = v246.size;
  v248 = (int) v247;
  v249 = v246.data;
  v250 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v248, v249);
  return v250;
}
static void module_destroy(void* v1) {
  struct module_t* v2;
  struct module_t* v3;
  iree_allocator_t v4;
  v2 = (struct module_t*) v1;
  ;
  v3 = v2;
  v4 = v3->allocator;
  iree_allocator_free(v4, v2);
  return;
}
static iree_status_t module_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3) {
  struct module_state_t* v4;
  iree_host_size_t v5;
  struct module_state_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct module_state_t* v10;
  const uint8_t* v11;
  const uint8_t* v12;
  void* v13;
  iree_host_size_t v14;
  iree_byte_span_t v15;
  iree_allocator_t v16;
  iree_vm_buffer_t* v17;
  iree_vm_buffer_t* v18;
  const uint8_t* v19;
  const uint8_t* v20;
  void* v21;
  iree_host_size_t v22;
  iree_byte_span_t v23;
  iree_allocator_t v24;
  iree_vm_buffer_t* v25;
  iree_vm_buffer_t* v26;
  const uint8_t* v27;
  const uint8_t* v28;
  void* v29;
  iree_host_size_t v30;
  iree_byte_span_t v31;
  iree_allocator_t v32;
  iree_vm_buffer_t* v33;
  iree_vm_buffer_t* v34;
  const uint8_t* v35;
  const uint8_t* v36;
  void* v37;
  iree_host_size_t v38;
  iree_byte_span_t v39;
  iree_allocator_t v40;
  iree_vm_buffer_t* v41;
  iree_vm_buffer_t* v42;
  const uint8_t* v43;
  const uint8_t* v44;
  void* v45;
  iree_host_size_t v46;
  iree_byte_span_t v47;
  iree_allocator_t v48;
  iree_vm_buffer_t* v49;
  iree_vm_buffer_t* v50;
  const uint8_t* v51;
  const uint8_t* v52;
  void* v53;
  iree_host_size_t v54;
  iree_byte_span_t v55;
  iree_allocator_t v56;
  iree_vm_buffer_t* v57;
  iree_vm_buffer_t* v58;
  const uint8_t* v59;
  const uint8_t* v60;
  void* v61;
  iree_host_size_t v62;
  iree_byte_span_t v63;
  iree_allocator_t v64;
  iree_vm_buffer_t* v65;
  iree_vm_buffer_t* v66;
  iree_vm_module_state_t* v67;
  iree_status_t v68;
  v4 = NULL;
  v5 = sizeof(struct module_state_t);
  v6 = &v4;
  v7 = (void**) v6;
  v8 = iree_allocator_malloc(v2, v5, v7);
  v9 = (bool) v8;
  if (v9) {
    goto label3;
  } else {
    goto label2;
  }
label2:
  v10 = v4;
  memset(v10, 0, v5);
  v4->allocator = v2;
  v11 = module__utf8_hal_device_id_C6650FF277232B5A;
  v12 = v11;
  v13 = (void*) v12;
  v14 = sizeof(module__utf8_hal_device_id_C6650FF277232B5A);
  v15 = iree_make_byte_span(v13, v14);
  v16 = iree_allocator_null();
  v17 = v4->rodata_buffers;
  v18 = &v17[0];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v15, v16, v18);
  v19 = module__utf8_local_1A8FF0278D7661D8;
  v20 = v19;
  v21 = (void*) v20;
  v22 = sizeof(module__utf8_local_1A8FF0278D7661D8);
  v23 = iree_make_byte_span(v21, v22);
  v24 = iree_allocator_null();
  v25 = v4->rodata_buffers;
  v26 = &v25[1];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v23, v24, v26);
  v27 = module__utf8_hal_executable_format_E03EECB63A2AAF52;
  v28 = v27;
  v29 = (void*) v28;
  v30 = sizeof(module__utf8_hal_executable_format_E03EECB63A2AAF52);
  v31 = iree_make_byte_span(v29, v30);
  v32 = iree_allocator_null();
  v33 = v4->rodata_buffers;
  v34 = &v33[2];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v31, v32, v34);
  v35 = module__utf8_embedded_elf_riscv_64_C3BA011E86B77EF5;
  v36 = v35;
  v37 = (void*) v36;
  v38 = sizeof(module__utf8_embedded_elf_riscv_64_C3BA011E86B77EF5);
  v39 = iree_make_byte_span(v37, v38);
  v40 = iree_allocator_null();
  v41 = v4->rodata_buffers;
  v42 = &v41[3];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v39, v40, v42);
  v43 = module_main_dispatch_0_embedded_elf_riscv_64;
  v44 = v43;
  v45 = (void*) v44;
  v46 = sizeof(module_main_dispatch_0_embedded_elf_riscv_64);
  v47 = iree_make_byte_span(v45, v46);
  v48 = iree_allocator_null();
  v49 = v4->rodata_buffers;
  v50 = &v49[4];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v47, v48, v50);
  v51 = module__utf8_input0_DCE99660CEB3F6B;
  v52 = v51;
  v53 = (void*) v52;
  v54 = sizeof(module__utf8_input0_DCE99660CEB3F6B);
  v55 = iree_make_byte_span(v53, v54);
  v56 = iree_allocator_null();
  v57 = v4->rodata_buffers;
  v58 = &v57[5];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v55, v56, v58);
  v59 = module__utf8_tensor_FC1814BC4A58F22A;
  v60 = v59;
  v61 = (void*) v60;
  v62 = sizeof(module__utf8_tensor_FC1814BC4A58F22A);
  v63 = iree_make_byte_span(v61, v62);
  v64 = iree_allocator_null();
  v65 = v4->rodata_buffers;
  v66 = &v65[6];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v63, v64, v66);
  v67 = (iree_vm_module_state_t*) v10;
  EMITC_DEREF_ASSIGN_VALUE(v3, v67);
  v68 = iree_ok_status();
  return v68;
label3:
  return v8;
}
static void module_free_state(void* v1, iree_vm_module_state_t* v2) {
  struct module_state_t* v3;
  struct module_state_t* v4;
  iree_vm_ref_t* v5;
  iree_vm_ref_t* v6;
  iree_vm_ref_t* v7;
  iree_vm_ref_t* v8;
  iree_allocator_t v9;
  v3 = (struct module_state_t*) v2;
  ;
  v4 = v3;
  v5 = v4->refs;
  v6 = &v5[0];
  iree_vm_ref_release(v6);
  v7 = &v5[1];
  iree_vm_ref_release(v7);
  v8 = &v5[2];
  iree_vm_ref_release(v8);
  v9 = v4->allocator;
  iree_allocator_free(v9, v3);
  return;
}
static iree_status_t module_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4) {
  iree_status_t v5;
  v5 = iree_make_status(IREE_STATUS_UNIMPLEMENTED);
  return v5;
}
static iree_status_t module_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5) {
  struct module_state_t* v6;
  struct module_state_t* v7;
  iree_vm_function_t* v8;
  iree_vm_function_t* v9;
  iree_status_t v10;
  v6 = (struct module_state_t*) v2;
  ;
  v7 = v6;
  v8 = v7->imports;
  v9 = &v8[v3];
  EMITC_DEREF_ASSIGN_PTR(v9, v4);
  v10 = iree_ok_status();
  return v10;
}
iree_status_t mobilenet_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3) {
  struct module_t* v4;
  iree_host_size_t v5;
  struct module_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct module_t* v10;
  iree_vm_ref_type_t* v11;
  iree_string_view_t v12;
  iree_vm_ref_type_t v13;
  iree_string_view_t v14;
  iree_vm_ref_type_t v15;
  iree_string_view_t v16;
  iree_vm_ref_type_t v17;
  iree_string_view_t v18;
  iree_vm_ref_type_t v19;
  iree_string_view_t v20;
  iree_vm_ref_type_t v21;
  iree_string_view_t v22;
  iree_vm_ref_type_t v23;
  iree_string_view_t v24;
  iree_vm_ref_type_t v25;
  iree_string_view_t v26;
  iree_vm_ref_type_t v27;
  iree_string_view_t v28;
  iree_vm_ref_type_t v29;
  iree_string_view_t v30;
  iree_vm_ref_type_t v31;
  iree_vm_module_t v32;
  iree_vm_module_t* v33;
  struct module_t* v34;
  iree_status_t v35;
  bool v36;
  iree_status_t v37;
  struct module_t* v38;
  v4 = NULL;
  v5 = sizeof(struct module_t);
  v6 = &v4;
  v7 = (void**) v6;
  v8 = iree_allocator_malloc(v2, v5, v7);
  v9 = (bool) v8;
  if (v9) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  v10 = v4;
  memset(v10, 0, v5);
  v4->allocator = v2;
  v11 = v4->types;
  v12 = iree_make_cstring_view("i32");
  v13 = iree_vm_instance_lookup_type(v1, v12);
  v11[0] = v13;
  v14 = iree_make_cstring_view("i64");
  v15 = iree_vm_instance_lookup_type(v1, v14);
  v11[1] = v15;
  v16 = iree_make_cstring_view("hal.allocator");
  v17 = iree_vm_instance_lookup_type(v1, v16);
  v11[2] = v17;
  v18 = iree_make_cstring_view("hal.buffer");
  v19 = iree_vm_instance_lookup_type(v1, v18);
  v11[3] = v19;
  v20 = iree_make_cstring_view("hal.buffer_view");
  v21 = iree_vm_instance_lookup_type(v1, v20);
  v11[4] = v21;
  v22 = iree_make_cstring_view("hal.command_buffer");
  v23 = iree_vm_instance_lookup_type(v1, v22);
  v11[5] = v23;
  v24 = iree_make_cstring_view("hal.device");
  v25 = iree_vm_instance_lookup_type(v1, v24);
  v11[6] = v25;
  v26 = iree_make_cstring_view("hal.executable");
  v27 = iree_vm_instance_lookup_type(v1, v26);
  v11[7] = v27;
  v28 = iree_make_cstring_view("hal.fence");
  v29 = iree_vm_instance_lookup_type(v1, v28);
  v11[8] = v29;
  v30 = iree_make_cstring_view("vm.buffer");
  v31 = iree_vm_instance_lookup_type(v1, v30);
  v11[9] = v31;
  ;
  v33 = &v32;
  v34 = v4;
  v35 = iree_vm_module_initialize(v33, v34);
  v36 = iree_status_is_ok(v35);
  if (v36) {
    goto label3;
  } else {
    goto label5;
  }
label3:
  v32.destroy = module_destroy;
  v32.alloc_state = module_alloc_state;
  v32.free_state = module_free_state;
  v32.fork_state = module_fork_state;
  v32.resolve_import = module_resolve_import;
  v37 = iree_vm_native_module_create(v33, &module_descriptor_, v1, v2, v3);
  return v37;
label4:
  return v8;
label5:
  v38 = v4;
  iree_allocator_free(v2, v38);
  return v35;
}
#endif   // EMITC_IMPLEMENTATION

