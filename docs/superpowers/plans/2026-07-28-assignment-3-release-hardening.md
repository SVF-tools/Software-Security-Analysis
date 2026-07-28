# Assignment 3 Release Hardening Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Publish a consistent, versioned, and testable Assignment 3 specification with meaningful public cases and automated drift checks.

**Architecture:** Keep the detailed student contract in `Assignment-3/README.md`, treat checkpoint stubs as expectation markers rather than detectors, and expose typed report counts in both harnesses. Register focused LLVM IR cases through the existing CMake structure and validate the documented commands in the existing build workflow.

**Tech Stack:** Markdown, Bash, CMake/CTest, C++17, Python 3/PySVF, LLVM 21 IR, GitHub Actions.

## Global Constraints

- Do not add solution implementations to `Assignment_3.cpp`, `Assignment_3.h`, or `Assignment_3.py` beyond pre-implemented reporter forwarding.
- C++ students continue to submit only `Assignment_3.cpp` and `Assignment_3.h`.
- Python students continue to submit only `Assignment_3.py`.
- Public checker expectations are compared separately for `buffer-overflow` and `nullptr-deref`.
- The provided Docker checkout and the documented `build/` checkout must both have working commands.
- The GitHub Wiki is committed separately and is not pushed without explicit approval.

---

### Task 1: Add the release contract checker

**Files:**
- Create: `scripts/check-assignment-3-release.sh`
- Modify: `.github/workflows/build.yml`

**Interfaces:**
- Consumes: a configured repository path as argument 1, defaulting to the repository root.
- Produces: exit status 0 only when documented Assignment 3 test selectors and paths are valid.

- [ ] **Step 1: Write the failing release checker**

Create a shell checker that runs `ctest -N` with
`^ass3-level-1-cpp/` and `^ass3-level-1-py/`, verifies a non-zero test count,
checks all public IR paths listed in the release manifest, and rejects
`Release-build` plus deleted flat test paths in Assignment 3 repository
documentation.

- [ ] **Step 2: Verify the checker fails on the current documentation**

Run:

```bash
bash scripts/check-assignment-3-release.sh .
```

Expected: non-zero exit because the current public-case README hard-codes
`Release-build`.

- [ ] **Step 3: Wire the checker into CI**

Add a build-workflow step after `make`:

```yaml
- name: validate-assignment-3-release
  run: |
    bash scripts/check-assignment-3-release.sh .
```

- [ ] **Step 4: Keep the checker red until Task 2 corrects the docs**

Do not weaken the validation to accept the current broken command.

### Task 2: Create the authoritative specification and portable commands

**Files:**
- Create: `Assignment-3/README.md`
- Modify: `Assignment-3/Tests/README.md`
- Modify: `Assignment-3/Tests/GRADING.md`
- Modify: `README.md`

**Interfaces:**
- Consumes: current CMake target names, public-case locations, harness APIs, and grading structure.
- Produces: one authoritative Assignment 3 contract linked from the repository root.

- [ ] **Step 1: Write `Assignment-3/README.md`**

Include deliverables, the five entry points, all six tasks, the checker and
external-call semantic tables, fixpoint pseudocode, portable `BUILD_DIR`
commands, public timeouts, marking language, submission commands, and a
student-authored test recipe using LLVM 21.

- [ ] **Step 2: Correct the public-case operator guide**

Replace `Release-build` with `BUILD_DIR`, add exact C++ and Python anchored
CTest selectors, and document direct Level-1 and Level-2 commands.

- [ ] **Step 3: Reconcile grading language**

State the 30/40/30 split, define target detection and report precision, state
the public CTest timeouts, and explicitly distinguish public development cases
from hidden marking cases.

- [ ] **Step 4: Link the authoritative specification**

Add an Assignment 3 link to the repository root README.

- [ ] **Step 5: Run the release checker**

Run:

```bash
bash scripts/check-assignment-3-release.sh .
```

Expected: CTest selector checks may remain unavailable until a configured
build is supplied, but documentation/path checks pass.

### Task 3: Separate checkpoint expectations from C++ reports

**Files:**
- Modify: `Assignment-3/CPP/AEReporter.h`
- Modify: `Assignment-3/CPP/AEHelper.cpp`

**Interfaces:**
- Produces: `noteExpectedReport(kind)`, `getExpectedReportCount(kind)`, and `getReportCount(kind)` on `AEReporter`.
- Consumes: typed reports already stored in `AssignmentBugReport`.

