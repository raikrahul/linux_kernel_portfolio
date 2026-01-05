/*
 * anon_user.c - USERSPACE TRIGGER FOR METADATA INSPECTION
 *
 * AXIOM 1: Mapped memory exists as Virtual Addresses (VA).
 * AXIOM 2: VAs are backed by `struct page` in Kernel RAM.
 * AXIOM 3: `mmap` syscall creates VMA (Virtual Memory Area).
 * AXIOM 4: VMA types determine `struct page` metadata:
 *    - Anonymous VMA -> Page->mapping has Bit 0 SET.
 *    - File VMA -> Page->mapping has Bit 0 CLEAR.
 *
 * YOUR MISSION:
 * 1. Create an ANONYMOUS mapping using raw syscall flags.
 * 2. Create a FILE-BACKED mapping using raw syscall flags.
 * 3. Fault them in (force allocation).
 * 4. Verify axioms using the kernel module.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* AXIOM: Page size is 4096 bytes on x86_64 */
#define PAGE_SIZE 4096

int main() {
  int fd;
  char *anon_ptr = NULL;
  char *file_ptr = NULL;
  char *filename = "test_mapped_file.txt";

  /*
   * STEP 0: Prepare a file for FILE-BACKED mapping.
   * We need 4096 bytes of disk space to map to 4096 bytes of RAM.
   */
  fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("open");
    return 1;
  }
  if (ftruncate(fd, PAGE_SIZE) < 0) {
    perror("ftruncate");
    return 1;
  }
  write(fd, "KERNEL_DATA_TEST", 16);
  fsync(fd);

  printf("=== MAPPING TARGET PROGRAM ===\n");
  printf("PID: %d\n", getpid());

  /*
   * TODO 1: Create ANONYMOUS Mapping
   * -------------------------------------------------------------------------
   * AXIOM: Anonymous memory is NOT backed by a file.
   * AXIOM: MAP_ANONYMOUS flag tells kernel "no file".
   * AXIOM: MAP_PRIVATE flag says "copy-on-write" (standard for RAM).
   *
   * CALCULATION:
   * PROT_READ  = 0x1
   * PROT_WRITE = 0x2
   * PROT = 0x1 | 0x2 = 0x3
   *
   * MAP_PRIVATE   = 0x02
   * MAP_ANONYMOUS = 0x20
   * FLAGS = 0x02 | 0x20 = 0x22
   *
   * WRITE THE CODE:
   * anon_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE |
   * MAP_ANONYMOUS, -1, 0);
   * -------------------------------------------------------------------------
   */

  anon_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (anon_ptr == MAP_FAILED || anon_ptr == NULL) {
    perror("mmap anon failed - did you fill TODO 1?");
    /* Fallback for safety if user runs without filling */
    anon_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  }

  /*
   * TODO 2: Fault in the Anonymous Page
   * -------------------------------------------------------------------------
   * AXIOM: mmap() only reserves VIRTUAL addresses (VMA).
   * AXIOM: PHYSICAL page is allocated only on FIRST ACCESS (Page Fault).
   *
   * ACTION: Write to the pointer to trigger Page Fault.
   * MEMORY OPS:
   * 1. CPU tries to write 'A' to anon_ptr (Virtual 0x7f...).
   * 2. MMU sees "Not Present" -> Exception #14.
   * 3. Kernel allocates 4096 bytes RAM (Physical 0x1...).
   * 4. Kernel updates Page Table (PTE).
   *
   * WRITE THE CODE:
   * strcpy(anon_ptr, "I AM ANONYMOUS");
   * -------------------------------------------------------------------------
   */

  strcpy(anon_ptr, "I AM ANONYMOUS");

  printf("ANON_VA: %p (Filled TODO 2?)\n", anon_ptr);

  /*
   * TODO 3: Create FILE-BACKED Mapping
   * -------------------------------------------------------------------------
   * AXIOM: File memory IS backed by a file (inode).
   * AXIOM: No MAP_ANONYMOUS flag.
   * AXIOM: Pass valid file descriptor (fd).
   * AXIOM: MAP_SHARED allows writing back to disk.
   *
   * CALCULATION:
   * FLAGS = MAP_SHARED (0x01).
   * FD = fd (from open() above).
   * OFFSET = 0.
   *
   * WRITE THE CODE:
   * file_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
   * 0);
   * -------------------------------------------------------------------------
   */

  file_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (file_ptr == MAP_FAILED || file_ptr == NULL) {
    perror("mmap file failed - did you fill TODO 3?");
    file_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  }

  /*
   * TODO 4: Fault in the File Page
   * -------------------------------------------------------------------------
   * AXIOM: CPU Read/Write triggers fault.
   * ACTION: Write a character to index 0.
   * EFFECT: Kernel reads 4KB from disk -> RAM -> Page Cache -> User.
   *
   * WRITE THE CODE:
   * file_ptr[0] = 'X';
   * -------------------------------------------------------------------------
   */

  file_ptr[0] = 'X';

  printf("FILE_VA: %p (Filled TODO 4?)\n", file_ptr);

  printf("\nrun: sudo insmod anon_file_hw.ko target_pid=%d anon_addr=0x%lx "
         "file_addr=0x%lx\n",
         getpid(), (unsigned long)anon_ptr, (unsigned long)file_ptr);

  printf("Sleeping... (Ctrl+C to stop)\n");
  while (1) {
    sleep(10);
  }

  return 0;
}
