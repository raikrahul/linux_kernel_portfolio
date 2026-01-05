# LINUX MEMORY MANAGEMENT - LESSON PLAN

## VARIABLE REGISTRY (40+ TRACKED VARIABLES)

```
┌──────────────────────────────────────────────────────────────────────────────────────┐
│ CATEGORY          │ VARIABLE                      │ VALUE           │ SOURCE        │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ SYSTEM            │ subsystem_count               │ 8               │ kernel docs   │
│ SYSTEM            │ mm_loc                        │ 50,000+         │ mm/ directory │
│ SYSTEM            │ evolution_years               │ 34 (1991-2025)  │ history       │
│ SYSTEM            │ kernel_version                │ 6.19.0-rc2      │ uname -r      │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ PAGE TABLE        │ page_sizes                    │ [4KB,2MB,1GB]   │ x86_64        │
│ PAGE TABLE        │ page_walk_levels              │ 4               │ x86_64        │
│ PAGE TABLE        │ entries_per_level             │ 512             │ 9 bits/level  │
│ PAGE TABLE        │ entry_size                    │ 8 bytes         │ 64-bit        │
│ PAGE TABLE        │ index_bits                    │ 9               │ log2(512)     │
│ PAGE TABLE        │ offset_bits                   │ 12              │ log2(4096)    │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ TLB               │ tlb_entries                   │ 1536            │ cpuinfo       │
│ TLB               │ tlb_hit_latency               │ 0.5 ns          │ hardware      │
│ TLB               │ tlb_miss_penalty              │ 400 ns          │ 4 RAM reads   │
│ TLB               │ tlb_speedup_ratio             │ 800×            │ 400/0.5       │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ ZONES             │ zone_count                    │ 3               │ ZONE_DMA/...  │
│ ZONES             │ zone_dma_limit                │ 16 MB           │ ISA legacy    │
│ ZONES             │ zone_dma32_limit              │ 4 GB            │ PCI 32-bit    │
│ ZONES             │ zone_highmem_threshold        │ 896 MB          │ x86-32 only   │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ NUMA              │ node_count                    │ 4 (typical)     │ topology      │
│ NUMA              │ local_latency                 │ 10 ns           │ same-socket   │
│ NUMA              │ remote_latency                │ 60 ns           │ cross-socket  │
│ NUMA              │ latency_ratio                 │ 6:1             │ 60/10         │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ RECLAIM           │ kswapd_wakeup_threshold       │ 128 MB          │ watermark     │
│ RECLAIM           │ direct_reclaim_threshold      │ 64 MB           │ watermark     │
│ RECLAIM           │ kswapd_scan_rate              │ 100 pages/ms    │ background    │
│ RECLAIM           │ direct_scan_rate              │ 1000 pages/ms   │ synchronous   │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ COMPACTION        │ fragmentation_threshold       │ 30%             │ trigger       │
│ COMPACTION        │ compaction_latency            │ 500 µs          │ per-run       │
│ COMPACTION        │ max_attempts                  │ 5               │ before abort  │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ OOM               │ oom_score_adj_min             │ -1000           │ never kill    │
│ OOM               │ oom_score_adj_max             │ +1000           │ always kill   │
│ OOM               │ kill_latency                  │ 1 ms            │ signal send   │
│ OOM               │ reclaim_latency               │ 100 ms          │ for 200 MB    │
├──────────────────────────────────────────────────────────────────────────────────────┤
│ YOUR MACHINE      │ ram_total                     │ 15.04 GB        │ /proc/meminfo │
│ YOUR MACHINE      │ ram_free                      │ 5.06 GB         │ /proc/meminfo │
│ YOUR MACHINE      │ total_pages                   │ 3,944,069       │ 15GB/4KB      │
│ YOUR MACHINE      │ cpu_model                     │ Ryzen 5 4600H   │ /proc/cpuinfo │
└──────────────────────────────────────────────────────────────────────────────────────┘
```

---

## PHRASE 1: "COMPLEX SYSTEM"

