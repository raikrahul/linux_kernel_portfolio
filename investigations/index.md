# Investigations Log

Chronological record of axiomatic inquiries into the Linux Kernel.

## Batch 1: The Foundations
*   **[Bootmem & E820](./bootmem_trace/derivation.md)**: How the kernel discovers RAM before it has an allocator.
*   **[The Buddy Allocator](./buddy_fragment/derivation.md)**: Deriving the XOR buddy formula and fighting fragmentation.
*   **[NUMA Zones](./numa_zone_trace/derivation.md)**: Why distance matters in memory access.
*   **[Atomic Allocations (GFP)](./gfp_context_bug/derivation.md)**: Why you cannot sleep in an interrupt handler.
*   **[The Struct Page](./struct_page_driver/derivation.md)**: The cost of metadata tracking.

## Batch 2: Mechanics of the Page
*   **[Packed Flags](./flags_zone_node/derivation.md)**: Squeezing Node and Zone into `page->flags`.
*   **[Malloc is a Lie](./malloc_pagefault/derivation.md)**: The difference between Virtual and Physical allocation.
*   **[The Union](./metadata_union/derivation.md)**: How `struct page` shapeshifts based on state.
*   **[Anon vs File](./anon_page_cache_metadata/derivation.md)**: Distinguishing Heap from Cache using pointer alignment.
*   **[LRU & Eviction](./lru_mlock_union/derivation.md)**: The flow of hot and cold pages.

## Batch 3: Advanced Concurrency & Mapping
*   **[Decoding the Map](./mapping_field_decode/derivation.md)**: A complete taxonomy of page states.
*   **[Split Page Table Locks](./ptlock_split/derivation.md)**: Manufacturing contention to prove the need for fine-grained locking.
*   **[Refcount & Fork Trace](./refcount_exercise/derivation.md)**: Proving Copy-On-Write mechanics via Kprobes and physical memory tracing.

---
*Return to [Home](../README.md)*
