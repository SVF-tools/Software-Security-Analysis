# 42519094 curl active 1k fallback

Source:

- Original target: `/src/curl/lib/curl_fnmatch.c`
- Linked C sources: `repro.c`, `target_min.c`
- Driver: `repro.c`, C-only.
- LOC: 2849

Driver policy:

- The active prelude exercises original container APIs before the final target
  trigger.
- The final trigger uses an undersized output buffer to reproduce the ARVO
  buffer-overflow statement.

Validation:

- ASAN confirms the target at `curl_fnmatch.c:389`.
- C++ command:
  `Release-build/bin/ass3 Assignment-3/Tests/level-2/cases/42519094-curl-expanded-min-active-1k/repro.bc`
- Python command:
  `python3 Assignment-3/Python/test-ae.py Assignment-3/Tests/level-2/cases/42519094-curl-expanded-min-active-1k/repro.bc`
- ICFG coverage: 7938 / 7939 = 99.99%
- Function coverage: 285 / 285 = 100.00%
- Buffer-overflow reports: 1
- TP: 1
- FP: 0

Status:

- `validated_expanded_min_active`
