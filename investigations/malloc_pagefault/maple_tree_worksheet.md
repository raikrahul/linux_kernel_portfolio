# MAPLE TREE WALK WORKSHEET

1. DRAW `mm->mm_mt` at t=0: `+--maple_tree--+` `| ma_flags=0x0 |` `| ma_lock=0    |` `| ma_root=NULL |` `+--------------+` → VERIFY: ma_root=0x0 ✓ means tree empty, 0 VMAs stored, mmap not called yet. → HOW DID YOU DO THIS? AXIOM 1: `mm_init` (kernel/fork.c) calls `mt_init` which sets `ma_root = NULL`. AXIOM 2: Start state is before `mmap` syscall. ∴ Tree must be empty.

2. CALCULATE vm_end from vm_start: vm_start=0x78d7ce727000, size=4096=0x1000, vm_end=0x78d7ce727000+0x1000=0x78d7ce728000 → VERIFY: 0x78d7ce728000-0x78d7ce727000=0x1000=4096 ✓. → HOW DID YOU DO THIS? AXIOM 1: `mmap` argument 2 is size (4096). AXIOM 2: `vma->vm_end` definition is "first byte AFTER range". ∴ vm_end = vm_start + size. AXIOM 3: vm_start is from live data print (line 88).

3. AXIOM: RAM=array of bytes, address 0 to N → AXIOM: Kernel RAM=addresses >= 0xffff800000000000 on x86_64 → AXIOM: struct in C=contiguous bytes, fields placed sequentially → AXIOM: VMA=Virtual Memory Area, describes ONE contiguous region of process address space → AXIOM: mmap syscall (mm_exercise_user.c:81) triggers kernel to allocate ~200 bytes in kernel RAM for new struct vm_area_struct → SOURCE: include/linux/mm_types.h:649 → OFFSET TABLE: vm_start(offset 0, 8 bytes) + vm_end(offset 8, 8 bytes) + vm_mm(offset 16, 8 bytes) + vm_page_prot(offset 24, 8 bytes) + vm_flags(offset 32, 8 bytes) → DERIVATION of vm_start: mmap(NULL,...) asks kernel to choose address, kernel's get_unmapped_area returns 0x78d7ce727000 (live data from line 88 print) → DERIVATION of vm_end: vm_end=vm_start+size=0x78d7ce727000+0x1000=0x78d7ce728000 ✓ → DERIVATION of vm_flags: PROT_READ(0x1)|PROT_WRITE(0x2) → VM_READ(0x01)+VM_WRITE(0x02)+VM_MAYREAD(0x10)+VM_MAYWRITE(0x20)+VM_MAYEXEC(0x40)=0x01+0x02+0x10+0x20+0x40=0x03+0x70=0x73 ✓ → DRAW: `+--vm_area_struct--+` `| vm_start=0x78d7ce727000 |` `| vm_end=0x78d7ce728000   |` `| vm_flags=0x73           |` `+----------------------+` → WHY THIS STEP: before mmap ma_root=NULL, after mmap ma_root→node→VMA, at page fault mas_walk() searches for this VMA, if VMA not in tree → returns NULL → fault fails ✓. → HOW DID YOU DO THIS? AXIOM 1: `mmap` syscall creates VMA. AXIOM 2: `struct vm_area_struct` definition (mm_types.h). AXIOM 3: `vm_start` from live data. AXIOM 4: `PROT_` constants from `man mmap`. AXIOM 5: `calc_vm_flag_bits` (kernel function) derivation from `mm.h`.

4. AXIOM: ma_root pointer encodes (Address + Type Bits) → SOURCE: lib/maple_tree.c:223 `entry >> MAPLE_NODE_TYPE_SHIFT` → AXIOM: MAPLE_NODE_TYPE_SHIFT=0x03, MAPLE_NODE_TYPE_MASK=0x0F → CALCULATION: If Type=2 (maple_range_64), bits 3-6=0010 → Encoded bits = (2 << 3) = 16 = 0x10. → PREDICTION: ma_root should look like 0xffff888200000016 (assuming some lower bits set or just 0x10) → DECODE EXAMPLE (0xffff888200000010): Node Address = 0xffff888200000010 & ~0xFF = 0xffff888200000000 ✓ → Type Code = (0x10 >> 3) & 0xF = 2 (maple_range_64) ✓ → VERIFY: maple_tree.h:147 `maple_range_64` is index 2. → HOW DID YOU DO THIS? AXIOM 1: Maple Tree structure (maple_tree.h). AXIOM 2: Type encoding macro (lib/maple_tree.c). AXIOM 3: Enum values (maple_tree.h). AXIOM 4: Bitwise shift logic (`x << 3`).

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
