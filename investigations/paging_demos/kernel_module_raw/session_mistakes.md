# SESSION MISTAKES - 2025-12-25

## MISTAKE 1: EXPONENT VS VALUE
```
WROTE:  PAGE_SIZE_RAW = 12
SHOULD: PAGE_SIZE_RAW = 4096

WHY WRONG: Confused 2^12 with 12. Brain stored "12" but question asked "4096".
ORTHOGONAL: If asked "how many entries?", would you write 9 or 512?
FIX: Read question: "Page size = ___ bytes" → bytes = actual value, not exponent.
```

## MISTAKE 2: WRONG HEX FOR 9-BIT MASK
```
WROTE:  0xFFF
SHOULD: 0x1FF

WHY WRONG: 0xFFF = 12 bits, 0x1FF = 9 bits. Typed "FFF" without counting Fs.
ORTHOGONAL: 0xFFF has 3 Fs = 3×4 = 12 bits. 0x1FF has 1+2×4 = 9 bits.
FIX: Count Fs. Each F = 4 bits. Prefix adds remainder bits.
```

## MISTAKE 3: CANNOT CONNECT HEX↔BINARY↔DECIMAL
```
SYMPTOM: "I know both tables but cannot connect"
ROOT CAUSE: Treating conversions as separate memorization, not one number in 3 spellings.
ORTHOGONAL: 60 = sixty = 0x3C = 0011_1100 = same quantity, different notation.
FIX: Pick ONE direction per task. Hex→binary for bit ops. Hex→decimal for printing.
```

---

## PATTERN DERIVED THIS SESSION

```
N-BIT MASK: N÷4 = Q remainder R → prefix = 2^R - 1 → mask = (prefix)(Q Fs)

LEFTOVER TABLE (2^R - 1):
  R=0 → prefix = 0 (nothing)
  R=1 → prefix = 1
  R=2 → prefix = 3
  R=3 → prefix = 7

EXAMPLES DONE BY HAND:
  9 bits:  9÷4=2r1 → 1 + FF = 0x1FF ✓
  10 bits: 10÷4=2r2 → 3 + FF = 0x3FF ✓
  17 bits: 17÷4=4r1 → 1 + FFFF = 0x1FFFF ✓
  21 bits: 21÷4=5r1 → 1 + FFFFF = 0x1FFFFF ✓
  47 bits: 47÷4=11r3 → 7 + FFFFFFFFFFF = 0x7FFFFFFFFFFF ✓

USAGE:
  RIGHTMOST bits: value & mask
  MIDDLE bits: (value >> shift) & mask
```

---

## PENDING ITEMS IN raw_pagewalk.c

| Block | Line | Status | Item |
|-------|------|--------|------|
| 02 | 45-47 | ✓ DONE | PAGE_SIZE=4096, ENTRIES=512, ENTRY_SIZE=8 |
| 03 | 59 | ✓ DONE | mask = 0x1FF |
| 03 | 68 | PENDING | pml4_index = (vaddr >> ___) & 0x___; |
| 04 | 100 | PENDING | asm volatile("mov %%cr3, %0"...) |
| 05 | 130 | PENDING | return KERNEL_DIRECT_MAP_BASE + phys |
| 06 | 167-174 | PENDING | All shift values (39,30,21,12) and masks |
| 07 | 198-204 | PENDING | read_entry_raw body |
| 08 | 224-226 | PENDING | is_present_raw body |
| 09 | 251-254 | PENDING | extract_next_table_phys body |
| 10 | 298-326 | PENDING | walk_raw body |

---

## NEXT ACTION
Fill line 68: `pml4_index = (vaddr >> 39) & 0x1FF;`

---

# SESSION MISTAKES - 2025-12-27

## ERROR 01: ASSUMED 4KB PAGES ONLY
- WRONG: "offset is always 12 bits"
- CORRECT: 12 bits (4KB), 21 bits (2MB), 30 bits (1GB)
- MISSED: Bit 7 (PS) determines page size

## ERROR 02: CONFUSED PHYSICAL VS VIRTUAL
- WRONG: "0x2FAE00000 is kernel base"
- CORRECT: 0x2FAE00000 = physical, 0xFFFF89DF00000000 = page_offset_base
- MISSED: Physical = 9 digits, Virtual = 16 digits

## ERROR 03: FLOATING POINT IN KERNEL
- WRONG: Used (double) and %.2f in kernel seq_printf
- CORRECT: Kernel cannot use FPU
- MISSED: Kernel disables SSE/FPU

## ERROR 04: UNEXPORTED SYMBOLS
- WRONG: Called for_each_online_pgdat in module
- CORRECT: Not exported to modules
- MISSED: Use si_meminfo() instead

## ERROR 05: MAX_ORDER RENAMED
- WRONG: Used MAX_ORDER constant
- CORRECT: Renamed in kernel 6.x
- MISSED: Check kernel headers

## ERROR 06: HARDCODED PAGE_OFFSET_BASE
- WRONG: Assumed 0xFFFF888000000000 works
- CORRECT: KASLR randomizes
- MISSED: Use extern symbol

## ERROR 07: WRONG INDEX CALCULATION
- WRONG: PDPT index = 498
- CORRECT: PDPT index = 511
- MISSED: (0x7FFE5E4ED123 >> 30) & 0x1FF = 511

## ERROR 08: FORGOT __VA EVERY LEVEL
- WRONG: Only CR3 needs __va()
- CORRECT: Every physical needs __va() to read
- MISSED: Entry values ARE physical

## ERROR 09: ADDRESS BITS CONFUSION
- WRONG: 34 bits stored in CR3
- CORRECT: 34 - 12 = 22 bits (alignment)
- MISSED: Low 12 bits implicit zeros

## ERROR 10: THOUGHT WALK STOP = NO DATA
- WRONG: Stop at PD = can't get data
- CORRECT: Stop = use larger offset
- MISSED: PS bit changes offset size

## PATTERN
- Mixing physical/virtual
- Copying without version checks
- Manual bit calculation errors