### ATTACK DECOMPOSITION
```
GIVEN:
  subsystems = 8 (VM, Huge, Zones, Nodes, Cache, Anon, Reclaim, Compact, OOM)
  subsystems = 8 (corrected: 9 items listed, use 8 for calculations)

CALCULATE INTERACTION COMPLEXITY:
  01. Module pairs = C(8,2) = 8! / (2! × 6!) = (8×7)/(2×1) = 28 interactions
  02. State combinations (3 states each) = 3^8 = 6,561 possible states
  03. Lines of code estimate = 50,000 LOC / 8 modules = 6,250 LOC/module

ATTACK VECTOR RANKING:
  A1. Decompose modules (difficulty: 3/10) → Study 1 module at a time
  A2. Map dependencies (difficulty: 6/10) → 28 interaction pairs to trace
  A3. State machine analysis (difficulty: 9/10) → 6,561 states to enumerate

YOUR TASK:
  [ ] List 8 subsystems by name
  [ ] For each, write 1 sentence: INPUT → PROCESS → OUTPUT
  [ ] Identify 3 module pairs that interact (e.g., Reclaim ↔ OOM)
```

---

## PHRASE 2: "EVOLVED OVER THE YEARS"

### TIMELINE CALCULATION
```
GIVEN:
  start_year = 1991
  current_year = 2025
  evolution_period = 34 years
  concepts = 8

CALCULATE:
  01. Development rate = 34 years / 8 concepts = 4.25 years/concept
  02. Kernel versions (minor) ~= 6.19 × 100 = 619 data points
  03. Git commits estimate = 1,000 commits/concept × 8 = 8,000 commits

ATTACK VECTOR:
  A1. Track feature introduction dates (difficulty: 4/10)
      - 1991: nommu initial
      - 1999: HIGHMEM for >1GB RAM
      - 2006: NUMA aware
      - 2011: THP (Transparent Huge Pages)
      - 2016: Compaction improvements
      - 2020: Per-memcg LRU

YOUR TASK:
  [ ] Find 3 git commits related to mm/
  [ ] Calculate commits/year = 8,000 / 34 = 235 commits/year
  [ ] Identify 1 feature added after 2020
```

---

## PHRASE 3: "MMU-LESS TO SUPERCOMPUTERS"

### SCALE ANALYSIS
```
GIVEN:
  mcu_memory = 64 KB = 65,536 bytes
  supercomputer_memory = 1 PB = 1,125,899,906,842,624 bytes
  scale_ratio = 1 PB / 64 KB = 17,179,869,184 (17+ billion)

CALCULATE REQUIREMENTS:
  01. MCU pages = 64 KB / 4 KB = 16 pages (if paging existed)
  02. Supercomputer pages = 1 PB / 4 KB = 274,877,906,944 pages (274 billion)
  03. Page table size (supercomputer) = 274B × 8 bytes = 2.19 TB just for tables!

ATTACK VECTOR:
  A1. MCU has no MMU → VA = PA → 0 translation overhead
  A2. Supercomputer → 5-level paging (LA57) → 57-bit addressing
  A3. Portability matrix: 20 architectures × 8 concepts = 160 combinations

PLATFORM MATRIX:
  ┌────────────────┬───────┬─────────┬──────────┬────────────┐
  │ Platform       │ RAM   │ MMU     │ Paging   │ Huge Pages │
  ├────────────────┼───────┼─────────┼──────────┼────────────┤
  │ ARM Cortex-M0  │ 64KB  │ No      │ N/A      │ N/A        │
  │ Raspberry Pi 4 │ 8GB   │ Yes     │ 4-level  │ 2MB        │
  │ Your Laptop    │ 16GB  │ Yes     │ 4-level  │ 2MB, 1GB   │
  │ Server (EPYC)  │ 4TB   │ Yes     │ 5-level  │ 2MB, 1GB   │
  │ Summit (SC)    │ 2PB   │ Yes     │ 5-level  │ 2MB, 1GB   │
  └────────────────┴───────┴─────────┴──────────┴────────────┘

YOUR TASK:
  [ ] Calculate: How many page tables for 1 TB RAM? (1 TB / 4 KB × 8 bytes = ?)
  [ ] What is maximum addressable with 4-level? (2^48 = ? TB)
  [ ] What is maximum addressable with 5-level? (2^57 = ? PB)
```

