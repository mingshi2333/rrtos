#ifndef BE_U1000_HELLO_WORLD_FLOAT_H_
#define BE_U1000_HELLO_WORLD_FLOAT_H_
#include "iree/vm/api.h"
#ifdef __cplusplus
extern "C" {
#endif //  __cplusplus
iree_status_t be_u1000_hello_world_float_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3);
#ifdef __cplusplus
}  // extern "C"
#endif //  __cplusplus
#endif //  BE_U1000_HELLO_WORLD_FLOAT_H_
#if defined(EMITC_IMPLEMENTATION)
#include "iree/vm/ops.h"
#include "iree/vm/ops_emitc.h"
#include "iree/vm/shims_emitc.h"
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_hal_device_id_C1DCB7DBC4F49AE6[] = {104, 97, 108, 46, 100, 101, 118, 105, 99, 101, 46, 105, 100};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_local_3D3B5147158B235A[] = {108, 111, 99, 97, 108, 42};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_hal_executable_format_EAB228F999C2D3A1[] = {104, 97, 108, 46, 101, 120, 101, 99, 117, 116, 97, 98, 108, 101, 46, 102, 111, 114, 109, 97, 116};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_static_96B31E405495E0B6[] = {115, 116, 97, 116, 105, 99};
iree_alignas(16) static const uint8_t be_u1000_hello_world_float_be_u1000_hello_world_float_linked_static[] = {98, 101, 95, 117, 49, 48, 48, 48, 95, 104, 101, 108, 108, 111, 95, 119, 111, 114, 108, 100, 95, 102, 108, 111, 97, 116, 95, 108, 105, 110, 107, 101, 100};
iree_alignas(64) static const uint8_t be_u1000_hello_world_float__const[] = {234, 133, 241, 190, 239, 28, 154, 63, 163, 35, 161, 63, 18, 148, 183, 62, 0, 137, 226, 61, 235, 122, 134, 190, 24, 111, 169, 62, 236, 109, 97, 63, 18, 74, 61, 190, 66, 107, 138, 63, 193, 195, 60, 191, 232, 192, 159, 62, 238, 175, 89, 191, 193, 81, 110, 191, 152, 140, 11, 62, 225, 195, 227, 61, 48, 119, 249, 189, 187, 48, 201, 190, 69, 245, 72, 62, 82, 20, 50, 63, 100, 204, 18, 62, 224, 225, 131, 189, 236, 137, 56, 190, 16, 208, 94, 189, 54, 127, 236, 190, 211, 166, 207, 62, 66, 43, 143, 62, 255, 158, 3, 191, 240, 136, 215, 190, 188, 39, 32, 63, 82, 165, 191, 190, 48, 163, 169, 190, 0, 0, 0, 0, 0, 0, 0, 0, 153, 222, 193, 62, 248, 223, 56, 191, 157, 66, 186, 61, 50, 143, 91, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 157, 161, 151, 63, 22, 206, 85, 190, 0, 0, 0, 0, 0, 0, 0, 0, 73, 190, 65, 188, 0, 0, 0, 0, 0, 0, 0, 0, 128, 109, 50, 59, 10, 100, 66, 62, 245, 164, 211, 62, 26, 199, 58, 190, 102, 153, 77, 62, 172, 233, 73, 190, 56, 115, 123, 189, 2, 179, 4, 190, 52, 174, 255, 189, 183, 245, 219, 190, 175, 179, 178, 190, 77, 149, 177, 62, 131, 166, 188, 62, 142, 20, 59, 62, 250, 124, 116, 62, 233, 249, 109, 190, 144, 49, 211, 188, 86, 242, 72, 62, 74, 1, 10, 61, 91, 130, 27, 191, 229, 38, 146, 61, 205, 90, 205, 62, 105, 67, 204, 190, 36, 170, 255, 189, 243, 2, 194, 190, 229, 42, 135, 62, 113, 98, 189, 190, 47, 163, 174, 62, 42, 71, 137, 190, 100, 59, 37, 62, 238, 111, 9, 190, 101, 224, 186, 62, 22, 255, 17, 62, 96, 105, 56, 190, 95, 124, 182, 62, 212, 236, 19, 191, 189, 163, 201, 190, 3, 65, 78, 63, 169, 242, 134, 190, 112, 79, 133, 188, 83, 236, 154, 62, 79, 201, 233, 62, 226, 250, 96, 62, 156, 127, 96, 190, 134, 199, 33, 62, 42, 197, 0, 191, 209, 218, 170, 62, 170, 139, 20, 190, 81, 96, 84, 190, 72, 193, 183, 190, 211, 8, 56, 62, 88, 46, 235, 62, 3, 146, 79, 62, 90, 73, 203, 190, 245, 30, 191, 62, 128, 220, 156, 190, 207, 153, 162, 62, 89, 130, 61, 190, 135, 111, 152, 62, 134, 165, 138, 190, 14, 155, 99, 190, 251, 122, 51, 190, 6, 16, 113, 190, 168, 252, 16, 189, 156, 70, 145, 61, 136, 60, 146, 190, 212, 188, 244, 189, 77, 116, 191, 62, 136, 108, 3, 189, 123, 233, 219, 190, 159, 223, 193, 62, 108, 228, 130, 61, 68, 120, 213, 61, 128, 138, 252, 187, 6, 123, 20, 62, 176, 36, 33, 61, 24, 206, 9, 61, 194, 40, 179, 190, 214, 176, 8, 190, 28, 99, 195, 190, 128, 152, 94, 59, 172, 216, 231, 61, 49, 18, 164, 60, 34, 2, 237, 61, 71, 243, 166, 187, 130, 171, 216, 62, 0, 252, 1, 187, 167, 201, 138, 62, 128, 93, 151, 189, 95, 232, 59, 190, 68, 98, 194, 189, 8, 150, 120, 189, 218, 216, 114, 62, 160, 243, 143, 62, 104, 216, 51, 189, 38, 20, 23, 62, 172, 192, 201, 190, 142, 138, 148, 190, 80, 61, 181, 188, 207, 187, 130, 62, 154, 136, 179, 190, 17, 138, 218, 190, 233, 217, 149, 62, 160, 19, 75, 61, 249, 182, 131, 62, 244, 20, 188, 190, 28, 137, 193, 61, 235, 238, 202, 62, 252, 48, 171, 190, 252, 98, 155, 189, 80, 116, 175, 190, 55, 22, 214, 62, 48, 62, 181, 188, 0, 209, 246, 58, 102, 189, 249, 61, 148, 42, 6, 63, 247, 200, 203, 190, 74, 165, 220, 190, 181, 208, 162, 190, 153, 240, 122, 190, 66, 27, 83, 190, 223, 144, 110, 190, 238, 254, 191, 62, 128, 211, 83, 60, 32, 0, 69, 60, 44, 208, 79, 190, 240, 103, 223, 189, 206, 177, 5, 62, 12, 74, 243, 61, 58, 241, 80, 62, 160, 178, 41, 190, 120, 105, 20, 62, 68, 147, 248, 61, 36, 103, 163, 61, 122, 155, 150, 190, 72, 105, 202, 189, 124, 234, 171, 61, 50, 214, 139, 62, 163, 202, 71, 189, 190, 26, 205, 190, 193, 84, 206, 62, 216, 187, 195, 62, 92, 252, 219, 190, 80, 240, 160, 60, 128, 48, 208, 60, 101, 41, 179, 190, 241, 31, 175, 188, 64, 250, 205, 62, 243, 51, 70, 190, 232, 154, 231, 190, 16, 161, 157, 190, 206, 196, 67, 62, 22, 175, 92, 190, 5, 248, 15, 191, 154, 129, 208, 62, 128, 148, 198, 59, 43, 12, 95, 190, 61, 198, 191, 62, 40, 246, 125, 189, 202, 97, 142, 190, 64, 36, 231, 60, 195, 241, 131, 62, 39, 48, 63, 62, 53, 21, 218, 190, 96, 163, 167, 187, 252, 0, 101, 63, 7, 150, 189, 62, 55, 147, 131, 62, 136, 69, 59, 61, 245, 255, 199, 188, 48, 93, 139, 189, 197, 121, 145, 62, 120, 20, 21, 190, 239, 229, 35, 191, 17, 42, 167, 62, 65, 126, 218, 62, 183, 50, 123, 190, 141, 99, 196, 62, 62, 41, 38, 62, 188, 91, 233, 189, 144, 73, 89, 61, 224, 135, 125, 188, 177, 239, 172, 62, 184, 48, 22, 189, 172, 86, 142, 189, 24, 88, 187, 190, 144, 111, 171, 189, 227, 97, 132, 62, 72, 65, 109, 61, 251, 34, 205, 62, 128, 155, 2, 60, 141, 195, 177, 190, 202, 178, 204, 190, 98, 171, 101, 190, 175, 23, 83, 62, 151, 223, 7, 190, 152, 33, 127, 62, 99, 16, 81, 63, 78, 30, 3, 62, 56, 163, 153, 190, 120, 31, 32, 190, 13, 218, 242, 62, 134, 172, 67, 190, 57, 203, 169, 62, 32, 114, 82, 61, 2, 151, 202, 190, 92, 232, 216, 189, 95, 56, 178, 62, 131, 21, 188, 62, 167, 251, 162, 62, 174, 60, 119, 190, 0, 228, 126, 190, 11, 196, 124, 190, 19, 76, 75, 63, 115, 132, 208, 62, 224, 103, 85, 60, 164, 39, 167, 190, 111, 111, 237, 189, 197, 184, 14, 63, 80, 93, 128, 60, 110, 55, 9, 62, 145, 116, 47, 191, 236, 43, 177, 61, 255, 173, 131, 62, 12, 4, 187, 189, 136, 220, 183, 189, 181, 27, 34, 190, 136, 155, 134, 62, 239, 26, 64, 62, 122, 98, 208, 190, 252, 77, 239, 61, 20, 233, 219, 190, 129, 124, 137, 190, 15, 215, 124, 62, 145, 205, 22, 190, 107, 251, 135, 62, 194, 191, 143, 190, 100, 105, 132, 62, 143, 28, 214, 190, 60, 99, 183, 189, 106, 104, 96, 62, 205, 105, 147, 62, 203, 35, 135, 190, 15, 225, 168, 190, 48, 64, 45, 62, 182, 201, 44, 61, 180, 30, 82, 190, 73, 148, 193, 190, 0, 43, 158, 187, 68, 158, 170, 62, 11, 162, 158, 62, 74, 38, 55, 190, 8, 142, 48, 190, 84, 191, 105, 61, 80, 51, 161, 190, 223, 41, 203, 190, 0, 0, 0, 0, 197, 179, 0, 63, 164, 186, 208, 62, 82, 206, 130, 190, 0, 0, 0, 0, 79, 27, 51, 62, 0, 0, 0, 0, 33, 114, 119, 190, 204, 205, 143, 61, 253, 163, 220, 190, 136, 227, 24, 63, 0, 0, 0, 0, 145, 140, 97, 62, 14, 118, 11, 63, 176, 85, 71, 190, 20, 9, 146, 189};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735[] = {105, 110, 112, 117, 116, 48};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_tensor_3C6209B4FD120BDC[] = {116, 101, 110, 115, 111, 114};
struct be_u1000_hello_world_float_t {iree_allocator_t allocator;iree_vm_ref_type_t types[11];};
struct be_u1000_hello_world_float_state_t {iree_allocator_t allocator;uint8_t rwdata[1];iree_vm_ref_t refs[4];iree_vm_buffer_t rodata_buffers[8];iree_vm_function_t imports[24];};
typedef iree_status_t(*begin_call_t)(void*, iree_vm_stack_t*, iree_vm_function_call_t);
static iree_status_t be_u1000_hello_world_float_call_0rIirIIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int64_t v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10);
static iree_status_t be_u1000_hello_world_float_call_0rIiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, int32_t v6, int64_t v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_hello_world_float_call_0riIiirII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t v5, int32_t v6, int32_t v7, iree_vm_ref_t* v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_hello_world_float_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8);
static iree_status_t be_u1000_hello_world_float_call_0rIIiiCID_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_hello_world_float_call_0rriiCID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8, int64_t v9);
static iree_status_t be_u1000_hello_world_float_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_hello_world_float_call_0ri_I_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t* v5);
static iree_status_t be_u1000_hello_world_float_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_hello_world_float_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3);
static iree_status_t be_u1000_hello_world_float_call_0riii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int32_t v6);
static iree_status_t be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_2_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, int32_t v14, int32_t v15, iree_vm_ref_t* v16, int64_t v17, int64_t v18, int32_t v19, int32_t v20, iree_vm_ref_t* v21, int64_t v22, int64_t v23, int32_t v24, int32_t v25, iree_vm_ref_t* v26, int64_t v27, int64_t v28);
static iree_status_t be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_4_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, int32_t v14, int32_t v15, int32_t v16, int32_t v17, iree_vm_ref_t* v18, int64_t v19, int64_t v20, int32_t v21, int32_t v22, iree_vm_ref_t* v23, int64_t v24, int64_t v25, int32_t v26, int32_t v27, iree_vm_ref_t* v28, int64_t v29, int64_t v30);
static iree_status_t be_u1000_hello_world_float_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7);
static iree_status_t be_u1000_hello_world_float_call_0rIrriiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int32_t v7, int32_t v8, int32_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_hello_world_float_call_0rIrrr_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7);
static iree_status_t be_u1000_hello_world_float_call_0rIrrrIrIIi_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, iree_vm_ref_t* v9, int64_t v10, int64_t v11, int32_t v12);
static iree_status_t be_u1000_hello_world_float_call_0rIrrCrD_v_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int32_t v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_hello_world_float_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3);
static iree_status_t be_u1000_hello_world_float_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_hello_world_float_call_0rrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7);
static iree_status_t be_u1000_hello_world_float_call_0ri_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, iree_vm_ref_t* v5);
static iree_status_t be_u1000_hello_world_float_call_0CrD_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6);
static iree_status_t be_u1000_hello_world_float_call_0iCrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int32_t v4, iree_vm_ref_t* v5, int32_t* v6);
static iree_status_t be_u1000_hello_world_float_main(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5);
static iree_status_t be_u1000_hello_world_float_main_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t be_u1000_hello_world_float___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t be_u1000_hello_world_float___init(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3);
static void be_u1000_hello_world_float_destroy(void* v1);
static iree_status_t be_u1000_hello_world_float_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3);
static void be_u1000_hello_world_float_free_state(void* v1, iree_vm_module_state_t* v2);
static iree_status_t be_u1000_hello_world_float_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4);
static iree_status_t be_u1000_hello_world_float_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5);
static const iree_vm_module_dependency_t be_u1000_hello_world_float_dependencies_[] = {{{"hal", 3}, 5, IREE_VM_MODULE_DEPENDENCY_FLAG_REQUIRED},};
static const iree_vm_native_import_descriptor_t be_u1000_hello_world_float_imports_[] = {{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.ex.file.from_memory", 23}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.allocator.allocate", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.allocator.import", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer.assert", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.create", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.assert", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.buffer", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.dim", 19}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.create", 25}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.finalize", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.execution_barrier", 36}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.dispatch", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.allocator", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.query.i64", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.alloca", 23}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.dealloca", 25}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.read", 21}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.execute", 24}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.count", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.get", 15}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.executable.create", 21}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.create", 16}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.join", 14}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.await", 15}},};
static const iree_vm_native_export_descriptor_t be_u1000_hello_world_float_exports_[] = {{{"__init", 6}, {"0v_v", 4}, 0, NULL},{{"main", 4}, {"0r_r", 4}, 0, NULL},};
static const iree_vm_native_function_ptr_t be_u1000_hello_world_float_funcs_[] = {{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)be_u1000_hello_world_float___init_export_shim},{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)be_u1000_hello_world_float_main_export_shim},};
static const iree_vm_native_module_descriptor_t be_u1000_hello_world_float_descriptor_ = {{"be_u1000_hello_world_float", 26},0,0,NULL,1,be_u1000_hello_world_float_dependencies_,24,be_u1000_hello_world_float_imports_,2,be_u1000_hello_world_float_exports_,2,be_u1000_hello_world_float_funcs_,};
static iree_status_t be_u1000_hello_world_float_call_0rIirIIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int64_t v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10) {
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
  int32_t v46;
  iree_host_size_t v47;
  int32_t* v48;
  iree_host_size_t v49;
  uint8_t* v50;
  iree_vm_ref_t* v51;
  iree_host_size_t v52;
  uint8_t* v53;
  int64_t v54;
  iree_host_size_t v55;
  int64_t* v56;
  iree_host_size_t v57;
  uint8_t* v58;
  int64_t v59;
  iree_host_size_t v60;
  int64_t* v61;
  iree_host_size_t v62;
  uint8_t* v63;
  int32_t v64;
  iree_host_size_t v65;
  int32_t* v66;
  begin_call_t v67;
  iree_vm_module_t* v68;
  iree_vm_function_call_t v69;
  iree_status_t v70;
  bool v71;
  uint8_t* v72;
  iree_vm_ref_t* v73;
  iree_status_t v74;
  iree_status_t v75;
  v11 = 0;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v11 + v12;
  v14 = sizeof(int64_t);
  v15 = v13 + v14;
  v16 = sizeof(int32_t);
  v17 = v15 + v16;
  v18 = sizeof(iree_vm_ref_t);
  v19 = v17 + v18;
  v20 = sizeof(int64_t);
  v21 = v19 + v20;
  v22 = sizeof(int64_t);
  v23 = v21 + v22;
  v24 = sizeof(int32_t);
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
  v47 = sizeof(int32_t);
  v48 = &v46;
  memcpy(v45, v48, v47);
  v49 = sizeof(int32_t);
  v50 = v45 + v49;
  v51 = (iree_vm_ref_t*) v50;
  iree_vm_ref_assign(v6, v51);
  v52 = sizeof(iree_vm_ref_t);
  v53 = v50 + v52;
  ;
  v54 = v7;
  v55 = sizeof(int64_t);
  v56 = &v54;
  memcpy(v53, v56, v55);
  v57 = sizeof(int64_t);
  v58 = v53 + v57;
  ;
  v59 = v8;
  v60 = sizeof(int64_t);
  v61 = &v59;
  memcpy(v58, v61, v60);
  v62 = sizeof(int64_t);
  v63 = v58 + v62;
  ;
  v64 = v9;
  v65 = sizeof(int32_t);
  v66 = &v64;
  memcpy(v63, v66, v65);
  v67 = v28->module->begin_call;
  v68 = v28->module;
  v69 = v31;
  v70 = EMITC_CALL_INDIRECT(v67, v68, v1, v69);
  v71 = (bool) v70;
  if (v71) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v72 = v31.results.data;
  v73 = (iree_vm_ref_t*) v72;
  iree_vm_ref_move(v73, v10);
  v74 = iree_ok_status();
  return v74;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v6);
  v75 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v75;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v6);
  return v70;
}

