:01. PRE-REQUISITES:
02. 1. Open Terminal #1.
03. 2. Run: `uname -r`. Record result: ___________ (Required: 6.14.0-37-generic).
04. 3. Run: `grep CONFIG_SPARSEMEM /boot/config-$(uname -r)`. Record: ___________ (Required: y).
05. 4. Run: `getconf PAGE_SIZE`. Record (Decimal): ___________ bytes.
06. 5. Calculator ready (hex/bin/dec).

:02. AXIOM 1: THE ATOMIC UNIT & ALIGNMENT (DERIVATION FROM SCRATCH)
07. 1. Value from Line 05 used here (e.g. 4096) → Convert to Power of 2: 2^___ = ________.
08. 2. EXPONENT = ________. This is the **PAGE_SHIFT**.
09. 3. Logic: If one page = 2^12 bytes → Then the lowest 12 bits of any address are the **OFFSET** within the page.
10. 4. Logic: The remaining upper bits (64 - 12 = 52) are the **PAGE FRAME NUMBER (PFN)**.
11. 5. CHECK: 0xFFFFFFFFFFFFFFFF (64 ones) >> 12 = 0x000FFFFFFFFFFFFF. Max PFN = ___________ (calculating...).
12. 6. ACTION: Take arbitrary kernel address `0xffff888123456789`.
13. 7. SPLIT by hand: Binary _________ _________ _________ (Group by 4 bits).
14. 8. MASK lower 12 bits (Offset): `Address & 0xFFF` = 0x________.
15. 9. SHIFT remaining bits right by 12: `Address >> 12` = 0x________.
16. 10. THIS IS THE PFN. Record it: PFN_A = 0x__________.

:03. AXIOM 2: PHYSICAL TO VIRTUAL (LINEAR MAPPING)
17. 1. On x86_64, Kernel Direct Map starts at `PAGE_OFFSET`.
18. 2. Fetch value: `grep "PAGE_OFFSET" /boot/System.map-$(uname -r)`. (May fail if not exported).
19. 3. ALTERNATIVE: Use standard x86_64 constant `0xffff888000000000` (Typical for 4-level paging) or `0xffff474000000000` (5-level).
20. 4. RUN CHECKER: Write small C code to print `PAGE_OFFSET` value (Boilerplate provided later).
21. 5. ASSUME (Verify later with dmesg): `PAGE_OFFSET` = 0xffff888000000000.
22. 6. DERIVATION: `virt_addr = phys_addr + PAGE_OFFSET`.
23. 7. REVERSE: `phys_addr = virt_addr - PAGE_OFFSET`.
24. 8. CALCULATION: Take `0xffff888123456789` from Line 12.
25. 9. SUBTRACT `0xffff888000000000`: Result = 0x__________.
26. 10. THIS IS THE PHYSICAL ADDRESS (PA).
27. 11. VERIFY PFN from PA: `PA >> 12` = 0x__________.
28. 12. COMPARE with Line 16. Match? [ ] Yes [ ] No. (Must match).

:04. AXIOM 3: THE MEMORY MAP ARRAY (STRUCT PAGE)
29. 1. The kernel maintains an array of `struct page` descriptors. One for EVERY physical page.
30. 2. Variable name: `vmemmap` (on x86_64 SPARSEMEM_VMEMMAP).
31. 3. BASE Address: `vmemmap_base` = 0xffffea0000000000 (Typical for 4-level).
32. 4. SIZE of `struct page`: Run C code `sizeof(struct page)`. (Likely 64 bytes).
33. 5. CALCULATION: `page_addr = vmemmap_base + (PFN * sizeof(struct page))`.
34. 6. INPUT: PFN_A from Line 16 (0x123456).
35. 7. MULTIPLY: 0x123456 * 64 (decimal 64 = 0x40 = 2^6).
36. 8. BITWISE SHORTCUT: 0x123456 << 6 = 0x__________.
37. 9. ADD BASE: 0xffffea0000000000 + 0x__________ = 0x__________.
38. 10. THIS IS THE VIRTUAL ADDRESS OF THE STRUCT PAGE.
39. 11. WHY? Accessing fields (flags, refcount) requires this address.

