# Assignment 3 Grading Cases

Assignment 3 is worth 30 points. The cases in this repository are public
examples, not the complete marking suite. Marking also uses hidden cases; their
number, distribution, and per-case weights are not published.

| Level | Points | Assessed capabilities |
| --- | ---: | --- |
| Level 1 | 10 | Individual abstract-execution features |
| Level 2 | 10 | Analysis of active real-program fixtures |
| Level 3 | 10 | Large-program execution and report precision |

## Level 1

The published Level 1 examples cover the following primary features. A public
example passes when it exits normally and all of its `svf_assert` or
`UNSAFE_*` checkpoints have the expected result.

| Published example | Primary feature |
| --- | --- |
| `01-stmt-basic` | SVF statement transfer |
| `02-branch-feasible` | Branch feasibility |
| `03-buffer-overflow` | Buffer-overflow detection |
| `04-nullptr-deref` | Null-pointer-dereference detection |

## Level 2

Level 2 contributes 10 points overall. Marking considers whether the expected
target is reported, report precision, and completion without a crash,
assertion, or timeout.

The published `curl` fixture is an active real-program example with one
intentional one-past buffer access. Runs are subject to a timeout in the
marking environment.

## Level 3

Level 3 contributes 10 points overall. Marking considers target detection,
report precision, and successful large-program execution. No Level-3 fixture
is published in this repository.

Level 3 marking runs are subject to a timeout.

The published Level 2 example is not registered in CTest because of its memory
use. Run it directly as described in `README.md`.
