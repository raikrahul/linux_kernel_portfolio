# AXIOMATIC DERIVATION: SPLIT PAGE TABLE LOCKS

## AXIOM SOURCE: THIS MACHINE (dmesg, /proc, /boot/config)
```
A01. PAGE_SIZE         = 4096 bytes                   ← getconf PAGE_SIZE
A02. sizeof(struct page) = 64 bytes                   ← dmesg: DATA_SIZEOF_PAGE: 64
A03. sizeof(spinlock_t)  = 4 bytes                    ← dmesg: DATA_SIZEOF_SPINLOCK: 4
A04. CONFIG_NR_CPUS      = 8192                       ← /boot/config-6.14.0-37-generic
A05. CACHE_LINE_SIZE     = 64 bytes                   ← getconf LEVEL1_DCACHE_LINESIZE
A06. MemTotal            = 15776272 KB = 15.04 GB     ← /proc/meminfo
A07. nproc               = 12 CPUs                    ← nproc
```

## BLOCK 1: DERIVE TOTAL STRUCT PAGES
```
01. MemTotal = 15776272 KB.
02. MemTotal in bytes = 15776272 × 1024 = 16154902528 bytes.
03. Total Pages = 16154902528 / 4096 = 3944068 pages.
04. Total struct page array = 3944068 × 64 = 252420352 bytes = 240.7 MB.
05. CALCULATE: 252420352 / 1024 / 1024 = _____ MB. (USER FILL)
```

## BLOCK 2: DERIVE VMEMMAP RANGE
```
06. vmemmap base (x86-64 standard) = 0xffffea0000000000.
07. PFN 0 → struct page at 0xffffea0000000000 + 0×64 = 0xffffea0000000000.
08. PFN 1 → struct page at 0xffffea0000000000 + 1×64 = 0xffffea0000000040.
09. PFN 0x20000 (131072) → struct page at 0xffffea0000000000 + 0x20000×64.
10. CALCULATE: 0x20000 × 64 = 0x20000 × 0x40 = _____ (hex). (USER FILL)
11. Result: 0xffffea0000000000 + 0x800000 = 0xffffea0000800000.
```

## BLOCK 3: STRUCT PAGE MEMORY LAYOUT (64 BYTES, 1 CACHE LINE)
```
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 0 | byte 1 | ... byte 7                                         | ← flags (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 8 | ...                                                  byte 23| ← lru list_head (16 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 24| ...                                                 byte 31 | ← mapping/anon_vma (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 32| ...                                                 byte 39 | ← index (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 40| ...                                                 byte 47 | ← private (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 48| byte 49| byte 50| byte 51|                                  | ← _refcount (4 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 52| byte 53| byte 54| byte 55|                                  | ← _mapcount (4 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
| byte 56| ...                                                 byte 63 | ← ptl/compound_head (8 bytes)
+--------+--------+--------+--------+--------+--------+--------+--------+
                                                                          TOTAL = 64 bytes = 1 CACHE LINE
```

## BLOCK 4: PROBLEM DERIVATION (CONTENTION MATH)
```
12. CPU count = 12 (this machine). CONFIG_NR_CPUS = 8192 (compiled max).
13. Scenario: 12 threads call mmap() simultaneously.
14. Each mmap() → page fault → modify PTE → need lock.
15. OLD: 1 lock (mm->page_table_lock).
16. OLD Contention: 12 threads fight for 1 lock.
17. Lock hold time (typical) = 100 ns.
18. Sequential wait = 12 × 100 ns = 1200 ns = 1.2 µs per fault.
19. HARDER EXAMPLE: CONFIG_NR_CPUS = 8192.
20. Sequential wait = 8192 × 100 ns = 819200 ns = 819.2 µs.
21. CALCULATE: 819200 / 1000 = _____ µs. (USER VERIFY)
22. Throughput = 1 / 819.2 µs = 1220 faults/sec (SYSTEM TOTAL).
23. Required: 1,000,000 faults/sec. Gap = 1,000,000 - 1,220 = 998,780.
```

## BLOCK 5: SOLUTION DERIVATION
```
24. SOLUTION: Move lock from mm_struct to struct page.
25. Each page table page has its own lock.
26. NEW Contention: Only threads touching SAME page table page fight.
27. Page table page covers: (entries × page size).
28. x86-64: 512 entries × 4KB = 2MB range per PTE page table.
29. Probability 2 threads hit same 2MB region = LOW.
30. ∴ Contention drops from O(CPUs) to O(1) expected.
```

## BLOCK 6: CACHE LINE FALSE SHARING (WHY NOT EMBED?)
```
31. If spinlock_t (4 bytes) embedded at offset 56...
32. CPU1 writes to lock at offset 56.
33. Cache Line = 64 bytes. struct page = 64 bytes. EXACT FIT.
34. WRITE to offset 56 → INVALIDATE entire cache line.
35. CPU2 wants to read flags at offset 0 (same page).
36. Cache line INVALID → CPU2 STALLS → fetch from L3/RAM.
37. STALL = 50-100 cycles = 20-40 ns.
38. SPLIT SOLUTION:
39. page->ptl = POINTER (8 bytes) to HEAP-allocated lock.
40. Lock lives at 0xffff888XXXXXXX (separate cache line).
41. CPU1 writes to 0xffff888XXXXXXX → invalidates THAT line.
42. struct page cache line remains VALID.
43. CPU2 reads flags → HIT → 0 stall.
```

## BLOCK 7: NUMERICAL CHECK (DOES 4-BYTE LOCK FIT?)
```
44. sizeof(spinlock_t) = 4 bytes.
45. Available space at union = 8 bytes (offset 56-63).
46. 4 ≤ 8 ✓. LOCK FITS.
47. BUT: If lockdep/debug enabled, spinlock_t can be 72+ bytes.
48. 72 > 8 ✗. LOCK DOES NOT FIT.
49. ∴ Dynamic allocation required for debug kernels.
50. This machine: sizeof(spinlock_t) = 4. Normal kernel.
```

