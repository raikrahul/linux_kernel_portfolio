# SEPARATE DEMO DRIVER PLANNING

## FOLDER STRUCTURE

```
/home/r/Desktop/lecpp/
├── demo_01_read_cr3/
├── demo_02_extract_indices/
├── demo_03_read_entry/
├── demo_04_check_present/
├── demo_05_check_huge/
├── demo_06_extract_address/
├── demo_07_walk_4kb/
├── demo_08_walk_2mb/
├── demo_09_walk_1gb/
├── demo_10_compare_va/
├── demo_11_dump_pml4/
├── demo_12_process_cr3/
```

---

## DEMO 01: READ CR3

### WHAT
Read CR3 register and output to /proc/demo_cr3

### QUESTIONS TO GRILL

Q1: What instruction reads CR3?
- mov %%cr3, %0 — but WHERE is this documented?
- Intel SDM Volume 3, Chapter 2.5

Q2: Why can't user space read CR3?
- CR3 is privileged register
- Ring 0 only
- What happens if user tries? → #GP fault

Q3: What format should output be?
```
CR3_RAW: 0x00000001337A3000
CR3_ADDR: 0x1337A3000
CR3_PCID: 0x000
BITS_USED: 33
```

Q4: How often does CR3 change?
- Every context switch
- Same process reading twice might see same value
- Different process = different CR3

Q5: Edge case: What if CR3 = 0?
- Invalid, kernel panic at boot
- But module load happens after boot, so impossible?

Q6: What if module loaded in interrupt context?
- CR3 might be from interrupted process
- Should document which process CR3 belongs to

### INPUT/OUTPUT
- INPUT: None (reads current CPU CR3)
- OUTPUT: /proc/demo_cr3 with formatted CR3 data

### FOLDER CONTENTS
```
demo_01_read_cr3/
├── cr3_driver.c
├── Makefile
├── README.md (explains what CR3 is, bit layout)
└── test.sh (insmod, cat /proc/demo_cr3, rmmod)
```

---

## DEMO 02: EXTRACT INDICES

### WHAT
Given a virtual address, extract PML4/PDPT/PD/PT indices and offset

### QUESTIONS TO GRILL

Q1: Where does virtual address come from?
- User provides via /proc write? echo "0x7FFE5E4ED123" > /proc/demo_indices
- Or fixed test address in code?
- Both? First fixed, then user-provided?

Q2: How to validate input?
- Must be 48-bit (or sign-extended 64-bit)
- If bits [63:48] ≠ 0x0000 and ≠ 0xFFFF, invalid
- What error to return?

Q3: What format should output be?
```
VA: 0x7FFE5E4ED123
BINARY: 0111_1111_1111_1110_0101_1110_0100_1110_1101_0001_0010_0011
PML4[47:39]: 255 (0xFF)
PDPT[38:30]: 511 (0x1FF)
PD[29:21]: 242 (0xF2)
PT[20:12]: 237 (0xED)
OFFSET[11:0]: 291 (0x123)
```

Q4: Should we show calculation steps?
- (VA >> 39) & 0x1FF = 255
- Yes, for learning purposes

Q5: Edge cases to test?
- VA = 0x0000000000000000 → all zeros
- VA = 0x7FFFFFFFFFFF → max user space
- VA = 0xFFFF800000000000 → kernel start
- VA = 0xFFFFFFFFFFFFFFFF → max kernel

Q6: Is this kernel space or user space task?
- Pure bit manipulation
- Could be user space program
- But we want kernel module for consistency

### INPUT/OUTPUT
- INPUT: Write VA to /proc/demo_indices
- OUTPUT: Read /proc/demo_indices for results

---

## DEMO 03: READ ENTRY

### WHAT
Given physical table address and index, read the 8-byte entry

### QUESTIONS TO GRILL

Q1: Where does table_phys come from?
- From CR3 (for PML4)
- From previous entry (for PDPT/PD/PT)
- User provides? Dangerous!

Q2: How to convert physical to virtual?
- __va(table_phys) = page_offset_base + table_phys
- What if table_phys > RAM size? → page fault?
- Should validate: table_phys < max_pfn * PAGE_SIZE

Q3: What format should output be?
```
TABLE_PHYS: 0x1337A3000
INDEX: 255
ENTRY_PHYS: 0x1337A3000 + 255×8 = 0x1337A37F8
ENTRY_VIRT: 0xFFFF89E0337A37F8
ENTRY_VALUE: 0x00000002FB845067
```

Q4: What if entry is swapped/not present?
- Still read the 8 bytes
- Interpretation differs (swap address vs physical)

Q5: Memory ordering concerns?
- Should we use READ_ONCE()?
- Page tables can change (TLB shootdown)

Q6: What if index > 511?
- Out of bounds read
- Could read adjacent page
- Must validate: 0 <= index <= 511

### INPUT/OUTPUT
- INPUT: table_phys, index via /proc write
- OUTPUT: entry value and calculation steps

