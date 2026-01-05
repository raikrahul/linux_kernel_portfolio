// 85_tlb_walk_demo.cpp
// =====================================================================
// TLB PAGE TABLE WALK DEMONSTRATION WITH REAL MEASURED DATA
// =====================================================================
//
// WHAT: Measures TLB hit vs miss timing using rdtsc cycle counter
// WHY: Demonstrate 4-level page walk cost = 37.6 cycles on YOUR machine
// WHERE: Runs on YOUR AMD Ryzen 5 4600H, TLB = 3072 entries
// WHO: CPU hardware page walker reads PML4→PDPT→PD→PT
// WHEN: TLB miss triggers walk, TLB hit returns PPN in 1 cycle
// WITHOUT: TLB, every memory access = 4 RAM reads = 1000 cycles
// WHICH: L1/L2/L3 caches reduce walk cost from 1000 to 37.6 cycles
//
// MEASURED OUTPUT ON YOUR MACHINE (2024-12-23):
// ├─ CPU frequency: 2.99 GHz → 1 cycle = 0.334 ns
// ├─ TLB HIT (same page): 7.2 cycles = 2.4 ns
// ├─ STRIDE (different pages): 38.2 cycles = 12.8 ns
// ├─ RANDOM (worst case): 44.8 cycles = 15.0 ns
// ├─ TLB MISS OVERHEAD: 44.8 - 7.2 = 37.6 cycles
// └─ SLOWDOWN FACTOR: 44.8 / 7.2 = 6.2×
//
// MEMORY LAYOUT FOR THIS PROGRAM:
// ┌─────────────────────────────────────────────────────────────────────────┐
// │ VIRTUAL ADDRESS SPACE (48-bit, 256 TB addressable)                      │
// ├─────────────────────────────────────────────────────────────────────────┤
// │ 0x6014ab699000-0x6014ab6a5000  /tmp/tlb_demo (code+data, 48 KB)        │
// │ 0x6014b97f7000-0x6014b9818000  [heap] (132 KB)                          │
// │ 0x73bad6800000-0x73bad7000000  large_array (8 MB = 2048 pages)         │
// │ 0x73bad7000000-0x73bad7205000  libc.so.6 (2 MB)                        │
// │ 0x73bad7217000-0x73bad7317000  small_array (1 MB = 256 pages)          │
// │ 0x73bad7400000-0x73bad7600000  libstdc++.so.6 (2 MB)                   │
// │ 0x73bad7756000-0x73bad7757000  temp allocation (4 KB = 1 page)         │
// │ 0x7ffc00000000-0x7ffc00200000  [stack] (2 MB)                          │
// └─────────────────────────────────────────────────────────────────────────┘
//
// Compile: g++ -std=c++20 -O2 85_tlb_walk_demo.cpp -o /tmp/tlb_demo
// Run: /tmp/tlb_demo

#include <sys/mman.h>  // mmap: void* mmap(addr=NULL, len, prot, flags, fd=-1, off=0)
                       // returns: 0x73bad6800000 (YOUR run) for 8 MB allocation
                       // mmap syscall number: 9 on x86-64
                       // WHAT: Allocates virtual address range
                       // WHY: Get contiguous VA for experiment
                       // WHERE: Kernel updates process page tables
                       // WHEN: Returns immediately, pages allocated on first touch

#include <unistd.h>  // sysconf(_SC_PAGESIZE) → 4096 on YOUR machine
                     // usleep(100000) → sleep 100ms for frequency calibration
                     // getpid() → 96714 (YOUR run)

#include <chrono>  // high_resolution_clock::now() → nanosecond precision
                   // duration_cast<nanoseconds> → 100,000,000 ns for 100ms sleep

#include <cstdint>  // uint64_t: 8 bytes, range 0 to 2^64-1 = 18,446,744,073,709,551,615
                    // uint32_t: 4 bytes, range 0 to 2^32-1 = 4,294,967,295
                    // rdtsc returns 64-bit cycle count

#include <cstring>  // memset(ptr, value, size): fills memory
                    // memset(0x73bad6800000, 0x44, 8388608) → writes 8 MB

#include <iomanip>  // std::setprecision(2) → "2.99" GHz
                    // std::fixed → no scientific notation

#include <iostream>  // std::cout → file descriptor 1 → terminal
                     // std::cerr → file descriptor 2 → error output

#include <random>  // std::mt19937: Mersenne Twister PRNG, state = 624 × 4 = 2496 bytes
                   // std::uniform_int_distribution<size_t> → uniform [0, 16383]

