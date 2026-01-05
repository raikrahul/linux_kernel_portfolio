# BITMASKING DRILLS - NUMERICAL ONLY

## PATTERN 0: N-BIT MASK DERIVATION (MEMORIZE BY DOING)

```
RULE: "Keep N rightmost bits" → Mask = N÷4 = Q remainder R → prefix = 2^R - 1 → mask = (prefix)(Q Fs)

LEFTOVER TABLE:
  R=0 → prefix = nothing (2⁰-1=0)
  R=1 → prefix = 1       (2¹-1=1)
  R=2 → prefix = 3       (2²-1=3)
  R=3 → prefix = 7       (2³-1=7)

EXAMPLES:
  N=8:  8÷4=2r0  → prefix=nothing, Fs=2 → 0xFF        (bits [7:0])
  N=9:  9÷4=2r1  → prefix=1, Fs=2       → 0x1FF       (bits [8:0])
  N=10: 10÷4=2r2 → prefix=3, Fs=2       → 0x3FF       (bits [9:0])
  N=11: 11÷4=2r3 → prefix=7, Fs=2       → 0x7FF       (bits [10:0])
  N=12: 12÷4=3r0 → prefix=nothing, Fs=3 → 0xFFF       (bits [11:0])
  N=17: 17÷4=4r1 → prefix=1, Fs=4       → 0x1FFFF     (bits [16:0])
  N=21: 21÷4=5r1 → prefix=1, Fs=5       → 0x1FFFFF    (bits [20:0])
  N=47: 47÷4=11r3→ prefix=7, Fs=11      → 0x7FFFFFFFFFFF (bits [46:0])

USAGE:
  RIGHTMOST bits: value & mask           → 0xABCD1234 & 0x1FFFF = 0x11234
  MIDDLE bits:    (value >> shift) & mask → (0xABCD1234 >> 20) & 0x1FF = extracts bits [28:20]
```

---

## DRILL 1: HEX → BINARY (NIBBLE PATTERN)

```
INPUT: 0xC
```
01. 0xC = 12₁₀ → 12 = 8+4+0+0 → 1100₂ ✓
02. Pattern: C=1100, D=1101, E=1110, F=1111 (8+4+2+1 combinations)

```
INPUT: 0x7F
```
03. 0x7 = 0111₂, 0xF = 1111₂ → 0x7F = 0111_1111₂ = 127₁₀ ✓
04. Verify: 64+32+16+8+4+2+1 = 127 ✓

```
INPUT: 0x1FF
```
05. 0x1 = 0001₂, 0xFF = 1111_1111₂ → 0x1FF = 0_0001_1111_1111₂ = 511₁₀ ✓
06. Verify: 256+128+64+32+16+8+4+2+1 = 511 ✓
07. ∴ 0x1FF = 9 bits all set to 1 = 2⁹-1 = 511 ✓

```
+-------+-------+-------+-------+
| Hex   | Binary| Decimal| Bits  |
+-------+-------+-------+-------+
| 0x0   | 0000  |   0   |  0    |
| 0x1   | 0001  |   1   |  1    |
| 0x7   | 0111  |   7   |  3    |
| 0xF   | 1111  |  15   |  4    |
| 0xFF  |11111111| 255   |  8    |
| 0x1FF |1_1111_1111| 511 |  9    |
| 0xFFF |1111_1111_1111|4095| 12   |
+-------+-------+-------+-------+
```

---

## DRILL 2: RIGHT SHIFT (>> N) = DIVIDE BY 2^N

```
INPUT: 0x123456789ABC >> 12
```
08. >> 12 bits = >> 3 hex digits (12/4=3)
09. Drop rightmost 3 hex digits: ABC
10. Result: 0x123456789 ✓

```
INPUT: 0xFFFF_FFFF_FFFF >> 39
STEP-BY-STEP:
```
11. 48-bit input: FFFF_FFFF_FFFF₁₆
12. >> 39 = drop 39 bits from right
13. 39 bits = 9 hex digits + 3 bits
14. Binary: 1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111_1111 (48 ones)
15. Keep top (48-39)=9 bits: 1_1111_1111₂ = 0x1FF = 511₁₀ ✓

```
INPUT: 0x7FFF_8000_0000 >> 39
```
16. Binary of 0x7FFF_8000_0000:
    - 0x7 = 0111, 0xF = 1111 (×3), 0x8 = 1000, 0x0 = 0000 (×7)
    - = 0111_1111_1111_1111_1000_0000_0000_0000_0000_0000_0000_0000
17. Total bits shown = 48 ✓
18. >> 39 → keep top 9 bits: 0_1111_1111₂ = 0xFF = 255₁₀ ✓

