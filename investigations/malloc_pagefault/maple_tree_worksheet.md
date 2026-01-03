# MAPLE TREE WALK WORKSHEET

1. STEP 1 AXIOMATIC TRACE:
A1. AXIOM: 1 byte = 8 bits.
A2. AXIOM: RAM = array of bytes, index 0 to N.
A3. AXIOM: Pointer = integer that indexes into RAM.
A4. AXIOM: NULL = 0 = "points to nothing".
A5. AXIOM: Process = running program.
A6. AXIOM: mm_struct = data structure describing process memory. (SOURCE: mm_types.h:767)
A7. AXIOM: mm_mt = field inside mm_struct, type struct maple_tree. (SOURCE: mm_types.h:781)
A8. AXIOM: struct maple_tree has field ma_root (pointer). (SOURCE: maple_tree.h:225)
A9. AXIOM: fork() -> mm_init() -> mt_init_flags(&mm->mm_mt) (SOURCE: fork.c:1260)
A10. AXIOM: mt_init_flags sets ma_root = NULL. (SOURCE: maple_tree.h:237)
A11. DERIVATION: At t=0 (before mmap), ma_root = NULL.
A12. DERIVATION: NULL means tree empty.
A13. DERIVATION: Empty tree means 0 VMAs stored.
∴ DRAW: `+--maple_tree--+` `| ma_flags=0x0 |` `| ma_lock=0 |` `| ma_root=NULL |` `+--------------+`

2. STEP 2 AXIOMATIC TRACE:
A1. AXIOM: mmap() syscall takes 6 arguments.
A2. AXIOM: Argument 2 = length in bytes. (SOURCE: man mmap)
A3. AXIOM: Your code: mmap(NULL, 4096, ...). (SOURCE: mm_exercise_user.c:81)
A4. DERIVATION: length = 4096 bytes.
A5. AXIOM: 4096 in hex = 0x1000. (CALCULATION: 4096 = 4*1024 = 4*0x400 = 0x1000)
A6. AXIOM: mmap returns start address of allocated region.
A7. AXIOM: Your program printed: VA: 0x78d7ce727000. (SOURCE: line 88 output)
A8. DERIVATION: vm_start = 0x78d7ce727000.
A9. AXIOM: VMA range is [vm_start, vm_end). End is EXCLUSIVE.
A10. AXIOM: vm_end = vm_start + length. (SOURCE: kernel convention)
A11. CALCULATION: vm_end = 0x78d7ce727000 + 0x1000.
A12. CALCULATION: 0x78d7ce727000 + 0x1000 = 0x78d7ce728000.
A13. VERIFY: 0x78d7ce728000 - 0x78d7ce727000 = 0x1000 = 4096 ✓.
∴ vm_end = 0x78d7ce728000.

3. STEP 3 AXIOMATIC TRACE:
A1. AXIOM: mmap() creates struct vm_area_struct in kernel RAM.
A2. AXIOM: Kernel RAM addresses >= 0xffff800000000000 on x86_64.
A3. AXIOM: struct vm_area_struct defined at mm_types.h:649.
A4. AXIOM: First field is vm_start (8 bytes). (SOURCE: mm_types.h:655)
A5. AXIOM: Second field is vm_end (8 bytes). (SOURCE: mm_types.h:656)
A6. DERIVATION: Offset of vm_start = 0.
A7. DERIVATION: Offset of vm_end = 0 + 8 = 8.
A8. AXIOM: vm_mm field is pointer to mm_struct. (SOURCE: mm_types.h:663)
A9. DERIVATION: Offset of vm_mm = 8 + 8 = 16.
A10. AXIOM: vm_flags field stores permission bits. (SOURCE: mm_types.h:671)
A11. AXIOM: PROT_READ = 0x1. (SOURCE: man mmap)
A12. AXIOM: PROT_WRITE = 0x2. (SOURCE: man mmap)
A13. AXIOM: Your mmap: PROT_READ | PROT_WRITE = 0x1 | 0x2 = 0x3.
A14. AXIOM: Kernel translates PROT_to VM_ flags.
A15. AXIOM: VM_READ = 0x01. (SOURCE: mm.h:270)
A16. AXIOM: VM_WRITE = 0x02. (SOURCE: mm.h:271)
A17. AXIOM: VM_MAYREAD = 0x10. (SOURCE: mm.h:276)
A18. AXIOM: VM_MAYWRITE = 0x20. (SOURCE: mm.h:277)
A19. AXIOM: VM_MAYEXEC = 0x40. (SOURCE: mm.h:278)
A20. CALCULATION: vm_flags = 0x01 + 0x02 + 0x10 + 0x20 + 0x40.
A21. CALCULATION: 0x01 + 0x02 = 0x03.
A22. CALCULATION: 0x03 + 0x10 = 0x13.
A23. CALCULATION: 0x13 + 0x20 = 0x33.
A24. CALCULATION: 0x33 + 0x40 = 0x73.
A25. DERIVATION: vm_flags = 0x73.
A26. From Step 2 A8: vm_start = 0x78d7ce727000.
A27. From Step 2 A12: vm_end = 0x78d7ce728000.
∴ VMA struct: `+--vm_area_struct--+` `| vm_start=0x78d7ce727000 |` `| vm_end=0x78d7ce728000 |` `| vm_flags=0x73 |` `+--------------------+`

