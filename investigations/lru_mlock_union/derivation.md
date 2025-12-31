# Investigation: The LRU Lists and mlock

**Date**: 2025-12-29
**Subject**: Page Replacement & Eviction
**Machine**: x86_64

---

## 1. Axiomatic Foundation

### The Reclaim Problem
*   **AXIOM**: RAM is finite.
*   **PROBLEM**: When RAM is full, we must "Evict" a page to make room.
*   **CHOICE**: Which page?
    *   Random? Bad performance.
    *   Oldest? Better.
    *   Least Recently Used (LRU)? Best.

## 2. Derivation: The Active/Inactive Logic

The kernel maintains two main lists for each zone:
1.  **Active LRU**: Pages currently being accessed. Hot.
2.  **Inactive LRU**: Pages not accessed recently. Cold. Candidates for eviction.

### The Algorithm
1.  Page accessed? Move to Active Head.
2.  Memory pressure? Move Active Tail to Inactive Head.
3.  Evict Inactive Tail (Write to disk if dirty, then free).

### mlock()
*   **DEFINITION**: `mlock(addr, len)` tells the kernel: "Never evict these pages."
*   **MECHANISM**: Page is removed from LRU lists entirely. `page->flags` gets `PG_mlocked`.

## 3. Experiment: Tracing Eviction Candidates

*   [lru_trace.c](https://github.com/raikrahul/linux_kernel_portfolio/blob/main/investigations/lru_mlock_union/lru_mlock_hw.c): Inspecting `page->lru` pointers.
*   **OBSERVATION**:
    *   Standard pages: `lru.next` and `lru.prev` point to other pages.
    *   Mlocked pages: `lru` pointers are NULL or point to special "Unevictable" list.

## 4. Conclusion
The LRU lists are the circulatory system of memory. Pages flow from Inactive to Active (Promotion) and Active to Inactive (Demotion). `mlock` is a clot that holds a page in place, immune to the flow.