---

## DRILL 3: BITWISE AND (&) = MODULUS FOR 2^N

```
INPUT: 0x123456789ABC & 0xFFF
```
19. & 0xFFF = keep only bottom 12 bits
20. 0xABC & 0xFFF = 0xABC ✓
21. Verify: 0xABC = 10×256 + 11×16 + 12 = 2560+176+12 = 2748₁₀
22. 2748 < 4096 ✓ (fits in 12 bits)

```
INPUT: 0x7FFF_8000_1234 & 0x1FF
```
23. & 0x1FF = keep bottom 9 bits
24. 0x1234 in binary: 0001_0010_0011_0100
25. Bottom 9 bits: 0_0011_0100₂ = 0x34 = 52₁₀ ✓
26. Verify: 0x1234 = 4660₁₀, 4660 mod 512 = 4660 - 9×512 = 4660-4608 = 52 ✓

---

## DRILL 4: INDEX EXTRACTION (KERNEL PAGE TABLE)

```
VIRTUAL ADDRESS: 0x7FFF_FFFF_F000
48-BIT LAYOUT: [PML4:9][PDPT:9][PD:9][PT:9][OFFSET:12]
BIT POSITIONS: [47:39][38:30][29:21][20:12][11:0]
```

### STEP 1: OFFSET (bits 11:0)
27. 0x7FFF_FFFF_F000 & 0xFFF = 0x000 ✓
28. Offset = 0 bytes into page

### STEP 2: PT INDEX (bits 20:12)
29. (0x7FFF_FFFF_F000 >> 12) & 0x1FF
30. >> 12: 0x7FFF_FFFF_F000 → 0x7FFF_FFFF_F
31. & 0x1FF: 0x...F & 0x1FF
32. 0xF = 15₁₀, 15 & 511 = 15 ✓
33. PT Index = 15

### STEP 3: PD INDEX (bits 29:21)
34. (0x7FFF_FFFF_F000 >> 21) & 0x1FF
35. >> 21 = >> 20 >> 1 = drop 5 hex digits + 1 bit
36. 0x7FFF_FFFF_F000 >> 20 = 0x7FFF_F
37. 0x7FFF_F >> 1 = 0x3FFFF (shift right 1 more bit)
38. Wait. Let me recalculate properly:
39. 0x7FFF_FFFF_F000 = 0111_1111_1111_1111_1111_1111_1111_1111_1111_0000_0000_0000
40. Bit positions 47→0 (48 bits total)
41. >> 21: drop bits 20→0, keep bits 47→21
42. Result bits: 0111_1111_1111_1111_1111_1111_111 (27 bits)
43. & 0x1FF: keep bottom 9 bits: 1_1111_1111 = 0x1FF = 511 ✓
44. PD Index = 511

### STEP 4: PDPT INDEX (bits 38:30)
45. (0x7FFF_FFFF_F000 >> 30) & 0x1FF
46. >> 30: keep bits 47→30 (18 bits)
47. Original top 18 bits: 0111_1111_1111_1111_11 = 0x1FFFF
48. & 0x1FF: 0x1FFFF & 0x1FF = 0x1FF = 511 ✓
49. PDPT Index = 511

### STEP 5: PML4 INDEX (bits 47:39)
50. (0x7FFF_FFFF_F000 >> 39) & 0x1FF
51. >> 39: keep bits 47→39 (9 bits)
52. Top 9 bits of 0x7FFF...: 0_1111_1111 = 0xFF = 255 ✓
53. PML4 Index = 255

```
+----------+--------+----------+
| Level    | Shift  | Index    |
+----------+--------+----------+
| PML4     | >> 39  | 255      |
| PDPT     | >> 30  | 511      |
| PD       | >> 21  | 511      |
| PT       | >> 12  | 15       |
| OFFSET   | & FFF  | 0        |
+----------+--------+----------+
```

---

## DRILL 5: PAGE TABLE ENTRY → PHYSICAL ADDRESS

```
PTE VALUE: 0x800000001234_5067
```
54. Bit 0 (Present): 0x...7 & 1 = 0111 & 0001 = 1 ✓ (Present)
55. Bit 1 (R/W): (0x...7 >> 1) & 1 = 0011 & 1 = 1 ✓ (Writable)
56. Bit 2 (U/S): (0x...7 >> 2) & 1 = 0001 & 1 = 1 ✓ (User)
57. Bits 12-51 (Physical Frame): (0x...5067 >> 12) << 12
58. 0x800000001234_5067 & 0x000F_FFFF_FFFF_F000 = 0x0000_0000_1234_5000