---

## DEMO 04: CHECK PRESENT

### WHAT
Given entry value, check if Present bit (bit 0) is set

### QUESTIONS TO GRILL

Q1: What does Present=0 mean?
- Entry not valid
- If CPU accesses → page fault
- Might be swapped, might be unmapped

Q2: What format?
```
ENTRY: 0x80000002FAE001A1
BIT_0: 1
PRESENT: YES

ENTRY: 0x0000000000000000
BIT_0: 0
PRESENT: NO
```

Q3: Can entry have address but Present=0?
- Yes: 0x80000002FAE001A0
- Address bits set, but P=0
- Kernel uses upper bits for swap info

Q4: Should we decode OTHER flags too?
- bit 1: R/W
- bit 2: U/S
- bit 3: PWT
- bit 4: PCD
- bit 5: A (Accessed)
- bit 6: D (Dirty)
- bit 7: PS
- bit 63: NX

Q5: All flags demo or just Present?
- Just Present for this demo
- Separate demo for all flags?

### INPUT/OUTPUT
- INPUT: Entry value
- OUTPUT: Present status with bit extraction shown

---

## DEMO 05: CHECK HUGE PAGE

### WHAT
Given PD or PDPT entry, check if PS bit (bit 7) indicates huge page

### QUESTIONS TO GRILL

Q1: At which levels is PS bit meaningful?
- PDPT: bit 7 = 1 → 1 GB page
- PD: bit 7 = 1 → 2 MB page
- PML4: bit 7 reserved (must be 0)
- PT: no PS bit (always 4 KB)

Q2: How does caller know which level?
- Module needs context
- User provides level number?

Q3: What format?
```
ENTRY: 0x80000002FAE001A1
LEVEL: PD (2)
BIT_7: (0x1A1 >> 7) & 1 = 1
PS: HUGE PAGE (2 MB)

ENTRY: 0x00000002FB846063
LEVEL: PDPT (3)
BIT_7: (0x063 >> 7) & 1 = 0
PS: POINTS TO TABLE
```

Q4: What if PS=1 in PML4 entry?
- CPU behavior undefined or fault
- Should we detect and warn?

Q5: How to calculate actual physical address?
- If PS=1 at PD: mask low 21 bits
- If PS=1 at PDPT: mask low 30 bits
- Different masks for different levels

### INPUT/OUTPUT
- INPUT: Entry value, level (2=PD, 3=PDPT)
- OUTPUT: PS status, page size

---

## DEMO 06: EXTRACT PHYSICAL ADDRESS

### WHAT
Given entry and level, extract physical address (next table or page)

### QUESTIONS TO GRILL

Q1: Which mask to use?
- Table pointer: bits [51:12] → 0x000FFFFFFFFFF000
- 2 MB page: bits [51:21] → 0x000FFFFFFFE00000
- 1 GB page: bits [51:30] → 0x000FFFFFC0000000

Q2: How to determine mask?
- Need PS bit + level
- PML4: always table (no PS check)
- PDPT: check PS, if 1 → 1 GB, if 0 → table
- PD: check PS, if 1 → 2 MB, if 0 → table
- PT: always 4 KB page (no PS, final level)

Q3: What format?
```
ENTRY: 0x80000002FAE001A1
LEVEL: PD
PS: 1 (2 MB huge)
MASK: 0x000FFFFFFFE00000
ADDRESS_BITS: 0x80000002FAE001A1 & 0x000FFFFFFFE00000 = 0x2FAE00000
PHYSICAL: 0x2FAE00000
```

Q4: Should we show individual bit operations?
- Binary of entry
- Binary of mask
- Binary of result

Q5: Validation?
- Result should be < max physical address
- Result should be properly aligned

### INPUT/OUTPUT
- INPUT: Entry, level
- OUTPUT: Physical address with mask shown

---

## DEMO 07: WALK 4KB PAGE

### WHAT
Complete 4-level walk for virtual address to physical (no huge pages)

### QUESTIONS TO GRILL

Q1: How to find a VA that uses 4KB pages?
- User stack often 4KB
- Heap allocations often 4KB
- How to verify before walk?

Q2: What if walk encounters huge page?
- Abort and report "use demo_08 for 2MB"
- Or auto-detect and switch?

Q3: Step-by-step output format?
```
VA: 0x7FFE5E4ED123
CR3: 0x1337A3000

STEP 1: CR3 → PML4[255]
  table_phys = 0x1337A3000
  index = 255
  entry_addr = 0x1337A3000 + 255×8 = 0x1337A37F8
  entry_virt = __va(0x1337A37F8) = 0xFFFF89E0337A37F8
  entry_value = *( 0xFFFF89E0337A37F8) = 0x00000002FB845067
  present = 1, PS = 0 → continue
  next_table = 0x2FB845000

STEP 2: PDPT[511]
  ...

STEP 3: PD[242]
  ...

STEP 4: PT[237]
  ...
  present = 1, PS = N/A (PT level)
  page_base = 0x????????000

FINAL: 0x????????000 | 0x123 = 0x????????123
```

