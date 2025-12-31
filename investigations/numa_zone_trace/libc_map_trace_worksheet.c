/*
 * libc_map_trace_worksheet.c
 *
 * COMPILE: gcc -o libc_map_trace_worksheet libc_map_trace_worksheet.c
 * RUN: ./libc_map_trace_worksheet
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

/* ═══════════════════════════════════════════════════════════════════════════
 * AXIOM DEFINITIONS (derived from first principles)
 *
 * AXIOM A01: RAM is array of bytes indexed by physical address
 * AXIOM A02: Page = fixed-size chunk of RAM = 4096 bytes on x86_64
 * AXIOM A03: getconf PAGESIZE = 4096 on your machine
 * AXIOM A04: File on disk = sequence of bytes, byte 0 to byte (size-1)
 * AXIOM A05: When file mmapped, disk bytes copied to RAM pages on demand
 * AXIOM A06: offset = byte position in file where VMA starts reading
 * AXIOM A07: vm_pgoff = offset / PAGE_SIZE = page number in file
 *
 * DERIVATION OF PAGE_SIZE = 4096:
 *   Page table entry (PTE) uses bits 51-12 for PFN (40 bits)
 *   Bits 11-0 = page offset = 12 bits
 *   2^12 = 4096 bytes per page
 *   ∴ PAGE_SIZE = 4096
 * ═══════════════════════════════════════════════════════════════════════════
 */
#define PAGE_SIZE 4096 /* AXIOM A02: 2^12 = 4096 bytes */

/* ═══════════════════════════════════════════════════════════════════════════
 * BOILERPLATE - DO NOT MODIFY
 * ═══════════════════════════════════════════════════════════════════════════
 */

