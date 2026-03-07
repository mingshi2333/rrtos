# Agent Workflow Draft (Concrete v1)

This workflow is designed for evidence-first operation with mandatory auditability.

## 1) State Machine

States:
1. intake
2. plan
3. execute
4. collect
5. evaluate
6. propose_claim
7. human_review
8. accept_or_reject
9. publish

Allowed transitions:
- intake -> plan
- plan -> execute
- execute -> collect
- collect -> evaluate
- evaluate -> propose_claim
- propose_claim -> human_review
- human_review -> accept_or_reject
- accept_or_reject -> publish
- accept_or_reject -> plan (rework loop)

Rejected transitions are invalid and must emit a policy violation event.

## 2) State Entry/Exit Contracts

### intake
- Entry requirements:
  - case_id exists
  - objective is non-empty
- Exit requirements:
  - hypothesis draft present

### plan
- Entry requirements:
  - case scope and target metrics defined
- Exit requirements:
  - executable plan with command templates
  - expected artifacts list

### execute
- Entry requirements:
  - plan approved
  - environment snapshot captured
- Exit requirements:
  - run_id generated
  - run status is passed/failed/cancelled

### collect
- Entry requirements:
  - run completed
- Exit requirements:
  - artifacts hashed and registered
  - raw metrics present

### evaluate
- Entry requirements:
  - minimum metrics set present
- Exit requirements:
  - statistical summary generated
  - anomaly notes attached

### propose_claim
- Entry requirements:
  - evidence_grade assigned
  - supporting_run_ids attached
- Exit requirements:
  - claim record created

### human_review
- Entry requirements:
  - claim exists and links to evidence
- Exit requirements:
  - review_event with decision and note

### accept_or_reject
- Entry requirements:
  - review_event present
- Exit requirements:
  - claim status updated (accepted or rejected)

### publish
- Entry requirements:
  - accepted claim only
- Exit requirements:
  - QA-facing answer record generated
  - replay reference included

## 3) Mandatory Gates

- Gate G1 (Plan Validity):
  - objective, metrics, and replay command template must exist
- Gate G2 (Traceability):
  - every artifact has sha256 and immutable URI
- Gate G3 (Evidence Completeness):
  - non-G0 claim must include supporting runs
- Gate G4 (Human Approval):
  - accepted claim must have explicit reviewer identity and note
- Gate G5 (Replayability):
  - accepted claim must link replay_script artifact

## 4) Agent Action Contract

For every action, emit:
- action_id
- run_id
- state
- actor_type (agent/human/system)
- actor_id
- input_refs
- output_refs
- rationale
- confidence (0.0-1.0)
- emitted_at

## 5) Audit Event Schema (Event Log)

Required fields:
- event_id
- parent_event_id
- run_id
- state
- action
- decision
- references
- policy_checks_passed
- emitted_at

Policy checks must be explicit booleans, not inferred text.

## 6) Conflict and Rollback Rules

If claim conflict is detected:
1. create conflict incident
2. schedule baseline rerun
3. mark both claims as under_review
4. route to human_review with side-by-side evidence diff

Rollback rule:
- Any accepted claim can be downgraded to reviewed if replay check fails.

## 7) Automation Boundaries (Phase 1)

Automated:
- execution, collection, metric extraction, draft claim generation

Human-required:
- claim acceptance
- contradiction arbitration
- policy override

## 8) Phase-1 Readiness Checklist

- [ ] end-to-end one-case workflow executed
- [ ] all states emitted at least one event
- [ ] one accepted claim has replay artifact
- [ ] one rejection path exercised and logged
