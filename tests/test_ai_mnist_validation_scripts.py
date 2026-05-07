import unittest
from pathlib import Path

from scripts.run_mnist_validation import validate_ai_validation_output
from scripts.run_mnist_quant_validation import validate_mnist_quant_validation_output
from scripts.run_miniresnet_validation import validate_miniresnet_validation_output


PROJECT_ROOT = Path(__file__).resolve().parent.parent


VALID_LOG = "\n".join(
    [
        "Booting AI validation: mnist_validation...",
        "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=2",
        "AI_VALIDATION_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
        "AI_VALIDATION_METRICS: sample=a idx=0 label=7 argmax=7 hash=1 top_score_q=10 latency_us=2 latency_cycles=200 latency_instructions=100 total=1 arena_peak=0",
        "AI_VALIDATION_METRICS: sample=b idx=1 label=2 argmax=2 hash=2 top_score_q=11 latency_us=3 latency_cycles=300 latency_instructions=150 total=1 arena_peak=0",
        "AI_VALIDATION_MODEL_PEAK: phase=invoke heap_current_bytes=120 heap_peak_bytes=300 heap_alloc_count=5 heap_free_count=4",
        "AI_VALIDATION_PASS count=2",
        "OS_TIMER_CALLBACK_PASS count=1 tick=26",
    ]
)


