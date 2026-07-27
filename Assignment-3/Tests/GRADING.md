# Assignment 3 Grading Cases

Assignment 3 is worth 100 points. The cases in this repository are public
examples, not the complete marking suite. Marking also uses hidden cases; their
number, distribution, and per-case weights are not published.

| Level | Points | Assessed capabilities |
| --- | ---: | --- |
| Level 1 | 30 | Individual abstract-execution features |
| Level 2 | 40 | Analysis of active real-program fixtures |
| Level 3 | 30 | Large-program execution and report precision |

## Level 1

The published Level-1 examples cover the following primary features. A public
example passes when it exits normally and all of its `svf_assert` or
`UNSAFE_*` checkpoints have the expected result.

| Published example | Primary feature |
| --- | --- |
| `01-stmt-zext-byte` | SVF statement transfer |
| `01-branch-var-const-true` | Branch refinement |
| `01-cycle-loop-counter` | Loop fixpoint |
| `01-extapi-memcpy-propagates` | External API summary |
| `01-null-unsafe-load-null` | Null-dereference detection |
| `01-buffer-gep-one-past` | Buffer-overflow detection |

## Level 2

Level 2 contributes 40 points overall. Marking considers whether the expected
target is reported, report precision, and completion without a crash,
assertion, or timeout.

The published `curl` fixture is a zero-FP active real-program example. Its
expected result and exact fixture hash are recorded in `level-2/manifest.csv`.
Runs are subject to a timeout in the marking environment.

## Level 3

Level 3 contributes 30 points overall. Marking considers target detection,
report precision, and successful large-program execution. No Level-3 fixture
is published in this repository.

Level-3 marking runs are subject to a timeout.

The published Level-2 example is not registered in CTest because of its memory
use. Run it directly as described in `README.md`.
