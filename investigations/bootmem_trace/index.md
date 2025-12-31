---
layout: default
title: Bootmem Trace Exercise
---

# Bootmem Trace: Page Allocation and Refcount Bug

## Files

| File | Purpose |
|------|---------|
| `bootmem_trace.c` | Kernel module source with axiomatic comments |
| `worksheet.md` | Step-by-step hand calculations and error report |
| `Makefile` | Build configuration |

## Build and Run

```bash
cd kernel_exercises/bootmem_trace
make
cat /proc/buddyinfo  # Record before
sudo insmod bootmem_trace.ko
dmesg | tail -30     # Observe values
sudo rmmod bootmem_trace
cat /proc/buddyinfo  # Compare after
```

## Exercise Goal

1. Trace page allocation from axioms
2. Calculate PFN, physical address, zone by hand
3. Trace _refcount transitions: 1→2→1→0→-1
4. Observe kernel BUG on refcount underflow

## Worksheet Contents

- Lines 01-30: RAM, byte, page, PFN definitions
- Lines 31-51: vmemmap, zone definitions
- Lines 52-72: _refcount, GFP_KERNEL derivation
- Lines 73-91: Code mapping and execution
- Lines 92-144: Error report and brain analysis
