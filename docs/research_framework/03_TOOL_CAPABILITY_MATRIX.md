# Tool Capability Matrix (Concrete Shortlist v1)

This matrix is filled for phase-1 decision making, with emphasis on scriptability, standardization, verifiability, and traceability.

## Scoring Model
- 5 = strong
- 3 = acceptable
- 1 = weak

Weighted score used for phase-1:
- Scriptability: 0.30
- Standardized output: 0.20
- Verifiability: 0.20
- Traceability: 0.20
- Operability cost (inverse): 0.10

## A. Capture and Profiling Layer

| Domain | Candidate | Scriptability | Standardized Output | Verifiability | Traceability | Operability Cost | Notes |
|---|---|---:|---:|---:|---:|---:|---|
| Frame capture | RenderDoc + Python API | 5 | 4 | 4 | 4 | 3 | Best cross-platform script-first baseline |
| Frame capture | PIX (Windows) | 4 | 3 | 4 | 3 | 3 | Strong on D3D workloads, platform constrained |
| GPU timeline/counters | Nsight Systems | 4 | 4 | 4 | 4 | 3 | Good timeline correlation and automation |
| GPU pipeline debug | Nsight Graphics | 3 | 3 | 4 | 3 | 2 | Strong manual depth, scriptability weaker |
| CPU profiling | Linux perf + FlameGraph | 5 | 4 | 4 | 4 | 4 | Strong CLI pipeline and reproducibility |
| System observability | OpenTelemetry collector | 4 | 5 | 4 | 5 | 3 | Best for unified event/metric traces |

## B. Benchmark and Experiment Runner Layer

| Domain | Candidate | Scriptability | Standardized Output | Verifiability | Traceability | Operability Cost | Notes |
|---|---|---:|---:|---:|---:|---:|---|
| Build/run orchestration | Pixi tasks + CMake (existing in repo) | 5 | 4 | 4 | 3 | 4 | Already present and easy to version-control |
| Benchmark harness | Custom Python harness + YAML case files | 5 | 5 | 4 | 5 | 3 | Recommended for schema-first control |
| Benchmark harness | Phoronix Test Suite profiles | 4 | 4 | 4 | 3 | 3 | Good reproducibility, less domain-specific semantics |
| Simulation runner | Renode scripts (.resc/.repl) | 4 | 3 | 4 | 3 | 3 | Useful for deterministic replay in embedded flows |

## C. Storage and Retrieval Layer

| Domain | Candidate | Scriptability | Standardized Output | Verifiability | Traceability | Operability Cost | Notes |
|---|---|---:|---:|---:|---:|---:|---|
| Relational evidence store | PostgreSQL | 5 | 5 | 5 | 5 | 4 | Best base for strict IDs and constraints |
| Vector retrieval | PostgreSQL + pgvector | 5 | 4 | 4 | 4 | 4 | Good single-stack option for phase-1/2 |
| Graph retrieval | Neo4j / Memgraph | 4 | 4 | 4 | 5 | 2 | Strong multi-hop, higher governance cost |
| Object artifacts | S3/MinIO-style immutable bucket | 4 | 4 | 5 | 5 | 3 | Required for hash-linked captures/traces |

## D. Agent Orchestration and Audit Layer

| Domain | Candidate | Scriptability | Standardized Output | Verifiability | Traceability | Operability Cost | Notes |
|---|---|---:|---:|---:|---:|---:|---|
| Workflow orchestrator | LangGraph state machine | 4 | 4 | 4 | 4 | 3 | Good for explicit state and branch control |
| Durable orchestrator | Temporal | 4 | 4 | 5 | 5 | 2 | Strong reliability and replay guarantees |
| Audit log backbone | Append-only event log + hash chain | 5 | 5 | 5 | 5 | 3 | Recommended regardless of orchestrator |

## Recommended Phase-1 Baseline (Practical)
1. Capture/profiling baseline:
   - RenderDoc (scripted) + perf + optional Nsight Systems
2. Execution baseline:
   - Pixi + CMake + Python harness + YAML case files
3. Data baseline:
   - PostgreSQL (+pgvector) + immutable artifact storage
4. Agent baseline:
   - LangGraph-like explicit state machine + append-only audit events

## Alternatives and Tradeoffs
- Graph-first retrieval:
  - Pros: better cross-entity reasoning and contradiction surfacing
  - Cons: steeper modeling and ops cost early
- Doc/vector-first retrieval:
  - Pros: faster bootstrap, lower complexity
  - Cons: weaker provenance-constrained multi-hop precision
- Hybrid (recommended):
  - Pros: balanced speed and rigor
  - Cons: requires schema discipline to avoid split-brain semantics

## Entry/Exit Criteria for Tool Selection
- Entry:
  - Tool has non-interactive script entrypoint
  - Output can be exported in machine-readable format
- Exit (must pass all):
  - Same case rerun variation within defined threshold
  - Artifact hash + environment snapshot recorded
  - At least one replay command succeeds end-to-end
