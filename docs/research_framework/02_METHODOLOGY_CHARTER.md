# Methodology Charter

## Principles
- Reproducibility over narrative convenience
- Evidence-linked conclusions only
- Structured records over free-form notes
- Explicit uncertainty and applicability boundaries

## Evidence Grades
- G0: claim only (no evidence)
- G1: single run evidence (non-repeated)
- G2: repeated run evidence (same environment)
- G3: cross-environment repeated evidence
- G4: independently reproduced evidence (different operator/toolchain)

## Minimum Evidence Bundle
Every accepted conclusion must include:
- command/script entrypoint
- environment snapshot (hardware/driver/tool versions)
- input assets/checksums
- produced artifacts (capture/trace/log/metrics)
- metric extraction method
- uncertainty note and scope boundary

## Reproducibility Rules
- fixed seed when possible
- explicit warmup and run count
- timestamped run_id
- immutable artifact hash

## Contradiction Handling
If two conclusions conflict:
1. Compare environment deltas
2. Re-run shared baseline case
3. Record conflict as separate hypothesis, not silent overwrite

## Decision Rules
- High-impact architecture decisions require at least G2 evidence
- Performance optimization recommendations require explicit tradeoff notes