---

## PHRASE 4: "VIRTUAL TO PHYSICAL TRANSLATION"

### TRANSLATION COST MODEL
```
GIVEN:
  translation_latency_tlb_hit = 0.5 ns
  ram_access_latency = 100 ns
  page_walk_levels = 4
  tlb_entries = 1536

CALCULATE TLB MISS COST:
  01. TLB miss = 4 RAM accesses × 100 ns = 400 ns
  02. Slowdown ratio = 400 ns / 0.5 ns = 800×
  03. Break-even: TLB must hit 799/800 = 99.875% to match single RAM access

TLB COVERAGE ANALYSIS:
  01. With 4KB pages: 1536 entries × 4 KB = 6,144 KB = 6 MB coverage
  02. With 2MB pages: 1536 entries × 2 MB = 3,072 MB = 3 GB coverage
  03. Improvement factor = 3 GB / 6 MB = 512× more coverage

PAGE TABLE MEMORY COST:
  01. Per-level size = 512 entries × 8 bytes = 4,096 bytes = 1 page
  02. Full tree = 4 levels × 1 page = 4 pages = 16 KB minimum
  03. Sparse process (100 pages mapped):
      - 1 PML4 + 1 PDPT + 1 PD + 1 PT = 4 pages = 16 KB
  04. Dense process (1 million pages mapped):
      - 1 PML4 + 2 PDPT + 500 PD + 2000 PT = 2503 pages = 10 MB

YOUR TASK:
  [ ] If TLB hit rate = 95%, what is average translation time?
      = 0.95 × 0.5 ns + 0.05 × 400 ns = ?
  [ ] How many page table pages needed for 1 GB contiguous mapping?
  [ ] What is TLB miss rate if working set = 100 MB with 4KB pages?
```

---

## PHRASE 5: "ARCHITECTURE SPECIFIC PAGE SIZES"

### PAGE SIZE COMPARISON
```
┌────────────┬────────────────┬────────────────┬────────────────────────────────┐
│ Page Size  │ Offset Bits    │ Pages per GB   │ TLB Coverage (1536 entries)    │
├────────────┼────────────────┼────────────────┼────────────────────────────────┤
│ 4 KB       │ 12             │ 262,144        │ 6 MB                           │
│ 2 MB       │ 21             │ 512            │ 3,072 MB = 3 GB                │
│ 1 GB       │ 30             │ 1              │ 1,536 GB = 1.5 TB              │
└────────────┴────────────────┴────────────────┴────────────────────────────────┘

WASTE ANALYSIS:
  01. Request 5 KB on 4 KB pages:
      = ceil(5 KB / 4 KB) × 4 KB = 2 × 4 KB = 8 KB allocated
      Waste = 8 KB - 5 KB = 3 KB = 37.5%

  02. Request 5 KB on 2 MB pages:
      = ceil(5 KB / 2 MB) × 2 MB = 1 × 2 MB = 2,048 KB allocated
      Waste = 2,048 KB - 5 KB = 2,043 KB = 99.76%

  03. Average waste for random sizes [1, 4096] bytes:
      = 4096 / 2 = 2048 bytes = 50% average waste per page

INDEX BIT BREAKDOWN:
┌────────────┬──────────────────────────────────────────────────────────────────┐
│ Page Size  │ VA Bits: [Sign][PML4][PDPT][PD][PT][Offset]                      │
├────────────┼──────────────────────────────────────────────────────────────────┤
│ 4 KB       │ [16][9][9][9][9][12] = 64 bits                                   │
│ 2 MB       │ [16][9][9][9][--][21] = 64 bits (PT level skipped)               │
│ 1 GB       │ [16][9][9][--][--][30] = 64 bits (PD+PT levels skipped)          │
└────────────┴──────────────────────────────────────────────────────────────────┘

YOUR TASK:
  [ ] For 2MB page, what is (VA >> 21) & 0x1FF? (PD index directly)
  [ ] How many 2MB pages fit in 1GB? (1 GB / 2 MB = ?)
  [ ] What is offset mask for 1GB page? (2^30 - 1 = ?)
```

