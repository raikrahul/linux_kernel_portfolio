```
═══════════════════════════════════════════════════════════════════════════════
DEMO 02: EXTRACT PAGE TABLE INDICES - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Phys=44 bits | Virt=48 bits
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: INDEX EXTRACTION FORMULA DERIVATION
─────────────────────────────────────────────────────────────────────────────────
01. Virtual Address = 48 bits total
02. Page Size = 4096 bytes = 2^12 → Offset = 12 bits
03. Remaining = 48 - 12 = 36 bits for indices
04. Page Table has 512 entries = 2^9 → each index = 9 bits
05. 36 / 9 = 4 levels ✓

BIT ASSIGNMENT:
┌─────────────────────────────────────────────────────────────────────────────┐
│ VA Bits:  [47:39] [38:30] [29:21] [20:12] [11:0]                           │
│ Meaning:   PML4    PDPT     PD      PT    Offset                           │
│ Width:     9 bits  9 bits  9 bits  9 bits 12 bits                          │
│ Range:     0-511   0-511   0-511   0-511  0-4095                           │
└─────────────────────────────────────────────────────────────────────────────┘

MASK DERIVATION:
06. 0x1FF = 511 = 2^9 - 1 = binary 1_1111_1111 (9 ones) ✓
07. 0xFFF = 4095 = 2^12 - 1 = binary 1111_1111_1111 (12 ones) ✓

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: EXTRACT INDICES FROM 0x7FFE5E4ED123 (User Stack Address)
─────────────────────────────────────────────────────────────────────────────────

STEP 0: Convert to Binary (required for verification)
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x7FFE5E4ED123                                                              │
│ 7 = 0111, F = 1111, F = 1111, E = 1110                                     │
│ 5 = 0101, E = 1110, 4 = 0100, E = 1110                                     │
│ D = 1101, 1 = 0001, 2 = 0010, 3 = 0011                                     │
│                                                                             │
│ Full: 0111 1111 1111 1110 0101 1110 0100 1110 1101 0001 0010 0011         │
│       ↑bit 47                                              bit 0↑          │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 1: PML4 Index = (VA >> 39) & 0x1FF
┌─────────────────────────────────────────────────────────────────────────────┐
│ Shift right by 39 bits:                                                     │
│ 0x7FFE5E4ED123 >> 39 = ?                                                   │
│                                                                             │
│ 0x7FFE5E4ED123 = 140,730,825,814,307 decimal                               │
│ 2^39 = 549,755,813,888                                                      │
│ 140,730,825,814,307 / 549,755,813,888 = 255.99... → integer = 255          │
│                                                                             │
│ Hex verification: Drop rightmost 39/4 = 9.75 → 10 hex digits               │
│ 0x7FFE5E4ED123 → keep high bits → 0xFF                                     │
│                                                                             │
│ 0xFF & 0x1FF = 0xFF = 255                                                  │
│ PML4_index = _____ (fill in: 255)                                          │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: PDPT Index = (VA >> 30) & 0x1FF
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x7FFE5E4ED123 >> 30 = ?                                                   │
│ 2^30 = 1,073,741,824                                                        │
│ 140,730,825,814,307 / 1,073,741,824 = 131,071.99... → 131071              │
│ 131071 = 0x1FFFF                                                            │
│ 0x1FFFF & 0x1FF = 0x1FF = 511                                              │
│                                                                             │
│ PDPT_index = _____ (fill in: 511)                                          │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 3: PD Index = (VA >> 21) & 0x1FF
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x7FFE5E4ED123 >> 21 = ?                                                   │
│ 2^21 = 2,097,152                                                            │
│ 140,730,825,814,307 / 2,097,152 = 67,108,863.99... → 67108863             │
│ 67108863 = 0x3FFFFFF                                                        │
│ 0x3FFFFFF & 0x1FF = 0x1FF & 0x1FF?                                         │
│ Wait: 0x3FFFFFF = ...1111111 (26 ones)                                     │
│ Low 9 bits = 0x1FF = 511? Let's check.                                     │
│                                                                             │
│ 67108863 mod 512 = 67108863 - (67108863/512)*512                          │
│ 67108863 / 512 = 131071.99... → 131071                                     │
│ 131071 * 512 = 67108352                                                     │
│ 67108863 - 67108352 = 511                                                   │
│                                                                             │
│ PD_index = _____ (fill in: ??? - TRAP! Let me recalculate)                 │
│                                                                             │
│ Actually: 0x7FFE5E4ED123 >> 21                                             │
│ = 0x7FFE5E4ED123 / 2097152                                                 │
│ = 0x3FFF2F27 (hex division)                                                │
│ 0x3FFF2F27 & 0x1FF = 0x127 = 295                                           │
│                                                                             │
│ PD_index = _____ (fill in: 295)                                            │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 4: PT Index = (VA >> 12) & 0x1FF
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x7FFE5E4ED123 >> 12 = 0x7FFE5E4ED                                         │
│ 0x7FFE5E4ED & 0x1FF = 0x0ED = 237                                          │
│                                                                             │
│ Verify: 0xED = 14*16 + 13 = 237 ✓                                          │
│ PT_index = _____ (fill in: 237)                                            │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 5: Offset = VA & 0xFFF
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x7FFE5E4ED123 & 0xFFF = 0x123 = 291                                       │
│                                                                             │
│ Verify: 0x123 = 1*256 + 2*16 + 3 = 256 + 32 + 3 = 291 ✓                   │
│ Offset = _____ (fill in: 291)                                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: EXTRACT INDICES FROM 0xFFFF89DF00000000 (Kernel Direct Map)
─────────────────────────────────────────────────────────────────────────────────

WARNING: KERNEL ADDRESS - BIT 47 IS SET - SIGN EXTENSION MATTERS

STEP 1: PML4 = (0xFFFF89DF00000000 >> 39) & 0x1FF
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0xFFFF89DF00000000 >> 39:                                                   │
│ This is a NEGATIVE number in signed interpretation                         │
│                                                                             │
│ 0xFFFF89DF00000000 = -114706598346883072 (signed 64-bit)                   │
│ Unsigned: 18,446,027,475,362,668,544                                       │
│                                                                             │
│ >> 39 (logical shift in C for unsigned):                                   │
│ 18,446,027,475,362,668,544 / 549,755,813,888 = 33,554,179                  │
│ 33,554,179 = 0x1FFFF13 (hex)                                               │
│ 0x1FFFF13 & 0x1FF = 0x113 = 275                                            │
│                                                                             │
│ Wait, let me verify by hex shifting:                                       │
│ 0xFFFF89DF00000000 >> 39                                                   │
│ 39 bits = 9 hex digits + 3 bits                                            │
│ Shift right by 9 hex digits: FFFF89DF0 → 1FFFF13B                          │
│ Then shift right by 3 more bits: 1FFFF13B >> 3 = 3FFFE27                   │
│ No wait, that's wrong method.                                               │
│                                                                             │
│ Correct: 0xFFFF89DF00000000 = binary (64 bits):                            │
│ 1111...1111 1000 1001 1101 1111 0000... (leading F's = all 1s)            │
│                                                                             │
│ Bits [47:39] for PML4:                                                      │
│ Need to extract 9 bits starting at position 39                             │
│ Shift 39, mask 0x1FF                                                        │
│                                                                             │
│ Simple hex math: 0xFFFF89DF00000000 >> 36 = 0xFFFF89DF                     │
│ >> 3 more = 0x1FFFF13B                                                      │
│ & 0x1FF = 0x13B = 315                                                       │
│                                                                             │
│ Hmm, let me just compute directly:                                         │
│ 0xFFFF89DF00000000 in bits, positions 47-39:                               │
│ F = 15 = 1111, so bits 47-44 = 1111                                        │
│ F = 1111, bits 43-40 = 1111                                                │
│ 8 = 1000, bits 39-36 = 1000                                                │
│ Bits [47:39] = 11111111_1 = wait, need exactly 9 bits                      │
│                                                                             │
│ Bit 47 = 1 (from F)                                                         │
│ Bit 46 = 1 (from F)                                                         │
│ Bit 45 = 1 (from F)                                                         │
│ Bit 44 = 1 (from F)                                                         │
│ Bit 43 = 1 (from F)                                                         │
│ Bit 42 = 1 (from F)                                                         │
│ Bit 41 = 1 (from F)                                                         │
│ Bit 40 = 1 (from F)                                                         │
│ Bit 39 = 1 (from 8 = 1000)                                                  │
│                                                                             │
│ 9 bits = 111111111 = 0x1FF = 511? No wait: 0xFF + one 1 = 256+255=511     │
│ 0b111111111 = 511                                                           │
│                                                                             │
│ Hmm, bit 39 is from 8 = 1000, so bit 39 = 1, bit 38 = 0, etc              │
│ Bits [47:40] = FF = 11111111                                               │
│ Bit 39 = MSB of 8 = 1                                                       │
│ Bits [47:39] = 1_11111111 = 0x1FF = 511? No that's 9 ones.                 │
│                                                                             │
│ Wait: 0x89 = 1000_1001                                                      │
│ Bits 43-40 = 1000 (the 8)                                                   │
│ Bits 39-36 = 1001 (the 9)                                                   │
│                                                                             │
│ So bit 39 = 1 (MSB of 9 = 1001)                                            │
│ Bits [47:40] = 0xFF = 8 bits                                               │
│ Bit 39 = 1                                                                  │
│ Bits [47:39] = 0xFF << 1 | 1 = 0x1FF = 511                                 │
│                                                                             │
│ WAIT. Let me think differently.                                            │
│ 0xFFFF89DF00000000                                                          │
│ Hex digit positions (0 from right):                                        │
│   F  F  F  F  8  9  D  F  0  0  0  0  0  0  0  0                           │
│  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0                           │
│                                                                             │
│ Hex digit 14 = F (bits 59-56)                                              │
│ Hex digit 10 = 9 (bits 43-40)                                              │
│ Hex digit 9  = D (bits 39-36)                                              │
│                                                                             │
│ Bits [47:39] spans hex digits 11 (bits 47-44) and part of 10 (bit 43-40)  │
│   and bit 39 which is in digit 9                                           │
│                                                                             │
│ Hex 11 = 8 = 1000 (bits 47-44)                                             │
│ Hex 10 = 9 = 1001 (bits 43-40)                                             │
│ Hex 9  = D = 1101 (bits 39-36)                                             │
│                                                                             │
│ Bits 47,46,45,44 = 1,0,0,0 (from 8)                                        │
│ Bits 43,42,41,40 = 1,0,0,1 (from 9)                                        │
│ Bit 39 = 1 (MSB of D = 1101)                                               │
│                                                                             │
│ Bits [47:39] = 1,0,0,0,1,0,0,1,1 = 100010011 = 0x113 = 275                 │
│                                                                             │
│ PML4_index = 275                                                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: EDGE CASES
─────────────────────────────────────────────────────────────────────────────────

CASE A: VA = 0x0000000000001000 (smallest valid user page)
┌─────────────────────────────────────────────────────────────────────────────┐
│ PML4 = 0x1000 >> 39 = 0                                                     │
│ PDPT = 0x1000 >> 30 = 0                                                     │
│ PD   = 0x1000 >> 21 = 0                                                     │
│ PT   = 0x1000 >> 12 = 1                                                     │
│ OFF  = 0x1000 & FFF = 0                                                     │
│                                                                             │
│ Path: PML4[0] → PDPT[0] → PD[0] → PT[1] → offset 0                        │
└─────────────────────────────────────────────────────────────────────────────┘

CASE B: VA = 0xFFFFFFFFFFFFFFFF (max kernel address)
┌─────────────────────────────────────────────────────────────────────────────┐
│ All bits = 1                                                                │
│ PML4 = 0x1FF = 511                                                          │
│ PDPT = 0x1FF = 511                                                          │
│ PD   = 0x1FF = 511                                                          │
│ PT   = 0x1FF = 511                                                          │
│ OFF  = 0xFFF = 4095                                                         │
│                                                                             │
│ Path: PML4[511] → PDPT[511] → PD[511] → PT[511] → offset 4095             │
│ This is the last byte of the last page of virtual address space           │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: VA = 0x7FFE5E4ED123
  PML4 = 255, PDPT = 511, PD = 295, PT = 237, OFF = 291

Problem 3: VA = 0xFFFF89DF00000000
  PML4 = 275, PDPT = 382, PD = 0, PT = 0, OFF = 0

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgetting & 0x1FF → garbage high bits included
F2. Using wrong shift amount → index from wrong level
F3. Not handling sign extension for kernel addresses
F4. Decimal/hex confusion in division
F5. Off-by-one in bit position counting
```
