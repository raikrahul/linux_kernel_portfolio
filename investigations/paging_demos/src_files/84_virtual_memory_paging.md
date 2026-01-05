84. Virtual Memory Page Table Mechanics
01. PROBLEM_STATEMENT: Process requests 1MB contiguous allocation → 262,144 pages (4KB each) scattered in physical RAM → OS presents illusion of contiguous virtual addresses → HOW?
02. INPUT→OUTPUT: Virtual_Addr=0x00400000 → Page_Table_Lookup → Physical_Addr=0x7FE30000, 1 virtual→1 physical, 262,144 mappings for 1MB, TLB_Miss=~100 cycles, TLB_Hit=~1 cycle
03. DATA_STRUCTURE: Page_Table[VPN] → PPN, VPN=Virtual_Page_Number, PPN=Physical_Page_Number, Page_Size=4096=0x1000=4KB, Offset=0..4095=0..0xFFF=12 bits
04. TRAP_F1: Page_Size=4096 ✗ 4000 → 4096=2^12 → offset_bits=12, VPN_bits=52 (64-bit addr - 12 offset), but only 48 bits used in practice → VPN_bits=48-12=36
05. TRAP_F2: Contiguous_Virtual ≠ Contiguous_Physical → V[0x00400000..0x004FFFFF] can map to P[0x7FE30000,0x1A5000,0x8C2000,...scattered]
06. CALC_01: 1MB = 1,048,576 bytes, Pages_Needed = 1,048,576 / 4,096 = 256 pages (0x100 pages), CHECK: 256 × 4096 = 1,048,576 ✓
07. CALC_02: Virtual_Addr=0x00412ABC → VPN = 0x00412ABC >> 12 = 0x00412A = 16,682, Offset = 0x00412ABC & 0xFFF = 0xABC = 2,748, DECODE: Page_16682, Byte_2748_within_page
08. CALC_03: Page_Table[16,682] = 0x7FE30 (PPN), Physical_Addr = (0x7FE30 << 12) | 0xABC = 0x7FE30ABC, VERIFY: 0x7FE30 × 4096 + 2748 = 0x7FE30ABC ✓
09. TLB_STRUCTURE: Translation_Lookaside_Buffer = Cache[VPN] → PPN, Size = 64 entries (typical), Miss → Walk_Page_Table → ~100 cycles, Hit → 1 cycle
10. CALC_04: Array_Access_Pattern: arr[i] where sizeof(int)=4, Stride=4 bytes, Per_Page = 4096/4 = 1024 ints, After 1024 accesses → Next_Page → Potential_TLB_Miss
11. PAGE_TABLE_WALK: CR3_Register → Page_Map_Level_4 → Page_Directory_Pointer → Page_Directory → Page_Table → Physical_Page, 4 levels × ~100 cycles = 400 cycles per TLB miss
12. CALC_05: Working_Set = 256 pages, TLB = 64 entries, 256 > 64 → TLB_Thrashing_Guaranteed for full sweep, Every 64 new pages → evict old, revisit → miss
13. DRAW_VIRTUAL_TO_PHYSICAL:
```
Virtual Address Space (Process View):
┌───────────────────────────────────────────────────────────────────┐
│ 0x00400000 [Page 0x400] → Physical 0x7FE30000                     │
│ 0x00401000 [Page 0x401] → Physical 0x1A5000 (scattered!)          │
│ 0x00402000 [Page 0x402] → Physical 0x8C2000 (scattered!)          │
│ 0x00403000 [Page 0x403] → Physical 0x3210000                      │
│ ...                                                               │
│ 0x004FF000 [Page 0x4FF] → Physical 0x5550000                      │
└───────────────────────────────────────────────────────────────────┘
         ↓ Page Table Lookup ↓
Physical RAM (Actual Hardware):
┌────────┬────────┬────────┬────────┬────────┬────────┐
│0x1A5000│0x3210..│0x5550..│...     │0x7FE30.│0x8C2000│
│ pg 0x401│ pg 0x403│ pg 0x4FF│        │ pg 0x400│ pg 0x402│
└────────┴────────┴────────┴────────┴────────┴────────┘
```
14. ADDRESS_DECODE_FORMULA: Virtual_Addr = (VPN × Page_Size) + Offset, Physical_Addr = (Page_Table[VPN] × Page_Size) + Offset, VPN = Virtual_Addr >> 12, Offset = Virtual_Addr & 0xFFF
15. CALC_06: Sequential_Read_1MB: 256 pages, TLB=64, Unique_Pages_Per_64_Accesses = 64, TLB_Misses = 256/64 × 64 = ? NO → TLB_Misses = 256 (each new page = miss once), Amortized_Cost = 256 × 100_cycles / 1,048,576_bytes = 0.024 cycles/byte
16. CALC_07: Random_Access_1MB: Each access = different page with probability, TLB_Hit_Rate = 64/256 = 25%, Miss_Rate = 75%, Per_Access_Cost = 0.25 × 1 + 0.75 × 100 = 75.25 cycles average
17. TRAP_F3: Pointer_Chase_Worst_Case: ptr = ptr->next where each node on different page, 100% TLB miss rate, 100 cycles per dereference, Linked_List = 256× slower than sequential
18. CALC_08: 4-Level_Page_Table_Walk: L4_Read + L3_Read + L2_Read + L1_Read + Data_Read = 5 memory accesses, 5 × 100 = 500 cycles worst case, TLB caches intermediate levels too
19. PERFORMANCE_MODEL: Time = (TLB_Hits × 1 + TLB_Misses × 100) + Memory_Accesses × 100, Optimize: Increase_Locality → Reduce_TLB_Misses
20. HOMEWORK_CALC_01: Virtual_Addr = 0x7FFF00001234, Page_Size = 4KB, EXTRACT: VPN = ?, Offset = ?, Show_Work_By_Hand
21. HOMEWORK_CALC_02: Array int arr[1000000], sizeof(int)=4, Total_Bytes = ?, Pages_Needed = ?, TLB_Misses_Sequential = ?, Show_Division_With_Remainder
22. HOMEWORK_CALC_03: Stride_Access arr[i*1024] for i=0..999, Accesses_Per_Page = ?, Pattern_Effect = ?, TLB_Performance = ?
23. EDGE_CASE_N1: Page_Size = 4096, Allocation = 4097 bytes → Pages = ceil(4097/4096) = 2 pages, Waste = 2 × 4096 - 4097 = 4095 bytes wasted (99.97% waste for +1 byte!)
24. EDGE_CASE_N2: Stack_Pointer = 0x7FFFFFFFE000, Stack_Grows_Down, Next_Page_Below = 0x7FFFFFFFD000, Guard_Page = Unmapped → Segfault on overflow
25. COMPLEXITY: Page_Table_Space = 2^36 VPNs × 8 bytes/entry = 512GB (impossible!) → Solution: Multi-Level_Paging, Only_Allocate_Used_Pages
26. MULTI_LEVEL_CALC: 4-Level: 9+9+9+9+12 = 48 bits, Each_Level = 512 entries (2^9), Entry_Size = 8 bytes, One_Page_Per_Level = 4KB, 4 × 4KB = 16KB for sparse address space vs 512GB flat

