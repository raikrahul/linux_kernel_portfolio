```
═══════════════════════════════════════════════════════════════════════════════
DEMO 04: CHECK PRESENT BIT - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Kernel 6.14.0-37-generic
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: DECODE ALL FLAGS FROM ENTRY
─────────────────────────────────────────────────────────────────────────────────

Given: Entry = 0x80000002FAE00067

STEP 1: Extract low 12 bits (flags)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Flags = Entry & 0xFFF = 0x80000002FAE00067 & 0xFFF = 0x067                 │
│                                                                             │
│ 0x067 in binary:                                                            │
│ 0 = 0000                                                                    │
│ 6 = 0110                                                                    │
│ 7 = 0111                                                                    │
│                                                                             │
│ Full: 0000 0110 0111 = 12 bits                                             │
│       b11  b10 b9 b8 b7 b6 b5 b4 b3 b2 b1 b0                               │
│       0    0   0  0  0  1  1  0  0  1  1  1                                │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Decode each bit
┌─────────────────────────────────────────────────────────────────────────────┐
│ Bit 0 (P - Present):                                                        │
│   0x067 & 0x001 = 0x01 & 0x01 = 1 ← Present ✓                              │
│                                                                             │
│ Bit 1 (R/W - Read/Write):                                                   │
│   (0x067 >> 1) & 1 = 0x33 & 1 = 1 ← Writable ✓                             │
│   Work: 0x067 = 103, 103 >> 1 = 51 = 0x33, 51 & 1 = 1                      │
│                                                                             │
│ Bit 2 (U/S - User/Supervisor):                                              │
│   (0x067 >> 2) & 1 = 0x19 & 1 = 1 ← User accessible ✓                      │
│   Work: 103 >> 2 = 25 = 0x19, 25 & 1 = 1                                   │
│                                                                             │
│ Bit 3 (PWT - Page Write Through):                                           │
│   (0x067 >> 3) & 1 = 0x0C & 1 = 0 ← Write-back caching                     │
│   Work: 103 >> 3 = 12 = 0x0C, 12 & 1 = 0                                   │
│                                                                             │
│ Bit 4 (PCD - Page Cache Disable):                                           │
│   (0x067 >> 4) & 1 = 0x06 & 1 = 0 ← Caching enabled                        │
│   Work: 103 >> 4 = 6, 6 & 1 = 0                                            │
│                                                                             │
│ Bit 5 (A - Accessed):                                                       │
│   (0x067 >> 5) & 1 = 0x03 & 1 = 1 ← Page accessed ✓                        │
│   Work: 103 >> 5 = 3, 3 & 1 = 1                                            │
│                                                                             │
│ Bit 6 (D - Dirty):                                                          │
│   (0x067 >> 6) & 1 = 0x01 & 1 = 1 ← Page written ✓                         │
│   Work: 103 >> 6 = 1, 1 & 1 = 1                                            │
│                                                                             │
│ Bit 7 (PS - Page Size):                                                     │
│   (0x067 >> 7) & 1 = 0x00 & 1 = 0 ← Not huge page                          │
│   Work: 103 >> 7 = 0, 0 & 1 = 0                                            │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 3: Check NX bit (bit 63)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Entry = 0x80000002FAE00067                                                  │
│ Bit 63 = (Entry >> 63) & 1                                                 │
│                                                                             │
│ 0x8... means bit 63 is set (0x8 = 1000 binary, MSB of nibble)              │
│ (0x80000002FAE00067 >> 63) = 1                                             │
│ NX = 1 ← No Execute (data page, not code)                                  │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: NOT-PRESENT ENTRY INTERPRETATION
─────────────────────────────────────────────────────────────────────────────────

Given: Entry = 0x0000000000000000

┌─────────────────────────────────────────────────────────────────────────────┐
│ P = 0x0 & 1 = 0 ← NOT PRESENT                                              │
│                                                                             │
│ When P = 0:                                                                 │
│ - All other bits are UNDEFINED by hardware                                 │
│ - OS can use bits [63:1] for any purpose                                   │
│                                                                             │
│ Common uses:                                                                │
│ 1. All zeros: Never mapped / truly empty                                   │
│ 2. Non-zero: Swap entry (contains swap device + offset)                    │
│                                                                             │
│ TRAP: Checking R/W, U/S, etc. when P=0 is MEANINGLESS                      │
│       The bits might contain garbage or swap metadata                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: SWAP ENTRY FORMAT (when P=0)
─────────────────────────────────────────────────────────────────────────────────

Given: Entry = 0x0000000345678002

┌─────────────────────────────────────────────────────────────────────────────┐
│ P = 0x002 & 1 = 0 ← NOT PRESENT (swapped out)                              │
│                                                                             │
│ Linux swap entry format (x86_64):                                          │
│ Bit 0: Always 0 (not present)                                              │
│ Bits [1:5]: Swap type (index into swap_info array) = 5 bits = 0-31        │
│ Bits [6:63]: Swap offset (position in swap file)                           │
│                                                                             │
│ Extract type:                                                               │
│ type = (Entry >> 1) & 0x1F                                                 │
│      = (0x345678002 >> 1) & 0x1F                                           │
│      = 0x1A2B3C001 & 0x1F                                                  │
│      = 0x01 = 1                                                             │
│ ∴ Swap device #1 (e.g., /swap.img)                                         │
│                                                                             │
│ Extract offset:                                                             │
│ offset = Entry >> 6                                                         │
│        = 0x345678002 >> 6                                                  │
│        = 0xD159E000 (approximately)                                        │
│ Actual: 0x0000000345678002 >> 6 = 0x00000000D159E00                       │
│ ∴ Sector/page offset within swap file                                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: FLAG SUMMARY TABLE
─────────────────────────────────────────────────────────────────────────────────

Fill in for Entry = 0x80000002FAE00067:
┌──────┬────────────────────┬─────────┬─────────┐
│ Bit  │ Name               │ Value   │ Meaning │
├──────┼────────────────────┼─────────┼─────────┤
│ 0    │ P (Present)        │ ___     │         │
│ 1    │ R/W (Read/Write)   │ ___     │         │
│ 2    │ U/S (User/Super)   │ ___     │         │
│ 3    │ PWT                │ ___     │         │
│ 4    │ PCD                │ ___     │         │
│ 5    │ A (Accessed)       │ ___     │         │
│ 6    │ D (Dirty)          │ ___     │         │
│ 7    │ PS (Page Size)     │ ___     │         │
│ 63   │ NX (No Execute)    │ ___     │         │
└──────┴────────────────────┴─────────┴─────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 4 Table:
Bit 0: 1 (Present)
Bit 1: 1 (Writable)
Bit 2: 1 (User accessible)
Bit 3: 0 (Write-back)
Bit 4: 0 (Cached)
Bit 5: 1 (Accessed)
Bit 6: 1 (Dirty)
Bit 7: 0 (Not huge)
Bit 63: 1 (No execute)

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Checking flags when P=0 → meaningless values
F2. Forgetting NX is bit 63, not low bits → missed security check
F3. Interpreting swap entry as normal entry → wrong address
F4. Assuming PS always at bit 7 → wrong at L4/L1
F5. Shift arithmetic error → wrong bit extracted
```
