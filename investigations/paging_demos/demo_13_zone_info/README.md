```
═══════════════════════════════════════════════════════════════════════════════
DEMO 13: ZONE INFO - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | RAM = 15406 MB
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: WHAT ARE MEMORY ZONES?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Linux divides physical memory into ZONES based on hardware constraints:    │
│                                                                             │
│ ZONE_DMA (0 - 16 MB):                                                      │
│   - For legacy ISA DMA which can only address 24 bits                      │
│   - 2^24 = 16,777,216 bytes = 16 MB                                        │
│   - Rarely used on modern systems                                          │
│                                                                             │
│ ZONE_DMA32 (16 MB - 4 GB):                                                 │
│   - For devices with 32-bit DMA addressing                                 │
│   - 2^32 = 4,294,967,296 bytes = 4 GB                                      │
│   - Common for older PCI devices                                           │
│                                                                             │
│ ZONE_NORMAL (4 GB - end of RAM):                                           │
│   - Main memory zone for general use                                       │
│   - No addressing restrictions                                              │
│   - Most allocations come from here                                        │
│                                                                             │
│ ZONE_HIGHMEM (x86-32 only):                                                │
│   - Memory above ~896 MB on 32-bit systems                                 │
│   - Not present on 64-bit systems (you)                                    │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: CALCULATE YOUR ZONE SIZES
─────────────────────────────────────────────────────────────────────────────────

Given: RAM = 15,776,276 KB = 15406 MB

┌─────────────────────────────────────────────────────────────────────────────┐
│ ZONE_DMA:                                                                   │
│   Range: 0x00000000 - 0x00FFFFFF (0 - 16 MB)                               │
│   Size: 16 MB = 16 × 1024 KB = 16,384 KB                                   │
│   Pages: 16,384 × 1024 / 4096 = 4,096 pages                                │
│                                                                             │
│ ZONE_DMA32:                                                                 │
│   Range: 0x01000000 - 0xFFFFFFFF (16 MB - 4 GB)                            │
│   Size: 4 GB - 16 MB = 4096 MB - 16 MB = 4080 MB                          │
│   Pages: 4080 × 1024 × 1024 / 4096 = 1,044,480 pages                      │
│                                                                             │
│ ZONE_NORMAL:                                                                │
│   Range: 0x100000000 - end of RAM (4 GB - 15.4 GB)                         │
│   Size: 15406 MB - 4096 MB = 11,310 MB                                    │
│   Pages: 11,310 × 1024 × 1024 / 4096 = 2,895,360 pages                    │
│                                                                             │
│ VERIFY:                                                                     │
│   Total pages = 4,096 + 1,044,480 + 2,895,360 = 3,943,936                 │
│   Total bytes = 3,943,936 × 4096 = 16,158,359,552 bytes                   │
│   Total MB = 15,409 MB ≈ 15,406 MB ✓ (rounding error)                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: WHY ZONES MATTER FOR ALLOCATION
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ GFP_KERNEL:                                                                 │
│   - Prefers ZONE_NORMAL                                                    │
│   - Falls back to ZONE_DMA32, then ZONE_DMA if needed                     │
│   - Most common allocation flag                                            │
│                                                                             │
│ GFP_DMA:                                                                    │
│   - ONLY allocates from ZONE_DMA (0-16 MB)                                 │
│   - Required for legacy ISA devices                                        │
│                                                                             │
│ GFP_DMA32:                                                                  │
│   - ONLY allocates from ZONE_DMA or ZONE_DMA32 (0-4 GB)                   │
│   - For devices with 32-bit addressing limits                              │
│                                                                             │
│ PROBLEM SCENARIO:                                                           │
│   Device needs DMA buffer below 4 GB                                       │
│   ZONE_DMA32 is exhausted                                                  │
│   Allocation fails even though ZONE_NORMAL has free pages                  │
│   → This is why we track per-zone free pages                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: si_meminfo STRUCTURE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ struct sysinfo {                                                            │
│     unsigned long totalram;   // Total usable RAM (pages)                  │
│     unsigned long freeram;    // Free RAM (pages)                          │
│     unsigned long sharedram;  // Shared RAM (pages)                        │
│     unsigned long bufferram;  // Buffer cache (pages)                      │
│     unsigned long totalswap;  // Total swap space (pages)                  │
│     unsigned long freeswap;   // Free swap space (pages)                   │
│     unsigned short procs;     // Number of processes                       │
│     unsigned long totalhigh;  // High memory size (0 on 64-bit)            │
│     unsigned long freehigh;   // Free high memory (0 on 64-bit)            │
│     unsigned int mem_unit;    // Memory unit size (usually 1)              │
│ };                                                                          │
│                                                                             │
│ Call: si_meminfo(&info);                                                   │
│                                                                             │
│ CALCULATE FROM YOUR SYSTEM:                                                │
│ totalram = 15,776,276 KB / 4 KB = 3,944,069 pages                         │
│ freeram = varies (check /proc/meminfo "MemFree")                           │
│                                                                             │
│ To get bytes: total_bytes = info.totalram * info.mem_unit                 │
│ Usually mem_unit = PAGE_SIZE = 4096                                        │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: READ /proc/zoneinfo
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ cat /proc/zoneinfo | grep -A5 "zone.*DMA\|zone.*Normal"                   │
│                                                                             │
│ Example output:                                                             │
│ Node 0, zone    DMA                                                        │
│   pages free     3892                                                       │
│   min      21                                                               │
│   low      26                                                               │
│   high     31                                                               │
│   spanned  4095                                                             │
│   present  3998                                                             │
│                                                                             │
│ INTERPRETATION:                                                             │
│   spanned: Total pages zone COULD contain                                  │
│   present: Pages actually installed in this zone                           │
│   free: Currently free pages                                               │
│   min/low/high: Watermarks for kswapd                                      │
│                                                                             │
│   When free < low: kswapd starts reclaiming                                │
│   When free < min: Direct reclaim (allocation blocked)                    │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: DMA=4096 pages, DMA32≈1M pages, Normal≈2.9M pages
Problem 4: totalram ≈ 3,944,069 pages

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Assuming zone symbols exported → many are not
F2. Using GFP_KERNEL for DMA buffer → address > 4GB possible
F3. Ignoring zone exhaustion → allocation fails unexpectedly
F4. Confusing pages with bytes → off by factor of 4096
F5. Not checking mem_unit → wrong total calculation
```
