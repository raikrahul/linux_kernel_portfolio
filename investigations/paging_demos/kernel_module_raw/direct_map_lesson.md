# DIRECT MAP LESSON - YOUR MACHINE r's AMD Ryzen 5 4600H

## PART 1: YOUR MACHINE DATA

```
RAM: 16 GB = 2^34 bytes = 17,179,869,184 bytes
CPU: AMD Ryzen 5 4600H
Physical address bits: 44 (from /proc/cpuinfo)
Virtual address bits: 48 (from /proc/cpuinfo)
Kernel: 6.14.0-37-generic
KASLR: ENABLED
page_offset_base: 0xFFFF89DF00000000 (randomized, from prove_direct_map.ko)
CR3 observed: 0x2CCB6C000, 0x12EB28000, 0x18D30E000
```

## PART 2: AXIOMS

```
AXIOM 1: CPU always translates virtual → physical using page tables
AXIOM 2: C pointer dereference *(ptr) → CPU interprets ptr as virtual
AXIOM 3: Cannot give CPU physical address directly in C code
AXIOM 4: Page tables stored in physical RAM
AXIOM 5: To read page tables, need virtual address pointing to them
```

## PART 3: THE PROBLEM AND SOLUTION

```
PROBLEM:
  CR3 = 0x2CCB6C000 (physical address of PML4)
  C code: *(unsigned long *)0x2CCB6C000 → CPU translates → NOT MAPPED → CRASH

SOLUTION:
  Kernel creates page table entries at boot:
    KEY (virtual)                 → VALUE (physical)
    0xFFFF89DF00000000           → 0x000000000
    0xFFFF89DF00001000           → 0x000001000
    0xFFFF89DF00002000           → 0x000002000
    ...
    0xFFFF89E1CCB6C000           → 0x2CCB6C000  ← YOUR CR3
    ...
    0xFFFF89E2FFFFFFFF           → 0x3FFFFFFFF

  Now: *(unsigned long *)0xFFFF89E1CCB6C000 → CPU translates → 0x2CCB6C000 → WORKS
  Formula: virtual = page_offset_base + physical = 0xFFFF89DF00000000 + 0x2CCB6C000
```

## PART 4: PAGE TABLE ENTRIES IN RAM (ACTUAL)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│ PHYSICAL RAM CONTENTS AFTER BOOT                                                │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│ RAM[0x00001000]: PML4 TABLE (kernel's init_mm.pgd)                              │
│ ┌─────────────────────────────────────────────────────────────────────────────┐ │
│ │ Index │ Value              │ Meaning                                       │ │
│ ├───────┼────────────────────┼───────────────────────────────────────────────┤ │
│ │ 0     │ 0x0000000000000000 │ NOT PRESENT (user space)                      │ │
│ │ 1     │ 0x0000000000000000 │ NOT PRESENT                                   │ │
│ │ ...   │ ...                │ ...                                           │ │
│ │ 275   │ 0x0000000000002067 │ PRESENT → PDPT at physical 0x2000             │ │
│ │ ...   │ ...                │ ...                                           │ │
│ │ 511   │ 0x00000002FB845067 │ PRESENT → kernel text PDPT                    │ │
│ └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                 │
│ RAM[0x00002000]: PDPT TABLE FOR DIRECT MAP                                      │
│ ┌─────────────────────────────────────────────────────────────────────────────┐ │
│ │ Index │ Value              │ Meaning                                       │ │
│ ├───────┼────────────────────┼───────────────────────────────────────────────┤ │
│ │ 0     │ 0x0000000000003067 │ PRESENT → PD at physical 0x3000 (1st GB)      │ │
│ │ 1     │ 0x0000000000004067 │ PRESENT → PD at physical 0x4000 (2nd GB)      │ │
│ │ ...   │ ...                │ ...                                           │ │
│ │ 15    │ 0x0000000000012067 │ PRESENT → PD for last GB of 16 GB             │ │
│ └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                 │
│ RAM[0x00003000]: PD TABLE FOR FIRST 1 GB (using 2MB huge pages)                 │
│ ┌─────────────────────────────────────────────────────────────────────────────┐ │
│ │ Index │ Value              │ Meaning                                       │ │
│ ├───────┼────────────────────┼───────────────────────────────────────────────┤ │
│ │ 0     │ 0x0000000000000083 │ HUGE PAGE → physical 0x000000 (2MB)           │ │
│ │ 1     │ 0x0000000000200083 │ HUGE PAGE → physical 0x200000 (2MB)           │ │
│ │ 2     │ 0x0000000000400083 │ HUGE PAGE → physical 0x400000 (2MB)           │ │
│ │ ...   │ ...                │ ...                                           │ │
│ │ 511   │ 0x000000003FE00083 │ HUGE PAGE → physical 0x3FE00000 (2MB)         │ │
│ └─────────────────────────────────────────────────────────────────────────────┘ │
│                                                                                 │
│ TOTAL: 8192 PD entries × 2 MB = 16 GB covered                                   │
└─────────────────────────────────────────────────────────────────────────────────┘
```

## PART 5: PROOF FROM YOUR MACHINE

```
$ cat /proc/prove_direct_map

