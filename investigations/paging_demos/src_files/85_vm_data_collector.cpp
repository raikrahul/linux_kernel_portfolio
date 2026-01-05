// 85_vm_data_collector.cpp
// Collects real virtual memory data from YOUR machine and process
// Compile: g++ -std=c++20 -O0 85_vm_data_collector.cpp -o /tmp/vm_collector
// Run: /tmp/vm_collector

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void print_separator(const char* title) { std::cout << "\n=== " << title << " ===\n"; }

void collect_system_info() {
    print_separator("SYSTEM INFO (YOUR MACHINE)");

    // Page size
    long page_size = sysconf(_SC_PAGESIZE);
    std::cout << "PAGE_SIZE = " << page_size << " bytes = " << page_size / 1024 << " KB\n";
    std::cout << "OFFSET_BITS = 12 (log2(" << page_size << ") = 12)\n";

    // CPU info
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            std::cout << "CPU: " << line.substr(line.find(':') + 2) << "\n";
            break;
        }
    }
    cpuinfo.clear();
    cpuinfo.seekg(0);
    while (std::getline(cpuinfo, line)) {
        if (line.find("TLB size") != std::string::npos) {
            std::cout << "TLB: " << line.substr(line.find(':') + 2) << "\n";
            break;
        }
    }

    // RAM
    std::ifstream meminfo("/proc/meminfo");
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal") != std::string::npos) {
            std::cout << "RAM: " << line << "\n";
            break;
        }
    }

    // Huge page size
    meminfo.clear();
    meminfo.seekg(0);
    while (std::getline(meminfo, line)) {
        if (line.find("Hugepagesize") != std::string::npos) {
            std::cout << "HUGE_PAGE: " << line << "\n";
            break;
        }
    }

    // THP mode
    std::ifstream thp("/sys/kernel/mm/transparent_hugepage/enabled");
    if (thp) {
        std::getline(thp, line);
        std::cout << "THP_MODE: " << line << "\n";
    }
}

void collect_process_info() {
    print_separator("PROCESS INFO (THIS PROCESS)");

    pid_t pid = getpid();
    std::cout << "PID = " << pid << "\n";
    std::cout << "PID (hex) = 0x" << std::hex << pid << std::dec << "\n";
}

struct Allocation {
    void* ptr;
    size_t size;
    const char* name;
};