```
PHYSICAL ADDRESS CALCULATION:
```
59. Physical Frame Base = 0x1234_5000
60. + Offset from virtual = 0x000
61. Physical Address = 0x1234_5000 + 0x000 = 0x1234_5000 ✓

---

## DRILL 6: ENTRY SIZE × INDEX = TABLE OFFSET

```
BASE TABLE ADDRESS: 0x1000
INDEX: 255
ENTRY SIZE: 8 bytes
```
62. Table Offset = Index × Entry_Size = 255 × 8 = 2040 bytes = 0x7F8
63. Entry Address = Base + Offset = 0x1000 + 0x7F8 = 0x17F8 ✓

```
BASE: 0x5000, INDEX: 511
```
64. Offset = 511 × 8 = 4088 = 0xFF8
65. Entry Address = 0x5000 + 0xFF8 = 0x5FF8 ✓
66. Verify: 0x5FF8 < 0x6000 ✓ (still within 4KB page)

---

## DRILL 7: FULL 4-LEVEL WALK (NUMERICAL TRACE)

```
INPUT:
  CR3 = 0x0010_0000 (Physical address of PML4 table)
  VADDR = 0x7FFF_FFFF_F000

INDICES (from Drill 4):
  PML4=255, PDPT=511, PD=511, PT=15, OFFSET=0

TABLE CONTENTS (GIVEN):
  PML4[255] at 0x0010_0000 + 255×8 = 0x0010_07F8 → value = 0x0020_0003
  PDPT[511] at 0x0020_0000 + 511×8 = 0x0020_0FF8 → value = 0x0030_0003
  PD[511]   at 0x0030_0000 + 511×8 = 0x0030_0FF8 → value = 0x0040_0003
  PT[15]    at 0x0040_0000 + 15×8  = 0x0040_0078 → value = 0x1234_5003
```

### WALK TRACE:
67. Read PML4[255]: Address = 0x0010_0000 + 0x7F8 = 0x0010_07F8
68. PML4 Entry = 0x0020_0003. Present? 0x03 & 1 = 1 ✓
69. PDPT Base = 0x0020_0003 & 0xFFFF_FFFF_F000 = 0x0020_0000

70. Read PDPT[511]: Address = 0x0020_0000 + 0xFF8 = 0x0020_0FF8
71. PDPT Entry = 0x0030_0003. Present? 1 ✓
72. PD Base = 0x0030_0000

73. Read PD[511]: Address = 0x0030_0000 + 0xFF8 = 0x0030_0FF8
74. PD Entry = 0x0040_0003. Present? 1 ✓ Huge? (bit 7) = 0 ✗ (not 2MB page)
75. PT Base = 0x0040_0000

76. Read PT[15]: Address = 0x0040_0000 + 0x78 = 0x0040_0078
77. PT Entry = 0x1234_5003. Present? 1 ✓
78. Frame Base = 0x1234_5003 & 0xFFFF_FFFF_F000 = 0x1234_5000

79. Physical Address = Frame Base + Offset = 0x1234_5000 + 0x000 = 0x1234_5000 ✓

```
+-----+-------+-----------+-------------+--------------+
|Level|Index  |Table Base |Entry Addr   |Entry Value   |
+-----+-------+-----------+-------------+--------------+
|PML4 | 255   |0x0010_0000|0x0010_07F8  |0x0020_0003   |
|PDPT | 511   |0x0020_0000|0x0020_0FF8  |0x0030_0003   |
|PD   | 511   |0x0030_0000|0x0030_0FF8  |0x0040_0003   |
|PT   | 15    |0x0040_0000|0x0040_0078  |0x1234_5003   |
+-----+-------+-----------+-------------+--------------+
RESULT: 0x7FFF_FFFF_F000 → 0x1234_5000
```

---

## EXERCISES (DO BY HAND)

### EX1: Extract PML4 index from 0xFFFF_8000_0000_0000
80. (0xFFFF_8000_0000_0000 >> 39) & 0x1FF = ?
81. Hint: Top 16 bits = 0xFFFF = sign extension (canonical)
82. Bits 47:39 = ?

### EX2: Calculate entry address
83. Base = 0x7000, Index = 256, Entry Size = 8
84. Entry Address = ?

### EX3: Is page present?
85. PTE = 0x8000_0000_1234_5006
86. Bit 0 = ?

---

## FAILURE PREDICTIONS

F1. >> 39 confused with / 39 → 0x1FF ≠ 0x1FF/39 ✗
F2. Index × 8 forgotten → Entry at wrong address ✗
F3. & 0x1FF confused with & 0xFF → 8-bit vs 9-bit extraction ✗
F4. Physical frame mask wrong → High bits pollute address ✗
F5. Little-endian read → 0x03500412 misread as 0x12045003 ✗