:05. TRAP & SURPRISE 1: THE HOLE
40. 1. Not all physical addresses exist (RAM is not contiguous).
41. 2. PFNs in "holes" are invalid.
42. 3. `pfn_valid(pfn)` check is mandatory.
43. 4. RUN COMMAND: `sudo dmesg | grep "e820: update"`.
44. 5. FIND a gap. (e.g. between end of one range and start of next).
45. 6. PICK a PFN in the gap: PFN_GAP = 0x________.
46. 7. CALCULATE struct page addr for PFN_GAP using formula from Line 33.
47. 8. EXPECTATION: Accessing this address (0x________) should CRASH/FAULT?
48. 9. FACT: With `SPARSEMEM_VMEMMAP`, the *virtual* array is contiguous, but backing pages for holes may be mapped to the "zero page" or invalid?
49. 10. HYPOTHESIS: Reading `flags` from PFN_GAP page. Result = ________? (Safety check needed).

:06. CODING TASK 1: THE VERIFIER
50. 1. Create `mm_exercise_user.c`.
51. 2. Allocate 1 PAGE of memory using `mmap` (Anonymous, Private).
52. 3. Lock it: `mlock`. (Why? Prevent swap out so PFN stays constant).
53. 4. Read `/proc/self/pagemap`.
54. 5. FORMAT: 64-bit value. Bits 0-54 = PFN. Bit 63 = Present.
55. 6. EXTRACT PFN of your allocated page.
56. 7. PRINT: Virtual Address (User), PFN (Decimal/Hex).

:07. CODING TASK 2: THE DRIVER
57. 1. Create `mm_exercise_hw.c`.
58. 2. Define `ioctl` to accept a PFN.
59. 3. INSIDE KERNEL:
60. 4.  Step A: `pfn_to_page(pfn)`.
61. 5.  Step B: Print `page` pointer address. (Real Kernel VA).
62. 6.  Step C: Print `page->flags` (Hex).
63. 7.  Step D: Print `page_ref_count(page)`.
64. 8.  Step E: Calculate EXPECTED `struct page` address by hand (vmemmap formula).
65. 9.  Step F: Compare Real vs Expected. Deviation implies... ?
66. 10. ERROR CHECK: Assume `sizeof(struct page)` != 64. What if 56? Alignment padding?
67. 11. CHECK: `offsetof(struct page, flags)` = 0?
68. 12. CHECK: `offsetof(struct page, _refcount)` = ?

:08. VIOLATION CHECK
69. NEW THINGS INTRODUCED WITHOUT DERIVATION: ____________________.

---

# SESSION SNAPSHOT: 2026-01-02 12:50

## LIVE DATA CAPTURED

| Item | Value | Source |
|------|-------|--------|
| Kernel | 6.14.0-37-generic | `uname -r` |
| PID | 46281 | `pgrep -f mm_exercise` |
| VA (mmap) | 0x7b9312dd6000 | `mm_exercise_user` stdout |
| Target VA | 0x7b9312dd6100 | VA + 0x100 |
| PFN (userspace) | 0x38944c | `/proc/46281/pagemap` |
| PFN (kernel) | 0x38944c | `dmesg` L4 PTE output |
| Phys Addr | 0x38944c100 | (PFN << 12) + 0x100 |
| PAGE_OFFSET | 0xffff8bfc40000000 | `dmesg` |
| RAM Content | "offset world" | Kernel memcpy |

## PAGEMAP ENTRY DECODED

Entry: `0x818000000038944c`
```
Bit 63 = 1 → Present in RAM
Bit 62 = 0 → Not swapped
Bit 61 = 0 → Anonymous (not file)
Bits 0-54 = 0x38944c → PFN
```

## ELF SEGMENT TO PMAP DERIVATION

Binary: `mm_exercise_user`
Command: `readelf -l mm_exercise_user`

| ELF VirtAddr | Flags | + Base (0x56debf103000) | pmap Address | Match |
|--------------|-------|------------------------|--------------|-------|
| 0x0000 | R | + base | 0x56debf103000 | ✓ |
| 0x1000 | R E | + base | 0x56debf104000 | ✓ |
| 0x2000 | R | + base | 0x56debf105000 | ✓ |
| 0x3000 | RW | + base | 0x56debf106000 | ✓ |
| 0x4000 | RW | + base | 0x56debf107000 | ✓ |

Formula: `pmap_address = ASLR_Base + ELF_VirtAddr`

## PMAP OUTPUT (LIVE)

```
00007b9312dd6000       4       4       4 rw---   [ anon ]  ← YOUR MMAP
```
- 4 KB allocated
- 4 KB in RAM (RSS)
- 4 KB dirty (written to)
- rw--- = read+write
- anon = anonymous (not file-backed)

---

# ERROR REPORT #006: Session Confusions

