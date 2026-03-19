# Kernel Health Policy Report

This report proves the assessment policy remains correctness-first.
It exercises positive and negative readiness cases so blockers, semantic gaps, simulator-only evidence, and performance-only evidence cannot be misreported as kernel health.

## Artifact Contract Checks

| Check | Artifact | Result | Detail |
| --- | --- | --- | --- |
| baseline-report-keeps-smp-experimental | `logs/kernel_baseline_report.md` | pass | Kernel baseline report must fence SMP out of the supported health conclusion. |
| smp-runtime-report-disclaims-supported-status | `logs/be_u1000_selftest_runtime.md` | pass | SMP runtime evidence must stay observational and non-promotional. |
| supported-rv32-lane-forces-single-core | `pixi.toml` | pass | Supported RV32 gate must continue building with SMP disabled. |
| supported-be-u1000-lane-forces-single-core | `pixi.toml` | pass | Supported BE-U1000 gate must continue building with SMP disabled. |

## Policy Cases

| Case | Expected Ready | Actual Ready | Result | Notes |
| --- | --- | --- | --- | --- |
| explicit-proof-acceptance | true | true | pass | explicit proof accepted |
| blocker-rejection | false | false | pass | known blocker remains: ipc wakeup reinsertion blocker; performance evidence cannot waive correctness gaps |
| missing-semantic-gap-rejection | false | false | pass | missing semantic coverage: event wait deterministic semantic coverage |
| simulator-only-rejection | false | false | pass | missing pure-kernel semantic proof; simulator-only evidence is insufficient |
| performance-does-not-waive-correctness | false | false | pass | missing semantic coverage: queue timeout semantic coverage; performance evidence cannot waive correctness gaps |
| smp-scope-expansion-rejection | false | false | pass | SMP evidence is not labeled experimental; SMP scope expanded beyond the supported single-core target |

## Notes

- The positive case requires semantic proof, supported integration proof, explicit acceptance evaluation, and the existing single-core SMP fence.
- The negative cases intentionally prove that remaining blockers, missing semantic coverage, simulator-only evidence, and baseline reports without correctness cannot pass readiness.
- SMP scope remains fenced by supported build flags and report wording rather than widened by observational runtime probes.
