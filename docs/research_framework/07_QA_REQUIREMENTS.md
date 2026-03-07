# Wiki / QA Requirements (Concrete v1)

## 1) QA Product Goal
Provide answers that are not only useful but replayable and auditable.

Every answer must be evidence-first:
- answer
- supporting claim IDs
- supporting run IDs
- replay reference
- confidence and scope boundary

## 2) Query Catalog (Phase 1)

### Q1 Diagnostic
- User query: "Why did frame time increase?"
- Required resolution path:
  - find claim by metric trend
  - return p95 delta and supporting runs

### Q2 Comparative
- User query: "Compare renderer A vs B under the same workload"
- Required resolution path:
  - normalize environment
  - compare same metric definitions

### Q3 Regression timeline
- User query: "When did this regression start?"
- Required resolution path:
  - map run -> source_commit
  - detect first statistically significant shift

### Q4 Reproducibility
- User query: "How can I replay this conclusion?"
- Required resolution path:
  - return replay artifact URI
  - return required environment snapshot

### Q5 Confidence
- User query: "How reliable is this claim?"
- Required resolution path:
  - return evidence grade
  - return run count and variance summary

### Q6 Conflict
- User query: "Why do two reports disagree?"
- Required resolution path:
  - return conflicting claims
  - show environment deltas
  - show arbitration status

## 3) Answer Contract (Machine-Readable)

```json
{
  "answer_id": "ANS-...",
  "question_type": "diagnostic|comparative|regression|reproducibility|confidence|conflict",
  "answer_text": "...",
  "claim_refs": ["CL-..."],
  "supporting_run_refs": ["RUN-..."],
  "evidence_grade": "G0|G1|G2|G3|G4",
  "confidence": 0.0,
  "applicability_scope": "...",
  "replay": {
    "artifact_ref": "AR-...",
    "command_ref": "..."
  },
  "provenance": {
    "source_commit": "...",
    "generated_at": "...",
    "generated_by": "..."
  }
}
```

## 4) Retrieval Requirements

Mandatory:
- entity lookup by case_id/run_id/claim_id
- multi-hop retrieval from claim -> runs -> artifacts
- environment-aware filtering (hardware/os/driver/tool versions)
- contradiction-aware retrieval (return competing claims)

Preferred:
- hybrid retrieval (structured constraints + semantic retrieval)
- answer generation blocked when evidence links are missing

## 5) QA Validation Metrics

- evidence_precision_at_1: top answer links correct claim
- evidence_coverage_rate: answers with >=1 valid claim + run + artifact
- replay_success_rate: returned replay command works in declared environment
- contradiction_recall: known conflicting claims surfaced in conflict queries
- stale_answer_rate: answers invalidated by source updates

## 6) Policy Rules

- No accepted recommendation without non-G0 evidence
- Draft claims can be shown only with explicit warning
- If replay reference missing, answer must be labeled non-reproducible
- If environment mismatch is detected, answer must include scope warning

## 7) Phase-1 Acceptance

- At least 20 QA test queries across 6 query classes
- >= 90% evidence_coverage_rate
- >= 80% replay_success_rate for reproducibility queries
- 100% accepted answers include claim + run + artifact links
