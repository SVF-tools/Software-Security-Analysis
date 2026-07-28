#!/usr/bin/env bash

set -euo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
repo_root=$(cd "$script_dir/.." && pwd)
build_dir=${1:-$repo_root}
failures=0

fail()
{
    printf 'ERROR: %s\n' "$*" >&2
    failures=$((failures + 1))
}

check_ctest_selector()
{
    selector=$1
    label=$2
    listing=$(ctest --test-dir "$build_dir" -N -R "$selector" 2>&1) || {
        fail "CTest could not list $label tests from $build_dir"
        return
    }
    count=$(printf '%s\n' "$listing" | sed -n 's/^[[:space:]]*Total Tests: \([0-9][0-9]*\)$/\1/p' | tail -1)
    if [[ -z "$count" || "$count" -eq 0 ]]; then
        fail "documented selector $selector matches no $label tests"
    else
        printf 'OK: %s selects %s test(s)\n' "$selector" "$count"
    fi
}

if [[ -f "$build_dir/CTestTestfile.cmake" ]]; then
    check_ctest_selector '^ass3-level-1-cpp/' 'Assignment 3 C++'
    check_ctest_selector '^ass3-level-1-py/' 'Assignment 3 Python'
    check_ctest_selector '^ass3-harness-py/reporter-contract$' \
        'Assignment 3 Python reporter-contract'
else
    printf 'SKIP: no configured CTest tree at %s\n' "$build_dir"
fi

documented_ir=$(
    {
        grep -ERoh --include='*.md' \
            'Assignment-3/Tests/[A-Za-z0-9._/-]+\.ll' \
            "$repo_root/Assignment-3" || true
        grep -Eoh 'Assignment-3/Tests/[A-Za-z0-9._/-]+\.ll' \
            "$repo_root/README.md" || true
    } |
        sort -u || true
)

if [[ -z "$documented_ir" ]]; then
    fail 'no public Assignment 3 LLVM IR paths are documented'
fi

while IFS= read -r relative_path; do
    [[ -z "$relative_path" ]] && continue
    if [[ ! -f "$repo_root/$relative_path" ]]; then
        fail "documented input does not exist: $relative_path"
    else
        printf 'OK: documented input exists: %s\n' "$relative_path"
    fi
done <<< "$documented_ir"

level1_sources_found=0
for source_path in "$repo_root"/Assignment-3/Tests/level-1/*.c; do
    [[ -e "$source_path" ]] || continue
    level1_sources_found=1
    ir_path=${source_path%.c}.ll
    if [[ ! -f "$ir_path" ]]; then
        fail "public source has no generated LLVM IR pair: ${source_path#"$repo_root/"}"
    fi
done
if [[ "$level1_sources_found" -eq 0 ]]; then
    fail 'no public Assignment 3 Level-1 sources were found'
fi

if grep -ERn --include='*.md' \
    'Assignment-3/Tests/(stmt|buf_overflow|null_deref)\.ll' \
    "$repo_root/Assignment-3" "$repo_root/README.md"; then
    fail 'obsolete flat Assignment 3 test paths remain in repository documentation'
fi

if grep -ERn --include='*.md' 'Release-build' "$repo_root/Assignment-3"; then
    fail 'Assignment 3 documentation hard-codes the unsupported Release-build layout'
fi

if [[ "$failures" -ne 0 ]]; then
    printf 'Assignment 3 release validation failed with %s error(s).\n' "$failures" >&2
    exit 1
fi

printf 'Assignment 3 release validation passed.\n'