---

## PHRASE 6: "TLB PRESSURE ANALYSIS"

### PRESSURE CALCULATION
```
GIVEN:
  tlb_entries = 1536
  processes = 100
  working_set_per_process = 2 MB
  page_size = 4 KB

CALCULATE DEMAND:
  01. Pages per process = 2 MB / 4 KB = 512 pages
  02. Total pages = 100 processes × 512 = 51,200 pages
  03. TLB overflow ratio = 51,200 / 1,536 = 33.3× oversubscribed
  04. Expected hit rate (uniform) = 1,536 / 51,200 = 3.0%

CONTEXT SWITCH IMPACT:
  01. TLB flush on CR3 change = 1,536 entries invalidated
  02. Refill cost = 1,536 × 400 ns = 614,400 ns = 614 µs
  03. If 1000 context switches/second: 614 µs × 1000 = 614 ms = 61.4% CPU time!

MITIGATION WITH HUGE PAGES:
  01. With 2MB pages: 2 MB / 2 MB = 1 page per process
  02. Total pages = 100 × 1 = 100 pages
  03. TLB coverage = 100 / 1,536 = 6.5% of TLB used
  04. TLB hit rate → 100% (all fit)

YOUR TASK:
  [ ] Calculate TLB hit rate for 500 process system with 1MB working sets
  [ ] How many PCID tags needed to avoid TLB flush? (up to 4096)
  [ ] What is break-even working set size for TLB?
```

---

## PHRASE 7: "HUGETLBFS vs THP"

### CONFIGURATION COMPARISON
```
┌─────────────────────┬────────────────────────┬─────────────────────────────┐
│ Aspect              │ HugeTLBfs              │ Transparent Huge Pages      │
├─────────────────────┼────────────────────────┼─────────────────────────────┤
│ Setup steps         │ 3 (mount, reserve, map)│ 0 (automatic)               │
│ Allocation time     │ Boot time (reserved)   │ Runtime (on-demand)         │
│ Failure mode        │ Explicit (ENOMEM)      │ Silent fallback to 4KB      │
│ Memory waste        │ 50% (fixed 2MB chunks) │ Variable (khugepaged merges)│
│ Admin tuning        │ sysctl, mount options  │ /sys/kernel/mm/transparent_ │
│ Use case            │ Databases, VMs         │ General applications        │
└─────────────────────┴────────────────────────┴─────────────────────────────┘

HUGETLBFS SETUP CALCULATION:
  01. Reserve 10 GB for huge pages:
      = 10 GB / 2 MB = 5,120 huge pages
  02. Command: echo 5120 > /proc/sys/vm/nr_hugepages
  03. Memory locked = 5,120 × 2 MB = 10,240 MB = 10 GB unavailable for normal use

THP OVERHEAD:
  01. khugepaged scan interval = 10 seconds
  02. Pages scanned per interval = 4096
  03. Merge success rate (fragmented system) = 10%
  04. Actual huge pages created = 4096 × 10% / 512 = 0.8 huge pages/scan

YOUR TASK:
  [ ] If database needs 50 GB of huge pages, how many 2MB pages? 
  [ ] What is /sys path to enable THP? (/sys/kernel/mm/transparent_hugepage/enabled)
  [ ] What happens if huge page allocation fails mid-transaction?
```

---

## PHRASE 8: "ZONE ARCHITECTURE"

