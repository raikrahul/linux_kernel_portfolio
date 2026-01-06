# Linux Kernel Portfolio

Axiomatic kernel investigations. Zero hand-waving.

## Investigations

| Name | Topic |
|------|-------|
| [anon_page_cache_metadata](investigations/anon_page_cache_metadata/) | Anonymous page metadata |
| [bootmem_trace](investigations/bootmem_trace/) | Boot memory allocator |
| [buddy_fragment](investigations/buddy_fragment/) | Buddy allocator fragmentation |
| [flags_zone_node](investigations/flags_zone_node/) | Page flags, zones, nodes |
| [gfp_context_bug](investigations/gfp_context_bug/) | GFP flags in atomic context |
| [handle_mm_fault_trace](investigations/handle_mm_fault_trace/) | Page fault handling |
| [handle_pte_fault_trace](investigations/handle_pte_fault_trace/) | PTE fault tracing |
| [kernel_exercises](investigations/kernel_exercises/) | Kernel exercises |
| [lru_mlock_union](investigations/lru_mlock_union/) | LRU and mlock union |
| [malloc_pagefault](investigations/malloc_pagefault/) | malloc page faults |
| [maple_tree_vma_lookup](investigations/maple_tree_vma_lookup/) | Maple tree VMA lookup |
| [mapping_field_decode](investigations/mapping_field_decode/) | struct page mapping field |
| [metadata_union](investigations/metadata_union/) | Page metadata union |
| [numa_zone_trace](investigations/numa_zone_trace/) | NUMA zone tracing |
| [paging_demos](investigations/paging_demos/) | Paging demonstrations |
| [ptlock_split](investigations/ptlock_split/) | Page table lock splitting |
| [refcount_exercise](investigations/refcount_exercise/) | Page refcounting |
| [struct_page_course](investigations/struct_page_course/) | struct page course |
| [struct_page_driver](investigations/struct_page_driver/) | struct page driver |

## Docs

| File | Topic |
|------|-------|
| [probe_0_logic_trace.md](docs/probe_0_logic_trace.md) | Probe logic trace |
| [step_0_comprehensive.md](docs/step_0_comprehensive.md) | Comprehensive walkthrough |

## Other

| File | Topic |
|------|-------|
| [error_report.md](error_report.md) | Error log |
| [README.md](README.md) | Repository overview |

---

Built on Linux 6.14+ x86_64. Data from real machine traces.
