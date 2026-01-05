```
═══════════════════════════════════════════════════════════════════════════════
DEMO 20: SWAP INFO - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Swap: /swap.img 4095 MB | Used: ~1380 MB
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: SWAP ENTRY FORMAT
─────────────────────────────────────────────────────────────────────────────────

WHEN PAGE IS SWAPPED OUT:
┌─────────────────────────────────────────────────────────────────────────────┐
│ PTE format when P=0 (not present, but was swapped):                        │
│                                                                             │
│ ┌─────────────────────────────────────────────────────────────┬─────┐     │
│ │                    Offset (58 bits)                         │Type │ P=0 │
│ │                    [63:6]                                   │[5:1]│ [0] │
│ └─────────────────────────────────────────────────────────────┴─────┴─────┘
│                                                                             │
│ Type (5 bits): Index into swap_info array (0-31 devices)                   │
│ Offset (58 bits): Position in swap file/partition                          │
│ P = 0: Indicates not present (triggers page fault)                         │
│                                                                             │
│ Example: PTE = 0x0000001234567802                                          │
│ P = 0x...2 & 1 = 0 (not present)                                          │
│ Type = (0x...2 >> 1) & 0x1F = 1 (swap device 1)                           │
│ Offset = 0x0000001234567802 >> 6 = 0x48D159E0                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: SWAP CAPACITY CALCULATION
─────────────────────────────────────────────────────────────────────────────────

Given: /swap.img size = 4,194,300 KB

┌─────────────────────────────────────────────────────────────────────────────┐
│ Swap size = 4,194,300 KB = 4,194,300 × 1024 bytes                          │
│           = 4,294,963,200 bytes ≈ 4 GB                                     │
│                                                                             │
│ Swap slots = 4,294,963,200 / 4096 = 1,048,576 slots                       │
│            = 0x100000 slots = 2^20 slots                                   │
│                                                                             │
│ Can offset fit in 58 bits?                                                 │
│ Max offset with 58 bits = 2^58 = 288,230,376,151,711,744                  │
│ We need only 1,048,576 = 2^20                                              │
│ 2^20 << 2^58, plenty of room ✓                                            │
│                                                                             │
│ Maximum swap file with 58-bit offset:                                      │
│ Max = 2^58 × 4 KB = 2^58 × 2^12 = 2^70 bytes = 1 ZB (zettabyte)           │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: PAGE FAULT ON SWAP ACCESS
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Process accesses VA 0x7FFE5E4ED000, page is swapped out

┌─────────────────────────────────────────────────────────────────────────────┐
│ STEP 1: MMU walks page table                                               │
│   PML4[255] → PDPT[511] → PD[295] → PT[237] = 0x0000001234567802          │
│   P = 0 → Page Fault                                                       │
│                                                                             │
│ STEP 2: Kernel handles page fault                                          │
│   Check if VMA covers address → Yes                                        │
│   Check PTE: Is it a swap entry? (P=0, non-zero value)                     │
│   Extract type = 1, offset = 0x48D159E0                                    │
│                                                                             │
│ STEP 3: Read from swap                                                     │
│   Open swap device 1 (/swap.img)                                           │
│   Seek to offset × 4096 = 0x48D159E0 × 4096 bytes                         │
│   = 0x48D159E0000 bytes from start                                         │
│   Read 4096 bytes into new physical page                                   │
│                                                                             │
│ STEP 4: Update page table                                                  │
│   Allocate new physical page at (e.g.) 0x5678A000                          │
│   Update PTE: 0x5678A000 | 0x67 = 0x00000005678A0067                      │
│   (P=1, R/W=1, U/S=1, A=1, D=1)                                           │
│                                                                             │
│ STEP 5: Resume process                                                     │
│   Return from page fault handler                                           │
│   Retry faulting instruction                                               │
│   Now P=1, MMU can translate successfully                                  │
│                                                                             │
│ TOTAL TIME:                                                                 │
│   Disk read: 5-15 ms (HDD) or 0.05-0.1 ms (SSD)                           │
│   Compare to RAM: 100 ns                                                   │
│   Slowdown: 50,000-150,000× (HDD) or 500-1000× (SSD)                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: SWAP STATISTICS
─────────────────────────────────────────────────────────────────────────────────

FROM YOUR SYSTEM:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Filename        Type    Size        Used        Priority                   │
│ /swap.img       file    4194300     1412936     -2                         │
│                                                                             │
│ Used = 1,412,936 KB = 1,412,936 × 1024 = 1,447,006,208 bytes              │
│      = 1,380 MB = 1.35 GB                                                  │
│                                                                             │
│ Swap slots used = 1,447,006,208 / 4096 = 353,272 slots                    │
│                                                                             │
│ Usage percentage = 1,412,936 / 4,194,300 × 100 = 33.7%                    │
│                                                                             │
│ Free swap = 4,194,300 - 1,412,936 = 2,781,364 KB = 2.65 GB               │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: WHEN DOES SWAPPING HAPPEN?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ TRIGGER: Memory pressure (free pages below low watermark)                  │
│                                                                             │
│ Selection algorithm (simplified):                                           │
│ 1. Scan inactive LRU list                                                  │
│ 2. For each page:                                                           │
│    - If clean file page: Discard (can re-read from file)                  │
│    - If dirty file page: Write back to file, then discard                 │
│    - If anonymous page: Write to swap, then discard                       │
│ 3. Add to free list                                                         │
│                                                                             │
│ Anonymous pages (heap, stack) must go to swap                              │
│ File-backed pages (.text, mmap'd files) can be re-read                    │
│                                                                             │
│ /proc/sys/vm/swappiness controls aggression (default 60)                  │
│   swappiness=0: Avoid swap unless OOM                                      │
│   swappiness=100: Aggressively swap anonymous pages                        │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: 1,048,576 swap slots (2^20), fits easily in 58 bits
Problem 3: Swap-in takes 50,000-150,000× longer than RAM (HDD)
Problem 4: 33.7% swap used, 353,272 slots occupied

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. No swap space + low RAM → OOM killer kills processes
F2. Swap on slow disk → system becomes unusable (thrashing)
F3. Corrupted swap file → data loss / crash on swap-in
F4. si_swapinfo not exported → kernel module won't load
F5. Type field overflow (>31 swap devices) → impossible to address
```
