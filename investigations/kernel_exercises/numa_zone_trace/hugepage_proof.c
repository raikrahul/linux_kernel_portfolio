/*
 * hugepage_proof.c
 *
 * GOAL: Allocate 2MB-aligned memory and prove huge page usage
 *
 * COMPILE: gcc -o hugepage_proof hugepage_proof.c
 * RUN: ./hugepage_proof
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * AXIOM: 2MB = 2097152 bytes = 0x200000
 * AXIOM: mmap with MAP_ANONYMOUS allocates anonymous memory
 * AXIOM: madvise(MADV_HUGEPAGE) requests kernel to use THP
 * AXIOM: /proc/self/smaps shows "AnonHugePages" for THP usage
 */

#define SIZE_2MB 2097152UL /* 2^21 = 2097152 */
#define SIZE_4MB 4194304UL /* 2^22 = 4194304 */

void show_smaps_for_addr(void *addr) {
  FILE *f = fopen("/proc/self/smaps", "r");
  if (!f)
    return;

  char line[256];
  int found = 0;
  unsigned long target = (unsigned long)addr;

  while (fgets(line, sizeof(line), f)) {
    unsigned long start, end;
    if (sscanf(line, "%lx-%lx", &start, &end) == 2) {
      found = (target >= start && target < end);
    }
    if (found && strstr(line, "AnonHugePages")) {
      printf("  %s", line);
    }
    if (found && strstr(line, "Size:")) {
      printf("  %s", line);
    }
  }
  fclose(f);
}

int main(void) {
  void *small_mem, *huge_mem;

  printf("=== STEP 1: VERIFY SYSTEM SETTINGS ===\n");
  printf("getconf PAGESIZE = ");
  fflush(stdout);
  system("getconf PAGESIZE");

  printf("Hugepagesize = ");
  fflush(stdout);
  system("grep Hugepagesize /proc/meminfo");

  printf("THP setting = ");
  fflush(stdout);
  system("cat /sys/kernel/mm/transparent_hugepage/enabled");

  printf("\n=== STEP 2: ALLOCATE 4KB MEMORY (too small for huge page) ===\n");
  small_mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (small_mem == MAP_FAILED) {
    perror("mmap small");
    return 1;
  }
  memset(small_mem, 'A', 4096); /* touch memory to allocate pages */
  printf("small_mem = %p (4096 bytes)\n", small_mem);
  show_smaps_for_addr(small_mem);

  printf("\n=== STEP 3: ALLOCATE 4MB MEMORY WITH MADV_HUGEPAGE ===\n");
  huge_mem = mmap(NULL, SIZE_4MB, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (huge_mem == MAP_FAILED) {
    perror("mmap huge");
    return 1;
  }

  /* Request THP for this memory region */
  if (madvise(huge_mem, SIZE_4MB, MADV_HUGEPAGE) != 0) {
    perror("madvise MADV_HUGEPAGE");
  }

  printf("huge_mem = %p (4194304 bytes = 4MB)\n", huge_mem);
  printf(
      "huge_mem & 0x1FFFFF = 0x%lx (0 = 2MB aligned, nonzero = not aligned)\n",
      (unsigned long)huge_mem & 0x1FFFFF);

  /* Touch all memory to force allocation */
  printf("Touching all 4MB to force page allocation...\n");
  memset(huge_mem, 'B', SIZE_4MB);

  printf("After touching, smaps shows:\n");
  show_smaps_for_addr(huge_mem);

  printf("\n=== STEP 4: CHECK SYSTEM-WIDE HUGE PAGE STATS ===\n");
  printf("AnonHugePages = ");
  fflush(stdout);
  system("grep AnonHugePages /proc/meminfo");

  printf("\n=== STEP 5: READ /proc/self/maps FOR HUGE REGION ===\n");
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "grep '%lx' /proc/self/maps",
           (unsigned long)huge_mem & 0xFFFFFFFFFFF00000UL);
  system(cmd);

  printf("\n=== PROOF ===\n");
  printf("If AnonHugePages > 0 in smaps for huge_mem region → THP used\n");
  printf("4KB region: AnonHugePages = 0 kB (too small)\n");
  printf("4MB region: AnonHugePages = 2048 or 4096 kB (1 or 2 huge pages)\n");

  /* Cleanup */
  munmap(small_mem, 4096);
  munmap(huge_mem, SIZE_4MB);

  return 0;
}
