// 85_page_walk_visualizer.cpp
// SHOWS THE 4-LEVEL PAGE WALK IN REAL TIME FROM YOUR MACHINE
// Reads /proc/self/pagemap to get actual physical frame numbers
//
// Compile: g++ -std=c++20 -O0 -g 85_page_walk_visualizer.cpp -o /tmp/pagewalk
// Run: sudo /tmp/pagewalk   (needs root for /proc/self/pagemap)

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

// ═══════════════════════════════════════════════════════════════════════════
// PAGEMAP ENTRY FORMAT (from Linux kernel documentation)
// ═══════════════════════════════════════════════════════════════════════════
// Bits 0-54:  Page frame number (if present)
// Bits 55-60: Page shift (always 12 for 4KB pages)
// Bit 61:     Reserved
// Bit 62:     Page swapped
// Bit 63:     Page present
//
struct PagemapEntry {
    uint64_t pfn : 55;        // Physical frame number
    uint64_t soft_dirty : 1;  // PTE soft-dirty bit
    uint64_t exclusive : 1;   // Page exclusively mapped
    uint64_t reserved : 4;    // Reserved
    uint64_t file_page : 1;   // File page or shared-anon
    uint64_t swapped : 1;     // Page swapped out
    uint64_t present : 1;     // Page present in RAM
};

void print_binary(uint64_t value, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        std::cout << ((value >> i) & 1);
        if (i > 0 && i % 4 == 0) std::cout << " ";
    }
}

