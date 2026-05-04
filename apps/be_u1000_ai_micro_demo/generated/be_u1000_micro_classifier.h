#ifndef BE_U1000_MICRO_CLASSIFIER_H_
#define BE_U1000_MICRO_CLASSIFIER_H_
#include "iree/vm/api.h"
#ifdef __cplusplus
extern "C" {
#endif //  __cplusplus
iree_status_t be_u1000_micro_classifier_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3);
#ifdef __cplusplus
}  // extern "C"
#endif //  __cplusplus
#endif //  BE_U1000_MICRO_CLASSIFIER_H_
#if defined(EMITC_IMPLEMENTATION)
#include "iree/vm/ops.h"
#include "iree/vm/ops_emitc.h"
#include "iree/vm/shims_emitc.h"
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_hal_device_id_C1DCB7DBC4F49AE6[] = {104, 97, 108, 46, 100, 101, 118, 105, 99, 101, 46, 105, 100};
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_local_3D3B5147158B235A[] = {108, 111, 99, 97, 108, 42};
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_hal_executable_format_EAB228F999C2D3A1[] = {104, 97, 108, 46, 101, 120, 101, 99, 117, 116, 97, 98, 108, 101, 46, 102, 111, 114, 109, 97, 116};
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_static_96B31E405495E0B6[] = {115, 116, 97, 116, 105, 99};
iree_alignas(16) static const uint8_t be_u1000_micro_classifier_predict_dispatch_0_static[] = {112, 114, 101, 100, 105, 99, 116, 95, 100, 105, 115, 112, 97, 116, 99, 104, 95, 48};
iree_alignas(64) static const uint8_t be_u1000_micro_classifier__const[] = {205, 204, 204, 61, 205, 204, 76, 190, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 0, 0, 128, 190, 0, 0, 128, 62, 0, 0, 64, 63, 0, 0, 0, 191, 0, 0, 0, 62, 0, 0, 0, 62, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_input0_F74E046E5FFA3735[] = {105, 110, 112, 117, 116, 48};
iree_alignas(1) static const uint8_t be_u1000_micro_classifier__utf8_tensor_3C6209B4FD120BDC[] = {116, 101, 110, 115, 111, 114};
struct be_u1000_micro_classifier_t {iree_allocator_t allocator;iree_vm_ref_type_t types[11];};
struct be_u1000_micro_classifier_state_t {iree_allocator_t allocator;uint8_t rwdata[1];iree_vm_ref_t refs[4];iree_vm_buffer_t rodata_buffers[8];iree_vm_function_t imports[21];};
typedef iree_status_t(*begin_call_t)(void*, iree_vm_stack_t*, iree_vm_function_call_t);
static iree_status_t be_u1000_micro_classifier___predict_memoize_apply(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_micro_classifier_call_0rIirIIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int64_t v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10);
static iree_status_t be_u1000_micro_classifier_call_0rIiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, int32_t v6, int64_t v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_micro_classifier_call_0riIiirII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t v5, int32_t v6, int32_t v7, iree_vm_ref_t* v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_micro_classifier_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8);
static iree_status_t be_u1000_micro_classifier_call_0rIIiiCID_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11);
static iree_status_t be_u1000_micro_classifier_call_0rriiCID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8, int64_t v9);
static iree_status_t be_u1000_micro_classifier_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_micro_classifier_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_micro_classifier_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3);
static iree_status_t be_u1000_micro_classifier_call_0riiI_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6);
static iree_status_t be_u1000_micro_classifier_call_0rriiiiICiDCiirIID_v_0_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, iree_vm_ref_t* v14, int64_t v15, int64_t v16, int32_t v17, int32_t v18, iree_vm_ref_t* v19, int64_t v20, int64_t v21, int32_t v22, int32_t v23, iree_vm_ref_t* v24, int64_t v25, int64_t v26);
static iree_status_t be_u1000_micro_classifier_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7);
static iree_status_t be_u1000_micro_classifier_call_0rIrrIiiII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int64_t v7, int32_t v8, int32_t v9, int64_t v10, int64_t v11, iree_vm_ref_t* v12);
static iree_status_t be_u1000_micro_classifier_call_0rIrrrIrIII_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, iree_vm_ref_t* v9, int64_t v10, int64_t v11, int64_t v12);
static iree_status_t be_u1000_micro_classifier_call_0rIrrrICrIID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10, int64_t v11, int64_t v12, iree_vm_ref_t* v13, int64_t v14, int64_t v15);
static iree_status_t be_u1000_micro_classifier_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3);
static iree_status_t be_u1000_micro_classifier_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4);
static iree_status_t be_u1000_micro_classifier_call_0rIrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, iree_vm_ref_t* v8);
static iree_status_t be_u1000_micro_classifier_call_0rI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5);
static iree_status_t be_u1000_micro_classifier_call_0iICrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int32_t* v7);
static iree_status_t be_u1000_micro_classifier_predict(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5);
static iree_status_t be_u1000_micro_classifier_predict_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t be_u1000_micro_classifier___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6);
static iree_status_t be_u1000_micro_classifier___init(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3);
static void be_u1000_micro_classifier_destroy(void* v1);
static iree_status_t be_u1000_micro_classifier_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3);
static void be_u1000_micro_classifier_free_state(void* v1, iree_vm_module_state_t* v2);
static iree_status_t be_u1000_micro_classifier_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4);
static iree_status_t be_u1000_micro_classifier_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5);
static const iree_vm_module_dependency_t be_u1000_micro_classifier_dependencies_[] = {{{"hal", 3}, 6, IREE_VM_MODULE_DEPENDENCY_FLAG_REQUIRED},};
static const iree_vm_native_import_descriptor_t be_u1000_micro_classifier_imports_[] = {{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.ex.file.from_memory", 23}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.allocator.allocate", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.allocator.import", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer.assert", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.create", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.assert", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.buffer_view.buffer", 22}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.create", 25}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.finalize", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.execution_barrier", 36}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.command_buffer.dispatch", 27}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.allocator", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.query.i64", 20}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.alloca", 23}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.read", 21}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.device.queue.execute.indirect", 33}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.count", 17}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.devices.get", 15}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.executable.create", 21}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.create", 16}},{IREE_VM_NATIVE_IMPORT_REQUIRED, {"hal.fence.await", 15}},};
static const iree_vm_native_export_descriptor_t be_u1000_micro_classifier_exports_[] = {{{"__init", 6}, {"0v_v", 4}, 0, NULL},{{"predict", 7}, {"0r_r", 4}, 0, NULL},};
static const iree_vm_native_function_ptr_t be_u1000_micro_classifier_funcs_[] = {{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)be_u1000_micro_classifier___init_export_shim},{(iree_vm_native_function_shim_t)iree_emitc_shim, (iree_vm_native_function_target_t)be_u1000_micro_classifier_predict_export_shim},};
static const iree_vm_native_module_descriptor_t be_u1000_micro_classifier_descriptor_ = {{"be_u1000_micro_classifier", 25},0,0,NULL,1,be_u1000_micro_classifier_dependencies_,21,be_u1000_micro_classifier_imports_,2,be_u1000_micro_classifier_exports_,2,be_u1000_micro_classifier_funcs_,};
static iree_status_t be_u1000_micro_classifier___predict_memoize_apply(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3, iree_vm_ref_t* v4) {
  int64_t v5;
  int32_t v6;
  int32_t v7;
  int32_t v8;
  int32_t v9;
  int64_t v10;
  int64_t v11;
  int64_t v12;
  int64_t v13;
  int32_t v14;
  int32_t v15;
  iree_vm_ref_t v16;
  iree_vm_ref_t* v17;
  iree_host_size_t v18;
  iree_vm_ref_t v19;
  iree_vm_ref_t* v20;
  iree_host_size_t v21;
  iree_vm_ref_t v22;
  iree_vm_ref_t* v23;
  iree_host_size_t v24;
  iree_vm_ref_t v25;
  iree_vm_ref_t* v26;
  iree_host_size_t v27;
  struct be_u1000_micro_classifier_state_t* v28;
  iree_vm_ref_t* v29;
  iree_vm_ref_t* v30;
  struct be_u1000_micro_classifier_t* v31;
  iree_vm_ref_type_t* v32;
  iree_vm_ref_type_t v33;
  iree_vm_type_def_t v34;
  iree_vm_ref_type_t v35;
  iree_status_t v36;
  bool v37;
  struct be_u1000_micro_classifier_state_t* v38;
  iree_vm_ref_t* v39;
  iree_vm_ref_t* v40;
  struct be_u1000_micro_classifier_t* v41;
  iree_vm_ref_type_t* v42;
  iree_vm_ref_type_t v43;
  iree_vm_type_def_t v44;
  iree_vm_ref_type_t v45;
  iree_status_t v46;
  bool v47;
  struct be_u1000_micro_classifier_state_t* v48;
  iree_vm_ref_t* v49;
  iree_vm_ref_t* v50;
  struct be_u1000_micro_classifier_t* v51;
  iree_vm_ref_type_t* v52;
  iree_vm_ref_type_t v53;
  iree_vm_type_def_t v54;
  iree_vm_ref_type_t v55;
  iree_status_t v56;
  bool v57;
  struct be_u1000_micro_classifier_state_t* v58;
  iree_vm_function_t* v59;
  iree_vm_function_t* v60;
  iree_vm_ref_t v61;
  iree_vm_ref_t* v62;
  iree_host_size_t v63;
  iree_status_t v64;
  bool v65;
  struct be_u1000_micro_classifier_state_t* v66;
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
  iree_vm_ref_t v78;
  iree_vm_ref_t* v79;
  iree_host_size_t v80;
  iree_vm_ref_t v81;
  iree_vm_ref_t* v82;
  iree_host_size_t v83;
  iree_status_t v84;
  bool v85;
  struct be_u1000_micro_classifier_state_t* v86;
  iree_vm_function_t* v87;
  iree_vm_function_t* v88;
  iree_vm_ref_t v89;
  iree_vm_ref_t* v90;
  iree_host_size_t v91;
  iree_status_t v92;
  bool v93;
  struct be_u1000_micro_classifier_state_t* v94;
  iree_vm_function_t* v95;
  iree_vm_function_t* v96;
  iree_vm_ref_t v97;
  iree_vm_ref_t* v98;
  iree_host_size_t v99;
  iree_status_t v100;
  bool v101;
  iree_vm_ref_t v102;
  iree_vm_ref_t* v103;
  iree_host_size_t v104;
  iree_status_t v105;
  v5 = -1;
  v6 = 2;
  v7 = 0;
  v8 = 1;
  v9 = 3;
  v10 = 0;
  v11 = 16;
  v12 = 128;
  v13 = 8;
  v14 = 28;
  v15 = 13;
  ;
  v17 = &v16;
  v18 = sizeof(iree_vm_ref_t);
  memset(v17, 0, v18);
  ;
  v20 = &v19;
  v21 = sizeof(iree_vm_ref_t);
  memset(v20, 0, v21);
  ;
  v23 = &v22;
  v24 = sizeof(iree_vm_ref_t);
  memset(v23, 0, v24);
  ;
  v26 = &v25;
  v27 = sizeof(iree_vm_ref_t);
  memset(v26, 0, v27);
  iree_vm_ref_release(v4);
  ;
  v28 = v3;
  v29 = v28->refs;
  v30 = &v29[0];
  ;
  v31 = v2;
  v32 = v31->types;
  v33 = v32[6];
  v34 = iree_vm_make_ref_type_def(v33);
  v35 = iree_vm_type_def_as_ref(v34);
  v36 = iree_vm_ref_retain_or_move_checked(false, v30, v35, v20);
  v37 = (bool) v36;
  if (v37) {
    goto label9;
  } else {
    goto label2;
  }
label2:
  ;
  v38 = v3;
  v39 = v38->refs;
  v40 = &v39[2];
  ;
  v41 = v2;
  v42 = v41->types;
  v43 = v42[3];
  v44 = iree_vm_make_ref_type_def(v43);
  v45 = iree_vm_type_def_as_ref(v44);
  v46 = iree_vm_ref_retain_or_move_checked(false, v40, v45, v23);
  v47 = (bool) v46;
  if (v47) {
    goto label10;
  } else {
    goto label3;
  }
label3:
  ;
  v48 = v3;
  v49 = v48->refs;
  v50 = &v49[1];
  ;
  v51 = v2;
  v52 = v51->types;
  v53 = v52[7];
  v54 = iree_vm_make_ref_type_def(v53);
  v55 = iree_vm_type_def_as_ref(v54);
  v56 = iree_vm_ref_retain_or_move_checked(false, v50, v55, v26);
  v57 = (bool) v56;
  if (v57) {
    goto label11;
  } else {
    goto label4;
  }
label4:
  ;
  v58 = v3;
  v59 = v58->imports;
  v60 = &v59[7];
  ;
  v62 = &v61;
  v63 = sizeof(iree_vm_ref_t);
  memset(v62, 0, v63);
  iree_vm_ref_assign(v20, v62);
  v64 = be_u1000_micro_classifier_call_0riiIi_r_import_shim(v1, v60, v62, v7, v9, v5, v6, v20);
  v65 = (bool) v64;
  if (v65) {
    goto label12;
  } else {
    goto label5;
  }
label5:
  ;
  v66 = v3;
  v67 = v66->imports;
  v68 = &v67[10];
  ;
  v70 = &v69;
  v71 = sizeof(iree_vm_ref_t);
  memset(v70, 0, v71);
  iree_vm_ref_assign(v20, v70);
  ;
  v73 = &v72;
  v74 = sizeof(iree_vm_ref_t);
  memset(v73, 0, v74);
  iree_vm_ref_assign(v26, v73);
  ;
  v76 = &v75;
  v77 = sizeof(iree_vm_ref_t);
  memset(v76, 0, v77);
  iree_vm_ref_assign(v4, v76);
  ;
  v79 = &v78;
  v80 = sizeof(iree_vm_ref_t);
  memset(v79, 0, v80);
  iree_vm_ref_assign(v23, v79);
  ;
  v82 = &v81;
  v83 = sizeof(iree_vm_ref_t);
  memset(v82, 0, v83);
  iree_vm_ref_assign(v4, v82);
  v84 = be_u1000_micro_classifier_call_0rriiiiICiDCiirIID_v_0_3_import_shim(v1, v68, v70, v73, v7, v8, v8, v8, v10, v7, v9, v7, v7, v76, v10, v11, v7, v7, v79, v10, v12, v7, v8, v82, v10, v13);
  v85 = (bool) v84;
  if (v85) {
    goto label13;
  } else {
    goto label6;
  }
label6:
  ;
  v86 = v3;
  v87 = v86->imports;
  v88 = &v87[9];
  ;
  v90 = &v89;
  v91 = sizeof(iree_vm_ref_t);
  memset(v90, 0, v91);
  iree_vm_ref_assign(v20, v90);
  v92 = be_u1000_micro_classifier_call_0riiI_v_import_shim(v1, v88, v90, v14, v15, v10);
  v93 = (bool) v92;
  if (v93) {
    goto label14;
  } else {
    goto label7;
  }
label7:
  ;
  v94 = v3;
  v95 = v94->imports;
  v96 = &v95[8];
  ;
  v98 = &v97;
  v99 = sizeof(iree_vm_ref_t);
  memset(v98, 0, v99);
  iree_vm_ref_assign(v20, v98);
  v100 = be_u1000_micro_classifier_call_0r_v_import_shim(v1, v96, v98);
  v101 = (bool) v100;
  if (v101) {
    goto label15;
  } else {
    goto label8;
  }
label8:
  ;
  v103 = &v102;
  v104 = sizeof(iree_vm_ref_t);
  memset(v103, 0, v104);
  iree_vm_ref_move(v20, v103);
  iree_vm_ref_move(v103, v4);
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  v105 = iree_ok_status();
  return v105;
label9:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v36;
label10:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v46;
label11:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v56;
label12:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v64;
label13:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v84;
label14:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v92;
label15:
  iree_vm_ref_release(v17);
  iree_vm_ref_release(v23);
  iree_vm_ref_release(v20);
  iree_vm_ref_release(v26);
  return v100;
}
static iree_status_t be_u1000_micro_classifier_call_0rIirIIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int64_t v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10) {
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
static iree_status_t be_u1000_micro_classifier_call_0rIiiI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int32_t v5, int32_t v6, int64_t v7, iree_vm_ref_t* v8) {
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
static iree_status_t be_u1000_micro_classifier_call_0riIiirII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int64_t v5, int32_t v6, int32_t v7, iree_vm_ref_t* v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11) {
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
static iree_status_t be_u1000_micro_classifier_call_0rrrIii_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int64_t v6, int32_t v7, int32_t v8) {
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
static iree_status_t be_u1000_micro_classifier_call_0rIIiiCID_r_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, int64_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int64_t v10, iree_vm_ref_t* v11) {
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
static iree_status_t be_u1000_micro_classifier_call_0rriiCID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int64_t v8, int64_t v9) {
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
static iree_status_t be_u1000_micro_classifier_call_0r_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4) {
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
static iree_status_t be_u1000_micro_classifier_call_0riiIi_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6, int32_t v7, iree_vm_ref_t* v8) {
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
static iree_status_t be_u1000_micro_classifier_call_0r_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3) {
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
static iree_status_t be_u1000_micro_classifier_call_0riiI_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int32_t v4, int32_t v5, int64_t v6) {
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
static iree_status_t be_u1000_micro_classifier_call_0rriiiiICiDCiirIID_v_0_3_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, int32_t v5, int32_t v6, int32_t v7, int32_t v8, int64_t v9, int32_t v10, int32_t v11, int32_t v12, int32_t v13, iree_vm_ref_t* v14, int64_t v15, int64_t v16, int32_t v17, int32_t v18, iree_vm_ref_t* v19, int64_t v20, int64_t v21, int32_t v22, int32_t v23, iree_vm_ref_t* v24, int64_t v25, int64_t v26) {
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
  iree_vm_function_t* v77;
  iree_vm_module_t* v78;
  bool v79;
  iree_vm_function_call_t v80;
  iree_vm_function_t v81;
  void* v82;
  uint8_t* v83;
  void* v84;
  uint8_t* v85;
  uint8_t* v86;
  iree_vm_ref_t* v87;
  iree_host_size_t v88;
  uint8_t* v89;
  iree_vm_ref_t* v90;
  iree_host_size_t v91;
  uint8_t* v92;
  int32_t v93;
  iree_host_size_t v94;
  int32_t* v95;
  iree_host_size_t v96;
  uint8_t* v97;
  int32_t v98;
  iree_host_size_t v99;
  int32_t* v100;
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
  int64_t v113;
  iree_host_size_t v114;
  int64_t* v115;
  iree_host_size_t v116;
  uint8_t* v117;
  int32_t v118;
  iree_host_size_t v119;
  int32_t* v120;
  iree_host_size_t v121;
  uint8_t* v122;
  int32_t v123;
  iree_host_size_t v124;
  int32_t* v125;
  iree_host_size_t v126;
  uint8_t* v127;
  int32_t v128;
  iree_host_size_t v129;
  int32_t* v130;
  iree_host_size_t v131;
  uint8_t* v132;
  int32_t v133;
  iree_host_size_t v134;
  int32_t* v135;
  iree_host_size_t v136;
  uint8_t* v137;
  iree_vm_ref_t* v138;
  iree_host_size_t v139;
  uint8_t* v140;
  int64_t v141;
  iree_host_size_t v142;
  int64_t* v143;
  iree_host_size_t v144;
  uint8_t* v145;
  int64_t v146;
  iree_host_size_t v147;
  int64_t* v148;
  iree_host_size_t v149;
  uint8_t* v150;
  int32_t v151;
  iree_host_size_t v152;
  int32_t* v153;
  iree_host_size_t v154;
  uint8_t* v155;
  int32_t v156;
  iree_host_size_t v157;
  int32_t* v158;
  iree_host_size_t v159;
  uint8_t* v160;
  iree_vm_ref_t* v161;
  iree_host_size_t v162;
  uint8_t* v163;
  int64_t v164;
  iree_host_size_t v165;
  int64_t* v166;
  iree_host_size_t v167;
  uint8_t* v168;
  int64_t v169;
  iree_host_size_t v170;
  int64_t* v171;
  iree_host_size_t v172;
  uint8_t* v173;
  int32_t v174;
  iree_host_size_t v175;
  int32_t* v176;
  iree_host_size_t v177;
  uint8_t* v178;
  int32_t v179;
  iree_host_size_t v180;
  int32_t* v181;
  iree_host_size_t v182;
  uint8_t* v183;
  iree_vm_ref_t* v184;
  iree_host_size_t v185;
  uint8_t* v186;
  int64_t v187;
  iree_host_size_t v188;
  int64_t* v189;
  iree_host_size_t v190;
  uint8_t* v191;
  int64_t v192;
  iree_host_size_t v193;
  int64_t* v194;
  begin_call_t v195;
  iree_vm_module_t* v196;
  iree_vm_function_call_t v197;
  iree_status_t v198;
  bool v199;
  iree_status_t v200;
  iree_status_t v201;
  v27 = 1;
  v28 = 0;
  v29 = sizeof(iree_vm_ref_t);
  v30 = v28 + v29;
  v31 = sizeof(iree_vm_ref_t);
  v32 = v30 + v31;
  v33 = sizeof(int32_t);
  v34 = v32 + v33;
  v35 = sizeof(int32_t);
  v36 = v34 + v35;
  v37 = sizeof(int32_t);
  v38 = v36 + v37;
  v39 = sizeof(int32_t);
  v40 = v38 + v39;
  v41 = sizeof(int64_t);
  v42 = v40 + v41;
  v43 = sizeof(int32_t);
  v44 = v42 + v43;
  v45 = sizeof(int32_t);
  v46 = v44 + v45;
  v47 = sizeof(int32_t);
  v48 = v46 + v47;
  v49 = sizeof(int32_t);
  v50 = v48 + v49;
  v51 = sizeof(iree_vm_ref_t);
  v52 = v50 + v51;
  v53 = sizeof(int64_t);
  v54 = v52 + v53;
  v55 = sizeof(int64_t);
  v56 = v54 + v55;
  v57 = sizeof(int32_t);
  v58 = v56 + v57;
  v59 = sizeof(int32_t);
  v60 = v58 + v59;
  v61 = sizeof(iree_vm_ref_t);
  v62 = v60 + v61;
  v63 = sizeof(int64_t);
  v64 = v62 + v63;
  v65 = sizeof(int64_t);
  v66 = v64 + v65;
  v67 = sizeof(int32_t);
  v68 = v66 + v67;
  v69 = sizeof(int32_t);
  v70 = v68 + v69;
  v71 = sizeof(iree_vm_ref_t);
  v72 = v70 + v71;
  v73 = sizeof(int64_t);
  v74 = v72 + v73;
  v75 = sizeof(int64_t);
  v76 = v74 + v75;
  ;
  v77 = v2;
  v78 = v77->module;
  v79 = !v78;
  if (v79) {
    goto label4;
  } else {
    goto label2;
  }
label2:
  ;
  v81 = *v2;
  v80.function = v81;
  v82 = iree_alloca(v76);
  v83 = (uint8_t*) v82;
  v80.arguments.data_length = v76;
  v80.arguments.data = v83;
  memset(v83, 0, v76);
  v84 = iree_alloca(v27);
  v85 = (uint8_t*) v84;
  v80.results.data_length = v28;
  v80.results.data = v85;
  memset(v85, 0, v27);
  v86 = v80.arguments.data;
  v87 = (iree_vm_ref_t*) v86;
  iree_vm_ref_assign(v3, v87);
  v88 = sizeof(iree_vm_ref_t);
  v89 = v86 + v88;
  v90 = (iree_vm_ref_t*) v89;
  iree_vm_ref_assign(v4, v90);
  v91 = sizeof(iree_vm_ref_t);
  v92 = v89 + v91;
  ;
  v93 = v5;
  v94 = sizeof(int32_t);
  v95 = &v93;
  memcpy(v92, v95, v94);
  v96 = sizeof(int32_t);
  v97 = v92 + v96;
  ;
  v98 = v6;
  v99 = sizeof(int32_t);
  v100 = &v98;
  memcpy(v97, v100, v99);
  v101 = sizeof(int32_t);
  v102 = v97 + v101;
  ;
  v103 = v7;
  v104 = sizeof(int32_t);
  v105 = &v103;
  memcpy(v102, v105, v104);
  v106 = sizeof(int32_t);
  v107 = v102 + v106;
  ;
  v108 = v8;
  v109 = sizeof(int32_t);
  v110 = &v108;
  memcpy(v107, v110, v109);
  v111 = sizeof(int32_t);
  v112 = v107 + v111;
  ;
  v113 = v9;
  v114 = sizeof(int64_t);
  v115 = &v113;
  memcpy(v112, v115, v114);
  v116 = sizeof(int64_t);
  v117 = v112 + v116;
  ;
  v118 = v10;
  v119 = sizeof(int32_t);
  v120 = &v118;
  memcpy(v117, v120, v119);
  v121 = sizeof(int32_t);
  v122 = v117 + v121;
  ;
  v123 = v11;
  v124 = sizeof(int32_t);
  v125 = &v123;
  memcpy(v122, v125, v124);
  v126 = sizeof(int32_t);
  v127 = v122 + v126;
  ;
  v128 = v12;
  v129 = sizeof(int32_t);
  v130 = &v128;
  memcpy(v127, v130, v129);
  v131 = sizeof(int32_t);
  v132 = v127 + v131;
  ;
  v133 = v13;
  v134 = sizeof(int32_t);
  v135 = &v133;
  memcpy(v132, v135, v134);
  v136 = sizeof(int32_t);
  v137 = v132 + v136;
  v138 = (iree_vm_ref_t*) v137;
  iree_vm_ref_assign(v14, v138);
  v139 = sizeof(iree_vm_ref_t);
  v140 = v137 + v139;
  ;
  v141 = v15;
  v142 = sizeof(int64_t);
  v143 = &v141;
  memcpy(v140, v143, v142);
  v144 = sizeof(int64_t);
  v145 = v140 + v144;
  ;
  v146 = v16;
  v147 = sizeof(int64_t);
  v148 = &v146;
  memcpy(v145, v148, v147);
  v149 = sizeof(int64_t);
  v150 = v145 + v149;
  ;
  v151 = v17;
  v152 = sizeof(int32_t);
  v153 = &v151;
  memcpy(v150, v153, v152);
  v154 = sizeof(int32_t);
  v155 = v150 + v154;
  ;
  v156 = v18;
  v157 = sizeof(int32_t);
  v158 = &v156;
  memcpy(v155, v158, v157);
  v159 = sizeof(int32_t);
  v160 = v155 + v159;
  v161 = (iree_vm_ref_t*) v160;
  iree_vm_ref_assign(v19, v161);
  v162 = sizeof(iree_vm_ref_t);
  v163 = v160 + v162;
  ;
  v164 = v20;
  v165 = sizeof(int64_t);
  v166 = &v164;
  memcpy(v163, v166, v165);
  v167 = sizeof(int64_t);
  v168 = v163 + v167;
  ;
  v169 = v21;
  v170 = sizeof(int64_t);
  v171 = &v169;
  memcpy(v168, v171, v170);
  v172 = sizeof(int64_t);
  v173 = v168 + v172;
  ;
  v174 = v22;
  v175 = sizeof(int32_t);
  v176 = &v174;
  memcpy(v173, v176, v175);
  v177 = sizeof(int32_t);
  v178 = v173 + v177;
  ;
  v179 = v23;
  v180 = sizeof(int32_t);
  v181 = &v179;
  memcpy(v178, v181, v180);
  v182 = sizeof(int32_t);
  v183 = v178 + v182;
  v184 = (iree_vm_ref_t*) v183;
  iree_vm_ref_assign(v24, v184);
  v185 = sizeof(iree_vm_ref_t);
  v186 = v183 + v185;
  ;
  v187 = v25;
  v188 = sizeof(int64_t);
  v189 = &v187;
  memcpy(v186, v189, v188);
  v190 = sizeof(int64_t);
  v191 = v186 + v190;
  ;
  v192 = v26;
  v193 = sizeof(int64_t);
  v194 = &v192;
  memcpy(v191, v194, v193);
  v195 = v77->module->begin_call;
  v196 = v77->module;
  v197 = v80;
  v198 = EMITC_CALL_INDIRECT(v195, v196, v1, v197);
  v199 = (bool) v198;
  if (v199) {
    goto label5;
  } else {
    goto label3;
  }
label3:
  v200 = iree_ok_status();
  return v200;
label4:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  v201 = iree_make_status(IREE_STATUS_NOT_FOUND);
  return v201;
label5:
  iree_vm_ref_release(v3);
  iree_vm_ref_release(v4);
  return v198;
}
static iree_status_t be_u1000_micro_classifier_call_0rrr_iI_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5, int32_t* v6, int64_t* v7) {
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
static iree_status_t be_u1000_micro_classifier_call_0rIrrIiiII_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, int64_t v7, int32_t v8, int32_t v9, int64_t v10, int64_t v11, iree_vm_ref_t* v12) {
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
static iree_status_t be_u1000_micro_classifier_call_0rIrrrIrIII_v_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, iree_vm_ref_t* v9, int64_t v10, int64_t v11, int64_t v12) {
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
  int64_t v80;
  iree_host_size_t v81;
  int64_t* v82;
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
  v33 = sizeof(int64_t);
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
  v81 = sizeof(int64_t);
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
static iree_status_t be_u1000_micro_classifier_call_0rIrrrICrIID_v_2_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, int64_t v8, int32_t v9, iree_vm_ref_t* v10, int64_t v11, int64_t v12, iree_vm_ref_t* v13, int64_t v14, int64_t v15) {
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
static iree_status_t be_u1000_micro_classifier_call_0v_i_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t* v3) {
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
static iree_status_t be_u1000_micro_classifier_call_0i_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, iree_vm_ref_t* v4) {
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
static iree_status_t be_u1000_micro_classifier_call_0rIrrr_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5, iree_vm_ref_t* v6, iree_vm_ref_t* v7, iree_vm_ref_t* v8) {
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
static iree_status_t be_u1000_micro_classifier_call_0rI_r_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, iree_vm_ref_t* v3, int64_t v4, iree_vm_ref_t* v5) {
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
static iree_status_t be_u1000_micro_classifier_call_0iICrD_i_1_import_shim(iree_vm_stack_t* v1, iree_vm_function_t* v2, int32_t v3, int64_t v4, int32_t v5, iree_vm_ref_t* v6, int32_t* v7) {
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
static iree_status_t be_u1000_micro_classifier_predict(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3, iree_vm_ref_t* v4, iree_vm_ref_t* v5) {
  int32_t v6;
  int32_t v7;
  int64_t v8;
  int64_t v9;
  int32_t v10;
  int32_t v11;
  int32_t v12;
  int32_t v13;
  int32_t v14;
  int64_t v15;
  int64_t v16;
  int64_t v17;
  int64_t v18;
  int64_t v19;
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
  iree_vm_ref_t v32;
  iree_vm_ref_t* v33;
  iree_host_size_t v34;
  struct be_u1000_micro_classifier_state_t* v35;
  iree_vm_ref_t* v36;
  iree_vm_ref_t* v37;
  struct be_u1000_micro_classifier_t* v38;
  iree_vm_ref_type_t* v39;
  iree_vm_ref_type_t v40;
  iree_vm_type_def_t v41;
  iree_vm_ref_type_t v42;
  iree_status_t v43;
  bool v44;
  struct be_u1000_micro_classifier_state_t* v45;
  iree_vm_ref_t* v46;
  iree_vm_ref_t* v47;
  struct be_u1000_micro_classifier_t* v48;
  iree_vm_ref_type_t* v49;
  iree_vm_ref_type_t v50;
  iree_vm_type_def_t v51;
  iree_vm_ref_type_t v52;
  iree_status_t v53;
  bool v54;
  struct be_u1000_micro_classifier_state_t* v55;
  iree_vm_buffer_t* v56;
  iree_vm_buffer_t* v57;
  iree_vm_ref_type_t v58;
  iree_status_t v59;
  bool v60;
  struct be_u1000_micro_classifier_state_t* v61;
  iree_vm_function_t* v62;
  iree_vm_function_t* v63;
  iree_vm_ref_t v64;
  iree_vm_ref_t* v65;
  iree_host_size_t v66;
  iree_vm_ref_t v67;
  iree_vm_ref_t* v68;
  iree_host_size_t v69;
  iree_status_t v70;
  bool v71;
  struct be_u1000_micro_classifier_state_t* v72;
  iree_vm_function_t* v73;
  iree_vm_function_t* v74;
  iree_vm_ref_t v75;
  iree_vm_ref_t* v76;
  iree_host_size_t v77;
  iree_status_t v78;
  bool v79;
  struct be_u1000_micro_classifier_state_t* v80;
  iree_vm_function_t* v81;
  iree_vm_function_t* v82;
  iree_vm_ref_t v83;
  iree_vm_ref_t* v84;
  iree_host_size_t v85;
  iree_status_t v86;
  bool v87;
  struct be_u1000_micro_classifier_state_t* v88;
  iree_vm_buffer_t* v89;
  iree_vm_buffer_t* v90;
  iree_vm_ref_type_t v91;
  iree_status_t v92;
  bool v93;
  struct be_u1000_micro_classifier_state_t* v94;
  iree_vm_function_t* v95;
  iree_vm_function_t* v96;
  iree_vm_ref_t v97;
  iree_vm_ref_t* v98;
  iree_host_size_t v99;
  iree_vm_ref_t v100;
  iree_vm_ref_t* v101;
  iree_host_size_t v102;
  iree_vm_ref_t v103;
  iree_vm_ref_t* v104;
  iree_host_size_t v105;
  iree_status_t v106;
  bool v107;
  struct be_u1000_micro_classifier_state_t* v108;
  iree_vm_function_t* v109;
  iree_vm_function_t* v110;
  iree_vm_ref_t v111;
  iree_vm_ref_t* v112;
  iree_host_size_t v113;
  iree_status_t v114;
  bool v115;
  struct be_u1000_micro_classifier_state_t* v116;
  iree_vm_function_t* v117;
  iree_vm_function_t* v118;
  iree_vm_ref_t v119;
  iree_vm_ref_t* v120;
  iree_host_size_t v121;
  iree_vm_ref_t v122;
  iree_vm_ref_t* v123;
  iree_host_size_t v124;
  iree_vm_ref_t v125;
  iree_vm_ref_t* v126;
  iree_host_size_t v127;
  iree_status_t v128;
  bool v129;
  struct be_u1000_micro_classifier_state_t* v130;
  iree_vm_function_t* v131;
  iree_vm_function_t* v132;
  iree_vm_ref_t v133;
  iree_vm_ref_t* v134;
  iree_host_size_t v135;
  iree_status_t v136;
  bool v137;
  struct be_u1000_micro_classifier_state_t* v138;
  iree_vm_function_t* v139;
  iree_vm_function_t* v140;
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
  iree_vm_ref_t v156;
  iree_vm_ref_t* v157;
  iree_host_size_t v158;
  iree_status_t v159;
  bool v160;
  struct be_u1000_micro_classifier_state_t* v161;
  iree_vm_function_t* v162;
  iree_vm_function_t* v163;
  iree_vm_ref_t v164;
  iree_vm_ref_t* v165;
  iree_host_size_t v166;
  int32_t v167;
  int32_t* v168;
  iree_status_t v169;
  bool v170;
  int32_t v171;
  int32_t v172;
  bool v173;
  struct be_u1000_micro_classifier_state_t* v174;
  iree_vm_function_t* v175;
  iree_vm_function_t* v176;
  iree_vm_ref_t v177;
  iree_vm_ref_t* v178;
  iree_host_size_t v179;
  iree_status_t v180;
  bool v181;
  iree_vm_ref_t v182;
  iree_vm_ref_t* v183;
  iree_host_size_t v184;
  iree_status_t v185;
  bool v186;
  iree_status_t v187;
  iree_string_view_t v188;
  iree_string_view_t v189;
  iree_host_size_t v190;
  int v191;
  const char* v192;
  iree_status_t v193;
  v6 = 2;
  v7 = -1;
  v8 = 0;
  v9 = -1;
  v10 = 1;
  v11 = 16;
  v12 = 48;
  v13 = 3075;
  v14 = 553648160;
  v15 = 1;
  v16 = 4;
  v17 = 16;
  v18 = 8;
  v19 = 2;
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
  ;
  v33 = &v32;
  v34 = sizeof(iree_vm_ref_t);
  memset(v33, 0, v34);
  iree_vm_ref_release(v5);
  ;
  v35 = v3;
  v36 = v35->refs;
  v37 = &v36[0];
  ;
  v38 = v2;
  v39 = v38->types;
  v40 = v39[6];
  v41 = iree_vm_make_ref_type_def(v40);
  v42 = iree_vm_type_def_as_ref(v41);
  v43 = iree_vm_ref_retain_or_move_checked(false, v37, v42, v24);
  v44 = (bool) v43;
  if (v44) {
    goto label18;
  } else {
    goto label2;
  }
label2:
  ;
  v45 = v3;
  v46 = v45->refs;
  v47 = &v46[3];
  ;
  v48 = v2;
  v49 = v48->types;
  v50 = v49[5];
  v51 = iree_vm_make_ref_type_def(v50);
  v52 = iree_vm_type_def_as_ref(v51);
  v53 = iree_vm_ref_retain_or_move_checked(false, v47, v52, v27);
  v54 = (bool) v53;
  if (v54) {
    goto label19;
  } else {
    goto label3;
  }
label3:
  ;
  v55 = v3;
  v56 = v55->rodata_buffers;
  v57 = &v56[6];
  v58 = iree_vm_buffer_type();
  v59 = iree_vm_ref_wrap_retain(v57, v58, v30);
  v60 = (bool) v59;
  if (v60) {
    goto label20;
  } else {
    goto label4;
  }
label4:
  ;
  v61 = v3;
  v62 = v61->imports;
  v63 = &v62[5];
  ;
  v65 = &v64;
  v66 = sizeof(iree_vm_ref_t);
  memset(v65, 0, v66);
  iree_vm_ref_assign(v4, v65);
  ;
  v68 = &v67;
  v69 = sizeof(iree_vm_ref_t);
  memset(v68, 0, v69);
  iree_vm_ref_assign(v30, v68);
  v70 = be_u1000_micro_classifier_call_0rriiCID_v_2_import_shim(v1, v63, v65, v68, v14, v10, v6, v15, v16);
  v71 = (bool) v70;
  if (v71) {
    goto label21;
  } else {
    goto label5;
  }
label5:
  ;
  v72 = v3;
  v73 = v72->imports;
  v74 = &v73[6];
  ;
  v76 = &v75;
  v77 = sizeof(iree_vm_ref_t);
  memset(v76, 0, v77);
  iree_vm_ref_assign(v4, v76);
  v78 = be_u1000_micro_classifier_call_0r_r_import_shim(v1, v74, v76, v4);
  v79 = (bool) v78;
  if (v79) {
    goto label22;
  } else {
    goto label6;
  }
label6:
  ;
  v80 = v3;
  v81 = v80->imports;
  v82 = &v81[11];
  ;
  v84 = &v83;
  v85 = sizeof(iree_vm_ref_t);
  memset(v84, 0, v85);
  iree_vm_ref_assign(v24, v84);
  v86 = be_u1000_micro_classifier_call_0r_r_import_shim(v1, v82, v84, v30);
  v87 = (bool) v86;
  if (v87) {
    goto label23;
  } else {
    goto label7;
  }
label7:
  ;
  v88 = v3;
  v89 = v88->rodata_buffers;
  v90 = &v89[7];
  v91 = iree_vm_buffer_type();
  v92 = iree_vm_ref_wrap_retain(v90, v91, v33);
  v93 = (bool) v92;
  if (v93) {
    goto label24;
  } else {
    goto label8;
  }
label8:
  ;
  v94 = v3;
  v95 = v94->imports;
  v96 = &v95[3];
  ;
  v98 = &v97;
  v99 = sizeof(iree_vm_ref_t);
  memset(v98, 0, v99);
  iree_vm_ref_assign(v4, v98);
  ;
  v101 = &v100;
  v102 = sizeof(iree_vm_ref_t);
  memset(v101, 0, v102);
  iree_vm_ref_assign(v33, v101);
  ;
  v104 = &v103;
  v105 = sizeof(iree_vm_ref_t);
  memset(v104, 0, v105);
  iree_vm_ref_assign(v30, v104);
  v106 = be_u1000_micro_classifier_call_0rrrIii_v_import_shim(v1, v96, v98, v101, v104, v17, v11, v13);
  v107 = (bool) v106;
  if (v107) {
    goto label25;
  } else {
    goto label9;
  }
label9:
  ;
  v108 = v3;
  v109 = v108->imports;
  v110 = &v109[19];
  ;
  v112 = &v111;
  v113 = sizeof(iree_vm_ref_t);
  memset(v112, 0, v113);
  iree_vm_ref_assign(v24, v112);
  v114 = be_u1000_micro_classifier_call_0rI_r_import_shim(v1, v110, v112, v8, v30);
  v115 = (bool) v114;
  if (v115) {
    goto label26;
  } else {
    goto label10;
  }
label10:
  ;
  v116 = v3;
  v117 = v116->imports;
  v118 = &v117[13];
  ;
  v120 = &v119;
  v121 = sizeof(iree_vm_ref_t);
  memset(v120, 0, v121);
  iree_vm_ref_assign(v24, v120);
  ;
  v123 = &v122;
  v124 = sizeof(iree_vm_ref_t);
  memset(v123, 0, v124);
  iree_vm_ref_assign(v5, v123);
  ;
  v126 = &v125;
  v127 = sizeof(iree_vm_ref_t);
  memset(v126, 0, v127);
  iree_vm_ref_assign(v30, v126);
  v128 = be_u1000_micro_classifier_call_0rIrrIiiII_r_import_shim(v1, v118, v120, v9, v123, v126, v8, v12, v13, v18, v8, v5);
  v129 = (bool) v128;
  if (v129) {
    goto label27;
  } else {
    goto label11;
  }
label11:
  ;
  v130 = v3;
  v131 = v130->imports;
  v132 = &v131[19];
  ;
  v134 = &v133;
  v135 = sizeof(iree_vm_ref_t);
  memset(v134, 0, v135);
  iree_vm_ref_assign(v24, v134);
  v136 = be_u1000_micro_classifier_call_0rI_r_import_shim(v1, v132, v134, v8, v33);
  v137 = (bool) v136;
  if (v137) {
    goto label28;
  } else {
    goto label12;
  }
label12:
  ;
  v138 = v3;
  v139 = v138->imports;
  v140 = &v139[15];
  ;
  v142 = &v141;
  v143 = sizeof(iree_vm_ref_t);
  memset(v142, 0, v143);
  iree_vm_ref_assign(v24, v142);
  ;
  v145 = &v144;
  v146 = sizeof(iree_vm_ref_t);
  memset(v145, 0, v146);
  iree_vm_ref_assign(v30, v145);
  ;
  v148 = &v147;
  v149 = sizeof(iree_vm_ref_t);
  memset(v148, 0, v149);
  iree_vm_ref_assign(v33, v148);
  ;
  v151 = &v150;
  v152 = sizeof(iree_vm_ref_t);
  memset(v151, 0, v152);
  iree_vm_ref_assign(v27, v151);
  ;
  v154 = &v153;
  v155 = sizeof(iree_vm_ref_t);
  memset(v154, 0, v155);
  iree_vm_ref_assign(v4, v154);
  ;
  v157 = &v156;
  v158 = sizeof(iree_vm_ref_t);
  memset(v157, 0, v158);
  iree_vm_ref_assign(v5, v157);
  v159 = be_u1000_micro_classifier_call_0rIrrrICrIID_v_2_import_shim(v1, v140, v142, v9, v145, v148, v151, v8, v6, v154, v8, v17, v157, v8, v18);
  v160 = (bool) v159;
  if (v160) {
    goto label29;
  } else {
    goto label13;
  }
label13:
  ;
  v161 = v3;
  v162 = v161->imports;
  v163 = &v162[20];
  ;
  v165 = &v164;
  v166 = sizeof(iree_vm_ref_t);
  memset(v165, 0, v166);
  iree_vm_ref_assign(v33, v165);
  ;
  v168 = &v167;
  v169 = be_u1000_micro_classifier_call_0iICrD_i_1_import_shim(v1, v163, v7, v8, v10, v165, v168);
  v170 = (bool) v169;
  if (v170) {
    goto label30;
  } else {
    goto label14;
  }
label14:
  v171 = v167;
  v172 = vm_cmp_nz_i32(v171);
  v173 = (bool) v172;
  if (v173) {
    goto label17;
  } else {
    goto label15;
  }
label15:
  ;
  v174 = v3;
  v175 = v174->imports;
  v176 = &v175[4];
  ;
  v178 = &v177;
  v179 = sizeof(iree_vm_ref_t);
  memset(v178, 0, v179);
  iree_vm_ref_assign(v5, v178);
  v180 = be_u1000_micro_classifier_call_0rIIiiCID_r_2_import_shim(v1, v176, v178, v8, v18, v14, v10, v6, v15, v19, v4);
  v181 = (bool) v180;
  if (v181) {
    goto label33;
  } else {
    goto label16;
  }
label16:
  ;
  v183 = &v182;
  v184 = sizeof(iree_vm_ref_t);
  memset(v183, 0, v184);
  iree_vm_ref_move(v4, v183);
  iree_vm_ref_move(v183, v5);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  v185 = iree_ok_status();
  return v185;
label17:
  v186 = (bool) v171;
  if (v186) {
    goto label32;
  } else {
    goto label31;
  }
label18:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v43;
label19:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v53;
label20:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v59;
label21:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v70;
label22:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v78;
label23:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v86;
label24:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v92;
label25:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v106;
label26:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v114;
label27:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v128;
label28:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v136;
label29:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v159;
label30:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v169;
label31:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  v187 = iree_ok_status();
  return v187;
label32:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  v188 = iree_make_cstring_view("failed to wait on timepoint");
  ;
  v189 = v188;
  v190 = v189.size;
  v191 = (int) v190;
  v192 = v189.data;
  v193 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v191, v192);
  return v193;
label33:
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v33);
  iree_vm_ref_release(v4);
  return v180;
}
struct be_u1000_micro_classifier_predict_args_t {iree_vm_ref_t arg0;};
struct be_u1000_micro_classifier_predict_result_t {iree_vm_ref_t res0;};
static iree_status_t be_u1000_micro_classifier_predict_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct be_u1000_micro_classifier_t* v7;
  struct be_u1000_micro_classifier_state_t* v8;
  iree_byte_span_t v9;
  uint8_t* v10;
  struct be_u1000_micro_classifier_predict_args_t* v11;
  struct be_u1000_micro_classifier_predict_args_t* v12;
  iree_byte_span_t v13;
  uint8_t* v14;
  struct be_u1000_micro_classifier_predict_result_t* v15;
  struct be_u1000_micro_classifier_predict_result_t* v16;
  iree_vm_ref_t* v17;
  iree_vm_ref_t* v18;
  iree_status_t v19;
  bool v20;
  iree_status_t v21;
  v7 = (struct be_u1000_micro_classifier_t*) v5;
  v8 = (struct be_u1000_micro_classifier_state_t*) v6;
  ;
  v9 = v3;
  v10 = v9.data;
  v11 = (struct be_u1000_micro_classifier_predict_args_t*) v10;
  ;
  v12 = v11;
  ;
  v13 = v4;
  v14 = v13.data;
  v15 = (struct be_u1000_micro_classifier_predict_result_t*) v14;
  ;
  v16 = v15;
  v17 = &v12->arg0;
  iree_vm_ref_retain_inplace(v17);
  v18 = &v16->res0;
  v19 = be_u1000_micro_classifier_predict(v1, v7, v8, v17, v18);
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
static iree_status_t be_u1000_micro_classifier___init_export_shim(iree_vm_stack_t* v1, uint32_t v2, iree_byte_span_t v3, iree_byte_span_t v4, void* v5, void* v6) {
  struct be_u1000_micro_classifier_t* v7;
  struct be_u1000_micro_classifier_state_t* v8;
  iree_status_t v9;
  bool v10;
  iree_status_t v11;
  v7 = (struct be_u1000_micro_classifier_t*) v5;
  v8 = (struct be_u1000_micro_classifier_state_t*) v6;
  v9 = be_u1000_micro_classifier___init(v1, v7, v8);
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
static iree_status_t be_u1000_micro_classifier___init(iree_vm_stack_t* v1, struct be_u1000_micro_classifier_t* v2, struct be_u1000_micro_classifier_state_t* v3) {
  int32_t v4;
  int32_t v5;
  int64_t v6;
  int32_t v7;
  int32_t v8;
  int32_t v9;
  int32_t v10;
  int32_t v11;
  int64_t v12;
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
  struct be_u1000_micro_classifier_state_t* v32;
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
  struct be_u1000_micro_classifier_state_t* v49;
  iree_vm_function_t* v50;
  iree_vm_function_t* v51;
  iree_status_t v52;
  bool v53;
  struct be_u1000_micro_classifier_state_t* v54;
  iree_vm_buffer_t* v55;
  iree_vm_buffer_t* v56;
  iree_vm_ref_type_t v57;
  iree_status_t v58;
  bool v59;
  struct be_u1000_micro_classifier_state_t* v60;
  iree_vm_buffer_t* v61;
  iree_vm_buffer_t* v62;
  iree_vm_ref_type_t v63;
  iree_status_t v64;
  bool v65;
  struct be_u1000_micro_classifier_state_t* v66;
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
  struct be_u1000_micro_classifier_state_t* v90;
  iree_vm_buffer_t* v91;
  iree_vm_buffer_t* v92;
  iree_vm_ref_type_t v93;
  iree_status_t v94;
  bool v95;
  struct be_u1000_micro_classifier_state_t* v96;
  iree_vm_buffer_t* v97;
  iree_vm_buffer_t* v98;
  iree_vm_ref_type_t v99;
  iree_status_t v100;
  bool v101;
  struct be_u1000_micro_classifier_state_t* v102;
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
  struct be_u1000_micro_classifier_t* v128;
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
  struct be_u1000_micro_classifier_state_t* v146;
  iree_vm_buffer_t* v147;
  iree_vm_buffer_t* v148;
  iree_vm_ref_type_t v149;
  iree_status_t v150;
  bool v151;
  struct be_u1000_micro_classifier_state_t* v152;
  iree_vm_buffer_t* v153;
  iree_vm_buffer_t* v154;
  iree_vm_ref_type_t v155;
  iree_status_t v156;
  bool v157;
  struct be_u1000_micro_classifier_state_t* v158;
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
  struct be_u1000_micro_classifier_state_t* v182;
  iree_vm_ref_t* v183;
  iree_vm_ref_t* v184;
  struct be_u1000_micro_classifier_t* v185;
  iree_vm_ref_type_t* v186;
  iree_vm_ref_type_t v187;
  iree_vm_type_def_t v188;
  iree_vm_ref_type_t v189;
  iree_status_t v190;
  bool v191;
  int32_t v192;
  bool v193;
  struct be_u1000_micro_classifier_state_t* v194;
  iree_vm_buffer_t* v195;
  iree_vm_buffer_t* v196;
  iree_vm_ref_type_t v197;
  iree_status_t v198;
  bool v199;
  struct be_u1000_micro_classifier_state_t* v200;
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
  struct be_u1000_micro_classifier_state_t* v217;
  iree_vm_ref_t* v218;
  iree_vm_ref_t* v219;
  struct be_u1000_micro_classifier_t* v220;
  iree_vm_ref_type_t* v221;
  iree_vm_ref_type_t v222;
  iree_vm_type_def_t v223;
  iree_vm_ref_type_t v224;
  iree_status_t v225;
  bool v226;
  struct be_u1000_micro_classifier_state_t* v227;
  iree_vm_buffer_t* v228;
  iree_vm_buffer_t* v229;
  iree_vm_ref_type_t v230;
  iree_status_t v231;
  bool v232;
  struct be_u1000_micro_classifier_state_t* v233;
  iree_vm_function_t* v234;
  iree_vm_function_t* v235;
  iree_vm_ref_t v236;
  iree_vm_ref_t* v237;
  iree_host_size_t v238;
  iree_status_t v239;
  bool v240;
  struct be_u1000_micro_classifier_state_t* v241;
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
  struct be_u1000_micro_classifier_state_t* v253;
  iree_vm_ref_t* v254;
  iree_vm_ref_t* v255;
  struct be_u1000_micro_classifier_t* v256;
  iree_vm_ref_type_t* v257;
  iree_vm_ref_type_t v258;
  iree_vm_type_def_t v259;
  iree_vm_ref_type_t v260;
  iree_status_t v261;
  bool v262;
  int32_t v263;
  bool v264;
  bool v265;
  struct be_u1000_micro_classifier_state_t* v266;
  iree_vm_function_t* v267;
  iree_vm_function_t* v268;
  iree_vm_ref_t v269;
  iree_vm_ref_t* v270;
  iree_host_size_t v271;
  iree_status_t v272;
  bool v273;
  struct be_u1000_micro_classifier_state_t* v274;
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
  struct be_u1000_micro_classifier_state_t* v285;
  iree_vm_function_t* v286;
  iree_vm_function_t* v287;
  iree_vm_ref_t v288;
  iree_vm_ref_t* v289;
  iree_host_size_t v290;
  iree_status_t v291;
  bool v292;
  struct be_u1000_micro_classifier_state_t* v293;
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
  struct be_u1000_micro_classifier_state_t* v325;
  iree_vm_function_t* v326;
  iree_vm_function_t* v327;
  iree_vm_ref_t v328;
  iree_vm_ref_t* v329;
  iree_host_size_t v330;
  int32_t v331;
  int32_t* v332;
  iree_status_t v333;
  bool v334;
  int32_t v335;
  int32_t v336;
  bool v337;
  struct be_u1000_micro_classifier_state_t* v338;
  iree_vm_ref_t* v339;
  iree_vm_ref_t* v340;
  struct be_u1000_micro_classifier_t* v341;
  iree_vm_ref_type_t* v342;
  iree_vm_ref_type_t v343;
  iree_vm_type_def_t v344;
  iree_vm_ref_type_t v345;
  iree_status_t v346;
  bool v347;
  iree_status_t v348;
  bool v349;
  struct be_u1000_micro_classifier_state_t* v350;
  iree_vm_ref_t* v351;
  iree_vm_ref_t* v352;
  struct be_u1000_micro_classifier_t* v353;
  iree_vm_ref_type_t* v354;
  iree_vm_ref_type_t v355;
  iree_vm_type_def_t v356;
  iree_vm_ref_type_t v357;
  iree_status_t v358;
  bool v359;
  iree_status_t v360;
  bool v361;
  iree_status_t v362;
  iree_string_view_t v363;
  iree_string_view_t v364;
  iree_host_size_t v365;
  int v366;
  const char* v367;
  iree_status_t v368;
  iree_status_t v369;
  iree_string_view_t v370;
  iree_string_view_t v371;
  iree_host_size_t v372;
  int v373;
  const char* v374;
  iree_status_t v375;
  iree_status_t v376;
  iree_string_view_t v377;
  iree_string_view_t v378;
  iree_host_size_t v379;
  int v380;
  const char* v381;
  iree_status_t v382;
  int32_t v383;
  int32_t v384;
  int32_t v385;
  v4 = 1;
  v5 = 18;
  v6 = -1;
  v7 = 14;
  v8 = -1;
  v9 = 0;
  v10 = 48;
  v11 = 527363;
  v12 = 0;
  v13 = 128;
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
  v34 = &v33[16];
  ;
  v36 = &v35;
  v37 = be_u1000_micro_classifier_call_0v_i_import_shim(v1, v34, v36);
  v38 = (bool) v37;
  if (v38) {
    goto label46;
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
  v383 = v9;
  v384 = v9;
  goto label3;
label3:
  v43 = vm_cmp_nz_ref(v24);
  v44 = vm_xor_i32(v43, v4);
  v45 = vm_cmp_lt_i32s(v383, v39);
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
  v51 = &v50[17];
  v52 = be_u1000_micro_classifier_call_0i_r_import_shim(v1, v51, v383, v24);
  v53 = (bool) v52;
  if (v53) {
    goto label47;
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
    goto label48;
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
    goto label49;
  } else {
    goto label7;
  }
label7:
  ;
  v66 = v3;
  v67 = v66->imports;
  v68 = &v67[12];
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
  v82 = be_u1000_micro_classifier_call_0rrr_iI_import_shim(v1, v68, v70, v73, v76, v79, v81);
  v83 = (bool) v82;
  if (v83) {
    goto label50;
  } else {
    goto label8;
  }
label8:
  v84 = v78;
  v85 = v80;
  v86 = vm_cmp_nz_i64(v85);
  v87 = vm_select_i32(v84, v86, v9);
  v88 = vm_cmp_nz_i32(v87);
  v89 = (bool) v88;
  if (v89) {
    goto label9;
  } else {
    v385 = v9;
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
    goto label51;
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
    goto label52;
  } else {
    goto label11;
  }
label11:
  ;
  v102 = v3;
  v103 = v102->imports;
  v104 = &v103[12];
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
  v118 = be_u1000_micro_classifier_call_0rrr_iI_import_shim(v1, v104, v106, v109, v112, v115, v117);
  v119 = (bool) v118;
  if (v119) {
    goto label53;
  } else {
    goto label12;
  }
label12:
  v120 = v114;
  v121 = v116;
  v122 = vm_cmp_nz_i64(v121);
  v123 = vm_select_i32(v120, v122, v9);
  v385 = v123;
  goto label13;
label13:
  v124 = vm_cmp_eq_i32(v384, v9);
  v125 = vm_select_i32(v385, v4, v9);
  v126 = vm_add_i32(v384, v125);
  v127 = vm_and_i32(v385, v124);
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
    goto label54;
  } else {
    goto label16;
  }
label15:
  v137 = iree_vm_ref_retain_or_move_checked(false, v21, v132, v24);
  v138 = (bool) v137;
  if (v138) {
    goto label55;
  } else {
    goto label16;
  }
label16:
  v139 = vm_add_i32(v383, v4);
  ;
  v141 = &v140;
  v142 = sizeof(iree_vm_ref_t);
  memset(v141, 0, v142);
  iree_vm_ref_retain(v24, v141);
  iree_vm_ref_assign(v141, v24);
  v383 = v139;
  v384 = v126;
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
    goto label57;
  } else {
    goto label56;
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
    goto label58;
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
    goto label59;
  } else {
    goto label21;
  }
label21:
  ;
  v158 = v3;
  v159 = v158->imports;
  v160 = &v159[12];
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
  v174 = be_u1000_micro_classifier_call_0rrr_iI_import_shim(v1, v160, v162, v165, v168, v171, v173);
  v175 = (bool) v174;
  if (v175) {
    goto label60;
  } else {
    goto label22;
  }
label22:
  v176 = v170;
  v177 = v172;
  v178 = vm_cmp_nz_i64(v177);
  v179 = vm_select_i32(v176, v178, v9);
  v180 = vm_select_i32(v179, v9, v8);
  v181 = vm_cmp_eq_i32(v180, v9);
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
    goto label61;
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
    goto label62;
  } else {
    goto label25;
  }