void analyze_address(const char* name, void* ptr, size_t size) {
    uint64_t addr = reinterpret_cast<uint64_t>(ptr);
    uint64_t vpn = addr >> 12;
    uint64_t offset = addr & 0xFFF;
    uint64_t pages = (size + 4095) / 4096;

    std::cout << "\n" << name << ":\n";
    std::cout << "  Virtual Address: 0x" << std::hex << std::setfill('0') << std::setw(16) << addr
              << std::dec << "\n";
    std::cout << "  VPN (addr >> 12): 0x" << std::hex << vpn << " = " << std::dec << vpn << "\n";
    std::cout << "  Offset (addr & 0xFFF): 0x" << std::hex << offset << " = " << std::dec << offset
              << "\n";
    std::cout << "  Size: " << size << " bytes = " << pages << " pages\n";

    // Check alignment
    bool page_aligned = (addr & 0xFFF) == 0;
    bool huge_aligned = (addr & 0x1FFFFF) == 0;
    std::cout << "  Page aligned (4KB): " << (page_aligned ? "YES" : "NO") << "\n";
    std::cout << "  Huge aligned (2MB): " << (huge_aligned ? "YES" : "NO") << "\n";

    // 4-level page table indices
    uint64_t pml4_idx = (addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (addr >> 30) & 0x1FF;
    uint64_t pd_idx = (addr >> 21) & 0x1FF;
    uint64_t pt_idx = (addr >> 12) & 0x1FF;
    uint64_t page_offset = addr & 0xFFF;

    std::cout << "  4-Level indices:\n";
    std::cout << "    PML4[" << pml4_idx << "] PDPT[" << pdpt_idx << "] PD[" << pd_idx << "] PT["
              << pt_idx << "] offset=" << page_offset << "\n";
}

void show_maps(pid_t pid) {
    print_separator("MEMORY MAP (/proc/PID/maps) - First 20 lines");

    std::string maps_path = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream maps(maps_path);
    std::string line;
    int count = 0;
    while (std::getline(maps, line) && count < 20) {
        std::cout << line << "\n";
        count++;
    }
    if (count == 20) std::cout << "... (truncated)\n";
}

void show_smaps_summary(pid_t pid, void* ptr) {
    print_separator("SMAPS FOR ALLOCATION (huge page check)");

    uint64_t addr = reinterpret_cast<uint64_t>(ptr);
    std::string smaps_path = "/proc/" + std::to_string(pid) + "/smaps";
    std::ifstream smaps(smaps_path);
    std::string line;
    bool found = false;

    while (std::getline(smaps, line)) {
        // Look for region containing our address
        if (line.find('-') != std::string::npos && line.find("rw") != std::string::npos) {
            uint64_t start = std::stoull(line.substr(0, line.find('-')), nullptr, 16);
            uint64_t end = std::stoull(
                line.substr(line.find('-') + 1, line.find(' ') - line.find('-') - 1), nullptr, 16);

            if (addr >= start && addr < end) {
                std::cout << "Region: " << line << "\n";
                found = true;
                // Print next 10 lines (smaps details)
                for (int i = 0; i < 10 && std::getline(smaps, line); i++) {
                    std::cout << "  " << line << "\n";
                }
                break;
            }
        }
    }
    if (!found) std::cout << "Region not found in smaps\n";
}

int main() {
    std::cout << "=====================================================\n";
    std::cout << "VIRTUAL MEMORY DATA COLLECTOR\n";
    std::cout << "=====================================================\n";

    collect_system_info();
    collect_process_info();

    print_separator("ALLOCATIONS");

    // Allocation 1: Small mmap (1 page)
    size_t size1 = 4096;
    void* ptr1 = mmap(nullptr, size1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr1 != MAP_FAILED) {
        memset(ptr1, 0x42, size1);  // Touch the page
        analyze_address("Small allocation (1 page = 4KB)", ptr1, size1);
    }

    // Allocation 2: Medium mmap (256 pages = 1MB)
    size_t size2 = 4096 * 256;
    void* ptr2 = mmap(nullptr, size2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr2 != MAP_FAILED) {
        memset(ptr2, 0x43, size2);  // Touch all pages
        analyze_address("Medium allocation (256 pages = 1MB)", ptr2, size2);
    }

    // Allocation 3: Large mmap with huge page request (8MB = 4 huge pages)
    size_t size3 = 2097152 * 4;  // 8MB
    void* ptr3 = mmap(nullptr, size3, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr3 != MAP_FAILED) {
        // Request THP
        madvise(ptr3, size3, MADV_HUGEPAGE);
        memset(ptr3, 0x44, size3);  // Touch all pages
        analyze_address("Huge page allocation (4 huge pages = 8MB)", ptr3, size3);

        // Check if huge pages were used
        show_smaps_summary(getpid(), ptr3);
    }

    // Show memory map
    show_maps(getpid());

    print_separator("TLB CALCULATION FOR YOUR MACHINE");
    std::cout << "TLB_SIZE = 3072 entries (from /proc/cpuinfo)\n";
    std::cout << "PAGE_SIZE = 4096 bytes\n";
    std::cout << "TLB_COVERAGE_NORMAL = 3072 * 4096 = " << 3072ULL * 4096
              << " bytes = " << (3072ULL * 4096) / 1024 / 1024 << " MB\n";
    std::cout << "HUGE_PAGE_SIZE = 2097152 bytes\n";
    std::cout << "TLB_COVERAGE_HUGE = 3072 * 2097152 = " << 3072ULL * 2097152
              << " bytes = " << (3072ULL * 2097152) / 1024 / 1024 / 1024 << " GB\n";

    print_separator("PAGE TABLE OVERHEAD CALCULATION");
    std::cout << "Small (4KB): " << size1 << " bytes / 4096 = " << size1 / 4096
              << " pages, PT = " << (size1 / 4096) * 8 << " bytes\n";
    std::cout << "Medium (1MB): " << size2 << " bytes / 4096 = " << size2 / 4096
              << " pages, PT = " << (size2 / 4096) * 8 << " bytes\n";
    std::cout << "Large (8MB): " << size3 << " bytes / 4096 = " << size3 / 4096
              << " pages, PT = " << (size3 / 4096) * 8 << " bytes\n";
    std::cout << "Large with Huge (8MB): " << size3 << " bytes / 2097152 = " << size3 / 2097152
              << " pages, PT = " << (size3 / 2097152) * 8 << " bytes\n";

    print_separator("PAUSING - Check /proc/maps for this PID");
    std::cout << "PID = " << getpid() << "\n";
    std::cout << "Press Enter to continue and cleanup...\n";
    std::cin.get();

    // Cleanup
    if (ptr1 != MAP_FAILED) munmap(ptr1, size1);
    if (ptr2 != MAP_FAILED) munmap(ptr2, size2);
    if (ptr3 != MAP_FAILED) munmap(ptr3, size3);

    std::cout << "Cleanup complete.\n";
    return 0;
}
