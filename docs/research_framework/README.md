# Research Framework Pack

This folder contains phase-1 implementation artifacts for a structured research system focused on graphics/rendering/frame-capture/performance analysis + knowledge system + agent automation.

## Files
- `01_PROJECT_SCOPE.md`: scope, boundaries, and acceptance criteria
- `02_METHODOLOGY_CHARTER.md`: evidence rules and reproducibility standards
- `03_TOOL_CAPABILITY_MATRIX.md`: concrete tool shortlist and tradeoffs
- `04_ANALYSIS_TEMPLATE.md`: per-study analysis template
- `05_CANONICAL_SCHEMA_V0.yaml`: standardized entities/relations/events schema (strict draft)
- `06_AGENT_WORKFLOW_DRAFT.md`: orchestration state machine and audit gates
- `07_QA_REQUIREMENTS.md`: wiki/QA requirement checklist and answer contract
- `08_RISK_REGISTER.md`: risk register and mitigation gates
- `09_CURRENT_ARCH_HEALTH_CHECK.md`: current architecture readiness assessment
- `10_BRANCH_PROTECTION_SETUP.md`: required check and branch protection setup
- `templates/research_case.template.yaml`: executable case template
- `examples/RC-0001.case.yaml`: concrete example case
- `examples/RC-0002.case.yaml`: second valid case for regression checks
- `examples_negative/RC-FAIL.invalid.yaml`: intentionally invalid case for negative testing
- `scripts/research_framework/validate_case.py`: schema-aware validator for case files
- `scripts/research_framework/validate_workflow_doc.py`: workflow gate validator
- `scripts/research_framework/scan_placeholders.py`: placeholder gate scanner
- `scripts/research_framework/run_ci_gates.py`: unified CI gate runner

## Suggested Execution Order
1. Lock methodology and scope (`01`, `02`)
2. Confirm shortlist and constraints (`03`)
3. Run 3 pilot cases with template + schema (`04`, `05`, `templates/*`)
4. Validate agent workflow events (`06`)
5. Validate QA contract on pilot queries (`07`)
6. Track and close top risks (`08`, `09`)

## Validator Usage

```bash
python scripts/research_framework/validate_case.py \
  --schema docs/research_framework/05_CANONICAL_SCHEMA_V0.yaml \
  --case docs/research_framework/examples/RC-0001.case.yaml \
  --strict
```

Expected output:
- `VALIDATION_OK`

## Full Gate Usage

```bash
python scripts/research_framework/run_ci_gates.py \
  --schema docs/research_framework/05_CANONICAL_SCHEMA_V0.yaml \
  --cases-dir docs/research_framework/examples \
  --workflow-doc docs/research_framework/06_AGENT_WORKFLOW_DRAFT.md \
  --docs-dir docs/research_framework
```

Or via Pixi tasks:

```bash
pixi run rf-validate-example
pixi run rf-validate-example-2
pixi run rf-validate-workflow
pixi run rf-scan-placeholders
pixi run rf-validate-negative
pixi run rf-gate
pixi run rf-regression
```

## Phase-1 Exit Criteria
- 3 pilot cases represented in schema and passing validation
- accepted claims include evidence chain and replay reference
- QA contract returns answer + evidence + replay linkage
