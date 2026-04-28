import unittest

from scripts.run_mnist_validation import validate_ai_validation_output


VALID_LOG = "\n".join(
    [
        "Booting AI validation: mnist_validation...",
        "AI_VALIDATION_SAMPLESET: name=mnist_t10k_official count=2",
        "AI_VALIDATION_METRICS: sample=a idx=0 label=7 argmax=7 hash=1 top_score_q=10 latency_us=2 total=1 arena_peak=0",
        "AI_VALIDATION_METRICS: sample=b idx=1 label=2 argmax=2 hash=2 top_score_q=11 latency_us=3 total=1 arena_peak=0",
        "AI_VALIDATION_PASS count=2",
        "OS_TIMER_CALLBACK_PASS count=1 tick=26",
    ]
)


class MnistValidationScriptTest(unittest.TestCase):
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
                    "AI_VALIDATION_PASS count=1",
                    "OS_TIMER_CALLBACK_PASS count=1 tick=26",
                ]
            )
        )
        self.assertIn("AI_VALIDATION_METRICS count 0 does not match pass count 1", errors)

    def test_rejects_multiple_pass_lines(self):
        errors = validate_ai_validation_output(VALID_LOG + "\nAI_VALIDATION_PASS count=2\n")
        self.assertIn("multiple AI_VALIDATION_PASS count lines", errors)


if __name__ == "__main__":
    unittest.main()