Q4: How many RAM reads?
- 4 reads × 8 bytes = 32 bytes from RAM

Q5: What if any level not present?
- Report which level failed
- Return error code

### INPUT/OUTPUT
- INPUT: VA
- OUTPUT: Full walk trace with all calculations

---

## DEMO 08: WALK 2MB PAGE

### WHAT
Walk that terminates at PD level (2 MB huge page)

### QUESTIONS TO GRILL

Q1: How to find VA using 2MB page?
- Kernel direct map uses 2MB
- Walk kernel address like page_offset_base + X

Q2: Output format difference?
- Only 3 levels walked
- Offset is 21 bits, not 12
- Show offset extraction differently

Q3: What if PD entry is NOT huge?
- Walk continues to PT
- Report "use demo_07 for 4KB"

### INPUT/OUTPUT
- INPUT: VA (kernel address recommended)
- OUTPUT: 3-level walk trace

---

## DEMO 09: WALK 1GB PAGE

### WHAT
Walk that terminates at PDPT level (1 GB huge page)

### QUESTIONS TO GRILL

Q1: Does your system use 1GB pages?
- pdpe1gb flag present → CPU supports
- Kernel may not use by default
- Need to find/create 1GB mapping

Q2: How to create 1GB huge page?
- echo 1 > /sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages
- mmap with MAP_HUGETLB | MAP_HUGE_1GB
- Then walk that address

Q3: What if no 1GB pages exist?
- Demo can't run
- Report system configuration

### INPUT/OUTPUT
- INPUT: VA using 1GB huge page
- OUTPUT: 2-level walk trace

---

## DEMO 10: COMPARE __VA()

### WHAT
Compare manual direct map calculation with __va() macro result

### QUESTIONS TO GRILL

Q1: What to compare?
- For physical address P:
  - Manual: page_offset_base + P
  - Macro: __va(P)
  - Should be identical

Q2: What format?
```
PHYSICAL: 0x1337A3000
PAGE_OFFSET_BASE: 0xFFFF89DF00000000
MANUAL: 0xFFFF89DF00000000 + 0x1337A3000 = 0xFFFF89E0337A3000
__VA(): 0xFFFF89E0337A3000
MATCH: YES ✓
```

Q3: Edge cases?
- P = 0
- P = max RAM
- P > RAM (what happens?)

### INPUT/OUTPUT
- INPUT: Physical address
- OUTPUT: Comparison of manual vs __va()

---

## DEMO 11: DUMP PML4

### WHAT
Dump all 512 entries of current PML4 table

### QUESTIONS TO GRILL

Q1: Most entries will be 0 (not present). How to display?
- Skip not-present entries?
- Show ranges: "entries 0-255: NOT PRESENT"
- Show all 512 lines? Too much output.

Q2: Format for present entries?
```
PML4[256] = 0x0000000100003067 Present RW User
PML4[257] = 0x0000000100004067 Present RW User
...
PML4[511] = 0x00000002FB845067 Present RW Kern
```

Q3: What insights to derive?
- User space entries: 0-255
- Kernel space entries: 256-511
- Count how many present
- Total page table memory estimate

### INPUT/OUTPUT
- INPUT: None
- OUTPUT: All PML4 entries with analysis

---

## DEMO 12: TARGET PROCESS CR3

### WHAT
Read CR3 from a target process (by PID)

### QUESTIONS TO GRILL

Q1: How to get target process pgd?
- find_task_by_vpid(pid) → task_struct
- task->mm->pgd → virtual address of PML4
- virt_to_phys(pgd) → physical address

Q2: What if process has no mm?
- Kernel threads: task->mm == NULL
- Use task->active_mm instead?

Q3: Locking concerns?
- Process could exit while we read
- RCU? task_lock()? get_task_struct()?

Q4: Security?
- Should require CAP_SYS_PTRACE?
- Any process can read any other's page tables?

Q5: Format?
```
TARGET_PID: 1234
COMM: bash
CR3_PHYS: 0x12345678000
COMPARISON: current CR3 = 0x1337A3000 (different ✓)
```

### INPUT/OUTPUT
- INPUT: PID
- OUTPUT: Target process CR3 and comparison

---

## QUESTIONS ABOUT DEMO ORGANIZATION

Q1: Should each demo build on previous?
- demo_01 output used as input for demo_03?
- Or each self-contained?

Q2: How to run all demos in sequence?
- Master test script?
- Dependency graph?

Q3: Should demos share common code?
- Copy-paste? DRY violation.
- Shared header? More complex build.
- Each standalone for learning? Best for understanding.

Q4: Error handling consistency?
- Same error codes across demos?
- Same /proc interface?

Q5: Documentation format?
- Each folder has README.md?
- Central index.md?
- Man pages?