---
VIDEO TRANSCRIPT ANALYSIS (Dense Numerical Derivations)
---

27. [00:00:11→00:00:15] "allocate very large block...single contiguous piece"
```
malloc(2147483648) → 0x7F000000
├─ 2147483648 bytes / 4096 = 524288 pages needed
├─ Physical RAM = 8589934592 bytes = 2097152 pages total
├─ Free pages = 1104256 scattered in 973 blocks
├─ Largest contiguous = 134217728 bytes = 32768 pages < 524288 ✗
├─ Virtual illusion: VA 0x7F000000-0xFF000000 appears contiguous
├─ Physical reality: PA scattered at 0xC0001000, 0xE0001000, 0xF0001000...
└─ Page table: 524288 PTEs × 8 bytes = 4194304 bytes overhead
```
28. TRAP: "contiguous" = virtual only → 524288 separate physical pages → OS hides fragmentation → user sees linear 0x7F000000,0x7F000001,0x7F000002...

29. [00:00:15→00:00:18] "other processes...might not be enough memory"
```
System state:
├─ 187 processes running
├─ Each process avg 45088768 bytes = 11010 pages
├─ Total allocated = 187 × 45088768 = 8428599296 bytes
├─ RAM = 8589934592 bytes
├─ Free = 8589934592 - 8428599296 = 161335296 bytes = 39390 pages
├─ BUT: fragmented into 973 blocks
├─ Largest block = 134217728 bytes < 2147483648 bytes needed ✗
└─ Virtual memory solution: map 39390 scattered pages to contiguous VA
```
30. CALC: Process table overhead = 187 × 2048 bytes = 383296 bytes, CR3 values: 0x3A4000, 0x5B8000, 0x7CC000... each process has own page tables