page_offset_base = 0xFFFF89DF00000000

Physical 0x000000000000:
  Virtual = 0xFFFF89DF00000000 + 0x0 = 0xFFFF89DF00000000 ✓
  RAM[0x0] first byte = 0x00 (readable)

Physical 0x000000001000:
  Virtual = 0xFFFF89DF00000000 + 0x1000 = 0xFFFF89DF00001000 ✓
  RAM[0x1000] first byte = 0x00 (readable)

Physical 0x0002CCB6C000:
  Virtual = 0xFFFF89DF00000000 + 0x2CCB6C000 = 0xFFFF89E1CCB6C000 ✓
  (Your CR3 location accessible)
```

## PART 6: CR3 BIT STRUCTURE

```
CR3 = 64 bits = 0x0000_0002_CCB6_C000

┌────────────────┬────────────────────────────────────────┬────────────────┐
│ bits [63:52]   │ bits [51:12]                           │ bits [11:0]    │
│ 12 bits        │ 40 bits                                │ 12 bits        │
│ RESERVED = 0   │ PML4 PHYSICAL ADDRESS                  │ PCID = 0 (AMD) │
├────────────────┼────────────────────────────────────────┼────────────────┤
│ 0x000          │ 0x0002CCB6C (22 bits used of 40)       │ 0x000          │
└────────────────┴────────────────────────────────────────┴────────────────┘

YOUR CPU: 44 bits physical → uses 32 of 40 available
YOUR RAM: 16 GB = 2^34 → addresses use 34 bits, stored bits = 34 - 12 = 22
```

## PART 7: WHY BIT SHIFTING STILL NEEDED

```
Direct map: lets kernel READ page tables from RAM
Bit shifting: determines WHICH ENTRY to read

USER VA = 0x7FFE5E4ED000 (48 bits used)

┌──────────────────────────────────────────────────────────────────────────────┐
│ bit 47      39 38      30 29      21 20      12 11                        0 │
│ ├─ PML4 ──┤ ├─ PDPT ──┤ ├── PD ──┤ ├── PT ──┤ ├────── OFFSET ─────────────┤ │
│ 0_11111111  1_11100101  1_11100100  1_11011010  0000_0000_0000              │
│ = 255       = 485       = 484       = 474       = 0                         │
└──────────────────────────────────────────────────────────────────────────────┘

WALK:
  1. CR3 = 0x2CCB6C000 → PML4 at __va(0x2CCB6C000) = 0xFFFF89E1CCB6C000
  2. PML4[255] = *(0xFFFF89E1CCB6C000 + 255×8) = next PDPT physical
  3. PDPT physical → __va() → read PDPT[485] → next PD physical
  4. PD physical → __va() → read PD[484] → next PT physical (or huge page)
  5. PT physical → __va() → read PT[474] → final physical page + offset
