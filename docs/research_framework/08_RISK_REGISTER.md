# Risk Register (Phase 1)

| Risk ID | Risk | Impact | Likelihood | Early Signal | Mitigation | Gate |
|---|---|---|---|---|---|---|
| R1 | Schema churn blocks automation | High | Medium | frequent field changes | freeze core entities early | schema review gate |
| R2 | Poor reproducibility | High | Medium | high run variance | standardize seeds/warmups/env snapshots | reproducibility gate |
| R3 | Agent hallucinated claims | High | Medium | claims without artifacts | enforce evidence-grade rules | claim acceptance gate |
| R4 | Retrieval mismatch | Medium | Medium | low evidence precision | adopt hybrid retrieval with evidence-first ranking | retrieval eval gate |
| R5 | Toolchain drift | Medium | High | environment mismatch | mandatory toolchain snapshot + version pinning | run registration gate |
| R6 | Over-investing in UI too early | Medium | Medium | design work without stable schema | postpone UI-heavy work to phase 2 | product scope gate |
| R7 | Storage growth from artifacts | Medium | Medium | high storage burn | artifact lifecycle policy + dedup hashes | storage governance gate |

## Decision Gates
- GATE-1: methodology and evidence-grade sign-off
- GATE-2: schema v0 pilot validation on 3 cases
- GATE-3: workflow audit pass with human review checkpoints
- GATE-4: QA response contract pass (answer + evidence + replay)