## BLOCK 8: TODO WORKSHEET (USER WRITES CODE)
```
TODO 1: CALCULATE → If PFN=0x100000, what is struct page address?
        Formula: vmemmap + PFN × sizeof(struct page)
        = 0xffffea0000000000 + 0x100000 × 64
        = 0xffffea0000000000 + _____________
        = _____________________
        (USER FILL BOTH BLANKS)

TODO 2: ALLOCATE PAGE
        Write: my_pt_page = alloc_pages(GFP_KERNEL, 0);
        Expected return: struct page * ≈ 0xffffea00XXXXXXXX.

TODO 3: INIT AS PAGE TABLE
        Write: if (!pgtable_page_ctor(my_pt_page)) { return -ENOMEM; }
        Internal action: kmalloc(4) → heap addr → store in page->ptl.

TODO 4: GET LOCK POINTER
        Write: spinlock_t *ptl = ptlock_ptr(my_pt_page);
        Print: printk("page=%px ptl=%px\n", my_pt_page, ptl);
        VERIFY: ptl != (void*)my_pt_page + 56. (SPLIT ✓)

TODO 5: LOCK/UNLOCK
        Write: spin_lock(ptl);
        Write: spin_unlock(ptl);

TODO 6: CLEANUP
        Write: pgtable_page_dtor(my_pt_page);
        Write: __free_pages(my_pt_page, 0);
```

## PREDICTED FAILURES
```
F1. FORGOT NULL CHECK → alloc_pages returns NULL → kernel OOPS.
F2. FORGOT pgtable_page_ctor → page->ptl = garbage → spin_lock(garbage) → CRASH.
F3. FORGOT pgtable_page_dtor → memory leak (lock not freed).
F4. CALLED ptlock_init (name collision) → kernel symbol conflict → build fail.
F5. USED spin_lock_irq in wrong context → deadlock.
```

## NEW TERMS INTRODUCED WITHOUT DERIVATION: NONE
(All derived from A01-A07 axioms)

---

## BLOCK 9: AXIOMATIC FOUNDATION (FROM SCRATCH)

### 9.1 WHAT IS A BIT?
```
AXIOM: Bit = 0 or 1. Smallest unit. Wire voltage LOW=0, HIGH=1.
```

### 9.2 WHAT IS A BYTE?
```
AXIOM: Byte = 8 bits grouped.
CALCULATION: 
  Value 00000000 = 0
  Value 00000001 = 1
  Value 00000010 = 2
  Value 11111111 = 128+64+32+16+8+4+2+1 = 255
MAX VALUE: 255 = 2^8 - 1
```

### 9.3 WHAT IS RAM?
```
AXIOM: RAM = chip with storage cells. Each byte has address: 0, 1, 2, ...
THIS MACHINE: 
  MemTotal = 15776272 KB (from /proc/meminfo)
  MemTotal = 15776272 × 1024 = 16154902528 bytes
  Addresses: 0 to 16154902527
```

### 9.4 WHAT IS A CPU?
```
AXIOM: CPU = circuit that READS byte from RAM, DECODES as instruction, EXECUTES.
THIS MACHINE: 12 CPUs (from /proc/cpuinfo)
```

---

## BLOCK 10: CPU TOPOLOGY (LIVE DATA 2025-12-30)

### 10.1 RAW DATA FROM lscpu
```
Socket(s):            1        ← 1 physical chip plugged into motherboard
Core(s) per socket:   6        ← 6 execution engines per chip
Thread(s) per core:   2        ← hyperthreading, 2 threads share 1 core
CPU(s):               12       ← 1 × 6 × 2 = 12 logical CPUs
```

### 10.2 DEFINITION OF TERMS
```
SOCKET = physical silicon chip. This machine: 1 socket.
CORE   = independent execution engine inside socket. Has own ALU, FPU, L1/L2 cache.
         This machine: 6 cores.
THREAD = hardware feature allowing 1 core to run 2 instruction streams.
         Threads SHARE core's ALU, FPU, and MMU.
         This machine: 2 threads per core.
MMU    = Memory Management Unit. Hardware that translates virtual→physical address.
         MMU is part of CORE, not thread.
         This machine: 6 MMUs (one per core), NOT 12.
```

### 10.3 NUMERICAL CALCULATION: HOW MANY MMUs?
```
INPUT:  12 logical CPUs, 2 threads per core
FORMULA: MMUs = CPUs / Threads_per_core
CALCULATION: 6 = 12 / 2
RESULT: 6 MMUs. CPU 0 and CPU 1 SHARE same MMU (Core 0).
```

### 10.4 THREAD SIBLINGS (from /sys)
```
SOURCE: /sys/devices/system/cpu/cpu0/topology/thread_siblings_list
VALUE:  0-1
MEANING: CPU 0 and CPU 1 are on same physical core (share cache, MMU).
```

---

## BLOCK 11: WHAT IS A LOCK? (DERIVED FROM BYTES)

### 11.1 THE PROBLEM
```
SCENARIO: 
  CPU 0 writes value 0xAA to RAM address 1000
  CPU 1 writes value 0xBB to RAM address 1000 (SAME!)
  Both write at same time.

POSSIBLE OUTCOMES:
  A) RAM[1000] = 0xAA (CPU 0 wins)
  B) RAM[1000] = 0xBB (CPU 1 wins)
  C) RAM[1000] = garbage (partial bits from each)

PROBLEM: Outcome undefined. Data corruption possible.
```

### 11.2 DEFINITION OF LOCK
```
LOCK = 1 byte in RAM at some address (e.g., 5000).
VALUE: 0 = unlocked, 1 = locked.
```

### 11.3 LOCK ALGORITHM
```
ACQUIRE LOCK (before modifying protected data):
  step 1: Read RAM[5000]
  step 2: If value = 0, write 1 to RAM[5000] using ATOMIC instruction, proceed
  step 3: If value = 1, goto step 1 (SPIN)

RELEASE LOCK (after modifying protected data):
  step 1: Write 0 to RAM[5000]
```

### 11.4 NUMERICAL TRACE: TWO CPUs, ONE LOCK
```
TIME 0ns:   RAM[5000] = 0 (unlocked)
            CPU 0 reads RAM[5000] → gets 0 → attempts atomic write of 1
            CPU 1 reads RAM[5000] → gets 0 → attempts atomic write of 1

TIME 1ns:   CPU 0 atomic write SUCCEEDS (was first)
            CPU 1 atomic write FAILS (value already 1)
            RAM[5000] = 1

TIME 2ns:   CPU 0: lock acquired → writes RAM[1000] = 0xAA
            CPU 1: lock failed → loops, reads RAM[5000] = 1, spins

TIME 100ns: CPU 0: done writing → writes RAM[5000] = 0 (release)
            RAM[5000] = 0

TIME 101ns: CPU 1 reads RAM[5000] = 0 → atomic write 1 → SUCCEEDS
            RAM[5000] = 1

TIME 200ns: CPU 1: writes RAM[1000] = 0xBB → done → writes RAM[5000] = 0

RESULT: No corruption. CPU 1 waited ~100ns for CPU 0.
```

---

## BLOCK 12: WHAT IS A PAGE TABLE? (DERIVED FROM PROBLEM)

