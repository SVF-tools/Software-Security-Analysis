# 42519094 curl active fixture

Source:

- Original target provenance: `/src/curl/lib/curl_fnmatch.c:389`
- Single self-contained C source: `repro.c`
- C source size: 707 lines
- Generated IR size: 3,315 lines and 71 function definitions

Driver policy:

- The driver calls 66 active prelude functions before the final target
  trigger.
- The final trigger writes one byte past a four-byte heap allocation.
- The C source and all generated artifacts are flat in this directory.

Validation:

- ASAN confirms a heap-buffer-overflow in `trigger_42519094` in `repro.c`.
- C++ command:
  `Release-build/bin/ass3 Assignment-3/Tests/level-2/repro.ll`
- Python command:
  `python3 Assignment-3/Python/test-ae.py Assignment-3/Tests/level-2/repro.ll`
- Docker image: `svftools/software-security-analysis:latest` (`linux/amd64`)
- Container limit: 8,589,934,592 bytes with swap disabled
- C++: exit status 0; sampled cgroup peak 4,174,487,552 bytes
- Python 3.12: exit status 0; sampled cgroup peak 3,921,240,064 bytes
- Expected result: exactly one buffer-overflow report in
  `trigger_42519094` in `repro.c`

Status:

- `validated_8g`
