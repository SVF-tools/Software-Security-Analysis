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

The published Level 1 examples cover the following primary features. A public
example passes when it exits normally, reaches every `svf_assert` and
checkpoint, validates every assertion, and produces exactly the expected
report count for each bug kind.

| Published example | Primary feature |
| --- | --- |
| `01-stmt-basic` | SVF statement transfer |
| `02-branch-feasible` | Branch feasibility |
| `03-buffer-overflow` | Real out-of-bounds access detection |
| `04-nullptr-deref` | Real null-pointer-dereference detection |
| `05-loop-fixpoint` | Loop widening and narrowing |
| `06-recursion-fixpoint` | Recursive-SCC fixpoint |
| `07-interprocedural-call-return` | Actual/formal and return propagation |
| `08-memory-summary` | Memory external-call summaries |
| `09-string-summary` | String external-call summaries |
| `10-safe-memory` | Safe-access report precision |

The public CTest timeout is 120 seconds per C++ case and 180 seconds per
Python case.

## Level 2

Level 2 contributes 40 points overall. Marking considers whether the expected
target is reported, report precision, and completion without a crash,
assertion, or timeout.

The published `curl` fixture is an active real-program example with one
intentional one-past buffer access. Runs are subject to a timeout in the
marking environment.

## Level 3

Level 3 contributes 30 points overall. Marking considers target detection,
report precision, and successful large-program execution. No Level-3 fixture
is published in this repository.

Level 3 marking runs are subject to a timeout.

The published Level 2 example is not registered in CTest because of its memory
use. Run it directly as described in `README.md`.

For Levels 2 and 3:

- target detection means reporting the ground-truth unsafe operation;
- report precision distinguishes target reports from non-target reports;
- completion means finishing without a crash, failed assertion, or timeout;
  and
- coverage, when displayed, is the percentage of reachable ICFG nodes
  analysed.

Hidden case counts, weights, and timeouts are controlled by the marking
environment and are not published. Passing every public case does not
guarantee a particular mark. The complete semantic and checkpoint contract is
in [`Assignment-3/README.md`](../README.md).