- [ ] **Step 1: Add an expectation-count regression test to the validation path**

Use the Level-1 buffer/null cases as the integration regression: with
checkpoint-side reporting removed, a no-op checker must no longer satisfy the
unsafe expected count.

- [ ] **Step 2: Record expected kinds at checkpoints**

For `UNSAFE_BUFACCESS`, record one expected `buffer-overflow`; for
`UNSAFE_PTRDEREF`, record one expected `nullptr-deref`. SAFE checkpoints record
no expected report. All checkpoints still record reachability.

- [ ] **Step 3: Remove checkpoint-side detection and reporting**

Delete the harness-only safety predicates and remove calls to
`reportBufOverflow` and `reportNullDeref` from `handleCheckpointStubs`.

- [ ] **Step 4: Validate exact per-kind counts**

In `ensureAllAssertsValidated`, compare expected and actual counts separately
and emit a diagnostic naming the mismatched kind before asserting.

- [ ] **Step 5: Build the C++ targets**

Run:

```bash
cmake --build "$BUILD_DIR" --target ass3 -j8
```

Expected: target builds without warnings or errors.

### Task 4: Add typed Python reporting and checkpoint expectations

**Files:**
- Create: `Assignment-3/Tests/test-reporter-contract.py`
- Modify: `Assignment-3/Python/AEReporter.py`
- Modify: `Assignment-3/Python/AEHelper.py`
- Modify: `Assignment-3/Python/Assignment_3.py`

**Interfaces:**
- Produces: `reportBug(kind, node, message)`, `reportNullDeref(node, message)`, `getReportCount(kind)`, `noteExpectedReport(kind)`, and `getExpectedReportCount(kind)`.
- Retains: `reportBufOverflow(node, message)` compatibility.

- [ ] **Step 1: Write the failing reporter contract test**

The test creates an `AEReporter`, records one report of each kind plus one
duplicate, and asserts independent counts and expected-count bookkeeping.

- [ ] **Step 2: Verify the reporter test fails**

Run in the course Python environment:

```bash
python3 Assignment-3/Tests/test-reporter-contract.py
```

Expected: failure because null-specific and count APIs do not exist.

- [ ] **Step 3: Implement typed Python reports**

Store reports by `(kind, node)`, retain human-readable printing, and implement
the count and expectation methods.

- [ ] **Step 4: Change Python checkpoints to expectation markers**

Remove harness-side safety decisions and compare exact per-kind counts in
`ensureAllAssertsValidated`.

- [ ] **Step 5: Correct the null-report forwarder**

Route `Assignment3.reportNullDeref` through
`AEReporter.reportNullDeref`.

- [ ] **Step 6: Verify the reporter test passes**

Run:

```bash
python3 Assignment-3/Tests/test-reporter-contract.py
```

Expected: one test process exits 0.

### Task 5: Expand focused public cases

**Files:**
- Modify: `Assignment-3/Tests/level-1/03-buffer-overflow.c`
- Modify: `Assignment-3/Tests/level-1/03-buffer-overflow.ll`
- Modify: `Assignment-3/Tests/level-1/04-nullptr-deref.c`
- Modify: `Assignment-3/Tests/level-1/04-nullptr-deref.ll`
- Create: `Assignment-3/Tests/level-1/05-loop-fixpoint.c`
- Create: `Assignment-3/Tests/level-1/05-loop-fixpoint.ll`
- Create: `Assignment-3/Tests/level-1/06-recursion-fixpoint.c`
- Create: `Assignment-3/Tests/level-1/06-recursion-fixpoint.ll`
- Create: `Assignment-3/Tests/level-1/07-interprocedural-call-return.c`
- Create: `Assignment-3/Tests/level-1/07-interprocedural-call-return.ll`
- Create: `Assignment-3/Tests/level-1/08-memory-summary.c`
- Create: `Assignment-3/Tests/level-1/08-memory-summary.ll`
- Create: `Assignment-3/Tests/level-1/09-string-summary.c`
- Create: `Assignment-3/Tests/level-1/09-string-summary.ll`
- Create: `Assignment-3/Tests/level-1/10-safe-memory.c`
- Create: `Assignment-3/Tests/level-1/10-safe-memory.ll`

**Interfaces:**
- Consumes: `svf_assert`, `SAFE_*`, and `UNSAFE_*` harness stubs.
- Produces: one self-validating IR case for each published primary contract.

