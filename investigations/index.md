# Investigations Log

Chronological record of axiomatic inquiries into the Linux Kernel.

## Batch 1: The Foundations

* **[Bootmem & E820](./bootmem_trace/derivation.md)**: How the kernel discovers RAM before it has an allocator.
* **[The Buddy Allocator](./buddy_fragment/derivation.md)**: Deriving the XOR buddy formula and fighting fragmentation.
* **[NUMA Zones](./numa_zone_trace/derivation.md)**: Why distance matters in memory access.
* **[Atomic Allocations (GFP)](./gfp_context_bug/derivation.md)**: Why you cannot sleep in an interrupt handler.
* **[The Struct Page](./struct_page_driver/derivation.md)**: The cost of metadata tracking.

## Batch 2: Mechanics of the Page

* **[Packed Flags](./flags_zone_node/derivation.md)**: Squeezing Node and Zone into `page->flags`.
* **[Malloc is a Lie](./malloc_pagefault/derivation.md)**: The difference between Virtual and Physical allocation.
* **[The Union](./metadata_union/derivation.md)**: How `struct page` shapeshifts based on state.
* **[Anon vs File](./anon_page_cache_metadata/derivation.md)**: Distinguishing Heap from Cache using pointer alignment.
* **[LRU & Eviction](./lru_mlock_union/derivation.md)**: The flow of hot and cold pages.

## Batch 3: Advanced Concurrency & Mapping

* **[Decoding the Map](./mapping_field_decode/derivation.md)**: A complete taxonomy of page states.
* **[Split Page Table Locks](./ptlock_split/derivation.md)**: Manufacturing contention to prove the need for fine-grained locking.
* **[Refcount & Fork Trace](./refcount_exercise/derivation.md)**: Proving Copy-On-Write mechanics via Kprobes and physical memory tracing.

## Batch 4: VMA & Maple Tree Tracing

* **[Maple Tree Walk](./malloc_pagefault/maple_tree_worksheet.md)**: Axiomatic derivation of VMA lookup via Maple Tree, including struct size, insertion sort, and pivot mechanics.
* **[VMA Lookup Driver](./maple_tree_vma_lookup/vma_lookup_worksheet.md)**: Proof that mmap() inserts VMA into Maple Tree before any page fault.
* **[__handle_mm_fault Trace](./handle_pte_fault_trace/handle_pte_fault_worksheet.md)**: Proving `struct vm_fault` accumulates data (PMD, address) to avoid stack spills during page table walk.
* **[__handle_mm_fault Trace](./handle_mm_fault_trace/handle_mm_fault_worksheet.md)**: Tracing the core page fault handler.

## Batch 5: Hardware Paging Demos

* **[Read CR3](./paging_demos/demo_01_read_cr3/)**: Inspecting the Page Global Directory pointer.
* **[Walk 4KB](./paging_demos/demo_07_walk_4kb/)**: Manual page table walk for standard pages.
* **[Walk 2MB](./paging_demos/demo_08_walk_2mb/)**: Handling Huge Pages in software.
* **[VMA Walk](./paging_demos/demo_15_vma_walk/)**: Traversing `vm_area_struct` red-black trees.
* **[TLB Flush](./paging_demos/demo_18_tlb_flush/)**: Invalidating translation caches.

## Batch 6: Imported Kernel Exercises

* **[Buddy Fragment](./kernel_exercises/buddy_fragment/)**: Investigating fragmentation in the buddy allocator.
* **[Page Table Split Lock](./kernel_exercises/ptlock_split/)**: Concurrency primitives for page tables.
* **[Malloc Pagefault](./kernel_exercises/malloc_pagefault/)**: User-kernel boundary analysis.
* **[Full Exercise Archive](./kernel_exercises/)**: Complete collection of 80+ kernel drills.

---
*Return to [Home](../README.md)*