31. [00:00:18→00:00:22] "OS assembles free blocks...virtual memory"
```
Assembly process:
├─ Block 0: 0xC0000000, 131072 pages (536870912 bytes)
├─ Block 1: 0xE0000000, 65536 pages (268435456 bytes)
├─ Block 2: 0xF0000000, 32768 pages (134217728 bytes)
├─ Block 3: 0xF8000000, 16384 pages (67108864 bytes)
├─ ...973 blocks total...
├─ Sum = 1104256 pages = 4524778496 bytes free
├─ Need 524288 pages for 2GB allocation
├─ After alloc: 1104256 - 524288 = 579968 pages remain
└─ PTE writes: PT[0x1FC00]=0xC0001007, PT[0x1FC01]=0xC0002007...
```
32. PAGE_FAULT_TRACE: First access 0x7F000000 → TLB miss → CR3=0x3A4000 → PML4[0]=0x5B9003 → PDPT[0]=0x7CC003 → PD[0x1FC]=0x9DD003 → PT[0]=0xC0001007 → PA=0xC0001000 → 400 cycles

33. [00:00:22→00:00:25] "illusion...continuous...cost"
```
Cost breakdown per memory access:
├─ TLB hit: 1 cycle = 0.4ns @ 2.5GHz
├─ TLB miss: 100 cycles = 40ns
├─ Page table walk: 4 levels × 100ns = 400ns
├─ L1 cache hit: 4 cycles = 1.6ns
├─ L3 cache hit: 40 cycles = 16ns
├─ RAM access: 100ns
│
Sequential 2GB scan:
├─ 524288 pages → 524288 TLB misses (first access each page)
├─ TLB size = 64 → after 64 pages, eviction starts
├─ Cost = 524288 × 100 cycles = 52428800 cycles = 21ms @ 2.5GHz
├─ vs physical allocation: 0 TLB overhead (no translation)
└─ Overhead = 21ms per 2GB scan
```
34. TLB_MATH: 2GB = 524288 pages, TLB = 64 entries, 524288/64 = 8192 full TLB flushes, Each flush = 64 misses × 100 cycles = 6400 cycles, Total = 8192 × 6400 = 52,428,800 cycles ✓

35. [00:00:28→00:00:33] "process...main memory...green box"
```
Process 4567 memory layout:
├─ 0x00400000-0x0040B000: code (45056 bytes, 11 pages)
├─ 0x00600000-0x00800000: data (2097152 bytes, 512 pages)
├─ 0x00800000-0x00D08000: heap (5373952 bytes, 1312 pages)
├─ 0x7FFE8000-0x7FFFFFFF: stack (98304 bytes, 24 pages)
├─ Total = 11 + 512 + 1312 + 24 = 1859 pages
├─ 1859 × 4096 = 7614464 bytes = 7.26 MB
├─ Page table for 1859 pages: 1859 × 8 = 14872 bytes
└─ CR3 = 0x8F56000, PML4 at physical 0x8F56000
```
36. VISUAL_MEMORY: Green box 640×480 = 307200 pixels × 4 bytes = 1228800 bytes = 300 pages, malloc(1228800) = 0x7A2B4000 → 300 PTEs at 0x3E95C000-0x3E95C958

37. [00:00:33→00:00:36] "RAM = Random Access Memory"
```
DDR4-3200 specs:
├─ Frequency: 1600 MHz (DDR = double)
├─ Transfer rate: 3200 MT/s
├─ Bus width: 64 bits = 8 bytes
├─ Bandwidth: 3200 × 8 = 25600 MB/s = 25.6 GB/s
├─ Latency: CL=16 cycles @ 1600 MHz = 10ns CAS
├─ Total access: 100ns (includes RAS, CAS, precharge)
├─ 8GB = 8589934592 bytes = 536870912 × 16-byte blocks
└─ Random access: any address in 100ns (±5ns variance)
```
38. RAM_ADDRESSING: Address 0x12345678 → Channel = (0x12345678/256)%2 = 0 → Chip = (0x12345678/8)%8 = 7 → Row = 0x91A2 → Col = 0x678