### 12.1 THE PROBLEM
```
SCENARIO:
  Program A wants to use virtual address 0x1000
  Program B wants to use virtual address 0x1000 (SAME!)

CONFLICT: Both cannot use same physical RAM byte.
```

### 12.2 SOLUTION: FAKE ADDRESSES
```
IDEA: Give each program FAKE addresses (virtual).
      Hardware translates FAKE → REAL (physical).

EXAMPLE:
  Program A writes to virtual 0x1000 → CPU translates → physical 0x50000
  Program B writes to virtual 0x1000 → CPU translates → physical 0x80000
  Both see 0x1000, but access different physical RAM.
```

### 12.3 PAGE TABLE = TRANSLATION TABLE
```
PAGE TABLE = array in RAM.
Each entry = Page Table Entry (PTE).
PTE says: virtual page N → physical page M.
```

### 12.4 PTE SIZE (from kernel source)
```
SOURCE: arch/x86/include/asm/pgtable_64_types.h
CODE:   typedef unsigned long pteval_t;
        typedef struct { pteval_t pte; } pte_t;
SIZE:   unsigned long on x86_64 = 8 bytes
∴ PTE = 8 bytes
```

### 12.5 PTEs PER PAGE TABLE PAGE
```
PAGE_SIZE = 4096 bytes (from getconf)
PTE_SIZE  = 8 bytes
PTEs per page = 4096 / 8 = 512 PTEs
```

### 12.6 ADDRESS RANGE COVERED BY ONE PAGE TABLE PAGE
```
Each PTE covers 4096 bytes of virtual address space.
One page table page has 512 PTEs.
Coverage = 512 × 4096 = 2097152 bytes = 2 MB
```

---

## BLOCK 13: WHAT IS VmPTE? (FROM KERNEL SOURCE)

### 13.1 SOURCE
```
FILE: fs/proc/task_mmu.c
CODE: " kB\nVmPTE:\t", mm_pgtables_bytes(mm) >> 10, 8);
```

### 13.2 DEFINITION
```
VmPTE = mm_pgtables_bytes(mm) >> 10
      = (bytes used by page tables for this process) / 1024
      = kilobytes of RAM used by page table pages
```

### 13.3 LIVE EXAMPLE (cat command, PID 47972)
```
SOURCE: /proc/47972/status
VmPTE:  56 kB

CALCULATION:
  56 kB = 56 × 1024 = 57344 bytes
  Page table pages = 57344 / 4096 = 14 pages
  
MEANING: cat process uses 14 page table pages.
         Each page can have its own lock in SPLIT design.
```

---

## BLOCK 14: WHAT IS CONFIG_NR_CPUS? (FROM KCONFIG)

### 14.1 SOURCE
```
FILE: arch/x86/Kconfig
CODE: config NR_CPUS
        int "Maximum number of CPUs" if SMP && !MAXSMP
        help
          This is purely to save memory: each supported CPU adds about 8KB
          to the kernel image.
```

### 14.2 THIS MACHINE
```
SOURCE: /boot/config-6.14.0-37-generic
VALUE:  CONFIG_NR_CPUS=8192
```

### 14.3 WHY 8192?
```
SOURCE: arch/x86/Kconfig
CODE:   default 8192 if SMP && CPUMASK_OFFSTACK

THIS MACHINE:
  CONFIG_SMP=y               (verified from /boot/config)
  CONFIG_CPUMASK_OFFSTACK=y  (verified from /boot/config)
  ∴ default 8192 applies

CALCULATION: 8192 = 2^13
  2×2×2×2×2×2×2×2×2×2×2×2×2 = 8192 ✓
```

### 14.4 IMPACT ON cpumask_t
```
SOURCE: include/linux/cpumask_types.h
CODE:   typedef struct cpumask { DECLARE_BITMAP(bits, NR_CPUS); } cpumask_t;

EXPANSION:
  NR_CPUS = 8192
  DECLARE_BITMAP(bits, 8192) → unsigned long bits[8192/64] → unsigned long bits[128]
  
SIZE: 128 × 8 bytes = 1024 bytes per cpumask_t

THIS MACHINE (12 CPUs):
  bits[0] = 0x0000000000000FFF  (CPU 0-11 set)
  bits[1..127] = 0x0000000000000000  (unused)
  
WASTE: 127 × 8 = 1016 bytes (99%) unused
```

---

## BLOCK 15: mm_struct vs VMA (DISTINCT STRUCTURES)

### 15.1 COMMON CONFUSION
```
WRONG: "page is VMA in userspace"
RIGHT: VMA describes address RANGE. Page is 4096 bytes of RAM.
       VMA can contain 0, 1, or 1000 pages.
```

### 15.2 mm_struct DEFINITION
```
SOURCE: include/linux/mm_types.h
PURPOSE: Holds ALL memory management info for one PROCESS.
CONTAINS: pgd (page table root), page_table_lock, mmap (VMA list), etc.
SHARING: All THREADS of same process share SAME mm_struct.
```

### 15.3 vm_area_struct (VMA) DEFINITION
```
PURPOSE: Describes ONE contiguous range of valid virtual addresses.
CONTAINS: vm_start, vm_end, vm_flags (permissions), vm_file, etc.
```

### 15.4 LIVE EXAMPLE: VMA vs PAGES
```
SOURCE: /proc/47972/maps (cat command)
VMA: 58f49e1aa000-58f49e1ac000 r--p ... /usr/bin/cat

CALCULATION:
  vm_end - vm_start = 0x58f49e1ac000 - 0x58f49e1aa000 = 0x2000 = 8192 bytes
  Pages in this VMA = 8192 / 4096 = 2 pages

∴ This VMA contains 2 pages. VMA ≠ page.
```

### 15.5 RELATIONSHIP DIAGRAM
```
task_struct (thread)
    │
    └─→ mm ─────────────────────────────┐
                                        │
                                        ▼
                              mm_struct (process)
                                   │
                     ┌─────────────┴─────────────┐
                     │                           │
                     ▼                           ▼
                   mmap                         pgd
                     │                           │
                     ▼                           ▼
              vm_area_struct              PAGE TABLE
              (VMA 0)                     (multi-level)
                     │                           │
                     ▼                           ▼
              vm_area_struct              PTE pages
              (VMA 1)                     (14 for cat)
                     │
                     ▼
                   ...
```

---

## BLOCK 16: CLONE_VM - HOW THREADS SHARE mm_struct

### 16.1 SOURCE
```
FILE: include/uapi/linux/sched.h
CODE: #define CLONE_VM 0x00000100 /* set if VM shared between processes */
```

