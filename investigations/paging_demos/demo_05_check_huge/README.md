```
═══════════════════════════════════════════════════════════════════════════════
DEMO 05: CHECK HUGE PAGE BIT - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | CPU flag: pdpe1gb ✓ (supports 1GB pages)
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: PS BIT EXTRACTION
─────────────────────────────────────────────────────────────────────────────────
Given: Entry = 0x80000002FAE001A1

STEP 1: Extract PS bit (bit 7)
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x1A1 = last 3 hex digits = flags                                          │
│ 0x1A1 in binary:                                                            │
│   1 = 0001                                                                  │
│   A = 1010                                                                  │
│   1 = 0001                                                                  │
│ = 0001 1010 0001 = 0x1A1                                                   │
│                                                                             │
│ Bit positions (right to left): 8 7 6 5 4 3 2 1 0                           │
│                                0 0 0 1 1 0 1 0 0 0 0 1                     │
│                                        ↑                                    │
│                                        bit 7                                │
│                                                                             │
│ Let's count properly for 0x1A1:                                            │
│ 0x1A1 = 417 decimal                                                         │
│ 417 / 128 = 3.25... → bit 7 contributes at least once                      │
│ 128 = 2^7 → if bit 7 set, value >= 128                                     │
│ 417 >= 128 → bit 7 COULD be set, need more work                            │
│                                                                             │
│ 417 in binary: 417 = 256 + 161 = 256 + 128 + 33 = 256 + 128 + 32 + 1       │
│ = 2^8 + 2^7 + 2^5 + 2^0                                                     │
│ = 1_1010_0001 = 0x1A1 ✓                                                    │
│ Bit 7 = 1 ✓                                                                │
│                                                                             │
│ Alternative: (0x1A1 >> 7) & 1 = ?                                          │
│ 0x1A1 >> 7 = 417 >> 7 = 417 / 128 = 3 (integer)                            │
│ 3 & 1 = 1 ✓                                                                │
│                                                                             │
│ PS = _____ (fill in: 1)                                                    │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: What does PS=1 mean?
┌─────────────────────────────────────────────────────────────────────────────┐
│ At L3 (PDPT): PS=1 → 1GB huge page                                         │
│ At L2 (PD):   PS=1 → 2MB huge page                                         │
│ At L4, L1:    PS bit is reserved, not checked                              │
│                                                                             │
│ If this entry is at L2 and PS=1:                                           │
│   → This is a 2MB huge page descriptor                                     │
│   → Do NOT descend to PT                                                   │
│   → Physical base = entry & 0x000FFFFFFFE00000                             │
│   → Offset = VA & 0x1FFFFF (21 bits)                                       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: PHYSICAL ADDRESS EXTRACTION FOR DIFFERENT PAGE SIZES
─────────────────────────────────────────────────────────────────────────────────

Given: Entry = 0x80000002FAE001A1

CASE A: 4KB Page (PS=0, at L1)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Mask = 0x000FFFFFFFFFF000                                                   │
│ 0x80000002FAE001A1 & 0x000FFFFFFFFFF000 = ?                                │
│                                                                             │
│ Bit-by-bit:                                                                 │
│ 0x8... high nibble → masked by 0x0 → 0                                     │
│ 0x000... middle → kept                                                      │
│ 0x...1A1 low 3 hex → 0x...000                                              │
│                                                                             │
│ Result = 0x00000002FAE00000                                                │
│ (But PS=1 here, so this is wrong interpretation)                           │
└─────────────────────────────────────────────────────────────────────────────┘

CASE B: 2MB Huge Page (PS=1, at L2)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Mask = 0x000FFFFFFFE00000                                                   │
│ Zero bits [20:0] = 21 bits                                                 │
│                                                                             │
│ 0x80000002FAE001A1 & 0x000FFFFFFFE00000 = ?                                │
│                                                                             │
│ 0x2FAE001A1 in relevant portion:                                           │
│ 0x2FAE00000 (already 2MB aligned in this example)                          │
│ Check: 0xE00000 = 14680064. 0xE00000 / 0x200000 = 7. Aligned ✓            │
│                                                                             │
│ But entry has 1A1 in low bits. After mask:                                 │
│ 0x2FAE001A1 & 0xFFE00000 = 0x2FAE00000                                     │
│                                                                             │
│ Wait, need full mask: 0x000FFFFFFFE00000                                   │
│ 0x80000002FAE001A1 & 0x000FFFFFFFE00000:                                   │
│   High: 8 masked to 0                                                       │
│   Mid: 2FAE00 stays                                                         │
│   Low: 01A1 & E00000 = ...? Let me redo.                                   │
│                                                                             │
│ Hex alignment:                                                              │
│   0x80000002FAE001A1                                                        │
│ & 0x000FFFFFFFE00000                                                        │
│ = 0x00000002FAE00000                                                        │
│                                                                             │
│ Verify low bits: 0x...001A1 & 0x...00000 = 0                               │
│ Result = 0x00000002FAE00000                                                │
└─────────────────────────────────────────────────────────────────────────────┘

CASE C: 1GB Huge Page (PS=1, at L3)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Mask = 0x000FFFFFC0000000                                                   │
│ Zero bits [29:0] = 30 bits                                                 │
│                                                                             │
│ 0x80000002FAE001A1 & 0x000FFFFFC0000000 = ?                                │
│                                                                             │
│ 0x2FAE001A1:                                                                │
│ 0x2FAE00000 / 0x40000000 = 0x2FAE00000 / 1073741824                        │
│ = 12888072192 / 1073741824 = 12.00...                                       │
│ Hmm, 0x2FAE00000 is larger than 0x40000000                                 │
│                                                                             │
│ Let's just compute the mask:                                               │
│ 0x80000002FAE001A1 & 0x000FFFFFC0000000                                    │
│ Look at bits [51:30] only                                                  │
│                                                                             │
│ 0x2FAE001A1: bits 31-30 = 0x2F >> 6 = no, let me think hex positions       │
│ 0x2FAE001A1 = 10 hex digits = 40 bits                                      │
│ Bit 30 is at hex position 7 (counting from 0)                              │
│ 0x40000000 = bit 30 set only                                               │
│                                                                             │
│ 0x2FAE001A1 & 0xFFFFFFC0000000:                                            │
│ 0x2FAE001A1 = 0x0_0000_0002_FAE0_01A1 (align to 16 digits)                │
│ bits [29:0] zeroed = 0x0_0000_0002_C000_0000? No...                        │
│                                                                             │
│ Easier: 0x00000002FAE001A1 & 0x000FFFFFC0000000                            │
│ 0x2FAE001A1 >> 30 = 2 (since 2FAE001A1 / 2^30 ≈ 2.8)                       │
│ 2 << 30 = 0x80000000                                                        │
│ Result = 0x0000000280000000                                                │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: FINAL PHYSICAL ADDRESS CALCULATION
─────────────────────────────────────────────────────────────────────────────────

Given: Entry = 0x80000002FAE001A1 (PS=1 at L2)
Given: VA = 0xFFFF89DF12345678

STEP 1: Extract 2MB offset from VA
┌─────────────────────────────────────────────────────────────────────────────┐
│ Offset = VA & 0x1FFFFF                                                      │
│ 0xFFFF89DF12345678 & 0x1FFFFF = ?                                          │
│                                                                             │
│ 0x1FFFFF = 2097151 = 2^21 - 1 = 21 ones in binary                          │
│ Last 6 hex digits of VA: 345678                                            │
│ 0x345678 & 0x1FFFFF:                                                        │
│ 0x345678 = 3,430,008 decimal                                               │
│ 0x1FFFFF = 2,097,151 decimal                                               │
│ 3,430,008 > 2,097,151 → need proper masking                                │
│                                                                             │
│ 0x345678 in binary: 0011 0100 0101 0110 0111 1000                          │
│ 0x1FFFFF in binary: 0001 1111 1111 1111 1111 1111                          │
│ AND result:         0001 0100 0101 0110 0111 1000 = 0x145678               │
│                                                                             │
│ Offset = 0x145678 = 1,332,856 bytes                                        │
│ Verify: 0x145678 < 0x200000 (2MB)? 1,332,856 < 2,097,152 ✓                 │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Calculate final physical
┌─────────────────────────────────────────────────────────────────────────────┐
│ Page_base = entry & 0x000FFFFFFFE00000 = 0x00000002FAE00000                │
│ Offset = 0x145678                                                           │
│                                                                             │
│ Final = Page_base | Offset                                                  │
│       = 0x00000002FAE00000 | 0x145678                                      │
│       = 0x00000002FAF45678                                                 │
│                                                                             │
│ Work:  FAE00000                                                             │
│     |     145678                                                            │
│     = FAF45678                                                              │
│                                                                             │
│ Check: E + 1 = F in hex position 5 from right                              │
│ FAE00000 + 145678:                                                          │
│ Actually OR is correct since they don't overlap:                           │
│ FAE00000 has 00000 in low 5 hex digits                                     │
│ 145678 has 0 in high positions                                             │
│ OR = FAF45678 ✓                                                            │
│                                                                             │
│ FINAL PHYSICAL = 0x00000002FAF45678                                        │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: WHICH LEVEL MATTERS?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ PS bit meaning by level:                                                    │
│                                                                             │
│ L4 (PML4): PS bit = RESERVED. Must be 0. If 1 → undefined behavior.        │
│ L3 (PDPT): PS bit = 1 → 1GB page. Coverage = 2^30 = 1,073,741,824 bytes.   │
│ L2 (PD):   PS bit = 1 → 2MB page. Coverage = 2^21 = 2,097,152 bytes.       │
│ L1 (PT):   PS bit = RESERVED/PAT. Not huge page indicator.                 │
│                                                                             │
│ TRAP: Checking PS at L4 or L1 is WRONG.                                    │
│ TRAP: Must check PRESENT (P) bit BEFORE checking PS bit.                   │
│       If P=0, PS is meaningless (entry might store swap info).             │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: PS = 1
Problem 2A: 4KB address = 0x00000002FAE00000
Problem 2B: 2MB address = 0x00000002FAE00000
Problem 2C: 1GB address = 0x0000000280000000
Problem 3: Final = 0x00000002FAF45678

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Checking PS at wrong level (L4 or L1) → misinterpretation
F2. Not checking P bit first → random PS value if not present
F3. Using wrong offset mask (0xFFF instead of 0x1FFFFF) → lost bits
F4. Using wrong address mask (4KB mask for 2MB page) → wrong base
F5. Forgetting OR vs ADD (they're same here but conceptually different)
```
