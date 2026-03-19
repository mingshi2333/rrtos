#include <stdio.h>

int os_test_suite_canary(void);
int os_test_suite_sched_semantics(void);
int os_test_suite_ipc_semantics(void);
int os_test_suite_timer_semantics(void);
int os_test_suite_baseline_metrics(void);
int os_test_suite_microbench_metrics(void);

static int arg_eq(const char *lhs, const char *rhs) {
    while (*lhs && *rhs) {
        if (*lhs != *rhs) {
            return 0;
        }
        lhs++;
        rhs++;
    }

    return *lhs == *rhs;
}

static int run_suite(const char *name, int (*fn)(void)) {
    int rc = fn();
    printf("[%s] %s\n", rc == 0 ? "PASS" : "FAIL", name);
    return rc;
}

int main(int argc, char **argv) {
    const char *suite = "all";
    int failures = 0;

    if (argc == 3 && arg_eq(argv[1], "--suite")) {
        suite = argv[2];
    }

    if (arg_eq(suite, "all") || arg_eq(suite, "canary")) {
        failures += run_suite("canary", os_test_suite_canary);
    }

    if (arg_eq(suite, "all") || arg_eq(suite, "core") || arg_eq(suite, "red")) {
        failures += run_suite("sched-timeout-oracle", os_test_suite_sched_semantics);
        failures += run_suite("ipc-semantics", os_test_suite_ipc_semantics);
        failures += run_suite("timer-semantics", os_test_suite_timer_semantics);
    }

    if (arg_eq(suite, "baseline")) {
        failures += run_suite("baseline-metrics", os_test_suite_baseline_metrics);
    }

    if (arg_eq(suite, "microbench")) {
        failures += run_suite("microbench-metrics", os_test_suite_microbench_metrics);
    }

    if (arg_eq(suite, "performance")) {
        failures += run_suite("baseline-metrics", os_test_suite_baseline_metrics);
        failures += run_suite("microbench-metrics", os_test_suite_microbench_metrics);
    }

    if (failures > 0) {
        fprintf(stderr, "%d suite(s) failed\n", failures);
        return 1;
    }

    return 0;
}