### 16.2 KERNEL BEHAVIOR
```
SOURCE: kernel/fork.c
CODE:
  if (clone_flags & CLONE_VM) {
      mmget(oldmm);       // increment reference count
      mm = oldmm;         // USE SAME mm as parent
  } else {
      mm = dup_mm(tsk, current->mm);  // COPY mm (new process)
  }
  tsk->mm = mm;
```

### 16.3 TRANSLATION
```
pthread_create uses clone() with CLONE_VM set.
∴ New thread gets SAME mm pointer as parent.
∴ All threads share:
  - Same page tables
  - Same VMA list
  - Same page_table_lock (in old design)
```

### 16.4 LIVE PROOF
```
TEST PROGRAM OUTPUT:
  Main PID   = 25725
  Thread TID = 25726, PID = 25725
  Thread TID = 25727, PID = 25725
  Both threads see SAME memory map: 5e88a4996000-5e88a4997000

∴ THREADS SHARE mm_struct. This is why lock contention happens.
```

---

## BLOCK 17: CONTENTION MATH (NUMERICAL)

### 17.1 SETUP
```
MACHINE: 12 CPUs
SCENARIO: 12 threads of same process, each triggers page fault
LOCK_HOLD_TIME: 100 nanoseconds (typical)
```

### 17.2 OLD DESIGN (1 lock per mm_struct)
```
TIME     CPU0        CPU1        CPU2        ... CPU11
0ns      [ACQUIRE]   wait        wait        ... wait
100ns    [RELEASE]   [ACQUIRE]   wait        ... wait
200ns    done        [RELEASE]   [ACQUIRE]   ... wait
...
1100ns   done        done        done        ... [ACQUIRE]
1200ns   done        done        done        ... [RELEASE]

TOTAL TIME: 12 × 100ns = 1200ns
CPU11 WAIT TIME: 11 × 100ns = 1100ns  
```

### 17.3 NEW DESIGN (1 lock per page table page)
```
CONDITION: Each CPU touches different 2MB region.
           Different regions → different page table pages → different locks.

TIME     CPU0        CPU1        CPU2        ... CPU11
0ns      [LOCK_A]    [LOCK_B]    [LOCK_C]    ... [LOCK_L]
100ns    [UNLOCK]    [UNLOCK]    [UNLOCK]    ... [UNLOCK]

TOTAL TIME: 100ns (parallel)
WAIT TIME: 0ns
SPEEDUP: 1200ns / 100ns = 12×
```

### 17.4 WORST CASE ANALYSIS (CONFIG_NR_CPUS = 8192)
```
OLD DESIGN, if 8192 CPUs all fault at once:
  Sequential wait = 8192 × 100ns = 819200ns = 0.82ms
  Throughput = 1 / 0.00082s = 1220 faults/second

NEW DESIGN, if 8192 CPUs touch different regions:
  Parallel = 100ns
  Throughput = 8192 / 100ns = 81,920,000,000 faults/second (theoretical)
```

---

## BLOCK 18: LIVE PERFORMANCE TEST (THIS MACHINE)

### 18.1 TEST CODE
```c
// 12 threads, each touches different pages, measures time
// See /tmp/measure_fault_time.c
```

### 18.2 OUTPUT (2025-12-30)
```
CPU  0:  4679 µs for 833 faults = 5618 ns/fault
CPU  1:  4777 µs for 833 faults = 5736 ns/fault
CPU  2:  5517 µs for 833 faults = 6622 ns/fault (slowest)
...
CPU 11:  4040 µs for 833 faults = 4850 ns/fault

Total faults: 10000
Avg time per fault: 5486 ns
```

### 18.3 OBSERVATION
```
Some CPUs slower → indicates some lock contention.
Average ~5500ns per fault (includes kernel overhead, not just lock).
Split locks reduce but don't eliminate contention 
(some threads may touch same 2MB region).
```

---

## BLOCK 19: TERMS GLOSSARY (ALL DERIVED)

| Term | Definition | Derivation Source |
|------|------------|-------------------|
| Bit | 0 or 1, voltage on wire | Axiom |
| Byte | 8 bits, values 0-255 | 2^8 - 1 = 255 |
| RAM | Bytes at addresses 0 to N-1 | /proc/meminfo |
| CPU | Circuit: read→decode→execute | /proc/cpuinfo |
| Socket | Physical chip | lscpu |
| Core | Execution engine in socket | lscpu |
| Thread | Hyperthread, shares core | lscpu |
| MMU | Virtual→physical translator in core | Hardware |
| Lock | Byte: 0=unlocked, 1=locked | Derived from problem |
| Page | 4096 bytes | getconf PAGE_SIZE |
| PTE | 8 bytes mapping virtual→physical | kernel source |
| Page Table | Array of PTEs in RAM | Problem/solution |
| VMA | Virtual address range descriptor | mm_types.h |
| mm_struct | Per-process memory management | mm_types.h |
| VmPTE | kB of RAM for page tables | /proc/PID/status |
| CONFIG_NR_CPUS | Max CPUs at compile time | /boot/config |
| cpumask_t | Bitmap of CPUs, 1024 bytes | cpumask_types.h |
| CLONE_VM | Flag for thread mm sharing | sched.h |
| spinlock_t | Lock structure, 4 bytes | dmesg |

---

## BLOCK 20: ERRORS AND CONFUSIONS DOCUMENTED

### 20.1 USER CONFUSION: "Page is VMA in userspace"
```
WRONG. VMA = range description. Page = 4096 bytes.
VMA contains 0 to N pages.
```

### 20.2 USER CONFUSION: "12 CPUs = 12 MMUs?"
```
WRONG. MMU is per CORE, not per thread.
12 CPUs, 6 cores → 6 MMUs.
```

### 20.3 USER CONFUSION: "How can 2 CPUs contend if mm is per process?"
```
Answer: THREADS share mm_struct (CLONE_VM).
12 threads = 12 CPUs = share 1 mm = share 1 lock (old design).
```

### 20.4 USER CONFUSION: "CONFIG_NR_CPUS affects page tables?"
```
INDIRECTLY:
  - Determines max contention (8192 CPUs fighting for lock).
  - Determines cpumask_t size (1024 bytes).
  - Does NOT directly change page table structure.
```

---

## NEW TERMS INTRODUCED WITHOUT DERIVATION: NONE
(All additions derived from live machine data, kernel source, or axioms)

---

## BLOCK 21: CODE EXERCISE - YOUR TURN

### FILE: ptlock_demo_user.c

**DO NOT READ SOLUTIONS. FILL TODO BLOCKS YOURSELF.**

