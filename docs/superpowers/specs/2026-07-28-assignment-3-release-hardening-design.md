# Assignment 3 Release Hardening Design

## Objective

Make Assignment 3 self-consistent and verifiable for students by versioning
the authoritative specification with the template, publishing commands that
work in both supported build layouts, separating checkpoint expectations from
student-generated bug reports, adding focused public cases, and rejecting
future documentation drift in CI.

## Source of truth

`Assignment-3/README.md` becomes the authoritative student specification.
It owns:

- the five driver entry points and six assessed tasks;
- the exact abstract-domain and checker policy;
- external-call summary contracts;
- public build and run commands;
- assessment and submission rules.

`Assignment-3/Tests/README.md` remains the public-case operator guide and
`Assignment-3/Tests/GRADING.md` remains the marking overview. The GitHub Wiki
keeps course navigation, setup guidance, and short language-specific pages,
but links to the versioned specification for assignment behavior.

## Build and test commands

All commands are written in terms of `BUILD_DIR`:

```bash
BUILD_DIR=build  # from-source guide
# BUILD_DIR=.    # provided Docker checkout
```

Language-specific public tests use anchored CTest names:

```bash
ctest --test-dir "$BUILD_DIR" -R '^ass3-level-1-cpp/' --output-on-failure
ctest --test-dir "$BUILD_DIR" -R '^ass3-level-1-py/' --output-on-failure
```

The documentation calls these “registered public Level-1 tests”; it does not
imply that Level 2 or Level 3 is registered with CTest.

## Checkpoint and reporting contract

`SAFE_*` and `UNSAFE_*` calls are ground-truth markers, not detectors. Reaching
a checkpoint records:

- the call site;
- the expected bug kind (`buffer-overflow` or `nullptr-deref`);
- whether the case expects an unsafe report.

Checkpoint dispatch does not call `reportBufOverflow` or `reportNullDeref`.
After analysis, validation checks:

1. every assertion/checkpoint call site was reached;
2. actual report counts match unsafe checkpoint counts separately by kind.

Public checker cases isolate one property per input, so exact per-kind counts
also reject false positives. Both language harnesses expose typed report
counts. Python retains its existing `reportBufOverflow` API while adding
`reportNullDeref` and a common typed reporting implementation.

## Public case coverage

The focused Level-1 corpus covers:

| Case | Primary contract |
|---|---|
| `01-stmt-basic` | scalar statement transfer |
| `02-branch-feasible` | conditional feasibility |
| `03-buffer-overflow` | direct out-of-bounds memory access |
| `04-nullptr-deref` | direct null dereference |
| `05-loop-fixpoint` | loop convergence and narrowing precision |
| `06-recursion-fixpoint` | recursive SCC convergence |
| `07-interprocedural-call-return` | actual/formal and return propagation |
| `08-memory-summary` | `memcpy`/`memset` value effects |
| `09-string-summary` | `strcpy`/`strlen` value effects |
| `10-safe-memory` | safe buffer and pointer accesses produce no report |

Each `.c` file is compiled to matching LLVM 21 textual IR with debug
locations, `-O0`, and `-fno-discard-value-names`. The committed `.ll` file is
the CTest input; the `.c` file documents intent.

## Required semantic policy

The specification publishes the behavior expected by the supplied harness and
reference grader:

- address sets use may semantics: one possible null, freed, or out-of-bounds
  address is reportable;
- unknown/black-hole objects do not manufacture a precise bug report;
- object and access sizes are bytes;
- a non-zero access beginning one-past the object is out of bounds;
- merely constructing a one-past pointer is not a dereference;
- reports are deduplicated by bug kind and ICFG node;
- external summaries preserve C return values and model the documented
  read/write range;
- WTO iteration uses precise iterations through the configured widening delay,
  widening to stability, then narrowing to recover precision.

## Assessment language

All student-facing pages use the 30/40/30 level split. Public examples are
development aids and do not map one-to-one to marks. Public CTest timeouts are
120 seconds for C++ and 180 seconds for Python. Hidden-case counts, weights,
and environment-specific timeouts remain unpublished, but the meaning of
target detection, false positives, completion, and coverage is stated.

## Drift prevention

`scripts/check-assignment-3-release.sh` validates the release from a configured
checkout:

- both documented language-specific CTest regexes select tests;
- every documented public `.ll` input exists;
- obsolete flat Assignment-3 test paths are absent;
- Assignment-3 repository documentation does not hard-code
  `Release-build`.

The existing build workflow runs this checker after configuration. This
validates commands and filesystem behavior without executing the intentionally
incomplete student analyzer.

## Wiki delivery

The wiki is a separate Git repository and GitHub does not support wiki pull
requests. Wiki changes are prepared as a local commit that:

- points Assignment 3 to the repository-owned specification;
- corrects test commands, anchors, snippets, wrapper terminology, and paths;
- corrects Assignment 3 paths in Docker, from-source, and Python IDE pages.

Publishing that commit requires an explicit direct push to the wiki’s
`master` branch.
