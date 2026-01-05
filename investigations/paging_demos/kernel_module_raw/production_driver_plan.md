# PRODUCTION DRIVER PLANNING - GRILL THE TASKS

## RAW DATA FROM MACHINE

```
CPU: AMD Ryzen 5 4600H
Physical bits: 44 → 2^44 = 17,592,186,044,416 bytes = 16 TB max
Virtual bits: 48 → 2^48 = 281,474,976,710,656 bytes = 256 TB max
MemTotal: 15,776,276 KB = 15,406 MB = 15.04 GB
PageTables: 95,888 KB = 93.6 MB used for page tables now
Present pages: 3,999 + 699,516 + 3,339,072 = 4,042,587 pages
4,042,587 × 4 KB = 16,170,348 KB = 15.43 GB

Flags present: pdpe1gb (1GB pages), pse (4MB pages in 32-bit)
Flags absent: la57 (5-level paging not supported by CPU)
Kernel compiled: CONFIG_PGTABLE_LEVELS=5 (but CPU lacks la57)
```

## LIVE KERNEL DATA

```
page_offset_base from /proc/kallsyms:
├── ADDRESS: ffffffff9695b668 D page_offset_base
├── WHAT: Kernel symbol at virtual 0xFFFFFFFF9695B668
├── TYPE: D = data symbol (not code)
├── VALUE AT RUNTIME: 0xFFFF89DF00000000 (from prove_direct_map.ko)
├── WHY DIFFERENT ADDRESSES:
│   ├── Symbol address: 0xFFFFFFFF9695B668 = where variable stored
│   └── Symbol value: 0xFFFF89DF00000000 = what variable contains
├── KASLR: Base randomized each boot
├── FORMULA: __va(phys) = page_offset_base + phys
└── IN CODE: extern unsigned long page_offset_base;

Current CR3 from /proc/pagewalk:
├── VALUE: CR3=0x00000001337A3000
├── PHYSICAL PML4 ADDRESS: 0x1337A3000
├── PCID: 0 (AMD = no PCID)
├── MASK: 0x1337A3000 & 0x000FFFFFFFFFF000 = 0x1337A3000 ✓
├── BITS USED: log2(0x1337A3000) = 32.3 → 33 bits actual, stored 21 bits
├── VIRTUAL PML4: 0xFFFF89DF00000000 + 0x1337A3000 = 0xFFFF89E0337A3000
├── PML4 TABLE SIZE: 512 × 8 = 4,096 bytes = 1 page
├── PML4 ENTRY RANGE: 0x1337A3000 to 0x1337A3FFF
└── CHANGED SINCE LAST CHECK: was 0x10AB4A000, now 0x1337A3000
    └── WHY: Different process reading /proc/pagewalk

CR3 CHANGE EXPLANATION:
├── CR3 = 0x10AB4A000 (earlier observation)
├── CR3 = 0x1337A3000 (current observation)
├── DIFFERENCE: Different processes have different PML4 tables
├── KERNEL HALF: PML4[256:511] same in both (shared kernel mappings)
├── USER HALF: PML4[0:255] different (each process has own user space)
└── WHEN CHANGES: Context switch (scheduler picks different process)
```

## PAGE_OFFSET_BASE BREAKDOWN

```
page_offset_base = 0xFFFF89DF00000000

BINARY ANALYSIS:
├── HEX: 0xFFFF89DF00000000
├── BITS [63:48]: 0xFFFF = sign extension (canonical high)
├── BITS [47:40]: 0x89 = 1000_1001 binary
├── PML4 INDEX: (0xFFFF89DF00000000 >> 39) & 0x1FF = 275
│   └── Direct map uses PML4[273-276] region
├── TOTAL: 64 bits, but only 48 significant (rest = sign extended)
└── RANDOMIZED: Different each boot (KASLR)

WHY THIS VALUE:
├── x86_64 canonical address: bits [63:48] must match bit 47
├── Kernel space: bit 47 = 1 → bits [63:47] = all 1s → 0xFFFF8...
├── User space: bit 47 = 0 → bits [63:47] = all 0s → 0x0000...
├── Gap in middle: non-canonical addresses → fault if used
└── KASLR offset: 0x89DF... randomized within kernel range

DIRECT MAP COVERAGE:
├── START: 0xFFFF89DF00000000 + 0 = 0xFFFF89DF00000000
├── END: 0xFFFF89DF00000000 + 0x3FFFFFFFF = 0xFFFF89E2FFFFFFFF
├── SIZE: 16 GB (your RAM)
├── MAPS: physical 0x0 to physical 0x3FFFFFFFF
└── ANY PHYSICAL: __va(phys) = base + phys → accessible
```

## MEMINFO BREAKDOWN

```
MemTotal: 15,776,276 KB
├── WHAT: Total usable RAM reported by kernel
├── WHY: BIOS reports hardware RAM, minus reserved regions
├── WHERE: /proc/meminfo line 1
├── CALCULATION: 15,776,276 ÷ 1024 = 15,406 MB = 15.04 GB
├── VS HARDWARE: You have 2×8 GB = 16 GB installed
├── DIFFERENCE: 16,384 MB - 15,406 MB = 978 MB reserved for:
│   ├── GPU: AMD integrated graphics uses ~512 MB
│   ├── BIOS/UEFI: ~64 MB
│   ├── ACPI tables: ~128 MB
│   └── Memory-mapped I/O: ~274 MB
└── IN CODE: totalram_pages() × PAGE_SIZE

PageTables: 95,888 KB
├── WHAT: RAM consumed by page table structures NOW
├── WHY: All running processes need page tables
├── CALCULATION: 95,888 KB = 23,972 pages of page tables
├── BREAKDOWN: ~200 processes × ~120 KB avg = 24,000 KB (rough)
├── INCLUDES: PML4 + PDPT + PD + PT for all processes
├── KERNEL SHARE: PML4[256:511] duplicated in each process
├── OVERHEAD: 95,888 / 15,776,276 = 0.6% of RAM for page tables
└── IN CODE: NR_PAGETABLE pages, /proc/<pid>/smaps shows per-process

SecPageTables: 4,836 KB
├── WHAT: Secondary page tables (AMD SEV, TDX, etc.)
├── WHY: Secure Encrypted Virtualization needs separate tables
├── YOUR CPU: sev flag present, sev_es flag present
└── USAGE: VM encryption, trusted execution

AnonHugePages: 2,048 KB
├── WHAT: Anonymous memory using 2 MB huge pages (THP)
├── WHY: Transparent Huge Pages for heap/stack performance
├── CALCULATION: 2,048 KB ÷ 2,048 KB = 1 huge page currently
├── WHERE: heap allocations that got promoted to THP
├── VERIFICATION: cat /sys/kernel/mm/transparent_hugepage/enabled
└── IN CODE: folio_test_large() for huge page detection

ShmemHugePages: 0 KB
├── WHAT: Shared memory using huge pages
└── WHY ZERO: No shmem configured for huge pages

FileHugePages: 0 KB
├── WHAT: File-backed memory using huge pages
└── WHY ZERO: No files mapped with huge pages

HugePages_Total: 0
├── WHAT: Pre-allocated huge pages (hugetlbfs)
├── WHY ZERO: Not manually configured
├── TO SET: echo 10 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
└── PREALLOCATION: For databases, VMs that need guaranteed huge pages

HugePages_Free: 0
├── WHAT: Unused preallocated huge pages
└── WHY ZERO: None preallocated

HugePages_Rsvd: 0
├── WHAT: Reserved but not yet faulted in
└── MEANING: mmap() done, page access not yet occurred

HugePages_Surp: 0
├── WHAT: Surplus huge pages above persistent pool
└── MEANING: vm.nr_overcommit_hugepages allows extra

Hugepagesize: 2,048 KB
├── WHAT: Default huge page size
├── WHY: x86_64 default = 2 MB
├── ALTERNATIVE: 1 GB huge pages (pdpe1gb flag present)
└── CONFIGURED AT: /sys/kernel/mm/hugepages/hugepages-*/
```

