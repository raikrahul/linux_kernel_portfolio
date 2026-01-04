/*
 * mmap_no_fault.c
 * PURPOSE: Allocate memory via mmap() but NEVER write to it.
 * RESULT: VMA exists in Maple Tree, but NO page fault occurs.
 *
 * AXIOM: mmap() creates VMA struct with vm_start = returned address.
 * AXIOM: VMA is inserted into mm->mm_mt (Maple Tree) immediately.
 * AXIOM: No page table entry (PTE) exists until first access (page fault).
 *
 * VERIFICATION: Load vma_lookup_driver.ko with this PID and VA.
 *               Driver should find VMA in Maple Tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
  /*
   * AXIOM: mmap signature:
   * void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t
   * offset);
   *
   * addr = NULL -> kernel chooses address
   * length = 4096 -> 1 page
   * prot = PROT_READ | PROT_WRITE -> 0x1 | 0x2 = 0x3
   * flags = MAP_PRIVATE | MAP_ANONYMOUS -> 0x02 | 0x20 = 0x22
   * fd = -1 -> no file (anonymous)
   * offset = 0 -> not applicable for anonymous
   */
  void *vaddr = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (vaddr == MAP_FAILED) {
    perror("mmap failed");
    return 1;
  }

  printf("=====================================\n");
  printf("MMAP COMPLETE - NO PAGE FAULT YET\n");
  printf("=====================================\n");
  printf("PID:  %d\n", getpid());
  printf("VA:   %p\n", vaddr);
  printf("SIZE: 4096 (0x1000)\n");
  printf("=====================================\n");
  printf("EXPECTATION:\n");
  printf("  - VMA exists in Maple Tree: YES\n");
  printf("  - Page Table Entry exists:  NO\n");
  printf("  - Physical page allocated:  NO\n");
  printf("=====================================\n");
  printf("[PAUSED] Press ENTER to exit...\n");
  printf("On another terminal, run:\n");
  printf("  sudo insmod vma_lookup_driver.ko target_pid=%d target_addr=%p\n",
         getpid(), vaddr);
  printf("  sudo dmesg | tail -30\n");
  printf("=====================================\n");

  getchar();

  /*
   * STILL NO WRITE - VMA will be unmapped without ever being faulted.
   * munmap is implicit on exit.
   */
  return 0;
}
