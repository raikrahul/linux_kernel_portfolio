/* malloc_user.c - Userspace program demonstrating malloc → page fault flow
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║ WHAT: Allocate memory with malloc, touch it, observe page fault          ║
 * ║ WHY: malloc returns virtual address, physical page allocated on WRITE    ║
 * ║ WHERE: Userspace, heap segment, virtual address 0x5555... or 0x7f...     ║
 * ║ WHO: glibc malloc → brk/mmap → kernel VMA → page fault → alloc_page      ║
 * ║ WHEN: First WRITE to malloc'd memory triggers page fault                 ║
 * ║ WITHOUT: Without write, page fault never happens, physical page never    ║
 * ║          allocated → "demand paging"                                     ║
 * ║ WHICH: Anonymous page (CASE 1), mapping=anon_vma, PageAnon()=1           ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * COMPILE: gcc -o malloc_user malloc_user.c
 * RUN: ./malloc_user
 * TRACE: In another terminal: sudo trace-cmd record -e
 * 'exceptions:page_fault_user'
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* ══════════════════════════════════════════════════════════════════════════
 * MEMORY LAYOUT BEFORE malloc():
 *
 * Process Virtual Address Space (example PID=12345):
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │ 0x000055555555a000 │ .text (code)                                      │
 * │ 0x000055555555b000 │ .data (initialized globals)                       │
 * │ 0x000055555555c000 │ .bss (uninitialized globals)                      │
 * │ 0x000055555555d000 │ [heap] ← brk starts here                          │
 * │         ...        │ (heap grows upward with brk)                      │
 * │ 0x00007ffff7a00000 │ libc.so.6 (shared library)                        │
 * │ 0x00007ffff7fce000 │ ld-linux.so (dynamic linker)                      │
 * │ 0x00007ffffffde000 │ [stack] (grows downward)                          │
 * │ 0x00007ffffffff000 │ [vsyscall]                                        │
 * └─────────────────────────────────────────────────────────────────────────┘
 * ══════════════════════════════════════════════════════════════════════════ */

/* ══════════════════════════════════════════════════════════════════════════
 * get_pfn() - Read PFN from /proc/self/pagemap
 *
 * /proc/self/pagemap = 8-byte entry per page
 * Entry format (bits):
 *   0-54:  PFN (if present)
 *   55-60: Soft-dirty, etc.
 *   61:    Page is file-backed (0 for anonymous)
 *   62:    Page is swapped
 *   63:    Page is present in RAM
 *
 * CALCULATION:
 * vaddr = 0x55555555d000
 * page_index = vaddr / 4096 = 0x55555555d
 * file_offset = page_index × 8 = 0x2AAAAAAAE8
 * read 8 bytes at that offset → entry
 * if (entry >> 63) & 1 → present → PFN = entry & 0x7FFFFFFFFFFFFF
 * ══════════════════════════════════════════════════════════════════════════ */
unsigned long get_pfn(void *vaddr) {
  int fd;
  uint64_t entry;
  unsigned long page_index;
  off_t offset;

  fd = open("/proc/self/pagemap", O_RDONLY); /* Needs CAP_SYS_ADMIN or root */
  if (fd < 0) {
    perror("open pagemap");
    return 0;
  }

  /* page_index = vaddr / PAGE_SIZE = vaddr >> 12 */
  page_index =
      (unsigned long)vaddr >> 12; /* 0x55555555d000 >> 12 = 0x55555555d */

  /* Each entry is 8 bytes, so offset = page_index × 8 */
  offset = page_index * sizeof(uint64_t); /* 0x55555555d × 8 = 0x2AAAAAAAE8 */

  if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
    perror("lseek pagemap");
    close(fd);
    return 0;
  }

  if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
    perror("read pagemap");
    close(fd);
    return 0;
  }

  close(fd);

  /* Bit 63 = present in RAM */
  if ((entry >> 63) & 1) {
    /* Bits 0-54 = PFN */
    return entry & 0x7FFFFFFFFFFFFF;
  } else {
    return 0; /* Page not present (not yet allocated or swapped) */
  }
}

