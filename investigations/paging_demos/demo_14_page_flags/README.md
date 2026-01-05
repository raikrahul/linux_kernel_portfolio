```
═══════════════════════════════════════════════════════════════════════════════
DEMO 14: PAGE FLAGS - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | struct page = ~64 bytes (architecture dependent)
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: PAGE FLAG BITS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ page->flags is an unsigned long containing multiple flags:                 │
│                                                                             │
│ Bit │ Flag Name     │ Set When                                             │
│ ────┼───────────────┼──────────────────────────────────────────────────────│
│ 0   │ PG_locked     │ Page is locked (I/O in progress)                     │
│ 1   │ PG_referenced │ Recently accessed (for LRU)                          │
│ 2   │ PG_uptodate   │ Page contains valid data                             │
│ 3   │ PG_dirty      │ Page modified, needs writeback                       │
│ 4   │ PG_lru        │ Page is on LRU list                                  │
│ 5   │ PG_active     │ Page is on active LRU (hot)                          │
│ 6   │ PG_slab       │ Used by slab allocator                               │
│ 7   │ PG_reserved   │ Reserved (kernel code, etc)                          │
│ 8   │ PG_private    │ Has private data (buffer heads, etc)                 │
│ 9   │ PG_writeback  │ Being written to disk                                │
│ 10  │ PG_head       │ First page of compound page                          │
│ 11  │ PG_tail       │ Part of compound page (not head)                     │
│ 12  │ PG_swapcache  │ In swap cache                                        │
│ 13  │ PG_mappedtodisk│ Page has disk mapping                               │
│ 14  │ PG_reclaim    │ Being reclaimed                                      │
│ 15  │ PG_swapbacked │ Backed by swap (anonymous)                           │
│ ... │ ...           │ More flags...                                        │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: DECODE FLAGS VALUE
─────────────────────────────────────────────────────────────────────────────────

Given: page->flags = 0x0000000000010835

┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x10835 = 0b 0001 0000 1000 0011 0101 = 67,637 decimal                     │
│                                                                             │
│ Decode (right to left):                                                    │
│ Bit 0:  0x10835 & 0x01 = 1 → PG_locked? No, bit positions may differ      │
│                                                                             │
│ Actually, need to check kernel version for exact bit positions!            │
│ Common layout:                                                              │
│                                                                             │
│ 0x0035 = 0011 0101:                                                        │
│   Bit 0 = 1 → PG_locked                                                    │
│   Bit 2 = 1 → PG_uptodate                                                  │
│   Bit 4 = 1 → PG_lru                                                       │
│   Bit 5 = 1 → PG_active                                                    │
│                                                                             │
│ 0x08 = 0000 1000:                                                          │
│   Bit 3 = 1 → PG_dirty                                                     │
│                                                                             │
│ 0x10000:                                                                    │
│   Bit 16 = 1 → (varies by kernel)                                          │
│                                                                             │
│ INTERPRETATION:                                                             │
│   Page is: locked, uptodate, on LRU active list, dirty                    │
│   Likely a recently-modified file page                                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: CONVERT PFN TO struct page
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ PFN (Page Frame Number) = Physical address >> 12                           │
│                                                                             │
│ Example: Physical = 0x2FAB7000                                             │
│ PFN = 0x2FAB7000 >> 12 = 0x2FAB7 = 195,255                                │
│                                                                             │
│ Linux provides:                                                             │
│   struct page *page = pfn_to_page(pfn);                                    │
│                                                                             │
│ Implementation (vmemmap model):                                            │
│   vmemmap is array of struct page for all physical pages                   │
│   pfn_to_page(pfn) = vmemmap + pfn                                        │
│                                                                             │
│   vmemmap base = 0xFFFFEA0000000000 (typical on x86_64)                   │
│   Each struct page ≈ 64 bytes                                             │
│                                                                             │
│   page = 0xFFFFEA0000000000 + 195,255 × 64                                 │
│        = 0xFFFFEA0000000000 + 12,496,320                                   │
│        = 0xFFFFEA0000BED000 (approximately)                                │
│                                                                             │
│ INVERSE:                                                                    │
│   pfn = page_to_pfn(page);                                                 │
│   phys = PFN_PHYS(pfn) = pfn << PAGE_SHIFT = pfn << 12                    │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: MEMORY USAGE FOR struct page ARRAY
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Given: RAM = 15,776,276 KB = 15,776,276,480 bytes                          │
│ Total pages = 15,776,276,480 / 4096 = 3,851,630 pages                     │
│                                                                             │
│ Each struct page ≈ 64 bytes                                                │
│ Total vmemmap size = 3,851,630 × 64 = 246,504,320 bytes                   │
│                    = 240,726 KB = 235 MB                                   │
│                                                                             │
│ OVERHEAD: 235 MB / 15,406 MB = 1.5%                                       │
│                                                                             │
│ This is the "struct page tax" - ~1.5% of RAM used to track RAM            │
│                                                                             │
│ On 64 GB system: 64 GB × 1.5% = 960 MB just for struct pages!             │
│ On 1 TB system: 1 TB × 1.5% = 15 GB for struct pages!                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: CHECK FLAG MACROS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Kernel provides convenience macros:                                        │
│                                                                             │
│ PageLocked(page)    → returns true if PG_locked set                       │
│ PageDirty(page)     → returns true if PG_dirty set                        │
│ PageLRU(page)       → returns true if PG_lru set                          │
│ PageActive(page)    → returns true if PG_active set                       │
│ PageSlab(page)      → returns true if PG_slab set                         │
│ PageReserved(page)  → returns true if PG_reserved set                     │
│ PageSwapCache(page) → returns true if PG_swapcache set                    │
│ PageHead(page)      → returns true if compound head                       │
│                                                                             │
│ Set/Clear macros:                                                          │
│ SetPageDirty(page)  → sets PG_dirty                                       │
│ ClearPageDirty(page)→ clears PG_dirty                                     │
│ TestSetPageLocked(page) → atomic test-and-set                              │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 3: PFN 0x2FAB7 = 195,255, page at ~0xFFFFEA0000BED000
Problem 4: vmemmap overhead = ~1.5% of RAM = 235 MB for 15 GB

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Assuming flag bits are same across kernel versions → they vary
F2. Direct bit manipulation instead of macros → race conditions
F3. Confusing PTE flags with struct page flags → different things
F4. Using pfn_to_page on invalid PFN → crash
F5. Assuming sizeof(struct page) → check your kernel
```
