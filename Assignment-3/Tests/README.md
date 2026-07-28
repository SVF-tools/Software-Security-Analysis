# Assignment 3 Public Cases

The repository includes public examples for two marking levels:

- `level-1/`: small, self-validating feature examples;
- `level-2/`: an active real-program example.

These examples are not the complete marking suite. Additional hidden cases are
used for marking. Level 1 provides each public case as a flat `.c`/`.ll` pair.
Level 2 keeps the source, LLVM IR, bitcode, and notes for its single fixture
directly in the level directory. `GRADING.md` defines the score allocation.
Marking runs are subject to a timeout.

## Level 1

The published examples are:

| Case | Primary feature |
| --- | --- |
| `01-stmt-basic` | SVF statement transfer |
| `02-branch-feasible` | Branch feasibility |
| `03-buffer-overflow` | Real out-of-bounds access detection |
| `04-nullptr-deref` | Real null-pointer-dereference detection |
| `05-loop-fixpoint` | Loop fixpoint and widening |
| `06-recursion-fixpoint` | Recursive-SCC fixpoint |
| `07-interprocedural-call-return` | Actual/formal and return propagation |
| `08-memory-summary` | Memory external-call summaries |
| `09-string-summary` | String external-call summaries |
| `10-safe-memory` | Safe-access report precision |

From the repository root, set `BUILD_DIR` to your configured CMake build tree:

```bash
# Normal out-of-source build:
BUILD_DIR=build

# Supplied Docker checkout:
# BUILD_DIR=.
```

Run all registered public Level-1 cases:

```bash
ctest --test-dir "$BUILD_DIR" \
  -R '^ass3-level-1-cpp/' --output-on-failure

ctest --test-dir "$BUILD_DIR" \
  -R '^ass3-level-1-py/' --output-on-failure

# Both implementations:
ctest --test-dir "$BUILD_DIR" \
  -L ass3-level-1 --output-on-failure
```

Run one case directly:

```bash
"$BUILD_DIR/bin/ass3" \
  Assignment-3/Tests/level-1/01-stmt-basic.ll

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-1/01-stmt-basic.ll
```

CMake enforces a 120-second timeout per public C++ case and a 180-second
timeout per public Python case.

## Level 2

Level 2 is intentionally excluded from CTest because of its memory use. Run
both implementations directly from the repository root:

```bash
"$BUILD_DIR/bin/ass3" \
  Assignment-3/Tests/level-2/repro.ll

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-2/repro.ll
```

Level 3 assesses large-program execution and report precision. No Level-3
fixture is published in this repository.

See the authoritative [Assignment 3 contract](../README.md) for required
semantics, checkpoint behaviour, assessment definitions, and submission
instructions.
