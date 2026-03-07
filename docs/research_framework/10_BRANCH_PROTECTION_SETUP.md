# Branch Protection Setup (Research Gates)

This guide configures branch protection so research evidence gates are mandatory before merge.

## Required Check
Use this workflow check as required:
- `research-framework-gates / gates`

If your repository UI shows a slightly different check label, use the exact label shown in PR checks.

## Recommended Branch Rules
For `main` (or your protected branch):
- Require pull request before merging
- Require approvals (>=1)
- Require conversation resolution
- Require status checks to pass
- Required checks include `research-framework-gates / gates`
- Dismiss stale approvals on new commits
- Restrict force pushes

## GitHub CLI Commands (example)

Set variables:

```bash
OWNER="<your-owner>"
REPO="<your-repo>"
BRANCH="main"
CHECK="research-framework-gates / gates"
```

Apply protection:

```bash
gh api \
  -X PUT \
  -H "Accept: application/vnd.github+json" \
  "/repos/${OWNER}/${REPO}/branches/${BRANCH}/protection" \
  -f required_linear_history=true \
  -f allow_force_pushes=false \
  -f allow_deletions=false \
  -f required_pull_request_reviews.dismiss_stale_reviews=true \
  -f required_pull_request_reviews.required_approving_review_count=1 \
  -f enforce_admins=true \
  -F required_status_checks.strict=true \
  -F required_status_checks.contexts[]="${CHECK}"
```

Verify:

```bash
gh api "/repos/${OWNER}/${REPO}/branches/${BRANCH}/protection"
```

## Practical Verification Flow
1. Open a PR that changes `docs/research_framework/*`.
2. Confirm workflow `research-framework-gates` runs.
3. Confirm PR merge is blocked until check passes.
4. Confirm failing gate (e.g., invalid case) blocks merge.