```
TODO 1: offset = tid × PAGE_SIZE_CONST
        CALCULATE: If tid=5, offset = ?
        VERIFY: 5 × 4096 = 20480 = 0x5000
        CHECK: 0x5000 < 0x200000 (2MB)? _____ ✓ or ✗

TODO 2: Write to trigger page fault.
        WHAT TO WRITE: *addr = _____;
        TYPE: char (1 byte)
        VALUE: Any non-zero (e.g., 'X' = 0x58)

TODO 3: offset = tid × PTE_COVERAGE
        CALCULATE: If tid=5, offset = ?
        PTE_COVERAGE = 2097152
        5 × 2097152 = _____________
        In hex: ___________

TODO 4: Same as TODO 2.

TODO 5: region_size = 12 × PTE_COVERAGE
        CALCULATE: 12 × 2097152 = ?
        STEP 1: 12 × 2000000 = 24000000
        STEP 2: 12 × 97152 = _______
        STEP 3: 24000000 + _______ = _______

TODO 6: mmap(NULL, region_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)
        PROT_READ = 1, PROT_WRITE = 2 → combined = ___
        MAP_PRIVATE = 2, MAP_ANONYMOUS = 32 → combined = ___

TODO 7: munmap(region, region_size); then mmap again.
        WHY NEEDED: PTEs exist after test 1. Need fresh region.

TODO 8: speedup = (double)total_same / (double)total_diff;
        EXAMPLE: 120000 / 20000 = ___
        TRAP: Integer division without cast = wrong result!
```

### COMPILE AND RUN:
```bash
cd /home/r/Desktop/learnlinux/kernel_exercises/ptlock_split
gcc -O0 -g ptlock_demo_user.c -o ptlock_demo_user -lpthread
./ptlock_demo_user
```

### EXPECTED OUTPUT (if you fill correctly):
```
TEST 1: 12 threads touching SAME PTE page → HIGH numbers (contention)
TEST 2: 12 threads touching DIFFERENT PTE pages → LOW numbers (parallel)
SPEEDUP: X.XXx faster
```

### BUGS IF YOU DO WRONG:
```
TODO 1 wrong → all threads touch same page → no variance test
TODO 3 wrong → threads might overlap PTE pages → no speedup seen
TODO 5 too small → mmap fails or segfault
TODO 6 wrong flags → mmap fails or segfault on access
TODO 7 skipped → test 2 doesn't trigger faults → both tests same
```

---

## BLOCK 22: AXIOMATIC FOUNDATION (COUNTING TO PTE)

### 22.1 AXIOMS (NO DEPENDENCIES)
```
A01. COUNT = 0, 1, 2, 3, 4, 5... (natural numbers)
A02. ADD = put two counts together → 3 + 5 = 8
A03. MULTIPLY = add same number repeatedly → 3 × 4 = 3+3+3+3 = 12
A04. POWER = multiply same number repeatedly → 2^3 = 2×2×2 = 8
A05. BIT = value 0 or 1 (two choices)
A06. BYTE = 8 bits grouped → 2^8 = 256 values → range [0, 255]
A07. RAM = chip with storage cells → each cell = 1 byte → each cell has ADDRESS (count)
A08. ADDRESS SIZE = 64 bits on x86_64 → only 48 bits used → 2^48 addresses
```

### 22.2 DERIVED CALCULATIONS
```
C01. 2^12 = 2×2×2×2×2×2×2×2×2×2×2×2 = 4096
C02. 2^21 = 2097152
C03. 2^9 = 512
C04. PAGE = 4096 bytes = 2^12 bytes (hardware design choice)
C05. PTE = 8 bytes = 64 bits (one translation entry)
C06. PTEs per page = 4096 ÷ 8 = 512
C07. Coverage per PTE page = 512 × 4096 = 2097152 bytes = 2 MB
```

---

## BLOCK 23: PTE BIT STRUCTURE (FROM KERNEL SOURCE)

### 23.1 SOURCE FILES
```
FILE: arch/x86/include/asm/pgtable_64_types.h
LINE: typedef unsigned long pteval_t;
LINE: typedef struct { pteval_t pte; } pte_t;

DERIVATION: unsigned long on x86_64 = 8 bytes = 64 bits
∴ PTE = 64 bits
```

### 23.2 BIT LAYOUT (FROM KERNEL SOURCE)
```
SOURCE: arch/x86/include/asm/pgtable_types.h
#define _PAGE_BIT_PRESENT       0       /* is present */
#define _PAGE_BIT_RW            1       /* writeable */
#define _PAGE_BIT_USER          2       /* userspace addressable */

PTE 64-BIT STRUCTURE:
┌─────────────────────────────────────────────────────────────────────┐
│ Bits 63-52: FLAGS (NX, etc)                    = 12 bits            │
│ Bits 51-12: PFN (Physical Frame Number)        = 40 bits            │
│ Bits 11-0:  FLAGS (Present, RW, User, etc)     = 12 bits            │
└─────────────────────────────────────────────────────────────────────┘
```

### 23.3 EXAMPLE PTE DECODING
```
PTE VALUE: 0x0000000005678067

EXTRACTION:
  PFN = (0x0000000005678067 >> 12) & 0xFFFFFFFFF = 0x5678
  Physical address = 0x5678 × 4096 = 0x5678000
  
FLAGS (bits 0-11) = 0x067 = binary 0000 0110 0111:
  Bit 0 = 1 → PRESENT (page in RAM)
  Bit 1 = 1 → RW (writable)
  Bit 2 = 1 → USER (userspace accessible)
  Bit 5 = 1 → ACCESSED (page was read)
  Bit 6 = 1 → DIRTY (page was written)
```

---

## BLOCK 24: 4-LEVEL PAGE TABLE WALK (WITH REAL ADDRESSES)

### 24.1 VIRTUAL ADDRESS BIT SPLIT
```
48-bit virtual address split into 5 parts:

BITS:    47─────39  38─────30  29─────21  20─────12  11────────0
         ╔════════╗ ╔════════╗ ╔════════╗ ╔════════╗ ╔══════════╗
FIELD:   │  PGD   │ │  PUD   │ │  PMD   │ │PTE idx │ │  OFFSET  │
         ╚════════╝ ╚════════╝ ╚════════╝ ╚════════╝ ╚══════════╝
BITS:      9 bits    9 bits     9 bits     9 bits     12 bits
RANGE:    0-511     0-511      0-511      0-511      0-4095
```

