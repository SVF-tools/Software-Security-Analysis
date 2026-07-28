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
| `03-buffer-overflow` | Buffer-overflow detection |
| `04-nullptr-deref` | Null-pointer-dereference detection |

After configuring and building the repository, run both implementations with:

```bash
ctest --test-dir Release-build -L ass3-level-1 --output-on-failure
```

Run one case directly:

```bash
Release-build/bin/ass3 \
  Assignment-3/Tests/level-1/01-stmt-basic.ll

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-1/01-stmt-basic.ll
```

## Level 2

Level 2 is intentionally excluded from CTest because of its memory use. Run
both implementations directly from the repository root:

```bash
Release-build/bin/ass3 \
  Assignment-3/Tests/level-2/repro.ll

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-2/repro.ll
```

Level 3 assesses large-program execution and report precision. No Level-3
fixture is published in this repository.