39. [00:00:36→00:00:40] "CPU executes...memory frequently"
```
Memory access frequency:
├─ CPU: 2.5 GHz = 2,500,000,000 cycles/second
├─ IPC: 2.0 → 5,000,000,000 instructions/second
├─ Memory ops: 45% of instructions
├─ Memory accesses: 5×10^9 × 0.45 = 2.25×10^9/second
├─ Per access: 100ns RAM latency
├─ Total memory time: 2.25×10^9 × 100ns = 225 seconds/second ✗
├─ Impossible → cache needed
├─ L1 hit rate 95%: 2.25×10^9 × 0.95 × 1ns = 2.14 seconds
├─ L1 miss → L3: 2.25×10^9 × 0.04 × 15ns = 1.35 seconds
├─ L3 miss → RAM: 2.25×10^9 × 0.01 × 100ns = 2.25 seconds
└─ Total: 2.14 + 1.35 + 2.25 = 5.74 seconds/second of memory time
```
40. CACHE_HIERARCHY: L1=32KB (4 cycles), L2=256KB (12 cycles), L3=8MB (40 cycles), RAM=8GB (250 cycles), Ratio: 1:3:10:62.5

41. [00:00:40→00:00:47] "memory transactions...critical for performance"
```
Transaction analysis for array sum:
├─ int arr[1048576] = 4194304 bytes = 1024 pages
├─ Sequential access: sum += arr[i]
├─ Cache line = 64 bytes = 16 ints
├─ Cache line fills: 4194304/64 = 65536
├─ L1 capacity: 32768 bytes = 512 lines
├─ L3 capacity: 8388608 bytes = 131072 lines
├─ Array fits in L3 ✓
├─ First pass: 65536 L3 misses × 40 cycles = 2621440 cycles = 1.05ms
├─ Second pass: 65536 L3 hits × 10 cycles = 655360 cycles = 0.26ms
├─ TLB: 1024 pages, TLB=64, misses = 1024 × 100 = 102400 cycles = 41μs
└─ Total first pass: 1.05ms + 0.041ms = 1.09ms
```
42. STRIDE_DISASTER: arr[i×1024] stride = 4096 bytes = 1 page, Every access = new page = TLB miss, 1024 accesses × 100 cycles = 102400 cycles = 41μs TLB overhead alone

43. 4-LEVEL_PAGE_TABLE_WALK:
```
Virtual Address: 0x00007F1234567890
├─ Bits [63:48] = 0x0000 (sign extension, must equal bit 47)
├─ Bits [47:39] = (0x7F1234567890 >> 39) & 0x1FF = 0xFE = 254 (PML4 index)
├─ Bits [38:30] = (0x7F1234567890 >> 30) & 0x1FF = 0x048 = 72 (PDPT index)
├─ Bits [29:21] = (0x7F1234567890 >> 21) & 0x1FF = 0x1A2 = 418 (PD index)
├─ Bits [20:12] = (0x7F1234567890 >> 12) & 0x1FF = 0x167 = 359 (PT index)
├─ Bits [11:0] = 0x7F1234567890 & 0xFFF = 0x890 = 2192 (offset)
│
Walk sequence (each step = memory read = 100ns):
├─ CR3 = 0x0000000003A4000 (PML4 base)
├─ PML4[254] @ 0x3A4000 + 254×8 = 0x3A47F0 → value 0x5B9003 → PDPT at 0x5B9000
├─ PDPT[72] @ 0x5B9000 + 72×8 = 0x5B9240 → value 0x7CC003 → PD at 0x7CC000
├─ PD[418] @ 0x7CC000 + 418×8 = 0x7CCD10 → value 0x9DD003 → PT at 0x9DD000
├─ PT[359] @ 0x9DD000 + 359×8 = 0x9DDB38 → value 0xC4567007 → Frame 0xC4567
└─ Physical: (0xC4567 << 12) | 0x890 = 0xC4567890
```
44. PTE_BITS: 0xC4567007 = Present(1) + R/W(1) + User(1) + PWT(0) + PCD(0) + Accessed(0) + Dirty(0) + PAT(0) + Global(0) + Frame(0xC4567)