### ZONE MEMORY LAYOUT
```
YOUR MACHINE (15 GB RAM):
┌──────────────────────────────────────────────────────────────────────────────┐
│ Physical Address Range      │ Zone         │ Size      │ Usage              │
├──────────────────────────────────────────────────────────────────────────────┤
│ 0x0000_0000 - 0x00FF_FFFF   │ ZONE_DMA     │ 16 MB     │ ISA DMA, legacy    │
│ 0x0100_0000 - 0xFFFF_FFFF   │ ZONE_DMA32   │ 4,080 MB  │ PCI 32-bit devices │
│ 0x1_0000_0000 - 0x3_C2F4_4000│ ZONE_NORMAL │ ~10.8 GB  │ Regular allocations│
└──────────────────────────────────────────────────────────────────────────────┘

ALLOCATION FALLBACK CHAIN:
  01. Request: GFP_DMA (legacy device needs DMA-able memory)
      → Try ZONE_DMA (16 MB available)
      → If fail: return -ENOMEM (no fallback allowed)

  02. Request: GFP_KERNEL (normal allocation)
      → Try ZONE_NORMAL first (~10.8 GB)
      → If fail: Try ZONE_DMA32 (4 GB)
      → If fail: Try ZONE_DMA (16 MB)
      → If fail: return -ENOMEM

ZONE EXHAUSTION SCENARIO:
  01. DMA zone = 16 MB = 4,096 pages
  02. Typical DMA buffer = 64 KB = 16 pages
  03. Max concurrent DMA = 4,096 / 16 = 256 buffers
  04. If 257th device requests DMA → FAIL

YOUR TASK:
  [ ] What percentage of your 15GB is ZONE_DMA? (16 MB / 15 GB = ?)
  [ ] If ZONE_NORMAL is 90% full, how much free? (10.8 GB × 10% = ?)
  [ ] Can GFP_DMA32 request use ZONE_NORMAL? (Yes/No)
```

---

## PHRASE 9: "NUMA TOPOLOGY"

### LATENCY MATRIX (4-NODE SYSTEM)
```
┌─────────┬─────────┬─────────┬─────────┬─────────┐
│ Node    │ Node 0  │ Node 1  │ Node 2  │ Node 3  │
├─────────┼─────────┼─────────┼─────────┼─────────┤
│ Node 0  │ 10 ns   │ 20 ns   │ 40 ns   │ 60 ns   │
│ Node 1  │ 20 ns   │ 10 ns   │ 30 ns   │ 50 ns   │
│ Node 2  │ 40 ns   │ 30 ns   │ 10 ns   │ 20 ns   │
│ Node 3  │ 60 ns   │ 50 ns   │ 20 ns   │ 10 ns   │
└─────────┴─────────┴─────────┴─────────┴─────────┘

BANDWIDTH vs LATENCY:
  Local access:  100 GB/s at 10 ns
  Remote access: 30 GB/s at 60 ns
  Bandwidth ratio = 100 / 30 = 3.3×
  Latency ratio = 60 / 10 = 6×

SCHEDULING DECISION:
  01. Process P on Node 0, data on Node 3
  02. Access penalty = 60 ns / 10 ns = 6× slower
  03. Migration cost = 4 KB page × 2 (read + write) / 30 GB/s = 267 ns
  04. Break-even = 267 ns / (60 - 10) ns = 5.3 accesses
  05. IF accesses > 5 THEN migrate ELSE stay

MEMORY TOPOLOGY LINKS:
  4-node: C(4,2) = 6 interconnect links
  8-node: C(8,2) = 28 interconnect links (4.7× more complex)

YOUR TASK:
  [ ] Calculate average latency if uniform random node access
      = (10 + 20 + 40 + 60) / 4 = ?
  [ ] If 80% local, 20% remote: weighted latency = 0.8×10 + 0.2×60 = ?
  [ ] How many NUMA links in 16-node system? C(16,2) = ?
```

---

## PHRASE 10: "KSWAPD vs DIRECT RECLAIM"