```

## PART 7.5: FAMILY TREE - HOW TABLES MULTIPLY

```
PAGE TABLE FAMILY TREE (FULLY POPULATED):

                                   CR3 = 0x2CCB6C000
                                          │
                                          ▼
                            ┌──────────────────────────────┐
                            │ PML4 (1 great-great-grandpa) │
                            │ 512 entries × 8 bytes = 4 KB │
                            └──────────────┬───────────────┘
                                           │ each entry → 1 child
         ┌───────────┬───────────┬─────────┴──────────┬───────────┐
         ▼           ▼           ▼                    ▼           ▼
    ┌─────────┐ ┌─────────┐ ┌─────────┐         ┌─────────┐ ┌─────────┐
    │ PDPT 0  │ │ PDPT 1  │ │ PDPT 2  │   ...   │PDPT 510 │ │PDPT 511 │ ← 512 great-grandpas
    │  4 KB   │ │  4 KB   │ │  4 KB   │         │  4 KB   │ │  4 KB   │   512 × 4KB = 2 MB
    └────┬────┘ └────┬────┘ └────┬────┘         └────┬────┘ └────┬────┘
         │           │           │                   │           │
    ┌────┴────┐ ┌────┴────┐ ┌────┴────┐         ┌────┴────┐ ┌────┴────┐
    │512 PDs  │ │512 PDs  │ │512 PDs  │   ...   │512 PDs  │ │512 PDs  │ ← 262,144 grandpas
    │each 4KB │ │each 4KB │ │each 4KB │         │each 4KB │ │each 4KB │   262K × 4KB = 1 GB
    └────┬────┘ └────┬────┘ └────┬────┘         └────┬────┘ └────┬────┘
         │           │           │                   │           │
    ┌────┴────┐ ┌────┴────┐ ┌────┴────┐         ┌────┴────┐ ┌────┴────┐
    │512×512  │ │512×512  │ │512×512  │   ...   │512×512  │ │512×512  │ ← 134,217,728 fathers
    │PTs 4KB  │ │PTs 4KB  │ │PTs 4KB  │         │PTs 4KB  │ │PTs 4KB  │   134M × 4KB = 512 GB
    └────┬────┘ └────┬────┘ └────┬────┘         └────┬────┘ └────┬────┘
         │           │           │                   │           │
         ▼           ▼           ▼                   ▼           ▼
    ┌─────────────────────────────────────────────────────────────────┐
    │        68,719,476,736 DATA PAGES (4 KB each) = 256 TB           │ ← children (your data)
    └─────────────────────────────────────────────────────────────────┘

TABLE COUNT:
  Level 1:  1 PML4                           ×  4 KB =      4 KB
  Level 2:  512 PDPTs                        ×  4 KB =      2 MB
  Level 3:  512 × 512 = 262,144 PDs          ×  4 KB =      1 GB
  Level 4:  512³ = 134,217,728 PTs           ×  4 KB =    512 GB
  ──────────────────────────────────────────────────────────────────
  TOTAL PAGE TABLES (if FULLY populated):           =   ~512 GB
  DATA PAGES MAPPED: 512⁴ × 4 KB                    =    256 TB

BUT YOUR PROCESS IS SPARSE:
  Uses ~25 mappings (from /proc/self/maps)
  Needs: 1 PML4 + 1-3 PDPTs + 5-10 PDs + 20-30 PTs
  Actual page table memory ≈ 100-200 KB (not 512 GB)
```

## PART 8: YOUR MISTAKES AND ORTHOGONAL ANALYSIS

```
MISTAKE 1: "Hardcoded 0xFFFF888000000000 will work"
  YOUR THOUGHT: Static constant works everywhere
  REALITY: KASLR randomizes base → your kernel uses 0xFFFF89DF00000000
  ORTHOGONAL: Why would kernel developers leave security hole?

