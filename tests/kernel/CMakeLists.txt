cmake_minimum_required(VERSION 3.16)

project(rrtos_kernel_semantics LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(kernel_semantics_tests
    ${REPO_ROOT}/kernel/src/os_sched.c
    ${REPO_ROOT}/kernel/src/os_ipc.c
    ${REPO_ROOT}/kernel/src/os_timer.c
    support/kernel_test_runtime.c
    test_main.c
    test_harness_canary.c
    test_sched_semantics.c
    test_ipc_semantics.c
    test_timer_semantics.c
    test_baseline_metrics.c
    test_microbench_metrics.c
)

target_compile_definitions(kernel_semantics_tests PRIVATE
    OS_TEST_HARNESS=1
    OS_CFG_ARCH_BITS=64
    OS_CFG_SMP_EN=0
    OS_CFG_AI_EN=0
    OS_CFG_FL_EN=0
    OS_CFG_CPU_MAX=1
    OS_CFG_CPU_COUNT=1
    OS_CFG_CPU_FREQ_HZ=100000000UL
    OS_CFG_TIMER_FREQ_HZ=10000000UL
)

target_include_directories(kernel_semantics_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/support/include
    ${REPO_ROOT}/kernel/include
    ${REPO_ROOT}/config
    ${REPO_ROOT}/hal/include
)

target_compile_options(kernel_semantics_tests PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unused-variable
)

add_test(NAME kernel-harness-canary COMMAND kernel_semantics_tests --suite canary)
add_test(NAME kernel-semantic-red COMMAND kernel_semantics_tests --suite core)