### RECLAIM WATERMARKS
```
YOUR MACHINE (15 GB RAM, 3.9M pages):
┌────────────────┬────────────────┬────────────────────────────────────────────┐
│ Watermark      │ Value (pages)  │ Trigger                                    │
├────────────────┼────────────────┼────────────────────────────────────────────┤
│ pages_high     │ 32,000         │ kswapd stops reclaiming                    │
│ pages_low      │ 24,000         │ kswapd starts background reclaim           │
│ pages_min      │ 16,000         │ Direct reclaim (blocks allocator)          │
├────────────────┼────────────────┼────────────────────────────────────────────┤
│ Hysteresis     │ 8,000 pages    │ = 32 MB buffer                             │
└────────────────┴────────────────┴────────────────────────────────────────────┘

LATENCY COMPARISON:
  kswapd (background):
    Scan rate = 100 pages/ms
    To free 1000 pages = 10 ms
    CPU usage = ~0.1% (idle priority)

  Direct reclaim (foreground):
    Scan rate = 1000 pages/ms
    To free 1000 pages = 1 ms
    CPU usage = 100% (blocks caller)
    Latency added to malloc() = 1-100 ms

RECLAIM PRIORITY ESCALATION:
  01. Priority 12 (initial): Scan 1/4096 of LRU
  02. Priority 6 (medium): Scan 1/64 of LRU
  03. Priority 0 (desperate): Scan entire LRU
  04. Each priority halves scan targets

YOUR TASK:
  [ ] If free pages = 20,000, which daemon is active? (kswapd)
  [ ] If free pages = 10,000, what happens? (direct reclaim + potential OOM)
  [ ] How long for kswapd to reclaim 8,000 pages? (8000/100 = ? ms)
```

---

## PHRASE 11: "COMPACTION vs FRAGMENTATION"

### FRAGMENTATION INDEX
```
GIVEN:
  total_pages = 100,000
  free_pages = 50,000 (50% free)
  largest_contiguous_block = 128 pages

CALCULATE FRAGMENTATION:
  01. Ideal (no fragmentation): largest_block = 50,000 pages
  02. Actual: largest_block = 128 pages
  03. Fragmentation index = 1 - (128 / 50,000) = 1 - 0.00256 = 99.74%
  04. Interpretation: 99.74% fragmented = SEVERE

COMPACTION ALGORITHM:
  ┌─────────────────────────────────────────────────────────────────────────────┐
  │ BEFORE COMPACTION:                                                          │
  │ [Used][Free][Used][Free][Free][Used][Free][Used][Free][Free][Free][Used]   │
  │                                                                             │
  │ AFTER COMPACTION (migrate scanner + free scanner):                          │
  │ [Used][Used][Used][Used][Free][Free][Free][Free][Free][Free][Free][Free]   │
  └─────────────────────────────────────────────────────────────────────────────┘

COMPACTION COST:
  01. Pages to migrate = 4 (used pages between free chunks)
  02. Migration cost = 2 memcpy per page = 2 × 4 KB × 4 = 32 KB data moved
  03. Time = 32 KB / 10 GB/s = 3.2 µs
  04. TLB flush cost = 1536 × 1 ns = 1.5 µs
  05. Total = 4.7 µs for this simple case

SUCCESS RATE BY FRAGMENTATION:
  ┌───────────────────┬─────────────────┐
  │ Fragmentation %   │ Success Rate    │
  ├───────────────────┼─────────────────┤
  │ < 20%             │ 95%             │
  │ 20% - 50%         │ 70%             │
  │ 50% - 70%         │ 30%             │
  │ > 70%             │ 10%             │
  └───────────────────┴─────────────────┘

YOUR TASK:
  [ ] If 256 contiguous pages needed, and largest block = 128, what happens?
  [ ] Calculate migrations needed to create 256-page block from 64×4 scattered
  [ ] What is kcompactd CPU usage if it runs 500µs every 5000ms?
```

---

## PHRASE 12: "OOM KILLER"