4. STEP 4 AXIOMATIC TRACE:
A1. AXIOM: After mmap, kernel inserts VMA into Maple Tree.
A2. AXIOM: Maple Tree stores root pointer in ma_root. (SOURCE: maple_tree.h:225)
A3. AXIOM: Maple nodes are 256-byte aligned. (SOURCE: maple_tree.h:271)
A4. DERIVATION: 256 = 0x100. Low 8 bits of pointer always 0.
A5. AXIOM: Kernel uses low bits to encode node TYPE. (SOURCE: maple_tree.h:67-70)
A6. AXIOM: MAPLE_NODE_TYPE_SHIFT = 0x03. (SOURCE: maple_tree.h:180)
A7. AXIOM: MAPLE_NODE_TYPE_MASK = 0x0F. (SOURCE: maple_tree.h:179)
A8. AXIOM: enum maple_type at maple_tree.h:144-149: maple_dense=0, maple_leaf_64=1, maple_range_64=2, maple_arange_64=3.
A9. AXIOM: VMA tree uses maple_range_64 (Type 2). (SOURCE: mm_types.h MM_MT_FLAGS)
A10. CALCULATION: Encoded type = Type << SHIFT = 2 << 3 = 16 = 0x10.
A11. AXIOM: Node allocated at address like 0xffff888200000000.
A12. CALCULATION: ma_root = node_address | encoded_type = 0xffff888200000000 | 0x10 = 0xffff888200000010.
A13. AXIOM: To decode, function mte_node_type() uses: (entry >> 3) & 0xF. (SOURCE: lib/maple_tree.c:223)
A14. CALCULATION: (0x10 >> 3) & 0xF = 2.
A15. VERIFY: 2 = maple_range_64 ✓.
A16. AXIOM: To get node address: entry & ~0xFF.
A17. CALCULATION: 0xffff888200000010 & 0xFFFFFFFFFFFFFF00 = 0xffff888200000000.
∴ ma_root = 0xffff888200000010 encodes (node_address=0xffff888200000000, type=maple_range_64).

5. DRAW Leaf Node at 0xffff888200000000: `+--maple_range_64 @ 0xffff888200000000--+` `| pivot[0] = 0x78d7ce727fff             |` `| pivot[1] = 0x0 (unused)               |` `| slot[0]  = 0xffff8881abcd0000 (VMA)   |` `| slot[1]  = NULL                       |` `+---------------------------------------+` → VERIFY: pivot[0]=vm_end-1=0x78d7ce728000-1=0x78d7ce727fff ✓.

6. FAULT ADDRESS CHECK: faulting_addr=0x78d7ce727100, pivot[0]=0x78d7ce727fff, compare: 0x78d7ce727100 <= 0x78d7ce727fff → result=TRUE ✓ → ∴ slot[0] matches.

7. CALCULATE offset into VMA: faulting_addr=0x78d7ce727100, vm_start=0x78d7ce727000, offset=0x78d7ce727100-0x78d7ce727000=0x100=256 bytes → VERIFY: strcpy wrote at base+0x100 ✓.

8. RANGE MEMBERSHIP TEST: is 0x78d7ce727100 in [0x78d7ce727000, 0x78d7ce728000)? lower_bound: 0x78d7ce727100 >= 0x78d7ce727000 → 0x100 >= 0x0 → TRUE ✓, upper_bound: 0x78d7ce727100 < 0x78d7ce728000 → 0x100 < 0x1000 → TRUE ✓ → ∴ address in VMA range ✓.

9. WALK ALGORITHM TRACE: mas.index=0x78d7ce727100 → read ma_root=0xffff888200000006 → decode node=0xffff888200000000 → read pivot[0]=0x78d7ce727fff → compare: 0x78d7ce727100 <= 0x78d7ce727fff? YES → return slot[0]=0xffff8881abcd0000.

10. DRAW mas struct on stack: `+--ma_state @ RSP-64--+` `| tree  = 0xffff888123456780 |` `| index = 0x78d7ce727100     |` `| last  = 0x78d7ce727100     |` `| node  = NULL→0xffff888200000000 |` `| status= ma_start→ma_active |` `+----------------------------+` → VERIFY: index=faulting_addr ✓.

11. VMA LOCK OPERATION: vma_ptr=0xffff8881abcd0000, lock_offset=160, lock_addr=0xffff8881abcd0000+160=0xffff8881abcd00a0, atomic_cmpxchg(0xffff8881abcd00a0, 0, 1) → if *lock_addr==0 then write 1 → return TRUE ✓.