static iree_status_t be_u1000_hello_world_float_call_0rIiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, int32_t v6, int64_t v7, iree_vm_ref_t* v8) {
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
  int32_t v40;
  iree_host_size_t v41;
  int32_t* v42;
  iree_host_size_t v43;
  uint8_t* v44;
  int32_t v45;
  iree_host_size_t v46;
  int32_t* v47;
  iree_host_size_t v48;
  uint8_t* v49;
  int64_t v50;
  iree_host_size_t v51;
  int64_t* v52;
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
  v12 = sizeof(int64_t);
  v13 = v11 + v12;
  v14 = sizeof(int32_t);
  v15 = v13 + v14;
  v16 = sizeof(int32_t);
  v17 = v15 + v16;
  v18 = sizeof(int64_t);
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
  ;
  v40 = v5;
  v41 = sizeof(int32_t);
  v42 = &v40;
  memcpy(v39, v42, v41);
  v43 = sizeof(int32_t);
  v44 = v39 + v43;
  ;
  v45 = v6;
  v46 = sizeof(int32_t);
  v47 = &v45;
  memcpy(v44, v47, v46);
  v48 = sizeof(int32_t);
  v49 = v44 + v48;
  ;
  v50 = v7;
  v51 = sizeof(int64_t);
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

static iree_status_t be_u1000_hello_world_float_call_0riIiirII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t v5, int32_t v6, int32_t v7, iree_vm_ref_t* v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11) {
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
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_vm_function_t* v31;
  iree_vm_module_t* v32;
  bool v33;
  iree_vm_function_call_t v34;
  iree_vm_function_t v35;
  void* v36;
  uint8_t* v37;
  void* v38;
  uint8_t* v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  int32_t v44;
  iree_host_size_t v45;
  int32_t* v46;
  iree_host_size_t v47;
  uint8_t* v48;
  int64_t v49;
  iree_host_size_t v50;
  int64_t* v51;
  iree_host_size_t v52;
  uint8_t* v53;
  int32_t v54;
  iree_host_size_t v55;
  int32_t* v56;
  iree_host_size_t v57;
  uint8_t* v58;
  int32_t v59;
  iree_host_size_t v60;
  int32_t* v61;
  iree_host_size_t v62;
  uint8_t* v63;
  iree_vm_ref_t* v64;
  iree_host_size_t v65;
  uint8_t* v66;
  int64_t v67;
  iree_host_size_t v68;
  int64_t* v69;
  iree_host_size_t v70;
  uint8_t* v71;
  int64_t v72;
  iree_host_size_t v73;
  int64_t* v74;
  begin_call_t v75;
  iree_vm_module_t* v76;
  iree_vm_function_call_t v77;
  iree_status_t v78;
  bool v79;
  uint8_t* v80;
  iree_vm_ref_t* v81;
  iree_status_t v82;
  iree_status_t v83;
  v12 = 0;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(int32_t);
  v16 = v14 + v15;
  v17 = sizeof(int64_t);
  v18 = v16 + v17;
  v19 = sizeof(int32_t);
  v20 = v18 + v19;
  v21 = sizeof(int32_t);
  v22 = v20 + v21;
  v23 = sizeof(iree_vm_ref_t);
  v24 = v22 + v23;
  v25 = sizeof(int64_t);
  v26 = v24 + v25;
  v27 = sizeof(int64_t);
  v28 = v26 + v27;
  v29 = sizeof(iree_vm_ref_t);
  v30 = v12 + v29;
  ;
  v31 = v2;
  v32 = v31->module;
  v33 = !v32;
  if (v33) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v35 = *v2;
  v34.function = v35;
  v36 = iree_alloca(v28);
  v37 = (uint8_t*) v36;
  v34.arguments.data_length = v28;
  v34.arguments.data = v37;
  memset(v37, 0, v28);
  v38 = iree_alloca(v30);
  v39 = (uint8_t*) v38;
  v34.results.data_length = v30;
  v34.results.data = v39;
  memset(v39, 0, v30);
  v40 = v34.arguments.data;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_assign(v3, v41);
  v42 = sizeof(iree_vm_ref_t);
  v43 = v40 + v42;
  ;
  v44 = v4;
  v45 = sizeof(int32_t);
  v46 = &v44;
  memcpy(v43, v46, v45);
  v47 = sizeof(int32_t);
  v48 = v43 + v47;
  ;
  v49 = v5;
  v50 = sizeof(int64_t);
  v51 = &v49;
  memcpy(v48, v51, v50);
  v52 = sizeof(int64_t);
  v53 = v48 + v52;
  ;
  v54 = v6;
  v55 = sizeof(int32_t);
  v56 = &v54;
  memcpy(v53, v56, v55);
  v57 = sizeof(int32_t);
  v58 = v53 + v57;
  ;
  v59 = v7;
  v60 = sizeof(int32_t);
  v61 = &v59;
  memcpy(v58, v61, v60);
  v62 = sizeof(int32_t);
  v63 = v58 + v62;
  v64 = (iree_vm_ref_t*) v63;
  iree_vm_ref_assign(v8, v64);
  v65 = sizeof(iree_vm_ref_t);
  v66 = v63 + v65;
  ;
  v67 = v9;
  v68 = sizeof(int64_t);
  v69 = &v67;
  memcpy(v66, v69, v68);
  v70 = sizeof(int64_t);
  v71 = v66 + v70;
  ;
  v72 = v10;
  v73 = sizeof(int64_t);
  v74 = &v72;
  memcpy(v71, v74, v73);
  v75 = v31->module->begin_call;
  v76 = v31->module;
  v77 = v34;
  v78 = EMITC_CALL_INDIRECT(v75, v76, v1, v77);
  v79 = (bool) v78;
  if (v79) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v80 = v34.results.data;
  v81 = (iree_vm_ref_t*) v80;
  iree_vm_ref_move(v81, v11);
  v82 = iree_ok_status();
  return v82;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v8);
  v83 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v83;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v8);
  return v78;
}

static iree_status_t be_u1000_hello_world_float_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8) {
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

static iree_status_t be_u1000_hello_world_float_call_0rIIiiCID_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11) {
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
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_vm_function_t* v31;
  iree_vm_module_t* v32;
  bool v33;
  iree_vm_function_call_t v34;
  iree_vm_function_t v35;
  void* v36;
  uint8_t* v37;
  void* v38;
  uint8_t* v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  int64_t v44;
  iree_host_size_t v45;
  int64_t* v46;
  iree_host_size_t v47;
  uint8_t* v48;
  int64_t v49;
  iree_host_size_t v50;
  int64_t* v51;
  iree_host_size_t v52;
  uint8_t* v53;
  int32_t v54;
  iree_host_size_t v55;
  int32_t* v56;
  iree_host_size_t v57;
  uint8_t* v58;
  int32_t v59;
  iree_host_size_t v60;
  int32_t* v61;
  iree_host_size_t v62;
  uint8_t* v63;
  int32_t v64;
  iree_host_size_t v65;
  int32_t* v66;
  iree_host_size_t v67;
  uint8_t* v68;
  int64_t v69;
  iree_host_size_t v70;
  int64_t* v71;
  iree_host_size_t v72;
  uint8_t* v73;
  int64_t v74;
  iree_host_size_t v75;
  int64_t* v76;
  begin_call_t v77;
  iree_vm_module_t* v78;
  iree_vm_function_call_t v79;
  iree_status_t v80;
  bool v81;
  uint8_t* v82;
  iree_vm_ref_t* v83;
  iree_status_t v84;
  iree_status_t v85;
  v12 = 0;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(int64_t);
  v16 = v14 + v15;
  v17 = sizeof(int64_t);
  v18 = v16 + v17;
  v19 = sizeof(int32_t);
  v20 = v18 + v19;
  v21 = sizeof(int32_t);
  v22 = v20 + v21;
  v23 = sizeof(int32_t);
  v24 = v22 + v23;
  v25 = sizeof(int64_t);
  v26 = v24 + v25;
  v27 = sizeof(int64_t);
  v28 = v26 + v27;
  v29 = sizeof(iree_vm_ref_t);
  v30 = v12 + v29;
  ;
  v31 = v2;
  v32 = v31->module;
  v33 = !v32;
  if (v33) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v35 = *v2;
  v34.function = v35;
  v36 = iree_alloca(v28);
  v37 = (uint8_t*) v36;
  v34.arguments.data_length = v28;
  v34.arguments.data = v37;
  memset(v37, 0, v28);
  v38 = iree_alloca(v30);
  v39 = (uint8_t*) v38;
  v34.results.data_length = v30;
  v34.results.data = v39;
  memset(v39, 0, v30);
  v40 = v34.arguments.data;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_assign(v3, v41);
  v42 = sizeof(iree_vm_ref_t);
  v43 = v40 + v42;
  ;
  v44 = v4;
  v45 = sizeof(int64_t);
  v46 = &v44;
  memcpy(v43, v46, v45);
  v47 = sizeof(int64_t);
  v48 = v43 + v47;
  ;
  v49 = v5;
  v50 = sizeof(int64_t);
  v51 = &v49;
  memcpy(v48, v51, v50);
  v52 = sizeof(int64_t);
  v53 = v48 + v52;
  ;
  v54 = v6;
  v55 = sizeof(int32_t);
  v56 = &v54;
  memcpy(v53, v56, v55);
  v57 = sizeof(int32_t);
  v58 = v53 + v57;
  ;
  v59 = v7;
  v60 = sizeof(int32_t);
  v61 = &v59;
  memcpy(v58, v61, v60);
  v62 = sizeof(int32_t);
  v63 = v58 + v62;
  ;
  v64 = v8;
  v65 = sizeof(int32_t);
  v66 = &v64;
  memcpy(v63, v66, v65);
  v67 = sizeof(int32_t);
  v68 = v63 + v67;
  ;
  v69 = v9;
  v70 = sizeof(int64_t);
  v71 = &v69;
  memcpy(v68, v71, v70);
  v72 = sizeof(int64_t);
  v73 = v68 + v72;
  ;
  v74 = v10;
  v75 = sizeof(int64_t);
  v76 = &v74;
  memcpy(v73, v76, v75);
  v77 = v31->module->begin_call;
  v78 = v31->module;
  v79 = v34;
  v80 = EMITC_CALL_INDIRECT(v77, v78, v1, v79);
  v81 = (bool) v80;
  if (v81) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v82 = v34.results.data;
  v83 = (iree_vm_ref_t*) v82;
  iree_vm_ref_move(v83, v11);
  v84 = iree_ok_status();
  return v84;
label4:
  iree_vm_ref_release(v3);
  v85 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v85;
label5:
  iree_vm_ref_release(v3);
  return v80;
}

static iree_status_t be_u1000_hello_world_float_call_0rriiCID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8, int64_t v9) {
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
  iree_host_size_t v23;
  iree_host_size_t v24;
  iree_host_size_t v25;
  iree_vm_function_t* v26;
  iree_vm_module_t* v27;
  bool v28;
  iree_vm_function_call_t v29;
  iree_vm_function_t v30;
  void* v31;
  uint8_t* v32;
  void* v33;
  uint8_t* v34;
  uint8_t* v35;
  iree_vm_ref_t* v36;
  iree_host_size_t v37;
  uint8_t* v38;
  iree_vm_ref_t* v39;
  iree_host_size_t v40;
  uint8_t* v41;
  int32_t v42;
  iree_host_size_t v43;
  int32_t* v44;
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
  iree_host_size_t v55;
  uint8_t* v56;
  int64_t v57;
  iree_host_size_t v58;
  int64_t* v59;
  iree_host_size_t v60;
  uint8_t* v61;
  int64_t v62;
  iree_host_size_t v63;
  int64_t* v64;
  begin_call_t v65;
  iree_vm_module_t* v66;
  iree_vm_function_call_t v67;
  iree_status_t v68;
  bool v69;
  iree_status_t v70;
  iree_status_t v71;
  v10 = 1;
  v11 = 0;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v11 + v12;
  v14 = sizeof(iree_vm_ref_t);
  v15 = v13 + v14;
  v16 = sizeof(int32_t);
  v17 = v15 + v16;
  v18 = sizeof(int32_t);
  v19 = v17 + v18;
  v20 = sizeof(int32_t);
  v21 = v19 + v20;
  v22 = sizeof(int64_t);
  v23 = v21 + v22;
  v24 = sizeof(int64_t);
  v25 = v23 + v24;
  ;
  v26 = v2;
  v27 = v26->module;
  v28 = !v27;
  if (v28) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v30 = *v2;
  v29.function = v30;
  v31 = iree_alloca(v25);
  v32 = (uint8_t*) v31;
  v29.arguments.data_length = v25;
  v29.arguments.data = v32;
  memset(v32, 0, v25);
  v33 = iree_alloca(v10);
  v34 = (uint8_t*) v33;
  v29.results.data_length = v11;
  v29.results.data = v34;
  memset(v34, 0, v10);
  v35 = v29.arguments.data;
  v36 = (iree_vm_ref_t*) v35;
  iree_vm_ref_assign(v3, v36);
  v37 = sizeof(iree_vm_ref_t);
  v38 = v35 + v37;
  v39 = (iree_vm_ref_t*) v38;
  iree_vm_ref_assign(v4, v39);
  v40 = sizeof(iree_vm_ref_t);
  v41 = v38 + v40;
  ;
  v42 = v5;
  v43 = sizeof(int32_t);
  v44 = &v42;
  memcpy(v41, v44, v43);
  v45 = sizeof(int32_t);
  v46 = v41 + v45;
  ;
  v47 = v6;
  v48 = sizeof(int32_t);
  v49 = &v47;
  memcpy(v46, v49, v48);
  v50 = sizeof(int32_t);
  v51 = v46 + v50;
  ;
  v52 = v7;
  v53 = sizeof(int32_t);
  v54 = &v52;
  memcpy(v51, v54, v53);
  v55 = sizeof(int32_t);
  v56 = v51 + v55;
  ;
  v57 = v8;
  v58 = sizeof(int64_t);
  v59 = &v57;
  memcpy(v56, v59, v58);
  v60 = sizeof(int64_t);
  v61 = v56 + v60;
  ;
  v62 = v9;
  v63 = sizeof(int64_t);
  v64 = &v62;
  memcpy(v61, v64, v63);
  v65 = v26->module->begin_call;
  v66 = v26->module;
  v67 = v29;
  v68 = EMITC_CALL_INDIRECT(v65, v66, v1, v67);
  v69 = (bool) v68;
  if (v69) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v70 = iree_ok_status();
  return v70;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v71 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v71;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v68;
}

static iree_status_t be_u1000_hello_world_float_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4) {
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

static iree_status_t be_u1000_hello_world_float_call_0ri_I_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t* v5) {
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
  int32_t v26;
  iree_host_size_t v27;
  int32_t* v28;
  begin_call_t v29;
  iree_vm_module_t* v30;
  iree_vm_function_call_t v31;
  iree_status_t v32;
  bool v33;
  uint8_t* v34;
  iree_host_size_t v35;
  iree_status_t v36;
  iree_status_t v37;
  v6 = 0;
  v7 = sizeof(iree_vm_ref_t);
  v8 = v6 + v7;
  v9 = sizeof(int32_t);
  v10 = v8 + v9;
  v11 = sizeof(int64_t);
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
  v27 = sizeof(int32_t);
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
  v35 = sizeof(int64_t);
  memcpy(v5, v34, v35);
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

static iree_status_t be_u1000_hello_world_float_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8) {
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

static iree_status_t be_u1000_hello_world_float_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3) {
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

static iree_status_t be_u1000_hello_world_float_call_0riii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int32_t v6) {
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
  int32_t v40;
  iree_host_size_t v41;
  int32_t* v42;
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
  v15 = sizeof(int32_t);
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
  v41 = sizeof(int32_t);
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

static iree_status_t be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_2_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, int32_t v14, int32_t v15, iree_vm_ref_t* v16, int64_t v17, int64_t v18, int32_t v19, int32_t v20, iree_vm_ref_t* v21, int64_t v22, int64_t v23, int32_t v24, int32_t v25, iree_vm_ref_t* v26, int64_t v27, int64_t v28) {
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
  iree_host_size_t v62;
  iree_host_size_t v63;
  iree_host_size_t v64;
  iree_host_size_t v65;
  iree_host_size_t v66;
  iree_host_size_t v67;
  iree_host_size_t v68;
  iree_host_size_t v69;
  iree_host_size_t v70;
  iree_host_size_t v71;
  iree_host_size_t v72;
  iree_host_size_t v73;
  iree_host_size_t v74;
  iree_host_size_t v75;
  iree_host_size_t v76;
  iree_host_size_t v77;
  iree_host_size_t v78;
  iree_host_size_t v79;
  iree_host_size_t v80;
  iree_host_size_t v81;
  iree_host_size_t v82;
  iree_vm_function_t* v83;
  iree_vm_module_t* v84;
  bool v85;
  iree_vm_function_call_t v86;
  iree_vm_function_t v87;
  void* v88;
  uint8_t* v89;
  void* v90;
  uint8_t* v91;
  uint8_t* v92;
  iree_vm_ref_t* v93;
  iree_host_size_t v94;
  uint8_t* v95;
  iree_vm_ref_t* v96;
  iree_host_size_t v97;
  uint8_t* v98;
  int32_t v99;
  iree_host_size_t v100;
  int32_t* v101;
  iree_host_size_t v102;
  uint8_t* v103;
  int32_t v104;
  iree_host_size_t v105;
  int32_t* v106;
  iree_host_size_t v107;
  uint8_t* v108;
  int32_t v109;
  iree_host_size_t v110;
  int32_t* v111;
  iree_host_size_t v112;
  uint8_t* v113;
  int32_t v114;
  iree_host_size_t v115;
  int32_t* v116;
  iree_host_size_t v117;
  uint8_t* v118;
  int64_t v119;
  iree_host_size_t v120;
  int64_t* v121;
  iree_host_size_t v122;
  uint8_t* v123;
  int32_t v124;
  iree_host_size_t v125;
  int32_t* v126;
  iree_host_size_t v127;
  uint8_t* v128;
  int32_t v129;
  iree_host_size_t v130;
  int32_t* v131;
  iree_host_size_t v132;
  uint8_t* v133;
  int32_t v134;
  iree_host_size_t v135;
  int32_t* v136;
  iree_host_size_t v137;
  uint8_t* v138;
  int32_t v139;
  iree_host_size_t v140;
  int32_t* v141;
  iree_host_size_t v142;
  uint8_t* v143;
  int32_t v144;
  iree_host_size_t v145;
  int32_t* v146;
  iree_host_size_t v147;
  uint8_t* v148;
  int32_t v149;
  iree_host_size_t v150;
  int32_t* v151;
  iree_host_size_t v152;
  uint8_t* v153;
  iree_vm_ref_t* v154;
  iree_host_size_t v155;
  uint8_t* v156;
  int64_t v157;
  iree_host_size_t v158;
  int64_t* v159;
  iree_host_size_t v160;
  uint8_t* v161;
  int64_t v162;
  iree_host_size_t v163;
  int64_t* v164;
  iree_host_size_t v165;
  uint8_t* v166;
  int32_t v167;
  iree_host_size_t v168;
  int32_t* v169;
  iree_host_size_t v170;
  uint8_t* v171;
  int32_t v172;
  iree_host_size_t v173;
  int32_t* v174;
  iree_host_size_t v175;
  uint8_t* v176;
  iree_vm_ref_t* v177;
  iree_host_size_t v178;
  uint8_t* v179;
  int64_t v180;
  iree_host_size_t v181;
  int64_t* v182;
  iree_host_size_t v183;
  uint8_t* v184;
  int64_t v185;
  iree_host_size_t v186;
  int64_t* v187;
  iree_host_size_t v188;
  uint8_t* v189;
  int32_t v190;
  iree_host_size_t v191;
  int32_t* v192;
  iree_host_size_t v193;
  uint8_t* v194;
  int32_t v195;
  iree_host_size_t v196;
  int32_t* v197;
  iree_host_size_t v198;
  uint8_t* v199;
  iree_vm_ref_t* v200;
  iree_host_size_t v201;
  uint8_t* v202;
  int64_t v203;
  iree_host_size_t v204;
  int64_t* v205;
  iree_host_size_t v206;
  uint8_t* v207;
  int64_t v208;
  iree_host_size_t v209;
  int64_t* v210;
  begin_call_t v211;
  iree_vm_module_t* v212;
  iree_vm_function_call_t v213;
  iree_status_t v214;
  bool v215;
  iree_status_t v216;
  iree_status_t v217;
  v29 = 1;
  v30 = 0;
  v31 = sizeof(iree_vm_ref_t);
  v32 = v30 + v31;
  v33 = sizeof(iree_vm_ref_t);
  v34 = v32 + v33;
  v35 = sizeof(int32_t);
  v36 = v34 + v35;
  v37 = sizeof(int32_t);
  v38 = v36 + v37;
  v39 = sizeof(int32_t);
  v40 = v38 + v39;
  v41 = sizeof(int32_t);
  v42 = v40 + v41;
  v43 = sizeof(int64_t);
  v44 = v42 + v43;
  v45 = sizeof(int32_t);
  v46 = v44 + v45;
  v47 = sizeof(int32_t);
  v48 = v46 + v47;
  v49 = sizeof(int32_t);
  v50 = v48 + v49;
  v51 = sizeof(int32_t);
  v52 = v50 + v51;
  v53 = sizeof(int32_t);
  v54 = v52 + v53;
  v55 = sizeof(int32_t);
  v56 = v54 + v55;
  v57 = sizeof(iree_vm_ref_t);
  v58 = v56 + v57;
  v59 = sizeof(int64_t);
  v60 = v58 + v59;
  v61 = sizeof(int64_t);
  v62 = v60 + v61;
  v63 = sizeof(int32_t);
  v64 = v62 + v63;
  v65 = sizeof(int32_t);
  v66 = v64 + v65;
  v67 = sizeof(iree_vm_ref_t);
  v68 = v66 + v67;
  v69 = sizeof(int64_t);
  v70 = v68 + v69;
  v71 = sizeof(int64_t);
  v72 = v70 + v71;
  v73 = sizeof(int32_t);
  v74 = v72 + v73;
  v75 = sizeof(int32_t);
  v76 = v74 + v75;
  v77 = sizeof(iree_vm_ref_t);
  v78 = v76 + v77;
  v79 = sizeof(int64_t);
  v80 = v78 + v79;
  v81 = sizeof(int64_t);
  v82 = v80 + v81;
  ;
  v83 = v2;
  v84 = v83->module;
  v85 = !v84;
  if (v85) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v87 = *v2;
  v86.function = v87;
  v88 = iree_alloca(v82);
  v89 = (uint8_t*) v88;
  v86.arguments.data_length = v82;
  v86.arguments.data = v89;
  memset(v89, 0, v82);
  v90 = iree_alloca(v29);
  v91 = (uint8_t*) v90;
  v86.results.data_length = v30;
  v86.results.data = v91;
  memset(v91, 0, v29);
  v92 = v86.arguments.data;
  v93 = (iree_vm_ref_t*) v92;
  iree_vm_ref_assign(v3, v93);
  v94 = sizeof(iree_vm_ref_t);
  v95 = v92 + v94;
  v96 = (iree_vm_ref_t*) v95;
  iree_vm_ref_assign(v4, v96);
  v97 = sizeof(iree_vm_ref_t);
  v98 = v95 + v97;
  ;
  v99 = v5;
  v100 = sizeof(int32_t);
  v101 = &v99;
  memcpy(v98, v101, v100);
  v102 = sizeof(int32_t);
  v103 = v98 + v102;
  ;
  v104 = v6;
  v105 = sizeof(int32_t);
  v106 = &v104;
  memcpy(v103, v106, v105);
  v107 = sizeof(int32_t);
  v108 = v103 + v107;
  ;
  v109 = v7;
  v110 = sizeof(int32_t);
  v111 = &v109;
  memcpy(v108, v111, v110);
  v112 = sizeof(int32_t);
  v113 = v108 + v112;
  ;
  v114 = v8;
  v115 = sizeof(int32_t);
  v116 = &v114;
  memcpy(v113, v116, v115);
  v117 = sizeof(int32_t);
  v118 = v113 + v117;
  ;
  v119 = v9;
  v120 = sizeof(int64_t);
  v121 = &v119;
  memcpy(v118, v121, v120);
  v122 = sizeof(int64_t);
  v123 = v118 + v122;
  ;
  v124 = v10;
  v125 = sizeof(int32_t);
  v126 = &v124;
  memcpy(v123, v126, v125);
  v127 = sizeof(int32_t);
  v128 = v123 + v127;
  ;
  v129 = v11;
  v130 = sizeof(int32_t);
  v131 = &v129;
  memcpy(v128, v131, v130);
  v132 = sizeof(int32_t);
  v133 = v128 + v132;
  ;
  v134 = v12;
  v135 = sizeof(int32_t);
  v136 = &v134;
  memcpy(v133, v136, v135);
  v137 = sizeof(int32_t);
  v138 = v133 + v137;
  ;
  v139 = v13;
  v140 = sizeof(int32_t);
  v141 = &v139;
  memcpy(v138, v141, v140);
  v142 = sizeof(int32_t);
  v143 = v138 + v142;
  ;
  v144 = v14;
  v145 = sizeof(int32_t);
  v146 = &v144;
  memcpy(v143, v146, v145);
  v147 = sizeof(int32_t);
  v148 = v143 + v147;
  ;
  v149 = v15;
  v150 = sizeof(int32_t);
  v151 = &v149;
  memcpy(v148, v151, v150);
  v152 = sizeof(int32_t);
  v153 = v148 + v152;
  v154 = (iree_vm_ref_t*) v153;
  iree_vm_ref_assign(v16, v154);
  v155 = sizeof(iree_vm_ref_t);
  v156 = v153 + v155;
  ;
  v157 = v17;
  v158 = sizeof(int64_t);
  v159 = &v157;
  memcpy(v156, v159, v158);
  v160 = sizeof(int64_t);
  v161 = v156 + v160;
  ;
  v162 = v18;
  v163 = sizeof(int64_t);
  v164 = &v162;
  memcpy(v161, v164, v163);
  v165 = sizeof(int64_t);
  v166 = v161 + v165;
  ;
  v167 = v19;
  v168 = sizeof(int32_t);
  v169 = &v167;
  memcpy(v166, v169, v168);
  v170 = sizeof(int32_t);
  v171 = v166 + v170;
  ;
  v172 = v20;
  v173 = sizeof(int32_t);
  v174 = &v172;
  memcpy(v171, v174, v173);
  v175 = sizeof(int32_t);
  v176 = v171 + v175;
  v177 = (iree_vm_ref_t*) v176;
  iree_vm_ref_assign(v21, v177);
  v178 = sizeof(iree_vm_ref_t);
  v179 = v176 + v178;
  ;
  v180 = v22;
  v181 = sizeof(int64_t);
  v182 = &v180;
  memcpy(v179, v182, v181);
  v183 = sizeof(int64_t);
  v184 = v179 + v183;
  ;
  v185 = v23;
  v186 = sizeof(int64_t);
  v187 = &v185;
  memcpy(v184, v187, v186);
  v188 = sizeof(int64_t);
  v189 = v184 + v188;
  ;
  v190 = v24;
  v191 = sizeof(int32_t);
  v192 = &v190;
  memcpy(v189, v192, v191);
  v193 = sizeof(int32_t);
  v194 = v189 + v193;
  ;
  v195 = v25;
  v196 = sizeof(int32_t);
  v197 = &v195;
  memcpy(v194, v197, v196);
  v198 = sizeof(int32_t);
  v199 = v194 + v198;
  v200 = (iree_vm_ref_t*) v199;
  iree_vm_ref_assign(v26, v200);
  v201 = sizeof(iree_vm_ref_t);
  v202 = v199 + v201;
  ;
  v203 = v27;
  v204 = sizeof(int64_t);
  v205 = &v203;
  memcpy(v202, v205, v204);
  v206 = sizeof(int64_t);
  v207 = v202 + v206;
  ;
  v208 = v28;
  v209 = sizeof(int64_t);
  v210 = &v208;
  memcpy(v207, v210, v209);
  v211 = v83->module->begin_call;
  v212 = v83->module;
  v213 = v86;
  v214 = EMITC_CALL_INDIRECT(v211, v212, v1, v213);
  v215 = (bool) v214;
  if (v215) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v216 = iree_ok_status();
  return v216;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v217 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v217;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v214;
}