## ZONEINFO BREAKDOWN

```
ZONE DMA (0-16 MB):
├── spanned: 4,095 pages = 16 MB
│   └── WHAT: Total address range covered
├── present: 3,999 pages = 15.6 MB
│   └── WHAT: Actually existing physical pages
├── managed: 3,842 pages = 15 MB
│   └── WHAT: Pages kernel can allocate
├── WHY EXISTS: Legacy ISA DMA needs addresses < 16 MB
├── DRIVER USE: sound cards, old network cards
└── CALCULATION: 4,095 × 4 KB = 16,380 KB ≈ 16 MB

ZONE DMA32 (16 MB - 4 GB):
├── spanned: 1,044,480 pages = 4,080 MB
├── present: 699,516 pages = 2,732 MB
├── managed: 683,053 pages = 2,668 MB
├── WHY EXISTS: 32-bit device DMA needs addresses < 4 GB
├── DRIVER USE: older GPUs, NICs, SATA controllers
├── GAP: 1,044,480 - 699,516 = 344,964 pages not present
│   └── REASON: memory holes, reserved regions
└── CALCULATION: 699,516 × 4 KB = 2,798,064 KB = 2.67 GB

ZONE Normal (4 GB - end):
├── spanned: 3,339,072 pages = 13,043 MB
├── present: 3,339,072 pages = 13,043 MB  ← ALL PRESENT
├── managed: 3,257,174 pages = 12,723 MB
├── WHY EXISTS: Main memory zone for most allocations
├── NO RESTRICTIONS: Any device can use this
├── RESERVED: 3,339,072 - 3,257,174 = 81,898 pages for kernel
│   └── WHAT: kernel code, kernel data structures
└── CALCULATION: 3,339,072 × 4 KB = 13,356,288 KB = 12.74 GB

ZONE Movable:
├── spanned/present/managed: 0
├── WHY EXISTS: For memory hotplug
└── YOUR SYSTEM: No hotplug configured

ZONE Device:
├── spanned/present/managed: 0
├── WHY EXISTS: For persistent memory (PMEM)
└── YOUR SYSTEM: No PMEM installed

TOTAL PRESENT: 3,999 + 699,516 + 3,339,072 = 4,042,587 pages
TOTAL MANAGED: 3,842 + 683,053 + 3,257,174 = 3,944,069 pages
RESERVED: 4,042,587 - 3,944,069 = 98,518 pages = 385 MB kernel reserved
```

## KERNEL CONFIG BREAKDOWN

```
CONFIG_PGTABLE_LEVELS=5
├── WHAT: Compiled with 5-level paging support
├── LEVELS: PML5 → PML4 → PDPT → PD → PT → data
├── ADDRESS BITS: 57-bit virtual (LA57)
├── VIRTUAL SPACE: 2^57 = 128 PB (petabytes)
├── WHY 5: Future-proofing for large memory servers
├── YOUR CPU: AMD Ryzen 5 4600H lacks la57 flag
├── RUNTIME: Kernel detects la57 absent → uses 4 levels
├── CODE CHECK: #ifdef CONFIG_X86_5LEVEL / pgtable_l5_enabled()
├── FALLBACK: P4D table "folded" into PML4
└── WHERE IN CODE:
    ├── arch/x86/include/asm/pgtable_64_types.h
    ├── PTRS_PER_P4D = 512 (5-level) or 1 (4-level folded)
    └── pgtable_l5_enabled() returns false on your CPU

CONFIG_X86_5LEVEL=y
├── WHAT: Enable/disable 5-level paging compile option
├── DEPENDENCY: Implies CONFIG_PGTABLE_LEVELS=5
├── RUNTIME CHECK: if (pgtable_l5_enabled()) { use 5 levels }
├── YOUR CPU RESULT: false → 4 levels used
├── DETECTION: CPUID.07H:ECX.LA57 bit = 0 on your CPU
└── KERNEL MESSAGE: dmesg | grep "LA57" (nothing = disabled)

HOW 5-LEVEL FOLDS TO 4-LEVEL:
├── P4D (Page Level 5 Directory) has 1 entry
├── That 1 entry points directly to PML4
├── No actual P4D table walk
├── Code: p4d_offset() returns pgd value unchanged
├── pgtable_l5_enabled() checked at boot:
│   ├── true: P4D table exists, 5 walks
│   └── false: P4D folded, 4 walks
└── PROOF: Your /proc/pagewalk shows L4/L3/L2 (4 levels)
```

## QUESTIONS ABOUT THIS DATA

Q1: Why is spanned ≠ present in DMA32?
- Spanned = address range covered
- Present = physical RAM chips at those addresses
- Gap = memory holes, reserved by BIOS, used by devices

Q2: Why is managed < present?
- Managed = allocatable by kernel
- Difference = kernel reserved (code, static data, early allocations)