void show_page_walk(void* ptr) {
    uint64_t vaddr = reinterpret_cast<uint64_t>(ptr);

    std::cout << "═══════════════════════════════════════════════════════════════════════\n";
    std::cout << "4-LEVEL PAGE TABLE WALK FOR ADDRESS 0x" << std::hex << vaddr << std::dec << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════\n\n";

    // ─────────────────────────────────────────────────────────────────────────
    // STEP 0: Show the virtual address in binary
    // ─────────────────────────────────────────────────────────────────────────
    std::cout << "STEP 0: VIRTUAL ADDRESS BREAKDOWN\n";
    std::cout << "─────────────────────────────────────────────────────────────────────────\n";
    std::cout << "Virtual Address: 0x" << std::hex << std::setfill('0') << std::setw(16) << vaddr
              << std::dec << "\n\n";

    std::cout << "Binary (48 bits used):\n";
    std::cout << "[Sign Ext] [PML4]     [PDPT]     [PD]       [PT]       [Offset]\n";
    std::cout << "[63:48]    [47:39]    [38:30]    [29:21]    [20:12]    [11:0]\n";

    // Extract indices
    uint64_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint64_t pd_idx = (vaddr >> 21) & 0x1FF;
    uint64_t pt_idx = (vaddr >> 12) & 0x1FF;
    uint64_t offset = vaddr & 0xFFF;

    std::cout << "\n";
    std::cout << "Bit Extraction (each index = 9 bits):\n";
    std::cout << "├─ PML4 index = (0x" << std::hex << vaddr << " >> 39) & 0x1FF = " << std::dec
              << pml4_idx << "\n";
    std::cout << "│                                                             \n";
    std::cout << "│   CALCULATION: " << std::hex << vaddr << " >> 39 = " << (vaddr >> 39)
              << std::dec << "\n";
    std::cout << "│                " << (vaddr >> 39) << " & 511 = " << pml4_idx << "\n";
    std::cout << "│                BINARY: ";
    print_binary(pml4_idx, 9);
    std::cout << "\n";
    std::cout << "│\n";
    std::cout << "├─ PDPT index = (0x" << std::hex << vaddr << " >> 30) & 0x1FF = " << std::dec
              << pdpt_idx << "\n";
    std::cout << "│   CALCULATION: " << (vaddr >> 30) << " & 511 = " << pdpt_idx << "\n";
    std::cout << "│   BINARY: ";
    print_binary(pdpt_idx, 9);
    std::cout << "\n";
    std::cout << "│\n";
    std::cout << "├─ PD index = (0x" << std::hex << vaddr << " >> 21) & 0x1FF = " << std::dec
              << pd_idx << "\n";
    std::cout << "│   CALCULATION: " << (vaddr >> 21) << " & 511 = " << pd_idx << "\n";
    std::cout << "│   BINARY: ";
    print_binary(pd_idx, 9);
    std::cout << "\n";
    std::cout << "│\n";
    std::cout << "├─ PT index = (0x" << std::hex << vaddr << " >> 12) & 0x1FF = " << std::dec
              << pt_idx << "\n";
    std::cout << "│   CALCULATION: " << (vaddr >> 12) << " & 511 = " << pt_idx << "\n";
    std::cout << "│   BINARY: ";
    print_binary(pt_idx, 9);
    std::cout << "\n";
    std::cout << "│\n";
    std::cout << "└─ Offset = 0x" << std::hex << vaddr << " & 0xFFF = " << std::dec << offset
              << "\n";
    std::cout << "    BINARY: ";
    print_binary(offset, 12);
    std::cout << "\n\n";

    // ─────────────────────────────────────────────────────────────────────────
    // STEP 1-4: Simulate page walk (we can't access real page tables from user space)
    // ─────────────────────────────────────────────────────────────────────────
    std::cout << "STEP 1-4: PAGE TABLE WALK (SIMULATED)\n";
    std::cout << "─────────────────────────────────────────────────────────────────────────\n";
    std::cout << "NOTE: User-space cannot read page table entries directly.\n";
    std::cout << "      Only kernel can access PML4/PDPT/PD/PT.\n";
    std::cout << "      We use /proc/self/pagemap to get the final result.\n\n";

    std::cout << "WHAT THE CPU HARDWARE DOES:\n";
    std::cout << "┌─────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ CR3 Register (contains physical address of PML4 table)                 │\n";
    std::cout << "│   │                                                                    │\n";
    std::cout << "│   ▼                                                                    │\n";
    std::cout << "│ ┌─────────────────────────────────────────────────────────────────┐    │\n";
    std::cout << "│ │ PML4 TABLE (512 entries × 8 bytes = 4096 bytes = 1 page)        │    │\n";
    std::cout << "│ │   Entry[" << std::setw(3) << pml4_idx
              << "] → contains physical address of PDPT table      │    │\n";
    std::cout << "│ └─────────────────────────────────────────────────────────────────┘    │\n";
    std::cout << "│           │                                                            │\n";
    std::cout << "│           ▼ (read 8 bytes from PML4[" << pml4_idx
              << "])                               │\n";
    std::cout << "│ ┌─────────────────────────────────────────────────────────────────┐    │\n";
    std::cout << "│ │ PDPT TABLE (512 entries × 8 bytes = 4096 bytes = 1 page)        │    │\n";
    std::cout << "│ │   Entry[" << std::setw(3) << pdpt_idx
              << "] → contains physical address of PD table        │    │\n";
    std::cout << "│ └─────────────────────────────────────────────────────────────────┘    │\n";
    std::cout << "│           │                                                            │\n";
    std::cout << "│           ▼ (read 8 bytes from PDPT[" << pdpt_idx
              << "])                              │\n";
    std::cout << "│ ┌─────────────────────────────────────────────────────────────────┐    │\n";
    std::cout << "│ │ PD TABLE (512 entries × 8 bytes = 4096 bytes = 1 page)          │    │\n";
    std::cout << "│ │   Entry[" << std::setw(3) << pd_idx
              << "] → contains physical address of PT table         │    │\n";
    std::cout << "│ └─────────────────────────────────────────────────────────────────┘    │\n";
    std::cout << "│           │                                                            │\n";
    std::cout << "│           ▼ (read 8 bytes from PD[" << pd_idx
              << "])                                │\n";
    std::cout << "│ ┌─────────────────────────────────────────────────────────────────┐    │\n";
    std::cout << "│ │ PT TABLE (512 entries × 8 bytes = 4096 bytes = 1 page)          │    │\n";
    std::cout << "│ │   Entry[" << std::setw(3) << pt_idx
              << "] → contains physical frame number (PFN)           │    │\n";
    std::cout << "│ └─────────────────────────────────────────────────────────────────┘    │\n";
    std::cout << "│           │                                                            │\n";
    std::cout << "│           ▼ Physical Address = (PFN << 12) | Offset                    │\n";
    std::cout << "└─────────────────────────────────────────────────────────────────────────┘\n\n";

    // ─────────────────────────────────────────────────────────────────────────
    // STEP 5: Read REAL physical frame number from /proc/self/pagemap
    // ─────────────────────────────────────────────────────────────────────────
    std::cout << "STEP 5: READ REAL PHYSICAL ADDRESS FROM /proc/self/pagemap\n";
    std::cout << "─────────────────────────────────────────────────────────────────────────\n";

    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        std::cout << "ERROR: Cannot open /proc/self/pagemap (need root)\n";
        std::cout << "Run with: sudo /tmp/pagewalk\n";
        return;
    }

    // Calculate offset in pagemap file
    // Each virtual page has an 8-byte entry
    uint64_t vpn = vaddr >> 12;  // Virtual page number
    off_t pagemap_offset = vpn * 8;

    std::cout << "Pagemap file offset = VPN × 8 = " << vpn << " × 8 = " << pagemap_offset
              << " bytes\n";

    if (lseek(fd, pagemap_offset, SEEK_SET) == (off_t)-1) {
        std::cout << "ERROR: Cannot seek in pagemap\n";
        close(fd);
        return;
    }

    uint64_t pagemap_entry = 0;
    if (read(fd, &pagemap_entry, 8) != 8) {
        std::cout << "ERROR: Cannot read pagemap entry\n";
        close(fd);
        return;
    }
    close(fd);

    // Parse pagemap entry
    bool present = (pagemap_entry >> 63) & 1;
    bool swapped = (pagemap_entry >> 62) & 1;
    uint64_t pfn = pagemap_entry & ((1ULL << 55) - 1);

    std::cout << "\nPagemap Entry (raw): 0x" << std::hex << pagemap_entry << std::dec << "\n";
    std::cout << "├─ Bit 63 (Present): " << present << " ("
              << (present ? "YES, page in RAM" : "NO, not in RAM") << ")\n";
    std::cout << "├─ Bit 62 (Swapped): " << swapped << " (" << (swapped ? "YES, in swap" : "NO")
              << ")\n";
    std::cout << "└─ Bits 0-54 (PFN):  " << pfn << " (physical frame number)\n\n";

    if (present && pfn > 0) {
        uint64_t physical_addr = (pfn << 12) | offset;
        std::cout << "PHYSICAL ADDRESS CALCULATION:\n";
        std::cout << "├─ Physical Frame Number (PFN) = " << pfn << "\n";
        std::cout << "├─ PFN in hex = 0x" << std::hex << pfn << std::dec << "\n";
        std::cout << "├─ Physical Page Address = PFN << 12 = 0x" << std::hex << (pfn << 12)
                  << std::dec << "\n";
        std::cout << "├─ Offset = " << offset << " = 0x" << std::hex << offset << std::dec << "\n";
        std::cout << "└─ PHYSICAL ADDRESS = 0x" << std::hex << physical_addr << std::dec << "\n\n";

        std::cout << "═══════════════════════════════════════════════════════════════════════\n";
        std::cout << "COMPLETE ADDRESS TRANSLATION:\n";
        std::cout << "═══════════════════════════════════════════════════════════════════════\n";
        std::cout << "Virtual:  0x" << std::hex << std::setfill('0') << std::setw(16) << vaddr
                  << "\n";
        std::cout << "Physical: 0x" << std::setfill('0') << std::setw(16) << physical_addr
                  << std::dec << "\n\n";

        std::cout << "TLB ENTRY THAT WOULD BE CACHED:\n";
        std::cout << "┌───────────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ VPN: 0x" << std::hex << std::setfill('0') << std::setw(12) << (vaddr >> 12);
        std::cout << "  →  PPN: 0x" << std::setfill('0') << std::setw(12) << pfn << " │\n";
        std::cout << "└───────────────────────────────────────────────────────────────────┘\n";
        std::cout << std::dec;
    } else {
        std::cout << "Page not present in RAM (might be swapped or not allocated)\n";
    }
}

int main() {
    std::cout << "═══════════════════════════════════════════════════════════════════════════\n";
    std::cout << "PAGE TABLE WALK VISUALIZER - SHOWS REAL DATA FROM YOUR MACHINE\n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════\n\n";

    std::cout << "Process ID: " << getpid() << "\n";
    std::cout << "Page Size: " << sysconf(_SC_PAGESIZE) << " bytes\n\n";

    // Allocate memory with mmap
    size_t size = 8 * 1024 * 1024;  // 8 MB
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED) {
        std::cerr << "mmap failed\n";
        return 1;
    }

    std::cout << "Allocated 8 MB at virtual address: 0x" << std::hex << (uint64_t)ptr << std::dec
              << "\n\n";

    // Touch the first page to ensure it's allocated
    memset(ptr, 0x42, 4096);
    std::cout << "Touched first page (wrote 0x42 to all 4096 bytes)\n\n";

    // Show page walk for this address
    show_page_walk(ptr);

    // Show another address deeper in the allocation
    void* ptr2 = static_cast<char*>(ptr) + (1024 * 4096);  // 1024 pages in
    memset(ptr2, 0x43, 4096);
    std::cout << "\n\n";
    show_page_walk(ptr2);

    munmap(ptr, size);
    return 0;
}