static iree_status_t be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_4_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, int32_t v14, int32_t v15, int32_t v16, int32_t v17, iree_vm_ref_t* v18, int64_t v19, int64_t v20, int32_t v21, int32_t v22, iree_vm_ref_t* v23, int64_t v24, int64_t v25, int32_t v26, int32_t v27, iree_vm_ref_t* v28, int64_t v29, int64_t v30) {
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
  iree_host_size_t v62;
  iree_host_size_t v63;
  iree_host_size_t v64;
  iree_host_size_t v65;
  iree_host_size_t v66;
  iree_host_size_t v67;
  iree_host_size_t v68;
  iree_host_size_t v69;
  iree_host_size_t v70;
  iree_host_size_t v71;
  iree_host_size_t v72;
  iree_host_size_t v73;
  iree_host_size_t v74;
  iree_host_size_t v75;
  iree_host_size_t v76;
  iree_host_size_t v77;
  iree_host_size_t v78;
  iree_host_size_t v79;
  iree_host_size_t v80;
  iree_host_size_t v81;
  iree_host_size_t v82;
  iree_host_size_t v83;
  iree_host_size_t v84;
  iree_host_size_t v85;
  iree_host_size_t v86;
  iree_host_size_t v87;
  iree_host_size_t v88;
  iree_vm_function_t* v89;
  iree_vm_module_t* v90;
  bool v91;
  iree_vm_function_call_t v92;
  iree_vm_function_t v93;
  void* v94;
  uint8_t* v95;
  void* v96;
  uint8_t* v97;
  uint8_t* v98;
  iree_vm_ref_t* v99;
  iree_host_size_t v100;
  uint8_t* v101;
  iree_vm_ref_t* v102;
  iree_host_size_t v103;
  uint8_t* v104;
  int32_t v105;
  iree_host_size_t v106;
  int32_t* v107;
  iree_host_size_t v108;
  uint8_t* v109;
  int32_t v110;
  iree_host_size_t v111;
  int32_t* v112;
  iree_host_size_t v113;
  uint8_t* v114;
  int32_t v115;
  iree_host_size_t v116;
  int32_t* v117;
  iree_host_size_t v118;
  uint8_t* v119;
  int32_t v120;
  iree_host_size_t v121;
  int32_t* v122;
  iree_host_size_t v123;
  uint8_t* v124;
  int64_t v125;
  iree_host_size_t v126;
  int64_t* v127;
  iree_host_size_t v128;
  uint8_t* v129;
  int32_t v130;
  iree_host_size_t v131;
  int32_t* v132;
  iree_host_size_t v133;
  uint8_t* v134;
  int32_t v135;
  iree_host_size_t v136;
  int32_t* v137;
  iree_host_size_t v138;
  uint8_t* v139;
  int32_t v140;
  iree_host_size_t v141;
  int32_t* v142;
  iree_host_size_t v143;
  uint8_t* v144;
  int32_t v145;
  iree_host_size_t v146;
  int32_t* v147;
  iree_host_size_t v148;
  uint8_t* v149;
  int32_t v150;
  iree_host_size_t v151;
  int32_t* v152;
  iree_host_size_t v153;
  uint8_t* v154;
  int32_t v155;
  iree_host_size_t v156;
  int32_t* v157;
  iree_host_size_t v158;
  uint8_t* v159;
  int32_t v160;
  iree_host_size_t v161;
  int32_t* v162;
  iree_host_size_t v163;
  uint8_t* v164;
  int32_t v165;
  iree_host_size_t v166;
  int32_t* v167;
  iree_host_size_t v168;
  uint8_t* v169;
  iree_vm_ref_t* v170;
  iree_host_size_t v171;
  uint8_t* v172;
  int64_t v173;
  iree_host_size_t v174;
  int64_t* v175;
  iree_host_size_t v176;
  uint8_t* v177;
  int64_t v178;
  iree_host_size_t v179;
  int64_t* v180;
  iree_host_size_t v181;
  uint8_t* v182;
  int32_t v183;
  iree_host_size_t v184;
  int32_t* v185;
  iree_host_size_t v186;
  uint8_t* v187;
  int32_t v188;
  iree_host_size_t v189;
  int32_t* v190;
  iree_host_size_t v191;
  uint8_t* v192;
  iree_vm_ref_t* v193;
  iree_host_size_t v194;
  uint8_t* v195;
  int64_t v196;
  iree_host_size_t v197;
  int64_t* v198;
  iree_host_size_t v199;
  uint8_t* v200;
  int64_t v201;
  iree_host_size_t v202;
  int64_t* v203;
  iree_host_size_t v204;
  uint8_t* v205;
  int32_t v206;
  iree_host_size_t v207;
  int32_t* v208;
  iree_host_size_t v209;
  uint8_t* v210;
  int32_t v211;
  iree_host_size_t v212;
  int32_t* v213;
  iree_host_size_t v214;
  uint8_t* v215;
  iree_vm_ref_t* v216;
  iree_host_size_t v217;
  uint8_t* v218;
  int64_t v219;
  iree_host_size_t v220;
  int64_t* v221;
  iree_host_size_t v222;
  uint8_t* v223;
  int64_t v224;
  iree_host_size_t v225;
  int64_t* v226;
  begin_call_t v227;
  iree_vm_module_t* v228;
  iree_vm_function_call_t v229;
  iree_status_t v230;
  bool v231;
  iree_status_t v232;
  iree_status_t v233;
  v31 = 1;
  v32 = 0;
  v33 = sizeof(iree_vm_ref_t);
  v34 = v32 + v33;
  v35 = sizeof(iree_vm_ref_t);
  v36 = v34 + v35;
  v37 = sizeof(int32_t);
  v38 = v36 + v37;
  v39 = sizeof(int32_t);
  v40 = v38 + v39;
  v41 = sizeof(int32_t);
  v42 = v40 + v41;
  v43 = sizeof(int32_t);
  v44 = v42 + v43;
  v45 = sizeof(int64_t);
  v46 = v44 + v45;
  v47 = sizeof(int32_t);
  v48 = v46 + v47;
  v49 = sizeof(int32_t);
  v50 = v48 + v49;
  v51 = sizeof(int32_t);
  v52 = v50 + v51;
  v53 = sizeof(int32_t);
  v54 = v52 + v53;
  v55 = sizeof(int32_t);
  v56 = v54 + v55;
  v57 = sizeof(int32_t);
  v58 = v56 + v57;
  v59 = sizeof(int32_t);
  v60 = v58 + v59;
  v61 = sizeof(int32_t);
  v62 = v60 + v61;
  v63 = sizeof(iree_vm_ref_t);
  v64 = v62 + v63;
  v65 = sizeof(int64_t);
  v66 = v64 + v65;
  v67 = sizeof(int64_t);
  v68 = v66 + v67;
  v69 = sizeof(int32_t);
  v70 = v68 + v69;
  v71 = sizeof(int32_t);
  v72 = v70 + v71;
  v73 = sizeof(iree_vm_ref_t);
  v74 = v72 + v73;
  v75 = sizeof(int64_t);
  v76 = v74 + v75;
  v77 = sizeof(int64_t);
  v78 = v76 + v77;
  v79 = sizeof(int32_t);
  v80 = v78 + v79;
  v81 = sizeof(int32_t);
  v82 = v80 + v81;
  v83 = sizeof(iree_vm_ref_t);
  v84 = v82 + v83;
  v85 = sizeof(int64_t);
  v86 = v84 + v85;
  v87 = sizeof(int64_t);
  v88 = v86 + v87;
  ;
  v89 = v2;
  v90 = v89->module;
  v91 = !v90;
  if (v91) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v93 = *v2;
  v92.function = v93;
  v94 = iree_alloca(v88);
  v95 = (uint8_t*) v94;
  v92.arguments.data_length = v88;
  v92.arguments.data = v95;
  memset(v95, 0, v88);
  v96 = iree_alloca(v31);
  v97 = (uint8_t*) v96;
  v92.results.data_length = v32;
  v92.results.data = v97;
  memset(v97, 0, v31);
  v98 = v92.arguments.data;
  v99 = (iree_vm_ref_t*) v98;
  iree_vm_ref_assign(v3, v99);
  v100 = sizeof(iree_vm_ref_t);
  v101 = v98 + v100;
  v102 = (iree_vm_ref_t*) v101;
  iree_vm_ref_assign(v4, v102);
  v103 = sizeof(iree_vm_ref_t);
  v104 = v101 + v103;
  ;
  v105 = v5;
  v106 = sizeof(int32_t);
  v107 = &v105;
  memcpy(v104, v107, v106);
  v108 = sizeof(int32_t);
  v109 = v104 + v108;
  ;
  v110 = v6;
  v111 = sizeof(int32_t);
  v112 = &v110;
  memcpy(v109, v112, v111);
  v113 = sizeof(int32_t);
  v114 = v109 + v113;
  ;
  v115 = v7;
  v116 = sizeof(int32_t);
  v117 = &v115;
  memcpy(v114, v117, v116);
  v118 = sizeof(int32_t);
  v119 = v114 + v118;
  ;
  v120 = v8;
  v121 = sizeof(int32_t);
  v122 = &v120;
  memcpy(v119, v122, v121);
  v123 = sizeof(int32_t);
  v124 = v119 + v123;
  ;
  v125 = v9;
  v126 = sizeof(int64_t);
  v127 = &v125;
  memcpy(v124, v127, v126);
  v128 = sizeof(int64_t);
  v129 = v124 + v128;
  ;
  v130 = v10;
  v131 = sizeof(int32_t);
  v132 = &v130;
  memcpy(v129, v132, v131);
  v133 = sizeof(int32_t);
  v134 = v129 + v133;
  ;
  v135 = v11;
  v136 = sizeof(int32_t);
  v137 = &v135;
  memcpy(v134, v137, v136);
  v138 = sizeof(int32_t);
  v139 = v134 + v138;
  ;
  v140 = v12;
  v141 = sizeof(int32_t);
  v142 = &v140;
  memcpy(v139, v142, v141);
  v143 = sizeof(int32_t);
  v144 = v139 + v143;
  ;
  v145 = v13;
  v146 = sizeof(int32_t);
  v147 = &v145;
  memcpy(v144, v147, v146);
  v148 = sizeof(int32_t);
  v149 = v144 + v148;
  ;
  v150 = v14;
  v151 = sizeof(int32_t);
  v152 = &v150;
  memcpy(v149, v152, v151);
  v153 = sizeof(int32_t);
  v154 = v149 + v153;
  ;
  v155 = v15;
  v156 = sizeof(int32_t);
  v157 = &v155;
  memcpy(v154, v157, v156);
  v158 = sizeof(int32_t);
  v159 = v154 + v158;
  ;
  v160 = v16;
  v161 = sizeof(int32_t);
  v162 = &v160;
  memcpy(v159, v162, v161);
  v163 = sizeof(int32_t);
  v164 = v159 + v163;
  ;
  v165 = v17;
  v166 = sizeof(int32_t);
  v167 = &v165;
  memcpy(v164, v167, v166);
  v168 = sizeof(int32_t);
  v169 = v164 + v168;
  v170 = (iree_vm_ref_t*) v169;
  iree_vm_ref_assign(v18, v170);
  v171 = sizeof(iree_vm_ref_t);
  v172 = v169 + v171;
  ;
  v173 = v19;
  v174 = sizeof(int64_t);
  v175 = &v173;
  memcpy(v172, v175, v174);
  v176 = sizeof(int64_t);
  v177 = v172 + v176;
  ;
  v178 = v20;
  v179 = sizeof(int64_t);
  v180 = &v178;
  memcpy(v177, v180, v179);
  v181 = sizeof(int64_t);
  v182 = v177 + v181;
  ;
  v183 = v21;
  v184 = sizeof(int32_t);
  v185 = &v183;
  memcpy(v182, v185, v184);
  v186 = sizeof(int32_t);
  v187 = v182 + v186;
  ;
  v188 = v22;
  v189 = sizeof(int32_t);
  v190 = &v188;
  memcpy(v187, v190, v189);
  v191 = sizeof(int32_t);
  v192 = v187 + v191;
  v193 = (iree_vm_ref_t*) v192;
  iree_vm_ref_assign(v23, v193);
  v194 = sizeof(iree_vm_ref_t);
  v195 = v192 + v194;
  ;
  v196 = v24;
  v197 = sizeof(int64_t);
  v198 = &v196;
  memcpy(v195, v198, v197);
  v199 = sizeof(int64_t);
  v200 = v195 + v199;
  ;
  v201 = v25;
  v202 = sizeof(int64_t);
  v203 = &v201;
  memcpy(v200, v203, v202);
  v204 = sizeof(int64_t);
  v205 = v200 + v204;
  ;
  v206 = v26;
  v207 = sizeof(int32_t);
  v208 = &v206;
  memcpy(v205, v208, v207);
  v209 = sizeof(int32_t);
  v210 = v205 + v209;
  ;
  v211 = v27;
  v212 = sizeof(int32_t);
  v213 = &v211;
  memcpy(v210, v213, v212);
  v214 = sizeof(int32_t);
  v215 = v210 + v214;
  v216 = (iree_vm_ref_t*) v215;
  iree_vm_ref_assign(v28, v216);
  v217 = sizeof(iree_vm_ref_t);
  v218 = v215 + v217;
  ;
  v219 = v29;
  v220 = sizeof(int64_t);
  v221 = &v219;
  memcpy(v218, v221, v220);
  v222 = sizeof(int64_t);
  v223 = v218 + v222;
  ;
  v224 = v30;
  v225 = sizeof(int64_t);
  v226 = &v224;
  memcpy(v223, v226, v225);
  v227 = v89->module->begin_call;
  v228 = v89->module;
  v229 = v92;
  v230 = EMITC_CALL_INDIRECT(v227, v228, v1, v229);
  v231 = (bool) v230;
  if (v231) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v232 = iree_ok_status();
  return v232;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v233 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v233;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v230;
}