Q3: How to see PageTables per process?
- cat /proc/<pid>/status | grep VmPTE
- Or: /proc/<pid>/smaps → "size" of PTEs

Q4: Why 2 MB default hugepage, not 1 GB?
- 1 GB = coarse, waste memory for small allocations
- 2 MB = balance between TLB coverage and granularity
- 1 GB requires pdpe1gb flag (you have it)

Q5: What uses DMA zone in modern systems?
- Nearly nothing
- Legacy compatibility
- Most DMA uses DMA32 or Normal via IOMMU

Q6: How does kernel know which zone to allocate from?
- GFP_DMA: must use DMA zone
- GFP_DMA32: must use DMA32 or DMA
- GFP_KERNEL: prefers Normal, fallback to others

## TASK 1: READ CR3 REGISTER

WHAT: 64-bit value from CPU control register 3
WHERE: mov %%cr3, %0 in inline assembly
WHEN: module_init() or on-demand via /proc read
WHO: kernel code only (ring 0)
WHY: CR3 bits [51:12] = physical address of PML4 × 4096
WITHOUT: cannot start page walk
WHICH: current process CR3 or target process task->mm->pgd

NUMERICAL GRILL:
- CR3 = 0x10AB4A000 (from your machine)
- bits [51:12] = 0x10AB4A (20 bits actually used of 40 available)
- bits [11:0] = 0x000 (PCID on Intel, reserved on AMD)
- Physical PML4 = 0x10AB4A × 0x1000 = 0x10AB4A000
- Virtual PML4 = 0xFFFF89DF00000000 + 0x10AB4A000 = 0xFFFF89E00AB4A000

EDGE CASES:
- CR3 = 0x0 → invalid (but should never happen)
- CR3 = 0xFFFFFFFFFFFFFFFF → masked to 0xFFFFFFFFF000 → 52-bit address
- CR3 with PCID = 0x123456789ABC → mask to 0x123456789000, PCID = 0xABC

## TASK 2: EXTRACT PAGE TABLE INDICES

WHAT: Split 48-bit VA into 4×9-bit indices + 12-bit offset
WHERE: extract_indices_raw() function
WHEN: before each walk
WHO: driver code
WHY: Each 9 bits selects 1 of 512 entries (2^9 = 512)
WITHOUT: cannot locate correct entry in table
WHICH: bits [47:39], [38:30], [29:21], [20:12], [11:0]

NUMERICAL GRILL:
- VA = 0x7FFE5E4ED123
- Binary bits [47:0]:
  0111 1111 1111 1110 0101 1110 0100 1110 1101 0001 0010 0011
  
- PML4 [47:39]: 0_1111_1111 = 255
- PDPT [38:30]: 1_1111_0010 = 498  ← VERIFY: (0x7FFE5E4ED123 >> 30) & 0x1FF
- PD [29:21]:   1_1110_0100 = 484
- PT [20:12]:   1_1110_1101 = 493  ← WAIT: (0x7FFE5E4ED123 >> 12) & 0x1FF = 0x4ED & 0x1FF = 0x0ED = 237
- offset [11:0]: 0001_0010_0011 = 0x123 = 291

RECALCULATE:
- 0x7FFE5E4ED123 >> 39 = 0x7FFE5E4ED123 / 2^39 = 140,734,598,676,771 / 549,755,813,888 = 255.99 → 255 ✓
- 0x7FFE5E4ED123 >> 30 = 140,734,598,676,771 / 1,073,741,824 = 131,071.36 → 131,071 & 0x1FF = 0x1FF = 511? 
  
Let me redo:
- 0x7FFE5E4ED123 = 140,734,598,676,771
- >> 39 = 255 (correct)
- >> 30 = 131,071 → & 0x1FF = 131,071 mod 512 = 511
- >> 21 = 67,108,146 → & 0x1FF = 67,108,146 mod 512 = 242
- >> 12 = 34,359,370,989 → & 0x1FF = 34,359,370,989 mod 512 = 237
- & 0xFFF = 0x123 = 291

CORRECTED:
- PML4 idx = 255
- PDPT idx = 511
- PD idx = 242
- PT idx = 237
- offset = 291

SCALE TEST:
- VA = 0x0000000000001000: PML4=0, PDPT=0, PD=0, PT=1, off=0
- VA = 0x7FFFFFFFFFFF: PML4=255, PDPT=511, PD=511, PT=511, off=4095
- VA = 0xFFFF800000000000: PML4=256 → kernel space start
- VA = 0xFFFFFFFFFFFFFFFF: PML4=511, PDPT=511, PD=511, PT=511, off=4095

## TASK 3: READ PAGE TABLE ENTRY

WHAT: 8 bytes from RAM at computed address
WHERE: read_entry_raw(table_phys, index)
WHEN: at each level of walk
WHO: kernel via __va() conversion
WHY: Entry contains next-level address + flags
WITHOUT: walk cannot proceed
WHICH: table_phys + index × 8

NUMERICAL GRILL:
- table_phys = 0x2FB846000
- index = 180
- byte_offset = 180 × 8 = 1,440 = 0x5A0
- entry_phys = 0x2FB846000 + 0x5A0 = 0x2FB8465A0
- entry_virt = 0xFFFF89DF00000000 + 0x2FB8465A0 = 0xFFFF89E1FB8465A0
- entry_value = *(0xFFFF89E1FB8465A0) = 0x80000002FAE001A1 (from your machine)

EDGE CASES:
- index = 0: offset = 0, read first entry
- index = 511: offset = 4088 = 0xFF8, last entry
- index = 512: INVALID, out of bounds (but no check in raw code)

## TASK 4: CHECK PRESENT BIT

WHAT: bit 0 of entry
WHERE: is_present_raw(entry)
WHEN: after each entry read
WHO: driver
WHY: if bit 0 = 0, entry invalid, page fault
WITHOUT: would dereference garbage addresses
WHICH: entry & 0x1

NUMERICAL GRILL:
- entry = 0x80000002FAE001A1
- 0x1A1 & 0x1 = 1 → present ✓
- entry = 0x0000000000000000
- 0x000 & 0x1 = 0 → NOT present
- entry = 0x80000002FAE001A0
- 0x1A0 & 0x1 = 0 → NOT present (even with address bits set!)

## TASK 5: CHECK HUGE PAGE BIT