45. BUDDY_ALLOCATOR:
```
Free list structure:
├─ Order 0 (4KB): 1104256 blocks
├─ Order 1 (8KB): 552128 blocks
├─ Order 2 (16KB): 276064 blocks
├─ Order 3 (32KB): 138032 blocks
├─ Order 4 (64KB): 69016 blocks
├─ Order 5 (128KB): 34508 blocks
├─ Order 6 (256KB): 17254 blocks
├─ Order 7 (512KB): 8627 blocks
├─ Order 8 (1MB): 4313 blocks
├─ Order 9 (2MB): 2156 blocks
├─ Order 10 (4MB): 1078 blocks
├─ Order 11 (8MB): 539 blocks
└─ Need order 19 (2GB) = 1 block → NOT AVAILABLE ✗
```
46. FRAGMENTATION_MATH: 973 blocks, avg size = 4524778496/973 = 4651107 bytes, Need 2147483648 bytes contiguous, Largest = 134217728 bytes, Gap = 2147483648 - 134217728 = 2013265920 bytes short

47. FAILURE_PREDICTIONS:
```
F1. VPN extraction: 0x00412ABC >> 12 = 0x412A ✓ NOT 0x412 ✗
F2. Offset mask: 0xFFF = 4095 ✓ NOT 0x1000 = 4096 ✗
F3. Page count: 2GB/4KB = 524288 ✓ NOT 512000 ✗
F4. TLB miss cost: 100 cycles ✓ NOT 10 cycles ✗
F5. 4-level indices: 9+9+9+9+12=48 ✓ NOT 10+10+10+10+12=52 ✗
F6. Stride 1024 ints × 4 bytes = 4096 bytes = 1 page ✓ NOT 1024 bytes ✗
F7. Physical address: (PPN << 12) | offset ✓ NOT PPN + offset ✗
F8. TLB size 64 < pages 524288 → thrashing ✓ NOT "fits" ✗
F9. mmap alignment: low 12 bits = 0 ✓ (page aligned)
F10. Sign extension: bits 63:48 must equal bit 47 ✓
```

48. HOMEWORK_DERIVATIONS:
```
CALC_A: VA = 0x7FFF00001234
├─ VPN = 0x7FFF00001234 >> 12 = 0x7FFF00001
├─ Offset = 0x7FFF00001234 & 0xFFF = 0x234 = 564
├─ PML4 = (0x7FFF00001234 >> 39) & 0x1FF = 0xFF = 255
├─ PDPT = (0x7FFF00001234 >> 30) & 0x1FF = 0x1FC = 508
├─ PD = (0x7FFF00001234 >> 21) & 0x1FF = 0 = 0
├─ PT = (0x7FFF00001234 >> 12) & 0x1FF = 1 = 1
└─ If PT[1] = 0xABCDE007 → PA = 0xABCDE234

CALC_B: Array int arr[2000000]
├─ Total bytes = 2000000 × 4 = 8000000
├─ Pages = ceil(8000000 / 4096) = ceil(1953.125) = 1954
├─ TLB misses (sequential) = 1954
├─ TLB cost = 1954 × 100 = 195400 cycles = 78μs @ 2.5GHz
├─ Ints per page = 4096 / 4 = 1024
└─ Accesses per TLB miss amortized = 1024

CALC_C: Linked list 10000 nodes, node size 16 bytes
├─ Best case (contiguous): pages = ceil(160000/4096) = 40
├─ Best TLB cost = 40 × 100 = 4000 cycles
├─ Worst case (scattered): pages = 10000 (1 node per page)
├─ Worst TLB cost = 10000 × 100 = 1000000 cycles
├─ Slowdown = 1000000 / 4000 = 250×
└─ Conclusion: locality matters 250× for TLB
```
49. SPACE_COMPLEXITY: Page_Table_Memory = 524288_PTEs × 8_bytes = 4194304_bytes = 4MB, Overhead = 4MB / 2GB = 0.2%, O(n) where n = virtual_pages
50. TIME_COMPLEXITY: TLB_Hit = O(1) = 1_cycle, TLB_Miss = O(4) = 4_memory_accesses × 100_cycles = 400_cycles, Amortized = O(1) with good locality
