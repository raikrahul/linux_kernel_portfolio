```
═══════════════════════════════════════════════════════════════════════════════
DEMO 12: PROCESS CR3 - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Each process has unique page tables
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: CR3 PER PROCESS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Each process has:                                                           │
│   - struct task_struct (process descriptor)                                │
│   - struct mm_struct (memory descriptor, task->mm)                         │
│   - pgd_t *pgd (pointer to PML4, mm->pgd)                                  │
│                                                                             │
│ CR3 value = virt_to_phys(mm->pgd) | PCID                                   │
│                                                                             │
│ DIFFERENT PROCESSES HAVE DIFFERENT CR3:                                    │
│ PID 1 (init):     CR3 = 0x0000000102FAB000 (example)                       │
│ PID 1234 (bash):  CR3 = 0x0000000305DEF000 (different PML4)                │
│ PID 5678 (nginx): CR3 = 0x00000004A1B2C000 (different PML4)                │
│                                                                             │
│ But kernel portions (PML4[256-511]) point to SAME physical pages           │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: READ CR3 OF ANOTHER PROCESS
─────────────────────────────────────────────────────────────────────────────────

FROM KERNEL MODULE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ #include <linux/sched.h>                                                   │
│ #include <linux/mm.h>                                                      │
│ #include <asm/mmu_context.h>                                               │
│                                                                             │
│ struct task_struct *task;                                                  │
│ struct mm_struct *mm;                                                      │
│ unsigned long pgd_phys;                                                    │
│                                                                             │
│ // Find task by PID                                                        │
│ rcu_read_lock();                                                           │
│ task = find_task_by_vpid(target_pid);                                      │
│ if (task && task->mm) {                                                    │
│     mm = task->mm;                                                         │
│     pgd_phys = virt_to_phys(mm->pgd);                                     │
│     printk("PID %d: PML4 phys = 0x%lx\n", target_pid, pgd_phys);          │
│ }                                                                          │
│ rcu_read_unlock();                                                         │
│                                                                             │
│ NOTE: Kernel threads (PID 2, kthreadd) have mm = NULL                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: CONTEXT SWITCH CR3 CHANGE
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Switch from PID 1234 to PID 5678

┌─────────────────────────────────────────────────────────────────────────────┐
│ BEFORE SWITCH:                                                             │
│   CR3 = 0x0000000305DEF000 (PID 1234's PML4)                              │
│   TLB contains translations for PID 1234                                   │
│                                                                             │
│ SWITCH CODE (simplified):                                                  │
│   1. Save current task state (registers, etc)                             │
│   2. Load new task->mm->pgd                                                │
│   3. Write new CR3: mov cr3, new_pgd                                      │
│      This flushes TLB (without PCID)                                       │
│   4. Load new task state                                                   │
│   5. Return to new task                                                    │
│                                                                             │
│ AFTER SWITCH:                                                              │
│   CR3 = 0x00000004A1B2C000 (PID 5678's PML4)                              │
│   TLB is empty (or has stale entries if PCID used)                        │
│   First memory access → TLB miss → page walk                              │
│                                                                             │
│ TIME COST:                                                                 │
│   CR3 write: ~100 cycles                                                   │
│   TLB refill: ~1000s of cycles (amortized over many accesses)             │
│                                                                             │
│ WITH PCID:                                                                 │
│   CR3 = 0x8000000305DEF001 (bit 63 = no flush, bits [11:0] = PCID 1)     │
│   TLB entries tagged with PCID, no flush needed                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: COMPARE TWO PROCESSES
─────────────────────────────────────────────────────────────────────────────────

Given: 
- PID 1234: CR3 = 0x0000000305DEF000
- PID 5678: CR3 = 0x00000004A1B2C000

┌─────────────────────────────────────────────────────────────────────────────┐
│ User space entries (PML4[0-255]):                                          │
│   DIFFERENT for each process                                               │
│   PID 1234 has its own stack, heap, code mappings                         │
│   PID 5678 has different stack, heap, code mappings                       │
│                                                                             │
│ Kernel space entries (PML4[256-511]):                                      │
│   SAME physical pages for ALL processes                                    │
│   PML4[256] → same PDPT physical address                                  │
│   PML4[275] → same PDPT (direct map)                                      │
│   PML4[511] → same PDPT (kernel core)                                     │
│                                                                             │
│ VERIFICATION:                                                               │
│   Read PML4[275] from PID 1234: 0x00000001030AB067                        │
│   Read PML4[275] from PID 5678: 0x00000001030AB067  ← SAME!               │
│                                                                             │
│ This is how syscalls work efficiently:                                     │
│   - No CR3 switch needed on syscall                                       │
│   - Just privilege change (ring 3 → ring 0)                               │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: KERNEL THREAD CR3
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Kernel threads (kworker, ksoftirqd, etc):                                  │
│   task->mm = NULL (no user-space memory)                                   │
│                                                                             │
│ What CR3 do they use?                                                      │
│   - Borrow previous task's mm (mm_struct)                                  │
│   - Use active_mm to track borrowed mm                                     │
│   - Only access kernel space (PML4[256-511])                              │
│   - Any valid CR3 works for kernel-only access                            │
│                                                                             │
│ WHY NOT DEDICATED KERNEL CR3?                                              │
│   - Saves memory (no extra page tables)                                    │
│   - Kernel portion is same in all page tables anyway                      │
│   - Context switch optimization                                            │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 3: CR3 write ~100 cycles, TLB refill ~1000s cycles
Problem 4: Kernel PML4 entries are identical across processes

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. task->mm is NULL for kernel threads → NULL pointer dereference
F2. Not holding rcu_read_lock → task could be freed while reading
F3. Assuming user entries are same → they're different per process
F4. Forgetting virt_to_phys → wrong address
F5. Reading task_struct without lock → race condition
```
