```
═══════════════════════════════════════════════════════════════════════════════
DEMO 09: 1GB HUGE PAGE WALK - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | CPU flag: pdpe1gb ✓ (1GB pages supported)
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: 1GB VS 2MB VS 4KB COMPARISON
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ PAGE SIZE │ TABLE LEVELS │ RAM READS │ OFFSET BITS │ COVERAGE             │
├───────────┼──────────────┼───────────┼─────────────┼──────────────────────│
│ 4 KB      │ 4            │ 4 reads   │ 12 bits     │ 4,096 bytes          │
│ 2 MB      │ 3            │ 3 reads   │ 21 bits     │ 2,097,152 bytes      │
│ 1 GB      │ 2            │ 2 reads   │ 30 bits     │ 1,073,741,824 bytes  │
└───────────┴──────────────┴───────────┴─────────────┴──────────────────────┘

WALK COMPARISON:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 4KB: CR3 → PML4 → PDPT → PD → PT → Page     (4 levels)                    │
│ 2MB: CR3 → PML4 → PDPT → PD(PS=1) → Page    (3 levels)                    │
│ 1GB: CR3 → PML4 → PDPT(PS=1) → Page         (2 levels!)                   │
│                                                                             │
│ Time savings (assuming 100ns per read):                                    │
│ 4KB: 4 × 100 = 400 ns                                                     │
│ 1GB: 2 × 100 = 200 ns                                                     │
│ Savings: 200 ns per TLB miss (50% faster)                                 │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: DETECT 1GB PAGE AT PDPT LEVEL
─────────────────────────────────────────────────────────────────────────────────

Given: PDPT entry = 0x80000040000001E3

┌─────────────────────────────────────────────────────────────────────────────┐
│ Extract PS bit (bit 7):                                                    │
│ 0x1E3 = 0b111100011                                                        │
│           ↑ bit 7                                                           │
│                                                                             │
│ 0x1E3 = 483 decimal                                                         │
│ 483 >> 7 = 3 (483 / 128 = 3.77...)                                         │
│ 3 & 1 = 1                                                                   │
│ PS = 1 → This is a 1GB huge page!                                          │
│                                                                             │
│ Verify P bit:                                                               │
│ 0x1E3 & 1 = 1 ✓ Present                                                    │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: EXTRACT 1GB PHYSICAL ADDRESS
─────────────────────────────────────────────────────────────────────────────────

Given: PDPT entry = 0x80000040000001E3
Given: VA = 0xFFFF89DF12345678

┌─────────────────────────────────────────────────────────────────────────────┐
│ 1GB mask = 0x000FFFFFC0000000                                              │
│                                                                             │
│ Base physical = entry & mask                                               │
│               = 0x80000040000001E3 & 0x000FFFFFC0000000                    │
│                                                                             │
│ Analysis:                                                                   │
│ High nibble: 0x8 & 0x0 = 0 (NX bit removed)                               │
│ 0x40000000 aligns to 1GB boundary: 0x40000000 = 2^30 = 1GB                │
│ Low bits: 0x01E3 & 0x0 = 0                                                 │
│                                                                             │
│ Base = 0x0000004000000000                                                  │
│        (this is physical address 1GB mark)                                 │
│                                                                             │
│ Offset mask = 0x3FFFFFFF (30 bits)                                         │
│ Offset = VA & 0x3FFFFFFF                                                   │
│        = 0xFFFF89DF12345678 & 0x3FFFFFFF                                  │
│        = 0x12345678 (low 30 bits)                                          │
│                                                                             │
│ Final physical = Base | Offset                                              │
│                = 0x0000004000000000 | 0x12345678                           │
│                = 0x0000004012345678                                         │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: WHEN ARE 1GB PAGES USED?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ COMMON USES:                                                                │
│ 1. Kernel direct map (if aligned and large enough)                         │
│ 2. HugePages for databases (explicitly configured)                         │
│ 3. Very large mmap regions                                                 │
│                                                                             │
│ REQUIREMENTS:                                                               │
│ - CPU must support (pdpe1gb flag in /proc/cpuinfo)                         │
│ - Physical memory must be 1GB aligned                                      │
│ - Contiguous 1GB of physical RAM available                                 │
│                                                                             │
│ YOUR SYSTEM:                                                                │
│ RAM = 15406 MB ≈ 15 GB                                                     │
│ Could have up to 15 × 1GB pages                                            │
│ But fragmentation may reduce this                                          │
│                                                                             │
│ Check: grep pdpe1gb /proc/cpuinfo → YES (your Ryzen has it)               │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: SEARCH FOR 1GB PAGES IN KERNEL
─────────────────────────────────────────────────────────────────────────────────

ALGORITHM:
┌─────────────────────────────────────────────────────────────────────────────┐
│ for each PML4 index 256-511 (kernel space):                                │
│     if PML4[i] & 1 == 0: continue  // not present                         │
│     pdpt_phys = PML4[i] & PTE_ADDR_MASK                                    │
│     pdpt = __va(pdpt_phys)                                                  │
│                                                                             │
│     for each PDPT index 0-511:                                             │
│         if pdpt[j] & 1 == 0: continue  // not present                     │
│         if (pdpt[j] >> 7) & 1 == 1:                                        │
│             // FOUND 1GB PAGE!                                              │
│             phys_base = pdpt[j] & 0x000FFFFFC0000000                       │
│             print("1GB @ PML4[%d] PDPT[%d] phys=0x%lx", i, j, phys_base)  │
│                                                                             │
│ NOTE: 1GB pages at PDPT level are rare                                     │
│ Most systems use 2MB for direct map                                        │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: 1GB saves 2 reads (200ns) vs 4KB
Problem 2: PS = 1, this is 1GB page
Problem 3: Final physical = 0x0000004012345678

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. CPU without pdpe1gb flag → kernel won't use 1GB pages
F2. Using 2MB offset mask (0x1FFFFF) instead of 1GB (0x3FFFFFFF)
F3. Checking PS at wrong level (PD instead of PDPT)
F4. Not finding any 1GB pages → not an error, just not used
F5. Misaligned physical address → hardware error
```