static iree_status_t be_u1000_hello_world_float_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7) {
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

static iree_status_t be_u1000_hello_world_float_call_0rIrriiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int32_t v7, int32_t v8, int32_t v9, int64_t v10, iree_vm_ref_t* v11) {
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
  iree_host_size_t v28;
  iree_host_size_t v29;
  iree_host_size_t v30;
  iree_vm_function_t* v31;
  iree_vm_module_t* v32;
  bool v33;
  iree_vm_function_call_t v34;
  iree_vm_function_t v35;
  void* v36;
  uint8_t* v37;
  void* v38;
  uint8_t* v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  int64_t v44;
  iree_host_size_t v45;
  int64_t* v46;
  iree_host_size_t v47;
  uint8_t* v48;
  iree_vm_ref_t* v49;
  iree_host_size_t v50;
  uint8_t* v51;
  iree_vm_ref_t* v52;
  iree_host_size_t v53;
  uint8_t* v54;
  int32_t v55;
  iree_host_size_t v56;
  int32_t* v57;
  iree_host_size_t v58;
  uint8_t* v59;
  int32_t v60;
  iree_host_size_t v61;
  int32_t* v62;
  iree_host_size_t v63;
  uint8_t* v64;
  int32_t v65;
  iree_host_size_t v66;
  int32_t* v67;
  iree_host_size_t v68;
  uint8_t* v69;
  int64_t v70;
  iree_host_size_t v71;
  int64_t* v72;
  begin_call_t v73;
  iree_vm_module_t* v74;
  iree_vm_function_call_t v75;
  iree_status_t v76;
  bool v77;
  uint8_t* v78;
  iree_vm_ref_t* v79;
  iree_status_t v80;
  iree_status_t v81;
  v12 = 0;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(int64_t);
  v16 = v14 + v15;
  v17 = sizeof(iree_vm_ref_t);
  v18 = v16 + v17;
  v19 = sizeof(iree_vm_ref_t);
  v20 = v18 + v19;
  v21 = sizeof(int32_t);
  v22 = v20 + v21;
  v23 = sizeof(int32_t);
  v24 = v22 + v23;
  v25 = sizeof(int32_t);
  v26 = v24 + v25;
  v27 = sizeof(int64_t);
  v28 = v26 + v27;
  v29 = sizeof(iree_vm_ref_t);
  v30 = v12 + v29;
  ;
  v31 = v2;
  v32 = v31->module;
  v33 = !v32;
  if (v33) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v35 = *v2;
  v34.function = v35;
  v36 = iree_alloca(v28);
  v37 = (uint8_t*) v36;
  v34.arguments.data_length = v28;
  v34.arguments.data = v37;
  memset(v37, 0, v28);
  v38 = iree_alloca(v30);
  v39 = (uint8_t*) v38;
  v34.results.data_length = v30;
  v34.results.data = v39;
  memset(v39, 0, v30);
  v40 = v34.arguments.data;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_assign(v3, v41);
  v42 = sizeof(iree_vm_ref_t);
  v43 = v40 + v42;
  ;
  v44 = v4;
  v45 = sizeof(int64_t);
  v46 = &v44;
  memcpy(v43, v46, v45);
  v47 = sizeof(int64_t);
  v48 = v43 + v47;
  v49 = (iree_vm_ref_t*) v48;
  iree_vm_ref_assign(v5, v49);
  v50 = sizeof(iree_vm_ref_t);
  v51 = v48 + v50;
  v52 = (iree_vm_ref_t*) v51;
  iree_vm_ref_assign(v6, v52);
  v53 = sizeof(iree_vm_ref_t);
  v54 = v51 + v53;
  ;
  v55 = v7;
  v56 = sizeof(int32_t);
  v57 = &v55;
  memcpy(v54, v57, v56);
  v58 = sizeof(int32_t);
  v59 = v54 + v58;
  ;
  v60 = v8;
  v61 = sizeof(int32_t);
  v62 = &v60;
  memcpy(v59, v62, v61);
  v63 = sizeof(int32_t);
  v64 = v59 + v63;
  ;
  v65 = v9;
  v66 = sizeof(int32_t);
  v67 = &v65;
  memcpy(v64, v67, v66);
  v68 = sizeof(int32_t);
  v69 = v64 + v68;
  ;
  v70 = v10;
  v71 = sizeof(int64_t);
  v72 = &v70;
  memcpy(v69, v72, v71);
  v73 = v31->module->begin_call;
  v74 = v31->module;
  v75 = v34;
  v76 = EMITC_CALL_INDIRECT(v73, v74, v1, v75);
  v77 = (bool) v76;
  if (v77) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v78 = v34.results.data;
  v79 = (iree_vm_ref_t*) v78;
  iree_vm_ref_move(v79, v11);
  v80 = iree_ok_status();
  return v80;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  v81 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v81;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  return v76;
}

static iree_status_t be_u1000_hello_world_float_call_0rIrrr_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7) {
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
  iree_host_size_t v19;
  iree_vm_function_t* v20;
  iree_vm_module_t* v21;
  bool v22;
  iree_vm_function_call_t v23;
  iree_vm_function_t v24;
  void* v25;
  uint8_t* v26;
  void* v27;
  uint8_t* v28;
  uint8_t* v29;
  iree_vm_ref_t* v30;
  iree_host_size_t v31;
  uint8_t* v32;
  int64_t v33;
  iree_host_size_t v34;
  int64_t* v35;
  iree_host_size_t v36;
  uint8_t* v37;
  iree_vm_ref_t* v38;
  iree_host_size_t v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  iree_vm_ref_t* v44;
  begin_call_t v45;
  iree_vm_module_t* v46;
  iree_vm_function_call_t v47;
  iree_status_t v48;
  bool v49;
  iree_status_t v50;
  iree_status_t v51;
  v8 = 1;
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
  ;
  v20 = v2;
  v21 = v20->module;
  v22 = !v21;
  if (v22) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v24 = *v2;
  v23.function = v24;
  v25 = iree_alloca(v19);
  v26 = (uint8_t*) v25;
  v23.arguments.data_length = v19;
  v23.arguments.data = v26;
  memset(v26, 0, v19);
  v27 = iree_alloca(v8);
  v28 = (uint8_t*) v27;
  v23.results.data_length = v9;
  v23.results.data = v28;
  memset(v28, 0, v8);
  v29 = v23.arguments.data;
  v30 = (iree_vm_ref_t*) v29;
  iree_vm_ref_assign(v3, v30);
  v31 = sizeof(iree_vm_ref_t);
  v32 = v29 + v31;
  ;
  v33 = v4;
  v34 = sizeof(int64_t);
  v35 = &v33;
  memcpy(v32, v35, v34);
  v36 = sizeof(int64_t);
  v37 = v32 + v36;
  v38 = (iree_vm_ref_t*) v37;
  iree_vm_ref_assign(v5, v38);
  v39 = sizeof(iree_vm_ref_t);
  v40 = v37 + v39;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_assign(v6, v41);
  v42 = sizeof(iree_vm_ref_t);
  v43 = v40 + v42;
  v44 = (iree_vm_ref_t*) v43;
  iree_vm_ref_assign(v7, v44);
  v45 = v20->module->begin_call;
  v46 = v20->module;
  v47 = v23;
  v48 = EMITC_CALL_INDIRECT(v45, v46, v1, v47);
  v49 = (bool) v48;
  if (v49) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v50 = iree_ok_status();
  return v50;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  v51 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v51;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  return v48;
}

static iree_status_t be_u1000_hello_world_float_call_0rIrrrIrIIi_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, iree_vm_ref_t* v9, int64_t v10, int64_t v11, int32_t v12) {
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
  iree_host_size_t v34;
  iree_vm_function_t* v35;
  iree_vm_module_t* v36;
  bool v37;
  iree_vm_function_call_t v38;
  iree_vm_function_t v39;
  void* v40;
  uint8_t* v41;
  void* v42;
  uint8_t* v43;
  uint8_t* v44;
  iree_vm_ref_t* v45;
  iree_host_size_t v46;
  uint8_t* v47;
  int64_t v48;
  iree_host_size_t v49;
  int64_t* v50;
  iree_host_size_t v51;
  uint8_t* v52;
  iree_vm_ref_t* v53;
  iree_host_size_t v54;
  uint8_t* v55;
  iree_vm_ref_t* v56;
  iree_host_size_t v57;
  uint8_t* v58;
  iree_vm_ref_t* v59;
  iree_host_size_t v60;
  uint8_t* v61;
  int64_t v62;
  iree_host_size_t v63;
  int64_t* v64;
  iree_host_size_t v65;
  uint8_t* v66;
  iree_vm_ref_t* v67;
  iree_host_size_t v68;
  uint8_t* v69;
  int64_t v70;
  iree_host_size_t v71;
  int64_t* v72;
  iree_host_size_t v73;
  uint8_t* v74;
  int64_t v75;
  iree_host_size_t v76;
  int64_t* v77;
  iree_host_size_t v78;
  uint8_t* v79;
  int32_t v80;
  iree_host_size_t v81;
  int32_t* v82;
  begin_call_t v83;
  iree_vm_module_t* v84;
  iree_vm_function_call_t v85;
  iree_status_t v86;
  bool v87;
  iree_status_t v88;
  iree_status_t v89;
  v13 = 1;
  v14 = 0;
  v15 = sizeof(iree_vm_ref_t);
  v16 = v14 + v15;
  v17 = sizeof(int64_t);
  v18 = v16 + v17;
  v19 = sizeof(iree_vm_ref_t);
  v20 = v18 + v19;
  v21 = sizeof(iree_vm_ref_t);
  v22 = v20 + v21;
  v23 = sizeof(iree_vm_ref_t);
  v24 = v22 + v23;
  v25 = sizeof(int64_t);
  v26 = v24 + v25;
  v27 = sizeof(iree_vm_ref_t);
  v28 = v26 + v27;
  v29 = sizeof(int64_t);
  v30 = v28 + v29;
  v31 = sizeof(int64_t);
  v32 = v30 + v31;
  v33 = sizeof(int32_t);
  v34 = v32 + v33;
  ;
  v35 = v2;
  v36 = v35->module;
  v37 = !v36;
  if (v37) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v39 = *v2;
  v38.function = v39;
  v40 = iree_alloca(v34);
  v41 = (uint8_t*) v40;
  v38.arguments.data_length = v34;
  v38.arguments.data = v41;
  memset(v41, 0, v34);
  v42 = iree_alloca(v13);
  v43 = (uint8_t*) v42;
  v38.results.data_length = v14;
  v38.results.data = v43;
  memset(v43, 0, v13);
  v44 = v38.arguments.data;
  v45 = (iree_vm_ref_t*) v44;
  iree_vm_ref_assign(v3, v45);
  v46 = sizeof(iree_vm_ref_t);
  v47 = v44 + v46;
  ;
  v48 = v4;
  v49 = sizeof(int64_t);
  v50 = &v48;
  memcpy(v47, v50, v49);
  v51 = sizeof(int64_t);
  v52 = v47 + v51;
  v53 = (iree_vm_ref_t*) v52;
  iree_vm_ref_assign(v5, v53);
  v54 = sizeof(iree_vm_ref_t);
  v55 = v52 + v54;
  v56 = (iree_vm_ref_t*) v55;
  iree_vm_ref_assign(v6, v56);
  v57 = sizeof(iree_vm_ref_t);
  v58 = v55 + v57;
  v59 = (iree_vm_ref_t*) v58;
  iree_vm_ref_assign(v7, v59);
  v60 = sizeof(iree_vm_ref_t);
  v61 = v58 + v60;
  ;
  v62 = v8;
  v63 = sizeof(int64_t);
  v64 = &v62;
  memcpy(v61, v64, v63);
  v65 = sizeof(int64_t);
  v66 = v61 + v65;
  v67 = (iree_vm_ref_t*) v66;
  iree_vm_ref_assign(v9, v67);
  v68 = sizeof(iree_vm_ref_t);
  v69 = v66 + v68;
  ;
  v70 = v10;
  v71 = sizeof(int64_t);
  v72 = &v70;
  memcpy(v69, v72, v71);
  v73 = sizeof(int64_t);
  v74 = v69 + v73;
  ;
  v75 = v11;
  v76 = sizeof(int64_t);
  v77 = &v75;
  memcpy(v74, v77, v76);
  v78 = sizeof(int64_t);
  v79 = v74 + v78;
  ;
  v80 = v12;
  v81 = sizeof(int32_t);
  v82 = &v80;
  memcpy(v79, v82, v81);
  v83 = v35->module->begin_call;
  v84 = v35->module;
  v85 = v38;
  v86 = EMITC_CALL_INDIRECT(v83, v84, v1, v85);
  v87 = (bool) v86;
  if (v87) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v88 = iree_ok_status();
  return v88;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  iree_vm_ref_release(v9);
  v89 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v89;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v7);
  iree_vm_ref_release(v9);
  return v86;
}

static iree_status_t be_u1000_hello_world_float_call_0rIrrCrD_v_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int32_t v7, iree_vm_ref_t* v8) {
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
  int64_t v36;
  iree_host_size_t v37;
  int64_t* v38;
  iree_host_size_t v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  uint8_t* v43;
  iree_vm_ref_t* v44;
  iree_host_size_t v45;
  uint8_t* v46;
  int32_t v47;
  iree_host_size_t v48;
  int32_t* v49;
  iree_host_size_t v50;
  uint8_t* v51;
  iree_vm_ref_t* v52;
  begin_call_t v53;
  iree_vm_module_t* v54;
  iree_vm_function_call_t v55;
  iree_status_t v56;
  bool v57;
  iree_status_t v58;
  iree_status_t v59;
  v9 = 1;
  v10 = 0;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v10 + v11;
  v13 = sizeof(int64_t);
  v14 = v12 + v13;
  v15 = sizeof(iree_vm_ref_t);
  v16 = v14 + v15;
  v17 = sizeof(iree_vm_ref_t);
  v18 = v16 + v17;
  v19 = sizeof(int32_t);
  v20 = v18 + v19;
  v21 = sizeof(iree_vm_ref_t);
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
  ;
  v36 = v4;
  v37 = sizeof(int64_t);
  v38 = &v36;
  memcpy(v35, v38, v37);
  v39 = sizeof(int64_t);
  v40 = v35 + v39;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_assign(v5, v41);
  v42 = sizeof(iree_vm_ref_t);
  v43 = v40 + v42;
  v44 = (iree_vm_ref_t*) v43;
  iree_vm_ref_assign(v6, v44);
  v45 = sizeof(iree_vm_ref_t);
  v46 = v43 + v45;
  ;
  v47 = v7;
  v48 = sizeof(int32_t);
  v49 = &v47;
  memcpy(v46, v49, v48);
  v50 = sizeof(int32_t);
  v51 = v46 + v50;
  v52 = (iree_vm_ref_t*) v51;
  iree_vm_ref_assign(v8, v52);
  v53 = v23->module->begin_call;
  v54 = v23->module;
  v55 = v26;
  v56 = EMITC_CALL_INDIRECT(v53, v54, v1, v55);
  v57 = (bool) v56;
  if (v57) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v58 = iree_ok_status();
  return v58;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v8);
  v59 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v59;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  iree_vm_ref_release(v8);
  return v56;
}

static iree_status_t be_u1000_hello_world_float_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3) {
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

static iree_status_t be_u1000_hello_world_float_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4) {
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

static iree_status_t be_u1000_hello_world_float_call_0rrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7) {
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
  iree_host_size_t v36;
  uint8_t* v37;
  iree_vm_ref_t* v38;
  begin_call_t v39;
  iree_vm_module_t* v40;
  iree_vm_function_call_t v41;
  iree_status_t v42;
  bool v43;
  uint8_t* v44;
  iree_vm_ref_t* v45;
  iree_status_t v46;
  iree_status_t v47;
  v8 = 0;
  v9 = sizeof(iree_vm_ref_t);
  v10 = v8 + v9;
  v11 = sizeof(iree_vm_ref_t);
  v12 = v10 + v11;
  v13 = sizeof(iree_vm_ref_t);
  v14 = v12 + v13;
  v15 = sizeof(iree_vm_ref_t);
  v16 = v14 + v15;
  v17 = sizeof(iree_vm_ref_t);
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
  v36 = sizeof(iree_vm_ref_t);
  v37 = v34 + v36;
  v38 = (iree_vm_ref_t*) v37;
  iree_vm_ref_assign(v6, v38);
  v39 = v19->module->begin_call;
  v40 = v19->module;
  v41 = v22;
  v42 = EMITC_CALL_INDIRECT(v39, v40, v1, v41);
  v43 = (bool) v42;
  if (v43) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v44 = v22.results.data;
  v45 = (iree_vm_ref_t*) v44;
  iree_vm_ref_move(v45, v7);
  v46 = iree_ok_status();
  return v46;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  v47 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v47;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  iree_vm_ref_release(v5);
  iree_vm_ref_release(v6);
  return v42;
}

static iree_status_t be_u1000_hello_world_float_call_0ri_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, iree_vm_ref_t* v5) {
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
  int32_t v26;
  iree_host_size_t v27;
  int32_t* v28;
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
  v9 = sizeof(int32_t);
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
  v27 = sizeof(int32_t);
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

static iree_status_t be_u1000_hello_world_float_call_0CrD_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6) {
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_vm_function_t* v16;
  iree_vm_module_t* v17;
  bool v18;
  iree_vm_function_call_t v19;
  iree_vm_function_t v20;
  void* v21;
  uint8_t* v22;
  void* v23;
  uint8_t* v24;
  uint8_t* v25;
  int32_t v26;
  iree_host_size_t v27;
  int32_t* v28;
  iree_host_size_t v29;
  uint8_t* v30;
  iree_vm_ref_t* v31;
  iree_host_size_t v32;
  uint8_t* v33;
  iree_vm_ref_t* v34;
  begin_call_t v35;
  iree_vm_module_t* v36;
  iree_vm_function_call_t v37;
  iree_status_t v38;
  bool v39;
  uint8_t* v40;
  iree_vm_ref_t* v41;
  iree_status_t v42;
  iree_status_t v43;
  v7 = 0;
  v8 = sizeof(int32_t);
  v9 = v7 + v8;
  v10 = sizeof(iree_vm_ref_t);
  v11 = v9 + v10;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v11 + v12;
  v14 = sizeof(iree_vm_ref_t);
  v15 = v7 + v14;
  ;
  v16 = v2;
  v17 = v16->module;
  v18 = !v17;
  if (v18) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v20 = *v2;
  v19.function = v20;
  v21 = iree_alloca(v13);
  v22 = (uint8_t*) v21;
  v19.arguments.data_length = v13;
  v19.arguments.data = v22;
  memset(v22, 0, v13);
  v23 = iree_alloca(v15);
  v24 = (uint8_t*) v23;
  v19.results.data_length = v15;
  v19.results.data = v24;
  memset(v24, 0, v15);
  v25 = v19.arguments.data;
  ;
  v26 = v3;
  v27 = sizeof(int32_t);
  v28 = &v26;
  memcpy(v25, v28, v27);
  v29 = sizeof(int32_t);
  v30 = v25 + v29;
  v31 = (iree_vm_ref_t*) v30;
  iree_vm_ref_assign(v4, v31);
  v32 = sizeof(iree_vm_ref_t);
  v33 = v30 + v32;
  v34 = (iree_vm_ref_t*) v33;
  iree_vm_ref_assign(v5, v34);
  v35 = v16->module->begin_call;
  v36 = v16->module;
  v37 = v19;
  v38 = EMITC_CALL_INDIRECT(v35, v36, v1, v37);
  v39 = (bool) v38;
  if (v39) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v40 = v19.results.data;
  v41 = (iree_vm_ref_t*) v40;
  iree_vm_ref_move(v41, v6);
  v42 = iree_ok_status();
  return v42;
label4:
  iree_vm_ref_release(v4);
  v43 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v43;
label5:
  iree_vm_ref_release(v4);
  return v38;
}

