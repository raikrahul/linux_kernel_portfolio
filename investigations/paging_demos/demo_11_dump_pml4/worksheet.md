```
═══════════════════════════════════════════════════════════════════════════════
DEMO 11: DUMP PML4 - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | page_offset_base = 0xFFFF89DF00000000
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: PML4 TABLE STRUCTURE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ PML4 = Page Map Level 4 (top level of 4-level paging)                      │
│                                                                             │
│ Size: 512 entries × 8 bytes = 4096 bytes = 1 page                          │
│                                                                             │
│ Layout:                                                                     │
│ ┌────────────────────────────────────────────────────┐                     │
│ │ PML4[0]    │ 8 bytes │ → User space (VA 0x0...)   │                     │
│ │ PML4[1]    │ 8 bytes │ → User space               │                     │
│ │ ...        │         │                            │                     │
│ │ PML4[255]  │ 8 bytes │ → Last user entry          │                     │
│ │ PML4[256]  │ 8 bytes │ → First kernel entry       │                     │
│ │ ...        │         │                            │                     │
│ │ PML4[511]  │ 8 bytes │ → Last kernel entry        │                     │
│ └────────────┴─────────┴────────────────────────────┘                     │
│                                                                             │
│ User space:   PML4[0-255]   → VA 0x0000000000000000 - 0x00007FFFFFFFFFFF  │
│ Kernel space: PML4[256-511] → VA 0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF  │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: CALCULATE VA RANGE PER PML4 ENTRY
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Each PML4 entry covers = 2^(48-9) = 2^39 bytes = 512 GB                   │
│                                                                             │
│ Derivation:                                                                 │
│ - VA is 48 bits                                                             │
│ - PML4 index uses bits [47:39] = 9 bits                                    │
│ - Remaining bits [38:0] = 39 bits for PDPT+PD+PT+offset                    │
│ - 2^39 = 549,755,813,888 bytes = 512 GB                                    │
│                                                                             │
│ Entry range:                                                                │
│ PML4[0]:   VA 0x0000000000000000 - 0x0000007FFFFFFFFF (512 GB)            │
│ PML4[1]:   VA 0x0000008000000000 - 0x000000FFFFFFFFFF                     │
│ PML4[255]: VA 0x00007F8000000000 - 0x00007FFFFFFFFFFF (last user)         │
│ PML4[256]: VA 0xFFFF800000000000 - 0xFFFF807FFFFFFFFF (first kernel)      │
│ PML4[511]: VA 0xFFFFFF8000000000 - 0xFFFFFFFFFFFFFFFF (last kernel)       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: COUNT PRESENT VS NOT-PRESENT
─────────────────────────────────────────────────────────────────────────────────

TYPICAL OBSERVATION:
┌─────────────────────────────────────────────────────────────────────────────┐
│ User space (0-255):                                                        │
│   - Most entries = 0 (not present)                                         │
│   - A few entries present for:                                             │
│     * Stack (high user addresses, ~PML4[255])                              │
│     * Heap (moderate addresses)                                            │
│     * Code/libraries (low-moderate addresses)                              │
│   - Expected: 5-20 entries present (process dependent)                     │
│                                                                             │
│ Kernel space (256-511):                                                    │
│   - More entries present (kernel is larger)                                │
│   - Direct map region (covers all RAM)                                     │
│   - vmalloc region                                                         │
│   - Module space                                                           │
│   - Expected: 10-50 entries present                                        │
│                                                                             │
│ Kernel entries are SHARED across all processes (same physical pages)       │
└─────────────────────────────────────────────────────────────────────────────┘

EXERCISE: Dump PML4 and count
┌─────────────────────────────────────────────────────────────────────────────┐
│ Present in user space [0-255]:   _____                                     │
│ Present in kernel space [256-511]: _____                                   │
│ Total present: _____                                                        │
│ Total not present: 512 - _____ = _____                                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: DECODE A PML4 ENTRY
─────────────────────────────────────────────────────────────────────────────────

Given: PML4[275] = 0x00000001030AB067

┌─────────────────────────────────────────────────────────────────────────────┐
│ Flags (low 12 bits = 0x067):                                               │
│   0x067 = 103 = 0b01100111                                                 │
│   P   = 1 (present)                                                        │
│   R/W = 1 (writable)                                                       │
│   U/S = 1 (user accessible)                                                │
│   PWT = 0                                                                   │
│   PCD = 0                                                                   │
│   A   = 1 (accessed)                                                       │
│   D   = 1 (dirty) - unusual for PML4, but valid                           │
│   PS  = 0 (must be 0 at PML4 level)                                       │
│                                                                             │
│ Next table physical = entry & 0x000FFFFFFFFFF000                           │
│                     = 0x00000001030AB000                                   │
│   This is the physical address of PDPT                                     │
│                                                                             │
│ Virtual address of PDPT = __va(0x1030AB000)                                │
│                        = 0xFFFF89DF00000000 + 0x1030AB000                  │
│                        = 0xFFFF89E0030AB000                                │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: WHY IS KERNEL MAPPED IN ALL PROCESSES?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ REASON 1: System calls                                                     │
│   Process calls read() → trap to kernel → kernel code runs                │
│   If kernel not mapped → can't execute system call handler                │
│                                                                             │
│ REASON 2: Interrupt handling                                               │
│   Hardware interrupt → CPU switches to kernel                              │
│   Must be able to access kernel code immediately                           │
│                                                                             │
│ REASON 3: Efficiency                                                       │
│   No need to switch page tables on syscall entry                           │
│   Just change privilege level (ring 3 → ring 0)                            │
│                                                                             │
│ PROTECTION:                                                                │
│   Kernel entries have U/S=0 (supervisor only)                              │
│   User code cannot read/write/execute kernel pages                         │
│   Ring 3 access → Page Fault                                               │
│                                                                             │
│ Implementation:                                                             │
│   PML4[256-511] point to SAME physical PDPT pages in all processes        │
│   When fork(): copy user entries, share kernel entries                    │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: Each PML4 entry = 512 GB
Problem 4: Next table at 0x1030AB000, virtual 0xFFFF89E0030AB000

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Assuming all 512 entries used → most are empty
F2. Confusing index with offset (index×8 = offset)
F3. Reading kernel entries from user mode → crash
F4. Assuming U/S=1 in kernel entries → should be 0
F5. Not using __va() before dereferencing → crash
```