- [ ] **Step 1: Make unsafe checker sources contain real accesses**

Add a real out-of-bounds volatile load to case 03 and a real volatile null
load to case 04 before their expectation markers.

- [ ] **Step 2: Add loop, recursion, and call/return sources**

Use exact small integer bounds so assertions are decidable after a correct
fixpoint.

- [ ] **Step 3: Add memory and string summary sources**

Use non-builtin calls and `svf_assert` checks on copied values and returned
lengths.

- [ ] **Step 4: Add a safe checker source**

Perform one in-bounds buffer read and one non-null dereference, followed by
their SAFE markers. Exact report-count validation must remain zero.

- [ ] **Step 5: Compile LLVM 21 IR**

For every source:

```bash
clang -g -O0 -fno-discard-value-names -fno-builtin \
  -S -emit-llvm case.c -o case.ll
```

- [ ] **Step 6: Run the reference implementation**

Run C++ and Python against each Level-1 IR case. Expected: every case exits 0;
unsafe cases produce exactly one report of the expected kind and safe cases
produce none.

### Task 6: Validate the complete repository change

**Files:**
- Review all modified repository files.

**Interfaces:**
- Consumes: all preceding tasks.
- Produces: build, reporter, release-contract, formatting, and diff evidence.

- [ ] **Step 1: Run the reporter contract test**

```bash
python3 Assignment-3/Tests/test-reporter-contract.py
```

- [ ] **Step 2: Build the repository**

```bash
cmake --build "$BUILD_DIR" -j8
```

- [ ] **Step 3: Validate documented selectors**

```bash
bash scripts/check-assignment-3-release.sh "$BUILD_DIR"
```

- [ ] **Step 4: Check patch quality**

```bash
git diff --check
git status --short
```

- [ ] **Step 5: Inspect the final diff**

Confirm no student solution implementation appears and every generated IR
file corresponds to its source case.

### Task 7: Prepare and validate the wiki commit

**Files:**
- Modify in the wiki repository: `Assignment-3.md`
- Modify in the wiki repository: `Assignment-3-CPP.md`
- Modify in the wiki repository: `Assignment-3-Python.md`
- Modify in the wiki repository: `Installation-of-Docker-and-Coding-Inside-Docker-Image.md`
- Modify in the wiki repository: `Building-Software-Security-Analysis-Repo-from-scratch.md`
- Modify in the wiki repository: `Building-Software-Security-Analysis-Repo-from-scratch-Python.md`
- Modify in the wiki repository: `Configure-Python-IDE-Environment.md`

**Interfaces:**
- Consumes: the authoritative repository specification and current public case paths.
- Produces: a local wiki commit ready for an explicitly approved direct push.

- [ ] **Step 1: Reduce duplicated assignment behavior**

Make `Assignment-3.md` link to `Assignment-3/README.md` as authoritative while
retaining the course summary and submission links.

- [ ] **Step 2: Correct language pages**

Use anchored CTest selectors, portable build-directory guidance, correct
anchors, `std::endl`, `toString()`, and accurate Python `AEState` wrapper
terminology.

- [ ] **Step 3: Correct stale setup paths**

Replace the three deleted flat test paths in every setup page with current
Level-1 paths.

- [ ] **Step 4: Validate wiki references**

Run:

```bash
rg 'Tests/(stmt|buf_overflow|null_deref)\.ll|Release-build|ass3-(cpp|py)' .
```

Expected: no obsolete Assignment 3 instructions.

- [ ] **Step 5: Commit without pushing**

Create a local wiki commit and report its hash. Do not push to `master`
without explicit user approval.

### Task 8: Create the pull request and change table

**Files:**
- No additional repository files.

**Interfaces:**
- Consumes: verified repository branch and committed wiki branch.
- Produces: fork branch, upstream pull request, and one-to-one change table.

- [ ] **Step 1: Commit repository changes**

Use focused commits for release checking, harness behavior, public cases, and
documentation.

- [ ] **Step 2: Push the feature branch to the authenticated fork**

```bash
git push -u fork docs/ass3-release-hardening
```

- [ ] **Step 3: Open the upstream pull request**

Target `SVF-tools/Software-Security-Analysis:main`. Include verification
evidence, the wiki limitation, and the change table.

- [ ] **Step 4: Produce the one-to-one table**

For every changed path, list:

| Changed file location | Before change | After change |
|---|---|---|

Include the table in both the PR description and the user handoff.