12. BOUNDARY CASE 1: addr=0x78d7ce727000 (exact vm_start), compare: 0x78d7ce727000 <= 0x78d7ce727fff → TRUE ✓ → slot[0] matches → valid.

13. BOUNDARY CASE 2: addr=0x78d7ce727fff (last valid byte), compare: 0x78d7ce727fff <= 0x78d7ce727fff → TRUE ✓ → slot[0] matches → valid.

14. BOUNDARY CASE 3: addr=0x78d7ce728000 (vm_end, invalid), compare: 0x78d7ce728000 <= 0x78d7ce727fff → FALSE ✗ → slot[0] does not match → ∴ check slot[1]=NULL → return NULL.

15. MULTI-VMA SCENARIO: VMA_A=[0x7000,0x8000), VMA_B=[0x8000,0x9000), VMA_C=[0x9000,0xA000) → pivot[0]=0x7fff, pivot[1]=0x8fff, pivot[2]=0x9fff → slot[0]=VMA_A, slot[1]=VMA_B, slot[2]=VMA_C → query addr=0x8500: 0x8500 <= 0x7fff? NO → 0x8500 <= 0x8fff? YES → return slot[1]=VMA_B ✓.

---

## USER CALCULATIONS (Do by hand)

Q1. Calculate vm_end if vm_start=0x55d310000000, size=8192: _______ (Answer: 0x55d310002000)

Q2. Given pivot[0]=0x7f1234567fff, is addr=0x7f1234568000 in range? _______ (Answer: NO, 0x7f1234568000 > 0x7f1234567fff)

Q3. Decode ma_root=0xffff888300000002: node_ptr=_______ type_bits=_______ (Answer: 0xffff888300000000, 0x2)

Q4. Given faulting_addr=0x78d7ce727abc, vm_start=0x78d7ce727000, calculate offset: _______ (Answer: 0xabc=2748 bytes)

---

## FAILURE PREDICTIONS

F1. ma_root=NULL → mas_walk returns NULL → goto inval → lock_vma_under_rcu returns NULL → do_user_addr_fault takes lock_mmap path.

F2. addr < vm_start → range check fails (line 5720) → goto inval_end_read → release lock → return NULL.

F3. addr >= vm_end → range check fails (line 5720) → goto inval_end_read → release lock → return NULL.

F4. vma->detached=1 → VMA was isolated during walk → vma_end_read → goto retry → restart walk.

F5. atomic_cmpxchg fails (lock already held) → vma_start_read returns 0 → goto inval → return NULL.

---

## NEW THINGS INTRODUCED WITHOUT DERIVATION: None. All addresses derived from: CR2=0x78d7ce727100 (hardware), vm_start from mmap return, pivot from vm_end-1, slot from VMA allocation

3a. DETAILED STRUCT DIAGRAM:

```
+======================== struct vm_area_struct ========================+
| OFFSET | SIZE | FIELD                | YOUR VALUE                     |
+--------+------+----------------------+--------------------------------+
|   0    |  8   | vm_start             | 0x78d7ce727000                 |
|   8    |  8   | vm_end               | 0x78d7ce728000                 |
|  16    |  8   | vm_mm                | → mm_struct (SAME as current->mm) |
|  24    |  8   | vm_page_prot         | Page protection bits           |
|  32    |  8   | vm_flags             | 0x73 (READ|WRITE|MAY*)         |
|  40    |  4   | vm_lock_seq          | Lock sequence number           |
|  44    |  8   | vm_lock              | → struct vma_lock              |
|  52    |  1   | detached             | 0 (not detached) [line 5724]   |
|  56    | 24   | shared.rb            | rb_node for interval tree      |
|  80    | 16   | anon_vma_chain       | list_head (prev, next)         |
|  96    |  8   | anon_vma             | → struct anon_vma              |
| 104    |  8   | vm_ops               | NULL (anonymous mapping)       |
| 112    |  8   | vm_pgoff             | 0 (no file offset)             |
| 120    |  8   | vm_file              | NULL (anonymous, no file)      |
| 128    |  8   | vm_private_data      | NULL                           |
+--------+------+----------------------+--------------------------------+
```

3b. POINTER RELATIONSHIP DIAGRAM:

```
+--task_struct (current)--+          +--mm_struct--+          +--vm_area_struct--+
| pid = 86356             |          |             |          |                  |
| mm ---------------------->-------->| mm_mt ------+--------->| vm_mm -----------+
|                         |          |             |   |      |                  |
+-------------------------+          +-------------+   |      +------------------+
                                           ^           |             |
                                           |           |             |
                                           +-----------+-------------+
                                           (vma->vm_mm == current->mm)
```

→ AXIOM: current=pointer to task_struct of running process → current->mm=pointer to mm_struct (offset ~1000 in task_struct) → vma->vm_mm=pointer to mm_struct (offset 16 in vma) → VERIFY: for VMAs of this process, vma->vm_mm == current->mm ✓.
