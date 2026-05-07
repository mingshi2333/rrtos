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
iree_alignas(64) static const uint8_t be_u1000_hello_world_float__const[] = {234, 133, 241, 190, 239, 28, 154, 63, 163, 35, 161, 63, 18, 148, 183, 62, 0, 137, 226, 61, 235, 122, 134, 190, 24, 111, 169, 62, 236, 109, 97, 63, 18, 74, 61, 190, 66, 107, 138, 63, 193, 195, 60, 191, 232, 192, 159, 62, 238, 175, 89, 191, 193, 81, 110, 191, 152, 140, 11, 62, 225, 195, 227, 61, 48, 119, 249, 189, 187, 48, 201, 190, 69, 245, 72, 62, 82, 20, 50, 63, 100, 204, 18, 62, 224, 225, 131, 189, 236, 137, 56, 190, 16, 208, 94, 189, 54, 127, 236, 190, 211, 166, 207, 62, 66, 43, 143, 62, 255, 158, 3, 191, 240, 136, 215, 190, 188, 39, 32, 63, 82, 165, 191, 190, 48, 163, 169, 190, 0, 0, 0, 0, 0, 0, 0, 0, 153, 222, 193, 62, 248, 223, 56, 191, 157, 66, 186, 61, 50, 143, 91, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 157, 161, 151, 63, 22, 206, 85, 190, 0, 0, 0, 0, 0, 0, 0, 0, 73, 190, 65, 188, 0, 0, 0, 0, 0, 0, 0, 0, 128, 109, 50, 59, 10, 100, 66, 62, 245, 164, 211, 62, 26, 199, 58, 190, 102, 153, 77, 62, 172, 233, 73, 190, 56, 115, 123, 189, 2, 179, 4, 190, 52, 174, 255, 189, 183, 245, 219, 190, 175, 179, 178, 190, 77, 149, 177, 62, 131, 166, 188, 62, 142, 20, 59, 62, 250, 124, 116, 62, 233, 249, 109, 190, 144, 49, 211, 188, 86, 242, 72, 62, 74, 1, 10, 61, 91, 130, 27, 191, 229, 38, 146, 61, 205, 90, 205, 62, 105, 67, 204, 190, 36, 170, 255, 189, 243, 2, 194, 190, 229, 42, 135, 62, 113, 98, 189, 190, 47, 163, 174, 62, 42, 71, 137, 190, 100, 59, 37, 62, 238, 111, 9, 190, 101, 224, 186, 62, 22, 255, 17, 62, 96, 105, 56, 190, 95, 124, 182, 62, 212, 236, 19, 191, 189, 163, 201, 190, 3, 65, 78, 63, 169, 242, 134, 190, 112, 79, 133, 188, 83, 236, 154, 62, 79, 201, 233, 62, 226, 250, 96, 62, 156, 127, 96, 190, 134, 199, 33, 62, 42, 197, 0, 191, 209, 218, 170, 62, 170, 139, 20, 190, 81, 96, 84, 190, 72, 193, 183, 190, 211, 8, 56, 62, 88, 46, 235, 62, 3, 146, 79, 62, 90, 73, 203, 190, 245, 30, 191, 62, 128, 220, 156, 190, 207, 153, 162, 62, 89, 130, 61, 190, 135, 111, 152, 62, 134, 165, 138, 190, 14, 155, 99, 190, 251, 122, 51, 190, 6, 16, 113, 190, 168, 252, 16, 189, 156, 70, 145, 61, 136, 60, 146, 190, 212, 188, 244, 189, 77, 116, 191, 62, 136, 108, 3, 189, 123, 233, 219, 190, 159, 223, 193, 62, 108, 228, 130, 61, 68, 120, 213, 61, 128, 138, 252, 187, 6, 123, 20, 62, 176, 36, 33, 61, 24, 206, 9, 61, 194, 40, 179, 190, 214, 176, 8, 190, 28, 99, 195, 190, 128, 152, 94, 59, 172, 216, 231, 61, 49, 18, 164, 60, 34, 2, 237, 61, 71, 243, 166, 187, 130, 171, 216, 62, 0, 252, 1, 187, 167, 201, 138, 62, 128, 93, 151, 189, 95, 232, 59, 190, 68, 98, 194, 189, 8, 150, 120, 189, 218, 216, 114, 62, 160, 243, 143, 62, 104, 216, 51, 189, 38, 20, 23, 62, 172, 192, 201, 190, 142, 138, 148, 190, 80, 61, 181, 188, 207, 187, 130, 62, 154, 136, 179, 190, 17, 138, 218, 190, 233, 217, 149, 62, 160, 19, 75, 61, 249, 182, 131, 62, 244, 20, 188, 190, 28, 137, 193, 61, 235, 238, 202, 62, 252, 48, 171, 190, 252, 98, 155, 189, 80, 116, 175, 190, 55, 22, 214, 62, 48, 62, 181, 188, 0, 209, 246, 58, 102, 189, 249, 61, 148, 42, 6, 63, 247, 200, 203, 190, 74, 165, 220, 190, 181, 208, 162, 190, 153, 240, 122, 190, 66, 27, 83, 190, 223, 144, 110, 190, 238, 254, 191, 62, 128, 211, 83, 60, 32, 0, 69, 60, 44, 208, 79, 190, 240, 103, 223, 189, 206, 177, 5, 62, 12, 74, 243, 61, 58, 241, 80, 62, 160, 178, 41, 190, 120, 105, 20, 62, 68, 147, 248, 61, 36, 103, 163, 61, 122, 155, 150, 190, 72, 105, 202, 189, 124, 234, 171, 61, 50, 214, 139, 62, 163, 202, 71, 189, 190, 26, 205, 190, 193, 84, 206, 62, 216, 187, 195, 62, 92, 252, 219, 190, 80, 240, 160, 60, 128, 48, 208, 60, 101, 41, 179, 190, 241, 31, 175, 188, 64, 250, 205, 62, 243, 51, 70, 190, 232, 154, 231, 190, 16, 161, 157, 190, 206, 196, 67, 62, 22, 175, 92, 190, 5, 248, 15, 191, 154, 129, 208, 62, 128, 148, 198, 59, 43, 12, 95, 190, 61, 198, 191, 62, 40, 246, 125, 189, 202, 97, 142, 190, 64, 36, 231, 60, 195, 241, 131, 62, 39, 48, 63, 62, 53, 21, 218, 190, 96, 163, 167, 187, 252, 0, 101, 63, 7, 150, 189, 62, 55, 147, 131, 62, 136, 69, 59, 61, 245, 255, 199, 188, 48, 93, 139, 189, 197, 121, 145, 62, 120, 20, 21, 190, 239, 229, 35, 191, 17, 42, 167, 62, 65, 126, 218, 62, 183, 50, 123, 190, 141, 99, 196, 62, 62, 41, 38, 62, 188, 91, 233, 189, 144, 73, 89, 61, 224, 135, 125, 188, 177, 239, 172, 62, 184, 48, 22, 189, 172, 86, 142, 189, 24, 88, 187, 190, 144, 111, 171, 189, 227, 97, 132, 62, 72, 65, 109, 61, 251, 34, 205, 62, 128, 155, 2, 60, 141, 195, 177, 190, 202, 178, 204, 190, 98, 171, 101, 190, 175, 23, 83, 62, 151, 223, 7, 190, 152, 33, 127, 62, 99, 16, 81, 63, 78, 30, 3, 62, 56, 163, 153, 190, 120, 31, 32, 190, 13, 218, 242, 62, 134, 172, 67, 190, 57, 203, 169, 62, 32, 114, 82, 61, 2, 151, 202, 190, 92, 232, 216, 189, 95, 56, 178, 62, 131, 21, 188, 62, 167, 251, 162, 62, 174, 60, 119, 190, 0, 228, 126, 190, 11, 196, 124, 190, 19, 76, 75, 63, 115, 132, 208, 62, 224, 103, 85, 60, 164, 39, 167, 190, 111, 111, 237, 189, 197, 184, 14, 63, 80, 93, 128, 60, 110, 55, 9, 62, 145, 116, 47, 191, 236, 43, 177, 61, 255, 173, 131, 62, 12, 4, 187, 189, 136, 220, 183, 189, 181, 27, 34, 190, 136, 155, 134, 62, 239, 26, 64, 62, 122, 98, 208, 190, 252, 77, 239, 61, 20, 233, 219, 190, 129, 124, 137, 190, 15, 215, 124, 62, 145, 205, 22, 190, 107, 251, 135, 62, 194, 191, 143, 190, 100, 105, 132, 62, 143, 28, 214, 190, 60, 99, 183, 189, 106, 104, 96, 62, 205, 105, 147, 62, 203, 35, 135, 190, 15, 225, 168, 190, 48, 64, 45, 62, 182, 201, 44, 61, 180, 30, 82, 190, 73, 148, 193, 190, 0, 43, 158, 187, 68, 158, 170, 62, 11, 162, 158, 62, 74, 38, 55, 190, 8, 142, 48, 190, 84, 191, 105, 61, 80, 51, 161, 190, 223, 41, 203, 190, 0, 0, 0, 0, 197, 179, 0, 63, 164, 186, 208, 62, 82, 206, 130, 190, 0, 0, 0, 0, 79, 27, 51, 62, 0, 0, 0, 0, 33, 114, 119, 190, 204, 205, 143, 61, 253, 163, 220, 190, 136, 227, 24, 63, 0, 0, 0, 0, 145, 140, 97, 62, 14, 118, 11, 63, 176, 85, 71, 190, 20, 9, 146, 189};
iree_alignas(1) static const uint8_t be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735[] = {105, 110, 112, 117, 116, 48};
struct be_u1000_hello_world_float_t {iree_allocator_t allocator;iree_vm_ref_type_t types[6];};
struct be_u1000_hello_world_float_state_t {iree_allocator_t allocator;uint8_t rwdata[1];iree_vm_ref_t refs[1];iree_vm_buffer_t rodata_buffers[2];iree_vm_function_t imports[6];};
typedef iree_status_t(*begin_call_t)(void*, iree_vm_stack_t*, iree_vm_function_call_t);
static iree_status_t be_u1000_hello_world_float_call_0iI_rr_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6);
static iree_status_t be_u1000_hello_world_float_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_hello_world_float_call_0rIIiiCID_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_hello_world_float_call_0rriiCID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8, int64_t v9);
static iree_status_t be_u1000_hello_world_float_call_0ri_I_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t* v5);
static iree_status_t be_u1000_hello_world_float_main(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5);
static iree_status_t be_u1000_hello_world_float_main_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static void be_u1000_hello_world_float_destroy(void* v1);
static iree_status_t be_u1000_hello_world_float_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3);
static void be_u1000_hello_world_float_free_state(void* v1, iree_vm_module_state_t* v2);
static iree_status_t be_u1000_hello_world_float_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4);
static iree_status_t be_u1000_hello_world_float_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5);
static const iree_vm_module_dependency_t be_u1000_hello_world_float_dependencies_[] = {{{"hal_inline", 10}, 0, IREE_VM_MODULE_DEPENDENCY_FLAG_REQUIRED},};
static const iree_vm_native_import_descriptor_t be_u1000_hello_world_float_imports_[] = {{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer.allocate", 26}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer.storage", 25}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer_view.create", 29}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer_view.assert", 29}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer_view.buffer", 29}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal_inline.buffer_view.dim", 26}},};
static const iree_vm_native_export_descriptor_t be_u1000_hello_world_float_exports_[] = {{{"main", 4}, {"0r_r", 4}, 0, NULL},};
static const iree_vm_native_function_ptr_t be_u1000_hello_world_float_funcs_[] = {{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)be_u1000_hello_world_float_main_export_shim},};
static const iree_vm_native_module_descriptor_t be_u1000_hello_world_float_descriptor_ = {{"be_u1000_hello_world_float", 26},0,0,NULL,1,be_u1000_hello_world_float_dependencies_,6,be_u1000_hello_world_float_imports_,1,be_u1000_hello_world_float_exports_,1,be_u1000_hello_world_float_funcs_,};
static iree_status_t be_u1000_hello_world_float_call_0iI_rr_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6) {
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
  int64_t v31;
  iree_host_size_t v32;
  int64_t* v33;
  begin_call_t v34;
  iree_vm_module_t* v35;
  iree_vm_function_call_t v36;
  iree_status_t v37;
  bool v38;
  uint8_t* v39;
  iree_vm_ref_t* v40;
  iree_host_size_t v41;
  uint8_t* v42;
  iree_vm_ref_t* v43;
  iree_status_t v44;
  iree_status_t v45;
  v7 = 0;
  v8 = sizeof(int32_t);
  v9 = v7 + v8;
  v10 = sizeof(int64_t);
  v11 = v9 + v10;
  v12 = sizeof(iree_vm_ref_t);
  v13 = v7 + v12;
  v14 = sizeof(iree_vm_ref_t);
  v15 = v13 + v14;
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
  v21 = iree_alloca(v11);
  v22 = (uint8_t*) v21;
  v19.arguments.data_length = v11;
  v19.arguments.data = v22;
  memset(v22, 0, v11);
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
  v32 = sizeof(int64_t);
  v33 = &v31;
  memcpy(v30, v33, v32);
  v34 = v16->module->begin_call;
  v35 = v16->module;
  v36 = v19;
  v37 = EMITC_CALL_INDIRECT(v34, v35, v1, v36);
  v38 = (bool) v37;
  if (v38) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v39 = v19.results.data;
  v40 = (iree_vm_ref_t*) v39;
  iree_vm_ref_move(v40, v5);
  v41 = sizeof(iree_vm_ref_t);
  v42 = v39 + v41;
  v43 = (iree_vm_ref_t*) v42;
  iree_vm_ref_move(v43, v6);
  v44 = iree_ok_status();
  return v44;
label4:
  v45 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v45;
label5:
  return v37;
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

static iree_status_t be_u1000_hello_world_float_main(iree_vm_stack_t* v1, struct be_u1000_hello_world_float_t* v2, struct be_u1000_hello_world_float_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5) {
  iree_vm_buffer_access_t v6;
  int32_t v7;
  float v8;
  int32_t v9;
  int32_t v10;
  int32_t v11;
  float v12;
  float v13;
  int32_t v14;
  int32_t v15;
  int32_t v16;
  int32_t v17;
  int32_t v18;
  int32_t v19;
  int32_t v20;
  int64_t v21;
  int64_t v22;
  int64_t v23;
  int64_t v24;
  int64_t v25;
  iree_vm_ref_t v26;
  iree_vm_ref_t* v27;
  iree_host_size_t v28;
  iree_vm_ref_t v29;
  iree_vm_ref_t* v30;
  iree_host_size_t v31;
  iree_vm_ref_t v32;
  iree_vm_ref_t* v33;
  iree_host_size_t v34;
  iree_vm_ref_t v35;
  iree_vm_ref_t* v36;
  iree_host_size_t v37;
  iree_vm_ref_t v38;
  iree_vm_ref_t* v39;
  iree_host_size_t v40;
  struct be_u1000_hello_world_float_state_t* v41;
  iree_vm_buffer_t* v42;
  iree_vm_buffer_t* v43;
  iree_vm_ref_type_t v44;
  iree_status_t v45;
  bool v46;
  struct be_u1000_hello_world_float_state_t* v47;
  iree_vm_function_t* v48;
  iree_vm_function_t* v49;
  iree_vm_ref_t v50;
  iree_vm_ref_t* v51;
  iree_host_size_t v52;
  int64_t v53;
  int64_t* v54;
  iree_status_t v55;
  bool v56;
  int64_t v57;
  int32_t v58;
  struct be_u1000_hello_world_float_state_t* v59;
  iree_vm_buffer_t* v60;
  iree_vm_buffer_t* v61;
  iree_vm_ref_type_t v62;
  iree_status_t v63;
  bool v64;
  int64_t v65;
  struct be_u1000_hello_world_float_state_t* v66;
  iree_vm_function_t* v67;
  iree_vm_function_t* v68;
  iree_vm_ref_t v69;
  iree_vm_ref_t* v70;
  iree_host_size_t v71;
  iree_vm_ref_t v72;
  iree_vm_ref_t* v73;
  iree_host_size_t v74;
  iree_status_t v75;
  bool v76;
  int32_t v77;
  struct be_u1000_hello_world_float_state_t* v78;
  iree_vm_function_t* v79;
  iree_vm_function_t* v80;
  iree_vm_ref_t v81;
  iree_vm_ref_t* v82;
  iree_host_size_t v83;
  iree_status_t v84;
  bool v85;
  int32_t v86;
  int64_t v87;
  struct be_u1000_hello_world_float_state_t* v88;
  iree_vm_function_t* v89;
  iree_vm_function_t* v90;
  iree_status_t v91;
  bool v92;
  int32_t v93;
  int64_t v94;
  struct be_u1000_hello_world_float_state_t* v95;
  iree_vm_function_t* v96;
  iree_vm_function_t* v97;
  iree_status_t v98;
  bool v99;
  int64_t v100;
  int64_t v101;
  int32_t v102;
  int64_t v103;
  int64_t v104;
  int32_t v105;
  struct be_u1000_hello_world_float_state_t* v106;
  iree_vm_function_t* v107;
  iree_vm_function_t* v108;
  iree_vm_ref_t v109;
  iree_vm_ref_t* v110;
  iree_host_size_t v111;
  iree_status_t v112;
  bool v113;
  int32_t v114;
  int32_t v115;
  int32_t v116;
  int32_t v117;
  int32_t v118;
  int32_t v119;
  int32_t v120;
  int32_t v121;
  int32_t v122;
  int64_t v123;
  int64_t v124;
  int64_t v125;
  int32_t v126;
  int32_t v127;
  int32_t v128;
  int32_t v129;
  bool v130;
  iree_vm_buffer_t* v131;
  iree_vm_buffer_t** v132;
  struct be_u1000_hello_world_float_state_t* v133;
  iree_allocator_t v134;
  iree_status_t v135;
  bool v136;
  iree_vm_ref_type_t v137;
  iree_vm_buffer_t* v138;
  iree_status_t v139;
  bool v140;
  int32_t v141;
  int32_t v142;
  int32_t v143;
  bool v144;
  int32_t v145;
  int32_t v146;
  int32_t v147;
  int32_t v148;
  bool v149;
  int32_t v150;
  int32_t v151;
  int32_t v152;
  bool v153;
  int32_t v154;
  int32_t v155;
  int64_t v156;
  int64_t v157;
  iree_vm_ref_t v158;
  iree_vm_buffer_t* v159;
  bool v160;
  iree_status_t v161;
  bool v162;
  int32_t v163;
  int32_t v164;
  int32_t v165;
  int32_t v166;
  bool v167;
  int32_t v168;
  int32_t v169;
  int32_t v170;
  int32_t v171;
  int32_t v172;
  bool v173;
  int64_t v174;
  int64_t v175;
  iree_vm_ref_t v176;
  iree_vm_buffer_t* v177;
  bool v178;
  float v179;
  float* v180;
  iree_status_t v181;
  bool v182;
  float v183;
  int32_t v184;
  int32_t v185;
  int64_t v186;
  int64_t v187;
  iree_vm_ref_t v188;
  iree_vm_buffer_t* v189;
  bool v190;
  float v191;
  float* v192;
  iree_status_t v193;
  bool v194;
  float v195;
  int32_t v196;
  int32_t v197;
  int64_t v198;
  int64_t v199;
  iree_vm_ref_t v200;
  iree_vm_buffer_t* v201;
  bool v202;
  float v203;
  float* v204;
  iree_status_t v205;
  bool v206;
  float v207;
  float v208;
  iree_vm_ref_t v209;
  iree_vm_buffer_t* v210;
  bool v211;
  iree_status_t v212;
  bool v213;
  int32_t v214;
  int32_t v215;
  int32_t v216;
  int32_t v217;
  int32_t v218;
  bool v219;
  int32_t v220;
  int32_t v221;
  int32_t v222;
  bool v223;
  int32_t v224;
  int32_t v225;
  int64_t v226;
  int64_t v227;
  iree_vm_ref_t v228;
  iree_vm_buffer_t* v229;
  bool v230;
  float v231;
  float* v232;
  iree_status_t v233;
  bool v234;
  float v235;
  int32_t v236;
  int32_t v237;
  int64_t v238;
  int64_t v239;
  iree_vm_ref_t v240;
  iree_vm_buffer_t* v241;
  bool v242;
  float v243;
  float* v244;
  iree_status_t v245;
  bool v246;
  float v247;
  float v248;
  int32_t v249;
  float v250;
  int32_t v251;
  float v252;
  int32_t v253;
  int32_t v254;
  int32_t v255;
  int64_t v256;
  int64_t v257;
  iree_vm_ref_t v258;
  iree_vm_buffer_t* v259;
  bool v260;
  iree_status_t v261;
  bool v262;
  int32_t v263;
  int32_t v264;
  int32_t v265;
  int32_t v266;
  int64_t v267;
  int64_t v268;
  int64_t v269;
  int32_t v270;
  int32_t v271;
  int32_t v272;
  int32_t v273;
  int32_t v274;
  int32_t v275;
  int32_t v276;
  int32_t v277;
  int32_t v278;
  bool v279;
  iree_vm_buffer_t* v280;
  iree_vm_buffer_t** v281;
  struct be_u1000_hello_world_float_state_t* v282;
  iree_allocator_t v283;
  iree_status_t v284;
  bool v285;
  iree_vm_ref_type_t v286;
  iree_vm_buffer_t* v287;
  iree_status_t v288;
  bool v289;
  int32_t v290;
  int32_t v291;
  int32_t v292;
  bool v293;
  int32_t v294;
  int32_t v295;
  int32_t v296;
  int32_t v297;
  bool v298;
  int32_t v299;
  int32_t v300;
  int32_t v301;
  bool v302;
  int32_t v303;
  int32_t v304;
  int64_t v305;
  int64_t v306;
  iree_vm_ref_t v307;
  iree_vm_buffer_t* v308;
  bool v309;
  iree_status_t v310;
  bool v311;
  int32_t v312;
  int32_t v313;
  int32_t v314;
  int32_t v315;
  int32_t v316;
  bool v317;
  int32_t v318;
  int32_t v319;
  int32_t v320;
  bool v321;
  int32_t v322;
  int32_t v323;
  int32_t v324;
  int32_t v325;
  int32_t v326;
  bool v327;
  int32_t v328;
  int32_t v329;
  int32_t v330;
  int64_t v331;
  int64_t v332;
  iree_vm_ref_t v333;
  iree_vm_buffer_t* v334;
  bool v335;
  float v336;
  float* v337;
  iree_status_t v338;
  bool v339;
  float v340;
  int32_t v341;
  int32_t v342;
  int32_t v343;
  int64_t v344;
  int64_t v345;
  iree_vm_ref_t v346;
  iree_vm_buffer_t* v347;
  bool v348;
  float v349;
  float* v350;
  iree_status_t v351;
  bool v352;
  float v353;
  int64_t v354;
  int64_t v355;
  iree_vm_ref_t v356;
  iree_vm_buffer_t* v357;
  bool v358;
  float v359;
  float* v360;
  iree_status_t v361;
  bool v362;
  float v363;
  float v364;
  iree_vm_ref_t v365;
  iree_vm_buffer_t* v366;
  bool v367;
  iree_status_t v368;
  bool v369;
  int32_t v370;
  int32_t v371;
  int32_t v372;
  int32_t v373;
  int32_t v374;
  bool v375;
  int32_t v376;
  int32_t v377;
  int32_t v378;
  bool v379;
  int32_t v380;
  int32_t v381;
  int64_t v382;
  int64_t v383;
  iree_vm_ref_t v384;
  iree_vm_buffer_t* v385;
  bool v386;
  float v387;
  float* v388;
  iree_status_t v389;
  bool v390;
  float v391;
  int32_t v392;
  int32_t v393;
  int64_t v394;
  int64_t v395;
  iree_vm_ref_t v396;
  iree_vm_buffer_t* v397;
  bool v398;
  float v399;
  float* v400;
  iree_status_t v401;
  bool v402;
  float v403;
  float v404;
  int32_t v405;
  float v406;
  int32_t v407;
  float v408;
  int32_t v409;
  int32_t v410;
  int32_t v411;
  int32_t v412;
  int64_t v413;
  int64_t v414;
  iree_vm_ref_t v415;
  iree_vm_buffer_t* v416;
  bool v417;
  iree_status_t v418;
  bool v419;
  int32_t v420;
  int32_t v421;
  int32_t v422;
  int32_t v423;
  int32_t v424;
  int32_t v425;
  bool v426;
  iree_vm_buffer_t* v427;
  iree_vm_buffer_t** v428;
  struct be_u1000_hello_world_float_state_t* v429;
  iree_allocator_t v430;
  iree_status_t v431;
  bool v432;
  iree_vm_ref_type_t v433;
  iree_vm_buffer_t* v434;
  iree_status_t v435;
  bool v436;
  int32_t v437;
  int32_t v438;
  int32_t v439;
  bool v440;
  int32_t v441;
  int32_t v442;
  int32_t v443;
  int32_t v444;
  bool v445;
  int32_t v446;
  int64_t v447;
  int64_t v448;
  iree_vm_ref_t v449;
  iree_vm_buffer_t* v450;
  bool v451;
  iree_status_t v452;
  bool v453;
  int32_t v454;
  int32_t v455;
  int32_t v456;
  int32_t v457;
  bool v458;
  int32_t v459;
  int32_t v460;
  int32_t v461;
  int32_t v462;
  bool v463;
  int32_t v464;
  int32_t v465;
  int32_t v466;
  int32_t v467;
  int64_t v468;
  int64_t v469;
  iree_vm_ref_t v470;
  iree_vm_buffer_t* v471;
  bool v472;
  float v473;
  float* v474;
  iree_status_t v475;
  bool v476;
  float v477;
  int32_t v478;
  int64_t v479;
  int64_t v480;
  iree_vm_ref_t v481;
  iree_vm_buffer_t* v482;
  bool v483;
  float v484;
  float* v485;
  iree_status_t v486;
  bool v487;
  float v488;
  int64_t v489;
  int64_t v490;
  iree_vm_ref_t v491;
  iree_vm_buffer_t* v492;
  bool v493;
  float v494;
  float* v495;
  iree_status_t v496;
  bool v497;
  float v498;
  float v499;
  iree_vm_ref_t v500;
  iree_vm_buffer_t* v501;
  bool v502;
  iree_status_t v503;
  bool v504;
  int32_t v505;
  int32_t v506;
  int32_t v507;
  int32_t v508;
  bool v509;
  int32_t v510;
  int64_t v511;
  int64_t v512;
  iree_vm_ref_t v513;
  iree_vm_buffer_t* v514;
  bool v515;
  float v516;
  float* v517;
  iree_status_t v518;
  bool v519;
  float v520;
  float v521;
  int32_t v522;
  int32_t v523;
  int64_t v524;
  int64_t v525;
  iree_vm_ref_t v526;
  iree_vm_buffer_t* v527;
  bool v528;
  iree_status_t v529;
  bool v530;
  int32_t v531;
  int32_t v532;
  int32_t v533;
  struct be_u1000_hello_world_float_state_t* v534;
  iree_vm_function_t* v535;
  iree_vm_function_t* v536;
  iree_vm_ref_t v537;
  iree_vm_ref_t* v538;
  iree_host_size_t v539;
  iree_status_t v540;
  bool v541;
  iree_vm_ref_t v542;
  iree_vm_ref_t* v543;
  iree_host_size_t v544;
  iree_status_t v545;
  iree_status_t v546;
  iree_status_t v547;
  iree_status_t v548;
  iree_status_t v549;
  iree_status_t v550;
  iree_status_t v551;
  iree_status_t v552;
  iree_status_t v553;
  iree_status_t v554;
  iree_status_t v555;
  iree_status_t v556;
  iree_status_t v557;
  iree_status_t v558;
  iree_status_t v559;
  iree_status_t v560;
  iree_status_t v561;
  iree_status_t v562;
  iree_status_t v563;
  iree_status_t v564;
  iree_status_t v565;
  iree_status_t v566;
  iree_status_t v567;
  iree_status_t v568;
  int32_t v569;
  int32_t v570;
  int32_t v571;
  int32_t v572;
  int32_t v573;
  int32_t v574;
  int32_t v575;
  int32_t v576;
  int32_t v577;
  int32_t v578;
  int32_t v579;
  int32_t v580;
  int32_t v581;
  int32_t v582;
  int32_t v583;
  int32_t v584;
  int32_t v585;
  int32_t v586;
  int32_t v587;
  int32_t v588;
  int32_t v589;
  int32_t v590;
  int32_t v591;
  v6 = IREE_VM_BUFFER_ACCESS_MUTABLE | IREE_VM_BUFFER_ACCESS_ORIGIN_GUEST;
  v7 = 2;
  v8 = -1.601323490e-01f;
  v9 = -1;
  v10 = 48;
  v11 = 304;
  v12 = 3.402823470e+38f;
  v13 = 0.0e+00f;
  v14 = 16;
  v15 = 32;
  v16 = 64;
  v17 = 4;
  v18 = 0;
  v19 = 553648160;
  v20 = 1;
  v21 = 1;
  v22 = 4096;
  v23 = 4;
  v24 = 256;
  v25 = 0;
  ;
  v27 = &v26;
  v28 = sizeof(iree_vm_ref_t);
  memset(v27, 0, v28);
  ;
  v30 = &v29;
  v31 = sizeof(iree_vm_ref_t);
  memset(v30, 0, v31);
  ;
  v33 = &v32;
  v34 = sizeof(iree_vm_ref_t);
  memset(v33, 0, v34);
  ;
  v36 = &v35;
  v37 = sizeof(iree_vm_ref_t);
  memset(v36, 0, v37);
  ;
  v39 = &v38;
  v40 = sizeof(iree_vm_ref_t);
  memset(v39, 0, v40);
  ;
  v41 = v3;
  v42 = v41->rodata_buffers;
  v43 = &v42[0];
  v44 = iree_vm_buffer_type();
  v45 = iree_vm_ref_wrap_retain(v43, v44, v5);
  v46 = (bool) v45;
  if (v46) {
    goto label128;
  } else {
    goto label2;
  }
label2:
  ;
  v47 = v3;
  v48 = v47->imports;
  v49 = &v48[5];
  ;
  v51 = &v50;
  v52 = sizeof(iree_vm_ref_t);
  memset(v51, 0, v52);
  iree_vm_ref_assign(v4, v51);
  ;
  v54 = &v53;
  v55 = be_u1000_hello_world_float_call_0ri_I_import_shim(v1, v49, v51, v18, v54);
  v56 = (bool) v55;
  if (v56) {
    goto label129;
  } else {
    goto label3;
  }
label3:
  v57 = v53;
  v58 = vm_trunc_i64i32(v57);
  ;
  v59 = v3;
  v60 = v59->rodata_buffers;
  v61 = &v60[1];
  v62 = iree_vm_buffer_type();
  v63 = iree_vm_ref_wrap_retain(v61, v62, v30);
  v64 = (bool) v63;
  if (v64) {
    goto label130;
  } else {
    goto label4;
  }
label4:
  v65 = vm_ext_i32i64s(v58);
  ;
  v66 = v3;
  v67 = v66->imports;
  v68 = &v67[3];
  ;
  v70 = &v69;
  v71 = sizeof(iree_vm_ref_t);
  memset(v70, 0, v71);
  iree_vm_ref_assign(v4, v70);
  ;
  v73 = &v72;
  v74 = sizeof(iree_vm_ref_t);
  memset(v73, 0, v74);
  iree_vm_ref_assign(v30, v73);
  v75 = be_u1000_hello_world_float_call_0rriiCID_v_2_import_shim(v1, v68, v70, v73, v19, v20, v7, v65, v21);
  v76 = (bool) v75;
  if (v76) {
    goto label131;
  } else {
    goto label5;
  }
label5:
  v77 = vm_mul_i32(v58, v17);
  ;
  v78 = v3;
  v79 = v78->imports;
  v80 = &v79[4];
  ;
  v82 = &v81;
  v83 = sizeof(iree_vm_ref_t);
  memset(v82, 0, v83);
  iree_vm_ref_assign(v4, v82);
  v84 = be_u1000_hello_world_float_call_0r_r_import_shim(v1, v80, v82, v4);
  v85 = (bool) v84;
  if (v85) {
    goto label132;
  } else {
    goto label6;
  }
label6:
  v86 = vm_mul_i32(v58, v16);
  v87 = vm_ext_i32i64s(v77);
  ;
  v88 = v3;
  v89 = v88->imports;
  v90 = &v89[0];
  v91 = be_u1000_hello_world_float_call_0iI_rr_import_shim(v1, v90, v16, v87, v30, v33);
  v92 = (bool) v91;
  if (v92) {
    goto label133;
  } else {
    goto label7;
  }
label7:
  v93 = vm_add_i32(v86, v86);
  v94 = vm_ext_i32i64s(v93);
  ;
  v95 = v3;
  v96 = v95->imports;
  v97 = &v96[0];
  v98 = be_u1000_hello_world_float_call_0iI_rr_import_shim(v1, v97, v16, v94, v36, v36);
  v99 = (bool) v98;
  if (v99) {
    goto label134;
  } else {
    goto label8;
  }
label8:
  v100 = vm_ext_i32i64u(v58);
  v101 = vm_shr_i64u(v100, v15);
  v102 = vm_trunc_i64i32(v101);
  v103 = vm_ext_i32i64u(v86);
  v104 = vm_shr_i64u(v103, v15);
  v105 = vm_trunc_i64i32(v104);
  ;
  v106 = v3;
  v107 = v106->imports;
  v108 = &v107[1];
  ;
  v110 = &v109;
  v111 = sizeof(iree_vm_ref_t);
  memset(v110, 0, v111);
  iree_vm_ref_assign(v4, v110);
  v112 = be_u1000_hello_world_float_call_0r_r_import_shim(v1, v108, v110, v4);
  v113 = (bool) v112;
  if (v113) {
    goto label135;
  } else {
    goto label9;
  }
label9:
  v114 = vm_cmp_lt_i32s(v18, v58);
  v115 = vm_xor_i32(v114, v20);
  v116 = vm_sub_i32(v18, v58);
  v117 = vm_sub_i32(v58, v20);
  v118 = vm_select_i32(v115, v116, v117);
  v119 = vm_div_i32s(v118, v16);
  v120 = vm_sub_i32(v18, v119);
  v121 = vm_add_i32(v119, v20);
  v122 = vm_select_i32(v115, v120, v121);
  v123 = vm_ext_i32i64u(v102);
  v124 = vm_shl_i64(v123, v15);
  v125 = vm_or_i64(v100, v124);
  v126 = vm_trunc_i64i32(v125);
  v127 = vm_mul_i32(v122, v16);
  v569 = v18;
  goto label10;
label10:
  v128 = vm_cmp_lt_i32s(v569, v122);
  v129 = vm_cmp_nz_i32(v128);
  v130 = (bool) v129;
  if (v130) {
    goto label11;
  } else {
    goto label50;
  }
label11:
  v131 = NULL;
  v132 = &v131;
  ;
  v133 = v3;
  v134 = v133->allocator;
  v135 = iree_vm_buffer_create(v6, v22, v18, v134, v132);
  v136 = (bool) v135;
  if (v136) {
    goto label136;
  } else {
    goto label12;
  }
label12:
  v137 = iree_vm_buffer_type();
  v138 = v131;
  v139 = iree_vm_ref_wrap_assign(v138, v137, v39);
  v140 = (bool) v139;
  if (v140) {
    goto label137;
  } else {
    goto label13;
  }
label13:
  v141 = vm_mul_i32(v569, v16);
  v570 = v141;
  goto label14;
label14:
  v142 = vm_cmp_lt_i32s(v570, v126);
  v143 = vm_cmp_nz_i32(v142);
  v144 = (bool) v143;
  if (v144) {
    goto label15;
  } else {
    goto label49;
  }
label15:
  v145 = vm_sub_i32(v126, v570);
  v146 = vm_min_i32s(v145, v16);
  v571 = v18;
  goto label16;
label16:
  v147 = vm_cmp_lt_i32s(v571, v146);
  v148 = vm_cmp_nz_i32(v147);
  v149 = (bool) v148;
  if (v149) {
    goto label17;
  } else {
    v573 = v18;
    goto label23;
  }
label17:
  v150 = vm_mul_i32(v571, v14);
  v572 = v18;
  goto label18;
label18:
  v151 = vm_cmp_lt_i32s(v572, v14);
  v152 = vm_cmp_nz_i32(v151);
  v153 = (bool) v152;
  if (v153) {
    goto label19;
  } else {
    goto label22;
  }
label19:
  v154 = vm_add_i32(v150, v572);
  v155 = vm_mul_i32(v154, v17);
  v156 = vm_ext_i32i64u(v155);
  v157 = vm_div_i64s(v156, v23);
  v158 = *v39;
  v159 = iree_vm_buffer_deref(v158);
  v160 = (bool) v159;
  if (v160) {
    goto label20;
  } else {
    goto label138;
  }
label20:
  v161 = vm_buffer_store_f32(v159, v157, v13);
  v162 = (bool) v161;
  if (v162) {
    goto label139;
  } else {
    goto label21;
  }
label21:
  v163 = vm_add_i32(v572, v20);
  v572 = v163;
  goto label18;
label22:
  v164 = vm_add_i32(v571, v20);
  v571 = v164;
  goto label16;
label23:
  v165 = vm_cmp_lt_i32s(v573, v146);
  v166 = vm_cmp_nz_i32(v165);
  v167 = (bool) v166;
  if (v167) {
    goto label24;
  } else {
    goto label36;
  }
label24:
  v168 = vm_add_i32(v570, v573);
  v169 = vm_mul_i32(v168, v17);
  v170 = vm_mul_i32(v573, v14);
  v574 = v18;
  goto label25;
label25:
  v171 = vm_cmp_lt_i32s(v574, v14);
  v172 = vm_cmp_nz_i32(v171);
  v173 = (bool) v172;
  if (v173) {
    goto label26;
  } else {
    goto label35;
  }
label26:
  v174 = vm_ext_i32i64u(v169);
  v175 = vm_div_i64s(v174, v23);
  v176 = *v4;
  v177 = iree_vm_buffer_deref(v176);
  v178 = (bool) v177;
  if (v178) {
    goto label27;
  } else {
    goto label140;
  }
label27:
  v179 = 0.0e+00f;
  v180 = &v179;
  v181 = vm_buffer_load_f32(v177, v175, v180);
  v182 = (bool) v181;
  if (v182) {
    goto label141;
  } else {
    goto label28;
  }
label28:
  v183 = v179;
  v184 = vm_add_i32(v574, v14);
  v185 = vm_mul_i32(v184, v17);
  v186 = vm_ext_i32i64u(v185);
  v187 = vm_div_i64s(v186, v23);
  v188 = *v5;
  v189 = iree_vm_buffer_deref(v188);
  v190 = (bool) v189;
  if (v190) {
    goto label29;
  } else {
    goto label142;
  }
label29:
  v191 = 0.0e+00f;
  v192 = &v191;
  v193 = vm_buffer_load_f32(v189, v187, v192);
  v194 = (bool) v193;
  if (v194) {
    goto label143;
  } else {
    goto label30;
  }
label30:
  v195 = v191;
  v196 = vm_add_i32(v170, v574);
  v197 = vm_mul_i32(v196, v17);
  v198 = vm_ext_i32i64u(v197);
  v199 = vm_div_i64s(v198, v23);
  v200 = *v39;
  v201 = iree_vm_buffer_deref(v200);
  v202 = (bool) v201;
  if (v202) {
    goto label31;
  } else {
    goto label144;
  }
label31:
  v203 = 0.0e+00f;
  v204 = &v203;
  v205 = vm_buffer_load_f32(v201, v199, v204);
  v206 = (bool) v205;
  if (v206) {
    goto label145;
  } else {
    goto label32;
  }
label32:
  v207 = v203;
  v208 = vm_fma_f32(v183, v195, v207);
  v209 = *v39;
  v210 = iree_vm_buffer_deref(v209);
  v211 = (bool) v210;
  if (v211) {
    goto label33;
  } else {
    goto label146;
  }
label33:
  v212 = vm_buffer_store_f32(v210, v199, v208);
  v213 = (bool) v212;
  if (v213) {
    goto label147;
  } else {
    goto label34;
  }
label34:
  v214 = vm_add_i32(v574, v20);
  v574 = v214;
  goto label25;
label35:
  v215 = vm_add_i32(v573, v20);
  v573 = v215;
  goto label23;
label36:
  v216 = vm_mul_i32(v570, v14);
  v575 = v18;
  goto label37;
label37:
  v217 = vm_cmp_lt_i32s(v575, v146);
  v218 = vm_cmp_nz_i32(v217);
  v219 = (bool) v218;
  if (v219) {
    goto label38;
  } else {
    goto label48;
  }
label38:
  v220 = vm_mul_i32(v575, v14);
  v576 = v18;
  goto label39;
label39:
  v221 = vm_cmp_lt_i32s(v576, v14);
  v222 = vm_cmp_nz_i32(v221);
  v223 = (bool) v222;
  if (v223) {
    goto label40;
  } else {
    goto label47;
  }
label40:
  v224 = vm_add_i32(v220, v576);
  v225 = vm_mul_i32(v224, v17);
  v226 = vm_ext_i32i64u(v225);
  v227 = vm_div_i64s(v226, v23);
  v228 = *v39;
  v229 = iree_vm_buffer_deref(v228);
  v230 = (bool) v229;
  if (v230) {
    goto label41;
  } else {
    goto label148;
  }
label41:
  v231 = 0.0e+00f;
  v232 = &v231;
  v233 = vm_buffer_load_f32(v229, v227, v232);
  v234 = (bool) v233;
  if (v234) {
    goto label149;
  } else {
    goto label42;
  }
label42:
  v235 = v231;
  v236 = vm_add_i32(v576, v15);
  v237 = vm_mul_i32(v236, v17);
  v238 = vm_ext_i32i64u(v237);
  v239 = vm_div_i64s(v238, v23);
  v240 = *v5;
  v241 = iree_vm_buffer_deref(v240);
  v242 = (bool) v241;
  if (v242) {
    goto label43;
  } else {
    goto label150;
  }
label43:
  v243 = 0.0e+00f;
  v244 = &v243;
  v245 = vm_buffer_load_f32(v241, v239, v244);
  v246 = (bool) v245;
  if (v246) {
    goto label151;
  } else {
    goto label44;
  }
label44:
  v247 = v243;
  v248 = vm_add_f32(v235, v247);
  v249 = vm_cmp_lt_f32u(v248, v12);
  v250 = vm_select_f32(v249, v248, v12);
  v251 = vm_cmp_lt_f32u(v13, v250);
  v252 = vm_select_f32(v251, v250, v13);
  v253 = vm_add_i32(v576, v216);
  v254 = vm_add_i32(v253, v220);
  v255 = vm_mul_i32(v254, v17);
  v256 = vm_ext_i32i64u(v255);
  v257 = vm_div_i64s(v256, v23);
  v258 = *v36;
  v259 = iree_vm_buffer_deref(v258);
  v260 = (bool) v259;
  if (v260) {
    goto label45;
  } else {
    goto label152;
  }
label45:
  v261 = vm_buffer_store_f32(v259, v257, v252);
  v262 = (bool) v261;
  if (v262) {
    goto label153;
  } else {
    goto label46;
  }
label46:
  v263 = vm_add_i32(v576, v20);
  v576 = v263;
  goto label39;
label47:
  v264 = vm_add_i32(v575, v20);
  v575 = v264;
  goto label37;
label48:
  v265 = vm_add_i32(v570, v127);
  v570 = v265;
  goto label14;
label49:
  v266 = vm_add_i32(v569, v20);
  v569 = v266;
  goto label10;
label50:
  v267 = vm_ext_i32i64u(v105);
  v268 = vm_shl_i64(v267, v15);
  v269 = vm_or_i64(v103, v268);
  v270 = vm_trunc_i64i32(v269);
  v271 = vm_cmp_lt_i32s(v270, v18);
  v272 = vm_sub_i32(v9, v270);
  v273 = vm_select_i32(v271, v272, v270);
  v274 = vm_div_i32s(v273, v17);
  v275 = vm_sub_i32(v9, v274);
  v276 = vm_select_i32(v271, v275, v274);
  v577 = v18;
  goto label51;
label51:
  v277 = vm_cmp_lt_i32s(v577, v122);
  v278 = vm_cmp_nz_i32(v277);
  v279 = (bool) v278;
  if (v279) {
    goto label52;
  } else {
    v586 = v18;
    goto label94;
  }
label52:
  v280 = NULL;
  v281 = &v280;
  ;
  v282 = v3;
  v283 = v282->allocator;
  v284 = iree_vm_buffer_create(v6, v22, v18, v283, v281);
  v285 = (bool) v284;
  if (v285) {
    goto label154;
  } else {
    goto label53;
  }
label53:
  v286 = iree_vm_buffer_type();
  v287 = v280;
  v288 = iree_vm_ref_wrap_assign(v287, v286, v4);
  v289 = (bool) v288;
  if (v289) {
    goto label155;
  } else {
    goto label54;
  }
label54:
  v290 = vm_mul_i32(v577, v16);
  v578 = v290;
  goto label55;
label55:
  v291 = vm_cmp_lt_i32s(v578, v126);
  v292 = vm_cmp_nz_i32(v291);
  v293 = (bool) v292;
  if (v293) {
    goto label56;
  } else {
    goto label93;
  }
label56:
  v294 = vm_sub_i32(v126, v578);
  v295 = vm_min_i32s(v294, v16);
  v579 = v18;
  goto label57;
label57:
  v296 = vm_cmp_lt_i32s(v579, v295);
  v297 = vm_cmp_nz_i32(v296);
  v298 = (bool) v297;
  if (v298) {
    goto label58;
  } else {
    goto label64;
  }
label58:
  v299 = vm_mul_i32(v579, v14);
  v580 = v18;
  goto label59;
label59:
  v300 = vm_cmp_lt_i32s(v580, v14);
  v301 = vm_cmp_nz_i32(v300);
  v302 = (bool) v301;
  if (v302) {
    goto label60;
  } else {
    goto label63;
  }
label60:
  v303 = vm_add_i32(v299, v580);
  v304 = vm_mul_i32(v303, v17);
  v305 = vm_ext_i32i64u(v304);
  v306 = vm_div_i64s(v305, v23);
  v307 = *v4;
  v308 = iree_vm_buffer_deref(v307);
  v309 = (bool) v308;
  if (v309) {
    goto label61;
  } else {
    goto label156;
  }
label61:
  v310 = vm_buffer_store_f32(v308, v306, v13);
  v311 = (bool) v310;
  if (v311) {
    goto label157;
  } else {
    goto label62;
  }
label62:
  v312 = vm_add_i32(v580, v20);
  v580 = v312;
  goto label59;
label63:
  v313 = vm_add_i32(v579, v20);
  v579 = v313;
  goto label57;
label64:
  v314 = vm_mul_i32(v578, v14);
  v581 = v18;
  goto label65;
label65:
  v315 = vm_cmp_lt_i32s(v581, v295);
  v316 = vm_cmp_nz_i32(v315);
  v317 = (bool) v316;
  if (v317) {
    goto label66;
  } else {
    v584 = v18;
    goto label81;
  }
label66:
  v318 = vm_mul_i32(v581, v14);
  v582 = v18;
  goto label67;
label67:
  v319 = vm_cmp_lt_i32s(v582, v14);
  v320 = vm_cmp_nz_i32(v319);
  v321 = (bool) v320;
  if (v321) {
    goto label68;
  } else {
    goto label80;
  }
label68:
  v322 = vm_mul_i32(v582, v14);
  v323 = vm_add_i32(v318, v582);
  v324 = vm_mul_i32(v323, v17);
  v583 = v18;
  goto label69;
label69:
  v325 = vm_cmp_lt_i32s(v583, v14);
  v326 = vm_cmp_nz_i32(v325);
  v327 = (bool) v326;
  if (v327) {
    goto label70;
  } else {
    goto label79;
  }
label70:
  v328 = vm_add_i32(v583, v314);
  v329 = vm_add_i32(v328, v318);
  v330 = vm_mul_i32(v329, v17);
  v331 = vm_ext_i32i64u(v330);
  v332 = vm_div_i64s(v331, v23);
  v333 = *v36;
  v334 = iree_vm_buffer_deref(v333);
  v335 = (bool) v334;
  if (v335) {
    goto label71;
  } else {
    goto label158;
  }
label71:
  v336 = 0.0e+00f;
  v337 = &v336;
  v338 = vm_buffer_load_f32(v334, v332, v337);
  v339 = (bool) v338;
  if (v339) {
    goto label159;
  } else {
    goto label72;
  }
label72:
  v340 = v336;
  v341 = vm_add_i32(v322, v583);
  v342 = vm_add_i32(v341, v10);
  v343 = vm_mul_i32(v342, v17);
  v344 = vm_ext_i32i64u(v343);
  v345 = vm_div_i64s(v344, v23);
  v346 = *v5;
  v347 = iree_vm_buffer_deref(v346);
  v348 = (bool) v347;
  if (v348) {
    goto label73;
  } else {
    goto label160;
  }
label73:
  v349 = 0.0e+00f;
  v350 = &v349;
  v351 = vm_buffer_load_f32(v347, v345, v350);
  v352 = (bool) v351;
  if (v352) {
    goto label161;
  } else {
    goto label74;
  }
label74:
  v353 = v349;
  v354 = vm_ext_i32i64u(v324);
  v355 = vm_div_i64s(v354, v23);
  v356 = *v4;
  v357 = iree_vm_buffer_deref(v356);
  v358 = (bool) v357;
  if (v358) {
    goto label75;
  } else {
    goto label162;
  }
label75:
  v359 = 0.0e+00f;
  v360 = &v359;
  v361 = vm_buffer_load_f32(v357, v355, v360);
  v362 = (bool) v361;
  if (v362) {
    goto label163;
  } else {
    goto label76;
  }
label76:
  v363 = v359;
  v364 = vm_fma_f32(v340, v353, v363);
  v365 = *v4;
  v366 = iree_vm_buffer_deref(v365);
  v367 = (bool) v366;
  if (v367) {
    goto label77;
  } else {
    goto label164;
  }
label77:
  v368 = vm_buffer_store_f32(v366, v355, v364);
  v369 = (bool) v368;
  if (v369) {
    goto label165;
  } else {
    goto label78;
  }
label78:
  v370 = vm_add_i32(v583, v20);
  v583 = v370;
  goto label69;
label79:
  v371 = vm_add_i32(v582, v20);
  v582 = v371;
  goto label67;
label80:
  v372 = vm_add_i32(v581, v20);
  v581 = v372;
  goto label65;
label81:
  v373 = vm_cmp_lt_i32s(v584, v295);
  v374 = vm_cmp_nz_i32(v373);
  v375 = (bool) v374;
  if (v375) {
    goto label82;
  } else {
    goto label92;
  }
label82:
  v376 = vm_mul_i32(v584, v14);
  v585 = v18;
  goto label83;
label83:
  v377 = vm_cmp_lt_i32s(v585, v14);
  v378 = vm_cmp_nz_i32(v377);
  v379 = (bool) v378;
  if (v379) {
    goto label84;
  } else {
    goto label91;
  }
label84:
  v380 = vm_add_i32(v376, v585);
  v381 = vm_mul_i32(v380, v17);
  v382 = vm_ext_i32i64u(v381);
  v383 = vm_div_i64s(v382, v23);
  v384 = *v4;
  v385 = iree_vm_buffer_deref(v384);
  v386 = (bool) v385;
  if (v386) {
    goto label85;
  } else {
    goto label166;
  }
label85:
  v387 = 0.0e+00f;
  v388 = &v387;
  v389 = vm_buffer_load_f32(v385, v383, v388);
  v390 = (bool) v389;
  if (v390) {
    goto label167;
  } else {
    goto label86;
  }
label86:
  v391 = v387;
  v392 = vm_add_i32(v585, v11);
  v393 = vm_mul_i32(v392, v17);
  v394 = vm_ext_i32i64u(v393);
  v395 = vm_div_i64s(v394, v23);
  v396 = *v5;
  v397 = iree_vm_buffer_deref(v396);
  v398 = (bool) v397;
  if (v398) {
    goto label87;
  } else {
    goto label168;
  }
label87:
  v399 = 0.0e+00f;
  v400 = &v399;
  v401 = vm_buffer_load_f32(v397, v395, v400);
  v402 = (bool) v401;
  if (v402) {
    goto label169;
  } else {
    goto label88;
  }
label88:
  v403 = v399;
  v404 = vm_add_f32(v391, v403);
  v405 = vm_cmp_lt_f32u(v404, v12);
  v406 = vm_select_f32(v405, v404, v12);
  v407 = vm_cmp_lt_f32u(v13, v406);
  v408 = vm_select_f32(v407, v406, v13);
  v409 = vm_add_i32(v585, v314);
  v410 = vm_add_i32(v409, v376);
  v411 = vm_add_i32(v410, v276);
  v412 = vm_mul_i32(v411, v17);
  v413 = vm_ext_i32i64u(v412);
  v414 = vm_div_i64s(v413, v23);
  v415 = *v36;
  v416 = iree_vm_buffer_deref(v415);
  v417 = (bool) v416;
  if (v417) {
    goto label89;
  } else {
    goto label170;
  }
label89:
  v418 = vm_buffer_store_f32(v416, v414, v408);
  v419 = (bool) v418;
  if (v419) {
    goto label171;
  } else {
    goto label90;
  }
label90:
  v420 = vm_add_i32(v585, v20);
  v585 = v420;
  goto label83;
label91:
  v421 = vm_add_i32(v584, v20);
  v584 = v421;
  goto label81;
label92:
  v422 = vm_add_i32(v578, v127);
  v578 = v422;
  goto label55;
label93:
  v423 = vm_add_i32(v577, v20);
  v577 = v423;
  goto label51;
label94:
  v424 = vm_cmp_lt_i32s(v586, v122);
  v425 = vm_cmp_nz_i32(v424);
  v426 = (bool) v425;
  if (v426) {
    goto label95;
  } else {
    goto label126;
  }
label95:
  v427 = NULL;
  v428 = &v427;
  ;
  v429 = v3;
  v430 = v429->allocator;
  v431 = iree_vm_buffer_create(v6, v24, v18, v430, v428);
  v432 = (bool) v431;
  if (v432) {
    goto label172;
  } else {
    goto label96;
  }
label96:
  v433 = iree_vm_buffer_type();
  v434 = v427;
  v435 = iree_vm_ref_wrap_assign(v434, v433, v4);
  v436 = (bool) v435;
  if (v436) {
    goto label173;
  } else {
    goto label97;
  }
label97:
  v437 = vm_mul_i32(v586, v16);
  v587 = v437;
  goto label98;
label98:
  v438 = vm_cmp_lt_i32s(v587, v126);
  v439 = vm_cmp_nz_i32(v438);
  v440 = (bool) v439;
  if (v440) {
    goto label99;
  } else {
    goto label125;
  }
label99:
  v441 = vm_sub_i32(v126, v587);
  v442 = vm_min_i32s(v441, v16);
  v588 = v18;
  goto label100;
label100:
  v443 = vm_cmp_lt_i32s(v588, v442);
  v444 = vm_cmp_nz_i32(v443);
  v445 = (bool) v444;
  if (v445) {
    goto label101;
  } else {
    goto label104;
  }
label101:
  v446 = vm_mul_i32(v588, v17);
  v447 = vm_ext_i32i64u(v446);
  v448 = vm_div_i64s(v447, v23);
  v449 = *v4;
  v450 = iree_vm_buffer_deref(v449);
  v451 = (bool) v450;
  if (v451) {
    goto label102;
  } else {
    goto label174;
  }
label102:
  v452 = vm_buffer_store_f32(v450, v448, v13);
  v453 = (bool) v452;
  if (v453) {
    goto label175;
  } else {
    goto label103;
  }
label103:
  v454 = vm_add_i32(v588, v20);
  v588 = v454;
  goto label100;
label104:
  v455 = vm_mul_i32(v587, v14);
  v589 = v18;
  goto label105;
label105:
  v456 = vm_cmp_lt_i32s(v589, v442);
  v457 = vm_cmp_nz_i32(v456);
  v458 = (bool) v457;
  if (v458) {
    goto label106;
  } else {
    v591 = v18;
    goto label118;
  }
label106:
  v459 = vm_mul_i32(v589, v14);
  v460 = vm_mul_i32(v589, v17);
  v590 = v18;
  goto label107;
label107:
  v461 = vm_cmp_lt_i32s(v590, v14);
  v462 = vm_cmp_nz_i32(v461);
  v463 = (bool) v462;
  if (v463) {
    goto label108;
  } else {
    goto label117;
  }
label108:
  v464 = vm_add_i32(v590, v455);
  v465 = vm_add_i32(v464, v459);
  v466 = vm_add_i32(v465, v276);
  v467 = vm_mul_i32(v466, v17);
  v468 = vm_ext_i32i64u(v467);
  v469 = vm_div_i64s(v468, v23);
  v470 = *v36;
  v471 = iree_vm_buffer_deref(v470);
  v472 = (bool) v471;
  if (v472) {
    goto label109;
  } else {
    goto label176;
  }
label109:
  v473 = 0.0e+00f;
  v474 = &v473;
  v475 = vm_buffer_load_f32(v471, v469, v474);
  v476 = (bool) v475;
  if (v476) {
    goto label177;
  } else {
    goto label110;
  }
label110:
  v477 = v473;
  v478 = vm_mul_i32(v590, v17);
  v479 = vm_ext_i32i64u(v478);
  v480 = vm_div_i64s(v479, v23);
  v481 = *v5;
  v482 = iree_vm_buffer_deref(v481);
  v483 = (bool) v482;
  if (v483) {
    goto label111;
  } else {
    goto label178;
  }
label111:
  v484 = 0.0e+00f;
  v485 = &v484;
  v486 = vm_buffer_load_f32(v482, v480, v485);
  v487 = (bool) v486;
  if (v487) {
    goto label179;
  } else {
    goto label112;
  }
label112:
  v488 = v484;
  v489 = vm_ext_i32i64u(v460);
  v490 = vm_div_i64s(v489, v23);
  v491 = *v4;
  v492 = iree_vm_buffer_deref(v491);
  v493 = (bool) v492;
  if (v493) {
    goto label113;
  } else {
    goto label180;
  }
label113:
  v494 = 0.0e+00f;
  v495 = &v494;
  v496 = vm_buffer_load_f32(v492, v490, v495);
  v497 = (bool) v496;
  if (v497) {
    goto label181;
  } else {
    goto label114;
  }
label114:
  v498 = v494;
  v499 = vm_fma_f32(v477, v488, v498);
  v500 = *v4;
  v501 = iree_vm_buffer_deref(v500);
  v502 = (bool) v501;
  if (v502) {
    goto label115;
  } else {
    goto label182;
  }
label115:
  v503 = vm_buffer_store_f32(v501, v490, v499);
  v504 = (bool) v503;
  if (v504) {
    goto label183;
  } else {
    goto label116;
  }
label116:
  v505 = vm_add_i32(v590, v20);
  v590 = v505;
  goto label107;
label117:
  v506 = vm_add_i32(v589, v20);
  v589 = v506;
  goto label105;
label118:
  v507 = vm_cmp_lt_i32s(v591, v442);
  v508 = vm_cmp_nz_i32(v507);
  v509 = (bool) v508;
  if (v509) {
    goto label119;
  } else {
    goto label124;
  }
label119:
  v510 = vm_mul_i32(v591, v17);
  v511 = vm_ext_i32i64u(v510);
  v512 = vm_div_i64s(v511, v23);
  v513 = *v4;
  v514 = iree_vm_buffer_deref(v513);
  v515 = (bool) v514;
  if (v515) {
    goto label120;
  } else {
    goto label184;
  }
label120:
  v516 = 0.0e+00f;
  v517 = &v516;
  v518 = vm_buffer_load_f32(v514, v512, v517);
  v519 = (bool) v518;
  if (v519) {
    goto label185;
  } else {
    goto label121;
  }
label121:
  v520 = v516;
  v521 = vm_add_f32(v520, v8);
  v522 = vm_add_i32(v587, v591);
  v523 = vm_mul_i32(v522, v17);
  v524 = vm_ext_i32i64u(v523);
  v525 = vm_div_i64s(v524, v23);
  v526 = *v33;
  v527 = iree_vm_buffer_deref(v526);
  v528 = (bool) v527;
  if (v528) {
    goto label122;
  } else {
    goto label186;
  }
label122:
  v529 = vm_buffer_store_f32(v527, v525, v521);
  v530 = (bool) v529;
  if (v530) {
    goto label187;
  } else {
    goto label123;
  }
label123:
  v531 = vm_add_i32(v591, v20);
  v591 = v531;
  goto label118;
label124:
  v532 = vm_add_i32(v587, v127);
  v587 = v532;
  goto label98;
label125:
  v533 = vm_add_i32(v586, v20);
  v586 = v533;
  goto label94;
label126:
  ;
  v534 = v3;
  v535 = v534->imports;
  v536 = &v535[2];
  ;
  v538 = &v537;
  v539 = sizeof(iree_vm_ref_t);
  memset(v538, 0, v539);
  iree_vm_ref_assign(v30, v538);
  v540 = be_u1000_hello_world_float_call_0rIIiiCID_r_2_import_shim(v1, v536, v538, v25, v87, v19, v20, v7, v65, v21, v4);
  v541 = (bool) v540;
  if (v541) {
    goto label188;
  } else {
    goto label127;
  }
label127:
  ;
  v543 = &v542;
  v544 = sizeof(iree_vm_ref_t);
  memset(v543, 0, v544);
  iree_vm_ref_move(v4, v543);
  iree_vm_ref_move(v543, v5);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v545 = iree_ok_status();
  return v545;
label128:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v45;
label129:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v55;
label130:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v63;
label131:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v75;
label132:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v84;
label133:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v91;
label134:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v98;
label135:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v112;
label136:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v135;
label137:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v139;
label138:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v546 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v546;
label139:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v161;
label140:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v547 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v547;
label141:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v181;
label142:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v548 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v548;
label143:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v193;
label144:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v549 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v549;
label145:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v205;
label146:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v550 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v550;
label147:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v212;
label148:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v551 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v551;
label149:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v233;
label150:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v552 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v552;
label151:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v245;
label152:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v553 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v553;
label153:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v261;
label154:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v284;
label155:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v288;
label156:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v554 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v554;
label157:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v310;
label158:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v555 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v555;
label159:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v338;
label160:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v556 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v556;
label161:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v351;
label162:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v557 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v557;
label163:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v361;
label164:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v558 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v558;
label165:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v368;
label166:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v559 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v559;
label167:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v389;
label168:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v560 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v560;
label169:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v401;
label170:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v561 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v561;
label171:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v418;
label172:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v431;
label173:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v435;
label174:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v562 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v562;
label175:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v452;
label176:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v563 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v563;
label177:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v475;
label178:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v564 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v564;
label179:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v486;
label180:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v565 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v565;
label181:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v496;
label182:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v566 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v566;
label183:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v503;
label184:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v567 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v567;
label185:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v518;
label186:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  v568 = iree_make_status(IREE_STATUS_INVALID_ARGUMENT);
  return v568;
label187:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v529;
label188:
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v36);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v39);
  iree_vm_ref_release(v4);
  return v540;
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
  iree_vm_module_state_t* v27;
  iree_status_t v28;
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
  v11 = be_u1000_hello_world_float__const;
  v12 = v11;
  v13 = (void*) v12;
  v14 = sizeof(be_u1000_hello_world_float__const);
  v15 = iree_make_byte_span(v13, v14);
  v16 = iree_allocator_null();
  v17 = v4->rodata_buffers;
  v18 = &v17[0];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v15, v16, v18);
  v19 = be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735;
  v20 = v19;
  v21 = (void*) v20;
  v22 = sizeof(be_u1000_hello_world_float__utf8_input0_F74E046E5FFA3735);
  v23 = iree_make_byte_span(v21, v22);
  v24 = iree_allocator_null();
  v25 = v4->rodata_buffers;
  v26 = &v25[1];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v23, v24, v26);
  v27 = (iree_vm_module_state_t*) v10;
  EMITC_DEREF_ASSIGN_VALUE(v3, v27);
  v28 = iree_ok_status();
  return v28;
