```
═══════════════════════════════════════════════════════════════════════════════
DEMO 19: COPY-ON-WRITE - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | RAM = 15406 MB
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: FORK WITHOUT COW - CALCULATE WASTE
─────────────────────────────────────────────────────────────────────────────────

Given: Parent process uses 512 MB of RAM
Given: Parent calls fork()

WITHOUT COW:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Step 1: Allocate new physical memory for child                             │
│   Child RAM = Parent RAM = 512 MB                                          │
│   New pages needed = 512 MB / 4 KB = 512 × 1024 / 4 = _____ pages         │
│   Work: 512 × 1024 = 524288, 524288 / 4 = 131072 pages                    │
│                                                                             │
│ Step 2: Copy all parent pages to child                                     │
│   Bytes to copy = 512 × 1024 × 1024 = 536,870,912 bytes                   │
│   If memcpy speed = 10 GB/s:                                               │
│   Time = 536,870,912 bytes / 10,000,000,000 bytes/s                        │
│        = 0.0536... seconds = 53.6 ms                                       │
│                                                                             │
│ Step 3: Total RAM after fork                                               │
│   Parent = 512 MB                                                           │
│   Child = 512 MB (copy)                                                     │
│   Total = 1024 MB                                                           │
│                                                                             │
│ PROBLEM: Child immediately calls exec("/bin/ls")                           │
│   → All 512 MB just copied gets DISCARDED                                  │
│   → Replaced with /bin/ls memory (maybe 2 MB)                              │
│   → 53.6 ms + 512 MB WASTED                                                │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: FORK WITH COW - CALCULATE SAVINGS
─────────────────────────────────────────────────────────────────────────────────

WITH COW:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Step 1: Do NOT copy pages                                                   │
│   Instead: Mark all parent pages as READ-ONLY (R/W bit = 0)                │
│   Child page tables point to SAME physical pages                           │
│   New pages needed = 0 (for data)                                          │
│                                                                             │
│ Step 2: Copy only page tables (not data)                                   │
│   Page table size = 4 levels × O(pages) entries                            │
│   Upper bound: 4 × 131072 × 8 = 4,194,304 bytes = 4 MB                    │
│   Actual: Sparse tables, much less. Maybe 1-10 KB.                         │
│                                                                             │
│ Step 3: Total RAM after fork                                               │
│   Parent = 512 MB (original)                                                │
│   Child = 0 MB (shares parent pages)                                       │
│   Tables = ~10 KB                                                           │
│   Total = 512 MB + negligible                                              │
│                                                                             │
│ Step 4: Fork time                                                           │
│   Copy tables: ~10 KB / 10 GB/s = 1 μs                                     │
│   Update PTEs: ~131072 entries × ~1ns = 131 μs                             │
│   Total: < 1 ms (vs 53 ms without COW)                                     │
│   SPEEDUP: 50×                                                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: WHAT HAPPENS ON WRITE?
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Child writes to address 0x7FFE5E4ED000

BEFORE WRITE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ PARENT PAGE TABLE:                                                          │
│   VA 0x7FFE5E4ED000 → PTE = 0x00000003FAE00041                             │
│                              ↑                                              │
│                              P=1, R/W=0 (bit 1 = 0)                        │
│   Points to physical page 0x3FAE00000                                      │
│                                                                             │
│ CHILD PAGE TABLE:                                                           │
│   VA 0x7FFE5E4ED000 → PTE = 0x00000003FAE00041 (SAME!)                     │
│                              P=1, R/W=0                                    │
│   Points to SAME physical page 0x3FAE00000                                 │
│                                                                             │
│ VMA for both processes:                                                     │
│   vm_flags = VM_READ | VM_WRITE (intent: should be writable)               │
│   But PTE says R/W=0 (current state: read-only)                            │
│   MISMATCH between VMA (writable) and PTE (read-only) = COW marker        │
└─────────────────────────────────────────────────────────────────────────────┘

WRITE ATTEMPT:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 1. Child executes: *(int*)0x7FFE5E4ED000 = 42                              │
│                                                                             │
│ 2. MMU checks PTE: R/W=0 → Cannot write → Page Fault                       │
│    CPU pushes: CR2 = 0x7FFE5E4ED000, error_code = 0x7 (write, user)       │
│                                                                             │
│ 3. Page fault handler (do_page_fault) runs:                                │
│    a. Find VMA containing 0x7FFE5E4ED000                                   │
│    b. Check: VMA says VM_WRITE? YES                                        │
│    c. Check: PTE says R/W=0? YES                                           │
│    d. CONCLUSION: This is a COW page, not really read-only                 │
│                                                                             │
│ 4. COW handler (do_wp_page):                                                │
│    a. Allocate new physical page from buddy allocator                      │
│       New page: 0x4567890000 (for example)                                  │
│    b. Copy 4096 bytes from 0x3FAE00000 → 0x4567890000                      │
│    c. Update CHILD'S PTE:                                                  │
│       Old: 0x00000003FAE00041 (P=1, R/W=0, phys=0x3FAE00000)              │
│       New: 0x00000456789000 | 0x67 = 0x0000045678900067                    │
│                              (P=1, R/W=1, phys=0x4567890000)              │
│    d. Flush TLB for this address                                           │
│                                                                             │
│ 5. Resume child at faulting instruction                                    │
│    Write *(int*)0x7FFE5E4ED000 = 42 succeeds                               │
│    This writes to 0x4567890000 (child's new private page)                  │
└─────────────────────────────────────────────────────────────────────────────┘

AFTER WRITE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ PARENT PAGE TABLE:                                                          │
│   VA 0x7FFE5E4ED000 → PTE = 0x00000003FAE00041 (UNCHANGED)                 │
│   Still points to 0x3FAE00000                                              │
│   If only 1 reference left, could become R/W again                         │
│                                                                             │
│ CHILD PAGE TABLE:                                                           │
│   VA 0x7FFE5E4ED000 → PTE = 0x0000045678900067 (NEW!)                     │
│   Points to NEW page 0x4567890000                                          │
│   R/W=1, can write freely now                                              │
│                                                                             │
│ RAM USAGE:                                                                  │
│   Before write: 1 page shared                                              │
│   After write:  2 pages (1 parent, 1 child)                                │
│   Incremental cost: 4 KB (just this one page)                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: COUNT THE BITS
─────────────────────────────────────────────────────────────────────────────────

PTE = 0x00000003FAE00041
EXTRACT AND VERIFY:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Low byte = 0x41 = 0100 0001 binary                                         │
│                                                                             │
│ Bit 0 (P):   0x41 & 1 = 1 → Present ✓                                      │
│ Bit 1 (R/W): (0x41 >> 1) & 1 = (32) & 1 = 0 → Read-Only ← COW!            │
│ Bit 2 (U/S): (0x41 >> 2) & 1 = (16) & 1 = 0 → Supervisor? Check.          │
│                                                                             │
│ Wait, let me recalculate 0x41:                                             │
│ 0x41 = 65 decimal = 64 + 1 = 0100_0001 binary                              │
│        bit: 7 6 5 4 3 2 1 0                                                │
│             0 1 0 0 0 0 0 1                                                │
│                                                                             │
│ Bit 0 = 1 ✓ (Present)                                                      │
│ Bit 1 = 0 (R/W=0, Read-Only)                                               │
│ Bit 2 = 0 (U/S=0, Supervisor? Hmm, user pages should have U=1)            │
│ Bit 3 = 0                                                                   │
│ Bit 4 = 0                                                                   │
│ Bit 5 = 0 (Accessed=0)                                                      │
│ Bit 6 = 1 (Dirty=1)                                                         │
│ Bit 7 = 0 (PS=0, 4KB page)                                                 │
│                                                                             │
│ Actually for user COW pages: flags = 0x67 = 0110_0111                      │
│ Bit 0 = 1 (P)                                                               │
│ Bit 1 = 1 (R/W) ← Wait, if COW then R/W should be 0                       │
│                                                                             │
│ Let me use correct COW encoding:                                           │
│ Before COW: original = 0x67 (R/W=1)                                        │
│ After fork: marked = 0x65 (R/W=0) = 0110_0101                              │
│ After COW triggers: child gets new page with 0x67 (R/W=1)                  │
│                                                                             │
│ 0x67 = 103 = 64+32+4+2+1 = 0110_0111                                       │
│ Bits: P=1, R/W=1, U/S=1, PWT=0, PCD=0, A=1, D=1, PS=0                     │
│                                                                             │
│ 0x65 = 101 = 64+32+4+1 = 0110_0101                                         │
│ Bits: P=1, R/W=0, U/S=1, PWT=0, PCD=0, A=1, D=1, PS=0                     │
│            ↑ COW marker: R/W=0 but VMA says writable                       │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Not setting R/W=0 on fork → no COW, pages diverge immediately
F2. Not copying on write → corruption (both processes write same page)
F3. Not flushing TLB after PTE update → stale mapping, crash
F4. Setting R/W=0 but VMA also VM_READ only → can't distinguish COW from real RO
F5. Reference count error → free page while still shared → use-after-free
```