WHAT: bit 7 of entry (PS = Page Size)
WHERE: (entry >> 7) & 0x1
WHEN: at PDPT level (1GB) and PD level (2MB)
WHO: driver
WHY: determines if walk stops early
WITHOUT: would continue into garbage tables
WHICH: bit 7 = 0x80

NUMERICAL GRILL:
- entry = 0x80000002FAE001A1
- 0x1A1 = 0001_1010_0001
- bit 7 = (0x1A1 >> 7) & 1 = 1 → HUGE PAGE
- entry = 0x00000002FB846063
- 0x063 = 0110_0011
- bit 7 = (0x063 >> 7) & 1 = 0 → NOT huge, continue walk

## TASK 6: EXTRACT PHYSICAL ADDRESS FROM ENTRY

WHAT: bits [51:12] for regular, [51:21] for 2MB, [51:30] for 1GB
WHERE: entry & mask
WHEN: after bit 7 check
WHO: driver
WHY: next table location or final page location
WITHOUT: cannot find next level
WHICH: 
  - 4KB/table: entry & 0x000FFFFFFFFFF000
  - 2MB huge:  entry & 0x000FFFFFFFE00000
  - 1GB huge:  entry & 0x000FFFFFC0000000

NUMERICAL GRILL FOR 2MB:
- entry = 0x80000002FAE001A1
- mask = 0x000FFFFFFFE00000 (clear bits [20:0] and [63:52])
- 0x80000002FAE001A1 & 0x000FFFFFFFE00000 = 0x0000000002FAE00000 → WAIT
  
Recalculate:
- 0x80000002FAE001A1 in binary [51:0]:
  1000_0000_0000_0000_0000_0000_0010_1111_1010_1110_0000_0000_0001_1010_0001
- bits [51:21] = 0000_0001_0111_1101_0111_0000_0000 = 0x2FAE00
- physical = 0x2FAE00 × 2^21 = 0x2FAE00 × 0x200000 = ... wait that's wrong

Actually: entry has physical address in bits [51:21], but stored shifted.
- 0x80000002FAE001A1 >> 21 = 0x40000017D700... no

Correct approach:
- entry & 0x000FFFFFFFE00000 = just mask, don't shift
- 0x80000002FAE001A1 & 0x000FFFFFFFE00000 = 0x0000000002FAE00000

Wait: 0x80000002FAE001A1 has bit 63 set. Let me mask properly:
- 0x80000002FAE001A1
  & 0x000FFFFFFFFFF000 (PTE_ADDR_MASK for 4KB)
  = 0x0000000002FAE01000

For 2MB:
- 0x80000002FAE001A1
  & 0x000FFFFFFFE00000 (clear low 21 bits)
  = 0x0000000002FAE00000

∴ 2MB page base = 0x2FAE00000

## TASK 7: CALCULATE FINAL PHYSICAL ADDRESS

WHAT: page_base | offset
WHERE: return statement
WHEN: walk complete
WHO: driver
WHY: combine page base with offset from VA
WITHOUT: would return only page base, not exact byte
WHICH: depends on page size

NUMERICAL GRILL:
- VA = 0xFFFFFFFF9695B668
- 2MB page base = 0x2FAE00000
- 21-bit offset = 0x9695B668 & 0x1FFFFF = 0x15B668
- Final PA = 0x2FAE00000 | 0x15B668 = 0x2FAF5B668

Verify offset calculation:
- 0x1FFFFF = 2,097,151
- 0x9695B668 = 2,526,595,688
- 2,526,595,688 mod 2,097,152 = 2,526,595,688 - (1 × 2,097,152) = ...
  Actually: 2,526,595,688 / 2,097,152 = 1,204.83
  1,204 × 2,097,152 = 2,524,999,008
  2,526,595,688 - 2,524,999,008 = 1,596,680 = 0x186068

Wait, let me use bitwise:
- 0x9695B668 AND 0x1FFFFF:
  0x9695B668 = 1001_0110_1001_0101_1011_0110_0110_1000
  0x001FFFFF = 0000_0001_1111_1111_1111_1111_1111_1111
  Result     = 0000_0000_0001_0101_1011_0110_0110_1000 = 0x15B668 ✓

## TASK 8: HANDLE ALL PAGE SIZES

WHAT: 4KB, 2MB, 1GB detection and handling
WHERE: walk_raw() at each level
WHEN: runtime based on bit 7
WHO: driver
WHY: cannot know page size beforehand
WITHOUT: incorrect offset calculation
WHICH: check entry >> 7 at PDPT (1GB) and PD (2MB)

NUMERICAL GRILL:
- 4KB offset mask = 0xFFF = 4,095 bytes max
- 2MB offset mask = 0x1FFFFF = 2,097,151 bytes max
- 1GB offset mask = 0x3FFFFFFF = 1,073,741,823 bytes max

Page base alignment:
- 4KB: last 12 bits = 0 → 0x????000
- 2MB: last 21 bits = 0 → 0x???00000
- 1GB: last 30 bits = 0 → 0x?0000000

## QUESTIONS TO GRILL

Q1: What if entry has NX bit (bit 63) set?
- Answer: NX = No Execute. Mask clears it. Address unaffected.
- 0x80000002FAE001A1: bit 63 = 1 (NX set), masked out by 0x000FFFFFFFFFF000

Q2: What if walk encounters NOT PRESENT at level 2?
- Answer: Return 0xDEAD0002, walk fails.
- Caller must handle error code.

Q3: What if huge page at PDPT level (1GB)?
- Answer: Currently returns 0xDEAD1001 (not implemented)
- Need: (entry & 0x000FFFFFC0000000) | (vaddr & 0x3FFFFFFF)

Q4: How many memory reads per walk?
- 4KB path: 4 reads (PML4, PDPT, PD, PT)
- 2MB path: 3 reads (PML4, PDPT, PD)
- 1GB path: 2 reads (PML4, PDPT)
- Each read: 8 bytes from RAM via __va()

Q5: What is TLB and why does driver bypass it?
- TLB = Translation Lookaside Buffer, CPU cache of translations
- Driver does manual walk = always reads from RAM
- CPU uses TLB for actual memory access after driver computes PA

Q6: Can driver modify page tables?
- Yes: write to *__va(table_phys + offset)
- Dangerous: could corrupt system
- Use case: map new memory, change permissions

Q7: What about PCID in CR3?
- Bits [11:0] = PCID on Intel (if CR4.PCIDE=1)
- AMD Ryzen 5 4600H: no PCID, bits [11:0] = 0
- Must mask: CR3 & 0x000FFFFFFFFFF000

