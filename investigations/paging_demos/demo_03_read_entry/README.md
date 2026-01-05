```
═══════════════════════════════════════════════════════════════════════════════
DEMO 03: READ PAGE TABLE ENTRY - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | page_offset_base = 0xFFFF89DF00000000
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: CALCULATE ENTRY ADDRESS
─────────────────────────────────────────────────────────────────────────────────

Given: PML4 physical base = 0x102FAB000
Given: Index = 255

STEP 1: Calculate byte offset
┌─────────────────────────────────────────────────────────────────────────────┐
│ Each entry = 8 bytes (64 bits)                                              │
│ Offset = index × 8                                                          │
│        = 255 × 8                                                            │
│        = _____ bytes (calculate)                                            │
│                                                                             │
│ Work: 255 × 8 = 2040 = 0x7F8                                               │
│ Verify: 0x7F8 = 7×256 + 15×16 + 8 = 1792 + 240 + 8 = 2040 ✓               │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Calculate entry physical address
┌─────────────────────────────────────────────────────────────────────────────┐
│ Entry_phys = PML4_base + offset                                             │
│            = 0x102FAB000 + 0x7F8                                           │
│            = 0x_______________                                              │
│                                                                             │
│ Work:                                                                       │
│   0x102FAB000                                                               │
│ +       0x7F8                                                               │
│ ─────────────                                                               │
│   0x102FAB7F8                                                               │
│                                                                             │
│ Check: B000 + 7F8 = B7F8 (no carry: 0xB000 + 0x7F8 < 0x10000)              │
│ 0xB000 = 45056, 0x7F8 = 2040, sum = 47096 = 0xB7F8 ✓                       │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 3: Convert to virtual address
┌─────────────────────────────────────────────────────────────────────────────┐
│ Entry_virt = page_offset_base + Entry_phys                                  │
│            = 0xFFFF89DF00000000 + 0x102FAB7F8                              │
│            = 0x_______________                                              │
│                                                                             │
│ Work:                                                                       │
│   0xFFFF89DF00000000                                                        │
│ + 0x0000000102FAB7F8                                                        │
│ ────────────────────                                                        │
│   0xFFFF89E002FAB7F8                                                        │
│                                                                             │
│ Check low 32 bits: 0x00000000 + 0x02FAB7F8 = 0x02FAB7F8                    │
│ Check next 16 bits: 0x89DF + 0x0001 = 0x89E0 (DF + 01 = E0)                │
│ High bits: 0xFFFF unchanged                                                 │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: EDGE CASES
─────────────────────────────────────────────────────────────────────────────────

CASE A: Index = 0 (first entry)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Offset = 0 × 8 = 0                                                          │
│ Entry_phys = 0x102FAB000 + 0 = 0x102FAB000                                 │
│ Entry_virt = 0xFFFF89E002FAB000                                            │
└─────────────────────────────────────────────────────────────────────────────┘

CASE B: Index = 511 (last entry)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Offset = 511 × 8 = _____ (calculate)                                       │
│ Work: 511 × 8 = 4088 = 0xFF8                                               │
│                                                                             │
│ Entry_phys = 0x102FAB000 + 0xFF8 = 0x102FABFF8                             │
│ Check: This is still within the 4KB page (0xFF8 < 0x1000) ✓               │
│                                                                             │
│ Entry_virt = 0xFFFF89E002FABFF8                                            │
└─────────────────────────────────────────────────────────────────────────────┘

CASE C: Index = 512 (INVALID!)
┌─────────────────────────────────────────────────────────────────────────────┐
│ Offset = 512 × 8 = 4096 = 0x1000                                           │
│ Entry_phys = 0x102FAB000 + 0x1000 = 0x102FAC000                            │
│                                                                             │
│ PROBLEM: This is the NEXT page, not PML4!                                  │
│ 0x102FAC000 could be:                                                       │
│   - Random kernel data                                                      │
│   - PDPT table                                                              │
│   - Unmapped                                                                │
│                                                                             │
│ TRAP: Valid indices are 0-511 only (9 bits)                                │
│ Check: if (index > 511) ERROR                                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: DEREFERENCE AND READ
─────────────────────────────────────────────────────────────────────────────────

Given: Entry_virt = 0xFFFF89E002FAB7F8

┌─────────────────────────────────────────────────────────────────────────────┐
│ C code: unsigned long entry = *(unsigned long *)0xFFFF89E002FAB7F8;        │
│                                                                             │
│ What happens:                                                               │
│ 1. CPU fetches 8 bytes from address 0xFFFF89E002FAB7F8                     │
│ 2. This address is in kernel direct map (0xFFFF89...)                      │
│ 3. MMU translates to physical 0x102FAB7F8                                  │
│ 4. 8 bytes read from RAM                                                   │
│ 5. Result placed in register (RAX)                                         │
│                                                                             │
│ If entry = 0x80000001030AB067:                                             │
│   P = 1, R/W = 1, U/S = 1, A = 1, D = 1                                   │
│   Next table phys = 0x1030AB000                                            │
│   NX = 1 (bit 63)                                                          │
│                                                                             │
│ TRAP: If we used physical address directly:                                │
│   *(unsigned long *)0x102FAB7F8  ← WRONG!                                  │
│   CPU interprets this as VIRTUAL address                                   │
│   0x102FAB7F8 is in user space (bit 47 = 0)                               │
│   Probably not mapped → Page Fault                                         │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: HARDER CALCULATION
─────────────────────────────────────────────────────────────────────────────────

Given: Walk path PML4[273] → PDPT[100] → PD[50] → PT[25]
Given: CR3 = 0x3AB7E000

STEP 1: Read PML4[273]
┌─────────────────────────────────────────────────────────────────────────────┐
│ PML4_phys = CR3 & 0x000FFFFFFFFFF000 = 0x3AB7E000                          │
│ Offset = 273 × 8 = _____ (calculate)                                       │
│ Work: 273 × 8 = 2184 = 0x888                                               │
│ Entry addr = 0x3AB7E000 + 0x888 = 0x3AB7E888                               │
│ Virtual = 0xFFFF89DF00000000 + 0x3AB7E888 = 0xFFFF89DF3AB7E888             │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: Read PDPT[100]
┌─────────────────────────────────────────────────────────────────────────────┐
│ Assume PML4[273] = 0x0000000456789067                                      │
│ PDPT_phys = 0x456789067 & 0x000FFFFFFFFFF000 = 0x456789000                 │
│ Offset = 100 × 8 = 800 = 0x320                                             │
│ Entry addr phys = 0x456789000 + 0x320 = 0x456789320                        │
│ Virtual = 0xFFFF89DF00000000 + 0x456789320 = 0xFFFF89DF56789320            │
│                                                                             │
│ Wait, calculation off. Let me redo:                                        │
│ 0xFFFF89DF00000000 + 0x456789320:                                          │
│ Low: 0x00000000 + 0x56789320 = 0x56789320                                  │
│ Hi: 0xFFFF89DF + 0x00000004 = 0xFFFF89E3                                   │
│ Result = 0xFFFF89E356789320                                                │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1 Step 1: 2040 bytes = 0x7F8
Problem 1 Step 2: 0x102FAB7F8
Problem 1 Step 3: 0xFFFF89E002FAB7F8
Problem 2B: 4088 = 0xFF8, Entry = 0x102FABFF8
Problem 4 Step 1: 2184 = 0x888

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgot ×8 → read middle of entry, garbage value
F2. Index > 511 → read outside table, undefined behavior
F3. Used physical directly → page fault (VA vs PA confusion)
F4. Arithmetic error in offset → wrong entry
F5. Read 4 bytes instead of 8 → truncated entry, wrong address
```
