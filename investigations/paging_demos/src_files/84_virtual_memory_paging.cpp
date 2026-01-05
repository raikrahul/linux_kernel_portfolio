// 84_virtual_memory_paging.cpp
// TOPIC: Virtual Memory Page Table Mechanics
// NO SOLUTIONS - BOILERPLATE ONLY - FILL BY HAND

#include <sys/mman.h>  // mmap, munmap
#include <unistd.h>    // getpagesize, sysconf

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

// =============================================================================
// PART 1: PAGE SIZE DISCOVERY
// Real data from YOUR machine: getpagesize() returns actual system page size
// =============================================================================

void discover_page_size() {
    // TODO BLOCK 1: Get page size from system
    // EXPECTED on x86-64 Linux: 4096 bytes
    // getpagesize() → ? on YOUR machine
    // sysconf(_SC_PAGESIZE) → ? on YOUR machine
    // VERIFY: 4096 = 2^? (calculate the exponent)
    // YOUR MACHINE DATA: __________

    int page_size = 0;        // TODO: = getpagesize();
    long page_size_conf = 0;  // TODO: = sysconf(_SC_PAGESIZE);

    // Calculation: 4096 = 2^12
    //   log2(4096) = log2(2^12) = 12
    //   Therefore: offset_bits = 12
    //   Virtual address split: [VPN (top bits)] [Offset (12 bits)]
    page_size = getpagesize();
    page_size_conf = sysconf(_SC_PAGESIZE);
    std::cout << "Page size: " << page_size << " bytes\n";
    std::cout << "Page size (sysconf): " << page_size_conf << " bytes\n";
}

// =============================================================================
// PART 2: ADDRESS DECOMPOSITION
// Virtual_Addr → VPN + Offset
// =============================================================================

struct AddressParts {
    uint64_t virtual_addr;  // 0x00412ABC
    uint64_t vpn;           // 0x00412ABC >> 12 = 0x412A = 16682
    uint64_t offset;        // 0x00412ABC & 0xFFF = 0xABC = 2748
    uint64_t page_size;     // 4096
};

AddressParts decompose_address(uint64_t virtual_addr, uint64_t page_size) {
    // TODO BLOCK 2: Decompose virtual address into VPN and offset
    //
    // Given: virtual_addr = 0x00412ABC, page_size = 4096
    //
    // Step 1: Calculate offset_bits
    //   page_size = 4096 = 2^12 → offset_bits = 12
    //
    // Step 2: Extract VPN (shift right by offset_bits)
    //   VPN = 0x00412ABC >> 12
    //   0x00412ABC = 0b 0000 0000 0100 0001 0010 1010 1011 1100
    //   >> 12      = 0b 0000 0000 0000 0000 0100 0001 0010 1010
    //              = 0x0000412A = 16682 (decimal)
    //
    // Step 3: Extract Offset (mask with page_size - 1)
    //   Offset = 0x00412ABC & 0xFFF
    //   0xFFF = 0b 1111 1111 1111 = 4095
    //   0x00412ABC & 0xFFF = 0xABC = 2748 (decimal)
    //
    // Verify: VPN * page_size + Offset = Virtual_Addr
    //   16682 * 4096 + 2748 = 68,321,592 + 2748 = 68,324,540
    //   0x00412ABC = 68,324,540 ✓

    AddressParts parts;
    parts.virtual_addr = virtual_addr;
    parts.page_size = page_size;
    parts.vpn = 0;     // TODO: virtual_addr >> __?
    parts.offset = 0;  // TODO: virtual_addr & __?

    return parts;
}

// =============================================================================
// PART 3: SIMULATED PAGE TABLE
// Page_Table[VPN] → PPN (Physical Page Number)
// =============================================================================

class SimulatedPageTable {
   private:
    // Sparse mapping: VPN → PPN
    // Real page table uses multi-level structure
    // We use flat map for simulation
    std::vector<uint64_t> mappings;  // mappings[VPN] = PPN
    uint64_t page_size;

   public:
    SimulatedPageTable(uint64_t num_virtual_pages, uint64_t page_sz)
        : mappings(num_virtual_pages, UINT64_MAX), page_size(page_sz) {}