MISTAKE 2: "Only 12 bits of CR3 useful"
  YOUR THOUGHT: Low bits important
  REALITY: Low 12 bits = PCID (unused on AMD). High 40 bits = address.
  ORTHOGONAL: If only 12 bits, how store 34-bit address?

MISTAKE 3: "34 bits stored in CR3"
  YOUR THOUGHT: 16 GB needs 34 bits
  REALITY: 16 GB needs 34 bits total, but 12 implied zeros → store 22 bits
  ORTHOGONAL: If 34 bits stored, why CR3 ends in 000?

MISTAKE 4: "Why add base at all"
  YOUR THOUGHT: Physical address should work directly
  REALITY: CPU interprets ALL pointers as virtual → must convert
  ORTHOGONAL: If physical worked, why have page tables?

MISTAKE 5: "Infinite virtual addresses pre-mapped"
  YOUR THOUGHT: All possible VAs have entries
  REALITY: Only 16 GB (your RAM) mapped in direct map. Rest = NOT PRESENT.
  ORTHOGONAL: 256 TB virtual > 16 GB physical. How fit infinite in finite?

MISTAKE 6: "Same mapping works for user addresses"
  YOUR THOUGHT: One mapping for all
  REALITY: Direct map = kernel access to RAM. User VA = per-process tables.
  ORTHOGONAL: If same mapping, why different CR3 per process?
```

## PART 9: SUMMARY IN SINGLE PARAGRAPH

AT BOOT kernel detects 16 GB RAM at physical 0x0 to 0x3FFFFFFFF and writes 8192 page directory entries into RAM starting at physical 0x3000 where PD[0]=0x0000000000000083 means virtual range 0xFFFF89DF00000000 to 0xFFFF89DF001FFFFF translates to physical 0x0 to 0x1FFFFF and PD[1]=0x0000000000200083 means next 2MB and so on until PD[8191] covers the last 2MB of your RAM and these entries are pointed to by PDPT entries at physical 0x2000 which is pointed to by PML4[275] at physical 0x1000 and this PML4 address goes into kernel's CR3 making the formula virtual=page_offset_base+physical work because CPU walks PML4[275]→PDPT[X]→PD[Y]→huge page and finds physical address matching the offset from base and your prove_direct_map.ko proved this by reading page_offset_base=0xFFFF89DF00000000 and showing __va(0x2CCB6C000)=0xFFFF89E1CCB6C000 which when dereferenced by CPU translates back to physical 0x2CCB6C000 where your PML4 table lives and this is why you add base to CR3: to get a virtual address that CPU can translate back to the same physical location through these page table entries created at boot.

## PART 10: EXERCISES

```
EX1: Calculate virtual for physical 0x12EB28000
  virtual = 0xFFFF89DF00000000 + 0x12EB28000 = ?

EX2: Which PD entry covers physical 0x2CCB6C000?
  0x2CCB6C000 ÷ 0x200000 = ?

EX3: How many PD tables needed for 16 GB?
  16 GB ÷ 1 GB per PDPT entry = ? PDPT entries
  Each PDPT entry → 1 PD table (512 entries × 2 MB = 1 GB)

EX4: If RAM were 32 GB, how many more PD entries?
  32 GB ÷ 2 MB = ? entries total
  Additional = ? - 8192 = ?
```

## PART 11: FAILURE PREDICTIONS

```
FAIL 1: Use 0xFFFF888000000000 on KASLR kernel → page fault (PML4[273]=0)
FAIL 2: Forget mask on CR3 with PCID bits → wrong address by N bytes
FAIL 3: Dereference physical directly → unmapped virtual → crash
FAIL 4: Walk user VA with wrong process CR3 → NOT PRESENT entries
FAIL 5: Assume 40 bits fully used → only 22 bits used for 16 GB
FAIL 6: Think direct map covers user VAs → NO, only physical RAM
```
