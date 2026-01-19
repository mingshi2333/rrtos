set(IREE_RUNTIME_ROOT ${CMAKE_SOURCE_DIR}/third_party/iree/runtime/src)

include_directories(${IREE_RUNTIME_ROOT})

# Corrected sources for EmitC runtime based on file existence
set(IREE_RUNTIME_SOURCES
    ${IREE_RUNTIME_ROOT}/iree/base/allocator.c
    ${IREE_RUNTIME_ROOT}/iree/base/status.c
    ${IREE_RUNTIME_ROOT}/iree/base/string_builder.c
    ${IREE_RUNTIME_ROOT}/iree/base/string_view.c
    ${IREE_RUNTIME_ROOT}/iree/base/time.c
    ${IREE_RUNTIME_ROOT}/iree/base/loop_sync.c
    
    # VM Core
    ${IREE_RUNTIME_ROOT}/iree/vm/context.c
    ${IREE_RUNTIME_ROOT}/iree/vm/instance.c
    ${IREE_RUNTIME_ROOT}/iree/vm/invocation.c
    ${IREE_RUNTIME_ROOT}/iree/vm/list.c
    ${IREE_RUNTIME_ROOT}/iree/vm/module.c
    ${IREE_RUNTIME_ROOT}/iree/vm/native_module.c
    ${IREE_RUNTIME_ROOT}/iree/vm/ref.c
    ${IREE_RUNTIME_ROOT}/iree/vm/stack.c
    ${IREE_RUNTIME_ROOT}/iree/vm/buffer.c
    ${IREE_RUNTIME_ROOT}/iree/vm/shims.c
    
    # EmitC specific - REMOVED: ${IREE_RUNTIME_ROOT}/iree/vm/module_impl_emitc.c
    # The EmitC implementation is already in iree_vec_add_c_module.c
)

# Configuration flags for bare-metal - moved to CMakeLists.txt target_compile_definitions
# These flags MUST be set on rv_aios_ai target only, not globally!