    // TODO BLOCK 3: Implement page table operations
    //
    // map_page(VPN, PPN):
    //   mappings[VPN] = PPN
    //   Example: map_page(0x412A, 0x7FE30)
    //            mappings[16682] = 522032
    //
    // translate(virtual_addr) → physical_addr:
    //   VPN = virtual_addr >> 12
    //   Offset = virtual_addr & 0xFFF
    //   PPN = mappings[VPN]
    //   physical_addr = (PPN << 12) | Offset
    //
    //   Example: translate(0x00412ABC)
    //     VPN = 0x412A = 16682
    //     Offset = 0xABC = 2748
    //     PPN = mappings[16682] = 0x7FE30 = 522032
    //     physical_addr = (522032 << 12) | 2748
    //                   = 0x7FE30000 | 0xABC
    //                   = 0x7FE30ABC = 2,145,484,476

    void map_page(uint64_t vpn, uint64_t ppn) {
        // TODO: mappings[vpn] = ppn;
    }

    uint64_t translate(uint64_t virtual_addr) {
        // TODO: Extract VPN, lookup PPN, reconstruct physical address
        return 0;
    }

    bool is_mapped(uint64_t vpn) {
        // TODO: return mappings[vpn] != UINT64_MAX;
        return false;
    }
};

// =============================================================================
// PART 4: TLB SIMULATION
// Cache recent VPN→PPN translations
// =============================================================================

struct TLBEntry {
    uint64_t vpn;
    uint64_t ppn;
    bool valid;
};

class SimulatedTLB {
   private:
    std::vector<TLBEntry> entries;
    size_t num_entries;
    uint64_t hits;
    uint64_t misses;

   public:
    SimulatedTLB(size_t size) : entries(size), num_entries(size), hits(0), misses(0) {
        for (auto& e : entries) e.valid = false;
    }

    // TODO BLOCK 4: Implement TLB lookup and insert
    //
    // lookup(VPN) → (hit, PPN)
    //   Scan all entries for matching VPN
    //   If found and valid: hits++, return (true, ppn)
    //   Else: misses++, return (false, 0)
    //
    // insert(VPN, PPN)
    //   Find invalid entry or LRU entry
    //   Write VPN, PPN, valid=true
    //
    // Performance model:
    //   TLB hit  = 1 cycle
    //   TLB miss = 100 cycles (page table walk)
    //
    // Example trace with 64-entry TLB:
    //   Access 0x00400000 (VPN=0x400) → miss → insert
    //   Access 0x00400100 (VPN=0x400) → hit (same page!)
    //   Access 0x00400FFF (VPN=0x400) → hit (still same page)
    //   Access 0x00401000 (VPN=0x401) → miss → insert
    //   ...
    //   After 64 unique pages, eviction starts

    std::pair<bool, uint64_t> lookup(uint64_t vpn) {
        // TODO: Linear search for vpn in entries
        return {false, 0};
    }

    void insert(uint64_t vpn, uint64_t ppn) {
        // TODO: Find slot, insert mapping
    }

    void print_stats() {
        uint64_t total = hits + misses;
        double hit_rate = total > 0 ? (double)hits / total * 100.0 : 0.0;
        std::cout << "TLB Stats: hits=" << hits << " misses=" << misses << " hit_rate=" << hit_rate
                  << "%\n";
    }
};

// =============================================================================
// PART 5: MEMORY ACCESS PATTERNS AND TLB PERFORMANCE
// =============================================================================

void analyze_access_pattern() {
    // TODO BLOCK 5: Calculate TLB behavior for different access patterns
    //
    // PATTERN A: Sequential array access
    //   int arr[1000000];  // 4,000,000 bytes
    //   Pages = 4000000 / 4096 = 976.5625 → ceil = 977 pages
    //   for (int i = 0; i < 1000000; i++) arr[i] = i;
    //   Accesses per page = 4096 / 4 = 1024 ints
    //   TLB misses = 977 (one per new page)
    //   Total accesses = 1,000,000
    //   Miss rate = 977 / 1,000,000 = 0.0977%
    //   Average cost = 0.999023 × 1 + 0.000977 × 100 = 1.0967 cycles/access
    //
    // YOUR CALCULATION:
    //   Array size = __________ bytes
    //   Pages needed = __________ / 4096 = __________
    //   Ints per page = 4096 / 4 = __________
    //   TLB misses = __________
    //   Miss rate = __________

    // PATTERN B: Stride-1024 access (every 1024th element)
    //   for (int i = 0; i < 1000; i++) arr[i * 1024] = i;
    //   Stride = 1024 ints = 4096 bytes = 1 page
    //   Each access = new page!
    //   TLB size = 64 entries
    //   Accesses = 1000, unique pages = 1000
    //   After 64 accesses, every access = miss (thrashing)
    //   Miss rate ≈ (1000 - 64) / 1000 = 93.6% (if no reuse)
    //   Actually: if sequential VPNs, 100% miss rate
    //
    // YOUR CALCULATION:
    //   Stride bytes = __________ × 4 = __________
    //   Pages per stride = __________ / 4096 = __________
    //   Unique pages touched = __________
    //   TLB capacity = 64
    //   Evictions = max(0, __________ - 64) = __________

    // PATTERN C: Random access
    //   Random index in arr[1000000]
    //   Probability of hitting same page as last access:
    //   P(same_page) = 1024 / 1000000 = 0.001024 = 0.1%
    //   TLB hit probability (64 entries, 977 pages):
    //   P(TLB_hit) ≈ 64 / 977 = 6.55%
    //   Average cost = 0.0655 × 1 + 0.9345 × 100 = 93.5 cycles/access
    //
    // YOUR CALCULATION:
    //   Pages in array = __________
    //   TLB entries = __________
    //   P(TLB_hit) = __________ / __________ = __________
    //   Average cost = __________ × 1 + __________ × 100 = __________

    std::cout << "TODO: Fill in calculations above\n";
}