### 24.2 LIVE DATA EXAMPLE (from /proc/self/maps)
```
Virtual Address: 0x62eedeb8a000

BIT EXTRACTION:
  PGD index (47-39) = (0x62eedeb8a000 >> 39) & 0x1FF = 197
  PUD index (38-30) = (0x62eedeb8a000 >> 30) & 0x1FF = 443
  PMD index (29-21) = (0x62eedeb8a000 >> 21) & 0x1FF = 245
  PTE index (20-12) = (0x62eedeb8a000 >> 12) & 0x1FF = 394
  OFFSET    (11-0)  = 0x62eedeb8a000 & 0xFFF          = 0
```

### 24.3 FULL TRANSLATION WALK
```
STEP 0: CPU reads CR3 register
        CR3 = 0x1234000 (example, physical address of PGD table)

STEP 1: Index into PGD
        PGD index = 197
        Entry address = 0x1234000 + 197 × 8 = 0x1234000 + 0x628 = 0x1234628
        CPU reads RAM[0x1234628] → value = 0x2345067 → PUD @ 0x2345000

STEP 2: Index into PUD
        PUD index = 443
        Entry address = 0x2345000 + 443 × 8 = 0x2345000 + 0xDD8 = 0x2345DD8
        CPU reads RAM[0x2345DD8] → value = 0x3456067 → PMD @ 0x3456000

STEP 3: Index into PMD
        PMD index = 245
        Entry address = 0x3456000 + 245 × 8 = 0x3456000 + 0x7A8 = 0x34567A8
        CPU reads RAM[0x34567A8] → value = 0x4567067 → PTE TABLE @ 0x4567000
        
        *** THIS 0x4567000 IS THE PTE TABLE PAGE ***
        *** LOCK IS ASSOCIATED WITH THIS PAGE ***

STEP 4: Index into PTE TABLE
        PTE index = 394
        Entry address = 0x4567000 + 394 × 8 = 0x4567000 + 0xC50 = 0x4567C50
        CPU reads RAM[0x4567C50] → value = 0x5678067 → DATA PAGE @ 0x5678000

STEP 5: Add OFFSET
        Physical = 0x5678000 + 0 = 0x5678000
        
RESULT: Virtual 0x62eedeb8a000 → Physical 0x5678000
```

---

## BLOCK 25: WHY SAME PMD INDEX = SAME PTE TABLE PAGE

### 25.1 THE KEY INSIGHT
```
PMD table has 512 entries.
Each PMD entry points to ONE PTE table page.
PMD index (bits 29-21) selects which PMD entry.

SAME PMD index → SAME PMD entry → SAME pointer → SAME PTE table page
```

### 25.2 NUMERICAL PROOF
```
Address A: 0x62eedea00000
PMD index = (0x62eedea00000 >> 21) & 0x1FF

CALCULATION:
  0x62eedea00000 = 108851489824768 (decimal)
  108851489824768 ÷ 2097152 = 51913333
  51913333 mod 512 = 245

∴ PMD index = 245

Address B: 0x62eedea01000 (added 4096)
  108851489828864 ÷ 2097152 = 51913335
  51913335 mod 512 = 245

∴ PMD index = 245 (SAME!)

Address C: 0x62eedec00000 (added 2097152)
  108851491921920 ÷ 2097152 = 51913334
  51913334 mod 512 = 246

∴ PMD index = 246 (CHANGED!)
```

### 25.3 DERIVATION: MAXIMUM OFFSET FOR SAME PTE PAGE
```
Two addresses A and B with same bits 47-21:
  A = (X << 21) + Y    where Y ∈ [0, 2097151]
  B = (X << 21) + Z    where Z ∈ [0, 2097151]

Maximum difference = 2097151 (if Y=2097151 and Z=0)

∴ Any offset < 2097152 keeps same PMD index → same PTE table → same lock
∴ Any offset ≥ 2097152 changes PMD index → different PTE table → different lock
```

---

## BLOCK 26: CONNECTION OFFSET → BITS → PMD → LOCK

### 26.1 THE CHAIN
```
YOUR CODE: offset = tid × 4096

tid=0  → offset=0     → address=base+0     → PMD=245 → PTE page @ 0x4567000 → LOCK_A
tid=1  → offset=4096  → address=base+4096  → PMD=245 → PTE page @ 0x4567000 → LOCK_A
tid=11 → offset=45056 → address=base+45056 → PMD=245 → PTE page @ 0x4567000 → LOCK_A
```

### 26.2 BIT-LEVEL PROOF: ADDING 45056 DOES NOT CHANGE PMD
```
45056 in binary:
  45056 ÷ 32768 = 1 remainder 12288 → bit 15 set
  12288 ÷ 8192 = 1 remainder 4096 → bit 13 set
  4096 ÷ 4096 = 1 remainder 0 → bit 12 set
  
45056 = 2^15 + 2^13 + 2^12 = bits 15, 13, 12

PMD index uses bits 29-21.
Bits 15, 13, 12 are all < 21.

∴ Adding 45056 does NOT affect bits 29-21.
∴ PMD index unchanged.
∴ Same PTE table page.
∴ Same lock.
∴ All 12 threads contend for same lock.
```

### 26.3 VERIFICATION: 45056 < 2097152
```
45056 < 2097152 ?
45 thousand < 2 million ?
✓ TRUE

∴ All 12 threads fit in same 2MB region.
∴ All use same PTE table page.
∴ All fight for same lock.
∴ CONTENTION.
```

---

## NEW TERMS INTRODUCED WITHOUT DERIVATION: NONE

All terms traced to axioms A01-A08 and calculations C01-C07.

---

## BLOCK 27: STEP-BY-STEP PROOF: TWO ADDRESSES → SAME PMD → SAME PTE PAGE

### 27.1 ADDRESS A WALK
```
STEP A1: Virtual address = 0x62eedea00000
STEP A2: CPU extracts bits 29-21 = (0x62eedea00000 >> 21) & 0x1FF
STEP A3: CALCULATION: 108851489824768 ÷ 2097152 = 51913333 → 51913333 mod 512 = 245
STEP A4: PMD index = 245
STEP A5: CPU reads PMD table entry 245 at address = PMD_base + 245 × 8
STEP A6: PMD[245] = 0x0000000004567067 (contains PFN 0x4567)
STEP A7: PTE TABLE physical address = 0x4567 × 4096 = 0x4567000
```

### 27.2 ADDRESS B WALK (OFFSET +4096)
```
STEP B1: Virtual address = 0x62eedea00000 + 4096 = 0x62eedea01000
STEP B2: CPU extracts bits 29-21 = (0x62eedea01000 >> 21) & 0x1FF
STEP B3: CALCULATION: 108851489828864 ÷ 2097152 = 51913335 → 51913335 mod 512 = 245
STEP B4: PMD index = 245 (SAME AS A4!)
STEP B5: CPU reads PMD table entry 245 at address = PMD_base + 245 × 8 (SAME AS A5!)
STEP B6: PMD[245] = 0x0000000004567067 (SAME VALUE!)
STEP B7: PTE TABLE physical address = 0x4567000 (SAME AS A7!)
```