/* ══════════════════════════════════════════════════════════════════════════
 * get_rss_pages() - Read RSS from /proc/self/statm
 *
 * /proc/self/statm format: "size resident share text lib data dt"
 * Second field = resident (pages in RAM)
 * ══════════════════════════════════════════════════════════════════════════ */
unsigned long get_rss_pages(void) {
  FILE *f;
  unsigned long size, resident;

  f = fopen("/proc/self/statm", "r");
  if (!f)
    return 0;

  if (fscanf(f, "%lu %lu", &size, &resident) != 2) {
    fclose(f);
    return 0;
  }
  fclose(f);
  return resident;
}

int main(int argc, char *argv[]) {
  void *ptr;
  unsigned long pfn_before, pfn_after;
  unsigned long rss_before, rss_after;
  size_t alloc_size = 4096; /* Exactly 1 page = 4096 bytes */

  printf("=== MALLOC PAGE FAULT DEMO ===\n\n");
  printf("PID = %d\n", getpid());
  printf("PAGE_SIZE = 4096 bytes\n");
  printf("Allocation size = %zu bytes = %zu pages\n\n", alloc_size,
         alloc_size / 4096);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 1: RECORD RSS BEFORE MALLOC
   *
   * RSS = Resident Set Size = pages actually in physical RAM
   * Before malloc, RSS = pages for code + stack + libc
   * ════════════════════════════════════════════════════════════════════ */
  rss_before = get_rss_pages();
  printf("STEP 1: RSS before malloc = %lu pages = %lu KB\n", rss_before,
         rss_before * 4);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 2: CALL malloc()
   *
   * FLOW:
   * malloc(4096) → glibc checks thread arena
   *   → If arena empty, glibc calls brk() or mmap()
   *   → brk() extends heap VMA
   *   → Kernel creates/extends VMA but DOES NOT allocate physical page
   *   → malloc() returns virtual address (e.g., 0x55555555d000)
   *
   * AT THIS POINT:
   * - Virtual address exists
   * - VMA covers this address
   * - NO physical page allocated (no PFN)
   * - PTE entry = NOT PRESENT
   * ════════════════════════════════════════════════════════════════════ */
  ptr = malloc(alloc_size);
  if (!ptr) {
    perror("malloc");
    return 1;
  }

  printf("\nSTEP 2: malloc(%zu) returned ptr = %p\n", alloc_size, ptr);
  printf("        Virtual address = 0x%lx\n", (unsigned long)ptr);
  printf("        Page-aligned base = 0x%lx000\n", (unsigned long)ptr >> 12);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 3: CHECK PFN BEFORE WRITE (no page allocated yet)
   *
   * Before touching the memory, query /proc/self/pagemap.
   * Expected: PFN = 0 (page not present) because no page fault yet.
   * ════════════════════════════════════════════════════════════════════ */
  pfn_before = get_pfn(ptr);
  printf("\nSTEP 3: PFN BEFORE write = 0x%lx\n", pfn_before);
  if (pfn_before == 0) {
    printf("        → Page NOT PRESENT in RAM (expected: demand paging)\n");
  } else {
    printf("        → Page already present (glibc pre-faulted or reused)\n");
  }

  rss_after = get_rss_pages();
  printf("        RSS after malloc (before write) = %lu pages\n", rss_after);
  printf("        RSS change = %ld pages\n",
         (long)rss_after - (long)rss_before);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 4: WRITE TO MALLOC'D MEMORY → TRIGGER PAGE FAULT
   *
   * FLOW:
   * CPU executes: movb $0x48, (%rdi)  [rdi = ptr, 0x48 = 'H']
   *   → MMU looks up PTE for ptr
   *   → PTE not present!
   *   → CPU generates PAGE FAULT exception (vector 14)
   *   → CPU pushes error_code, saves state
   *   → CPU jumps to page_fault handler (idt[14])
   *
   * KERNEL:
   * do_page_fault(regs, error_code, cr2)
   *   → cr2 = faulting address = ptr
   *   → error_code bit 1 = write (1)
   *   → find_vma(current->mm, cr2) → heap VMA
   *   → handle_mm_fault(vma, cr2, FAULT_FLAG_WRITE)
   *   → __handle_mm_fault() → __do_huge_pmd_anonymous_page or do_anonymous_page
   *   → alloc_page(GFP_HIGHUSER_MOVABLE)
   *   → set_pte() → Maps vaddr to new PFN
   *   → page->mapping = anon_vma | PAGE_MAPPING_ANON
   *
   * RETURN TO USERSPACE:
   *   → CPU retries instruction
   *   → MMU finds PTE present
   *   → Write succeeds
   * ════════════════════════════════════════════════════════════════════ */
  printf("\nSTEP 4: Writing 'Hello' to malloc'd memory...\n");
  printf("        About to execute: *ptr = 'H'\n");
  printf(
      "        This triggers PAGE FAULT → kernel allocates physical page\n\n");

  /* THE WRITE - This triggers page fault */
  ((char *)ptr)[0] = 'H'; /* Page fault happens HERE */
  ((char *)ptr)[1] = 'e';
  ((char *)ptr)[2] = 'l';
  ((char *)ptr)[3] = 'l';
  ((char *)ptr)[4] = 'o';
  ((char *)ptr)[5] = '\0';

  printf("        Write complete: \"%s\"\n", (char *)ptr);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 5: CHECK PFN AFTER WRITE (page now allocated)
   *
   * After page fault, physical page assigned.
   * Query /proc/self/pagemap again.
   * Expected: PFN != 0, page present.
   * ════════════════════════════════════════════════════════════════════ */
  pfn_after = get_pfn(ptr);
  printf("\nSTEP 5: PFN AFTER write = 0x%lx\n", pfn_after);
  if (pfn_after != 0) {
    printf("        → Page NOW PRESENT in RAM!\n");
    printf("        → Physical address = 0x%lx000\n", pfn_after);
    printf("        → Physical address = %lu bytes = %.2f GB into RAM\n",
           pfn_after * 4096, (double)(pfn_after * 4096) / (1024 * 1024 * 1024));
  } else {
    printf("        → Still not present? (need root for pagemap)\n");
  }

  rss_after = get_rss_pages();
  printf("        RSS after write = %lu pages\n", rss_after);
  printf("        RSS change from before malloc = %ld pages\n",
         (long)rss_after - (long)rss_before);

  /* ════════════════════════════════════════════════════════════════════
   * STEP 6: SUMMARY
   * ════════════════════════════════════════════════════════════════════ */
  printf("\n=== SUMMARY ===\n");
  printf("malloc() returned:     0x%lx (virtual address)\n",
         (unsigned long)ptr);
  printf("PFN before write:      0x%lx (not present = demand paging)\n",
         pfn_before);
  printf("PFN after write:       0x%lx (now allocated)\n", pfn_after);
  if (pfn_after != 0) {
    printf("Physical address:      0x%lx\n", pfn_after * 4096);
  }
  printf("\nPage type: ANONYMOUS page\n");
  printf("  → page->mapping has PAGE_MAPPING_ANON bit set\n");
  printf("  → PageAnon(page) = 1\n");
  printf("  → Uses lru/mapping/index union fields (CASE 1)\n");

  /* ════════════════════════════════════════════════════════════════════
   * TO SEE PAGE FAULT IN KERNEL:
   *
   * Terminal 1: sudo trace-cmd record -e 'exceptions:page_fault_user' &
   * Terminal 2: ./malloc_user
   * Terminal 1: sudo trace-cmd report
   *
   * Look for: page_fault_user: address=0x55555555d000 ip=0x55555555xxxx
   * error_code=0x6 error_code bit 0 = 0 (not protection violation, page not
   * present) error_code bit 1 = 1 (write access) error_code bit 2 = 0 (not
   * instruction fetch)
   * ════════════════════════════════════════════════════════════════════ */
  printf("\n=== TO TRACE PAGE FAULT ===\n");
  printf("Run: sudo trace-cmd record -e 'exceptions:page_fault_user' -p %d\n",
         getpid());

  free(ptr);
  return 0;
}