// =============================================================================
// PART 6: REAL MEMORY MAPPING WITH mmap
// =============================================================================

void real_memory_mapping() {
    // TODO BLOCK 6: Use mmap to allocate memory and observe addresses
    //
    // mmap returns virtual address chosen by OS
    // Physical address is hidden (kernel only)
    // But we can observe:
    //   - Virtual address alignment (page-aligned)
    //   - Page fault behavior
    //   - /proc/self/maps content

    size_t alloc_size = 4096 * 256;  // 256 pages = 1 MB

    // Request 256 pages from OS
    // mmap(addr, length, prot, flags, fd, offset)
    //   addr = NULL → OS chooses address
    //   prot = PROT_READ | PROT_WRITE → readable/writable
    //   flags = MAP_PRIVATE | MAP_ANONYMOUS → not file-backed
    //   fd = -1, offset = 0 → no file

    void* ptr = nullptr;
    // TODO: ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE,
    //                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Examine returned address
    // Expected: page-aligned (low 12 bits = 0)
    //
    // Example: ptr = 0x7f1234560000
    //   0x7f1234560000 & 0xFFF = 0 ✓ (page aligned)
    //   VPN = 0x7f1234560000 >> 12 = 0x7f1234560

    if (ptr && ptr != MAP_FAILED) {
        uint64_t addr = (uint64_t)ptr;
        uint64_t vpn = addr >> 12;
        uint64_t offset = addr & 0xFFF;

        std::cout << "mmap returned: 0x" << std::hex << addr << std::dec << "\n";
        std::cout << "VPN: 0x" << std::hex << vpn << std::dec << "\n";
        std::cout << "Offset: " << offset << " (should be 0)\n";

        // Touch each page to trigger page faults
        // Before touch: page not in physical RAM
        // After touch: page fault → OS allocates physical page → maps it
        //
        // for (size_t i = 0; i < 256; i++) {
        //     char* page_start = (char*)ptr + i * 4096;
        //     *page_start = 42;  // Page fault on first touch
        // }

        // TODO: munmap(ptr, alloc_size);
    }
}

// =============================================================================
// PART 7: PAGE TABLE WALK SIMULATION (4-LEVEL PAGING)
// =============================================================================

// x86-64 uses 4-level page tables (9+9+9+9+12 = 48 bits)
// Virtual Address bits:
//   [63:48] = sign extension (must equal bit 47)
//   [47:39] = PML4 index (9 bits, 512 entries)
//   [38:30] = PDPT index (9 bits, 512 entries)
//   [29:21] = PD index   (9 bits, 512 entries)
//   [20:12] = PT index   (9 bits, 512 entries)
//   [11:0]  = Offset     (12 bits, 4096 bytes)

struct PageTableIndices {
    uint16_t pml4_idx;  // bits 47:39
    uint16_t pdpt_idx;  // bits 38:30
    uint16_t pd_idx;    // bits 29:21
    uint16_t pt_idx;    // bits 20:12
    uint16_t offset;    // bits 11:0
};