Q8: What if accessing another process's page tables?
- Get target task_struct→mm→pgd (virtual address)
- Convert to physical: virt_to_phys(pgd)
- Walk with that CR3 value
- Danger: target could exit, invalidating tables

## DRIVER FEATURES TO IMPLEMENT

1. Walk current process VA → PA
2. Walk target PID VA → PA
3. Dump all page table entries at each level
4. Count 4KB/2MB/1GB pages in mapping
5. Detect huge page thresholds
6. Show flags for each entry (NX, RW, US, etc.)
7. Calculate page table overhead
8. Compare __va() result with manual walk

## OUTPUT FORMAT EXAMPLE

```
/proc/raw_pagewalk output:

VA: 0x7FFE5E4ED123
CR3: 0x10AB4A000 (phys) → 0xFFFF89E00AB4A000 (virt)

L4: PML4[255] = 0x00000002FB845067
    Present=1 RW=1 US=1 PWT=0 PCD=0 A=1 PS=0
    → PDPT @ phys 0x2FB845000

L3: PDPT[511] = 0x00000002FB846063
    Present=1 RW=1 US=0 PWT=0 PCD=0 A=1 PS=0
    → PD @ phys 0x2FB846000

L2: PD[242] = 0x80000002FAE001A1
    Present=1 RW=0 US=0 PWT=0 PCD=0 A=1 PS=1 ← HUGE PAGE
    → 2MB @ phys 0x2FAE00000

OFFSET: 0x15B668 (21 bits)
FINAL PA: 0x2FAF5B668

Walk stats:
  Levels traversed: 3
  Page size: 2 MB
  RAM reads: 3 × 8 = 24 bytes
```

## ALL TERMINAL DATA COLLECTED FROM YOUR MACHINE

### CPU Information
```
$ cat /proc/cpuinfo | grep -E "address sizes|flags" | head -2

address sizes   : 44 bits physical, 48 bits virtual

flags: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov 
       pat pse36 clflush mmx fxsr sse sse2 ht syscall nx mmxext fxsr_opt 
       pdpe1gb rdtscp lm constant_tsc rep_good nopl xtopology nonstop_tsc 
       cpuid extd_apicid aperfmperf rapl pni pclmulqdq monitor ssse3 fma 
       cx16 sse4_1 sse4_2 movbe popcnt aes xsave avx f16c rdrand lahf_lm 
       cmp_legacy svm extapic cr8_legacy abm sse4a misalignsse 3dnowprefetch 
       osvw ibs skinit wdt tce topoext perfctr_core perfctr_nb bpext 
       perfctr_llc mwaitx cpb cat_l3 cdp_l3 hw_pstate ssbd mba ibrs ibpb 
       stibp vmmcall fsgsbase bmi1 avx2 smep bmi2 cqm rdt_a rdseed adx 
       smap clflushopt clwb sha_ni xsaveopt xsavec xgetbv1 cqm_llc 
       cqm_occup_llc cqm_mbm_total cqm_mbm_local clzero irperf xsaveerptr 
       rdpru wbnoinvd cppc arat npt lbrv svm_lock nrip_save tsc_scale 
       vmcb_clean flushbyasid decodeassists pausefilter pfthreshold avic 
       v_vmsave_vmload vgif v_spec_ctrl umip rdpid overflow_recov succor 
       smca sev sev_es

KEY FLAGS:
├── pdpe1gb: 1 GB huge page support ✓
├── pse: 4 MB pages in 32-bit mode ✓
├── nx: No-Execute bit support ✓
├── sev sev_es: AMD Secure Encrypted Virtualization ✓
├── la57: 5-level paging ✗ (ABSENT)
└── pae: Physical Address Extension ✓
```

### Hardware Memory (dmidecode)
```
$ sudo dmidecode -t memory | grep -E "Maximum Capacity|Number Of Devices|Size:|Type:"

Maximum Capacity: 32 GB
Number Of Devices: 2
Size: 8 GB
Type: DDR4
Size: 8 GB
Type: DDR4

SUMMARY:
├── Installed: 2 × 8 GB = 16 GB DDR4
├── Maximum: 32 GB (2 slots × 16 GB max per slot)
├── Usable: 15.04 GB (kernel reports after reservations)
└── Reserved: ~1 GB for GPU, BIOS, MMIO
```

### Memory Information
```
$ cat /proc/meminfo | grep -E "MemTotal|PageTables|HugePages|Hugepagesize"

MemTotal:       15776276 kB     = 15.04 GB
PageTables:     95888 kB        = 93.6 MB (current page table usage)
SecPageTables:  4836 kB         = 4.7 MB (SEV tables)
AnonHugePages:  2048 kB         = 1 × 2MB THP
HugePages_Total: 0              = no preallocated hugepages
Hugepagesize:   2048 kB         = 2 MB default
```

### Zone Information
```
$ cat /proc/zoneinfo | grep -E "^Node|managed|present|spanned"

Node 0, zone DMA:     spanned=4095    present=3999    managed=3842
Node 0, zone DMA32:   spanned=1044480 present=699516  managed=683053
Node 0, zone Normal:  spanned=3339072 present=3339072 managed=3257174
Node 0, zone Movable: spanned=0       present=0       managed=0
Node 0, zone Device:  spanned=0       present=0       managed=0

TOTALS:
├── Present: 3999 + 699516 + 3339072 = 4,042,587 pages
├── Managed: 3842 + 683053 + 3257174 = 3,944,069 pages
├── Reserved: 4,042,587 - 3,944,069 = 98,518 pages = 385 MB
└── Present × 4KB = 16,170,348 KB = 15.43 GB
```

### Kernel Configuration
```
$ grep -E "PGTABLE_LEVELS|X86_5LEVEL" /boot/config-$(uname -r)

CONFIG_PGTABLE_LEVELS=5
CONFIG_X86_5LEVEL=y

MEANING:
├── Kernel compiled for 5-level paging
├── CPU lacks la57 flag → runtime fallback to 4-level
└── P4D layer "folded" (1 entry, passthrough)
```

### Kernel Symbols
```
$ sudo cat /proc/kallsyms | grep "page_offset_base"

ffffffff9695b668 D page_offset_base

MEANING:
├── Symbol location: 0xFFFFFFFF9695B668 (kernel text/data)
├── Type: D = data (not code)
├── Value at runtime: 0xFFFF89DF00000000 (KASLR randomized)
└── Usage: __va(phys) = *page_offset_base + phys
```