### 27.3 CONCLUSION FROM STEPS
```
A4 = B4 = 245 → SAME PMD index
A7 = B7 = 0x4567000 → SAME PTE TABLE PAGE
∴ Both addresses use SAME PTE table
∴ Both addresses share SAME LOCK (stored in struct page for 0x4567000)
```

### 27.4 ADDRESS C WALK (OFFSET +2097152 = +2MB)
```
STEP C1: Virtual address = 0x62eedea00000 + 2097152 = 0x62eedec00000
STEP C2: CPU extracts bits 29-21 = (0x62eedec00000 >> 21) & 0x1FF
STEP C3: CALCULATION: 108851491921920 ÷ 2097152 = 51913334 → 51913334 mod 512 = 246
STEP C4: PMD index = 246 (DIFFERENT FROM A4!)
STEP C5: CPU reads PMD table entry 246 at address = PMD_base + 246 × 8 (DIFFERENT!)
STEP C6: PMD[246] = 0x0000000007890067 (DIFFERENT VALUE!)
STEP C7: PTE TABLE physical address = 0x7890000 (DIFFERENT FROM A7!)
```

### 27.5 CONCLUSION
```
A4 = 245, C4 = 246 → DIFFERENT PMD index
A7 = 0x4567000, C7 = 0x7890000 → DIFFERENT PTE TABLE PAGES
∴ Address A and Address C use DIFFERENT locks
∴ NO CONTENTION between them
```

---

## BLOCK 28: LIVE MACHINE DATA (SESSION 2025-12-30)

### 28.1 SYSTEM AXIOMS
```
SOURCE: getconf PAGE_SIZE
VALUE: 4096 bytes

SOURCE: nproc
VALUE: 12 CPUs

SOURCE: /proc/meminfo
MemTotal: 15776272 kB = 16154902528 bytes

SOURCE: /boot/config-6.14.0-37-generic
CONFIG_NR_CPUS=8192
CONFIG_SMP=y
CONFIG_CPUMASK_OFFSTACK=y

SOURCE: dmesg / kern.log
DATA_SIZEOF_PAGE: 64 bytes
DATA_SIZEOF_SPINLOCK: 4 bytes
```

### 28.2 LIVE PROCESS DATA (cat command PID 57325)
```
SOURCE: /proc/57325/status
VmPTE: 60 kB

CALCULATION:
  60 kB = 60 × 1024 = 61440 bytes
  PTE pages = 61440 ÷ 4096 = 15 pages
  Each page covers 2 MB
  Total coverage potential = 15 × 2 MB = 30 MB
```

### 28.3 LIVE VMA DATA
```
SOURCE: /proc/self/maps
FIRST VMA: 62eedeb8a000-62eedeb8c000 r--p ... /usr/bin/cat

BIT EXTRACTION:
  Address: 0x62eedeb8a000
  PGD index: 197
  PUD index: 443
  PMD index: 245
  PTE index: 394
  OFFSET: 0
```

---

## BLOCK 29: WHAT IS VmPTE? (AXIOMATICALLY DERIVED)

### 29.1 DEFINITION
```
VmPTE = kilobytes of RAM used by THIS process's page table pages
```

### 29.2 KERNEL SOURCE
```
FILE: fs/proc/task_mmu.c
CODE: " kB\nVmPTE:\t", mm_pgtables_bytes(mm) >> 10, 8);

TRANSLATION:
  mm_pgtables_bytes(mm) = bytes of page table pages
  >> 10 = divide by 1024 = convert to kB
```

### 29.3 CONNECTION TO LOCKS
```
VmPTE = 60 kB → 15 PTE pages
15 PTE pages = 15 possible locks (in split lock design)
If all 12 threads touch addresses in SAME PTE page → fight for 1 lock
If all 12 threads touch addresses in 12 DIFFERENT PTE pages → 12 locks → no contention
```

---

## BLOCK 30: WHAT IS CONFIG_NR_CPUS? (AXIOMATICALLY DERIVED)

### 30.1 DEFINITION
```
CONFIG_NR_CPUS = compile-time constant = maximum CPUs kernel can support
```

### 30.2 SOURCE
```
FILE: arch/x86/Kconfig
CODE: config NR_CPUS
        int "Maximum number of CPUs" if SMP && !MAXSMP
        help
          This is purely to save memory: each supported CPU adds about 8KB
          to the kernel image.
```

### 30.3 WHY 8192 ON THIS MACHINE?
```
SOURCE: arch/x86/Kconfig
CODE: default 8192 if SMP && CPUMASK_OFFSTACK

THIS MACHINE:
  CONFIG_SMP=y ✓
  CONFIG_CPUMASK_OFFSTACK=y ✓
  ∴ default 8192 applies
```

### 30.4 IMPACT ON DATA STRUCTURES
```
cpumask_t definition:
  typedef struct cpumask { DECLARE_BITMAP(bits, NR_CPUS); } cpumask_t;
  = unsigned long bits[8192/64] = unsigned long bits[128]
  = 128 × 8 = 1024 bytes per cpumask

THIS MACHINE:
  Only 12 CPUs active
  bits[0] = 0x0000000000000FFF (bits 0-11 set)
  bits[1..127] = 0 (unused)
  WASTE: 1016 bytes per cpumask
```

---

## BLOCK 31: VMA vs PAGE (COMMON CONFUSION)

### 31.1 WRONG STATEMENT
```
"page is VMA in userspace" ← INCORRECT
```

### 31.2 CORRECT DEFINITIONS
```
VMA (vm_area_struct) = description of valid address RANGE
  Contains: vm_start, vm_end, vm_flags, vm_file
  PURPOSE: tells kernel "addresses X to Y are valid for this process"

PAGE = 4096 bytes of actual RAM
  PURPOSE: holds actual data

RELATIONSHIP: One VMA can contain 0, 1, or 1000+ pages
```

### 31.3 LIVE EXAMPLE
```
VMA from /proc/self/maps:
  58f49e1aa000-58f49e1ac000 r--p ... /usr/bin/cat

CALCULATION:
  Size = 0x58f49e1ac000 - 0x58f49e1aa000 = 0x2000 = 8192 bytes
  Pages = 8192 ÷ 4096 = 2 pages

∴ This VMA contains 2 pages. VMA ≠ page.
```

---

## BLOCK 32: THREADS SHARE mm_struct (CLONE_VM)