PageTableIndices decode_virtual_address(uint64_t vaddr) {
    // TODO BLOCK 7: Extract each index from virtual address
    //
    // Example: vaddr = 0x00007F1234567890
    //
    // Binary breakdown (48 significant bits):
    //   0x7F1234567890 = 0111 1111 0001 0010 0011 0100 0101 0110 0111 1000 1001 0000
    //
    //   Bits 47:39 (PML4): 0111 1111 0 = 0xFE = 254
    //   Bits 38:30 (PDPT): 00 1001 000 = 0x048 = 72
    //   Bits 29:21 (PD):   01 0011 010 = 0x09A = 154
    //   Bits 20:12 (PT):   0 0101 0110 = 0x056 = 86
    //   Bits 11:0  (OFF):  0111 1000 1001 0000 = ... wait, recalc
    //
    // Correct extraction formulas:
    //   pml4_idx = (vaddr >> 39) & 0x1FF
    //   pdpt_idx = (vaddr >> 30) & 0x1FF
    //   pd_idx   = (vaddr >> 21) & 0x1FF
    //   pt_idx   = (vaddr >> 12) & 0x1FF
    //   offset   = vaddr & 0xFFF
    //
    // 0x1FF = 0b 1 1111 1111 = 511 (9 bits)
    // 0xFFF = 0b 1111 1111 1111 = 4095 (12 bits)
    //
    // YOUR CALCULATION for vaddr = 0x00007FFF00001234:
    //   pml4_idx = (0x7FFF00001234 >> 39) & 0x1FF = ___
    //   pdpt_idx = (0x7FFF00001234 >> 30) & 0x1FF = ___
    //   pd_idx   = (0x7FFF00001234 >> 21) & 0x1FF = ___
    //   pt_idx   = (0x7FFF00001234 >> 12) & 0x1FF = ___
    //   offset   = 0x7FFF00001234 & 0xFFF = ___

    PageTableIndices idx;
    idx.pml4_idx = 0;  // TODO
    idx.pdpt_idx = 0;  // TODO
    idx.pd_idx = 0;    // TODO
    idx.pt_idx = 0;    // TODO
    idx.offset = 0;    // TODO
    return idx;
}

// =============================================================================
// PART 8: COST ANALYSIS
// =============================================================================

void cost_analysis() {
    // TODO BLOCK 8: Calculate costs for different scenarios
    //
    // GIVEN CONSTANTS (typical x86-64):
    //   TLB_HIT_CYCLES = 1
    //   TLB_MISS_CYCLES = 100  (page table walk)
    //   MEMORY_LATENCY = 100 ns
    //   CPU_FREQUENCY = 3 GHz = 3,000,000,000 Hz
    //   CYCLES_PER_NS = 3
    //   TLB_SIZE = 64 entries
    //   PAGE_SIZE = 4096 bytes
    //
    // SCENARIO 1: Linked list traversal
    //   List length = 10,000 nodes
    //   Node size = 16 bytes (next ptr + data)
    //   Nodes per page = 4096 / 16 = 256 nodes
    //
    //   BEST CASE: All nodes contiguous in memory
    //     Pages = ceil(10000 / 256) = 40 pages
    //     TLB misses = 40 (one per new page)
    //     Cost = 40 × 100 + 9960 × 1 = 4000 + 9960 = 13,960 cycles
    //
    //   WORST CASE: Each node on different page
    //     TLB_SIZE = 64, List = 10,000 nodes
    //     First 64 nodes: 64 misses (fill TLB)
    //     Remaining 9,936 nodes: all misses (evicted)
    //     Cost = 10,000 × 100 = 1,000,000 cycles
    //
    //   Ratio: 1,000,000 / 13,960 = 71.6x slower!
    //
    // YOUR CALCULATION:
    //   Nodes = __________
    //   Nodes per page = 4096 / __________ = __________
    //   Best case pages = ceil(__________ / __________) = __________
    //   Best case cost = __________ × 100 + __________ × 1 = __________
    //   Worst case cost = __________ × 100 = __________
    //   Slowdown factor = __________ / __________ = __________x

    std::cout << "TODO: Fill in cost calculations\n";
}

// =============================================================================
// PART 9: HUGE PAGE ALLOCATION AND VERIFICATION
// =============================================================================