static iree_status_t be_u1000_hello_world_float_call_0iCrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int32_t v4, iree_vm_ref_t* v5, int32_t* v6) {
  iree_host_size_t v7;
  iree_host_size_t v8;
  iree_host_size_t v9;
  iree_host_size_t v10;
  iree_host_size_t v11;
  iree_host_size_t v12;
  iree_host_size_t v13;
  iree_host_size_t v14;
  iree_host_size_t v15;
  iree_vm_function_t* v16;
  iree_vm_module_t* v17;
  bool v18;
  iree_vm_function_call_t v19;
  iree_vm_function_t v20;
  void* v21;
  uint8_t* v22;
  void* v23;
  uint8_t* v24;
  uint8_t* v25;
  int32_t v26;
  iree_host_size_t v27;
  int32_t* v28;
  iree_host_size_t v29;
  uint8_t* v30;
  int32_t v31;
  iree_host_size_t v32;
  int32_t* v33;
  iree_host_size_t v34;
  uint8_t* v35;
  iree_vm_ref_t* v36;
  begin_call_t v37;
  iree_vm_module_t* v38;
  iree_vm_function_call_t v39;
  iree_status_t v40;
  bool v41;
  uint8_t* v42;
  iree_host_size_t v43;
  iree_status_t v44;
  iree_status_t v45;
  v7 = 0;
  v8 = sizeof(int32_t);
  v9 = v7 + v8;
  v10 = sizeof(int32_t);
  v11 = v9 + v10;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v11 + v12;
  v14 = sizeof(int32_t);
  v15 = v7 + v14;
  ;
  v16 = v2;
  v17 = v16->module;
  v18 = !v17;
  if (v18) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v20 = *v2;
  v19.function = v20;
  v21 = iree_alloca(v13);
  v22 = (uint8_t*) v21;
  v19.arguments.data_length = v13;
  v19.arguments.data = v22;
  memset(v22, 0, v13);
  v23 = iree_alloca(v15);
  v24 = (uint8_t*) v23;
  v19.results.data_length = v15;
  v19.results.data = v24;
  memset(v24, 0, v15);
  v25 = v19.arguments.data;
  ;
  v26 = v3;
  v27 = sizeof(int32_t);
  v28 = &v26;
  memcpy(v25, v28, v27);
  v29 = sizeof(int32_t);
  v30 = v25 + v29;
  ;
  v31 = v4;
  v32 = sizeof(int32_t);
  v33 = &v31;
  memcpy(v30, v33, v32);
  v34 = sizeof(int32_t);
  v35 = v30 + v34;
  v36 = (iree_vm_ref_t*) v35;
  iree_vm_ref_assign(v5, v36);
  v37 = v16->module->begin_call;
  v38 = v16->module;
  v39 = v19;
  v40 = EMITC_CALL_INDIRECT(v37, v38, v1, v39);
  v41 = (bool) v40;
  if (v41) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v42 = v19.results.data;
  v43 = sizeof(int32_t);
  memcpy(v6, v42, v43);
  v44 = iree_ok_status();
  return v44;
label4:
  iree_vm_ref_release(v5);
  v45 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v45;
label5:
  iree_vm_ref_release(v5);
  return v40;
}

