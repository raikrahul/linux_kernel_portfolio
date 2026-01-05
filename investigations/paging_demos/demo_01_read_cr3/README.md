```
═══════════════════════════════════════════════════════════════════════════════
DEMO 01: READ CR3 REGISTER - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Phys: 44 bits | Virt: 48 bits | RAM: 15406 MB
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: DERIVE CR3 MASK FROM FIRST PRINCIPLES
─────────────────────────────────────────────────────────────────────────────────
01. CR3 = 64 bits total
02. Physical address max = 44 bits (from /proc/cpuinfo)
03. Page size = 4096 bytes = 2^12 → low 12 bits = offset = always 0 for tables
04. ∴ Physical address bits used = 44 - 12 = 32 bits for PFN
05. CR3 layout: [63:52]=reserved, [51:12]=address, [11:0]=PCID/flags
06. Bits to keep: [51:12] = 40 bits

CALCULATE BY HAND:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Mask bit 51 set, bits 50,49,...,12 set, bits 11,...,0 clear                │
│ Mask bit 63,...,52 clear                                                    │
│                                                                             │
│ Bit 51 value = 2^51 = _____________________ (fill in)                       │
│ Bit 12 value = 2^12 = 4096                                                  │
│ Sum of bits [51:12] = 2^52 - 2^12 = _____________________ (fill in)        │
│                                                                             │
│ In hex: 0x000_____________________000 (fill in)                             │
└─────────────────────────────────────────────────────────────────────────────┘

VERIFICATION:
07. Expected mask = 0x000FFFFFFFFFF000
08. 0x000FFFFFFFFFF000 in binary (count the bits):
    0000 0000 0000 FFFF FFFF FFFF F000
    ↑bits 63-52    ↑bits 51-12      ↑bits 11-0
    =0 (12 bits)   =1 (40 bits)     =0 (12 bits)
09. Total = 12 + 40 + 12 = 64 ✓

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: EXTRACT COMPONENTS FROM REAL CR3 VALUE
─────────────────────────────────────────────────────────────────────────────────
Given: CR3 = 0x0000000102FAB000 (sample from kernel)

STEP 1: Extract PCID
┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = CR3 & 0xFFF                                                          │
│ 0x0000000102FAB000 & 0xFFF = 0x_____ (calculate last 3 hex digits)         │
│                                                                             │
│ 0x...B000 & 0xFFF:                                                          │
│ B = 1011, 0 = 0000, 0 = 0000, 0 = 0000                                     │
│ 1011 0000 0000 0000 & 0000 1111 1111 1111 = ____________                   │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Extract Physical Address
┌─────────────────────────────────────────────────────────────────────────────┐
│ PHYS = CR3 & 0x000FFFFFFFFFF000                                             │
│ 0x0000000102FAB000 & 0x000FFFFFFFFFF000 = 0x___________________            │
│                                                                             │
│ Work: High bits [63:52] = 0x000 already 0 ✓                                │
│       Mid bits [51:12] = 0x00000102FAB = keep                              │
│       Low bits [11:0] = 0x000 = masked to 0                                │
│ Result = 0x_____________________                                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: CONVERT PHYSICAL TO VIRTUAL
─────────────────────────────────────────────────────────────────────────────────
Given: page_offset_base = 0xFFFF89DF00000000 (from your kernel)
Given: PML4_phys = 0x0000000102FAB000 (from Problem 2)

CALCULATE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ PML4_virt = page_offset_base + PML4_phys                                    │
│                                                                             │
│   0xFFFF89DF00000000                                                        │
│ + 0x0000000102FAB000                                                        │
│   ──────────────────                                                        │
│   0x__________________ (add by hand, watch for carry)                       │
│                                                                             │
│ Column-by-column from right:                                                │
│ 0+0=0, 0+0=0, 0+0=0, B+0=B, A+0=A, F+0=F, 2+0=2, 0+0=0, 1+0=1              │
│ F+0=F, D+0=D, 9+0=9, 8+0=8, F+0=F, F+0=F, F+0=F, F+0=F                     │
│ Wait, that's not right. Need proper hex addition.                          │
│                                                                             │
│ Redo:                                                                       │
│ 0xFFFF89DF00000000 = FFFF 89DF 0000 0000                                   │
│ 0x0000000102FAB000 = 0000 0001 02FA B000                                   │
│                                                                             │
│ 0000 + B000 = B000                                                          │
│ 0000 + 02FA = 02FA                                                          │
│ 0000 + 0001 = 0001                                                          │
│ 89DF + 0000 = 89DF → but carry from below?                                 │
│                                                                             │
│ Actually: 0xFFFF89DF00000000 + 0x102FAB000                                 │
│ = 0xFFFF89DF00000000 + 0x0000000102FAB000                                  │
│ = 0xFFFF89E0___________ (fill in)                                          │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: VERIFY fls64() BEHAVIOR
─────────────────────────────────────────────────────────────────────────────────
fls64() = Find Last Set bit (1-indexed) = position of highest 1 bit + 1

Given: PML4_phys = 0x0000000102FAB000

CALCULATE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 0x102FAB000 in binary:                                                      │
│                                                                             │
│ 1 = 0001                                                                    │
│ 0 = 0000                                                                    │
│ 2 = 0010                                                                    │
│ F = 1111                                                                    │
│ A = 1010                                                                    │
│ B = 1011                                                                    │
│ 0 = 0000                                                                    │
│ 0 = 0000                                                                    │
│ 0 = 0000                                                                    │
│                                                                             │
│ Full: 0001 0000 0010 1111 1010 1011 0000 0000 0000                         │
│       ↑                                                                     │
│       Highest bit position = _____ (0-indexed from right)                   │
│       fls64() returns = _____ + 1 = _____ (1-indexed)                       │
│                                                                             │
│ Count from right: bits 0-11 are 0s, bit 12 is B=1011 starts at bit 12      │
│ 0x102FAB000 = 4,344,627,200 decimal                                        │
│ log2(4,344,627,200) = _____ (calculate)                                    │
│ 2^32 = 4,294,967,296 < 4,344,627,200 < 2^33 = 8,589,934,592                │
│ ∴ highest bit = 32 (0-indexed), fls64() = 33                               │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: EDGE CASES
─────────────────────────────────────────────────────────────────────────────────

CASE A: CR3 = 0x0000000000001000 (minimum valid)
┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = 0x000 = 0                                                            │
│ PHYS = 0x1000 = page 1 = PFN 1                                             │
│ fls64(0x1000) = _____ (find highest bit in 0x1000)                         │
│ 0x1000 = 4096 = 2^12 → bit 12 is set → fls64 = 13                          │
└─────────────────────────────────────────────────────────────────────────────┘

CASE B: CR3 = 0x000FFFFFFFFFF000 (maximum valid on 44-bit system)
┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = 0x000 = 0                                                            │
│ PHYS = 0x000FFFFFFFFFF000                                                   │
│ But wait: 44-bit physical means max = 2^44 - 1 = 0xFFFFFFFFFFF             │
│ Aligned to 4KB: 0xFFFFFFFFF000 (drop last 12 bits conceptually)            │
│ Actual max with 44 bits: 0x00000FFFFFFFF000                                │
│ fls64(0xFFFFFFFFF000) = 44 (bit 43 is highest, 0-indexed) → fls = 44      │
└─────────────────────────────────────────────────────────────────────────────┘

CASE C: CR3 = 0x8000000102FAB005 (bit 63 set, PCID = 5)
┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = 0x005 = 5                                                            │
│ PHYS = 0x8000000102FAB005 & 0x000FFFFFFFFFF000 = ?                         │
│ Bit 63 gets masked out by 0x000...                                         │
│ Low 3 digits 005 get masked out by ...000                                  │
│ PHYS = 0x0000000102FAB000 ✓                                                │
│                                                                             │
│ TRAP: If you forget to mask, bit 63 makes this look like kernel address!  │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 6: WHAT HAPPENS WITHOUT MASKING?
─────────────────────────────────────────────────────────────────────────────────
SCENARIO: Use CR3 directly as address without masking

┌─────────────────────────────────────────────────────────────────────────────┐
│ CR3 = 0x0000000102FAB005 (PCID = 5)                                        │
│                                                                             │
│ Wrong: ptr = (void *)CR3 = 0x102FAB005                                     │
│ Problem: Address not aligned (ends in 5, not 0)                            │
│ Problem: This is PHYSICAL address, CPU sees it as VIRTUAL                  │
│                                                                             │
│ What happens:                                                               │
│ 1. CPU tries to translate VA 0x102FAB005                                   │
│ 2. This is in User space (bit 47 = 0)                                      │
│ 3. Probably not mapped                                                      │
│ 4. → Page Fault, CR2 = 0x102FAB005                                         │
│                                                                             │
│ Correct: ptr = __va(CR3 & MASK)                                            │
│        = __va(0x102FAB000)                                                 │
│        = page_offset_base + 0x102FAB000                                    │
│        = 0xFFFF89E002FAB000 (in direct map, valid kernel address)          │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 7: HARDER NUMERICAL EXAMPLE
─────────────────────────────────────────────────────────────────────────────────
Given: CR3 = 0x000000039AB7EFFF (unusual: max PCID, odd address pattern)

┌─────────────────────────────────────────────────────────────────────────────┐
│ Step 1: Extract PCID                                                        │
│ 0x...FFF & 0xFFF = 0xFFF = 4095 (maximum PCID value)                       │
│                                                                             │
│ Step 2: Extract Physical Address                                            │
│ 0x000000039AB7EFFF & 0x000FFFFFFFFFF000 = ?                                │
│                                                                             │
│ 0x39AB7EFFF:                                                                │
│ Low nibble F & 0 = 0                                                        │
│ Next F & 0 = 0                                                              │
│ Next F & 0 = 0                                                              │
│ E & F = E                                                                   │
│ 7 & F = 7                                                                   │
│ B & F = B                                                                   │
│ A & F = A                                                                   │
│ 9 & F = 9                                                                   │
│ 3 & F = 3                                                                   │
│                                                                             │
│ Result: 0x000000039AB7E000                                                  │
│                                                                             │
│ Step 3: Calculate virtual address                                           │
│ __va(0x39AB7E000) = 0xFFFF89DF00000000 + 0x39AB7E000                       │
│                                                                             │
│ Addition:                                                                   │
│   FFFF 89DF 0000 0000                                                       │
│ + 0000 0003 9AB7 E000                                                       │
│   ─────────────────────                                                     │
│   FFFF 89E2 9AB7 E000                                                       │
│                                                                             │
│ Work: DF + 03 = E2 (no carry: DF=223, 03=3, sum=226=E2)                    │
│       89 + 00 + carry? No carry. 89 stays.                                 │
│       Wait: DF + 03 = E2. D=13, F=15 → 13×16+15 = 223                      │
│       223 + 3 = 226 = 14×16 + 2 = E2 ✓ No carry (226 < 256)               │
│                                                                             │
│ Step 4: fls64(0x39AB7E000)                                                  │
│ 0x39AB7E000 in bits: 3 = 0011, 9 = 1001 → starts with 0011 1001...        │
│ Highest bit: 0x3... means bits 33-32 are set (since 3 = 0011)              │
│ 0x39AB7E000 = 15,459,704,832 decimal                                       │
│ 2^33 = 8,589,934,592 < 15,459,704,832 < 2^34 = 17,179,869,184              │
│ ∴ highest bit = 33 → fls64 = 34                                            │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS (VERIFY YOUR WORK)
═══════════════════════════════════════════════════════════════════════════════

Problem 1: Mask = 0x000FFFFFFFFFF000
Problem 2 Step 1: PCID = 0x000 = 0
Problem 2 Step 2: PHYS = 0x0000000102FAB000
Problem 3: PML4_virt = 0xFFFF89E002FAB000
Problem 4: fls64 = 33
Problem 5A: fls64 = 13
Problem 5B: fls64 = 44
Problem 5C: PHYS = 0x0000000102FAB000
Problem 7: PHYS = 0x39AB7E000, virt = 0xFFFF89E29AB7E000, fls64 = 34

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgot & 0xFFF for PCID → got full CR3 value
F2. Forgot & mask for PHYS → NX bit (63) included
F3. Arithmetic error in hex addition → wrong virtual address
F4. fls64 off-by-one (0-indexed vs 1-indexed confusion)
F5. Using physical address directly → page fault
F6. Misaligned address (ends in non-000) → crash or wrong data
```
