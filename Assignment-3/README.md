# Assignment 3: Abstract Execution

This document is the versioned, authoritative contract for Assignment 3.
The [course wiki](https://github.com/SVF-tools/Software-Security-Analysis/wiki/Assignment-3)
provides navigation and API reference material. If a wiki page and this file
disagree about the assignment, follow this file from the repository revision
you are using.

## Deliverables

Implement an abstract executor over SVF's interval and address-set domains.
It must analyse interprocedural control flow, model the listed external APIs,
and detect buffer overflows and null-pointer dereferences.

| Language | Files accepted for submission |
| --- | --- |
| C++ | `Assignment_3.cpp` and `Assignment_3.h` |
| Python | `Assignment_3.py` |

You may add helpers inside the accepted files. The other files under
`Assignment-3/` are supplied infrastructure and are replaced during marking.

The harness starts `analyse()` and calls, directly or indirectly, these five
student entry points:

| Entry point | Required responsibility |
| --- | --- |
| `handleGlobalNode` | Initialise and execute the global ICFG node. |
| `handleFunction` | Walk a function's interprocedural WTO components. |
| `handleICFGNode` | Merge incoming state, execute the node, run checkers, and return whether its post-state changed. |
| `handleICFGCycle` | Compute a terminating loop or recursive-SCC fixpoint. |
| `handleCallSite` | Dispatch stubs, external calls, ordinary calls, and recursive calls. |

You may choose a different internal decomposition, but every reachable node,
assertion, and checkpoint must be handled.

## Required tasks and semantics

### 1. Statement transfer functions

Initialise global and allocated objects, then dispatch every statement on the
current ICFG node. The required abstract effects are:

| Statement family | Required effect |
| --- | --- |
| Address | Bind the result to the abstract address of its object; initialise allocated object memory. |
| Copy and cast | Copy the source abstract value to the result without changing its value kind. |
| Binary and comparison | Apply the corresponding abstract interval operator. |
| Load | Read the joined value of every address represented by the pointer. |
| Store | Write the value through every represented address. |
| GEP | Add the abstract byte offset to every represented base address. |
| Phi and select | Join only values from feasible incoming alternatives. |
| Call parameter | Propagate the actual value to its formal parameter. |
| Return parameter | Propagate the callee's return value to the caller's result. |

Preserve explicit null addresses. Do not silently convert an address set to an
interval or an interval to an address set. Use the supplied `AEState` and
abstract-execution helpers rather than maintaining a second, inconsistent
memory model.

### 2. Branch feasibility

For each incoming edge:

1. Clone the predecessor post-state.
2. For a conditional or switch edge, meet the condition interval with the
   edge's required value.
3. Discard the cloned state if the meet is bottom.
4. Join all remaining states.

Keep branch refinements local to that edge. Preserve the function-entry
pre-state if no ordinary predecessor state is available. Phi values must use
only feasible incoming paths.

### 3. Cycle and recursion fixpoint

Walk the supplied WTO in order. Handle singleton components once and cycle
components with the following policy:

```text
old_head = bottom
for iteration = 0, 1, ...:
    execute the cycle head and body in WTO order
    new_head = current head state
    if new_head == old_head:
        break
    if iteration >= widen_delay:
        store widening(old_head, new_head) at the head
    old_head = stored head state

repeat in WTO order using narrowing(old_head, new_head)
until the head state no longer changes
```

Use `Options::WidenDelay()` in C++ and `self.widen_delay` in Python. Apply the
same mechanism to loops and recursive call-graph SCCs. Nested cycles are
visited in WTO order. Iteration must terminate; the harness imposes an
emergency cap of 10,000 cycle iterations.

Python state ownership is significant: `clone()`, `widening()`, and
`narrowing()` return raw `pysvf.AbstractState` objects. Wrap a cloned raw
state in `AEState` before placing it in `pre_abs_trace` or
`post_abs_trace`; do not keep a mutable trace alias as an iteration snapshot.

### 4. External-API summaries

`handleCallSite` must dispatch assertion/checkpoint stubs first, then
`nd`/`rand`, the supported external functions, ordinary callees, and
recursive call sites. `nd` and `rand` return top.

For the table below, `dst`, `src`, `n`, and `pos` denote the corresponding
call arguments. A copy transfers the abstract byte values in the indicated
range. The destination is also checked as a write and the source as a read.

| Function | Required value/memory summary |
| --- | --- |
| `memcpy(dst, src, n)` | Copy `n` bytes from `src` to `dst`; return `dst`. |
| `memmove(dst, src, n)` | As `memcpy`, but read the source values before writing overlapping destinations; return `dst`. |
| `memset(dst, value, n)` | Store the low byte of `value` into `n` bytes at `dst`; return `dst`. |
| `strcpy(dst, src)` | Copy `strlen(src) + 1` bytes, including the null terminator, from offset 0; return `dst`. |
| `strncpy(dst, src, n)` | Copy exactly `n` bytes from offset 0, padding with null bytes when the source is shorter; return `dst`. |
| `strcat(dst, src)` | Copy `strlen(src) + 1` bytes, including the terminator, at destination offset `strlen(dst)`; return `dst`. |
| `strncat(dst, src, n)` | Copy at most `n` non-null source bytes at destination offset `strlen(dst)`, then append a null terminator; return `dst`. |
| `strlen(src)` | Return an interval containing the possible index of the first null byte. |
| `wcslen(src)` | As `strlen`, measured in wide-character elements. |
| `mem_insert(buffer, data, data_size, position)` | Copy `data_size` bytes from `data` to `buffer + position`. |
| `str_insert(buffer, data, position)` | Copy `strlen(data)` bytes from `data` to `buffer + position`. |

If a size, length, or position is an interval, conservatively join the effects
of its possible range. If it is unknown, do not invent a precise constant.
An unlisted external call has no assignment-specific summary: leave the
abstract state unchanged and do not invent a return value.

Run the null and bounds checkers on pointer arguments dereferenced by a
summary. At minimum:

| Operation | Pointer arguments dereferenced |
| --- | --- |
| `memcpy`, `memmove`, `strcpy`, `strncpy`, `strcat`, `strncat` | destination and source |
| `memset` | destination |
| `strlen`, `wcslen` | source |
| `mem_insert`, `str_insert` | buffer and data |

### 5. Buffer-overflow checker

Check direct load, store, and GEP accesses plus the external operations above.
For every possible concrete target object:

1. obtain the base object and its byte size;
2. accumulate the pointer's byte offset from that base;
3. include the access length in the accessed byte range; and
4. report if the lower bound can be below zero or the upper bound can be at
   least the object size.

Do not report a scalar that is not an address. If the target is null,
black-hole, or has unknown size, leave that target to the null/unknown policy
instead of inventing a buffer size. Report the actual access node with
`reportBufOverflow(node)`.

### 6. Null-pointer-dereference checker

Check the pointer operand of loads, stores, GEPs, and supported external
operations. A pointer is unsafe if its address set may contain the null
address or an address recorded as freed. A black-hole address alone is
unknown, not proof of null. Branch refinement may establish a pointer as
non-null.

Report the actual dereference node with `reportNullDeref(node)`. For an
external operation, report its call node.

## Assertions, checkpoints, and reports

The public cases use `svf_assert`, `svf_assert_eq`, and these checkpoint
stubs:

- `SAFE_BUFACCESS` / `UNSAFE_BUFACCESS`
- `SAFE_PTRDEREF` / `UNSAFE_PTRDEREF`

Checkpoints are ground-truth expectation markers, not detector
implementations. Every marker must be reachable. Each `UNSAFE_BUFACCESS`
records one expected `buffer-overflow` report and each `UNSAFE_PTRDEREF`
records one expected `nullptr-deref` report. Safe markers add no expected
report. For each bug kind represented by at least one checkpoint in a case,
the harness compares expected and actual counts exactly. A real-program case
with no checkpoint for a kind may still emit reports of that kind.
Consequently:

- an unsafe marker passes only when student analysis reports a real unsafe
  access;
- a missed report fails;
- a report of the wrong kind fails; and
- an extra report, including a report at a safe access, fails.

Duplicate reports of the same kind at the same ICFG node are counted once.
Different bug kinds at one node remain separate reports.

## Build and run

Commands below are run from the repository root. Choose the build layout that
matches your checkout:

```bash
# A normal out-of-source checkout:
BUILD_DIR=build

# The supplied Docker image is already configured in source:
# BUILD_DIR=.
```

Build the C++ implementation:

```bash
cmake --build "$BUILD_DIR" --target ass3 -j8
```

Run all registered public Level-1 cases:

```bash
ctest --test-dir "$BUILD_DIR" \
  -R '^ass3-level-1-cpp/' --output-on-failure

ctest --test-dir "$BUILD_DIR" \
  -R '^ass3-level-1-py/' --output-on-failure

# Both languages:
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

CMake gives each public C++ case 120 seconds and each public Python case 180
seconds. These commands run all *registered public Level-1 cases*, not the
complete assignment marking suite. The Level-2 fixture is intentionally run
directly because of its memory use; see
[Tests/README.md](Tests/README.md).

### Write an additional case

Write a small C program that declares the needed stubs, exercises one
property, and places a checkpoint after the real access. Compile with the
course's LLVM 21 `clang`:

```bash
clang -g -O0 -fno-discard-value-names -fno-builtin \
  -S -emit-llvm my-case.c -o my-case.ll

"$BUILD_DIR/bin/ass3" my-case.ll
python3 Assignment-3/Python/test-ae.py my-case.ll
```

Keep cases deterministic and feature-focused. Do not replace the real
operation with only an `UNSAFE_*` marker.

## Assessment

Assignment 3 is worth 100 points:

| Level | Points | Assessed capability |
| --- | ---: | --- |
| Level 1 | 30 | Individual abstract-execution and checker features. |
| Level 2 | 40 | End-to-end analysis of active real-program fixtures. |
| Level 3 | 30 | Large-program execution, target detection, and report precision. |

Public examples support development; they are not the complete marking suite.
Hidden case counts, per-case weights, and marking timeouts are controlled by
the marking environment and are not published.

For Levels 2 and 3:

- **target detection** means reporting the ground-truth unsafe operation;
- **report precision** distinguishes target reports from non-target reports;
- **completion** means finishing without a crash, failed assertion, or
  timeout; and
- **coverage**, when shown by the harness, is the percentage of reachable
  ICFG nodes analysed.

Passing every public test does not guarantee a particular mark.

## Submit

Submit from the directory containing the implementation. File names are
case-sensitive:

```bash
# C++
give cs6131 ass3 Assignment_3.cpp Assignment_3.h

# Python
give cs6131 ass3 Assignment_3.py
```

For C++, both files are required. A successful submission reports
`Your submission is ACCEPTED.` You may resubmit before the deadline; only the
latest accepted submission is marked.