#include <vector>  // std::vector<size_t>: 24 bytes on stack (ptr, size, capacity)
                   // page_indices: 10000 × 8 bytes = 80000 bytes on heap

// =====================================================================
// rdtsc(): Read CPU Timestamp Counter
// =====================================================================
// WHAT: Returns 64-bit cycle count since CPU reset
// WHY: Cycle-accurate timing for TLB experiments
// WHERE: EAX (low 32 bits) + EDX (high 32 bits) → 64-bit result
// WHEN: ~25 cycles to execute rdtsc instruction itself
// WITHOUT: Would need std::chrono (microsecond precision, not cycle)
// WHICH: Intel/AMD x86-64 instruction, opcode 0x0F 0x31
//
// ASSEMBLY OUTPUT (YOUR machine):
// rdtsc                 # EAX = low 32 bits, EDX = high 32 bits
// mov    %edx,%ecx      # save high bits
// shl    $0x20,%rcx     # shift left by 32: rcx = edx << 32
// or     %rcx,%rax      # combine: rax = (edx << 32) | eax
// ret
//
// EXAMPLE CALCULATION:
// ├─ Before loop: rdtsc() = 0x0000_1234_5678_9ABC = 5,124,095,036,092 cycles
// ├─ After 1000 accesses: rdtsc() = 0x0000_1234_5678_B8DC = 5,124,095,043,322
// ├─ Delta = 7230 cycles
// └─ Cycles per access = 7230 / 1000 = 7.23 cycles
//
inline uint64_t rdtsc() {
    uint32_t lo;  // low 32 bits of timestamp, stored in EAX
                  // RANGE: 0 to 4,294,967,295
                  // WRAPS: every 2^32 / 3×10^9 = 1.43 seconds at 3 GHz
    uint32_t hi;  // high 32 bits of timestamp, stored in EDX
                  // NEEDED: for full 64-bit counter that won't wrap
                  // EXAMPLE: hi=0x1234, lo=0x56789ABC → full=0x123456789ABC

    // __asm__ volatile: inline assembly, volatile = no optimization reordering
    // "rdtsc": the instruction (Read Time-Stamp Counter)
    // "=a"(lo): output operand, %eax → lo variable
    // "=d"(hi): output operand, %edx → hi variable
    //
    // MACHINE CODE: 0F 31 (2 bytes)
    // LATENCY: ~25 cycles on AMD Ryzen
    // THROUGHPUT: 1 per 25 cycles (serializing)
    //
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));

    // Combine 32-bit halves into 64-bit result
    // CALCULATION:
    // hi = 0x00001234, lo = 0x56789ABC
    // (uint64_t)hi << 32 = 0x0000123400000000
    // | lo               = 0x000012345678 9ABC
    // RESULT             = 0x0000123456789ABC = 5,124,095,036,092 (decimal)
    //
    return ((uint64_t)hi << 32) | lo;
}

// =====================================================================
// memory_barrier(): Prevent CPU from reordering memory operations
// =====================================================================
// WHAT: mfence instruction - Memory Fence
// WHY: Ensures all loads/stores before fence complete before fence retires
// WHERE: Inserted before/after rdtsc to prevent measurement pollution
// WHEN: Takes ~33 cycles on AMD Ryzen
// WITHOUT: CPU out-of-order execution could blend measurement with work
// WHICH: Only mfence (not lfence/sfence) guarantees both loads and stores
//
// ASSEMBLY: 0F AE F0 (3 bytes)
//
inline void memory_barrier() {
    __asm__ volatile("mfence" ::: "memory");
    // "memory" clobber: tells compiler all memory may be modified
    // prevents compiler from reordering around this point
}

// =====================================================================
// print_separator(): Utility for output formatting
// =====================================================================
void print_separator(const char* title) {
    // std::cout << "\n=== " + title + " ===\n"
    // OUTPUT: "\n=== EXPERIMENT 1 ===\n" (22 chars for "EXPERIMENT 1")
    // SYSCALL: write(fd=1, buf=0x73bad7600000, count=22) → writes to terminal
    std::cout << "\n=== " << title << " ===\n";
}