## ERR-A: "Why is pte_t a struct?"
- USER thought: Wrapper is nonsense.
- FACT: Type safety. Prevents `pgd_t` passed where `pte_t` expected.
- SOURCE: `/usr/src/linux-headers-6.14.0-37-generic/arch/x86/include/asm/pgtable_64_types.h:21`
- PROOF: `typedef struct { pteval_t pte; } pte_t;`

## ERR-B: "Why __va() macro?"
- USER thought: Should read physical directly.
- FACT: CPU paging is ON. Must convert Phys→Virtual.
- FORMULA: `kernel_va = phys + PAGE_OFFSET`
- REPLACED: `__va(x)` with `(void*)(x + PAGE_OFFSET)`
- VERIFIED: Same result in dmesg.

## ERR-C: "cat /proc/PID/pagemap"
- USER tried: `cat /proc/46281/pagemap`
- RESULT: Garbage (binary file).
- FIX: Use `dd` + `od` or Python.
- REASON: pagemap is 8-byte binary entries, not text.

## ERR-D: "pgrep mm_exercise_user"
- USER tried: `pgrep mm_exercise_user`
- RESULT: No matches. Process name > 15 chars.
- FIX: `pgrep -f mm_exercise`
- REASON: Linux truncates `/proc/PID/comm` to 15 chars.

## ERR-E: "Why multiple mm_exercise_user in pmap?"
- USER thought: Duplicates.
- FACT: Different ELF segments with different permissions.
- PROOF: `readelf -l mm_exercise_user` shows 4 LOAD segments.
- EACH LOAD → separate mmap with different r/w/x flags.

---

# AXIOMS DERIVED THIS SESSION

## AXIOM: CR2 Register
CPU hardware writes faulting VA to CR2 before calling handler.
Source: `asm/special_insns.h:33-38`

## AXIOM: PAGE_MASK
`PAGE_MASK = ~(PAGE_SIZE - 1) = 0xFFFFFFFFFFFFF000`
Source: `include/vdso/page.h:28`

## AXIOM: current Macro
`current = pcpu_hot.current_task` (per-CPU pointer to running task).
Interrupt does NOT change it.
Source: `asm/current.h:44-52`

## AXIOM: Pagemap Format
`/proc/PID/pagemap` = array of 8-byte entries.
Index = VA >> 12.
Bits 0-54 = PFN. Bit 63 = Present.

## AXIOM: ELF LOAD Segments
Each segment → separate mmap.
Permissions from ELF Flags (R, R E, RW).
Base = random (ASLR).

---

# FORWARD TRACE STATUS

| Step | Status | Data |
|------|--------|------|
| mmap returns VA | ✓ | 0x7b9312dd6000 |
| Write triggers fault | ✓ | "offset world" at +0x100 |
| Kernel allocates page | ✓ | PFN 0x38944c |
| PTE written | ✓ | 0x80000001189cb867 |
| Userspace sees PFN | ✓ | /proc/pagemap matches |
| Kernel reads RAM | ✓ | "offset world" confirmed |

## NOT YET TRACED
- Page fault handler internals (do_page_fault → handle_mm_fault)
- Buddy allocator call
- PTE insertion code

---

# KERNEL PTE FLAGS (Hardware-Defined, Intel x86_64)

Source: `/usr/src/linux-headers-6.14.0-37-generic/arch/x86/include/asm/pgtable_types.h:10-30`

## Bits 0-11: Hardware Flags

| Bit | Name | Value=1 Means | Used For |
|-----|------|---------------|----------|
| 0 | PRESENT | Page is in RAM | CPU checks before access |
| 1 | RW | Writeable | Write protection |
| 2 | USER | Userspace accessible | Ring 3 can access |
| 3 | PWT | Page Write-Through | Cache policy |
| 4 | PCD | Page Cache Disabled | Disable caching |
| 5 | ACCESSED | Page was read | LRU tracking |
| 6 | DIRTY | Page was written | Write-back tracking |
| 7 | PSE | 2MB/4MB page | Huge pages |
| 8 | GLOBAL | TLB not flushed on CR3 change | Kernel pages |
| 9 | SOFTW1 | Available for kernel | SPECIAL flag |
| 10 | SOFTW2 | Available for kernel | UFFD_WP flag |
| 11 | SOFTW3 | Available for kernel | SOFT_DIRTY flag |

## Bits 12-51: PFN (40 bits)

Physical Frame Number. The actual physical page location.

## Bits 52-62: Reserved/Software

| Bit | Name | Used For |
|-----|------|----------|
| 57 | SOFTW4 | DEVMAP |
| 58 | SOFTW5 | SAVED_DIRTY |
| 59-62 | PKEY | Memory Protection Keys |

