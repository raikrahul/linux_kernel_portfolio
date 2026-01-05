```
═══════════════════════════════════════════════════════════════════════════════
ASID DRIVER: ADDRESS SPACE IDENTIFIERS - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | CR3 bits [11:0] = PCID
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: PCID/ASID BASICS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = Process Context ID (Intel term)                                     │
│ ASID = Address Space ID (AMD term, same concept)                           │
│                                                                             │
│ CR3 FORMAT WITH PCID:                                                      │
│ ┌────────────────────────────────────────────────────────────────┐         │
│ │ 63 │ 62-52 │ 51:12                │ 11:0              │        │         │
│ │ NF │ Rsvd  │ PML4 Physical Addr   │ PCID (12 bits)    │        │         │
│ └────────────────────────────────────────────────────────────────┘         │
│                                                                             │
│ NF (bit 63): No Flush                                                      │
│   = 0: Flush TLB when writing CR3                                         │
│   = 1: Don't flush TLB when writing CR3 (keep entries)                    │
│                                                                             │
│ PCID (bits 11:0): 12 bits = 4096 possible values                          │
│   Each value tags TLB entries                                              │
│   TLB entry includes: {VA, PA, flags, PCID}                                │
│   Lookup: Match VA AND PCID                                                │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: EXTRACT PCID FROM CR3
─────────────────────────────────────────────────────────────────────────────────

Given: CR3 = 0x0000000305DEF005

┌─────────────────────────────────────────────────────────────────────────────┐
│ PCID = CR3 & 0xFFF = 0x0000000305DEF005 & 0xFFF                           │
│      = 0x005 = 5                                                           │
│                                                                             │
│ This process has PCID = 5                                                  │
│                                                                             │
│ PML4_phys = CR3 & 0x000FFFFFFFFFF000                                       │
│           = 0x0000000305DEF000                                             │
│                                                                             │
│ ANOTHER EXAMPLE: CR3 = 0x80000004A1B2C010                                 │
│ Bit 63 = 1 (NF flag set, no flush on CR3 write)                           │
│ PCID = 0x010 = 16                                                          │
│ PML4_phys = 0x4A1B2C000                                                    │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: CONTEXT SWITCH COMPARISON
─────────────────────────────────────────────────────────────────────────────────

WITHOUT PCID:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 1. Current: Process A, CR3 = 0x305DEF000                                  │
│    TLB: {0x7FFE0001: PA=0x12345000, 0x7FFE0002: PA=0x12346000, ...}       │
│                                                                             │
│ 2. Switch to Process B:                                                    │
│    Write CR3 = 0x4A1B2C000                                                │
│    TLB: FLUSHED! All entries gone.                                        │
│                                                                             │
│ 3. Process B runs:                                                         │
│    First access 0x7FFE0001 → TLB miss → page walk (400ns)                 │
│    Second access 0x7FFE0002 → TLB miss → page walk (400ns)                │
│    ...and so on for every page accessed                                   │
│                                                                             │
│ 4. Switch back to Process A:                                               │
│    Write CR3 = 0x305DEF000                                                │
│    TLB: FLUSHED again!                                                    │
│    Process A's TLB entries were lost, must rewalk.                        │
└─────────────────────────────────────────────────────────────────────────────┘

WITH PCID:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 1. Current: Process A, CR3 = 0x305DEF005 (PCID=5)                         │
│    TLB: {0x7FFE0001/5: PA=0x12345000, 0x7FFE0002/5: PA=0x12346000}       │
│                                                                             │
│ 2. Switch to Process B:                                                    │
│    Write CR3 = 0x8000004A1B2C010 (PCID=16, NF=1)                          │
│    TLB: NOT FLUSHED! Old entries kept.                                    │
│    TLB now: {                                                              │
│      0x7FFE0001/5: PA=0x12345000 (A's entry, marked PCID=5)              │
│      0x7FFE0002/5: PA=0x12346000 (A's entry)                             │
│      // B's entries will be added as accessed                             │
│    }                                                                        │
│                                                                             │
│ 3. Process B runs:                                                         │
│    Access 0x7FFE0001 → TLB lookup: VA=0x7FFE0001, PCID=16                 │
│    No match (A's entry has PCID=5) → TLB miss → page walk                 │
│    Add entry: 0x7FFE0001/16: PA=0x56789000                                │
│                                                                             │
│ 4. Switch back to Process A:                                               │
│    Write CR3 = 0x8000000305DEF005 (PCID=5, NF=1)                          │
│    TLB still contains: 0x7FFE0001/5: PA=0x12345000                        │
│    Process A's first access: TLB HIT! No page walk needed.               │
│                                                                             │
│ SAVINGS: All previous TLB entries still valid!                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: CALCULATE PCID SAVINGS
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: 
- Process has 1000 frequently accessed pages
- Context switches happen every 10ms
- TLB can hold 1536 entries (typical L2 TLB)
- Page walk costs 400ns

┌─────────────────────────────────────────────────────────────────────────────┐
│ WITHOUT PCID:                                                               │
│   Each context switch → TLB flush                                          │
│   After switch: 1000 pages × 400ns = 400,000 ns = 0.4 ms to refill       │
│   Context switches per second: 100 (10ms interval)                        │
│   TLB refill overhead: 100 × 0.4ms = 40ms per second                      │
│   Percentage: 40ms / 1000ms = 4% CPU time on TLB misses!                  │
│                                                                             │
│ WITH PCID:                                                                  │
│   Context switch → no flush                                                │
│   Previous entries still valid                                             │
│   TLB refill overhead: ~0 (entries preserved)                             │
│   Speedup: 4% → ~0% (context switch TLB overhead eliminated)              │
│                                                                             │
│ REAL BENEFIT ESTIMATE:                                                     │
│   Heavy multitasking: 5-15% performance improvement                       │
│   Databases with many connections: significant                             │
│   Single long-running process: minimal benefit                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: PCID LIMITATIONS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ LIMIT 1: Only 4096 PCIDs                                                   │
│   12 bits = 4096 possible values                                           │
│   More than 4096 processes → must reuse PCIDs                             │
│   Reuse → need to flush old entries for that PCID                         │
│                                                                             │
│ LIMIT 2: TLB size                                                          │
│   TLB can hold ~1536 entries                                               │
│   With many PCIDs, entries from each process compete for space            │
│   More PCIDs → fewer entries per process → higher miss rate               │
│                                                                             │
│ LIMIT 3: Global pages                                                      │
│   Kernel pages marked G=1 (global)                                        │
│   Not tagged with PCID                                                     │
│   Shared across all address spaces                                        │
│                                                                             │
│ LIMIT 4: invlpg still needed                                              │
│   Unmap a page → must invalidate in ALL PCIDs                             │
│   invlpg invalidates for all PCIDs (or need invpcid instruction)         │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: PCID=5 from 0x305DEF005, PCID=16 from 0x4A1B2C010
Problem 4: Without PCID ~4% overhead, with PCID ~0%

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgetting to mask PCID bits → wrong physical address
F2. Not setting NF bit → TLB flushed anyway
F3. PCID exhaustion → must implement recycling
F4. Stale entries after munmap → security vulnerability
F5. CPU without PCID support → feature not available
```
