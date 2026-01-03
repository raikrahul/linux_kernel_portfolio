# MAPLE TREE WALK WORKSHEET

1. DRAW `mm->mm_mt` at t=0: `+--maple_tree--+` `| ma_flags=0x0 |` `| ma_lock=0    |` `| ma_root=NULL |` `+--------------+` → VERIFY: ma_root=0x0 ✓ means tree empty, 0 VMAs stored, mmap not called yet.

2. CALCULATE vm_end from vm_start: vm_start=0x78d7ce727000, size=4096=0x1000, vm_end=0x78d7ce727000+0x1000=0x78d7ce728000 → VERIFY: 0x78d7ce728000-0x78d7ce727000=0x1000=4096 ✓.

3. WHY STEP 3: mmap() creates VMA struct in kernel RAM, this struct is what mas_walk() will find later, without VMA the tree is empty and page fault returns NULL → DRAW VMA struct after mmap: `+--vm_area_struct @ [SIMULATED]--+` `| vm_start = 0x78d7ce727000      |` `| vm_end   = 0x78d7ce728000      |` `| vm_flags = 0x73 [DERIVED]      |` `+--------------------------------+` → DERIVATION of vm_flags: PROT_READ|PROT_WRITE → VM_READ(0x1)+VM_WRITE(0x2)+VM_MAYREAD(0x10)+VM_MAYWRITE(0x20)+VM_MAYEXEC(0x40)=0x1+0x2+0x10+0x20+0x40=0x73 ✓ → VERIFY: 0x78d7ce728000 > 0x78d7ce727000 ✓.

4. DECODE ma_root after mmap: ma_root=0xffff888200000006, node_ptr=0xffff888200000006 & 0xFFFFFFFFFFFFFFF0=0xffff888200000000, type_bits=0xffff888200000006 & 0xF=0x6 → VERIFY: 0x6=MAPLE_LEAF_64 ✓ (from maple_tree.h:144 enum maple_type).

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