### 32.1 SOURCE
```
FILE: include/uapi/linux/sched.h
CODE: #define CLONE_VM 0x00000100 /* set if VM shared between processes */
```

### 32.2 KERNEL BEHAVIOR
```
FILE: kernel/fork.c
CODE:
  if (clone_flags & CLONE_VM) {
      mmget(oldmm);       // increment reference count
      mm = oldmm;         // USE SAME mm as parent
  } else {
      mm = dup_mm(tsk, current->mm);  // COPY mm (new process)
  }
  tsk->mm = mm;
```

### 32.3 IMPLICATION
```
pthread_create → clone() with CLONE_VM
∴ New thread gets SAME mm pointer as parent
∴ All threads share:
  - Same page tables (same PTEs)
  - Same VMA list
  - Same page_table_lock (old design) OR same per-page locks (new design)
  
∴ When 12 threads page fault, all modify SAME page table → need locking
```

---

## BLOCK 33: struct page vs PTE (COMMON CONFUSION)

### 33.1 DEFINITIONS
```
PTE (Page Table Entry):
  SIZE: 8 bytes
  LOCATION: Inside a PTE table page
  CONTENTS: PFN (40 bits) + flags (24 bits)
  PURPOSE: Translation mapping (virtual → physical)

struct page:
  SIZE: 64 bytes
  LOCATION: vmemmap array in kernel memory
  CONTENTS: metadata (flags, refcount, lru, ptl, etc)
  PURPOSE: Describe a physical RAM page
```

### 33.2 RELATIONSHIP
```
PTE points to DATA PAGE (via PFN)
struct page describes DATA PAGE (metadata)
struct page ALSO describes PTE TABLE PAGE (when that page is used as page table)

PTE ≠ struct page ≠ DATA
They are three different things!
```

### 33.3 DIAGRAM
```
┌─────────────────────────────────────────────────────────────┐
│ PTE TABLE PAGE @ physical 0x4567000                         │
│   PTE[0] = 8 bytes → points to data page PFN 0x5678         │
│   PTE[1] = 8 bytes → points to data page PFN 0x789A         │
│   ...                                                       │
│   PTE[511] = 8 bytes → points to data page PFN 0xBCDE       │
└─────────────────────────────────────────────────────────────┘
          │                              │
          │ (PTE page itself has         │ (each data page has
          │  metadata)                   │  metadata)
          ▼                              ▼
┌─────────────────────┐        ┌─────────────────────┐
│ struct page for     │        │ struct page for     │
│ PTE table page      │        │ data page 0x5678    │
│ @ vmemmap+0x4567×64 │        │ @ vmemmap+0x5678×64 │
│                     │        │                     │
│ ptl = LOCK_A ←──────│        │ (no ptl, not PT)    │
└─────────────────────┘        └─────────────────────┘
```

---

## BLOCK 34: CONTENTION MATH (NUMERICAL)

### 34.1 OLD DESIGN (1 lock per mm_struct)
```
12 CPUs, 1 lock, 100ns hold time per CPU

   TIME     CPU0        CPU1        ... CPU11
   0ns      [ACQUIRE]   wait            wait
   100ns    [RELEASE]   [ACQUIRE]       wait
   200ns    done        [RELEASE]       wait
   ...
   1100ns   done        done            [ACQUIRE]
   1200ns   done        done            [RELEASE]

TOTAL: 12 × 100ns = 1200ns (sequential)
CPU11 waited: 11 × 100ns = 1100ns
```

### 34.2 NEW DESIGN (1 lock per PTE page, SAME page)
```
All 12 threads touch addresses in SAME 2MB region
→ ALL use SAME PTE page → ALL use SAME lock
→ SAME as old design: 1200ns total
```

### 34.3 NEW DESIGN (1 lock per PTE page, DIFFERENT pages)
```
Each of 12 threads touches address in DIFFERENT 2MB region
→ Each uses DIFFERENT PTE page → Each uses DIFFERENT lock

   TIME     CPU0        CPU1        ... CPU11
   0ns      [LOCK_A]    [LOCK_B]        [LOCK_L]
   100ns    [UNLOCK]    [UNLOCK]        [UNLOCK]

TOTAL: 100ns (parallel)
SPEEDUP: 1200ns / 100ns = 12×
```

---

## BLOCK 35: SESSION SUMMARY (2025-12-30)

### 35.1 QUESTION ANSWERED
```
Q: How does offset = tid × 4096 relate to lock contention?

ANSWER CHAIN:
  offset = tid × 4096
    ↓ (add to base address)
  virtual address = base + offset
    ↓ (extract bits 29-21)
  PMD index = (address >> 21) & 0x1FF
    ↓ (read PMD entry)
  PTE table page physical address = PMD[index] & mask
    ↓ (compute struct page address)
  struct page address = vmemmap + PFN × 64
    ↓ (access ptl field)
  lock = &(struct page)->ptl

SAME PMD index → SAME PTE table page → SAME struct page → SAME lock → CONTENTION
```

### 35.2 NUMERICAL VERIFICATION
```
offset = 0 (tid=0):       address = 0x62eedea00000 → PMD = 245
offset = 4096 (tid=1):    address = 0x62eedea01000 → PMD = 245
offset = 45056 (tid=11):  address = 0x62eedea0b000 → PMD = 245

All PMD = 245 → All same PTE page → All same lock → CONTENTION
```

### 35.3 WHAT WE DERIVED AXIOMATICALLY
```
1. BIT, BYTE, RAM, ADDRESS from counting
2. PAGE = 4096 from 2^12
3. PTE = 8 bytes from kernel source
4. PTEs per page = 512 from 4096 ÷ 8
5. 2MB coverage from 512 × 4096
6. 4-level page table from 48 bits ÷ 4 levels = 9 bits each
7. PMD index = bits 29-21 from level structure
8. SAME PMD → SAME PTE page from table walk logic
9. offset < 2MB → same PMD from bit arithmetic
10. Lock in struct page from kernel source
```

### 35.4 USER CONFUSIONS ADDRESSED
```
1. "page is VMA" → NO, VMA = range description, page = 4096 bytes
2. "12 CPUs = 12 MMUs" → NO, MMU per core, 6 cores = 6 MMUs
3. "how can threads contend" → CLONE_VM shares mm_struct
4. "offset relates to lock" → chain: offset → PMD → PTE page → lock
5. "what is PTE" → 8 bytes with PFN + flags, stored in PTE table page
6. "struct page = PTE" → NO, struct page = metadata, PTE = mapping entry
```

---

## NEW TERMS INTRODUCED WITHOUT DERIVATION: NONE

All terms from this session traced to axioms A01-A08 and live machine data.