label3:
  return v8;
}

static void be_u1000_hello_world_float_free_state(void* v1, iree_vm_module_state_t* v2) {
  struct be_u1000_hello_world_float_state_t* v3;
  struct be_u1000_hello_world_float_state_t* v4;
  iree_allocator_t v5;
  v3 = (struct be_u1000_hello_world_float_state_t*) v2;
  ;
  v4 = v3;
  v5 = v4->allocator;
  iree_allocator_free(v5, v3);
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
  iree_vm_module_t v24;
  iree_vm_module_t* v25;
  struct be_u1000_hello_world_float_t* v26;
  iree_status_t v27;
  bool v28;
  iree_status_t v29;
  struct be_u1000_hello_world_float_t* v30;
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
  v12 = iree_make_cstring_view("f32");
  v13 = iree_vm_instance_lookup_type(v1, v12);
  v11[0] = v13;
  v14 = iree_make_cstring_view("i32");
  v15 = iree_vm_instance_lookup_type(v1, v14);
  v11[1] = v15;
  v16 = iree_make_cstring_view("i64");
  v17 = iree_vm_instance_lookup_type(v1, v16);
  v11[2] = v17;
  v18 = iree_make_cstring_view("hal.buffer");
  v19 = iree_vm_instance_lookup_type(v1, v18);
  v11[3] = v19;
  v20 = iree_make_cstring_view("hal.buffer_view");
  v21 = iree_vm_instance_lookup_type(v1, v20);
  v11[4] = v21;
  v22 = iree_make_cstring_view("vm.buffer");
  v23 = iree_vm_instance_lookup_type(v1, v22);
  v11[5] = v23;
  ;
  v25 = &v24;
  v26 = v4;
  v27 = iree_vm_module_initialize(v25, v26);
  v28 = iree_status_is_ok(v27);
  if (v28) {
    goto label3;
  } else {
    goto label5;
  }
label3:
  v24.destroy = be_u1000_hello_world_float_destroy;
  v24.alloc_state = be_u1000_hello_world_float_alloc_state;
  v24.free_state = be_u1000_hello_world_float_free_state;
  v24.fork_state = be_u1000_hello_world_float_fork_state;
  v24.resolve_import = be_u1000_hello_world_float_resolve_import;
  v29 = iree_vm_native_module_create(v25, &be_u1000_hello_world_float_descriptor_, v1, v2, v3);
  return v29;
label4:
  return v8;
label5:
  v30 = v4;
  iree_allocator_free(v2, v30);
  return v27;
}

#endif   // EMITC_IMPLEMENTATION
