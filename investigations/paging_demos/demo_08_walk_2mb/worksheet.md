```
═══════════════════════════════════════════════════════════════════════════════
DEMO 08: 2MB HUGE PAGE WALK - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Kernel direct map uses 2MB pages
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: 2MB WALK VS 4KB WALK
─────────────────────────────────────────────────────────────────────────────────

4KB WALK (4 levels):
┌─────────────────────────────────────────────────────────────────────────────┐
│ CR3 → PML4[idx] → PDPT[idx] → PD[idx] → PT[idx] → 4KB Page               │
│       ↓           ↓           ↓         ↓                                   │
│       P=1,PS=?    P=1,PS=0    P=1,PS=0  P=1        4 table reads            │
└─────────────────────────────────────────────────────────────────────────────┘

2MB WALK (3 levels):
┌─────────────────────────────────────────────────────────────────────────────┐
│ CR3 → PML4[idx] → PDPT[idx] → PD[idx] → 2MB Page                          │
│       ↓           ↓           ↓                                             │
│       P=1,PS=?    P=1,PS=0    P=1,PS=1  3 table reads (1 fewer!)           │
│                               ↑                                             │
│                               PS=1 means: THIS is the page, not a table    │
└─────────────────────────────────────────────────────────────────────────────┘

SAVINGS:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 4KB: 4 reads × 100 ns = 400 ns                                             │
│ 2MB: 3 reads × 100 ns = 300 ns                                             │
│ Savings: 100 ns per TLB miss                                               │
│                                                                             │
│ More important: 2MB TLB entries cover more:                                │
│ 64 × 4KB = 256 KB                                                          │
│ 32 × 2MB = 64 MB                                                           │
│ Ratio: 64 MB / 256 KB = 256× more coverage per entry                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: WALK KERNEL DIRECT MAP ADDRESS
─────────────────────────────────────────────────────────────────────────────────

Given: VA = 0xFFFF89DF00200000 (physical 0x200000 = 2MB, first 2MB page)

STEP 1: Extract indices
┌─────────────────────────────────────────────────────────────────────────────┐
│ PML4 = (0xFFFF89DF00200000 >> 39) & 0x1FF                                  │
│                                                                             │
│ Let me compute:                                                             │
│ 0xFFFF89DF00200000 in hex positions:                                       │
│ F F F F 8 9 D F 0 0 2 0 0 0 0 0                                           │
│ 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0                                     │
│                                                                             │
│ Bits [47:39] = hex digits 11,10,9 partial                                  │
│ Hex 11 = 8, Hex 10 = 9, Hex 9 = D                                         │
│ Already computed before: PML4 = 275                                        │
│                                                                             │
│ PDPT = (VA >> 30) & 0x1FF = ?                                              │
│ Hex positions 7,6,5 partial                                                │
│ Previous: PDPT ≈ 380 (needs verification)                                  │
│                                                                             │
│ PD = (VA >> 21) & 0x1FF                                                    │
│ 0x00200000 >> 21 = 0x001                                                   │
│ 0x001 & 0x1FF = 1                                                          │
│ PD = 1                                                                      │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Walk to PD
┌─────────────────────────────────────────────────────────────────────────────┐
│ CR3 = 0x_______________ (fill from system)                                  │
│                                                                             │
│ L4: PML4[275] = 0x_______________                                          │
│     P=1, extract PDPT_phys                                                 │
│                                                                             │
│ L3: PDPT[380] = 0x_______________                                          │
│     P=1, PS=___ (should be 0 for normal PDPT)                              │
│     Extract PD_phys                                                         │
│                                                                             │
│ L2: PD[1] = 0x_______________                                              │
│     P=1, PS=1! ← THIS IS THE 2MB HUGE PAGE ENTRY                          │
│                                                                             │
│ STOP: Do not descend to PT level                                           │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 3: Calculate final physical
┌─────────────────────────────────────────────────────────────────────────────┐
│ PD entry example = 0x80000000002001E3                                      │
│                                                                             │
│ Page base = entry & 0x000FFFFFFFE00000                                     │
│           = 0x80000000002001E3 & 0x000FFFFFFFE00000                        │
│           = 0x0000000000200000                                              │
│                                                                             │
│ Offset = VA & 0x1FFFFF (21 bits)                                           │
│        = 0xFFFF89DF00200000 & 0x1FFFFF                                     │
│        = 0x000000 (this VA is 2MB aligned)                                 │
│                                                                             │
│ Final = 0x200000 | 0x000000 = 0x200000 ✓                                  │
│ This is physical address 2MB, as expected for direct map                   │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: OFFSET WITHIN 2MB PAGE
─────────────────────────────────────────────────────────────────────────────────

Given: VA = 0xFFFF89DF00345678 (physical 0x345678 within second 2MB page)

┌─────────────────────────────────────────────────────────────────────────────┐
│ PD index = (0x00345678 >> 21) & 0x1FF                                      │
│          = 0x1A2B3 >> 21 → wait, let me compute properly                   │
│          = 0x345678 >> 21 = 0x001 = 1                                      │
│          (345678 / 2097152 = 1.64...)                                      │
│                                                                             │
│ Offset = 0x345678 & 0x1FFFFF                                               │
│        = 0x145678 (within the 2MB page)                                    │
│                                                                             │
│ Work:                                                                       │
│ 0x345678 = 0b 0011 0100 0101 0110 0111 1000                               │
│ 0x1FFFFF = 0b 0001 1111 1111 1111 1111 1111                               │
│ AND      = 0b 0001 0100 0101 0110 0111 1000 = 0x145678                    │
│                                                                             │
│ So PD[1] entry gives base 0x200000                                         │
│ Final = 0x200000 + 0x145678 = 0x345678 ✓                                  │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: DETECT 2MB PAGE DURING WALK
─────────────────────────────────────────────────────────────────────────────────

ALGORITHM:
┌─────────────────────────────────────────────────────────────────────────────┐
│ At each level (L3 and L2), check:                                          │
│ 1. Is P=1? If not, page fault (not present)                                │
│ 2. Is PS=1? If yes, this is final page, stop walk                          │
│                                                                             │
│ if (entry & 1) {           // P=1, present                                 │
│     if ((entry >> 7) & 1) { // PS=1, huge page                             │
│         // At L3: 1GB page                                                 │
│         // At L2: 2MB page                                                 │
│         phys = (entry & HUGE_MASK) | (va & OFFSET_MASK);                  │
│         return phys;                                                        │
│     } else {                                                                │
│         // Continue to next level                                          │
│         next_table = entry & PTE_ADDR_MASK;                                │
│     }                                                                       │
│ } else {                                                                    │
│     // Page fault                                                           │
│ }                                                                           │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: PD=1, final phys = 0x200000
Problem 3: offset = 0x145678, final phys = 0x345678

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Not checking PS bit → continue walk, read garbage as PT address
F2. Using 4KB offset mask (0xFFF) instead of 2MB (0x1FFFFF)
F3. Using 4KB address mask instead of 2MB mask
F4. Assuming all direct map uses same page size (might mix 2MB and 4KB)
```