static iree_status_t be_u1000_hello_world_float_main(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5) {
  int32_t v6;
  int32_t v7;
  int32_t v8;
  int32_t v9;
  int32_t v10;
  int64_t v11;
  int64_t v12;
  int32_t v13;
  int32_t v14;
  int64_t v15;
  int32_t v16;
  int32_t v17;
  int32_t v18;
  int32_t v19;
  int32_t v20;
  int64_t v21;
  int32_t v22;
  int32_t v23;
  iree_vm_ref_t v24;
  iree_vm_ref_t* v25;
  iree_host_size_t v26;
  iree_vm_ref_t v27;
  iree_vm_ref_t* v28;
  iree_host_size_t v29;
  iree_vm_ref_t v30;
  iree_vm_ref_t* v31;
  iree_host_size_t v32;
  iree_vm_ref_t v33;
  iree_vm_ref_t* v34;
  iree_host_size_t v35;
  iree_vm_ref_t v36;
  iree_vm_ref_t* v37;
  iree_host_size_t v38;
  iree_vm_ref_t v39;
  iree_vm_ref_t* v40;
  iree_host_size_t v41;
  iree_vm_ref_t v42;
  iree_vm_ref_t* v43;
  iree_host_size_t v44;
  struct be_u1000_hello_world_float_state_t* v45;
  iree_vm_ref_t* v46;
  iree_vm_ref_t* v47;
  struct be_u1000_hello_world_float_t* v48;
  iree_vm_ref_type_t* v49;
  iree_vm_ref_type_t v50;
  iree_vm_type_def_t v51;
  iree_vm_ref_type_t v52;
  iree_status_t v53;
  bool v54;
  struct be_u1000_hello_world_float_state_t* v55;
  iree_vm_ref_t* v56;
  iree_vm_ref_t* v57;
  struct be_u1000_hello_world_float_t* v58;
  iree_vm_ref_type_t* v59;
  iree_vm_ref_type_t v60;
  iree_vm_type_def_t v61;
  iree_vm_ref_type_t v62;
  iree_status_t v63;
  bool v64;
  struct be_u1000_hello_world_float_state_t* v65;
  iree_vm_ref_t* v66;
  iree_vm_ref_t* v67;
  struct be_u1000_hello_world_float_t* v68;
  iree_vm_ref_type_t* v69;
  iree_vm_ref_type_t v70;
  iree_vm_type_def_t v71;
  iree_vm_ref_type_t v72;
  iree_status_t v73;
  bool v74;
  struct be_u1000_hello_world_float_state_t* v75;
  iree_vm_ref_t* v76;
  iree_vm_ref_t* v77;
  struct be_u1000_hello_world_float_t* v78;
  iree_vm_ref_type_t* v79;
  iree_vm_ref_type_t v80;
  iree_vm_type_def_t v81;
  iree_vm_ref_type_t v82;
  iree_status_t v83;
  bool v84;
  struct be_u1000_hello_world_float_state_t* v85;
  iree_vm_function_t* v86;
  iree_vm_function_t* v87;
  iree_vm_ref_t v88;
  iree_vm_ref_t* v89;
  iree_host_size_t v90;
  int64_t v91;
  int64_t* v92;
  iree_status_t v93;
  bool v94;
  int64_t v95;
  int32_t v96;
  struct be_u1000_hello_world_float_state_t* v97;
  iree_vm_buffer_t* v98;
  iree_vm_buffer_t* v99;
  iree_vm_ref_type_t v100;
  iree_status_t v101;
  bool v102;
  int64_t v103;
  struct be_u1000_hello_world_float_state_t* v104;
  iree_vm_function_t* v105;
  iree_vm_function_t* v106;
  iree_vm_ref_t v107;
  iree_vm_ref_t* v108;
  iree_host_size_t v109;
  iree_vm_ref_t v110;
  iree_vm_ref_t* v111;
  iree_host_size_t v112;
  iree_status_t v113;
  bool v114;
  int32_t v115;
  struct be_u1000_hello_world_float_state_t* v116;
  iree_vm_function_t* v117;
  iree_vm_function_t* v118;
  iree_vm_ref_t v119;
  iree_vm_ref_t* v120;
  iree_host_size_t v121;
  iree_status_t v122;
  bool v123;
  struct be_u1000_hello_world_float_state_t* v124;
  iree_vm_function_t* v125;
  iree_vm_function_t* v126;
  iree_vm_ref_t v127;
  iree_vm_ref_t* v128;
  iree_host_size_t v129;
  iree_status_t v130;
  bool v131;
  struct be_u1000_hello_world_float_state_t* v132;
  iree_vm_buffer_t* v133;
  iree_vm_buffer_t* v134;
  iree_vm_ref_type_t v135;
  iree_status_t v136;
  bool v137;
  int64_t v138;
  struct be_u1000_hello_world_float_state_t* v139;
  iree_vm_function_t* v140;
  iree_vm_function_t* v141;
  iree_vm_ref_t v142;
  iree_vm_ref_t* v143;
  iree_host_size_t v144;
  iree_vm_ref_t v145;
  iree_vm_ref_t* v146;
  iree_host_size_t v147;
  iree_vm_ref_t v148;
  iree_vm_ref_t* v149;
  iree_host_size_t v150;
  iree_status_t v151;
  bool v152;
  int32_t v153;
  struct be_u1000_hello_world_float_state_t* v154;
  iree_vm_function_t* v155;
  iree_vm_function_t* v156;
  iree_vm_ref_t v157;
  iree_vm_ref_t* v158;
  iree_host_size_t v159;
  iree_status_t v160;
  bool v161;
  struct be_u1000_hello_world_float_state_t* v162;
  iree_vm_function_t* v163;
  iree_vm_function_t* v164;
  iree_vm_ref_t v165;
  iree_vm_ref_t* v166;
  iree_host_size_t v167;
  iree_vm_ref_t v168;
  iree_vm_ref_t* v169;
  iree_host_size_t v170;
  iree_vm_ref_t v171;
  iree_vm_ref_t* v172;
  iree_host_size_t v173;
  iree_status_t v174;
  bool v175;
  int32_t v176;
  struct be_u1000_hello_world_float_state_t* v177;
  iree_vm_function_t* v178;
  iree_vm_function_t* v179;
  iree_vm_ref_t v180;
  iree_vm_ref_t* v181;
  iree_host_size_t v182;
  iree_status_t v183;
  bool v184;
  int64_t v185;
  struct be_u1000_hello_world_float_state_t* v186;
  iree_vm_function_t* v187;
  iree_vm_function_t* v188;
  iree_vm_ref_t v189;
  iree_vm_ref_t* v190;
  iree_host_size_t v191;
  iree_vm_ref_t v192;
  iree_vm_ref_t* v193;
  iree_host_size_t v194;
  iree_vm_ref_t v195;
  iree_vm_ref_t* v196;
  iree_host_size_t v197;
  iree_status_t v198;
  bool v199;
  struct be_u1000_hello_world_float_state_t* v200;
  iree_vm_function_t* v201;
  iree_vm_function_t* v202;
  iree_vm_ref_t v203;
  iree_vm_ref_t* v204;
  iree_host_size_t v205;
  iree_vm_ref_t v206;
  iree_vm_ref_t* v207;
  iree_host_size_t v208;
  iree_status_t v209;
  bool v210;
  int64_t v211;
  int64_t v212;
  int32_t v213;
  int64_t v214;
  int64_t v215;
  int32_t v216;
  struct be_u1000_hello_world_float_state_t* v217;
  iree_vm_function_t* v218;
  iree_vm_function_t* v219;
  iree_vm_ref_t v220;
  iree_vm_ref_t* v221;
  iree_host_size_t v222;
  iree_status_t v223;
  bool v224;
  int32_t v225;
  int32_t v226;
  int32_t v227;
  int32_t v228;
  int32_t v229;
  int32_t v230;
  int32_t v231;
  int32_t v232;
  int32_t v233;
  struct be_u1000_hello_world_float_state_t* v234;
  iree_vm_function_t* v235;
  iree_vm_function_t* v236;
  iree_vm_ref_t v237;
  iree_vm_ref_t* v238;
  iree_host_size_t v239;
  iree_vm_ref_t v240;
  iree_vm_ref_t* v241;
  iree_host_size_t v242;
  iree_vm_ref_t v243;
  iree_vm_ref_t* v244;
  iree_host_size_t v245;
  iree_vm_ref_t v246;
  iree_vm_ref_t* v247;
  iree_host_size_t v248;
  iree_vm_ref_t v249;
  iree_vm_ref_t* v250;
  iree_host_size_t v251;
  iree_status_t v252;
  bool v253;
  struct be_u1000_hello_world_float_state_t* v254;
  iree_vm_function_t* v255;
  iree_vm_function_t* v256;
  iree_vm_ref_t v257;
  iree_vm_ref_t* v258;
  iree_host_size_t v259;
  iree_status_t v260;
  bool v261;
  struct be_u1000_hello_world_float_state_t* v262;
  iree_vm_function_t* v263;
  iree_vm_function_t* v264;
  iree_vm_ref_t v265;
  iree_vm_ref_t* v266;
  iree_host_size_t v267;
  iree_vm_ref_t v268;
  iree_vm_ref_t* v269;
  iree_host_size_t v270;
  iree_vm_ref_t v271;
  iree_vm_ref_t* v272;
  iree_host_size_t v273;
  iree_vm_ref_t v274;
  iree_vm_ref_t* v275;
  iree_host_size_t v276;
  iree_vm_ref_t v277;
  iree_vm_ref_t* v278;
  iree_host_size_t v279;
  iree_status_t v280;
  bool v281;
  struct be_u1000_hello_world_float_state_t* v282;
  iree_vm_function_t* v283;
  iree_vm_function_t* v284;
  iree_vm_ref_t v285;
  iree_vm_ref_t* v286;
  iree_host_size_t v287;
  iree_status_t v288;
  bool v289;
  struct be_u1000_hello_world_float_state_t* v290;
  iree_vm_function_t* v291;
  iree_vm_function_t* v292;
  iree_vm_ref_t v293;
  iree_vm_ref_t* v294;
  iree_host_size_t v295;
  iree_vm_ref_t v296;
  iree_vm_ref_t* v297;
  iree_host_size_t v298;
  iree_vm_ref_t v299;
  iree_vm_ref_t* v300;
  iree_host_size_t v301;
  iree_vm_ref_t v302;
  iree_vm_ref_t* v303;
  iree_host_size_t v304;
  iree_vm_ref_t v305;
  iree_vm_ref_t* v306;
  iree_host_size_t v307;
  iree_status_t v308;
  bool v309;
  struct be_u1000_hello_world_float_state_t* v310;
  iree_vm_function_t* v311;
  iree_vm_function_t* v312;
  iree_vm_ref_t v313;
  iree_vm_ref_t* v314;
  iree_host_size_t v315;
  iree_status_t v316;
  bool v317;
  struct be_u1000_hello_world_float_state_t* v318;
  iree_vm_function_t* v319;
  iree_vm_function_t* v320;
  iree_vm_ref_t v321;
  iree_vm_ref_t* v322;
  iree_host_size_t v323;
  iree_status_t v324;
  bool v325;
  struct be_u1000_hello_world_float_state_t* v326;
  iree_vm_function_t* v327;
  iree_vm_function_t* v328;
  iree_vm_ref_t v329;
  iree_vm_ref_t* v330;
  iree_host_size_t v331;
  iree_status_t v332;
  bool v333;
  struct be_u1000_hello_world_float_state_t* v334;
  iree_vm_function_t* v335;
  iree_vm_function_t* v336;
  iree_vm_ref_t v337;
  iree_vm_ref_t* v338;
  iree_host_size_t v339;
  iree_vm_ref_t v340;
  iree_vm_ref_t* v341;
  iree_host_size_t v342;
  iree_vm_ref_t v343;
  iree_vm_ref_t* v344;
  iree_host_size_t v345;
  iree_vm_ref_t v346;
  iree_vm_ref_t* v347;
  iree_host_size_t v348;
  iree_status_t v349;
  bool v350;
  struct be_u1000_hello_world_float_state_t* v351;
  iree_vm_function_t* v352;
  iree_vm_function_t* v353;
  iree_vm_ref_t v354;
  iree_vm_ref_t* v355;
  iree_host_size_t v356;
  iree_status_t v357;
  bool v358;
  struct be_u1000_hello_world_float_state_t* v359;
  iree_vm_function_t* v360;
  iree_vm_function_t* v361;
  iree_vm_ref_t v362;
  iree_vm_ref_t* v363;
  iree_host_size_t v364;
  iree_vm_ref_t v365;
  iree_vm_ref_t* v366;
  iree_host_size_t v367;
  iree_vm_ref_t v368;
  iree_vm_ref_t* v369;
  iree_host_size_t v370;
  iree_vm_ref_t v371;
  iree_vm_ref_t* v372;
  iree_host_size_t v373;
  iree_status_t v374;
  bool v375;
  struct be_u1000_hello_world_float_state_t* v376;
  iree_vm_function_t* v377;
  iree_vm_function_t* v378;
  iree_vm_ref_t v379;
  iree_vm_ref_t* v380;
  iree_host_size_t v381;
  int32_t v382;
  int32_t* v383;
  iree_status_t v384;
  bool v385;
  int32_t v386;
  int32_t v387;
  bool v388;
  struct be_u1000_hello_world_float_state_t* v389;
  iree_vm_function_t* v390;
  iree_vm_function_t* v391;
  iree_vm_ref_t v392;
  iree_vm_ref_t* v393;
  iree_host_size_t v394;
  iree_status_t v395;
  bool v396;
  iree_vm_ref_t v397;
  iree_vm_ref_t* v398;
  iree_host_size_t v399;
  iree_status_t v400;
  bool v401;
  iree_status_t v402;
  iree_string_view_t v403;
  iree_string_view_t v404;
  iree_host_size_t v405;
  int v406;
  const char* v407;
  iree_status_t v408;
  v6 = 2;
  v7 = 0;
  v8 = 1;
  v9 = 4;
  v10 = 64;
  v11 = -1;
  v12 = 0;
  v13 = -1;
  v14 = 553648160;
  v15 = 1;
  v16 = 16;
  v17 = 3075;
  v18 = 48;
  v19 = 32;
  v20 = 3;
  v21 = 1280;
  v22 = 28;
  v23 = 13;
  ;
  v25 = &v24;
  v26 = sizeof(iree_vm_ref_t);
  memset(v25, 0, v26);
  ;
  v28 = &v27;
  v29 = sizeof(iree_vm_ref_t);
  memset(v28, 0, v29);
  ;
  v31 = &v30;
  v32 = sizeof(iree_vm_ref_t);
  memset(v31, 0, v32);
  ;
  v34 = &v33;
  v35 = sizeof(iree_vm_ref_t);
  memset(v34, 0, v35);
  ;
  v37 = &v36;
  v38 = sizeof(iree_vm_ref_t);
  memset(v37, 0, v38);
  ;
  v40 = &v39;
  v41 = sizeof(iree_vm_ref_t);
  memset(v40, 0, v41);
  ;
  v43 = &v42;
  v44 = sizeof(iree_vm_ref_t);
  memset(v43, 0, v44);
  ;
  v45 = v3;
  v46 = v45->refs;
  v47 = &v46[2];
  ;
  v48 = v2;
  v49 = v48->types;
  v50 = v49[8];
  v51 = iree_vm_make_ref_type_def(v50);
  v52 = iree_vm_type_def_as_ref(v51);
  v53 = iree_vm_ref_retain_or_move_checked(false, v47, v52, v5);
  v54 = (bool) v53;
  if (v54) {
    goto label34;
  } else {
    goto label2;
  }
label2:
  ;
  v55 = v3;
  v56 = v55->refs;
  v57 = &v56[3];
  ;
  v58 = v2;
  v59 = v58->types;
  v60 = v59[3];
  v61 = iree_vm_make_ref_type_def(v60);
  v62 = iree_vm_type_def_as_ref(v61);
  v63 = iree_vm_ref_retain_or_move_checked(false, v57, v62, v28);
  v64 = (bool) v63;
  if (v64) {
    goto label35;
  } else {
    goto label3;
  }
label3:
  ;
  v65 = v3;
  v66 = v65->refs;
  v67 = &v66[0];
  ;
  v68 = v2;
  v69 = v68->types;
  v70 = v69[6];
  v71 = iree_vm_make_ref_type_def(v70);
  v72 = iree_vm_type_def_as_ref(v71);
  v73 = iree_vm_ref_retain_or_move_checked(false, v67, v72, v31);
  v74 = (bool) v73;
  if (v74) {
    goto label36;
  } else {
    goto label4;
  }
label4:
  ;
  v75 = v3;
  v76 = v75->refs;
  v77 = &v76[1];
  ;
  v78 = v2;
  v79 = v78->types;
  v80 = v79[7];
  v81 = iree_vm_make_ref_type_def(v80);
  v82 = iree_vm_type_def_as_ref(v81);
  v83 = iree_vm_ref_retain_or_move_checked(false, v77, v82, v34);
  v84 = (bool) v83;
  if (v84) {
    goto label37;
  } else {
    goto label5;
  }
label5:
  ;
  v85 = v3;
  v86 = v85->imports;
  v87 = &v86[7];
  ;
  v89 = &v88;
  v90 = sizeof(iree_vm_ref_t);
  memset(v89, 0, v90);
  iree_vm_ref_assign(v4, v89);
  ;
  v92 = &v91;
  v93 = be_u1000_hello_world_float_call_0ri_I_import_shim(v1, v87, v89, v7, v92);
  v94 = (bool) v93;
  if (v94) {
    goto label38;
  } else {
    goto label6;
  }
label6:
  v95 = v91;
  v96 = vm_trunc_i64i32(v95);
  ;
  v97 = v3;
  v98 = v97->rodata_buffers;
  v99 = &v98[6];
  v100 = iree_vm_buffer_type();
  v101 = iree_vm_ref_wrap_retain(v99, v100, v37);
  v102 = (bool) v101;
  if (v102) {
    goto label39;
  } else {
    goto label7;
  }
label7:
  v103 = vm_ext_i32i64s(v96);
  ;
  v104 = v3;
  v105 = v104->imports;
  v106 = &v105[5];
  ;
  v108 = &v107;
  v109 = sizeof(iree_vm_ref_t);
  memset(v108, 0, v109);
  iree_vm_ref_assign(v4, v108);
  ;
  v111 = &v110;
  v112 = sizeof(iree_vm_ref_t);
  memset(v111, 0, v112);
  iree_vm_ref_assign(v37, v111);
  v113 = be_u1000_hello_world_float_call_0rriiCID_v_2_import_shim(v1, v106, v108, v111, v14, v8, v6, v103, v15);
  v114 = (bool) v113;
  if (v114) {
    goto label40;
  } else {
    goto label8;
  }
label8:
  v115 = vm_mul_i32(v96, v9);
  ;
  v116 = v3;
  v117 = v116->imports;
  v118 = &v117[6];
  ;
  v120 = &v119;
  v121 = sizeof(iree_vm_ref_t);
  memset(v120, 0, v121);
  iree_vm_ref_assign(v4, v120);
  v122 = be_u1000_hello_world_float_call_0r_r_import_shim(v1, v118, v120, v4);
  v123 = (bool) v122;
  if (v123) {
    goto label41;
  } else {
    goto label9;
  }
label9:
  ;
  v124 = v3;
  v125 = v124->imports;
  v126 = &v125[12];
  ;
  v128 = &v127;
  v129 = sizeof(iree_vm_ref_t);
  memset(v128, 0, v129);
  iree_vm_ref_assign(v31, v128);
  v130 = be_u1000_hello_world_float_call_0r_r_import_shim(v1, v126, v128, v37);
  v131 = (bool) v130;
  if (v131) {
    goto label42;
  } else {
    goto label10;
  }
label10:
  ;
  v132 = v3;
  v133 = v132->rodata_buffers;
  v134 = &v133[7];
  v135 = iree_vm_buffer_type();
  v136 = iree_vm_ref_wrap_retain(v134, v135, v40);
  v137 = (bool) v136;
  if (v137) {
    goto label43;
  } else {
    goto label11;
  }
label11:
  v138 = vm_ext_i32i64s(v115);
  ;
  v139 = v3;
  v140 = v139->imports;
  v141 = &v140[3];
  ;
  v143 = &v142;
  v144 = sizeof(iree_vm_ref_t);
  memset(v143, 0, v144);
  iree_vm_ref_assign(v4, v143);
  ;
  v146 = &v145;
  v147 = sizeof(iree_vm_ref_t);
  memset(v146, 0, v147);
  iree_vm_ref_assign(v40, v146);
  ;
  v149 = &v148;
  v150 = sizeof(iree_vm_ref_t);
  memset(v149, 0, v150);
  iree_vm_ref_assign(v37, v149);
  v151 = be_u1000_hello_world_float_call_0rrrIii_v_import_shim(v1, v141, v143, v146, v149, v138, v16, v17);
  v152 = (bool) v151;
  if (v152) {
    goto label44;
  } else {
    goto label12;
  }
label12:
  v153 = vm_mul_i32(v96, v10);
  ;
  v154 = v3;
  v155 = v154->imports;
  v156 = &v155[21];
  ;
  v158 = &v157;
  v159 = sizeof(iree_vm_ref_t);
  memset(v158, 0, v159);
  iree_vm_ref_assign(v31, v158);
  v160 = be_u1000_hello_world_float_call_0ri_r_import_shim(v1, v156, v158, v7, v37);
  v161 = (bool) v160;
  if (v161) {
    goto label45;
  } else {
    goto label13;
  }
label13:
  ;
  v162 = v3;
  v163 = v162->imports;
  v164 = &v163[14];
  ;
  v166 = &v165;
  v167 = sizeof(iree_vm_ref_t);
  memset(v166, 0, v167);
  iree_vm_ref_assign(v31, v166);
  ;
  v169 = &v168;
  v170 = sizeof(iree_vm_ref_t);
  memset(v169, 0, v170);
  iree_vm_ref_assign(v5, v169);
  ;
  v172 = &v171;
  v173 = sizeof(iree_vm_ref_t);
  memset(v172, 0, v173);
  iree_vm_ref_assign(v37, v172);
  v174 = be_u1000_hello_world_float_call_0rIrriiiI_r_import_shim(v1, v164, v166, v11, v169, v172, v7, v18, v17, v138, v40);
  v175 = (bool) v174;
  if (v175) {
    goto label46;
  } else {
    goto label14;
  }
label14:
  v176 = vm_add_i32(v153, v153);
  ;
  v177 = v3;
  v178 = v177->imports;
  v179 = &v178[21];
  ;
  v181 = &v180;
  v182 = sizeof(iree_vm_ref_t);
  memset(v181, 0, v182);
  iree_vm_ref_assign(v31, v181);
  v183 = be_u1000_hello_world_float_call_0ri_r_import_shim(v1, v179, v181, v7, v43);
  v184 = (bool) v183;
  if (v184) {
    goto label47;
  } else {
    goto label15;
  }
label15:
  v185 = vm_ext_i32i64s(v176);
  ;
  v186 = v3;
  v187 = v186->imports;
  v188 = &v187[14];
  ;
  v190 = &v189;
  v191 = sizeof(iree_vm_ref_t);
  memset(v190, 0, v191);
  iree_vm_ref_assign(v31, v190);
  ;
  v193 = &v192;
  v194 = sizeof(iree_vm_ref_t);
  memset(v193, 0, v194);
  iree_vm_ref_assign(v5, v193);
  ;
  v196 = &v195;
  v197 = sizeof(iree_vm_ref_t);
  memset(v196, 0, v197);
  iree_vm_ref_assign(v43, v196);
  v198 = be_u1000_hello_world_float_call_0rIrriiiI_r_import_shim(v1, v188, v190, v11, v193, v196, v7, v18, v17, v185, v5);
  v199 = (bool) v198;
  if (v199) {
    goto label48;
  } else {
    goto label16;
  }
label16:
  ;
  v200 = v3;
  v201 = v200->imports;
  v202 = &v201[22];
  ;
  v204 = &v203;
  v205 = sizeof(iree_vm_ref_t);
  memset(v204, 0, v205);
  iree_vm_ref_assign(v37, v204);
  ;
  v207 = &v206;
  v208 = sizeof(iree_vm_ref_t);
  memset(v207, 0, v208);
  iree_vm_ref_assign(v43, v207);
  v209 = be_u1000_hello_world_float_call_0CrD_r_2_import_shim(v1, v202, v6, v204, v207, v37);
  v210 = (bool) v209;
  if (v210) {
    goto label49;
  } else {
    goto label17;
  }
label17:
  v211 = vm_ext_i32i64u(v96);
  v212 = vm_shr_i64u(v211, v19);
  v213 = vm_trunc_i64i32(v212);
  v214 = vm_ext_i32i64u(v153);
  v215 = vm_shr_i64u(v214, v19);
  v216 = vm_trunc_i64i32(v215);
  ;
  v217 = v3;
  v218 = v217->imports;
  v219 = &v218[8];
  ;
  v221 = &v220;
  v222 = sizeof(iree_vm_ref_t);
  memset(v221, 0, v222);
  iree_vm_ref_assign(v31, v221);
  v223 = be_u1000_hello_world_float_call_0riiIi_r_import_shim(v1, v219, v221, v8, v20, v11, v7, v43);
  v224 = (bool) v223;
  if (v224) {
    goto label50;
  } else {
    goto label18;
  }
label18:
  v225 = vm_cmp_lt_i32u(v7, v96);
  v226 = vm_xor_i32(v225, v8);
  v227 = vm_sub_i32(v7, v96);
  v228 = vm_sub_i32(v96, v8);
  v229 = vm_select_i32(v226, v227, v228);
  v230 = vm_div_i32s(v229, v10);
  v231 = vm_sub_i32(v7, v230);
  v232 = vm_add_i32(v230, v8);
  v233 = vm_select_i32(v226, v231, v232);
  ;
  v234 = v3;
  v235 = v234->imports;
  v236 = &v235[11];
  ;
  v238 = &v237;
  v239 = sizeof(iree_vm_ref_t);
  memset(v238, 0, v239);
  iree_vm_ref_assign(v43, v238);
  ;
  v241 = &v240;
  v242 = sizeof(iree_vm_ref_t);
  memset(v241, 0, v242);
  iree_vm_ref_assign(v34, v241);
  ;
  v244 = &v243;
  v245 = sizeof(iree_vm_ref_t);
  memset(v244, 0, v245);
  iree_vm_ref_assign(v4, v244);
  ;
  v247 = &v246;
  v248 = sizeof(iree_vm_ref_t);
  memset(v247, 0, v248);
  iree_vm_ref_assign(v28, v247);
  ;
  v250 = &v249;
  v251 = sizeof(iree_vm_ref_t);
  memset(v250, 0, v251);
  iree_vm_ref_assign(v5, v250);
  v252 = be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_2_3_import_shim(v1, v236, v238, v241, v7, v233, v8, v8, v12, v6, v96, v213, v20, v7, v7, v244, v12, v138, v7, v7, v247, v12, v21, v7, v7, v250, v12, v185);
  v253 = (bool) v252;
  if (v253) {
    goto label51;
  } else {
    goto label19;
  }
label19:
  ;
  v254 = v3;
  v255 = v254->imports;
  v256 = &v255[10];
  ;
  v258 = &v257;
  v259 = sizeof(iree_vm_ref_t);
  memset(v258, 0, v259);
  iree_vm_ref_assign(v43, v258);
  v260 = be_u1000_hello_world_float_call_0riii_v_import_shim(v1, v256, v258, v22, v23, v7);
  v261 = (bool) v260;
  if (v261) {
    goto label52;
  } else {
    goto label20;
  }
label20:
  ;
  v262 = v3;
  v263 = v262->imports;
  v264 = &v263[11];
  ;
  v266 = &v265;
  v267 = sizeof(iree_vm_ref_t);
  memset(v266, 0, v267);
  iree_vm_ref_assign(v43, v266);
  ;
  v269 = &v268;
  v270 = sizeof(iree_vm_ref_t);
  memset(v269, 0, v270);
  iree_vm_ref_assign(v34, v269);
  ;
  v272 = &v271;
  v273 = sizeof(iree_vm_ref_t);
  memset(v272, 0, v273);
  iree_vm_ref_assign(v5, v272);
  ;
  v275 = &v274;
  v276 = sizeof(iree_vm_ref_t);
  memset(v275, 0, v276);
  iree_vm_ref_assign(v28, v275);
  ;
  v278 = &v277;
  v279 = sizeof(iree_vm_ref_t);
  memset(v278, 0, v279);
  iree_vm_ref_assign(v5, v278);
  v280 = be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_4_3_import_shim(v1, v264, v266, v269, v8, v233, v8, v8, v12, v9, v153, v216, v96, v213, v20, v7, v7, v272, v12, v185, v7, v7, v275, v12, v21, v7, v7, v278, v12, v185);
  v281 = (bool) v280;
  if (v281) {
    goto label53;
  } else {
    goto label21;
  }
label21:
  ;
  v282 = v3;
  v283 = v282->imports;
  v284 = &v283[10];
  ;
  v286 = &v285;
  v287 = sizeof(iree_vm_ref_t);
  memset(v286, 0, v287);
  iree_vm_ref_assign(v43, v286);
  v288 = be_u1000_hello_world_float_call_0riii_v_import_shim(v1, v284, v286, v22, v23, v7);
  v289 = (bool) v288;
  if (v289) {
    goto label54;
  } else {
    goto label22;
  }
label22:
  ;
  v290 = v3;
  v291 = v290->imports;
  v292 = &v291[11];
  ;
  v294 = &v293;
  v295 = sizeof(iree_vm_ref_t);
  memset(v294, 0, v295);
  iree_vm_ref_assign(v43, v294);
  ;
  v297 = &v296;
  v298 = sizeof(iree_vm_ref_t);
  memset(v297, 0, v298);
  iree_vm_ref_assign(v34, v297);
  ;
  v300 = &v299;
  v301 = sizeof(iree_vm_ref_t);
  memset(v300, 0, v301);
  iree_vm_ref_assign(v5, v300);
  ;
  v303 = &v302;
  v304 = sizeof(iree_vm_ref_t);
  memset(v303, 0, v304);
  iree_vm_ref_assign(v28, v303);
  ;
  v306 = &v305;
  v307 = sizeof(iree_vm_ref_t);
  memset(v306, 0, v307);
  iree_vm_ref_assign(v40, v306);
  v308 = be_u1000_hello_world_float_call_0rriiiiICiDCiirIID_v_4_3_import_shim(v1, v292, v294, v297, v6, v233, v8, v8, v12, v9, v153, v216, v96, v213, v20, v7, v7, v300, v12, v185, v7, v7, v303, v12, v21, v7, v7, v306, v12, v138);
  v309 = (bool) v308;
  if (v309) {
    goto label55;
  } else {
    goto label23;
  }
label23:
  ;
  v310 = v3;
  v311 = v310->imports;
  v312 = &v311[10];
  ;
  v314 = &v313;
  v315 = sizeof(iree_vm_ref_t);
  memset(v314, 0, v315);
  iree_vm_ref_assign(v43, v314);
  v316 = be_u1000_hello_world_float_call_0riii_v_import_shim(v1, v312, v314, v22, v23, v7);
  v317 = (bool) v316;
  if (v317) {
    goto label56;
  } else {
    goto label24;
  }
label24:
  ;
  v318 = v3;
  v319 = v318->imports;
  v320 = &v319[9];
  ;
  v322 = &v321;
  v323 = sizeof(iree_vm_ref_t);
  memset(v322, 0, v323);
  iree_vm_ref_assign(v43, v322);
  v324 = be_u1000_hello_world_float_call_0r_v_import_shim(v1, v320, v322);
  v325 = (bool) v324;
  if (v325) {
    goto label57;
  } else {
    goto label25;
  }
label25:
  ;
  v326 = v3;
  v327 = v326->imports;
  v328 = &v327[21];
  ;
  v330 = &v329;
  v331 = sizeof(iree_vm_ref_t);
  memset(v330, 0, v331);
  iree_vm_ref_assign(v31, v330);
  v332 = be_u1000_hello_world_float_call_0ri_r_import_shim(v1, v328, v330, v7, v4);
  v333 = (bool) v332;
  if (v333) {
    goto label58;
  } else {
    goto label26;
  }
label26:
  ;
  v334 = v3;
  v335 = v334->imports;
  v336 = &v335[17];
  ;
  v338 = &v337;
  v339 = sizeof(iree_vm_ref_t);
  memset(v338, 0, v339);
  iree_vm_ref_assign(v31, v338);
  ;
  v341 = &v340;
  v342 = sizeof(iree_vm_ref_t);
  memset(v341, 0, v342);
  iree_vm_ref_assign(v37, v341);
  ;
  v344 = &v343;
  v345 = sizeof(iree_vm_ref_t);
  memset(v344, 0, v345);
  iree_vm_ref_assign(v4, v344);
  ;
  v347 = &v346;
  v348 = sizeof(iree_vm_ref_t);
  memset(v347, 0, v348);
  iree_vm_ref_assign(v43, v347);
  v349 = be_u1000_hello_world_float_call_0rIrrCrD_v_1_import_shim(v1, v336, v338, v11, v341, v344, v8, v347);
  v350 = (bool) v349;
  if (v350) {
    goto label59;
  } else {
    goto label27;
  }
label27:
  ;
  v351 = v3;
  v352 = v351->imports;
  v353 = &v352[21];
  ;
  v355 = &v354;
  v356 = sizeof(iree_vm_ref_t);
  memset(v355, 0, v356);
  iree_vm_ref_assign(v31, v355);
  v357 = be_u1000_hello_world_float_call_0ri_r_import_shim(v1, v353, v355, v7, v28);
  v358 = (bool) v357;
  if (v358) {
    goto label60;
  } else {
    goto label28;
  }
label28:
  ;
  v359 = v3;
  v360 = v359->imports;
  v361 = &v360[15];
  ;
  v363 = &v362;
  v364 = sizeof(iree_vm_ref_t);
  memset(v363, 0, v364);
  iree_vm_ref_assign(v31, v363);
  ;
  v366 = &v365;
  v367 = sizeof(iree_vm_ref_t);
  memset(v366, 0, v367);
  iree_vm_ref_assign(v4, v366);
  ;
  v369 = &v368;
  v370 = sizeof(iree_vm_ref_t);
  memset(v369, 0, v370);
  iree_vm_ref_assign(v28, v369);
  ;
  v372 = &v371;
  v373 = sizeof(iree_vm_ref_t);
  memset(v372, 0, v373);
  iree_vm_ref_assign(v5, v372);
  v374 = be_u1000_hello_world_float_call_0rIrrr_v_import_shim(v1, v361, v363, v11, v366, v369, v372);
  v375 = (bool) v374;
  if (v375) {
    goto label61;
  } else {
    goto label29;
  }
label29:
  ;
  v376 = v3;
  v377 = v376->imports;
  v378 = &v377[23];
  ;
  v380 = &v379;
  v381 = sizeof(iree_vm_ref_t);
  memset(v380, 0, v381);
  iree_vm_ref_assign(v28, v380);
  ;
  v383 = &v382;
  v384 = be_u1000_hello_world_float_call_0iCrD_i_1_import_shim(v1, v378, v13, v8, v380, v383);
  v385 = (bool) v384;
  if (v385) {
    goto label62;
  } else {
    goto label30;
  }
label30:
  v386 = v382;
  v387 = vm_cmp_nz_i32(v386);
  v388 = (bool) v387;
  if (v388) {
    goto label33;
  } else {
    goto label31;
  }
label31:
  ;
  v389 = v3;
  v390 = v389->imports;
  v391 = &v390[4];
  ;
  v393 = &v392;
  v394 = sizeof(iree_vm_ref_t);
  memset(v393, 0, v394);
  iree_vm_ref_assign(v40, v393);
  v395 = be_u1000_hello_world_float_call_0rIIiiCID_r_2_import_shim(v1, v391, v393, v12, v138, v14, v8, v6, v103, v15, v4);
  v396 = (bool) v395;
  if (v396) {
    goto label65;
  } else {
    goto label32;
  }
label32:
  ;
  v398 = &v397;
  v399 = sizeof(iree_vm_ref_t);
  memset(v398, 0, v399);
  iree_vm_ref_move(v4, v398);
  iree_vm_ref_move(v398, v5);
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  v400 = iree_ok_status();
  return v400;
label33:
  v401 = (bool) v386;
  if (v401) {
    goto label64;
  } else {
    goto label63;
  }
label34:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v53;
label35:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v63;
label36:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v73;
label37:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v83;
label38:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v93;
label39:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v101;
label40:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v113;
label41:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v122;
label42:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v130;
label43:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v136;
label44:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v151;
label45:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v160;
label46:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v174;
label47:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v183;
label48:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v198;
label49:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v209;
label50:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v223;
label51:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v252;
label52:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v260;
label53:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v280;
label54:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v288;
label55:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v308;
label56:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v316;
label57:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v324;
label58:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v332;
label59:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v349;
label60:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v357;
label61:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v374;
label62:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v384;
label63:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  v402 = iree_ok_status();
  return v402;
label64:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  v403 = iree_make_cstring_view("failed to wait on timepoint");
  ;
  v404 = v403;
  v405 = v404.size;
  v406 = (int) v405;
  v407 = v404.data;
  v408 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v406, v407);
  return v408;
label65:
  iree_vm_ref_release(v43);
  iree_vm_ref_release(v28);
  iree_vm_ref_release(v34);
  iree_vm_ref_release(v40);
  iree_vm_ref_release(v25);
  iree_vm_ref_release(v31);
  iree_vm_ref_release(v37);
  iree_vm_ref_release(v4);
  return v395;
}

struct be_u1000_hello_world_float_main_args_t {iree_vm_ref_t arg0;};
struct be_u1000_hello_world_float_main_result_t {iree_vm_ref_t res0;};
static iree_status_t be_u1000_hello_world_float_main_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct be_u1000_hello_world_float_t* v7;
  struct be_u1000_hello_world_float_state_t* v8;
  iree_byte_span_t v9;
  uint8_t* v10;
  struct be_u1000_hello_world_float_main_args_t* v11;
  struct be_u1000_hello_world_float_main_args_t* v12;
  iree_byte_span_t v13;
  uint8_t* v14;
  struct be_u1000_hello_world_float_main_result_t* v15;
  struct be_u1000_hello_world_float_main_result_t* v16;
  iree_vm_ref_t* v17;
  iree_vm_ref_t* v18;
  iree_status_t v19;
  bool v20;
  iree_status_t v21;
  v7 = (struct be_u1000_hello_world_float_t*) v5;
  v8 = (struct be_u1000_hello_world_float_state_t*) v6;
  ;
  v9 = v3;
  v10 = v9.data;
  v11 = (struct be_u1000_hello_world_float_main_args_t*) v10;
  ;
  v12 = v11;
  ;
  v13 = v4;
  v14 = v13.data;
  v15 = (struct be_u1000_hello_world_float_main_result_t*) v14;
  ;
  v16 = v15;
  v17 = &v12->arg0;
  iree_vm_ref_retain_inplace(v17);
  v18 = &v16->res0;
  v19 = be_u1000_hello_world_float_main(v1, v7, v8, v17, v18);
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

