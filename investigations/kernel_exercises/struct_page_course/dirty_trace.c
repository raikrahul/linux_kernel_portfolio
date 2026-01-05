/* dirty_trace.c - Userspace program to make a page dirty
 * COMPILE: gcc -o dirty_trace dirty_trace.c
 * RUN: ./dirty_trace
 * TRACE: sudo cat /proc/vmstat | grep -i dirty
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
  int fd;
  char *map;
  struct stat sb;

  /* LINE 18: Open file for read/write */
  fd = open("/tmp/dirty_test.txt", O_RDWR | O_CREAT, 0644);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  /* LINE 22: Extend file to 4096 bytes (1 page) */
  ftruncate(fd, 4096);
  fstat(fd, &sb);

  /* LINE 26: Memory-map the file */
  /* This creates a VMA in process address space */
  /* No page allocated yet - just virtual mapping */
  map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return 1;
  }

  printf("Mapped at virtual address: %p\n", map);
  printf("Before write: page is CLEAN (PG_dirty=0)\n");

  /* LINE 35: WRITE TO MAPPED MEMORY - THIS MAKES PAGE DIRTY */
  /* CPU writes to virtual address */
  /* MMU translates to physical page */
  /* CPU sets dirty bit in PTE (hardware) */
  /* Kernel later calls SetPageDirty(page) */
  map[0] = 'H';
  map[1] = 'E';
  map[2] = 'L';
  map[3] = 'L';
  map[4] = 'O';
  map[5] = '\0';

  printf("After write: page is DIRTY (PG_dirty=1)\n");
  printf("Data differs from disk - must sync before free\n");

  /* LINE 49: SYNC - Forces writeback, page becomes CLEAN again */
  /* msync calls: lock_page → clear PG_dirty → set PG_writeback → write to disk
   * → clear PG_writeback → unlock */
  msync(map, 4096, MS_SYNC);

  printf("After msync: page is CLEAN again (PG_dirty=0)\n");

  /* Cleanup */
  munmap(map, 4096);
  close(fd);
  unlink("/tmp/dirty_test.txt");

  return 0;
}