### Physical Page Mask
```
$ grep "__PHYSICAL_MASK_SHIFT" /usr/src/linux-headers-$(uname -r)/arch/x86/include/asm/page_64_types.h

#define __PHYSICAL_MASK_SHIFT   52

DERIVED:
├── __PHYSICAL_MASK = (1ULL << 52) - 1 = 0x000FFFFFFFFFFFFF
├── PHYSICAL_PAGE_MASK = PAGE_MASK & __PHYSICAL_MASK = 0x000FFFFFFFFFF000
├── PTE_ADDR_MASK = 0x000FFFFFFFFFF000 (bits [51:12])
└── 52 bits max physical addressing supported by kernel
```

### Live CR3 Values Observed
```
$ cat /proc/pagewalk (multiple reads)

CR3 = 0x2CCB6C000    (process_a)
CR3 = 0x12EB28000    (process_b)
CR3 = 0x18D30E000    (another process)
CR3 = 0x106C42000    (pagewalk itself)
CR3 = 0x10AB4A000    (later observation)
CR3 = 0x1337A3000    (current shell)

OBSERVATIONS:
├── All end in 0x000 (4KB aligned)
├── Range: 0x106C42000 to 0x2CCB6C000 (~4.6 GB spread)
├── Each process has unique PML4 physical address
└── Kernel half (PML4[256:511]) same across all
```

### Direct Map Proof
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
  (CR3 location accessible via direct map)

PROVES: Formula __va(phys) = base + phys WORKS for any physical address
```

### Page Walk Example
```
$ cat /proc/pagewalk (kernel address walk)

VA=0xFFFFFFFF9695B668 → PML4[511] PDPT[510] PD[180] PT[347] OFF=0x668

L4:PML4[511]=0x00000002FB845067 P=Y
  → PDPT @ phys 0x2FB845000

L3:PDPT[510]=0x00000002FB846063 P=Y H=N
  → PD @ phys 0x2FB846000

L2:PD[180]=0x80000002FAE001A1 P=Y H=Y
  → 2MB @ phys 0x2FAE00000

RESULT: VA 0xFFFFFFFF9695B668 → PA 0x2FAF5B668 (2MB huge page)
```

## NUMERICAL CONSTANTS SUMMARY

```
YOUR MACHINE:
├── Physical address bits: 44 (CPU limit: 16 TB)
├── Virtual address bits: 48 (256 TB per process)
├── page_offset_base: 0xFFFF89DF00000000 (KASLR, changes each boot)
├── RAM installed: 16 GB (2 × 8 GB DDR4)
├── RAM usable: 15,776,276 KB = 15.04 GB
├── Present pages: 4,042,587 (×4KB = 15.43 GB)
├── Managed pages: 3,944,069 (×4KB = 15.04 GB)
├── Page table overhead: 95,888 KB = 0.6% of RAM
└── THP in use: 2,048 KB = 1 huge page

MASKS:
├── CR3_ADDR_MASK:      0x000FFFFFFFFFF000 (bits [51:12])
├── PTE_ADDR_MASK:      0x000FFFFFFFFFF000 (bits [51:12])
├── 2MB_ADDR_MASK:      0x000FFFFFFFE00000 (bits [51:21])
├── 1GB_ADDR_MASK:      0x000FFFFFC0000000 (bits [51:30])
├── 4KB_OFFSET_MASK:    0xFFF (12 bits)
├── 2MB_OFFSET_MASK:    0x1FFFFF (21 bits)
└── 1GB_OFFSET_MASK:    0x3FFFFFFF (30 bits)

