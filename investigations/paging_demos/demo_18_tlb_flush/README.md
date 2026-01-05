```
═══════════════════════════════════════════════════════════════════════════════
DEMO 18: TLB FLUSH - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | 6 cores × 2 threads = 12 logical CPUs
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: TLB MISS PENALTY CALCULATION
─────────────────────────────────────────────────────────────────────────────────

Given: 4-level page table walk
Given: RAM latency = 100 ns per access
Given: TLB hit latency = 1 ns

CALCULATE WITHOUT TLB:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Step 1: Read PML4[index]  → 100 ns                                         │
│ Step 2: Read PDPT[index]  → 100 ns                                         │
│ Step 3: Read PD[index]    → 100 ns                                         │
│ Step 4: Read PT[index]    → 100 ns                                         │
│ Step 5: Read Data         → 100 ns                                         │
│                                                                             │
│ Total = 5 × 100 ns = _____ ns                                              │
│ Without TLB: 500 ns per memory access!                                     │
│                                                                             │
│ TRAP: This assumes no page table caching. In reality:                      │
│ - Page table entries are cacheable (L1/L2/L3)                              │
│ - Prefetching may help                                                      │
│ - But cache lines are 64 bytes, entry is 8 bytes                           │
└─────────────────────────────────────────────────────────────────────────────┘

CALCULATE WITH TLB HIT:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Step 1: TLB lookup: VA → PA  → 1 ns                                        │
│ Step 2: Read Data            → 100 ns                                      │
│                                                                             │
│ Total = 1 + 100 = 101 ns ≈ 100 ns                                         │
│ Speedup = 500 / 100 = 5×                                                   │
│                                                                             │
│ Actually, TLB and cache work together:                                     │
│ - If data in L1 cache: ~1-4 cycles = ~1 ns @ 4 GHz                        │
│ - TLB + L1 hit: ~1-5 ns total                                              │
│ - Speedup vs cold miss: 500 / 5 = 100×                                    │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: TLB SIZE AND COVERAGE
─────────────────────────────────────────────────────────────────────────────────

Given: L1 dTLB = 64 entries for 4KB pages, 32 entries for 2MB pages (typical)
Given: L2 TLB = 1536 entries unified (typical AMD Zen2)

CALCULATE 4KB COVERAGE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ L1 dTLB 4KB entries = 64                                                   │
│ Each entry maps 1 page = 4096 bytes                                        │
│ Total L1 coverage = 64 × 4096 = _____ bytes                                │
│ Work: 64 × 4096 = 262,144 bytes = 256 KB                                  │
│                                                                             │
│ L2 TLB entries = 1536                                                      │
│ L2 coverage = 1536 × 4096 = 6,291,456 bytes = 6 MB                        │
│                                                                             │
│ TRAP: 6 MB seems small. Process with 512 MB working set:                   │
│ 512 MB / 6 MB = 85× larger than TLB coverage                               │
│ Many TLB misses expected!                                                   │
└─────────────────────────────────────────────────────────────────────────────┘

CALCULATE 2MB COVERAGE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ L1 dTLB 2MB entries = 32                                                   │
│ Each entry maps 2 MB                                                        │
│ L1 2MB coverage = 32 × 2 MB = 64 MB                                        │
│                                                                             │
│ With 2MB pages, same 512 MB working set:                                   │
│ 512 MB / 2 MB = 256 entries needed                                         │
│ L1 miss rate: (256 - 32) / 256 = 87.5% L1 miss                            │
│ But L2 may cover: 1536 entries × 2 MB = 3 GB                               │
│ L2 can hold all 256 entries → 0% L2 miss                                  │
│                                                                             │
│ ADVANTAGE: 2MB pages have 8× fewer misses than 4KB                         │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: TLB FLUSH TRIGGER EVENTS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ EVENT                     │ FLUSH TYPE                │ COST              │
│───────────────────────────┼───────────────────────────┼───────────────────│
│ CR3 write                 │ Full flush (except G=1)   │ ~1000s cycles     │
│ Context switch            │ Full or PCID switch       │ ~1000s cycles     │
│ munmap() / mprotect()     │ Selective (invlpg)        │ ~100 cycles each  │
│ fork() COW break          │ Single page               │ ~100 cycles       │
│ mremap()                  │ Range flush               │ ~100 × pages      │
│ exec() (new mm)           │ Full flush                │ ~1000s cycles     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: INVLPG INSTRUCTION
─────────────────────────────────────────────────────────────────────────────────

Given: Need to invalidate translation for VA = 0x7FFE5E4ED000

┌─────────────────────────────────────────────────────────────────────────────┐
│ Assembly: invlpg [0x7FFE5E4ED000]                                          │
│ Opcode: 0F 01 /7 (with memory operand)                                     │
│                                                                             │
│ Effect:                                                                     │
│ - Removes TLB entry for VA 0x7FFE5E4ED000                                  │
│ - Does NOT affect other entries                                            │
│ - Next access to this VA causes TLB miss → page walk                       │
│                                                                             │
│ Kernel C wrapper:                                                           │
│ static inline void __native_flush_tlb_one_user(unsigned long addr)         │
│ {                                                                           │
│     asm volatile("invlpg (%0)" : : "r" (addr) : "memory");                │
│ }                                                                           │
│                                                                             │
│ TRAP: Must be ring 0 (kernel mode)                                         │
│ TRAP: Only affects CURRENT CPU! Other CPUs need IPI.                       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: CROSS-CPU TLB SHOOTDOWN
─────────────────────────────────────────────────────────────────────────────────

Given: 12 CPU system, process runs on all CPUs
Given: Need to unmap page 0x7FFE5E4ED000

┌─────────────────────────────────────────────────────────────────────────────┐
│ WITHOUT SHOOTDOWN (BUG!):                                                  │
│ - CPU 0: invlpg(0x7FFE5E4ED000)  → TLB flushed on CPU 0                   │
│ - CPU 1-11: Still have old TLB entry!                                      │
│ - CPU 5 reads 0x7FFE5E4ED000 → Gets OLD physical page (stale data!)       │
│ - Security vulnerability: Can access freed/remapped memory                 │
│                                                                             │
│ WITH SHOOTDOWN:                                                             │
│ - CPU 0: Prepare IPI (Inter-Processor Interrupt)                           │
│ - CPU 0: Send IPI to CPUs 1-11                                             │
│ - CPUs 1-11: Receive IPI, execute invlpg, send ACK                        │
│ - CPU 0: Wait for all ACKs                                                 │
│ - Total time: ~10,000 cycles (IPI overhead)                                │
│                                                                             │
│ Kernel function: flush_tlb_mm_range(mm, start, end, stride)               │
│ This sends IPIs to all CPUs running threads of this mm                    │
└─────────────────────────────────────────────────────────────────────────────┘

CALCULATE IPI OVERHEAD:
┌─────────────────────────────────────────────────────────────────────────────┐
│ IPI latency ≈ 1-10 μs (depends on CPU, interconnect)                       │
│ 12 CPUs, worst case: 12 × 10 μs = 120 μs (serial)                         │
│ Actually parallel: 1 × 10 μs ≈ 10 μs (send to all, wait for all)          │
│                                                                             │
│ Compare to invlpg alone: ~100 cycles = ~25 ns                              │
│ IPI overhead: 10,000 ns / 25 ns = 400× more expensive                      │
│                                                                             │
│ OPTIMIZATION: Batch TLB shootdowns                                         │
│ Unmap 1000 pages: 1 shootdown with range instead of 1000 shootdowns       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 6: PCID OPTIMIZATION
─────────────────────────────────────────────────────────────────────────────────

Given: CR3[11:0] = PCID (Process Context ID, 12 bits = 4096 possible)

┌─────────────────────────────────────────────────────────────────────────────┐
│ WITHOUT PCID:                                                               │
│ - Context switch: Write CR3 with new PML4 address                          │
│ - Effect: Full TLB flush (all non-global entries removed)                  │
│ - Next memory access: All TLB misses                                       │
│ - Warm-up cost: ~1000s of page walks                                       │
│                                                                             │
│ WITH PCID:                                                                  │
│ - Each process has unique PCID (0-4095)                                    │
│ - TLB entries tagged with PCID                                             │
│ - Context switch: Write CR3 with new PCID, set bit 63 = 1                  │
│ - Effect: NO flush! Old entries kept, tagged with old PCID                 │
│ - Result: Returning process finds its TLB entries still warm               │
│                                                                             │
│ SAVINGS:                                                                    │
│ - If process has 1000 hot pages                                            │
│ - Without PCID: 1000 × 500 ns = 500,000 ns = 0.5 ms                        │
│ - With PCID: 0 ns (TLB already warm)                                       │
│ - Speedup: Infinite (0 vs 0.5 ms)                                          │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: 500 ns without TLB, 100 ns with TLB
Problem 2: L1 4KB = 256 KB, L2 = 6 MB
Problem 5: IPI overhead ~10 μs for 12 CPU shootdown

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgetting IPI on SMP → stale TLB on other CPUs → security hole
F2. Flushing too aggressively → performance degradation
F3. Not flushing when needed → stale translations → wrong data
F4. Global pages (G=1) not flushed by CR3 write → need invlpg or MOV CR4
F5. PCID exhaustion (>4096 processes) → need to reuse and flush
```