class MnistValidationScriptTest(unittest.TestCase):
    def test_qemu_test_linker_is_large_and_not_the_default_or_be_u1000_contract(self):
        default_linker = (PROJECT_ROOT / "arch" / "riscv" / "link.ld").read_text()
        qemu_test_linker = PROJECT_ROOT / "arch" / "riscv" / "link_qemu_test.ld"
        be_u1000_flash_linker = (
            PROJECT_ROOT / "boards" / "be_u1000" / "link_be_u1000_flash.ld"
        ).read_text()

        self.assertIn("RAM (rwx)   : ORIGIN = 0x80000000, LENGTH = 4M", default_linker)
        self.assertIn("FLASH (rx)  : ORIGIN = 0x20000000, LENGTH = 2M", default_linker)
        self.assertIn("TCMB   (rwx) : ORIGIN = 0x40020000, LENGTH = 96K", be_u1000_flash_linker)
        self.assertNotIn("TCM    (rwx)", be_u1000_flash_linker)

        qemu_test_text = qemu_test_linker.read_text()
        self.assertIn("QEMU-only", qemu_test_text)
        self.assertIn("RAM (rwx)   : ORIGIN = 0x80000000, LENGTH = 64M", qemu_test_text)
        self.assertIn("FLASH (rx)  : ORIGIN = 0x20000000, LENGTH = 64M", qemu_test_text)

    def test_miniresnet_runner_accepts_qemu_memory_size_argument(self):
        runner_text = (PROJECT_ROOT / "scripts" / "run_miniresnet_validation.py").read_text()

        self.assertIn('"--memory",', runner_text)
        self.assertIn('"--memory 64M"', runner_text)
        self.assertIn("args.memory", runner_text)

    def test_mnist_runners_accept_qemu_memory_size_argument(self):
        for runner_name in (
            "run_mnist_validation.py",
            "run_mnist_static_direct_validation.py",
            "run_mnist_descriptor_validation.py",
        ):
            with self.subTest(runner=runner_name):
                runner_text = (PROJECT_ROOT / "scripts" / runner_name).read_text()

                self.assertIn('"--memory",', runner_text)
                self.assertIn('"--memory 64M"', runner_text)
                self.assertIn("args.memory", runner_text)

    def test_pixi_exposes_qemu_only_miniresnet_test_lane(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("configure-qemu-test", pixi_text)
        self.assertIn("-DLINKER_SCRIPT=arch/riscv/link_qemu_test.ld", pixi_text)
        self.assertIn("build-miniresnet-qemu-test", pixi_text)
        self.assertIn("validate-miniresnet-qemu-test", pixi_text)
        self.assertIn("--memory 64M", pixi_text)

    def test_pixi_exposes_qemu_only_mnist_descriptor_test_lane(self):
        pixi_text = (PROJECT_ROOT / "pixi.toml").read_text()

        self.assertIn("build-mnist-descriptor-qemu-test", pixi_text)
        self.assertIn("validate-mnist-descriptor-qemu-test", pixi_text)
        self.assertIn("mnist_descriptor_validation", pixi_text)
        self.assertIn("run_mnist_descriptor_validation.py", pixi_text)

    def test_qemu_mnist_vmvx_validation_lane_is_declared(self):
        config_text = (PROJECT_ROOT / "ai_models_mnist_vmvx.yaml").read_text()
        cmake_text = (PROJECT_ROOT / "apps" / "mnist_app" / "CMakeLists.txt").read_text()

        self.assertIn("output_dir: \"apps/mnist_app/generated_mnist_vmvx\"", config_text)
        self.assertIn("iree_backend: \"vmvx_inline\"", config_text)
        self.assertIn("add_subdirectory(generated_mnist_vmvx)", cmake_text)
        self.assertIn("rv_aios_mnist_vmvx_models", cmake_text)
        self.assertIn("mnist_vmvx_validation", cmake_text)

    def test_accepts_matching_pass_count_metrics_and_timer(self):
        self.assertEqual(validate_ai_validation_output(VALID_LOG), [])

    def test_rejects_fail_token_even_with_pass_token(self):
        errors = validate_ai_validation_output(
            VALID_LOG + "\nAI_VALIDATION_FAIL: sample=a code=1\n"
        )
        self.assertIn("AI_VALIDATION_FAIL token present", errors)

    def test_rejects_pass_count_metric_mismatch(self):
        errors = validate_ai_validation_output(
            VALID_LOG.replace("AI_VALIDATION_PASS count=2", "AI_VALIDATION_PASS count=3")
        )
        self.assertIn("AI_VALIDATION_METRICS count 2 does not match pass count 3", errors)

    def test_rejects_missing_metrics(self):
        errors = validate_ai_validation_output(
            "\n".join(
                [
                    "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=1",
                    "AI_VALIDATION_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
                    "AI_VALIDATION_MODEL_PEAK: phase=invoke heap_current_bytes=120 heap_peak_bytes=300 heap_alloc_count=5 heap_free_count=4",
                    "AI_VALIDATION_PASS count=1",
                    "OS_TIMER_CALLBACK_PASS count=1 tick=26",
                ]
            )
        )
        self.assertIn("AI_VALIDATION_METRICS count 0 does not match pass count 1", errors)

    def test_rejects_missing_invoke_heap_peak(self):
        errors = validate_ai_validation_output(
            "\n".join(
                [
                    "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=1",
                    "AI_VALIDATION_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
                    "AI_VALIDATION_METRICS: sample=a idx=0 label=7 argmax=7 hash=1 top_score_q=10 latency_us=2 latency_cycles=200 latency_instructions=100 total=1 arena_peak=0",
                    "AI_VALIDATION_PASS count=1",
                    "OS_TIMER_CALLBACK_PASS count=1 tick=26",
                ]
            )
        )

        self.assertIn("missing AI_VALIDATION_MODEL_PEAK phase=invoke", errors)

    def test_rejects_metrics_without_cycle_latency(self):
        errors = validate_ai_validation_output(
            "\n".join(
                [
                    "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=1",
                    "AI_VALIDATION_METRICS: sample=a idx=0 label=7 argmax=7 hash=1 top_score_q=10 latency_us=2 total=1 arena_peak=0",
                    "AI_VALIDATION_PASS count=1",
                    "OS_TIMER_CALLBACK_PASS count=1 tick=26",
                ]
            )
        )
        self.assertIn("AI_VALIDATION_METRICS count 0 does not match pass count 1", errors)

    def test_rejects_metrics_without_instruction_latency(self):
        errors = validate_ai_validation_output(
            "\n".join(
                [
                    "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=1",
                    "AI_VALIDATION_METRICS: sample=a idx=0 label=7 argmax=7 hash=1 top_score_q=10 latency_us=2 latency_cycles=200 total=1 arena_peak=0",
                    "AI_VALIDATION_PASS count=1",
                    "OS_TIMER_CALLBACK_PASS count=1 tick=26",
                ]
            )
        )
        self.assertIn("AI_VALIDATION_METRICS count 0 does not match pass count 1", errors)

    def test_rejects_multiple_pass_lines(self):
        errors = validate_ai_validation_output(VALID_LOG + "\nAI_VALIDATION_PASS count=2\n")
        self.assertIn("multiple AI_VALIDATION_PASS count lines", errors)

    def test_quant_validator_accepts_one_matching_result(self):
        log = "\n".join(
            [
                "Booting ST MNISTv1 quant validation...",
                "ST_MNISTV1_QUANT_SAMPLESET: name=mnist_t10k_official count=1",
                "ST_MNISTV1_QUANT_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_score_q=80078 latency_us=54375 latency_cycles=5437500 latency_instructions=123456 total=1",
                "ST_MNISTV1_QUANT_PASS count=1",
            ]
        )

        self.assertEqual(validate_mnist_quant_validation_output(log), [])

    def test_quant_validator_rejects_fail_token(self):
        errors = validate_mnist_quant_validation_output(
            "ST_MNISTV1_QUANT_FAIL: inference failed\n"
        )

        self.assertIn("ST_MNISTV1_QUANT_FAIL token present", errors)

    def test_quant_validator_rejects_result_without_cycle_latency(self):
        errors = validate_mnist_quant_validation_output(
            "\n".join(
                [
                    "ST_MNISTV1_QUANT_SAMPLESET: name=mnist_t10k_official count=1",
                    "ST_MNISTV1_QUANT_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_score_q=80078 latency_us=54375 total=1",
                    "ST_MNISTV1_QUANT_PASS count=1",
                ]
            )
        )

        self.assertIn("ST_MNISTV1_QUANT_RESULT count 0 does not match pass count 1", errors)

    def test_quant_validator_rejects_result_without_instruction_latency(self):
        errors = validate_mnist_quant_validation_output(
            "\n".join(
                [
                    "ST_MNISTV1_QUANT_SAMPLESET: name=mnist_t10k_official count=1",
                    "ST_MNISTV1_QUANT_RESULT: sample=mnist_t10k_00000 label=7 argmax=7 top_score_q=80078 latency_us=54375 latency_cycles=5437500 total=1",
                    "ST_MNISTV1_QUANT_PASS count=1",
                ]
            )
        )

        self.assertIn("ST_MNISTV1_QUANT_RESULT count 0 does not match pass count 1", errors)

    def test_miniresnet_validator_accepts_one_matching_result(self):
        log = "\n".join(
            [
                "Booting MiniResNet validation...",
                "MINIRESNET_SAMPLESET: name=synthetic_pattern count=1",
                "MINIRESNET_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
                "MINIRESNET_MODEL_PEAK: phase=invoke heap_current_bytes=100 heap_peak_bytes=300 heap_alloc_count=2 heap_free_count=2",
                "MINIRESNET_HEAP_TRACE_PEAK: phase=invoke seq=1 size=80 current_used_bytes=300 ptr=4096 caller=8192",
                "MINIRESNET_HEAP_TRACE_TOP: phase=invoke rank=1 seq=1 size=80 current_used_bytes=300 ptr=4096 caller=8192",
                "MINIRESNET_HAL_TRACE_PEAK: phase=invoke seq=1 allocation_size=315448 requested_type=49 requested_usage=4278194175 requested_access=3 compat_type=54 compat_usage=4278194175 compat_access=3 alignment=64 buffer=1234",
                "MINIRESNET_HAL_TRACE_TOP: phase=invoke rank=1 seq=1 allocation_size=315448 requested_type=49 requested_usage=4278194175 requested_access=3 compat_type=54 compat_usage=4278194175 compat_access=3 alignment=64 buffer=1234",
                "MINIRESNET_RESULT: sample=synthetic_pattern argmax=3 hash=123 top_score_q=-42 latency_us=99 latency_cycles=9900 latency_instructions=8800 total=1",
                "MINIRESNET_PASS count=1",
            ]
        )

        self.assertEqual(validate_miniresnet_validation_output(log), [])

    def test_miniresnet_validator_rejects_missing_invoke_peak(self):
        errors = validate_miniresnet_validation_output(
            "\n".join(
                [
                    "MINIRESNET_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
                    "MINIRESNET_RESULT: sample=synthetic_pattern argmax=3 hash=123 top_score_q=42 latency_us=99 latency_cycles=9900 latency_instructions=8800 total=1",
                    "MINIRESNET_PASS count=1",
                ]
            )
        )

        self.assertIn("missing MINIRESNET_MODEL_PEAK phase=invoke", errors)

    def test_miniresnet_validator_rejects_missing_instruction_latency(self):
        errors = validate_miniresnet_validation_output(
            "\n".join(
                [
                    "MINIRESNET_MODEL_PEAK: phase=init heap_current_bytes=100 heap_peak_bytes=200 heap_alloc_count=3 heap_free_count=1",
                    "MINIRESNET_MODEL_PEAK: phase=invoke heap_current_bytes=100 heap_peak_bytes=300 heap_alloc_count=2 heap_free_count=2",
                    "MINIRESNET_HEAP_TRACE_PEAK: phase=invoke seq=1 size=80 current_used_bytes=300 ptr=4096 caller=8192",
                    "MINIRESNET_HEAP_TRACE_TOP: phase=invoke rank=1 seq=1 size=80 current_used_bytes=300 ptr=4096 caller=8192",
                    "MINIRESNET_HAL_TRACE_PEAK: phase=invoke seq=1 allocation_size=315448 requested_type=49 requested_usage=4278194175 requested_access=3 compat_type=54 compat_usage=4278194175 compat_access=3 alignment=64 buffer=1234",
                    "MINIRESNET_HAL_TRACE_TOP: phase=invoke rank=1 seq=1 allocation_size=315448 requested_type=49 requested_usage=4278194175 requested_access=3 compat_type=54 compat_usage=4278194175 compat_access=3 alignment=64 buffer=1234",
                    "MINIRESNET_RESULT: sample=synthetic_pattern argmax=3 hash=123 top_score_q=42 latency_us=99 latency_cycles=9900 total=1",
                    "MINIRESNET_PASS count=1",
                ]
            )
        )

        self.assertIn("MINIRESNET_RESULT count 0 does not match pass count 1", errors)


if __name__ == "__main__":
    unittest.main()
