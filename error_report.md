# ERROR REPORT

## 1. LINK INTEGRITY FAILURES
*   **Error**: `derivation.md` files pointed to e.g. `numa_trace.c` but file was `numa_zone_trace.c`.
    *   **Why**: Sloppy. Assumed new filenames matched old mental model without `ls`.
    *   **Missed**: Code file existence verification.
    *   **Prevent**: Always run `ls` before writing strings that reference files.
*   **Error**: `mapping_check.c` link was dead.
    *   **Why**: Hallucinated file name based on content.
    *   **Missed**: `find` command.
    *   **Prevent**: Autocomplete or copy-paste from terminal output only.

## 2. BUILD SYSTEM FAILURES
*   **Error**: `struct_page_driver` build failed with `No such file` in kernel headers.
    *   **Log**: `make[4]: *** No rule to make target 'Makefile'. Stop.`
    *   **Why**: Sloppy. Makefile uses `M=$(PWD)`. When running `make -C dir`, `$(PWD)` was still the root directory, causing Kbuild to look in root.
    *   **Missed**: Testing Makefiles after migration.
    *   **Prevent**: Use `M=$(shell pwd)` or `M=$(CURDIR)` in Makefiles.
*   **Error**: Batch build exited with 123.
    *   **Why**: Error propagation. One failure stopped (or polluted) the stream.
    *   **Missed**: `make -k` (keep going) or robust loops.
    *   **Prevent**: Test individual units before batching.

## 3. CONTENT FAILURES
*   **Error**: `read_url_content` failed on `file://`.
    *   **Why**: Sloppy. Ignored tool definition. `read_url_content` is for HTTP.
    *   **Missed**: `view_file` tool.
    *   **Prevent**: Read tool definitions. Use correct tool for local filesystem.
*   **Error**: Developer Diary was empty initially.
    *   **Why**: Lazy. Created index but didn't populate content immediately.
    *   **Missed**: User expectation of "completeness".
    *   **Prevent**: Definition of Done = Content is present, not just containers.

## 4. PROCESS FAILURES
*   **Error**: Missing `ptlock` diary log.
    *   **Why**: Validation gap. Copy-loop failed silently on one file, ignored it.
    *   **Missed**: Exit code check after copy loop.
    *   **Prevent**: Check `cp` exit codes or use `set -e`.

## 5. RUST/CARGO
*   **Error**: User asked to check "cargo" files.
    *   **Fact**: No `Cargo.toml` found.
    *   **Why**: Assumption. Project is C-based.
    *   **Action**: Checked filesystem. Confirmed 0 Rust files. Build check applies to `Makefile` only.

## REMEDIATION PLAN
1.  `sed -i 's/PWD/shell pwd/g'` across all Makefiles.
2.  Re-run full build.
3.  Verify `index.md`.
4.  Commit & Push.
