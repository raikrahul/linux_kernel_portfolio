:ERROR REPORT #001
00. MISTAKE: Conflating hardware register (CR3) with kernel's software interface (/proc/self/pagemap).
01. SYMPTOM: Confusion over "54 bits" vs "40 bits".
02. ROOT CAUSE: User assumed pagemap format mirrors CR3 physical address format.
03. FACT: They are SEPARATE. Diagram below.

:DIAGRAM 1: WHY THE CONFUSION?
+--------------------------------------------------------------------------------------------------+
| CONCEPT A: CR3 REGISTER (HARDWARE)                                                               |
| What: Hardware register holding physical address of the Page Global Directory (PGD).             |
| Where: CPU internal register. Read via `movq %cr3, %rax`.                                        |
| Width: Depends on CPU's "address sizes". On THIS machine: 44 bits physical.                      |
| Source: `cat /proc/cpuinfo | grep "address sizes"` → `44 bits physical, 48 bits virtual`.       |
+--------------------------------------------------------------------------------------------------+
                  |
                  |  ≠ NOT EQUAL
                  |
+--------------------------------------------------------------------------------------------------+
| CONCEPT B: /proc/self/pagemap (KERNEL SOFTWARE INTERFACE)                                        |
| What: A file interface to READ page table entries from userspace. Defined by KERNEL.             |
| Where: Kernel source `fs/proc/task_mmu.c`.                                                       |
| Width: Kernel defines `PM_PFRAME_BITS = 55`. Mask = `GENMASK_ULL(54, 0)` = bits 0-54.            |
| Source: `/home/r/Desktop/learn_kernel/source/linux/fs/proc/task_mmu.c` Line 1847.                |
+--------------------------------------------------------------------------------------------------+

:DIAGRAM 2: THE 64-BIT PAGEMAP ENTRY LAYOUT (KERNEL-DEFINED)
+----+----+----+----+----+----+----+----+----+----+ ... +----+----+----+----+
| 63 | 62 | 61 | 60 | 59 | 58 | 57 | 56 | 55 | 54 | ... |  1 |  0 |        |
+----+----+----+----+----+----+----+----+----+----+ ... +----+----+----+----+
|PRES|SWAP|FILE| R  | R  |GUAR|WFFD| EX |SOFT|                PFN          |
+----+----+----+----+----+----+----+----+----+-----------------------------+
Bit 63: PM_PRESENT (Page in RAM).
Bit 62: PM_SWAP (Page in swap).
Bit 61: PM_FILE (Page is file-backed).
Bit 58: PM_GUARD_REGION.
Bit 57: PM_UFFD_WP (Userfaultfd write-protected).
Bit 56: PM_MMAP_EXCLUSIVE.
Bit 55: PM_SOFT_DIRTY.
Bits 0-54: PFN (Page Frame Number).

:DERIVATION: WHY 55 BITS FOR PFN?
04. AXIOM: Page Size = 4096 = 2^12.
05. FACT: Max User Virtual Address (48-bit) / Page Size = 2^48 / 2^12 = 2^36 max VPNs.
06. FACT: Kernel designers chose 55 bits to allow for FUTURE expansion of physical memory.
07. FACT: 55 bits for PFN → Max Addressable = 2^55 * 4096 = 2^67 bytes.
08. FACT: This machine has 44 physical address bits → Max RAM = 2^44 = 16 TB.
09. FACT: PFN on this machine fits in 44-12 = 32 bits. Bits 32-54 of PFN are zeros.
10. ∴ The 55-bit field is a SOFTWARE generalization. It is NOT tied to THIS CPU's CR3.

