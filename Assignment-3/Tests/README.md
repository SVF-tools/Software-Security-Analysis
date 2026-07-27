# Assignment 3 Public Cases

The repository includes public examples for Level 1 and Level 2:

- `level-1`: small, self-validating feature examples;
- `level-2`: an active real-program example.

These examples are not the complete marking suite. Additional hidden cases are
used for marking, and their number and allocation are not published.

The C and LLVM bitcode/IR inputs are both included so the tested behavior can
be inspected. `GRADING.md` defines the score allocation, and
`level-2/manifest.csv` defines the expected public large-program result.

The manifest is authoritative. Per-case `notes.md` files record
fixture provenance, the current commands, and validation results.
Marking runs are subject to a timeout.

## Level 1

After configuring and building the repository, run both implementations with:

```bash
ctest --test-dir Release-build -L ass3-level-1 --output-on-failure
```

Run one case directly:

```bash
Release-build/bin/ass3 \
  Assignment-3/Tests/level-1/cases/svf_statements/public/01-stmt-zext-byte/01-stmt-zext-byte.ll

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-1/cases/svf_statements/public/01-stmt-zext-byte/01-stmt-zext-byte.ll
```

## Level 2

Level 2 is intentionally excluded from CTest because of its memory use. Run
both implementations directly from the repository root:

```bash
Release-build/bin/ass3 \
  Assignment-3/Tests/level-2/cases/42519094-curl-expanded-min-active-1k/repro.bc

python3 Assignment-3/Python/test-ae.py \
  Assignment-3/Tests/level-2/cases/42519094-curl-expanded-min-active-1k/repro.bc
```

## Level 3

Level 3 assesses large-program execution and report precision. No Level-3
fixture is published in this repository.