void huge_page_test() {
    // HUGE PAGE SIZES ON x86-64:
    // Normal page:  4 KB  = 4096 bytes      = 2^12  → 12 offset bits
    // Huge page:    2 MB  = 2097152 bytes   = 2^21  → 21 offset bits
    // Gigantic:     1 GB  = 1073741824 bytes = 2^30  → 30 offset bits

    // TLB COVERAGE COMPARISON (64 TLB entries):
    // Normal:  64 × 4096       = 262144 bytes     = 256 KB
    // Huge:    64 × 2097152    = 134217728 bytes  = 128 MB  (512× more)

    constexpr size_t HUGE_PAGE_SIZE = 2097152;         // 2 MB
    constexpr size_t ALLOC_SIZE = HUGE_PAGE_SIZE * 4;  // 8 MB = 4 huge pages

    std::cout << "Huge page size: " << HUGE_PAGE_SIZE << " bytes = 2 MB\n";
    std::cout << "Allocating: " << ALLOC_SIZE << " bytes = " << ALLOC_SIZE / HUGE_PAGE_SIZE
              << " huge pages\n";

    // Allocate memory aligned to 2MB boundary
    void* ptr = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        std::cout << "mmap failed!\n";
        return;
    }

    uint64_t addr = (uint64_t)ptr;
    std::cout << "mmap returned: 0x" << std::hex << addr << std::dec << "\n";

    // Check if 2MB aligned (low 21 bits = 0)
    bool is_2mb_aligned = (addr & 0x1FFFFF) == 0;
    std::cout << "2MB aligned: " << (is_2mb_aligned ? "YES" : "NO") << "\n";
    std::cout << "Low 21 bits: 0x" << std::hex << (addr & 0x1FFFFF) << std::dec << "\n";

    // REQUEST TRANSPARENT HUGE PAGES (THP)
    // Your machine is in "madvise" mode, so we must explicitly request
    int result = madvise(ptr, ALLOC_SIZE, MADV_HUGEPAGE);
    std::cout << "madvise(MADV_HUGEPAGE) returned: " << result << " (0=success)\n";

    // Touch all pages to trigger allocation
    // Kernel will TRY to use huge pages if:
    //   1. Address is 2MB aligned
    //   2. 512 contiguous physical pages (2MB) are available
    //   3. THP is enabled
    char* p = (char*)ptr;
    for (size_t i = 0; i < ALLOC_SIZE; i += 4096) {
        p[i] = 42;  // Touch each 4KB page
    }
    std::cout << "Touched " << ALLOC_SIZE / 4096 << " pages\n";

    // Read /proc/self/smaps to check if huge pages were used
    // Look for "AnonHugePages:" field
    std::cout << "\nTo verify huge pages, run:\n";
    std::cout << "  grep -A 20 '" << std::hex << addr << "' /proc/" << getpid()
              << "/smaps | grep -E '(AnonHugePages|Size)'\n"
              << std::dec;

    // TLB BENEFIT CALCULATION:
    // Without huge pages: 8MB / 4KB = 2048 pages = 2048 TLB misses
    // With huge pages:    8MB / 2MB = 4 pages    = 4 TLB misses
    // Speedup: 2048 / 4 = 512× fewer TLB misses
    std::cout << "\nTLB BENEFIT:\n";
    std::cout << "  Normal 4KB pages: " << ALLOC_SIZE / 4096 << " pages → " << ALLOC_SIZE / 4096
              << " TLB misses\n";
    std::cout << "  Huge 2MB pages:   " << ALLOC_SIZE / HUGE_PAGE_SIZE << " pages → "
              << ALLOC_SIZE / HUGE_PAGE_SIZE << " TLB misses\n";
    std::cout << "  Speedup: " << (ALLOC_SIZE / 4096) / (ALLOC_SIZE / HUGE_PAGE_SIZE)
              << "× fewer TLB misses\n";

    // Cleanup
    munmap(ptr, ALLOC_SIZE);
}

// =============================================================================
// MAIN: Run all parts
// =============================================================================

int main() {
    std::cout << "=== PART 1: Page Size Discovery ===\n";
    discover_page_size();

    std::cout << "\n=== PART 2: Address Decomposition ===\n";
    AddressParts parts = decompose_address(0x00412ABC, 4096);
    std::cout << "VA: 0x" << std::hex << parts.virtual_addr << " → VPN: 0x" << parts.vpn
              << " + Offset: 0x" << parts.offset << std::dec << "\n";

    std::cout << "\n=== PART 5: Access Pattern Analysis ===\n";
    analyze_access_pattern();

    std::cout << "\n=== PART 6: Real Memory Mapping ===\n";
    real_memory_mapping();

    std::cout << "\n=== PART 7: 4-Level Page Table Decode ===\n";
    PageTableIndices idx = decode_virtual_address(0x00007FFF00001234);
    std::cout << "VA: 0x00007FFF00001234\n";
    std::cout << "PML4: " << idx.pml4_idx << " PDPT: " << idx.pdpt_idx << " PD: " << idx.pd_idx
              << " PT: " << idx.pt_idx << " Offset: " << idx.offset << "\n";

    std::cout << "\n=== PART 8: Cost Analysis ===\n";
    cost_analysis();

    std::cout << "\n=== PART 9: Huge Page Test ===\n";
    huge_page_test();

    return 0;
}