:NUMERICAL EXAMPLE (THIS MACHINE)
11. CPU Physical Bits: 44.
12. Max Physical Address: 2^44 - 1 = 0xFFFFFFFFFFF.
13. Max PFN = Max Phys >> 12 = 0xFFFFFFFFFFF >> 12 = 0xFFFFFFFF (32 bits).
14. In pagemap entry: PFN fits in bits 0-31. Bits 32-54 = 0.
15. Entry example: 0x8180000000123456.
16. Bit 63 = 1 → Present.
17. Bits 0-54 = 0x0000000000123456 → PFN = 0x123456.

:YOUR MISTAKE LOG
18. CARELESS READING: Assumed hardware defines pagemap format. ✗.
19. SLOPPY INFERENCE: Confused CR3 (phys addr of page table) with pagemap entry (kernel abstraction). ✗.
20. MISSED DOCUMENTATION: Did not read `task_mmu.c` which defines the format. ✗.
21. MUMBO JUMBO TYPING: Writing code without understanding the 55 vs 44 distinction. ✗.
22. FIX: Always trace constant definitions back to kernel source.

:CONVERSATION SNAPSHOT (2026-01-01 16:34)
:TOPIC: PFN vs PTE vs Pagemap bit widths

:LIVE DATA FROM THIS MACHINE
23. `cat /proc/cpuinfo | grep "address sizes"` → 44 bits physical, 48 bits virtual.

================================================================================
:AXIOMATIC DERIVATION - REVERSE MAPPING (struct page → VMA) (2026-01-01 19:48)
================================================================================

:GOAL: Given a struct page, find the VMA that allocated it.
:CONSTRAINT: Start from primitives. Each line uses ONLY prior lines.

:AXIOM A0: Byte = 8 bits. Address = 64-bit integer on x86_64.
:AXIOM A1: Pointer = 64-bit value holding a memory address.
:AXIOM A2: Struct = contiguous block of bytes at some address.
:AXIOM A3: Struct field = bytes at (struct address + offset).

24. struct page = kernel struct describing ONE 4096-byte physical page.
25. Source: mm_types.h:78. struct page { memdesc_flags_t flags; union { ... } }
26. struct page has field `mapping` at some offset.
27. `mapping` is type: struct address_space* (pointer, 8 bytes).

:DERIVATION OF MAPPING FIELD TRICK
28. When kernel allocates an anonymous page (heap, stack, malloc):
29. Kernel stores pointer to `struct anon_vma` in `page->mapping`.
30. BUT kernel sets Bit 0 = 1 to signal "this is Anonymous, not File".
31. Proof: page-flags.h:717. #define FOLIO_MAPPING_ANON 0x1

:DERIVATION STEP BY STEP
32. Let page = pointer to struct page = 0xffffea0001234000.
33. Let page->mapping = 0xffff888012345681 (ODD, Bit 0 = 1).
34. Check: 0xffff888012345681 & 1 = 1. ∴ Anonymous page.
35. Remove Bit 0: 0xffff888012345681 & ~1 = 0xffff888012345680.
36. Cast: (struct anon_vma*)0xffff888012345680.
37. NOW we have pointer to struct anon_vma.

:DEFINITION OF struct anon_vma (SOURCE: rmap.h:32-68)
38. struct anon_vma {
39.   struct anon_vma *root;         // Offset 0, 8 bytes. Points to tree root.
40.   struct rw_semaphore rwsem;     // Offset 8, ~40 bytes. Lock.
41.   atomic_t refcount;             // Offset ~48, 4 bytes. Reference count.
42.   unsigned long num_children;    // Offset ~52, 8 bytes. Child count.
43.   unsigned long num_active_vmas; // Offset ~60, 8 bytes. VMA count.
44.   struct anon_vma *parent;       // Offset ~68, 8 bytes. Parent pointer.
45.   struct rb_root_cached rb_root; // Offset ~76, 16 bytes. Tree of VMAs.
46. }

:DEFINITION OF rb_root_cached
47. rb_root_cached = struct { struct rb_node *rb_node; struct rb_node *rb_leftmost; }
48. rb_node = pointer to root of Red-Black tree.
49. Red-Black tree = balanced binary search tree. O(log N) lookup.
50. Each node in tree is: struct anon_vma_chain.

