# Current Architecture Health Check

Assessment target: readiness for a graphics/rendering/frame-capture/performance-analysis knowledge system with agent automation.

Scoring:
- 5 = strong and production-oriented
- 3 = workable but incomplete
- 1 = missing or fragile

## Scorecard by Layer

| Layer | Score | Current Strengths | Current Gaps | Evidence |
|---|---:|---|---|---|
| Methodology | 3/5 | Existing process docs and report structure | evidence grades/provenance policy not enforced in code | `docs/reports/SIMULATION_TEST_REPORT.md`, `docs/reports/PIXI_PLAN.md` |
| Toolchain | 4/5 | Pixi + CMake + scripts + simulation stack already operational | capture/profiler integration not yet formalized | `pixi.toml`, `scripts/test_ai_registry.sh`, `scripts/simulation/*` |
| Standardized Data Model | 2/5 (before this pack), 3/5 (after this pack draft) | typed configs and C structs exist | no unified cross-layer schema enforcement yet | `ai/include/ai_model_registry.h`, `config/os_config.h` |
| Agent Workflow | 2/5 (before), 3/5 (after this pack draft) | task automation patterns exist | no end-to-end auditable state machine in runtime | `pixi.toml`, `scripts/*.py`, `scripts/test_ai_registry.sh` |
| Database / Retrieval | 1/5 | file-level metadata exists in docs/scripts | no evidence database, no retrieval layer, no query semantics | repository-wide observation |
| Wiki / QA Product | 1/5 | markdown reports exist | no QA contract, no answer-evidence-replay path | `docs/reports/*` |

## What Is Already Good

1. Automation baseline is real, not theoretical:
   - deterministic command paths via Pixi tasks
   - simulation scripts for repeatable runs
2. Benchmark culture already exists:
   - model benchmark docs with environment/metrics summary
3. Engineering discipline is compatible with structured rollout:
   - explicit build/sim/test entrypoints

## Critical Gaps to Close Next

1. Provenance hardening:
   - enforce run_id, artifact hash, source commit linkage in every reportable case
2. Unified schema enforcement:
   - convert schema draft into validator-backed checks in CI
3. QA contract enforcement:
   - block accepted answers without evidence/replay references
4. Retrieval substrate bootstrap:
   - start with relational evidence store and deterministic ID lookups

## Risk Hotspots

- R-H1: Fast iteration produces reports without replayability
- R-H2: Agent-generated conclusions outpace human review policy
- R-H3: Multi-tool output semantics drift without canonical mapping

## 2-Week Action Plan (Practical)

Week 1:
- implement schema validator (YAML/JSON checker)
- enforce artifact hash + run metadata template
- create 3 pilot cases using `templates/research_case.template.yaml`

Week 2:
- wire claim review workflow events
- implement minimal evidence index (case/run/claim/artifact)
- run QA contract tests (20 questions over 6 classes)

## Exit Criteria for Next Stage

- 3 pilot cases pass schema + provenance gates
- at least 1 accepted claim is replayable end-to-end
- QA answers return claim + run + artifact refs consistently
