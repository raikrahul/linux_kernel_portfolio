/* dirty_trace_v2.c - Trace dirty page with PFN and zone detection
 * COMPILE: gcc -o dirty_trace_v2 dirty_trace_v2.c
 * RUN: ./dirty_trace_v2
 * REQUIRES: /proc/PID/pagemap access (run as root or with CAP_SYS_ADMIN)
 */
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/* Read PFN from /proc/self/pagemap */
uint64_t get_pfn(void *vaddr) {
  int fd;
  uint64_t entry;
  uint64_t offset;

  fd = open("/proc/self/pagemap", O_RDONLY);
  if (fd < 0) {
    perror("open pagemap");
    return 0;
  }

  /* Each entry is 8 bytes, indexed by virtual page number */
  offset = ((uint64_t)vaddr / 4096) * 8;
  if (pread(fd, &entry, 8, offset) != 8) {
    perror("pread pagemap");
    close(fd);
    return 0;
  }
  close(fd);

  /* Bit 63 = present, Bits 0-54 = PFN */
  if (!(entry & (1ULL << 63))) {
    printf("Page not present in RAM\n");
    return 0;
  }

  return entry & ((1ULL << 55) - 1);
}

/* Read /proc/vmstat nr_dirty */
long get_nr_dirty(void) {
  FILE *f;
  char line[256];
  long nr_dirty = 0;

  f = fopen("/proc/vmstat", "r");
  if (!f)
    return -1;

  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "nr_dirty ", 9) == 0) {
      nr_dirty = strtol(line + 9, NULL, 10);
      break;
    }
  }
  fclose(f);
  return nr_dirty;
}

int main(void) {
  int fd;
  char *map;
  uint64_t pfn;
  long dirty_before, dirty_after;

  printf("=== DIRTY PAGE TRACE ===\n\n");

  /* READ DIRTY COUNT BEFORE */
  dirty_before = get_nr_dirty();
  printf("01. nr_dirty BEFORE = %ld\n", dirty_before);

  /* OPEN FILE */
  fd = open("/tmp/dirty_test.txt", O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  /* EXTEND TO 4096 BYTES */
  ftruncate(fd, 4096);

  /* MMAP - NO PAGE ALLOCATED YET */
  map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return 1;
  }

  printf("02. mmap returned vaddr = %p\n", map);
  printf("03. Page not yet allocated (lazy allocation)\n");

  /* FIRST ACCESS - TRIGGERS PAGE FAULT - ALLOCATES PAGE */
  map[0] = 'D';
  map[1] = 'I';
  map[2] = 'R';
  map[3] = 'T';
  map[4] = 'Y';
  map[5] = '\0';

  printf("04. Wrote 'DIRTY' to mapped memory\n");

  /* GET PFN - NOW PAGE IS ALLOCATED */
  pfn = get_pfn(map);
  printf("05. PFN = 0x%lx (decimal %lu)\n", pfn, pfn);
  printf("06. Physical address = PFN × 4096 = 0x%lx\n", pfn * 4096);

  /* DERIVE ZONE FROM PFN */
  if (pfn < 4096) {
    printf("07. Zone = DMA (PFN < 4096)\n");
  } else if (pfn < 1048576) {
    printf("07. Zone = DMA32 (PFN < 1048576)\n");
  } else {
    printf("07. Zone = Normal (PFN >= 1048576)\n");
  }

  /* READ DIRTY COUNT AFTER WRITE */
  dirty_after = get_nr_dirty();
  printf("08. nr_dirty AFTER = %ld\n", dirty_after);
  printf("09. Dirty pages increased by = %ld\n", dirty_after - dirty_before);

  /* SYNC - CLEARS DIRTY */
  printf("\n10. Calling msync(MS_SYNC) to writeback...\n");
  msync(map, 4096, MS_SYNC);

  dirty_after = get_nr_dirty();
  printf("11. nr_dirty AFTER SYNC = %ld\n", dirty_after);

  /* MAKE DIRTY AGAIN FOR FTRACE CAPTURE */
  printf("\n12. Making page dirty again for ftrace...\n");
  printf("    Run in another terminal:\n");
  printf("    sudo trace-cmd record -e 'writeback:*' -e 'filemap:*' sleep 5\n");
  printf("    Then press Enter here within 5 seconds...\n");

  /* Wait for user to start trace */
  /* getchar(); */

  map[0] = 'X'; /* Make dirty again */

  dirty_after = get_nr_dirty();
  printf("13. nr_dirty AFTER 2nd write = %ld\n", dirty_after);

  /* SHOW STRUCT PAGE ADDRESS (VMEMMAP) */
  printf("\n14. struct page address calculation:\n");
  printf("    VMEMMAP_BASE = 0xFFFFEA0000000000 (typical x86_64)\n");
  printf("    struct_page_addr = VMEMMAP_BASE + PFN × 64\n");
  printf("    struct_page_addr = 0xFFFFEA0000000000 + 0x%lx × 64\n", pfn);
  printf("    struct_page_addr = 0x%lx\n", 0xFFFFEA0000000000UL + pfn * 64);

  printf("\n15. To see flags in kernel, load flags_lab.ko and check dmesg\n");
  printf("    Or use crash/drgn with struct_page_addr above\n");

  /* CLEANUP */
  munmap(map, 4096);
  close(fd);
  unlink("/tmp/dirty_test.txt");

  printf("\n=== DONE ===\n");
  return 0;
}