label25:
  ;
  v200 = v3;
  v201 = v200->imports;
  v202 = &v201[18];
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
  v215 = be_u1000_micro_classifier_call_0rIrrr_r_import_shim(v1, v202, v204, v6, v207, v210, v213, v15);
  v216 = (bool) v215;
  if (v216) {
    goto label63;
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
    goto label64;
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
    goto label65;
  } else {
    goto label28;
  }
label28:
  ;
  v233 = v3;
  v234 = v233->imports;
  v235 = &v234[11];
  ;
  v237 = &v236;
  v238 = sizeof(iree_vm_ref_t);
  memset(v237, 0, v238);
  iree_vm_ref_assign(v21, v237);
  v239 = be_u1000_micro_classifier_call_0r_r_import_shim(v1, v235, v237, v27);
  v240 = (bool) v239;
  if (v240) {
    goto label66;
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
  v250 = be_u1000_micro_classifier_call_0riIiirII_r_import_shim(v1, v243, v245, v4, v6, v10, v11, v248, v12, v13, v30);
  v251 = (bool) v250;
  if (v251) {
    goto label67;
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
    goto label68;
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
  v265 = (bool) v7;
  if (v265) {
    goto label80;
  } else {
    goto label79;
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
  v272 = be_u1000_micro_classifier_call_0rIiiI_r_import_shim(v1, v268, v270, v6, v10, v11, v13, v15);
  v273 = (bool) v272;
  if (v273) {
    goto label75;
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
  v283 = be_u1000_micro_classifier_call_0rIirIIi_r_import_shim(v1, v276, v278, v6, v4, v281, v12, v13, v9, v24);
  v284 = (bool) v283;
  if (v284) {
    goto label76;
  } else {
    goto label35;
  }
label35:
  ;
  v285 = v3;
  v286 = v285->imports;
  v287 = &v286[19];
  ;
  v289 = &v288;
  v290 = sizeof(iree_vm_ref_t);
  memset(v289, 0, v290);
  iree_vm_ref_assign(v21, v289);
  v291 = be_u1000_micro_classifier_call_0rI_r_import_shim(v1, v287, v289, v12, v27);
  v292 = (bool) v291;
  if (v292) {
    goto label77;
  } else {
    goto label36;
  }
label36:
  ;
  v293 = v3;
  v294 = v293->imports;
  v295 = &v294[14];
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
  v311 = be_u1000_micro_classifier_call_0rIrrrIrIII_v_import_shim(v1, v295, v297, v6, v300, v303, v306, v12, v309, v12, v13, v12);
  v312 = (bool) v311;
  if (v312) {
    goto label78;
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
  v326 = v325->imports;
  v327 = &v326[20];
  ;
  v329 = &v328;
  v330 = sizeof(iree_vm_ref_t);
  memset(v329, 0, v330);
  iree_vm_ref_assign(v15, v329);
  ;
  v332 = &v331;
  v333 = be_u1000_micro_classifier_call_0iICrD_i_1_import_shim(v1, v327, v8, v12, v4, v329, v332);
  v334 = (bool) v333;
  if (v334) {
    goto label69;
  } else {
    goto label40;
  }
label40:
  v335 = v331;
  v336 = vm_cmp_nz_i32(v335);
  v337 = (bool) v336;
  if (v337) {
    goto label45;
  } else {
    goto label41;
  }
label41:
  ;
  v338 = v3;
  v339 = v338->refs;
  v340 = &v339[2];
  ;
  v341 = v2;
  v342 = v341->types;
  v343 = v342[3];
  v344 = iree_vm_make_ref_type_def(v343);
  v345 = iree_vm_type_def_as_ref(v344);
  v346 = iree_vm_ref_retain_or_move_checked(false, v18, v345, v340);
  v347 = (bool) v346;
  if (v347) {
    goto label72;
  } else {
    goto label42;
  }
label42:
  v348 = be_u1000_micro_classifier___predict_memoize_apply(v1, v2, v3, v15);
  v349 = (bool) v348;
  if (v349) {
    goto label73;
  } else {
    goto label43;
  }
label43:
  ;
  v350 = v3;
  v351 = v350->refs;
  v352 = &v351[3];
  ;
  v353 = v2;
  v354 = v353->types;
  v355 = v354[5];
  v356 = iree_vm_make_ref_type_def(v355);
  v357 = iree_vm_type_def_as_ref(v356);
  v358 = iree_vm_ref_retain_or_move_checked(false, v15, v357, v352);
  v359 = (bool) v358;
  if (v359) {
    goto label74;
  } else {
    goto label44;
  }
label44:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v360 = iree_ok_status();
  return v360;
label45:
  v361 = (bool) v335;
  if (v361) {
    goto label71;
  } else {
    goto label70;
  }
label46:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v37;
label47:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v52;
label48:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v58;
label49:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v64;
label50:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v82;
label51:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v94;
label52:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v100;
label53:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v118;
label54:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v135;
label55:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v137;
label56:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v362 = iree_ok_status();
  return v362;
label57:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v363 = iree_make_cstring_view("HAL device `__device_0` not found or unavailable: #hal.device.target<\"local\", [#hal.executable.target<\"llvm-cpu\", \"static\", {cpu = \"\", cpu_features = \"+m,+a,+f,+zifencei\", data_layout = \"e-m:e-p:32:32-i64:64-n32-S128\", debug_symbols = false, iree.encoding.resolver = #iree_cpu.cpu_encoding_resolver<>, link_embedded = false, link_static = true, max_stack_allocation_size = 32768 : i64, native_vector_size = 16 : i64, static_library_output = \"/home/mingshi/Project/PF/rrtos/apps/be_u1000_ai_micro_demo/generated/be_u1000_micro_classifier.o\", target_abi = \"ilp32f\", target_triple = \"riscv32-unknown-elf\"}>]>");
  ;
  v364 = v363;
  v365 = v364.size;
  v366 = (int) v365;
  v367 = v364.data;
  v368 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v366, v367);
  return v368;
label58:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v150;
label59:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v156;
label60:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v174;
label61:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v190;
label62:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v198;
label63:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v215;
label64:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v225;
label65:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v231;
label66:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v239;
label67:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v250;
label68:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v261;
label69:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v333;
label70:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v369 = iree_ok_status();
  return v369;
label71:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v370 = iree_make_cstring_view("failed to wait on timepoint");
  ;
  v371 = v370;
  v372 = v371.size;
  v373 = (int) v372;
  v374 = v371.data;
  v375 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v373, v374);
  return v375;
label72:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v346;
label73:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v348;
label74:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v358;
label75:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v272;
label76:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v283;
label77:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v291;
label78:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  return v311;
label79:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v376 = iree_ok_status();
  return v376;
label80:
  iree_vm_ref_release(v15);
  iree_vm_ref_release(v21);
  iree_vm_ref_release(v27);
  iree_vm_ref_release(v18);
  iree_vm_ref_release(v24);
  iree_vm_ref_release(v30);
  v377 = iree_make_cstring_view("HAL device `__device_0` does not support any variant of executable `predict_dispatch_0`; available formats: [static]");
  ;
  v378 = v377;
  v379 = v378.size;
  v380 = (int) v379;
  v381 = v378.data;
  v382 = iree_status_allocate_f(IREE_STATUS_FAILED_PRECONDITION, "<vm>", 0, "%.*s", v380, v381);
  return v382;
}
static void be_u1000_micro_classifier_destroy(void* v1) {
  struct be_u1000_micro_classifier_t* v2;
  struct be_u1000_micro_classifier_t* v3;
  iree_allocator_t v4;
  v2 = (struct be_u1000_micro_classifier_t*) v1;
  ;
  v3 = v2;
  v4 = v3->allocator;
  iree_allocator_free(v4, v2);
  return;
}
static iree_status_t be_u1000_micro_classifier_alloc_state(void* v1, iree_allocator_t v2, iree_vm_module_state_t** v3) {
  struct be_u1000_micro_classifier_state_t* v4;
  iree_host_size_t v5;
  struct be_u1000_micro_classifier_state_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct be_u1000_micro_classifier_state_t* v10;
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
  v5 = sizeof(struct be_u1000_micro_classifier_state_t);
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
  v11 = be_u1000_micro_classifier__utf8_hal_device_id_C1DCB7DBC4F49AE6;
  v12 = v11;
  v13 = (void*) v12;
  v14 = sizeof(be_u1000_micro_classifier__utf8_hal_device_id_C1DCB7DBC4F49AE6);
  v15 = iree_make_byte_span(v13, v14);
  v16 = iree_allocator_null();
  v17 = v4->rodata_buffers;
  v18 = &v17[0];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v15, v16, v18);
  v19 = be_u1000_micro_classifier__utf8_local_3D3B5147158B235A;
  v20 = v19;
  v21 = (void*) v20;
  v22 = sizeof(be_u1000_micro_classifier__utf8_local_3D3B5147158B235A);
  v23 = iree_make_byte_span(v21, v22);
  v24 = iree_allocator_null();
  v25 = v4->rodata_buffers;
  v26 = &v25[1];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v23, v24, v26);
  v27 = be_u1000_micro_classifier__utf8_hal_executable_format_EAB228F999C2D3A1;
  v28 = v27;
  v29 = (void*) v28;
  v30 = sizeof(be_u1000_micro_classifier__utf8_hal_executable_format_EAB228F999C2D3A1);
  v31 = iree_make_byte_span(v29, v30);
  v32 = iree_allocator_null();
  v33 = v4->rodata_buffers;
  v34 = &v33[2];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v31, v32, v34);
  v35 = be_u1000_micro_classifier__utf8_static_96B31E405495E0B6;
  v36 = v35;
  v37 = (void*) v36;
  v38 = sizeof(be_u1000_micro_classifier__utf8_static_96B31E405495E0B6);
  v39 = iree_make_byte_span(v37, v38);
  v40 = iree_allocator_null();
  v41 = v4->rodata_buffers;
  v42 = &v41[3];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v39, v40, v42);
  v43 = be_u1000_micro_classifier_predict_dispatch_0_static;
  v44 = v43;
  v45 = (void*) v44;
  v46 = sizeof(be_u1000_micro_classifier_predict_dispatch_0_static);
  v47 = iree_make_byte_span(v45, v46);
  v48 = iree_allocator_null();
  v49 = v4->rodata_buffers;
  v50 = &v49[4];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v47, v48, v50);
  v51 = be_u1000_micro_classifier__const;
  v52 = v51;
  v53 = (void*) v52;
  v54 = sizeof(be_u1000_micro_classifier__const);
  v55 = iree_make_byte_span(v53, v54);
  v56 = iree_allocator_null();
  v57 = v4->rodata_buffers;
  v58 = &v57[5];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v55, v56, v58);
  v59 = be_u1000_micro_classifier__utf8_input0_F74E046E5FFA3735;
  v60 = v59;
  v61 = (void*) v60;
  v62 = sizeof(be_u1000_micro_classifier__utf8_input0_F74E046E5FFA3735);
  v63 = iree_make_byte_span(v61, v62);
  v64 = iree_allocator_null();
  v65 = v4->rodata_buffers;
  v66 = &v65[6];
  iree_vm_buffer_initialize(IREE_VM_BUFFER_ACCESS_ORIGIN_MODULE, v63, v64, v66);
  v67 = be_u1000_micro_classifier__utf8_tensor_3C6209B4FD120BDC;
  v68 = v67;
  v69 = (void*) v68;
  v70 = sizeof(be_u1000_micro_classifier__utf8_tensor_3C6209B4FD120BDC);
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
static void be_u1000_micro_classifier_free_state(void* v1, iree_vm_module_state_t* v2) {
  struct be_u1000_micro_classifier_state_t* v3;
  struct be_u1000_micro_classifier_state_t* v4;
  iree_vm_ref_t* v5;
  iree_vm_ref_t* v6;
  iree_vm_ref_t* v7;
  iree_vm_ref_t* v8;
  iree_vm_ref_t* v9;
  iree_allocator_t v10;
  v3 = (struct be_u1000_micro_classifier_state_t*) v2;
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
static iree_status_t be_u1000_micro_classifier_fork_state(void* v1, iree_vm_module_state_t* v2, iree_allocator_t v3, iree_vm_module_state_t** v4) {
  iree_status_t v5;
  v5 = iree_make_status(IREE_STATUS_UNIMPLEMENTED);
  return v5;
}
static iree_status_t be_u1000_micro_classifier_resolve_import(void* v1, iree_vm_module_state_t* v2, iree_host_size_t v3, const iree_vm_function_t* v4, const iree_vm_function_signature_t* v5) {
  struct be_u1000_micro_classifier_state_t* v6;
  struct be_u1000_micro_classifier_state_t* v7;
  iree_vm_function_t* v8;
  iree_vm_function_t* v9;
  iree_status_t v10;
  v6 = (struct be_u1000_micro_classifier_state_t*) v2;
  ;
  v7 = v6;
  v8 = v7->imports;
  v9 = &v8[v3];
  EMITC_DEREF_ASSIGN_PTR(v9, v4);
  v10 = iree_ok_status();
  return v10;
}
iree_status_t be_u1000_micro_classifier_create(iree_vm_instance_t* v1, iree_allocator_t v2, iree_vm_module_t** v3) {
  struct be_u1000_micro_classifier_t* v4;
  iree_host_size_t v5;
  struct be_u1000_micro_classifier_t** v6;
  void** v7;
  iree_status_t v8;
  bool v9;
  struct be_u1000_micro_classifier_t* v10;
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
  struct be_u1000_micro_classifier_t* v36;
  iree_status_t v37;
  bool v38;
  iree_status_t v39;
  struct be_u1000_micro_classifier_t* v40;
  v4 = NULL;
  v5 = sizeof(struct be_u1000_micro_classifier_t);
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
  v34.destroy = be_u1000_micro_classifier_destroy;
  v34.alloc_state = be_u1000_micro_classifier_alloc_state;
  v34.free_state = be_u1000_micro_classifier_free_state;
  v34.fork_state = be_u1000_micro_classifier_fork_state;
  v34.resolve_import = be_u1000_micro_classifier_resolve_import;
  v39 = iree_vm_native_module_create(v35, &be_u1000_micro_classifier_descriptor_, v1, v2, v3);
  return v39;
label4:
  return v8;
label5:
  v40 = v4;
  iree_allocator_free(v2, v40);
  return v37;
}
#endif   // EMITC_IMPLEMENTATION