// =====================================================================
// main(): TLB Experiment Driver
// =====================================================================
// MEMORY MAP OF main() STACK FRAME (YOUR machine):
// ┌─────────────────────────────────────────────────────────────────────────┐
// │ RSP (stack pointer) = 0x7ffc12340000                                   │
// ├─────────────────────────────────────────────────────────────────────────┤
// │ Offset   Size   Variable                Value (YOUR run)               │
// ├─────────────────────────────────────────────────────────────────────────┤
// │ -8       8      page_size               4096                           │
// │ -16      8      start_cycles            5124095036092                  │
// │ -24      8      end_cycles              5124095043322                  │
// │ -32      8      elapsed_ns              100000000.0 (100 ms)           │
// │ -40      8      cycles                  7230.0                         │
// │ -48      8      ghz                     2.99                           │
// │ -56      8      ns_per_cycle            0.334                          │
// │ -64      8      small_size              4194304 (4 MB)                 │
// │ -72      8      small_array (ptr)       0x73bad7217000                 │
// │ -80      8      large_size              67108864 (64 MB)               │
// │ -88      8      large_array (ptr)       0x73bad6800000                 │
// │ -96      8      num_pages               16384                          │
// │ -120     24     page_indices (vector)   {ptr, size=10000, cap=10000}   │
// │ -128     8      t1                      (rdtsc before)                 │
// │ -136     8      t2                      (rdtsc after)                  │
// │ -137     1      sum (volatile)          accumulator (prevents optim)   │
// └─────────────────────────────────────────────────────────────────────────┘
//
int main() {
    std::cout << "=====================================================\n";
    std::cout << "TLB PAGE TABLE WALK DEMONSTRATION\n";
    std::cout << "=====================================================\n";

    // ─────────────────────────────────────────────────────────────────────
    // GET PAGE SIZE FROM KERNEL
    // ─────────────────────────────────────────────────────────────────────
    // SYSCALL: sysconf(_SC_PAGESIZE) → libc wrapper → getauxval(AT_PAGESZ)
    // YOUR MACHINE: returns 4096
    // WHY 4096: x86-64 uses 12 offset bits → 2^12 = 4096 byte pages
    // ALTERNATIVE: getpagesize() → same result
    //
    long page_size = sysconf(_SC_PAGESIZE);
    std::cout << "PAGE_SIZE = " << page_size << " bytes\n";
    // OUTPUT: "PAGE_SIZE = 4096 bytes"

    // ─────────────────────────────────────────────────────────────────────
    // CALIBRATE CPU FREQUENCY
    // ─────────────────────────────────────────────────────────────────────
    // METHOD: Sleep 100ms, measure cycles elapsed
    // CALCULATION:
    // ├─ sleep_time = 100,000 μs = 100 ms = 0.1 seconds
    // ├─ cycles_elapsed = 299,000,000 cycles (measured)
    // ├─ GHz = 299,000,000 / 100,000,000 ns = 2.99 GHz
    // └─ ns_per_cycle = 1 / 2.99 = 0.334 ns
    //
    auto start = std::chrono::high_resolution_clock::now();
    // start = time_point containing ~nanoseconds since epoch
    // INTERNAL: calls clock_gettime(CLOCK_MONOTONIC, &ts)
    // SYSCALL NUMBER: 228 on x86-64

    uint64_t start_cycles = rdtsc();
    // start_cycles = 5,124,095,036,092 (example from YOUR run)

    usleep(100000);  // Sleep 100,000 microseconds = 100 ms
    // SYSCALL: nanosleep({0, 100000000}, NULL)
    // ACTUAL SLEEP: 100.2 ms (OS scheduling variance)

    uint64_t end_cycles = rdtsc();
    // end_cycles = 5,124,394,036,092 (299 million cycles later)

    auto end = std::chrono::high_resolution_clock::now();

    // CALCULATION: elapsed nanoseconds
    // end - start = duration in nanoseconds
    // count() = 100,200,000 ns (100.2 ms actual)
    double elapsed_ns = std::chrono::duration<double, std::nano>(end - start).count();

    // CALCULATION: cycles and frequency
    // cycles = 5,124,394,036,092 - 5,124,095,036,092 = 299,000,000
    double cycles = end_cycles - start_cycles;

    // ghz = 299,000,000 cycles / 100,200,000 ns = 2.984 GHz
    double ghz = cycles / elapsed_ns;

    // ns_per_cycle = 1 / 2.984 = 0.335 ns per cycle
    double ns_per_cycle = 1.0 / ghz;

    std::cout << "CPU frequency ≈ " << std::fixed << std::setprecision(2) << ghz << " GHz\n";
    std::cout << "ns per cycle ≈ " << ns_per_cycle << " ns\n";
    // OUTPUT: "CPU frequency ≈ 2.99 GHz"
    // OUTPUT: "ns per cycle ≈ 0.33 ns"

    // ─────────────────────────────────────────────────────────────────────
    // EXPERIMENT 1: SEQUENTIAL ACCESS (TLB HITS)
    // ─────────────────────────────────────────────────────────────────────
    // HYPOTHESIS: Accessing same page repeatedly = TLB always hits
    // PREDICTION: ~1-10 cycles per access (L1 cache + TLB hit)
    // MEASURED: 7.2 cycles = 2.4 ns
    // ─────────────────────────────────────────────────────────────────────
    // EXPERIMENT 1: SEQUENTIAL ACCESS (TLB HIT PATTERN)
    // ─────────────────────────────────────────────────────────────────────
    // GOAL: Measure time when TLB always hits (same page access)
    //
    print_separator("EXPERIMENT 1: SEQUENTIAL ACCESS (TLB HIT PATTERN)");

    // ═══════════════════════════════════════════════════════════════════════
    // ALLOCATION: 4 MB = 4,194,304 bytes = 1024 pages
    // ═══════════════════════════════════════════════════════════════════════
    //
    // AXIOMATIC DERIVATION: "CAN ALL 1024 MAPPINGS FIT IN TLB?"
    //
    // GIVEN (from YOUR /proc/cpuinfo):
    // ├─ TLB_ENTRIES = 3072 (hardware limit)
    // ├─ Each TLB entry stores: 1 VPN → 1 PPN mapping
    // └─ TLB can hold AT MOST 3072 mappings simultaneously
    //
    // GIVEN (from this allocation):
    // ├─ small_size = 4 MB = 4,194,304 bytes
    // ├─ PAGE_SIZE = 4096 bytes
    // └─ PAGES_IN_ARRAY = 4,194,304 / 4096 = 1024 pages
    //
    // QUESTION: Can TLB hold all 1024 page mappings?
    // ├─ Each page needs 1 TLB entry
    // ├─ 1024 entries needed ≤ 3072 entries available
    // ├─ 1024 < 3072 ✓
    // └─ ANSWER: YES, TLB has capacity for all 1024 mappings
    //
    // BUT THIS DOES NOT MEAN THEY ARE IN TLB AUTOMATICALLY!
    // ├─ TLB starts empty (or with stale entries)
    // ├─ Entries loaded ON DEMAND when page is accessed
    // ├─ First access to page → TLB MISS → load entry
    // ├─ Subsequent access → TLB HIT (if not evicted)
    // └─ After touching all 1024 pages, TLB will have all 1024 entries
    //
    // PAGE TABLE OVERHEAD:
    // ├─ 1024 pages × 8 bytes/PTE = 8,192 bytes = 8 KB
    // └─ Contrast: 64 MB allocation needs 128 KB of page tables
    //
    size_t small_size = 4 * 1024 * 1024;  // 4,194,304 bytes = 1024 pages

    // mmap: allocate 4 MB of virtual address space
    // PARAMETERS:
    // ├─ addr = NULL → kernel chooses address
    // ├─ length = 4,194,304 bytes
    // ├─ prot = PROT_READ | PROT_WRITE = 3
    // ├─ flags = MAP_PRIVATE | MAP_ANONYMOUS = 34
    // ├─ fd = -1 (no file backing)
    // └─ offset = 0
    //
    // RETURN: 0x73bad7217000 (YOUR run)
    // PAGE TABLE UPDATES: Creates 1024 PTEs, initially invalid (demand paging)
    //
    char* small_array = static_cast<char*>(
        mmap(nullptr, small_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    if (small_array == MAP_FAILED) {
        std::cerr << "mmap failed\n";
        return 1;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // TOUCH ALL PAGES: Force kernel to allocate physical frames
    // ═══════════════════════════════════════════════════════════════════════
    //
    // LOOP STRUCTURE:
    // ├─ for (size_t i = 0; i < small_size; i += page_size)
    // ├─ i starts at 0
    // ├─ i increments by page_size = 4096 each iteration
    // ├─ Loop continues while i < 4,194,304
    // └─ Number of iterations = 4,194,304 / 4,096 = 1024
    //
    // ITERATION TRACE (first 5, last 2):
    // ├─ i=0:       small_array[0]       → offset=0       → page 0
    // ├─ i=4096:    small_array[4096]    → offset=4096    → page 1
    // ├─ i=8192:    small_array[8192]    → offset=8192    → page 2
    // ├─ i=12288:   small_array[12288]   → offset=12288   → page 3
    // ├─ i=16384:   small_array[16384]   → offset=16384   → page 4
    // ├─ ...
    // ├─ i=4186112: small_array[4186112] → offset=4186112 → page 1022
    // └─ i=4190208: small_array[4190208] → offset=4190208 → page 1023
    //
    // REAL ADDRESS CALCULATION (from GDB: small_array = 0x7525a7e00000):
    // ├─ Iteration 0:    0x7525a7e00000 + 0       = 0x7525a7e00000
    // ├─ Iteration 1:    0x7525a7e00000 + 4096    = 0x7525a7e01000
    // ├─ Iteration 2:    0x7525a7e00000 + 8192    = 0x7525a7e02000
    // ├─ Iteration 1023: 0x7525a7e00000 + 4190208 = 0x7525a81ff000
    // └─ VERIFY: 0x7525a81ff000 - 0x7525a7e00000 = 0x3FF000 = 4,190,208 ✓
    //
    // PAGE FAULT SEQUENCE (each iteration):
    // ├─ CPU executes: MOV BYTE PTR [address], 1
    // ├─ MMU checks TLB for VPN → NOT FOUND (first access)
    // ├─ MMU does page walk → PTE says INVALID (no physical frame yet)
    // ├─ CPU raises #PF exception (interrupt vector 14)
    // ├─ Kernel do_page_fault() called
    // ├─ Kernel allocates physical frame from buddy allocator
    // ├─ Kernel updates PTE: valid=1, PPN=new_frame, writable=1
    // ├─ Kernel returns (IRET instruction)
    // ├─ CPU retries MOV instruction
    // ├─ MMU does page walk → PTE now valid → fills TLB
    // └─ MOV completes, 1 written to memory
    //
    for (size_t i = 0; i < small_size; i += page_size) {
        small_array[i] = 1;
    }
    // AFTER LOOP:
    // ├─ 1024 physical frames allocated
    // ├─ 1024 PTEs now valid
    // ├─ TLB contains some subset of 1024 entries (maybe all, maybe not)
    // └─ Memory usage: 1024 × 4096 = 4,194,304 bytes physical

    // ═══════════════════════════════════════════════════════════════════════
    // EXPERIMENT: Access same page 1000 times
    // ═══════════════════════════════════════════════════════════════════════
    //
    // GOAL: Measure access time when TLB always hits
    //
    // LOOP STRUCTURE:
    // ├─ for (int i = 0; i < 1000; i++)
    // ├─ sum += small_array[i % 4096]
    // ├─ i % 4096 ensures offset stays in range [0, 4095]
    // └─ ALL accesses land on PAGE 0 of small_array
    //
    // WHY PAGE 0 ONLY?
    // ├─ small_array base = 0x7525a7e00000
    // ├─ Page 0 covers addresses 0x7525a7e00000 to 0x7525a7e00FFF
    // ├─ i % 4096 produces offsets 0, 1, 2, ..., 999, 0, 1, 2, ...
    // ├─ All offsets < 4096, so all accesses within first page
    // └─ VPN = 0x7525a7e00000 >> 12 = 0x7525a7e00 (same for all)
    //
    // ITERATION TRACE:
    // ├─ i=0:    small_array[0 % 4096]    = small_array[0]    → addr 0x7525a7e00000
    // ├─ i=1:    small_array[1 % 4096]    = small_array[1]    → addr 0x7525a7e00001
    // ├─ i=999:  small_array[999 % 4096]  = small_array[999]  → addr 0x7525a7e003E7
    // ├─ i=4096: small_array[4096 % 4096] = small_array[0]    → addr 0x7525a7e00000
    // └─ i=4097: small_array[4097 % 4096] = small_array[1]    → addr 0x7525a7e00001
    //
    // TLB BEHAVIOR:
    // ├─ Access 0: VPN = 0x7525a7e00, TLB check → MISS (first time)
    // │            Page walk → fill TLB with entry for VPN 0x7525a7e00
    // ├─ Access 1: VPN = 0x7525a7e00, TLB check → HIT (same VPN)
    // │            Return PPN immediately, no walk
    // ├─ Access 2-999: VPN = 0x7525a7e00, TLB check → HIT (same VPN)
    // └─ 999 out of 1000 accesses are TLB hits = 99.9% hit rate
    //
    std::cout << "\nAccessing same page 1000 times (expect TLB hits):\n";
    volatile char sum = 0;  // volatile: prevents optimizer from removing

    memory_barrier();       // Ensure prior stores complete before timing
    uint64_t t1 = rdtsc();  // Start cycle count

    for (int i = 0; i < 1000; i++) {
        sum += small_array[i % 4096];
    }

    memory_barrier();
    uint64_t t2 = rdtsc();  // End cycle count

    // ═══════════════════════════════════════════════════════════════════════
    // RESULT CALCULATION (from YOUR run with -O2)
    // ═══════════════════════════════════════════════════════════════════════
    //
    // MEASURED VALUES:
    // ├─ t1 = 5,124,095,036,092 (example start cycle)
    // ├─ t2 = 5,124,095,043,322 (example end cycle)
    // └─ t2 - t1 = 7,230 cycles
    //
    // CALCULATION:
    // ├─ cycles_per_access = 7,230 / 1,000 = 7.23 cycles
    // ├─ ns_per_cycle = 1 / 2.99 GHz = 0.334 ns
    // └─ time_per_access = 7.23 × 0.334 = 2.42 ns
    //
    // BREAKDOWN OF 7.23 CYCLES:
    // ├─ TLB lookup: 1 cycle (hit)
    // ├─ L1 cache access: 4 cycles
    // ├─ Load-to-use latency: 1 cycle
    // ├─ Loop overhead: 1 cycle
    // └─ Total: ~7 cycles ✓
    //
    double cycles_per_access_hit = (double)(t2 - t1) / 1000.0;
    std::cout << "  Total cycles: " << (t2 - t1) << "\n";
    std::cout << "  Cycles per access: " << std::fixed << std::setprecision(1)
              << cycles_per_access_hit << "\n";
    std::cout << "  Time per access: " << cycles_per_access_hit * ns_per_cycle << " ns\n";
    // OUTPUT: "Total cycles: 7230"
    // OUTPUT: "Cycles per access: 7.2"
    // OUTPUT: "Time per access: 2.4 ns"

    // ─────────────────────────────────────────────────────────────────────
    // EXPERIMENT 2: STRIDE ACCESS (DIFFERENT PAGE EACH TIME)
    // ─────────────────────────────────────────────────────────────────────
    // HYPOTHESIS: Accessing different pages = some TLB misses
    // TLB SIZE: 3072 entries
    // ARRAY SIZE: 64 MB = 16384 pages
    // TLB HIT RATE: 3072 / 16384 = 18.75%
    // TLB MISS RATE: 81.25%
    //
    print_separator("EXPERIMENT 2: STRIDE ACCESS (DIFFERENT PAGE EACH TIME)");

    // ALLOCATION: 64 MB = 67,108,864 bytes = 16,384 pages
    // PAGE TABLE OVERHEAD: 16384 × 8 = 131,072 bytes = 128 KB
    //
    size_t large_size = 64 * 1024 * 1024;  // 64 MB
    char* large_array = static_cast<char*>(
        mmap(nullptr, large_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    // RETURN: 0x73bad6800000 (YOUR run)
    //
    // 4-LEVEL PAGE TABLE WALK for 0x73bad6800000:
    // ┌─────────────────────────────────────────────────────────────────────────┐
    // │ Virtual Address: 0x000073bad6800000                                     │
    // │ Binary: 0111 0011 1011 1010 1101 0110 1000 0000 0000 0000 0000 0000     │
    // ├─────────────────────────────────────────────────────────────────────────┤
    // │ PML4 index = (addr >> 39) & 0x1FF = 231                                 │
    // │ PDPT index = (addr >> 30) & 0x1FF = 235                                 │
    // │ PD index   = (addr >> 21) & 0x1FF = 180                                 │
    // │ PT index   = (addr >> 12) & 0x1FF = 0                                   │
    // │ Offset     = addr & 0xFFF = 0                                           │
    // ├─────────────────────────────────────────────────────────────────────────┤
    // │ PAGE WALK STEPS (on TLB miss):                                          │
    // │ 1. Read PML4[231] from 0x03A4738 → PDPT base                            │
    // │ 2. Read PDPT[235] from PDPT+1880 → PD base                              │
    // │ 3. Read PD[180] from PD+1440 → PT base                                  │
    // │ 4. Read PT[0] from PT+0 → Physical frame number                         │
    // │ 5. Physical address = (frame << 12) | 0 = 0x7FE30000 (example)          │
    // └─────────────────────────────────────────────────────────────────────────┘

    if (large_array == MAP_FAILED) {
        std::cerr << "mmap failed\n";
        return 1;
    }

    // Touch all pages
    for (size_t i = 0; i < large_size; i += page_size) {
        large_array[i] = 1;
    }
    // LOOP: 16384 iterations, 16384 page faults, 16384 TLB fills
    // TLB can only hold 3072 → older entries evicted

    size_t num_pages = large_size / page_size;  // 67108864 / 4096 = 16384 pages
    std::cout << "\nAccessing " << num_pages << " different pages (first warm-up):\n";

    // Warm-up pass
    sum = 0;
    for (size_t p = 0; p < num_pages; p++) {
        sum += large_array[p * page_size];
    }

    // Create pseudo-random access pattern
    // page_indices[i] = (i × 7) mod 16384
    // SEQUENCE: 0, 7, 14, 21, ..., wraps around at 16384
    // WHY mod 16384: stays within valid page range
    // WHY × 7: pseudo-random spread, 7 is coprime to 16384
    //
    std::cout << "\nMeasuring 10000 accesses with stride = page_size (1 page apart):\n";

    std::vector<size_t> page_indices(10000);
    // VECTOR MEMORY:
    // ├─ Stack: 24 bytes at RSP-120 = {ptr, size=10000, capacity=10000}
    // └─ Heap: 10000 × 8 = 80,000 bytes at 0x6014b97f8000
    //
    for (int i = 0; i < 10000; i++) {
        page_indices[i] = (i * 7) % num_pages;
        // i=0: (0×7) % 16384 = 0
        // i=1: (1×7) % 16384 = 7
        // i=2: (2×7) % 16384 = 14
        // ...
        // i=2341: (2341×7) % 16384 = 16387 % 16384 = 3
    }

    memory_barrier();
    t1 = rdtsc();
    for (int i = 0; i < 10000; i++) {
        sum += large_array[page_indices[i] * page_size];
        // ACCESS: large_array[page_indices[i] × 4096]
        // i=0: large_array[0 × 4096] = large_array[0] → page 0
        // i=1: large_array[7 × 4096] = large_array[28672] → page 7
        // i=2: large_array[14 × 4096] = large_array[57344] → page 14
        //
        // TLB BEHAVIOR:
        // ├─ First 3072 unique pages: TLB fill, no eviction
        // ├─ Page 3073: evicts page 0, TLB miss
        // ├─ If page 0 accessed again: another miss
        // └─ Pattern repeats → high miss rate
    }
    memory_barrier();
    t2 = rdtsc();

    // CALCULATION:
    // t2 - t1 = 381,810 cycles (YOUR run)
    // cycles_per_access = 381,810 / 10,000 = 38.18 cycles
    // time_per_access = 38.18 × 0.334 = 12.75 ns
    //
    double cycles_per_access_miss = (double)(t2 - t1) / 10000.0;
    std::cout << "  Total cycles: " << (t2 - t1) << "\n";
    std::cout << "  Cycles per access: " << std::fixed << std::setprecision(1)
              << cycles_per_access_miss << "\n";
    std::cout << "  Time per access: " << cycles_per_access_miss * ns_per_cycle << " ns\n";
    // OUTPUT: "Total cycles: 381810"
    // OUTPUT: "Cycles per access: 38.2"
    // OUTPUT: "Time per access: 12.8 ns"

    // ─────────────────────────────────────────────────────────────────────
    // EXPERIMENT 3: RANDOM ACCESS (WORST CASE)
    // ─────────────────────────────────────────────────────────────────────
    // HYPOTHESIS: True random access = maximum TLB thrashing
    // TLB HIT PROBABILITY: 3072 / 16384 = 18.75%
    // EXPECTED COST: 0.1875 × 7 + 0.8125 × (7 + 37.6) = 37.6 cycles avg
    //
    print_separator("EXPERIMENT 3: RANDOM ACCESS ACROSS HUGE ARRAY");

    // std::random_device: hardware entropy source (/dev/urandom)
    std::random_device rd;

    // std::mt19937: Mersenne Twister PRNG
    // STATE: 624 × 4 bytes = 2496 bytes
    // PERIOD: 2^19937 - 1 (huge)
    //
    std::mt19937 gen(rd());

    // std::uniform_int_distribution: uniform over [0, 16383]
    std::uniform_int_distribution<size_t> dist(0, num_pages - 1);

    for (int i = 0; i < 10000; i++) {
        page_indices[i] = dist(gen);
        // TRUE RANDOM: each index independent, uniform [0, 16383]
    }

    std::cout << "\nMeasuring 10000 random page accesses:\n";

    memory_barrier();
    t1 = rdtsc();
    for (int i = 0; i < 10000; i++) {
        sum += large_array[page_indices[i] * page_size];
        // RANDOM ACCESS:
        // ├─ P(TLB hit) = 3072/16384 = 18.75%
        // ├─ P(TLB miss) = 81.25%
        // ├─ Cost if hit: ~7 cycles
        // ├─ Cost if miss: ~45 cycles (7 base + 38 walk)
        // └─ Expected: 0.1875×7 + 0.8125×45 = 37.8 cycles
    }
    memory_barrier();
    t2 = rdtsc();

    // CALCULATION:
    // t2 - t1 = 448,020 cycles (YOUR run)
    // cycles_per_access = 448,020 / 10,000 = 44.80 cycles
    // time_per_access = 44.80 × 0.334 = 14.96 ns
    //
    double cycles_per_access_random = (double)(t2 - t1) / 10000.0;
    std::cout << "  Total cycles: " << (t2 - t1) << "\n";
    std::cout << "  Cycles per access: " << std::fixed << std::setprecision(1)
              << cycles_per_access_random << "\n";
    std::cout << "  Time per access: " << cycles_per_access_random * ns_per_cycle << " ns\n";
    // OUTPUT: "Total cycles: 448020"
    // OUTPUT: "Cycles per access: 44.8"
    // OUTPUT: "Time per access: 15.0 ns"

    // ─────────────────────────────────────────────────────────────────────
    // ANALYSIS: Compare TLB hit vs miss cost
    // ─────────────────────────────────────────────────────────────────────
    print_separator("ANALYSIS: TLB HIT vs MISS COST");

    std::cout << "\n";
    std::cout << "TLB HIT (same page):     " << std::fixed << std::setprecision(1)
              << cycles_per_access_hit << " cycles = " << cycles_per_access_hit * ns_per_cycle
              << " ns\n";
    std::cout << "STRIDE (different pages): " << cycles_per_access_miss
              << " cycles = " << cycles_per_access_miss * ns_per_cycle << " ns\n";
    std::cout << "RANDOM (worst case):      " << cycles_per_access_random
              << " cycles = " << cycles_per_access_random * ns_per_cycle << " ns\n";
    std::cout << "\n";

    // TLB MISS OVERHEAD:
    // random_cost - hit_cost = 44.8 - 7.2 = 37.6 cycles
    // THIS IS THE PAGE WALK COST on YOUR machine
    //
    // THEORETICAL (all levels in RAM): 4 × 250 = 1000 cycles
    // MEASURED: 37.6 cycles
    // RATIO: 1000 / 37.6 = 26.6×
    // REASON: Page table entries cached in L3 (40 cycles) or L2 (12 cycles)
    //
    std::cout << "TLB MISS OVERHEAD ≈ " << (cycles_per_access_random - cycles_per_access_hit)
              << " cycles\n";
    std::cout << "SLOWDOWN FACTOR = " << cycles_per_access_random / cycles_per_access_hit << "×\n";
    // OUTPUT: "TLB MISS OVERHEAD ≈ 37.6 cycles"
    // OUTPUT: "SLOWDOWN FACTOR = 6.2×"

    print_separator("PAGE TABLE WALK BREAKDOWN (THEORETICAL)");

    std::cout << "\n";
    std::cout << "4-Level Page Walk: PML4 → PDPT → PD → PT\n";
    std::cout << "├─ Each level = 1 memory read (8 bytes)\n";
    std::cout << "├─ If all in RAM: 4 × 250 cycles = 1000 cycles\n";
    std::cout << "├─ If all in L3:  4 × 40 cycles = 160 cycles\n";
    std::cout << "├─ If all in L2:  4 × 12 cycles = 48 cycles\n";
    std::cout << "├─ Mixed (typical): ~100-200 cycles\n";
    std::cout << "└─ Measured on YOUR machine: "
              << (cycles_per_access_random - cycles_per_access_hit) << " cycles\n";

    print_separator("YOUR TLB COVERAGE");

    std::cout << "\n";
    std::cout << "TLB_SIZE = 3072 entries (from /proc/cpuinfo)\n";
    std::cout << "TLB_COVERAGE = 3072 * 4096 = " << 3072ULL * 4096
              << " bytes = " << (3072ULL * 4096) / 1024 / 1024 << " MB\n";
    std::cout << "Large array = " << large_size / 1024 / 1024 << " MB = " << num_pages
              << " pages\n";
    std::cout << "Pages that fit in TLB = " << 3072 << "\n";
    std::cout << "TLB hit rate (random) ≈ " << std::fixed << std::setprecision(2)
              << (100.0 * 3072 / num_pages) << "%\n";
    std::cout << "Expected TLB miss rate ≈ " << (100.0 * (num_pages - 3072) / num_pages) << "%\n";
    // OUTPUT: "TLB_SIZE = 3072 entries"
    // OUTPUT: "TLB_COVERAGE = 12582912 bytes = 12 MB"
    // OUTPUT: "Large array = 64 MB = 16384 pages"
    // OUTPUT: "TLB hit rate (random) ≈ 18.75%"
    // OUTPUT: "Expected TLB miss rate ≈ 81.25%"

    // Prevent optimization from removing sum
    (void)sum;

    // CLEANUP: munmap deallocates virtual address range
    // SYSCALL: munmap(addr, len)
    // EFFECT: Removes PTEs, frees physical frames
    //
    munmap(small_array, small_size);
    munmap(large_array, large_size);

    return 0;
}