static iree_status_t be_u1000_hello_world_float___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct be_u1000_hello_world_float_t* v7;
  struct be_u1000_hello_world_float_state_t* v8;
  iree_status_t v9;
  bool v10;
  iree_status_t v11;
  v7 = (struct be_u1000_hello_world_float_t*) v5;
  v8 = (struct be_u1000_hello_world_float_state_t*) v6;
  v9 = be_u1000_hello_world_float___init(v1, v7, v8);
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

static iree_status_t be_u1000_hello_world_float___init(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3) {
  int32_t v4;
  int32_t v5;
  int32_t v6;
  int32_t v7;
  int64_t v8;
  int64_t v9;
  int32_t v10;
  int32_t v11;
  int32_t v12;
  int64_t v13;
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
  iree_vm_ref_t v26;
  iree_vm_ref_t* v27;
  iree_host_size_t v28;
  iree_vm_ref_t v29;
  iree_vm_ref_t* v30;
  iree_host_size_t v31;
  struct be_u1000_hello_world_float_state_t* v32;
  iree_vm_function_t* v33;
  iree_vm_function_t* v34;
  int32_t v35;
  int32_t* v36;
  iree_status_t v37;
  bool v38;
  int32_t v39;
  iree_vm_ref_t v40;
  iree_vm_ref_t* v41;
  iree_host_size_t v42;
  int32_t v43;
  int32_t v44;
  int32_t v45;
  int32_t v46;
  int32_t v47;
  bool v48;
  struct be_u1000_hello_world_float_state_t* v49;
  iree_vm_function_t* v50;
  iree_vm_function_t* v51;
  iree_status_t v52;
  bool v53;
  struct be_u1000_hello_world_float_state_t* v54;
  iree_vm_buffer_t* v55;
  iree_vm_buffer_t* v56;
  iree_vm_ref_type_t v57;
  iree_status_t v58;
  bool v59;
  struct be_u1000_hello_world_float_state_t* v60;
  iree_vm_buffer_t* v61;
  iree_vm_buffer_t* v62;
  iree_vm_ref_type_t v63;
  iree_status_t v64;
  bool v65;
  struct be_u1000_hello_world_float_state_t* v66;
  iree_vm_function_t* v67;
  iree_vm_function_t* v68;
  iree_vm_ref_t v69;
  iree_vm_ref_t* v70;
  iree_host_size_t v71;
  iree_vm_ref_t v72;
  iree_vm_ref_t* v73;
  iree_host_size_t v74;
  iree_vm_ref_t v75;
  iree_vm_ref_t* v76;
  iree_host_size_t v77;
  int32_t v78;
  int32_t* v79;
  int64_t v80;
  int64_t* v81;
  iree_status_t v82;
  bool v83;
  int32_t v84;
  int64_t v85;
  int32_t v86;
  int32_t v87;
  int32_t v88;
  bool v89;
  struct be_u1000_hello_world_float_state_t* v90;
  iree_vm_buffer_t* v91;
  iree_vm_buffer_t* v92;
  iree_vm_ref_type_t v93;
  iree_status_t v94;
  bool v95;
  struct be_u1000_hello_world_float_state_t* v96;
  iree_vm_buffer_t* v97;
  iree_vm_buffer_t* v98;
  iree_vm_ref_type_t v99;
  iree_status_t v100;
  bool v101;
  struct be_u1000_hello_world_float_state_t* v102;
  iree_vm_function_t* v103;
  iree_vm_function_t* v104;
  iree_vm_ref_t v105;
  iree_vm_ref_t* v106;
  iree_host_size_t v107;
  iree_vm_ref_t v108;
  iree_vm_ref_t* v109;
  iree_host_size_t v110;
  iree_vm_ref_t v111;
  iree_vm_ref_t* v112;
  iree_host_size_t v113;
  int32_t v114;
  int32_t* v115;
  int64_t v116;
  int64_t* v117;
  iree_status_t v118;
  bool v119;
  int32_t v120;
  int64_t v121;
  int32_t v122;
  int32_t v123;
  int32_t v124;
  int32_t v125;
  int32_t v126;
  int32_t v127;
  struct be_u1000_hello_world_float_t* v128;
  iree_vm_ref_type_t* v129;
  iree_vm_ref_type_t v130;
  iree_vm_type_def_t v131;
  iree_vm_ref_type_t v132;
  int32_t v133;
  bool v134;
  iree_status_t v135;
  bool v136;
  iree_status_t v137;
  bool v138;
  int32_t v139;
  iree_vm_ref_t v140;
  iree_vm_ref_t* v141;
  iree_host_size_t v142;
  int32_t v143;
  bool v144;
  bool v145;
  struct be_u1000_hello_world_float_state_t* v146;
  iree_vm_buffer_t* v147;
  iree_vm_buffer_t* v148;
  iree_vm_ref_type_t v149;
  iree_status_t v150;
  bool v151;
  struct be_u1000_hello_world_float_state_t* v152;
  iree_vm_buffer_t* v153;
  iree_vm_buffer_t* v154;
  iree_vm_ref_type_t v155;
  iree_status_t v156;
  bool v157;
  struct be_u1000_hello_world_float_state_t* v158;
  iree_vm_function_t* v159;
  iree_vm_function_t* v160;
  iree_vm_ref_t v161;
  iree_vm_ref_t* v162;
  iree_host_size_t v163;
  iree_vm_ref_t v164;
  iree_vm_ref_t* v165;
  iree_host_size_t v166;
  iree_vm_ref_t v167;
  iree_vm_ref_t* v168;
  iree_host_size_t v169;
  int32_t v170;
  int32_t* v171;
  int64_t v172;
  int64_t* v173;
  iree_status_t v174;
  bool v175;
  int32_t v176;
  int64_t v177;
  int32_t v178;
  int32_t v179;
  int32_t v180;
  int32_t v181;
  struct be_u1000_hello_world_float_state_t* v182;
  iree_vm_ref_t* v183;
  iree_vm_ref_t* v184;
  struct be_u1000_hello_world_float_t* v185;
  iree_vm_ref_type_t* v186;
  iree_vm_ref_type_t v187;
  iree_vm_type_def_t v188;
  iree_vm_ref_type_t v189;
  iree_status_t v190;
  bool v191;
  int32_t v192;
  bool v193;
  struct be_u1000_hello_world_float_state_t* v194;
  iree_vm_buffer_t* v195;
  iree_vm_buffer_t* v196;
  iree_vm_ref_type_t v197;
  iree_status_t v198;
  bool v199;
  struct be_u1000_hello_world_float_state_t* v200;
  iree_vm_function_t* v201;
  iree_vm_function_t* v202;
  iree_vm_ref_t v203;
  iree_vm_ref_t* v204;
  iree_host_size_t v205;
  iree_vm_ref_t v206;
  iree_vm_ref_t* v207;
  iree_host_size_t v208;
  iree_vm_ref_t v209;
  iree_vm_ref_t* v210;
  iree_host_size_t v211;
  iree_vm_ref_t v212;
  iree_vm_ref_t* v213;
  iree_host_size_t v214;
  iree_status_t v215;
  bool v216;
  struct be_u1000_hello_world_float_state_t* v217;
  iree_vm_ref_t* v218;
  iree_vm_ref_t* v219;
  struct be_u1000_hello_world_float_t* v220;
  iree_vm_ref_type_t* v221;
  iree_vm_ref_type_t v222;
  iree_vm_type_def_t v223;
  iree_vm_ref_type_t v224;
  iree_status_t v225;
  bool v226;
  struct be_u1000_hello_world_float_state_t* v227;
  iree_vm_buffer_t* v228;
  iree_vm_buffer_t* v229;
  iree_vm_ref_type_t v230;
  iree_status_t v231;
  bool v232;
  struct be_u1000_hello_world_float_state_t* v233;
  iree_vm_function_t* v234;
  iree_vm_function_t* v235;
  iree_vm_ref_t v236;
  iree_vm_ref_t* v237;
  iree_host_size_t v238;
  iree_status_t v239;
  bool v240;
  struct be_u1000_hello_world_float_state_t* v241;
  iree_vm_function_t* v242;
  iree_vm_function_t* v243;
  iree_vm_ref_t v244;
  iree_vm_ref_t* v245;
  iree_host_size_t v246;
  iree_vm_ref_t v247;
  iree_vm_ref_t* v248;
  iree_host_size_t v249;
  iree_status_t v250;
  bool v251;
  int32_t v252;
  struct be_u1000_hello_world_float_state_t* v253;
  iree_vm_ref_t* v254;
  iree_vm_ref_t* v255;
  struct be_u1000_hello_world_float_t* v256;
  iree_vm_ref_type_t* v257;
  iree_vm_ref_type_t v258;
  iree_vm_type_def_t v259;
  iree_vm_ref_type_t v260;
  iree_status_t v261;
  bool v262;
  int32_t v263;
  bool v264;
  bool v265;
  struct be_u1000_hello_world_float_state_t* v266;
  iree_vm_function_t* v267;
  iree_vm_function_t* v268;
  iree_vm_ref_t v269;
  iree_vm_ref_t* v270;
  iree_host_size_t v271;
  iree_status_t v272;
  bool v273;
  struct be_u1000_hello_world_float_state_t* v274;
  iree_vm_function_t* v275;
  iree_vm_function_t* v276;
  iree_vm_ref_t v277;
  iree_vm_ref_t* v278;
  iree_host_size_t v279;
  iree_vm_ref_t v280;
  iree_vm_ref_t* v281;
  iree_host_size_t v282;
  iree_status_t v283;
  bool v284;
  struct be_u1000_hello_world_float_state_t* v285;
  iree_vm_function_t* v286;
  iree_vm_function_t* v287;
  iree_vm_ref_t v288;
  iree_vm_ref_t* v289;
  iree_host_size_t v290;
  iree_status_t v291;
  bool v292;
  struct be_u1000_hello_world_float_state_t* v293;
  iree_vm_function_t* v294;
  iree_vm_function_t* v295;
  iree_vm_ref_t v296;
  iree_vm_ref_t* v297;
  iree_host_size_t v298;
  iree_vm_ref_t v299;
  iree_vm_ref_t* v300;
  iree_host_size_t v301;
  iree_vm_ref_t v302;
  iree_vm_ref_t* v303;
  iree_host_size_t v304;
  iree_vm_ref_t v305;
  iree_vm_ref_t* v306;
  iree_host_size_t v307;
  iree_vm_ref_t v308;
  iree_vm_ref_t* v309;
  iree_host_size_t v310;
  iree_status_t v311;
  bool v312;
  iree_vm_ref_t v313;
  iree_vm_ref_t* v314;
  iree_host_size_t v315;
  iree_vm_ref_t v316;
  iree_vm_ref_t* v317;
  iree_host_size_t v318;
  iree_vm_ref_t v319;
  iree_vm_ref_t* v320;
  iree_host_size_t v321;
  iree_vm_ref_t v322;
  iree_vm_ref_t* v323;
  iree_host_size_t v324;
  struct be_u1000_hello_world_float_state_t* v325;
  iree_vm_ref_t* v326;
  iree_vm_ref_t* v327;
  struct be_u1000_hello_world_float_t* v328;
  iree_vm_ref_type_t* v329;
  iree_vm_ref_type_t v330;
  iree_vm_type_def_t v331;
  iree_vm_ref_type_t v332;
  iree_status_t v333;
  bool v334;
  struct be_u1000_hello_world_float_state_t* v335;
  iree_vm_ref_t* v336;
  iree_vm_ref_t* v337;
  struct be_u1000_hello_world_float_t* v338;
  iree_vm_ref_type_t* v339;
  iree_vm_ref_type_t v340;
  iree_vm_type_def_t v341;
  iree_vm_ref_type_t v342;
  iree_status_t v343;
  bool v344;
  iree_status_t v345;
  iree_status_t v346;
  iree_string_view_t v347;
  iree_string_view_t v348;
  iree_host_size_t v349;
  int v350;
  const char* v351;
  iree_status_t v352;
  iree_status_t v353;
  iree_string_view_t v354;
  iree_string_view_t v355;
  iree_host_size_t v356;
  int v357;
  const char* v358;
  iree_status_t v359;
  int32_t v360;
  int32_t v361;
  int32_t v362;
  v4 = 1;
  v5 = 18;
  v6 = 14;
  v7 = -1;
  v8 = 0;
  v9 = -1;
  v10 = 0;
  v11 = 48;
  v12 = 527363;
  v13 = 1280;
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
  ;
  v27 = &v26;
  v28 = sizeof(iree_vm_ref_t);
  memset(v27, 0, v28);
  ;
  v30 = &v29;
  v31 = sizeof(iree_vm_ref_t);
  memset(v30, 0, v31);
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v21);
  ;
  v32 = v3;
  v33 = v32->imports;
  v34 = &v33[18];
  ;
  v36 = &v35;
  v37 = be_u1000_hello_world_float_call_0v_i_import_shim(v1, v34, v36);
  v38 = (bool) v37;
  if (v38) {
    goto label42;
  } else {
    goto label2;
  }
label2:
  v39 = v35;
  ;
  v41 = &v40;
  v42 = sizeof(iree_vm_ref_t);
  memset(v41, 0, v42);
  iree_vm_ref_retain(v21, v41);
  iree_vm_ref_assign(v41, v24);
  v360 = v10;
  v361 = v10;
  goto label3;
label3:
  v43 = vm_cmp_nz_ref(v24);
  v44 = vm_xor_i32(v43, v4);
  v45 = vm_cmp_lt_i32s(v360, v39);
  v46 = vm_and_i32(v44, v45);
  v47 = vm_cmp_nz_i32(v46);
  v48 = (bool) v47;
  if (v48) {
    goto label4;
  } else {
    goto label17;
  }
label4:
  ;
  v49 = v3;
  v50 = v49->imports;
  v51 = &v50[19];
  v52 = be_u1000_hello_world_float_call_0i_r_import_shim(v1, v51, v360, v24);
  v53 = (bool) v52;
  if (v53) {
    goto label43;
  } else {
    goto label5;
  }
label5:
  ;
  v54 = v3;
  v55 = v54->rodata_buffers;
  v56 = &v55[0];
  v57 = iree_vm_buffer_type();
  v58 = iree_vm_ref_wrap_retain(v56, v57, v27);
  v59 = (bool) v58;
  if (v59) {
    goto label44;
  } else {
    goto label6;
  }
label6:
  ;
  v60 = v3;
  v61 = v60->rodata_buffers;
  v62 = &v61[1];
  v63 = iree_vm_buffer_type();
  v64 = iree_vm_ref_wrap_retain(v62, v63, v30);
  v65 = (bool) v64;
  if (v65) {
    goto label45;
  } else {
    goto label7;
  }
label7:
  ;
  v66 = v3;
  v67 = v66->imports;
  v68 = &v67[13];
  ;
  v70 = &v69;
  v71 = sizeof(iree_vm_ref_t);
  memset(v70, 0, v71);
  iree_vm_ref_assign(v24, v70);
  ;
  v73 = &v72;
  v74 = sizeof(iree_vm_ref_t);
  memset(v73, 0, v74);
  iree_vm_ref_assign(v27, v73);
  ;
  v76 = &v75;
  v77 = sizeof(iree_vm_ref_t);
  memset(v76, 0, v77);
  iree_vm_ref_assign(v30, v76);
  ;
  v79 = &v78;
  ;
  v81 = &v80;
  v82 = be_u1000_hello_world_float_call_0rrr_iI_import_shim(v1, v68, v70, v73, v76, v79, v81);
  v83 = (bool) v82;
  if (v83) {
    goto label46;
  } else {
    goto label8;
  }
label8:
  v84 = v78;
  v85 = v80;
  v86 = vm_cmp_nz_i64(v85);
  v87 = vm_select_i32(v84, v86, v10);
  v88 = vm_cmp_nz_i32(v87);
  v89 = (bool) v88;
  if (v89) {
    goto label9;
  } else {
    v362 = v10;
    goto label13;
  }
label9:
  ;
  v90 = v3;
  v91 = v90->rodata_buffers;
  v92 = &v91[2];
  v93 = iree_vm_buffer_type();
  v94 = iree_vm_ref_wrap_retain(v92, v93, v27);
  v95 = (bool) v94;
  if (v95) {
    goto label47;
  } else {
    goto label10;
  }
label10:
  ;
  v96 = v3;
  v97 = v96->rodata_buffers;
  v98 = &v97[3];
  v99 = iree_vm_buffer_type();
  v100 = iree_vm_ref_wrap_retain(v98, v99, v30);
  v101 = (bool) v100;
  if (v101) {
    goto label48;
  } else {
    goto label11;
  }
label11:
  ;
  v102 = v3;
  v103 = v102->imports;
  v104 = &v103[13];
  ;
  v106 = &v105;
  v107 = sizeof(iree_vm_ref_t);
  memset(v106, 0, v107);
  iree_vm_ref_assign(v24, v106);
  ;
  v109 = &v108;
  v110 = sizeof(iree_vm_ref_t);
  memset(v109, 0, v110);
  iree_vm_ref_assign(v27, v109);
  ;
  v112 = &v111;
  v113 = sizeof(iree_vm_ref_t);
  memset(v112, 0, v113);
  iree_vm_ref_assign(v30, v112);
  ;
  v115 = &v114;
  ;
  v117 = &v116;
  v118 = be_u1000_hello_world_float_call_0rrr_iI_import_shim(v1, v104, v106, v109, v112, v115, v117);
  v119 = (bool) v118;
  if (v119) {
    goto label49;
  } else {
    goto label12;
  }
label12:
  v120 = v114;
  v121 = v116;
  v122 = vm_cmp_nz_i64(v121);
  v123 = vm_select_i32(v120, v122, v10);
  v362 = v123;
  goto label13;
label13:
  v124 = vm_cmp_eq_i32(v361, v10);
  v125 = vm_select_i32(v362, v4, v10);
  v126 = vm_add_i32(v361, v125);
  v127 = vm_and_i32(v362, v124);
  ;
  v128 = v2;
  v129 = v128->types;
  v130 = v129[6];
  v131 = iree_vm_make_ref_type_def(v130);
  v132 = iree_vm_type_def_as_ref(v131);
  v133 = vm_cmp_nz_i32(v127);
  v134 = (bool) v133;
  if (v134) {
    goto label14;
  } else {
    goto label15;
  }
label14:
  v135 = iree_vm_ref_retain_or_move_checked(false, v24, v132, v24);
  v136 = (bool) v135;
  if (v136) {
    goto label50;
  } else {
    goto label16;
  }
label15:
  v137 = iree_vm_ref_retain_or_move_checked(false, v21, v132, v24);
  v138 = (bool) v137;
  if (v138) {
    goto label51;
  } else {
    goto label16;
  }
label16:
  v139 = vm_add_i32(v360, v4);
  ;
  v141 = &v140;
  v142 = sizeof(iree_vm_ref_t);
  memset(v141, 0, v142);
  iree_vm_ref_retain(v24, v141);
  iree_vm_ref_assign(v141, v24);
  v360 = v139;
  v361 = v126;
  goto label3;
label17:
  v143 = vm_cmp_nz_i32(v44);
  v144 = (bool) v143;
  if (v144) {
    goto label18;
  } else {
    goto label19;
  }
label18:
  v145 = (bool) v5;
  if (v145) {
    goto label53;
  } else {
    goto label52;
  }
label19:
  ;
  v146 = v3;
  v147 = v146->rodata_buffers;
  v148 = &v147[2];
  v149 = iree_vm_buffer_type();
  v150 = iree_vm_ref_wrap_retain(v148, v149, v21);
  v151 = (bool) v150;
  if (v151) {
    goto label54;
  } else {
    goto label20;
  }
label20:
  ;
  v152 = v3;
  v153 = v152->rodata_buffers;
  v154 = &v153[3];
  v155 = iree_vm_buffer_type();
  v156 = iree_vm_ref_wrap_retain(v154, v155, v27);
  v157 = (bool) v156;
  if (v157) {
    goto label55;
  } else {
    goto label21;
  }