BIT POSITIONS:
├── Bit 0: Present (P)
├── Bit 1: Read/Write (RW)
├── Bit 2: User/Supervisor (US)
├── Bit 3: Page Write-Through (PWT)
├── Bit 4: Page Cache Disable (PCD)
├── Bit 5: Accessed (A)
├── Bit 6: Dirty (D)
├── Bit 7: Page Size (PS) - 0=table, 1=huge
├── Bit 63: No Execute (NX)
└── Bits [51:12]: Physical address
```
# PLAN AUDIT - AXIOMATIC GAPS

## CLAIM: "512 entries × 8 bytes = 4096 bytes"

WHERE DID 512 COME FROM?
  512 = 2^9
  WHY 9? → 9 bits of virtual address used for each level index
  WHY 9 bits? → 48-bit VA split: 9 + 9 + 9 + 9 + 12 = 48
  WHY 48? → x86_64 4-level paging defines 48-bit virtual
  
WHERE DID 8 COME FROM?
  8 bytes = 64 bits
  WHY 64 bits? → PTE format: 52 bits address + 12 bits flags = 64 bits
  WHY 52 bits address? → __PHYSICAL_MASK_SHIFT = 52 (kernel constant)

DERIVE 4096:
  512 × 8 = ?
  512 = 2^9
  8 = 2^3
  2^9 × 2^3 = 2^(9+3) = 2^12 = 4096 ✓

HARDER: What if 5-level paging?
  57-bit VA: 9 + 9 + 9 + 9 + 9 + 12 = 57
  Still 512 entries per table (9 bits each)
  Still 8 bytes per entry (64-bit format)
  Still 4096 bytes per table ✓

EDGE: What if 32-bit paging?
  32-bit VA: 10 + 10 + 12 = 32
  1024 entries per table (10 bits)
  4 bytes per entry (32-bit format)
  1024 × 4 = 4096 bytes ✓ (same page size, different split)


## CLAIM: "page_offset_base = 0xFFFF89DF00000000"

HOW DO I KNOW THIS IS VALID KERNEL ADDRESS?

TEST: Is bit 47 = 1?
  0xFFFF89DF00000000 in binary:
  1111_1111_1111_1111_1000_1001_1101_1111_0000_0000_0000_0000_0000_0000_0000_0000
  bit 47 = 1 ✓ (kernel space)

TEST: Are bits [63:48] = 0xFFFF (sign extension)?
  bits [63:48] = 0xFFFF ✓

TEST: Is it in direct map range?
  Direct map starts at ~0xFFFF880000000000 (without KASLR)
  0xFFFF89DF00000000 is 0x1DF00000000 = 2.06 TB offset from 0xFFFF880000000000
  This is KASLR randomization (~2 TB offset within range)

DERIVE PML4 INDEX:
  (0xFFFF89DF00000000 >> 39) & 0x1FF = ?
  
  0xFFFF89DF00000000 >> 39:
    = 0xFFFF89DF00000000 / 2^39
    = 0xFFFF89DF00000000 / 549,755,813,888
    
  Let me use hex shift:
    0xFFFF89DF00000000 >> 39 = 0xFFFF89DF00000000 >> 32 >> 7
    >> 32: 0xFFFF89DF
    >> 7: 0xFFFF89DF >> 7 = 0x1FFFF13 (approximately)
    
  Better: 
    39 bits = 9 + 30 bits
    0xFFFF89DF00000000 >> 39:
    Low 39 bits: 0x7F00000000 (if we take low 39 bits of the value)
    
  Actually: Let me compute properly.
    0xFFFF89DF00000000 = -8,239,802,433,536
    But as unsigned: 18,446,502,271,276,015,616
    
    18,446,502,271,276,015,616 >> 39 = 18,446,502,271,276,015,616 / 549,755,813,888
    = 33,552,859 (approximately)
    
    33,552,859 & 0x1FF = 33,552,859 mod 512 = 33,552,859 - 65,533 × 512 = ?
    65,533 × 512 = 33,552,896
    33,552,859 - 33,552,896 = -37 → WRONG
    
  Let me redo with hex:
    0xFFFF89DF00000000 >> 39:
    = 0x1FFFF13BE (if we take the value and shift)
    
  Binary approach:
    0xFFFF89DF = 1111_1111_1111_1111_1000_1001_1101_1111
    bits [47:39] = bits [15:7] of 0xFFFF89DF = 1_0001_0011 = 275
    
    PML4 index = 275 ✓ (direct map region)


## CLAIM: "CR3 = 0x1337A3000, PCID = 0"

WHERE IS PCID?
  PCID = bits [11:0] of CR3
  0x1337A3000 & 0xFFF = 0x000 = 0 ✓

WHERE IS ADDRESS?
  Address = bits [51:12] of CR3
  0x1337A3000 & 0x000FFFFFFFFFF000 = 0x1337A3000 ✓
  
  Physical PML4 at 0x1337A3000

HOW MANY BITS ACTUALLY USED?
  log2(0x1337A3000) = log2(5,163,425,792) = 32.3 bits
  But stored as 40 bits (CR3 field width)
  Actual value uses ~33 bits

HARDER: What if PCID enabled on Intel?
  CR3 = 0x1337A3ABC (with PCID = 0xABC)
  Address = 0x1337A3ABC & 0x000FFFFFFFFFF000 = 0x1337A3000 ✓
  PCID = 0x1337A3ABC & 0xFFF = 0xABC = 2748


## CLAIM: "(vaddr >> 39) & 0x1FF extracts PML4 index"

WHY 39?
  VA bit layout: [47:39][38:30][29:21][20:12][11:0]
  PML4 index = bits [47:39] = 9 bits
  To get bits [47:39], shift right by 39, then mask 9 bits

WHY 0x1FF?
  0x1FF = 0b111111111 = 9 ones = 2^9 - 1 = 511
  Mask to keep only low 9 bits after shift

DERIVE FOR VA = 0x7FFE5E4ED123:

STEP 1: Convert to binary (48 bits)
  0x7FFE5E4ED123 = 
  0111_1111_1111_1110_0101_1110_0100_1110_1101_0001_0010_0011

STEP 2: Extract bits [47:39]
  bits [47:39] = 0_1111_1111 = 0xFF = 255
  
  Verify: (0x7FFE5E4ED123 >> 39) & 0x1FF
  0x7FFE5E4ED123 >> 39 = 0x7FFE5E4ED123 / 2^39
  
  0x7FFE5E4ED123 = 140,734,598,676,771
  2^39 = 549,755,813,888
  140,734,598,676,771 / 549,755,813,888 = 255.99968...
  floor(255.99968) = 255
  255 & 0x1FF = 255 ✓

STEP 3: Extract bits [38:30] = PDPT index
  Shift: 0x7FFE5E4ED123 >> 30 = 140,734,598,676,771 / 1,073,741,824 = 131,071.999
  131,071 & 0x1FF = 131,071 mod 512 = 131,071 - 256 × 512 = 131,071 - 131,072 = -1 → WRONG
  
  Redo: 131,071 / 512 = 255.998...
  255 × 512 = 130,560
  131,071 - 130,560 = 511
  PDPT index = 511 ✓

STEP 4: Extract bits [29:21] = PD index
  Shift: 0x7FFE5E4ED123 >> 21 = 140,734,598,676,771 / 2,097,152 = 67,108,146.96
  67,108,146 & 0x1FF = 67,108,146 mod 512 = ?
  67,108,146 / 512 = 131,070.59
  131,070 × 512 = 67,107,840
  67,108,146 - 67,107,840 = 306 → WAIT
  
  Let me verify with bits:
  bits [29:21] of 0x7FFE5E4ED123:
  Full binary: 0111_1111_1111_1110_0101_1110_0100_1110_1101_0001_0010_0011
  positions:   47                  29        21        12
               0111 1111 1111 1110 0101 1110 0100 1110 1101 0001 0010 0011
                                   ^^^^^^^^^
  bits [29:21] = 1_0010_1111 = 0x12F = 303? No
  
  Let me number correctly:
  position 47 46 45 44 43 42 41 40 39 38 37 36 35 34 33 32 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
  bit      0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1  1  1  0  0  1  0  0  1  1  1  0  1  1  0  1  0  0  0  1  0  0  1  0  0  0  1  1
  
  bits [47:39] = 0_1111_1111 = 255 ✓
  bits [38:30] = 1_1111_1110 = 510? No, should be 511
  
  This is getting confusing. Let me use hex directly.
  
  0x7FFE5E4ED123
  Split: 7 FFE 5E4 ED1 23
  But that's nibble boundaries, not index boundaries.

CORRECT APPROACH:
  Index bit boundaries: 48|39|30|21|12|0
  
  0x7FFE5E4ED123 in groups of 9 from bit 47:
  PML4 [47:39]: need top 9 bits of 48
  
  0x7FFE5E4ED123 = 0x7FFE_5E4E_D123
  
  >> 39: 0x7FFE5E4ED123 >> 39 = 0xFF (255)
  >> 30: 0x7FFE5E4ED123 >> 30 = 0x1FFFF (131,071) → & 0x1FF = 0x1FF (511)
  >> 21: 0x7FFE5E4ED123 >> 21 = 0x3FFFAF2 → & 0x1FF = 0x0F2 (242)
  >> 12: 0x7FFE5E4ED123 >> 12 = 0x7FFE5E4ED → & 0x1FF = 0x0ED (237)
  & 0xFFF: 0x7FFE5E4ED123 & 0xFFF = 0x123 (291)

FINAL:
  PML4: 255, PDPT: 511, PD: 242, PT: 237, offset: 291 ✓


## CLAIM: "2 MB = 2^21 bytes, offset is 21 bits"

DERIVE:
  2 MB = 2 × 1,048,576 = 2,097,152 bytes
  2,097,152 = 2^21 ✓
  
  21 bits can address 0 to 2^21 - 1 = 0 to 2,097,151
  That's exactly 2 MB ✓

HOW DOES 2 MB RELATE TO PAGE TABLE STRUCTURE?
  Normal: PD entry → PT (512 entries) → 512 × 4KB = 2 MB
  Huge:   PD entry → 2 MB directly (skip PT)
  
  512 × 4 KB = 512 × 4,096 = 2,097,152 = 2 MB ✓

HARDER: 1 GB huge page
  1 GB = 1,073,741,824 bytes = 2^30
  PDPT entry → 1 GB directly (skip PD and PT)
  
  Normal: PDPT entry → PD (512) → PT (512 each) → pages (512 each)
  = 512 × 512 × 4 KB = 262,144 × 4,096 = 1,073,741,824 = 1 GB ✓


## CLAIM: "bit 7 = 1 means huge page"

WHY BIT 7?
  PTE format (Intel manual):
  bit 0: Present
  bit 1: R/W
  bit 2: U/S
  bit 3: PWT
  bit 4: PCD
  bit 5: Accessed
  bit 6: Dirty (PT only) / Ignored (PD/PDPT)
  bit 7: PS (Page Size) - 0=points to table, 1=huge page

VERIFY WITH REAL DATA:
  PD[180] = 0x80000002FAE001A1
  
  Low 12 bits = 0x1A1 = 0001_1010_0001
  bit 7 = (0x1A1 >> 7) & 1 = 1 ✓ (huge page)
  bit 0 = 0x1A1 & 1 = 1 ✓ (present)


## GAPS IDENTIFIED - THINGS I SKIPPED

1. WHY is kernel space PML4[256:511] and user PML4[0:255]?
   → Need to derive from canonical address rules

2. WHY does KASLR randomize page_offset_base?
   → Security: attackers can't predict kernel addresses

3. HOW does CPU know when to stop walking?
   → Bit 7 checked at PDPT and PD levels, not at PML4 or PT

4. WHY can't PT entries be huge pages?
   → PT is the final level, entry always points to 4KB data page

5. WHERE do page tables live in physical RAM?
   → Kernel allocates from managed pages, anywhere in RAM

6. HOW does __va() macro work?
   → #define __va(x) ((void *)((unsigned long)(x) + page_offset_base))

7. WHY is PTE_ADDR_MASK = 0x000FFFFFFFFFF000 and not 0xFFFFFFFFFFFFF000?
   → Bits [63:52] reserved for NX, OS use. Must clear them.


## NUMERICAL PUNISHMENT - HARDER EXAMPLES

EXAMPLE 1: Walk VA = 0xFFFF800000000000 (kernel space start)

  (0xFFFF800000000000 >> 39) & 0x1FF = ?
  0xFFFF800000000000 = -140,737,488,355,328 (signed)
  = 18,446,603,336,221,196,288 (unsigned)
  
  >> 39: 18,446,603,336,221,196,288 / 549,755,813,888 = 33,554,432
  & 0x1FF: 33,554,432 mod 512 = 33,554,432 - 65,536 × 512 = 0
  
  Wait: 65,536 × 512 = 33,554,432 exactly
  PML4 index = 0? That's user space!
  
  Let me reconsider. For signed addresses, high bits matter.
  
  Actually: 0xFFFF800000000000 >> 39 in unsigned = 0x1FFFF0
  0x1FFFF0 & 0x1FF = 0x1F0 = 496? No.
  
  0x1FFFF0 = 2,097,136
  2,097,136 mod 512 = 2,097,136 - 4,096 × 512 = 2,097,136 - 2,097,152 = -16 → WRONG
  
  4,095 × 512 = 2,096,640
  2,097,136 - 2,096,640 = 496
  
  PML4 index = 496? But kernel starts at 256...
  
  Let me verify: 0xFFFF800000000000 >> 39 = ?
  In hex: shift right by 39 bits = shift right by 32+7 = shift right 32, then 7
  0xFFFF800000000000 >> 32 = 0xFFFF8000
  0xFFFF8000 >> 7 = 0x1FFFF0
  
  0x1FFFF0 & 0x1FF = 0x1F0 = 496
  
  But 0xFFFF800000000000 is bit 47 = 1, which should give PML4 > 255...
  
  AH! I see the issue. Let's check:
  0xFFFF800000000000 binary:
  1111_1111_1111_1111_1000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000_0000
  
  bits [47:39] = 1_0000_0000 = 256 ✓
  
  So (0xFFFF800000000000 >> 39) & 0x1FF should be 256.
  
  0xFFFF800000000000 >> 39 = 0x1FFFF00 (if we take top bits properly)
  
  Actually the issue is sign extension. Let me use unsigned properly:
  
  0xFFFF800000000000 >> 39 = 0x1FFFF00 (logical shift)
  0x1FFFF00 & 0x1FF = 0x100 = 256 ✓
  
  YES! PML4 index = 256 = kernel space start ✓


EXAMPLE 2: How many 2MB pages fit in 15.04 GB?

  15.04 GB = 15.04 × 1,073,741,824 = 16,148,961,198 bytes
  16,148,961,198 / 2,097,152 = 7,700.3 pages
  
  So ~7,700 PD entries needed (using 2MB huge pages)
  
  Each PD has 512 entries
  7,700 / 512 = 15.04 PD tables needed
  = 16 PD tables (rounded up)


EXAMPLE 3: Physical address bits used in CR3 = 0x2CCB6C000

  0x2CCB6C000 = 12,009,693,184
  log2(12,009,693,184) = 33.5
  
  34 bits needed to represent this address
  But low 12 bits must be 0 (page aligned)
  So 34 - 12 = 22 bits actually stored

  Verify: 0x2CCB6C000 >> 12 = 0x2CCB6C = 2,935,660
  log2(2,935,660) = 21.5 → 22 bits ✓