int main(void) {
  DIR *proc_dir;
  struct dirent *entry;
  char maps_path[256];
  char line[512];
  FILE *maps_file;
  int libc_process_count = 0;

  struct stat libc_stat;

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 01: CALL stat() ON libc.so.6
   *
   * PREVIOUS STEP: none (first step)
   *
   * stat() syscall → fills struct stat with file metadata
   * stat() returns 0 on success, -1 on failure
   *
   * YOUR MACHINE DATA (run: stat /usr/lib/x86_64-linux-gnu/libc.so.6):
   *   File: /usr/lib/x86_64-linux-gnu/libc.so.6
   *   Size: 2125328 bytes
   *   Inode: 5160837
   *   Device: 259,5
   *
   * TASK: Fill in the path string argument to stat()
   * ═══════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 01: What is the full path to libc.so.6 on your machine? */
  /* HINT: You ran "stat /usr/lib/x86_64-linux-gnu/libc.so.6" */
  if (stat("/usr/lib/x86_64-linux-gnu/libc.so.6", &libc_stat) != 0) {
    perror("stat failed");
    return 1;
  }

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 02: EXTRACT INODE FROM struct stat
   *
   * PREVIOUS STEP: stat() filled libc_stat with file metadata
   *
   * struct stat layout (from /usr/include/x86_64-linux-gnu/bits/stat.h):
   *   st_dev   = device ID
   *   st_ino   = inode number
   *   st_mode  = file type and mode
   *   st_nlink = number of hard links
   *   st_size  = total size in bytes
   *
   * CALCULATION: What field of libc_stat contains the inode?
   * EXPECTED OUTPUT: "inode = 5160837"
   *
   * TASK: Access the inode field from libc_stat
   * ═══════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 02: Which field of libc_stat gives the inode? */
  /* HINT: The field name starts with "st_" and ends with "ino" */
  unsigned long inode_value = libc_stat.st_ino;
  printf("inode = %lu\n", inode_value);
  /* VERIFY: Output should be 5160837 on your machine */

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 03: CALCULATE NUMBER OF PAGES FROM FILE SIZE
   *
   * PREVIOUS STEP: libc_stat.st_size = 2125328 bytes
   *
   * PAGE_SIZE = 4096 bytes (from getconf PAGESIZE)
   * Pages in file = st_size / PAGE_SIZE
   *
   * CALCULATION BY HAND:
   *   2125328 / 4096 = ???
   *   2125328 = 4096 × ??? + remainder???
   *
   *   4096 × 500 = 2048000
   *   2125328 - 2048000 = 77328
   *   4096 × 18 = 73728
   *   77328 - 73728 = 3600
   *   ∴ 2125328 = 4096 × 518 + 3600
   *   ∴ pages = 518 (integer division)
   *   ∴ remainder = 3600 bytes in last partial page
   *
   * TASK: Calculate pages using integer division
   * ═══════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 03: What is the divisor for converting bytes to pages? */
  /* HINT: PAGE_SIZE = 4096 = 0x1000 */
  unsigned long file_size = libc_stat.st_size;
  unsigned long page_count = file_size / 4096;
  printf("size = %lu bytes, pages = %lu\n", file_size, page_count);
  /* VERIFY: Output should be "size = 2125328 bytes, pages = 518" */

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 04: OPEN /proc DIRECTORY
   *
   * PREVIOUS STEP: We know libc.so.6 has inode 5160837
   *
   * /proc = virtual filesystem containing process information
   * /proc/1 = init process
   * /proc/1234 = process with PID 1234
   * /proc/self = current process
   *
   * opendir() returns DIR* on success, NULL on failure
   *
   * TASK: Provide the path to opendir()
   * ═══════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 04: What directory contains all process subdirectories? */
  /* HINT: Starts with "/" and ends with "proc" */
  proc_dir = opendir("/proc");
  if (!proc_dir) {
    perror("opendir failed");
    return 1;
  }

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 05: CHECK IF DIRECTORY ENTRY IS A PROCESS DIRECTORY
   *
   * PREVIOUS STEP: opendir() returned proc_dir handle
   *
   * readdir() returns struct dirent*:
   *   d_type = DT_DIR (4) for directory, DT_REG (8) for regular file
   *   d_name = name string (e.g., "1234" for PID 1234)
   *
   * PIDs are numeric directories: "1", "1234", "39427"
   * Non-PIDs: "self", "meminfo", "cpuinfo" (start with letter)
   *
   * CALCULATION: ASCII '0' = 48, ASCII '9' = 57
   *   PID "1234" → d_name[0] = '1' = ASCII 49 → 49 >= 48 ∧ 49 <= 57 ✓
   *   Non-PID "self" → d_name[0] = 's' = ASCII 115 → 115 >= 48 ∧ 115 <= 57 ✗
   *
   * TASK: Fill in the character range check
   * ═══════════════════════════════════════════════════════════════════════════
   */

  while ((entry = readdir(proc_dir)) != NULL) {
    /* Skip non-directories */
    if (entry->d_type != DT_DIR)
      continue;

    /* TODO BLOCK 05: What characters are valid for first digit of PID? */
    /* HINT: '0' through '9' in ASCII */
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
      continue;

    /* ═══════════════════════════════════════════════════════════════════════════
     * STEP 06: BUILD PATH TO /proc/PID/maps
     *
     * PREVIOUS STEP: entry->d_name = "39427" (example PID)
     *
     * TARGET PATH: "/proc/39427/maps"
     *
     * snprintf(buffer, size, format, args...)
     *   buffer = maps_path (256 bytes)
     *   format = "/proc/%s/maps"
     *   args = entry->d_name
     *
     * CALCULATION for PID 39427:
     *   "/proc/" = 6 chars
     *   "39427" = 5 chars
     *   "/maps" = 5 chars
     *   '\0' = 1 char
     *   Total = 17 chars < 256 ✓
     *
     * TASK: Fill in the format string
     * ═══════════════════════════════════════════════════════════════════════════
     */

    /* TODO BLOCK 06: What format string produces "/proc/PID/maps"? */
    /* HINT: %s is replaced by entry->d_name */
    snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", entry->d_name);

    /* ═══════════════════════════════════════════════════════════════════════════
     * STEP 07: SEARCH FOR "libc.so.6" IN MAPS FILE
     *
     * PREVIOUS STEP: maps_path = "/proc/39427/maps"
     *
     * /proc/39427/maps file content (one line):
     *   "795df3800000-795df3828000 r--p 00000000 103:05 5160837
     * /usr/lib/.../libc.so.6"
     *
     * strstr(haystack, needle) returns:
     *   pointer to first occurrence of needle in haystack
     *   NULL if not found
     *
     * CALCULATION:
     *   haystack = "795df3800000-795df3828000 r--p 00000000 103:05 5160837
     * /usr/lib/.../libc.so.6" needle = "libc.so.6" result = pointer to 'l' in
     * "libc.so.6" → not NULL ✓
     *
     * TASK: Fill in the needle string
     * ═══════════════════════════════════════════════════════════════════════════
     */

    maps_file = fopen(maps_path, "r");
    if (!maps_file)
      continue;

    while (fgets(line, sizeof(line), maps_file)) {
      /* TODO BLOCK 07: What string identifies a libc mapping? */
      /* HINT: The library name with version suffix */
      if (strstr(line, ".so.6")) {
        libc_process_count++;
        break; /* Count each process once */
      }
    }
    fclose(maps_file);
  }
  closedir(proc_dir);

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 08: INTERPRET THE COUNT
   *
   * PREVIOUS STEP: libc_process_count = 105 (measured on your machine
   * 2025-12-28)
   *
   * PROOF THAT EACH PROCESS HAS VMA FOR LIBC (empirical, not assumed):
   *   Your code did: for each PID in /proc, grep ".so.6" in /proc/PID/maps
   *   strstr(line, ".so.6") matched → libc_process_count++
   *   Final count = 105 → 105 /proc/PID/maps files contained matching line
   *   Each line in /proc/PID/maps = one VMA (kernel generates this on read)
   *   ∴ 105 processes have VMA for libc.so.6 (proven by data, not assumed)
   *
   * REAL DATA FROM YOUR MACHINE (cat /proc/self/maps | grep libc):
   *   739714a00000-739714a28000 r--p 00000000 103:05 5160837 libc.so.6
   *   739714a28000-739714bb0000 r-xp 00028000 103:05 5160837 libc.so.6
   *   (5 VMAs total, all with inode 5160837)
   *
   * MEANING:
   *   105 processes have at least one VMA mapping libc.so.6
   *   address_space->i_mmap interval tree contains ≥105 entries
   *   If kernel needs to unmap page X of libc.so.6:
   *     - Query interval tree: O(log 105) = O(7) comparisons
   *     - For each matching VMA: walk page table to find PTE
   *     - Unmap 105 PTEs
   *
   * TASK: Print the count
   * ═══════════════════════════════════════════════════════════════════════════
   */

  printf("Processes mapping libc.so.6 = %d\n", libc_process_count);

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 09: READ THIS PROCESS'S MAPS
   *
   * PREVIOUS STEP: libc_process_count = 105
   *
   * /proc/self/maps = maps of current process
   * "self" is symlink to current process's PID directory
   *
   * TASK: Fill in the path to current process's maps
   * ═══════════════════════════════════════════════════════════════════════════
   */

  printf("\nThis process libc VMAs:\n");
  /* TODO BLOCK 09: What is the path to current process's maps? */
  /* HINT: /proc/something/maps where "something" = current process */
  maps_file = fopen("/proc/self/maps", "r");

  if (maps_file) {
    while (fgets(line, sizeof(line), maps_file)) {
      if (strstr(line, "libc.so.6")) {

        /* ═══════════════════════════════════════════════════════════════════════════
         * STEP 10: PARSE MAPS LINE
         *
         * PREVIOUS STEP: line = "795df3828000-795df39b0000 r-xp 00028000 103:05
         * 5160837 /.../libc.so.6"
         *
         * FORMAT: start-end perms offset dev inode pathname
         *
         * PARSING:
         *   %lx = unsigned long hex (start, end, offset)
         *   %s = string (perms, dev, pathname)
         *   %lu = unsigned long decimal (inode)
         *
         * FOR LINE "795df3828000-795df39b0000 r-xp 00028000 103:05 5160837
         * /.../libc.so.6": start = 0x795df3828000 end = 0x795df39b0000 perms =
         * "r-xp" offset = 0x28000 dev = "103:05" inode = 5160837
         *
         * CALCULATION: What is offset in decimal?
         *   0x28000 = 2×16^4 + 8×16^3 = 2×65536 + 8×4096 = 131072 + 32768 =
         * 163840
         *
         * CALCULATION: What is vm_pgoff?
         *   vm_pgoff = offset / PAGE_SIZE = 163840 / 4096 = 40
         *
         * TASK: Fill in the format string for sscanf
         * ═══════════════════════════════════════════════════════════════════════════
         */

        unsigned long start, end, offset;
        char perms[8], dev[16], path[256];
        unsigned long inode;

        /* TODO BLOCK 10: What format string parses the maps line? */
        /* HINT: start-end perms offset dev inode pathname */
        /* HINT: Use %lx for hex, %s for string, %lu for decimal */
        sscanf(line, "%lx-%lx %s %lx %s %lu %s", &start, &end, perms, &offset,
               dev, &inode, path);

        /* ═══════════════════════════════════════════════════════════════════════════
         * STEP 11: CALCULATE vm_pgoff FROM offset
         *
         * PREVIOUS STEP: offset = 0x28000 = 163840 bytes
         *
         * vm_pgoff = offset / PAGE_SIZE
         * PAGE_SIZE = 4096
         *
         * CALCULATION:
         *   vm_pgoff = 163840 / 4096 = 40
         *
         * MEANING:
         *   This VMA maps file starting at page 40 of libc.so.6
         *   vaddr 0x795df3828000 corresponds to file byte 163840
         *
         * TASK: Calculate vm_pgoff
         * ═══════════════════════════════════════════════════════════════════════════
         */

        /* TODO BLOCK 11: How do you convert byte offset to page offset? */
        /*
         * AXIOMATIC DERIVATION OF vm_pgoff:
         *
         * Step 1: offset = 0x28000 (from maps line, column 3)
         * Step 2: 0x28000 = 2*16^4 + 8*16^3 = 2*65536 + 8*4096 = 131072 + 32768
         * = 163840 bytes (uses only hex→decimal conversion, no new concepts)
         * Step 3: PAGE_SIZE = 4096 (from AXIOM A02 defined above)
         * Step 4: vm_pgoff = offset / PAGE_SIZE = 163840 / 4096
         * Step 5: 163840 / 4096 = 40 (integer division)
         *         Verify: 40 * 4096 = 163840 ✓
         * Step 6: ∴ vm_pgoff = 40
         *
         * MEANING (derived from AXIOM A06, A07):
         *   AXIOM A06: offset = byte 163840 of libc.so.6 file
         *   AXIOM A07: vm_pgoff = page 40 of libc.so.6 file
         *   ∴ VMA vm_start maps to file page 40
         *   ∴ VMA vm_start+4096 maps to file page 41
         *   ∴ VMA vm_start+N*4096 maps to file page (40+N)
         */
        unsigned long vm_pgoff = offset / PAGE_SIZE;

        /* ═══════════════════════════════════════════════════════════════════════════
         * STEP 12: CALCULATE vma_pages FROM start AND end
         *
         * PREVIOUS STEP: start = 0x795df3828000, end = 0x795df39b0000
         *
         * CALCULATION:
         *   size_bytes = end - start
         *              = 0x795df39b0000 - 0x795df3828000
         *              = 0x188000
         *
         *   0x188000 in decimal:
         *     1×16^5 + 8×16^4 + 8×16^3 = 1048576 + 524288 + 32768 = 1605632
         *
         *   vma_pages = size_bytes / 4096 = 1605632 / 4096 = 392
         *
         * MEANING:
         *   This VMA covers 392 pages
         *   Pages [40, 40+392) = pages [40, 432) of libc.so.6
         *
         * TASK: Calculate vma_pages
         * ═══════════════════════════════════════════════════════════════════════════
         */

        /* TODO BLOCK 12: What is the formula for vma_pages? */
        /*
         * AXIOMATIC DERIVATION OF vma_pages:
         *
         * Step 1: start = 0x795df3828000 (from maps line, before '-')
         * Step 2: end = 0x795df39b0000 (from maps line, after '-')
         * Step 3: size_bytes = end - start
         *         = 0x795df39b0000 - 0x795df3828000
         *         = 0x188000
         *         (subtraction, no new concepts)
         * Step 4: 0x188000 = 1*16^5 + 8*16^4 + 8*16^3
         *         = 1*1048576 + 8*65536 + 8*4096
         *         = 1048576 + 524288 + 32768
         *         = 1605632 bytes
         * Step 5: vma_pages = size_bytes / PAGE_SIZE = 1605632 / 4096
         * Step 6: 1605632 / 4096 = 392
         *         Verify: 392 * 4096 = 1605632 ✓
         * Step 7: ∴ vma_pages = 392
         *
         * MEANING (derived from previous steps):
         *   vm_pgoff = 40 (from STEP 11)
         *   vma_pages = 392 (from this step)
         *   ∴ VMA covers file pages [40, 40+392) = [40, 432)
         *   ∴ File page 45 is in this VMA: 40 <= 45 < 432 ✓
         *   ∴ File page 500 is NOT in this VMA: 500 >= 432 ✗
         */
        unsigned long size_bytes = end - start;
        unsigned long vma_pages = size_bytes / PAGE_SIZE;

        printf("  start=0x%lx end=0x%lx perms=%s vm_pgoff=%lu pages=%lu\n",
               start, end, perms, vm_pgoff, vma_pages);

        /* ═══════════════════════════════════════════════════════════════════════════
         * STEP 13: CALCULATE vaddr FOR A GIVEN page->index
         *
         * PREVIOUS STEP: vm_start=0x795df3828000, vm_pgoff=40, vma_pages=392
         *
         * SCENARIO: Kernel needs to find PTE for page 45 of libc.so.6
         *
         * FORMULA: vaddr = vm_start + (page_index - vm_pgoff) × PAGE_SIZE
         *
         * CALCULATION:
         *   vaddr = 0x795df3828000 + (45 - 40) × 4096
         *         = 0x795df3828000 + 5 × 4096
         *         = 0x795df3828000 + 20480
         *         = 0x795df3828000 + 0x5000
         *         = 0x795df382D000
         *
         * CHECK: Is page 45 in this VMA?
         *   vm_pgoff <= page_index < vm_pgoff + vma_pages
         *   40 <= 45 < 432 ✓
         *
         * TASK: Calculate example vaddr for page 45
         * ═══════════════════════════════════════════════════════════════════════════
         */

        if (vm_pgoff <= 45 && 45 < vm_pgoff + vma_pages) {
          /* TODO BLOCK 13: What is the vaddr formula? */
          unsigned long page_index = 45;
          unsigned long vaddr = start + (page_index - vm_pgoff) * PAGE_SIZE;
          printf("    → page %lu maps to vaddr 0x%lx\n", page_index, vaddr);
          /* VERIFY: Should print 0x795df382D000 or similar */
        }
      }
    }
    fclose(maps_file);
  }

  return 0;
}

/* ═══════════════════════════════════════════════════════════════════════════
 * ANSWERS (do not look until you've tried all TODOs)
 *
 * TODO 01: "/usr/lib/x86_64-linux-gnu/libc.so.6"
 * TODO 02: st_ino
 * TODO 03: 4096
 * TODO 04: "/proc"
 * TODO 05: '0', '9'
 * TODO 06: "/proc/%s/maps"
 * TODO 07: "libc.so.6"
 * TODO 09: "/proc/self/maps"
 * TODO 10: "%lx-%lx %s %lx %s %lu %s"
 * TODO 11: 4096
 * TODO 12: 4096
 * TODO 13: 4096
 * ═══════════════════════════════════════════════════════════════════════════
 */
