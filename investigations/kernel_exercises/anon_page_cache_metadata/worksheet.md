# ANON VS PAGE CACHE WORK SHEET

## AXIOM: The `struct page` Anonymous Union (Listing 2-3)
01. DEFINITION: `struct page` is packed. Fields reuse memory.
02. AXIOM: `mapping` field (Offset 24) is OVERLOADED.
    - Pointer to `struct address_space` is ALWAYS aligned to 4 bytes (ends in 00).
    - Kernel uses bottom bits for FLAGS.
    - Bit 0 = 1 → `PAGE_MAPPING_ANON` (Anonymous Page).
    - Bit 0 = 0 → File Page (Page Cache).
    - Bit 1 = 1 → `PAGE_MAPPING_MOVABLE` (KSM/Movable).

## EXERCISE 1: The Anonymous Mapping
03. RUN: `./anon_user`. Copy `target_pid` and `anon_addr`.
04. RUN: `sudo insmod anon_file_hw.ko target_pid=... anon_addr=...`.
05. READ dmesg. Only look at "ANONYMOUS" section.

### DERIVATION 1: The Mapping Pointer
06. READ: `raw mapping` = 0x__________.
07. CALCULATE: `bit_0` = `raw_mapping & 1` = __________.
08. CHECK: Is `bit_0` == 1? ✓/✗ (Expected: 1 for Anon).
09. DERIVE: `real_mapping` = `raw_mapping & ~3` (Clear bottom 2 bits).
    - 0x__________ & ~3 = 0x__________.
    - IMPORTANT: If you use `raw_mapping` as a pointer, CPU crashes (Odd Address).
    - `real_mapping` points to `anon_vma` (Kernel Structure).

### DERIVATION 2: The Index
10. READ: `index` = __________.
11. QUESTION: For Anonymous page, what is index?
    - It's the "Linear Page Index" within the VMA.
    - Formula: `(addr - vma->vm_start) >> PAGE_SHIFT` + `vma->vm_pgoff`.
    - Try to verify: `index` should match offset from start of `mmap`.

## EXERCISE 2: The File Mapping
12. LOOK at "FILE-BACKED" section in dmesg.
13. READ: `raw mapping` = 0x__________.
14. CHECK: Is `bit_0` == 0? ✓/✗ (Expected: 0 for File).
15. DERIVE: `real_mapping` = `raw_mapping`. No masking needed!
    - Points to `inode->i_mapping`.
16. READ: `index` = __________.
17. VERIFY: We mapped file at offset 0. Index should be 0.

## EXERCISE 3: The Union (LRU vs Mlock)
18. LOOK at `lru.next` / `lru.prev`.
19. LOOK at `mlock_count`.
20. DISCOVERY: If page is on LRU list, `lru` pointers are valid kernel addresses (0xffff...).
    - `mlock_count` will look like garbage integer (huge number).
21. CHALLENGE: If `anon_user` successfully MLOCKED the page...
    - Does it leave LRU? (Unevictable LRU).
    - Modern approach: Unevictable IS an LRU list.
    - `mlock_count` might be valid inside a *different* struct (page->mlock_count)?
    - Listing 2-3 says: `union { struct list_head lru; ... mlock_count; }`.
    - This implies MUTUALLY EXCLUSIVE.
    - If `mlock_count` is used, `lru` is NOT used?
    - WAIT. Check kernel source `include/linux/mm_types.h` matching 6.14.
    - **CRITICAL**: Sometimes documentation lags behind code.
    - **TASK**: Look at the values. If `lru.next` looks like a pointer, it's LRU.

## FAILURE PREDICTIONS (Don't fall for these!)
22. F1: Using `page->mapping` directly in C code for Anon pages.
    - Result: `Oops: general protection fault`. Address ends in 1.
23. F2: Assuming `file_mapping` has bits set.
    - Result: Usually clean pointer (ends in 0).
24. F3: Mistaking `index` for File Descriptor.
    - Result: Index is *logical page offset*, not FD.

## YOUR MISSION
25. Fill `TODO` blocks in `anon_file_hw.c`.
26. Correctly mask the mapping pointer.
27. Recompile and verify `bit_0` matches expectation.
