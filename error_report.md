# Error Report: Session Audit

## LIST OF ERRORS

1.  **investigations/buddy_fragment/Makefile:4**
    *   **What went wrong**: Used `PWD := $(shell pwd)`.
    *   **What should be**: `PWD := $(shell pwd)` is correct, but target `modules` was missing or implicit rule failed.
    *   **Why sloppy**: Assumed standard Kbuild behavior without checking implicit rules.
    *   **What missed**: The `all:` target definition.
    *   **How to prevent**: Explicitly define `all: modules` in every Makefile.

2.  **investigations/bootmem_trace/bootmem_trace.c:46**
    *   **What went wrong**: Used `max_pfn` symbol.
    *   **What should be**: Symbol is not exported to modules. Should use available APIs or skip.
    *   **Why sloppy**: Assumed core kernel symbols are available to modules.
    *   **What missed**: `grep "max_pfn" /proc/kallsyms` check or `Module.symvers` check.
    *   **How to prevent**: Verify symbol export status before using in module code.

3.  **investigations/buddy_fragment/buddy_fragment.c:Global**
    *   **What went wrong**: Initial comments were descriptive, not numerical.
    *   **What should be**: Strict 7-W format with Punishment-level math.
    *   **Why sloppy**: Defaulted to "teaching" tone instead of "derive from axioms".
    *   **What missed**: User's specific "Primate/Punishment" instruction.
    *   **How to prevent**: Re-read user constraints 3 times before generating text.

4.  **investigations/numa_zone_trace/numa_zone_trace.c:Global**
    *   **What went wrong**: Defaulted to narrative explanation.
    *   **What should be**: Dense axiomatic trace.
    *   **Why sloppy**: Habit.
    *   **What missed**: The "single, dense paragraph" rule.
    *   **How to prevent**: Apply strict template to all file generations by default.

5.  **General Project Structure**
    *   **What went wrong**: Relative paths in markdown (`./file.c`).
    *   **What should be**: GitHub blob URLs for proper rendering.
    *   **Why sloppy**: Forgot GitHub Pages raw text limitation.
    *   **What missed**: User's initial complaint about "raw text".
    *   **How to prevent**: Use absolute URLs for code files in documentation.

## VIOLATION CHECK
*   **Adjectives used**: Minimal (e.g. "Strict", "Initial").
*   **Stories**: None.
*   **Format**: List.
*   **Tone**: Critical.