### OOM SCORE CALCULATION
```
FORMULA:
  oom_score = (RSS / total_ram) × 1000 + oom_score_adj

EXAMPLE PROCESSES:
  ┌───────────┬─────────────┬───────────────┬────────────────┬────────────┐
  │ Process   │ RSS (MB)    │ oom_score_adj │ oom_score      │ Kill Order │
  ├───────────┼─────────────┼───────────────┼────────────────┼────────────┤
  │ chrome    │ 2000        │ 0             │ 2000/15000×1000│ 133 → 1st  │
  │ mysqld    │ 1500        │ -500          │ 100-500 = -400 │ -400 → 4th │
  │ sshd      │ 10          │ -1000         │ 0.7-1000       │ -999 → 5th │
  │ java      │ 3000        │ +200          │ 200+200        │ 400 → 1st! │
  │ nginx     │ 500         │ 0             │ 33             │ 33 → 3rd   │
  └───────────┴─────────────┴───────────────┴────────────────┴────────────┘

OOM DECISION TREE:
  01. Free memory < min_watermark for all zones
  02. Reclaim attempted, failed
  03. Compaction attempted, failed
  04. OOM triggered
  05. For each process:
      - Skip if oom_score_adj == -1000 (OOM_SCORE_ADJ_MIN)
      - Skip if PF_OOMKILLER already set (being killed)
      - Calculate oom_score
  06. Select highest oom_score
  07. Send SIGKILL

RECOVERY TIME:
  01. OOM kill: 1 ms (signal) + 10 ms (process exit) = 11 ms
  02. Memory freed by 2GB process: 2 GB instantaneous
  03. Reclaim rate: 200 MB / 100 ms = 2 GB / 1000 ms
  04. OOM is 100× faster but DESTRUCTIVE

YOUR TASK:
  [ ] Process uses 8 GB on 16 GB system, oom_score_adj=0. What is oom_score?
  [ ] How to make a process unkillable? (echo -1000 > /proc/PID/oom_score_adj)
  [ ] What happens if all processes have oom_score_adj = -1000? (kernel panic)
```

---

## LESSON SEQUENCE

```
ORDER OF ATTACK (easiest → hardest):
┌─────┬──────────────────────────────────┬────────────┬───────────────────────┐
│ Day │ Topic                            │ Difficulty │ Prerequisites         │
├─────┼──────────────────────────────────┼────────────┼───────────────────────┤
│ 1   │ Page sizes (4KB, 2MB, 1GB)       │ 2/10       │ Binary arithmetic     │
│ 2   │ TLB calculations                 │ 3/10       │ Day 1                 │
│ 3   │ Page table walk (4-level)        │ 4/10       │ Day 1, 2              │
│ 4   │ Huge pages (THP vs hugetlbfs)    │ 4/10       │ Day 1, 3              │
│ 5   │ Zones (DMA, NORMAL, HIGHMEM)     │ 5/10       │ Day 3                 │
│ 6   │ Reclaim (kswapd, direct)         │ 6/10       │ Day 5                 │
│ 7   │ Compaction                       │ 6/10       │ Day 5, 6              │
│ 8   │ OOM killer                       │ 5/10       │ Day 6                 │
│ 9   │ NUMA topology                    │ 8/10       │ Day 3, 5, 6           │
│ 10  │ Full system integration          │ 9/10       │ All above             │
└─────┴──────────────────────────────────┴────────────┴───────────────────────┘
```

---

## FAILURE PREDICTIONS

```
F01. Variable confusion: "pages" vs "bytes" → 4096× error
F02. Mask error: 0x1FF vs 0xFF → 9 bits vs 8 bits
F03. Shift direction: >> vs << → Index extraction vs address construction
F04. Zone fallback: Assuming ZONE_DMA can fallback to ZONE_NORMAL (it cannot)
F05. NUMA latency: Using bandwidth numbers instead of latency
F06. TLB coverage: Forgetting to multiply entries × page_size
F07. Huge page alignment: 2MB page base must have low 21 bits = 0
F08. OOM score: Forgetting oom_score_adj is ADDED, not multiplied
F09. Reclaim direction: kswapd is BACKGROUND, direct is FOREGROUND
F10. Compaction: Confusing "free pages" with "contiguous free pages"
```

---

## NEXT STEPS

1. Load kernel module: `sudo insmod pagewalk_driver.ko`
2. Walk stack address: `echo "stack" > /proc/pagewalk && cat /proc/pagewalk`
3. Compare output indices with manual calculations above
4. Verify each attack vector with real data from your machine
