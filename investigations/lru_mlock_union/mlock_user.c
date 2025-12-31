/*
 * mlock_user.c - USERSPACE TRIGGER FOR LRU vs MLOCK UNION
 *
 * AXIOM 01: RAM = bytes, PAGE = 4096 bytes
 * AXIOM 02: struct page bytes [8-23] = union { lru, mlock_struct, buddy_list,
 * pcp_list } AXIOM 03: mmap() creates VMA, page fault allocates page, kernel
 * adds to LRU list AXIOM 04: mlock() pins page in RAM, kernel may change
 * interpretation of bytes [8-23]
 *
 * KERNEL SOURCE:
 * /usr/src/linux-headers-6.14.0-37-generic/include/linux/mm_types.h Line 89:
 * struct list_head lru; Line 93:  void *__filler; Line 96:  unsigned int
 * mlock_count;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

int main() {
  char *ptr;
  int ret;

  printf("=== LRU vs MLOCK UNION DEMO ===\n");
  printf("PID: %d\n", getpid());

  /*
   * TODO 1: Create anonymous mapping
   * -------------------------------------------------------------------------
   * PROBLEM: need 1 page of anonymous memory
   * SYSCALL: mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,
   * -1, 0) EXPECTED: returns pointer like 0x7f... CALCULATION: PROT_READ=0x1,
   * PROT_WRITE=0x2, PROT=0x3 CALCULATION: MAP_PRIVATE=0x02, MAP_ANONYMOUS=0x20,
   * FLAGS=0x22
   *
   * WRITE THE CODE:
   * ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE |
   * MAP_ANONYMOUS, -1, 0);
   * -------------------------------------------------------------------------
   */

  // ptr = ... /* FILL THIS */
  ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ptr == MAP_FAILED) {
    perror("mmap failed");
    return 1;
  }

  /*
   * TODO 2: Fault in the page by writing to it
   * -------------------------------------------------------------------------
   * PROBLEM: mmap only creates VMA, physical page not yet allocated
   * ACTION: write any byte to ptr[0]
   * KERNEL EFFECT: page fault → alloc_page() → add to LRU list
   * EXPECTED: bytes [8-23] of struct page now contain lru.next, lru.prev
   *
   * WRITE THE CODE:
   * ptr[0] = 'A';
   * -------------------------------------------------------------------------
   */

  ptr[0] = 'A';

  printf("STEP 1: Page faulted in, should be on LRU list\n");
  printf("VA: %p\n", ptr);
  printf("Run kernel module NOW to see LRU pointers\n");
  printf("Press ENTER to continue to mlock...\n");
  getchar();

  /*
   * TODO 3: Call mlock() to pin the page
   * -------------------------------------------------------------------------
   * PROBLEM: want to prevent kernel from swapping this page
   * SYSCALL: mlock(ptr, PAGE_SIZE)
   * KERNEL EFFECT: may change bytes [8-23] interpretation
   * EXPECTED: mlock_count = 1 after first call
   *
   * WRITE THE CODE:
   * ret = mlock(ptr, PAGE_SIZE);
   * -------------------------------------------------------------------------
   */

  ret = mlock(ptr, PAGE_SIZE);

  if (ret < 0) {
    perror("mlock failed (try sudo)");
  } else {
    printf("STEP 2: mlock() succeeded\n");
    printf("Run kernel module AGAIN to see mlock_count\n");
  }

  printf("Press ENTER to exit mmap demo, continue to MALLOC demo...\n");
  getchar();

  munlock(ptr, PAGE_SIZE);
  munmap(ptr, PAGE_SIZE);

  /*
   * ==========================================================================
   * MALLOC + MLOCK DEMO: THE ALIGNMENT TRAP
   * ==========================================================================
   * AXIOM 01: malloc(N) returns pointer to N bytes, NOT page-aligned
   * AXIOM 02: mlock() operates on WHOLE PAGES, not byte ranges
   * AXIOM 03: PAGE_SIZE = 4096 = 0x1000, page boundary = address & ~0xFFF
   *
   * DANGER SCENARIO:
   * malloc(100) returns 0x555555558010 (offset 0x10 into page)
   * PAGE CONTAINING THIS: 0x555555558010 & ~0xFFF = 0x555555558000
   * PAGE RANGE: [0x555555558000, 0x555555558FFF] = 4096 bytes
   * YOUR DATA: bytes [0x010, 0x073] = 100 bytes
   * OTHER DATA: bytes [0x000, 0x00F] = 16 bytes (malloc header)
   *             bytes [0x074, 0xFFF] = other allocations
   *
   * IF YOU mlock(0x555555558010, 100):
   *   KERNEL LOCKS ENTIRE PAGE [0x555555558000, 0x555555558FFF]
   *   YOU PINNED: your 100 bytes + malloc header + other heap data
   *   RESULT: accidental over-locking, wasted RAM pinning
   * ==========================================================================
   */

  printf("\n=== MALLOC + MLOCK DEMO ===\n");

  char *heap_ptr1, *heap_ptr2, *heap_ptr3;
  unsigned long page_of_ptr1, page_of_ptr2, offset_in_page;

  /*
   * TODO 4: Allocate 3 small blocks to show heap fragmentation
   * -------------------------------------------------------------------------
   * CALCULATION: malloc(100) → glibc returns heap pointer
   * EXAMPLE: heap_ptr1 = 0x5555555592a0
   * EXAMPLE: heap_ptr2 = 0x555555559310 (100 + 16 header = 116 bytes later)
   * EXAMPLE: heap_ptr3 = 0x555555559380
   *
   * WRITE THE CODE:
   * heap_ptr1 = malloc(100);
   * heap_ptr2 = malloc(100);
   * heap_ptr3 = malloc(100);
   * -------------------------------------------------------------------------
   */

  heap_ptr1 = malloc(100); /* FILL THIS */
  heap_ptr2 = malloc(100); /* FILL THIS */
  heap_ptr3 = malloc(100); /* FILL THIS */

  printf("heap_ptr1 = %p\n", heap_ptr1);
  printf("heap_ptr2 = %p\n", heap_ptr2);
  printf("heap_ptr3 = %p\n", heap_ptr3);

  /*
   * TODO 5: Calculate which PAGE contains each pointer
   * -------------------------------------------------------------------------
   * AXIOM: page boundary = address & ~0xFFF = address & 0xFFFFFFFFFFFFF000
   * CALCULATION for 0x5555555592a0:
   *   0x5555555592a0 & ~0xFFF
   *   = 0x5555555592a0 & 0xFFFFFFFFFFFFF000
   *   = 0x555555559000
   *
   * CALCULATION for 0x555555559310:
   *   0x555555559310 & ~0xFFF = 0x555555559000 (SAME PAGE!)
   *
   * WRITE THE CODE:
   * page_of_ptr1 = (unsigned long)heap_ptr1 & ~0xFFFUL;
   * page_of_ptr2 = (unsigned long)heap_ptr2 & ~0xFFFUL;
   * -------------------------------------------------------------------------
   */

  page_of_ptr1 = (unsigned long)heap_ptr1 & ~0xFFFUL;
  page_of_ptr2 = (unsigned long)heap_ptr2 & ~0xFFFUL;

  printf("Page containing ptr1: 0x%lx\n", page_of_ptr1);
  printf("Page containing ptr2: 0x%lx\n", page_of_ptr2);

  /*
   * TODO 6: Calculate offset within page
   * -------------------------------------------------------------------------
   * AXIOM: offset = address & 0xFFF (bottom 12 bits)
   * CALCULATION for 0x5555555592a0:
   *   0x5555555592a0 & 0xFFF = 0x2a0 = 672 bytes into page
   *
   * WRITE THE CODE:
   * offset_in_page = (unsigned long)heap_ptr1 & 0xFFFUL;
   * -------------------------------------------------------------------------
   */

  offset_in_page = (unsigned long)heap_ptr1 & 0xFFFUL;

  printf("Offset of ptr1 in page: 0x%lx = %lu bytes\n", offset_in_page,
         offset_in_page);

  /*
   * THE TRAP VISUALIZATION:
   * ┌────────────────────────────────────────────────────────────────────────┐
   * │ PAGE 0x555555559000 (4096 bytes)                                       │
   * ├────────────────────────────────────────────────────────────────────────┤
   * │ [0x000-0x00F] malloc header                                            │
   * │ [0x010-0x073] ptr1 data (100 bytes) ← YOU LOCKED THIS                  │
   * │ [0x074-0x08F] malloc header                                            │
   * │ [0x090-0x0F3] ptr2 data (100 bytes) ← ALSO LOCKED (UNINTENDED)         │
   * │ [0x0F4-0x10F] malloc header                                            │
   * │ [0x110-0x173] ptr3 data (100 bytes) ← ALSO LOCKED (UNINTENDED)         │
   * │ [0x174-0xFFF] future malloc space   ← ALSO LOCKED (WASTED)             │
   * └────────────────────────────────────────────────────────────────────────┘
   *
   * mlock(heap_ptr1, 100) → locks ENTIRE page → pins 4096 bytes
   * YOU WANTED: lock 100 bytes
   * YOU GOT: locked 4096 bytes including OTHER allocations
   */

  if (page_of_ptr1 == page_of_ptr2) {
    printf("DANGER: ptr1 and ptr2 are on SAME PAGE!\n");
    printf("mlock(ptr1, 100) will also lock ptr2's data!\n");
  }

  /*
   * TODO 7: Actually mlock the malloc'd memory
   * -------------------------------------------------------------------------
   * WRITE THE CODE:
   * ret = mlock(heap_ptr1, 100);
   * -------------------------------------------------------------------------
   */

  ret = mlock(heap_ptr1, 100);

  if (ret < 0) {
    perror("mlock on malloc failed");
  } else {
    printf("mlock(heap_ptr1, 100) succeeded\n");
    printf("LOCKED: entire page 0x%lx (4096 bytes, not just 100)\n",
           page_of_ptr1);
  }

  printf("Press ENTER to exit...\n");
  getchar();

  munlock(heap_ptr1, 100);
  free(heap_ptr1);
  free(heap_ptr2);
  free(heap_ptr3);

  return 0;
}