## Bit 63: NX (No Execute)

| Value | Meaning |
|-------|---------|
| 0 | Page can contain executable code |
| 1 | Execute causes Page Fault (security) |

## Your PTE Decoded

Raw PTE: `0x800000038944c867`

Binary (split):
```
1 | 000000000 | 00000000000111000100101001001100 | 100001100111
^   ^           ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   ^^^^^^^^^^^^
NX  Reserved    PFN (40 bits)                     Flags (12 bits)
```

| Field | Bits | Value | Meaning |
|-------|------|-------|---------|
| NX | 63 | 1 | No Execute |
| PFN | 12-51 | 0x38944c | Physical Frame |
| DIRTY | 6 | 1 | Page was written |
| ACCESSED | 5 | 1 | Page was read |
| USER | 2 | 1 | Userspace can access |
| RW | 1 | 1 | Writeable |
| PRESENT | 0 | 1 | Page in RAM |

Flags 0x867 = binary 100001100111:
```
Bit 0 = 1 (PRESENT)
Bit 1 = 1 (RW)
Bit 2 = 1 (USER)
Bit 5 = 1 (ACCESSED)
Bit 6 = 1 (DIRTY)
Bit 11 = 1 (SOFT_DIRTY)
```

---

:09. NEW TERMS INTRODUCED WITHOUT DERIVATION: None.

# ERROR REPORT #007: SLOPPY BRAIN AUTOPSY

## ERR-F: "Hardcoded Shifts"
- Line: 134-137 (`va >> 39`, `va >> 30`...)
- What went wrong: Assumed 4-level paging (x86_64 legacy).
- What should be: Use kernel macros `PGDIR_SHIFT`, `PUD_SHIFT`, `PMD_SHIFT`.
- Why sloppy: Hardcoding numbers is lazy memory, ignores 5-level paging hardware.
- How to prevent: Grep `include/asm/pgtable_types.h` for macros before typing numbers.

## ERR-G: "Hardcoded PTE Mask"
- Line: 185 (`0x000FFFFFFFFFF000`)
- What went wrong: Manual bitmask construction.
- What should be: `PTE_PFN_MASK` or `pte_pfn()` helper.
- Why sloppy: Reliance on visual hex pattern instead of symbolic constant.
- How to prevent: Always assume specific bits can change (e.g., encryption bits like AMD SME).

## ERR-H: "Missing Huge Page Check"
- Line: 176 (`pte_offset_kernel(pmd, va)`)
- What went wrong: Descended into PMD without checking if it's a leaf.
- What should be: Check `pmd_leaf(*pmd)` or `pmd_large(*pmd)` first.
- Why sloppy: Tunnel vision on 4KB pages.
- How to prevent: Ask "Can this level terminate?" at every node of a tree walk.

## ERR-I: "Unsafe Linear Map Access"
- Line: 202 (`raw_phys + PAGE_OFFSET`)
- What went wrong: Pointer arithmetic on unchecked PFN.
- What should be: Check `pfn_valid(pfn)` AND `pfn_to_online_page` (or similar) before dereferencing.
- Why sloppy: Assumed "if I calculated it, it's RAM". MMIO works differently.
- How to prevent: Treat every physical address as potentially explodable unless validated.

## ERR-J: "Refcount Use-After-Free/Underflow"
- Line: 222 (`put_task_struct(task)` on `current` path)
- What went wrong: Decremented refcount on `current` without incrementing it first.
- What should be: Only call `put` if `get_pid_task` was used.
- Why sloppy: Copy-paste logic from "remote" path to "local" path without adjusting ownership rules.
- How to prevent: Draw ownership graph. If I didn't take it, I don't release it.

## ERR-K: "Implicit Function Blindness"
- Line: 197 (`phys_to_virt` usage)
- What went wrong: Assumed `phys_to_virt` was a global macro.
- What should be: `__va()` (x86_64 standard) or include `asm/io.h` (deprecated/arch-specific).
- Why sloppy: Copied from StackOverflow/AI memory without verifying headers.
- How to prevent: Check `include/linux/` headers for the function declaration before use.

## ERR-L: "Legacy Macro Zombie"
- Line: 161 (`pmd_large` vs `pmd_leaf`)
- What went wrong: Used `pmd_large` (legacy/arch-specific).
- What should be: `pmd_leaf` (Modern generic).
- Why sloppy: Outdated internal knowledge model (pre-5.10 kernel).
- How to prevent: Check `include/linux/pgtable.h` for the latest API.