:DEFINITION OF struct anon_vma_chain (SOURCE: rmap.h:83-92)
51. struct anon_vma_chain {
52.   struct vm_area_struct *vma;    // Offset 0, 8 bytes. ← THIS IS THE VMA!
53.   struct anon_vma *anon_vma;     // Offset 8, 8 bytes. Back-pointer.
54.   struct list_head same_vma;     // Offset 16, 16 bytes. List node.
55.   struct rb_node rb;             // Offset 32, 24 bytes. Tree node.
56.   unsigned long rb_subtree_last; // Offset 56, 8 bytes. Interval tree.
57. }

:FULL TRACE (PAGE → VMA)
58. START: struct page* page = 0xffffea0001234000.
59. STEP 1: Read page->mapping = 0xffff888012345681.
60. STEP 2: Check (mapping & 1) == 1. YES → Anonymous page.
61. STEP 3: anon_vma ptr = mapping & ~1 = 0xffff888012345680.
62. STEP 4: struct anon_vma* av = (struct anon_vma*)0xffff888012345680.
63. STEP 5: Read av->rb_root.rb_node (offset ~76).
64. STEP 6: Walk Red-Black tree. Each node is anon_vma_chain.
65. STEP 7: Read avc->vma (offset 0 of anon_vma_chain).
66. STEP 8: avc->vma = 0xffff8880abcdef00. ← THIS IS THE VMA.

:SUMMARY CHAIN
page → mapping (& ~1) → anon_vma → rb_root → anon_vma_chain → vma

================================================================================
:ERROR REPORT #005 - SLOPPY BRAIN ANALYSIS (2026-01-01 20:51)
================================================================================

:MISTAKE 01: "how does one go from pte fro the vma"
  Line: User Prompt Step 461.
  Wrong: Assumed PTE contains metadata or pointers to VMA.
  Right: PTE is a u64 integer. Contains ONLY PFN + Flags. No back-pointers.
  Sloppy: Did not look at PTE definition (arch/x86/include/asm/pgtable_64.h).
  Prevent: "grep" struct/typedef definitions before assuming fields exist.

:MISTAKE 02: "how does one know for a given struct page what was the vma"
  Line: User Prompt Step 461.
  Wrong: Confused "page made by VMA" with "page mapped by VMA".
  Right: Anon pages created by Fault → anon_vma. File pages created by FS → address_space.
  Sloppy: Ignoring `page->mapping` semantics defined in mm_types.h.
  Prevent: Read struct definitions. `struct page` has `mapping`. Tracing `mapping` finds VMA.

:MISTAKE 03: "you are introdcuing new ideas without deriving them"
  Line: User Prompt Step 464.
  Wrong: Rejection of necessary vocabulary (`anon_vma`).
  Right: Impossible to explain "Page → VMA" without `anon_vma`. It IS the link.
  Sloppy: Thinking explanation is "magic" rather than "data structure traversal".
  Prevent: Accept that new Structs = New Axioms.

:MISTAKE 04: "check" (Disk Full)
  Line: User Prompt Step 513.
  Wrong: Ignoring write errors.
  Right: `cat: write error: No space left on device`.
  Sloppy: Blindly moving forward when filesystem screams STOP.
  Prevent: Read error logs. 

:ORTHOGONAL THOUGHT PROCESS
  State 0: "I want to find VMA from Page."
  Fact 0: Page does not store VMA pointer directly (too expensive, 8 bytes per page).
  Fact 1: Page stores `mapping` (shared).
  Fact 2: `mapping` must point to something that points to VMA.
  Deduction: `mapping` → [Intermediate Struct] → VMA.
  Name it: Intermediate Struct = `anon_vma` (Anon) or `address_space` (File).
  Conclusion: No magic. Just indirection to save memory.