label21:
  ;
  v158 = v3;
  v159 = v158->imports;
  v160 = &v159[13];
  ;
  v162 = &v161;
  v163 = sizeof(iree_vm_ref_t);
  memset(v162, 0, v163);
  iree_vm_ref_assign(v24, v162);
  ;
  v165 = &v164;
  v166 = sizeof(iree_vm_ref_t);
  memset(v165, 0, v166);
  iree_vm_ref_assign(v21, v165);
  ;
  v168 = &v167;
  v169 = sizeof(iree_vm_ref_t);
  memset(v168, 0, v169);
  iree_vm_ref_assign(v27, v168);
  ;
  v171 = &v170;
  ;
  v173 = &v172;
  v174 = be_u1000_hello_world_float_call_0rrr_iI_import_shim(v1, v160, v162, v165, v168, v171, v173);
  v175 = (bool) v174;
  if (v175) {
    goto label56;
  } else {
    goto label22;
  }
label22:
  v176 = v170;
  v177 = v172;
  v178 = vm_cmp_nz_i64(v177);
  v179 = vm_select_i32(v176, v178, v10);
  v180 = vm_select_i32(v179, v10, v7);
  v181 = vm_cmp_eq_i32(v180, v10);
  ;
  v182 = v3;
  v183 = v182->refs;
  v184 = &v183[0];
  ;
  v185 = v2;
  v186 = v185->types;
  v187 = v186[6];
  v188 = iree_vm_make_ref_type_def(v187);
  v189 = iree_vm_type_def_as_ref(v188);
  v190 = iree_vm_ref_retain_or_move_checked(false, v24, v189, v184);
  v191 = (bool) v190;
  if (v191) {
    goto label57;
  } else {
    goto label23;
  }
label23:
  v192 = vm_cmp_nz_i32(v181);
  v193 = (bool) v192;
  if (v193) {
    goto label24;
  } else {
    goto label32;
  }
label24:
  ;
  v194 = v3;
  v195 = v194->rodata_buffers;
  v196 = &v195[4];
  v197 = iree_vm_buffer_type();
  v198 = iree_vm_ref_wrap_retain(v196, v197, v21);
  v199 = (bool) v198;
  if (v199) {
    goto label58;
  } else {
    goto label25;
  }
label25:
  ;
  v200 = v3;
  v201 = v200->imports;
  v202 = &v201[20];
  ;
  v204 = &v203;
  v205 = sizeof(iree_vm_ref_t);
  memset(v204, 0, v205);
  iree_vm_ref_assign(v24, v204);
  ;
  v207 = &v206;
  v208 = sizeof(iree_vm_ref_t);
  memset(v207, 0, v208);
  iree_vm_ref_assign(v27, v207);
  ;
  v210 = &v209;
  v211 = sizeof(iree_vm_ref_t);
  memset(v210, 0, v211);
  iree_vm_ref_assign(v21, v210);
  ;
  v213 = &v212;
  v214 = sizeof(iree_vm_ref_t);
  memset(v213, 0, v214);
  iree_vm_ref_assign(v15, v213);
  v215 = be_u1000_hello_world_float_call_0rrrr_r_import_shim(v1, v202, v204, v207, v210, v213, v15);
  v216 = (bool) v215;
  if (v216) {
    goto label59;
  } else {
    goto label26;
  }
label26:
  ;
  v217 = v3;
  v218 = v217->refs;
  v219 = &v218[0];
  ;
  v220 = v2;
  v221 = v220->types;
  v222 = v221[6];
  v223 = iree_vm_make_ref_type_def(v222);
  v224 = iree_vm_type_def_as_ref(v223);
  v225 = iree_vm_ref_retain_or_move_checked(false, v219, v224, v21);
  v226 = (bool) v225;
  if (v226) {
    goto label60;
  } else {
    goto label27;
  }
label27:
  ;
  v227 = v3;
  v228 = v227->rodata_buffers;
  v229 = &v228[5];
  v230 = iree_vm_buffer_type();
  v231 = iree_vm_ref_wrap_retain(v229, v230, v24);
  v232 = (bool) v231;
  if (v232) {
    goto label61;
  } else {
    goto label28;
  }
label28:
  ;
  v233 = v3;
  v234 = v233->imports;
  v235 = &v234[12];
  ;
  v237 = &v236;
  v238 = sizeof(iree_vm_ref_t);
  memset(v237, 0, v238);
  iree_vm_ref_assign(v21, v237);
  v239 = be_u1000_hello_world_float_call_0r_r_import_shim(v1, v235, v237, v27);
  v240 = (bool) v239;
  if (v240) {
    goto label62;
  } else {
    goto label29;
  }
label29:
  ;
  v241 = v3;
  v242 = v241->imports;
  v243 = &v242[2];
  ;
  v245 = &v244;
  v246 = sizeof(iree_vm_ref_t);
  memset(v245, 0, v246);
  iree_vm_ref_assign(v27, v245);
  ;
  v248 = &v247;
  v249 = sizeof(iree_vm_ref_t);
  memset(v248, 0, v249);
  iree_vm_ref_assign(v24, v248);
  v250 = be_u1000_hello_world_float_call_0riIiirII_r_import_shim(v1, v243, v245, v4, v9, v11, v12, v248, v8, v13, v30);
  v251 = (bool) v250;
  if (v251) {
    goto label63;
  } else {
    goto label30;
  }
label30:
  v252 = vm_cmp_nz_ref(v30);
  ;
  v253 = v3;
  v254 = v253->refs;
  v255 = &v254[1];
  ;
  v256 = v2;
  v257 = v256->types;
  v258 = v257[7];
  v259 = iree_vm_make_ref_type_def(v258);
  v260 = iree_vm_type_def_as_ref(v259);
  v261 = iree_vm_ref_retain_or_move_checked(false, v15, v260, v255);
  v262 = (bool) v261;
  if (v262) {
    goto label64;
  } else {
    goto label31;
  }
label31:
  v263 = vm_cmp_nz_i32(v252);
  v264 = (bool) v263;
  if (v264) {
    goto label37;
  } else {
    goto label33;
  }
label32:
  v265 = (bool) v6;
  if (v265) {
    goto label72;
  } else {
    goto label71;
  }
label33:
  ;
  v266 = v3;
  v267 = v266->imports;
  v268 = &v267[1];
  ;
  v270 = &v269;
  v271 = sizeof(iree_vm_ref_t);
  memset(v270, 0, v271);
  iree_vm_ref_assign(v27, v270);
  v272 = be_u1000_hello_world_float_call_0rIiiI_r_import_shim(v1, v268, v270, v9, v11, v12, v13, v15);
  v273 = (bool) v272;
  if (v273) {
    goto label67;
  } else {
    goto label34;
  }
label34:
  ;
  v274 = v3;
  v275 = v274->imports;
  v276 = &v275[0];
  ;
  v278 = &v277;
  v279 = sizeof(iree_vm_ref_t);
  memset(v278, 0, v279);
  iree_vm_ref_assign(v21, v278);
  ;
  v281 = &v280;
  v282 = sizeof(iree_vm_ref_t);
  memset(v281, 0, v282);
  iree_vm_ref_assign(v24, v281);
  v283 = be_u1000_hello_world_float_call_0rIirIIi_r_import_shim(v1, v276, v278, v9, v4, v281, v8, v13, v10, v24);
  v284 = (bool) v283;
  if (v284) {
    goto label68;
  } else {
    goto label35;
  }
label35:
  ;
  v285 = v3;
  v286 = v285->imports;
  v287 = &v286[21];
  ;
  v289 = &v288;
  v290 = sizeof(iree_vm_ref_t);
  memset(v289, 0, v290);
  iree_vm_ref_assign(v21, v289);
  v291 = be_u1000_hello_world_float_call_0ri_r_import_shim(v1, v287, v289, v10, v27);
  v292 = (bool) v291;
  if (v292) {
    goto label69;
  } else {
    goto label36;
  }
label36:
  ;
  v293 = v3;
  v294 = v293->imports;
  v295 = &v294[16];
  ;
  v297 = &v296;
  v298 = sizeof(iree_vm_ref_t);
  memset(v297, 0, v298);
  iree_vm_ref_assign(v21, v297);
  ;
  v300 = &v299;
  v301 = sizeof(iree_vm_ref_t);
  memset(v300, 0, v301);
  iree_vm_ref_assign(v18, v300);
  ;
  v303 = &v302;
  v304 = sizeof(iree_vm_ref_t);
  memset(v303, 0, v304);
  iree_vm_ref_assign(v27, v303);
  ;
  v306 = &v305;
  v307 = sizeof(iree_vm_ref_t);
  memset(v306, 0, v307);
  iree_vm_ref_assign(v24, v306);
  ;
  v309 = &v308;
  v310 = sizeof(iree_vm_ref_t);
  memset(v309, 0, v310);
  iree_vm_ref_assign(v15, v309);
  v311 = be_u1000_hello_world_float_call_0rIrrrIrIIi_v_import_shim(v1, v295, v297, v9, v300, v303, v306, v8, v309, v8, v13, v10);
  v312 = (bool) v311;
  if (v312) {
    goto label70;
  } else {
    goto label38;
  }
label37:
  ;
  v314 = &v313;
  v315 = sizeof(iree_vm_ref_t);
  memset(v314, 0, v315);
  iree_vm_ref_retain(v30, v314);
  ;
  v317 = &v316;
  v318 = sizeof(iree_vm_ref_t);
  memset(v317, 0, v318);
  iree_vm_ref_retain(v18, v317);
  iree_vm_ref_assign(v314, v18);
  iree_vm_ref_assign(v317, v15);
  goto label39;
label38:
  ;
  v320 = &v319;
  v321 = sizeof(iree_vm_ref_t);
  memset(v320, 0, v321);
  iree_vm_ref_retain(v27, v320);
  ;
  v323 = &v322;
  v324 = sizeof(iree_vm_ref_t);
  memset(v323, 0, v324);
  iree_vm_ref_retain(v15, v323);
  iree_vm_ref_assign(v320, v15);
  iree_vm_ref_assign(v323, v18);
  goto label39;
label39:
  ;
  v325 = v3;
  v326 = v325->refs;
  v327 = &v326[3];
  ;
  v328 = v2;
  v329 = v328->types;
  v330 = v329[3];
  v331 = iree_vm_make_ref_type_def(v330);
  v332 = iree_vm_type_def_as_ref(v331);
  v333 = iree_vm_ref_retain_or_move_checked(false, v18, v332, v327);
  v334 = (bool) v333;
  if (v334) {
    goto label65;
  } else {
    goto label40;
  }
label40:
  ;
  v335 = v3;
  v336 = v335->refs;
  v337 = &v336[2];
  ;
  v338 = v2;
  v339 = v338->types;
  v340 = v339[8];
  v341 = iree_vm_make_ref_type_def(v340);
  v342 = iree_vm_type_def_as_ref(v341);
  v343 = iree_vm_ref_retain_or_move_checked(false, v15, v342, v337);
  v344 = (bool) v343;
  if (v344) {
    goto label66;
  } else {
    goto label41;
  }
label41:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v345 = iree_ok_status();
  return v345;
label42:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v37;
label43:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v52;
label44:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v58;
label45:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v64;
label46:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v82;
label47:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v94;
label48:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v100;
label49:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v118;
label50:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v135;
label51:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v137;
label52:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v346 = iree_ok_status();
  return v346;
label53:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v347 = iree_make_cstring_view("HAL device `__device_0` not found or unavailable: #hal.device.target<\"local\", [#hal.executable.target<\"llvm-cpu\", \"static\", {cpu = \"\", cpu_features = \"+m,+a,+f,+zifencei\", data_layout = \"e-m:e-p:32:32-i64:64-n32-S128\", debug_symbols = false, link_embedded = false, link_static = true, native_vector_size = 16 : i64, static_library_output = \"/home/mingshi/Project/PF/rrtos/apps/be_u1000_ai_micro_demo/generated/be_u1000_hello_world_float.o\", target_abi = \"ilp32f\", target_triple = \"riscv32-unknown-elf\"}>]>");
  ;
  v348 = v347;
  v349 = v348.size;
  v350 = (int) v349;
  v351 = v348.data;
  v352 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v350, v351);
  return v352;
label54:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v150;
label55:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v156;
label56:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v174;
label57:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v190;
label58:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v198;
label59:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v215;
label60:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v225;
label61:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v231;
label62:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v239;
label63:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v250;
label64:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v261;
label65:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v333;
label66:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v343;
label67:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v272;
label68:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v283;
label69:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v291;
label70:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v311;
label71:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v353 = iree_ok_status();
  return v353;
label72:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v354 = iree_make_cstring_view("HAL device `__device_0` does not support any variant of executable `be_u1000_hello_world_float_linked`; available formats: [static]");
  ;
  v355 = v354;
  v356 = v355.size;
  v357 = (int) v356;
  v358 = v355.data;
  v359 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v357, v358);
  return v359;
}

static void be_u1000_hello_world_float_destroy(void* v1) {
  struct be_u1000_hello_world_float_t* v2;
  struct be_u1000_hello_world_float_t* v3;
  iree_allocator_t v4;
  v2 = (struct be_u1000_hello_world_float_t*) v1;
  ;
  v3 = v2;
  v4 = v3->allocator;
  iree_allocator_free(v4, v2);
  return;
}

static iree_status_t be_u1000_hello_world_float_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3) {
  struct be_u1000_hello_world_float_state_t* v4;
  iree_host_size_t v5;
  struct be_u1000_hello_world_float_state_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct be_u1000_hello_world_float_state_t* v10;
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
  const uint8_t* v67;
  const uint8_t* v68;
  void* v69;
  iree_host_size_t v70;
  iree_byte_span_t v71;
  iree_allocator_t v72;
  iree_vm_buffer_t* v73;
  iree_vm_buffer_t* v74;
  iree_vm_module_state_t* v75;
  iree_status_t v76;
  v4 = NULL;
  v5 = sizeof(struct be_u1000_hello_world_float_state_t);
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
  v11 = be_u1000_hello_world_float__utf8_hal_device_id_C1DCB7DBC4F49AE6;
  v12 = v11;
  v13 = (void*) v12;
  v14 = sizeof(be_u1000_hello_world_float__utf8_hal_device_id_C1DCB7DBC4F49AE6);
  v15 = iree_make_byte_span(v13, v14);
  v16 = iree_allocator_null();
  v17 = v4->rodata_buffers;
  v18 = &v17[0];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v15, v16, v18);
  v19 = be_u1000_hello_world_float__utf8_local_3D3B5147158B235A;
  v20 = v19;
  v21 = (void*) v20;
  v22 = sizeof(be_u1000_hello_world_float__utf8_local_3D3B5147158B235A);
  v23 = iree_make_byte_span(v21, v22);
  v24 = iree_allocator_null();
  v25 = v4->rodata_buffers;
  v26 = &v25[1];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v23, v24, v26);
  v27 = be_u1000_hello_world_float__utf8_hal_executable_format_EAB228F999C2D3A1;
  v28 = v27;
  v29 = (void*) v28;
  v30 = sizeof(be_u1000_hello_world_float__utf8_hal_executable_format_EAB228F999C2D3A1);
  v31 = iree_make_byte_span(v29, v30);
  v32 = iree_allocator_null();
  v33 = v4->rodata_buffers;
  v34 = &v33[2];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v31, v32, v34);
  v35 = be_u1000_hello_world_float__utf8_static_96B31E405495E0B6;
  v36 = v35;
  v37 = (void*) v36;
  v38 = sizeof(be_u1000_hello_world_float__utf8_static_96B31E405495E0B6);
  v39 = iree_make_byte_span(v37, v38);
  v40 = iree_allocator_null();
  v41 = v4->rodata_buffers;
  v42 = &v41[3];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v39, v40, v42);
  v43 = be_u1000_hello_world_float_be_u1000_hello_world_float_linked_static;
  v44 = v43;
  v45 = (void*) v44;
  v46 = sizeof(be_u1000_hello_world_float_be_u1000_hello_world_float_linked_static);
  v47 = iree_make_byte_span(v45, v46);
  v48 = iree_allocator_null();
  v49 = v4->rodata_buffers;
  v50 = &v49[4];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v47, v48, v50);
  v51 = be_u1000_hello_world_float__const;
  v52 = v51;
  v53 = (void*) v52;
  v54 = sizeof(be_u1000_hello_world_float__const);
  v55 = iree_make_byte_span(v53, v54);
  v56 = iree_allocator_null();
  v57 = v4->rodata_buffers;
  v58 = &v57[5];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v55, v56, v58);
  v59 = be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735;
  v60 = v59;
  v61 = (void*) v60;
  v62 = sizeof(be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735);
  v63 = iree_make_byte_span(v61, v62);
  v64 = iree_allocator_null();
  v65 = v4->rodata_buffers;
  v66 = &v65[6];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v63, v64, v66);
  v67 = be_u1000_hello_world_float__utf8_tensor_3C6209B4FD120BDC;
  v68 = v67;
  v69 = (void*) v68;
  v70 = sizeof(be_u1000_hello_world_float__utf8_tensor_3C6209B4FD120BDC);
  v71 = iree_make_byte_span(v69, v70);
  v72 = iree_allocator_null();
  v73 = v4->rodata_buffers;
  v74 = &v73[7];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v71, v72, v74);
  v75 = (iree_vm_module_state_t*) v10;
  EMITC_DEREF_ASSIGN_VALUE(v3, v75);
  v76 = iree_ok_status();
  return v76;
label3:
  return v8;
}

static void be_u1000_hello_world_float_free_state(void* v1, iree_vm_module_state_t* v2) {
  struct be_u1000_hello_world_float_state_t* v3;
  struct be_u1000_hello_world_float_state_t* v4;
  iree_vm_ref_t* v5;
  iree_vm_ref_t* v6;
  iree_vm_ref_t* v7;
  iree_vm_ref_t* v8;
  iree_vm_ref_t* v9;
  iree_allocator_t v10;
  v3 = (struct be_u1000_hello_world_float_state_t*) v2;
  ;
  v4 = v3;
  v5 = v4->refs;
  v6 = &v5[0];
  iree_vm_ref_release(v6);
  v7 = &v5[1];
  iree_vm_ref_release(v7);
  v8 = &v5[2];
  iree_vm_ref_release(v8);
  v9 = &v5[3];
  iree_vm_ref_release(v9);
  v10 = v4->allocator;
  iree_allocator_free(v10, v3);
  return;
}

static iree_status_t be_u1000_hello_world_float_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4) {
  iree_status_t v5;
  v5 = iree_make_status(IREE_STATUS_UNIMPLEMENTED);
  return v5;
}

static iree_status_t be_u1000_hello_world_float_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5) {
  struct be_u1000_hello_world_float_state_t* v6;
  struct be_u1000_hello_world_float_state_t* v7;
  iree_vm_function_t* v8;
  iree_vm_function_t* v9;
  iree_status_t v10;
  v6 = (struct be_u1000_hello_world_float_state_t*) v2;
  ;
  v7 = v6;
  v8 = v7->imports;
  v9 = &v8[v3];
  EMITC_DEREF_ASSIGN_PTR(v9, v4);
  v10 = iree_ok_status();
  return v10;
}

iree_status_t be_u1000_hello_world_float_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3) {
  struct be_u1000_hello_world_float_t* v4;
  iree_host_size_t v5;
  struct be_u1000_hello_world_float_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct be_u1000_hello_world_float_t* v10;
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
  iree_string_view_t v32;
  iree_vm_ref_type_t v33;
  iree_vm_module_t v34;
  iree_vm_module_t* v35;
  struct be_u1000_hello_world_float_t* v36;
  iree_status_t v37;
  bool v38;
  iree_status_t v39;
  struct be_u1000_hello_world_float_t* v40;
  v4 = NULL;
  v5 = sizeof(struct be_u1000_hello_world_float_t);
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
  v30 = iree_make_cstring_view("hal.file");
  v31 = iree_vm_instance_lookup_type(v1, v30);
  v11[9] = v31;
  v32 = iree_make_cstring_view("vm.buffer");
  v33 = iree_vm_instance_lookup_type(v1, v32);
  v11[10] = v33;
  ;
  v35 = &v34;
  v36 = v4;
  v37 = iree_vm_module_initialize(v35, v36);
  v38 = iree_status_is_ok(v37);
  if (v38) {
    goto label3;
  } else {
    goto label5;
  }
label3:
  v34.destroy = be_u1000_hello_world_float_destroy;
  v34.alloc_state = be_u1000_hello_world_float_alloc_state;
  v34.free_state = be_u1000_hello_world_float_free_state;
  v34.fork_state = be_u1000_hello_world_float_fork_state;
  v34.resolve_import = be_u1000_hello_world_float_resolve_import;
  v39 = iree_vm_native_module_create(v35, &be_u1000_hello_world_float_descriptor_, v1, v2, v3);
  return v39;
label4:
  return v8;
label5:
  v40 = v4;
  iree_allocator_free(v2, v40);
  return v37;
}

#endif   // EMITC_IMPLEMENTATION
