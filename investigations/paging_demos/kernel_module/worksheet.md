```
═══════════════════════════════════════════════════════════════════════════════
PAGEWALK DRIVER: COMPLETE PAGE TABLE WALKER - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | page_offset_base = 0xFFFF89DF00000000
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: COMPLETE 4-LEVEL WALK ALGORITHM
─────────────────────────────────────────────────────────────────────────────────

ALGORITHM (pseudocode with real bits):
┌─────────────────────────────────────────────────────────────────────────────┐
│ INPUT: Virtual Address (VA)                                                │
│ OUTPUT: Physical Address or error                                          │
│                                                                             │
│ 1. Read CR3                                                                │
│    cr3 = read_cr3()                                                        │
│    pml4_phys = cr3 & 0x000FFFFFFFFFF000 // bits [51:12]                   │
│                                                                             │
│ 2. Level 4: PML4                                                           │
│    pml4_idx = (va >> 39) & 0x1FF       // bits [47:39]                    │
│    pml4_virt = __va(pml4_phys)                                             │
│    entry = pml4_virt[pml4_idx]                                             │
│    if !(entry & 1): return NOT_PRESENT                                     │
│    pdpt_phys = entry & 0x000FFFFFFFFFF000                                  │
│                                                                             │
│ 3. Level 3: PDPT                                                           │
│    pdpt_idx = (va >> 30) & 0x1FF       // bits [38:30]                    │
│    pdpt_virt = __va(pdpt_phys)                                             │
│    entry = pdpt_virt[pdpt_idx]                                             │
│    if !(entry & 1): return NOT_PRESENT                                     │
│    if (entry >> 7) & 1:                 // PS bit = 1                     │
│        // 1GB huge page                                                    │
│        phys = (entry & 0x000FFFFFC0000000) | (va & 0x3FFFFFFF)            │
│        return phys                                                         │
│    pd_phys = entry & 0x000FFFFFFFFFF000                                    │
│                                                                             │
│ 4. Level 2: PD                                                             │
│    pd_idx = (va >> 21) & 0x1FF         // bits [29:21]                    │
│    pd_virt = __va(pd_phys)                                                 │
│    entry = pd_virt[pd_idx]                                                 │
│    if !(entry & 1): return NOT_PRESENT                                     │
│    if (entry >> 7) & 1:                 // PS bit = 1                     │
│        // 2MB huge page                                                    │
│        phys = (entry & 0x000FFFFFFFE00000) | (va & 0x1FFFFF)              │
│        return phys                                                         │
│    pt_phys = entry & 0x000FFFFFFFFFF000                                    │
│                                                                             │
│ 5. Level 1: PT                                                             │
│    pt_idx = (va >> 12) & 0x1FF         // bits [20:12]                    │
│    pt_virt = __va(pt_phys)                                                 │
│    entry = pt_virt[pt_idx]                                                 │
│    if !(entry & 1): return NOT_PRESENT                                     │
│    page_phys = entry & 0x000FFFFFFFFFF000                                  │
│                                                                             │
│ 6. Final                                                                   │
│    offset = va & 0xFFF                  // bits [11:0]                    │
│    phys = page_phys | offset                                               │
│    return phys                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: COMPLETE NUMERICAL TRACE
─────────────────────────────────────────────────────────────────────────────────

Given: VA = 0x7FFE5E4ED123 (user stack)
Given: CR3 = 0x0000000305DEF000

┌─────────────────────────────────────────────────────────────────────────────┐
│ STEP 1: Extract indices                                                    │
│   PML4_idx = (0x7FFE5E4ED123 >> 39) & 0x1FF = _____ (calculate)           │
│   PDPT_idx = (0x7FFE5E4ED123 >> 30) & 0x1FF = _____                       │
│   PD_idx   = (0x7FFE5E4ED123 >> 21) & 0x1FF = _____                       │
│   PT_idx   = (0x7FFE5E4ED123 >> 12) & 0x1FF = _____                       │
│   Offset   = 0x7FFE5E4ED123 & 0xFFF = _____                               │
│                                                                             │
│   Work:                                                                     │
│   0x7FFE5E4ED123 >> 39:                                                   │
│   0x7FFE5E4ED123 = 0111 1111 1111 1110 0101 1110 0100 1110 1101 ...       │
│   39 bits from bit 39: bits [47:39]                                        │
│   Hex: 7FFE >> 3 = 0xFFF (but we need 9 bits, not 12)                     │
│   Better: 0x7FFE5E4ED123 / 2^39 = 0xFF (integer)                          │
│   0xFF & 0x1FF = 255                                                       │
│   PML4_idx = 255                                                           │
│                                                                             │
│   PDPT_idx = (0x7FFE5E4ED123 >> 30) & 0x1FF                               │
│   0x7FFE5E4ED123 / 2^30 = 0x1FFF97 (approximately)                        │
│   0x1FFF97 & 0x1FF = 0x197 = 407                                          │
│   Wait, that seems too large. Let me recalculate:                         │
│   0x7FFE5E4ED123 >> 30 = 0x7FFE5E4ED >> 30-bits still in...              │
│                                                                             │
│   Actually easier: 0x7FFE5E4ED123 = 140735064903971                       │
│   140735064903971 >> 30 = 131035 = 0x1FFEB                                │
│   0x1FFEB & 0x1FF = 0x1EB = 491? No wait, 0xEB = 235                      │
│   Hmm, let me use bit positions:                                           │
│   0x7FFE5 = bits [47:28]                                                   │
│   bits [38:30] = 9 bits starting at 30                                    │
│   ...                                                                       │
│                                                                             │
│   Skip detailed calc, use formula in code to verify.                      │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Walk trace (fill in from driver output)
┌─────────────────────────────────────────────────────────────────────────────┐
│ L4: PML4[___] = 0x_________________, P=_, next=0x_________________        │
│ L3: PDPT[___] = 0x_________________, P=_, PS=_, next=0x______________     │
│ L2: PD[___]   = 0x_________________, P=_, PS=_, next=0x______________     │
│ L1: PT[___]   = 0x_________________, P=_                                  │
│ FINAL: 0x_________________ | 0x_____ = 0x_________________                 │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: ERROR CONDITIONS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ ERROR 1: Present bit = 0                                                   │
│   At any level, if P=0, walk stops                                        │
│   Meanings:                                                                 │
│   - Never mapped (entry = 0)                                               │
│   - Swapped out (entry ≠ 0, has swap info)                                │
│   - Lazy allocation (VMA exists but no physical page yet)                 │
│                                                                             │
│ ERROR 2: Invalid VA                                                        │
│   VA not in canonical form (sign extended)                                │
│   User VA: 0x0000000000000000 - 0x00007FFFFFFFFFFF                        │
│   Kernel VA: 0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF                      │
│   "Hole": 0x0000800000000000 - 0xFFFF7FFFFFFFFFFF (#GP on access)         │
│                                                                             │
│ ERROR 3: Permission violation                                              │
│   U/S=0 but ring 3 access → page fault                                    │
│   R/W=0 but write access → page fault                                     │
│   NX=1 but instruction fetch → page fault                                 │
│   (These are handled by CPU, not page walk function)                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: MEMORY READ COUNT
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ 4KB page: 4 table reads (PML4, PDPT, PD, PT)                              │
│ 2MB page: 3 table reads (PML4, PDPT, PD with PS=1)                        │
│ 1GB page: 2 table reads (PML4, PDPT with PS=1)                            │
│                                                                             │
│ Each read: 8 bytes (one 64-bit entry)                                     │
│ Latency: ~100ns DRAM (or less if cached)                                  │
│                                                                             │
│ TOTAL WALK TIME:                                                           │
│ 4KB cold: 4 × 100ns = 400ns                                               │
│ 4KB warm (L3): 4 × 10ns = 40ns                                            │
│ TLB hit: 0-1ns                                                             │
│                                                                             │
│ Speedup from TLB: 400× (cold) to 40× (warm cache)                         │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: PML4=255 for user high addresses (verify others with code)
Problem 4: 4 reads for 4KB, 3 for 2MB, 2 for 1GB

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Not checking P bit before extracting address → garbage pointer
F2. Not checking PS bit → continue walk past huge page
F3. Wrong mask for huge page → lost offset bits
F4. Not using __va() → crash
F5. Reading entries without proper lock → race condition possible
```
