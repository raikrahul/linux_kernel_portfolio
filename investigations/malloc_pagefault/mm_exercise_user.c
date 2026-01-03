#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * AXIOM 0: THE BYTE
 * 1 Byte = 8 bits.
 *
 * AXIOM 1: THE PAGE
 * Hardware processes memory in chunks.
 * On x86_64, default chunk size is 4096 bytes.
 * CALCULATION: 4096 = 2^12.
 * 12 is the "Shift".
 *
 * TASK 1: DEFINE PAGE_SIZE
 * Input: 1 (Page) -> Shift 12 bits left -> Output: Bytes.
 * Type: Integer.
 * Value: 4096.
 * Derivation: 1 << 12.
 */
#define PAGE_SIZE_AXIOM (1UL << 12)

/*
 * AXIOM 2: THE PAGEMAP ENTRY
 * Kernel exposes page tables via /proc/pid/pagemap.
 * Each entry represents ONE page.
 * Entry Size = 64 bits = 8 bytes.
 *
 * TASK 2: DEFINE ENTRY SIZE
 * Input: 8 bytes.
 */
#define PAGEMAP_ENTRY_BYTES 8

/*
 * AXIOM 3: THE BITMASKS
 * Entry is 64 bits.
 * Bits 0-54 = PFN.
 * Bit 63    = PAGE_PRESENT.
 *
 * TASK 3: CONSTRUCT MASKS
 * PFN MASK: Needs 55 ones (0 to 54).
 * CALCULATION: (1 << 55) - 1.
 * HEX: 0x007FFFFFFFFFFFFF.
 *
 * PRESENT MASK: Bit 63.
 * CALCULATION: 1 << 63.
 * HEX: 0x8000000000000000.
 */
// TODO: User to fill these
#define MY_PFN_MASK 0x007FFFFFFFFFFFFF     /* REPLACE WITH HEX */
#define MY_PRESENT_MASK 0x8000000000000000 /* REPLACE WITH HEX */

void print_binary(uint64_t v) {
  for (int i = 63; i >= 0; i--) {
    printf("%lu", (v >> i) & 1);
    if (i % 8 == 0)
      printf(" ");
  }
  printf("\n");
}

int main(int argc, char **argv) {
  /*
   * STEP 1: ALLOCATE MEMORY
   * Syscall: mmap.
   * Arguments numerical analysis:
   * 1. Addr: NULL (0) -> Kernel chooses.
   * 2. Len: One page -> 4096.
   * 3. Prot: Read(1) | Write(2) = 3 (0x11 binary).
   * 4. Flags: MapPrivate(2) | MapAnonymous(32) = 34 (0x22).
   * 5. Fd: -1 (No file).
   * 6. Offset: 0.
   */
  printf("1. Allocating...\n");
  void *vaddr = mmap(NULL, PAGE_SIZE_AXIOM, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (vaddr == MAP_FAILED) {
    perror("mmap failed");
    return 1;
  }
  printf("   VA: %p\n", vaddr);
  printf("   PID: %d\n", getpid());
  printf("   [PAUSED] Press ENTER to trigger Page Fault...\n");
  printf("   [PAUSED] Sleeping 10s before triggering Page Fault...\n");
  fflush(stdout);
  sleep(10);

  /*
   * STEP 2: FAULT IN
   * Memory is "lazy". It does not exist until touched.
   * Action: Write a string at OFFSET 0x100 (256).
   * Hardware: Validates page table -> Empty -> Trigger Page Fault -> Allocates
   * Phys Page -> Update Table.
   */
  char *target_addr = (char *)vaddr + 0x100;
  strcpy(target_addr, "offset world");
  printf("2. Faulted. Written 'offset world' at offset 0x100.\n");

  /*
   * STEP 3: PREVENT SWAP
   * If we sleep, kernel might swap page to disk. PFN would eventually
   * change/vanish. Lock it in RAM.
   */
  if (mlock(vaddr, PAGE_SIZE_AXIOM) != 0) {
    perror("mlock failed");
    return 1;
  }
  printf("3. Locked.\n");

  /*
   * STEP 4: ACCESS PAGEMAP
   * Interface: File `/proc/self/pagemap`.
   * Data: Array of uint64_t.
   * Index: Virtual Page Number (VPN).
   */
  int fd = open("/proc/self/pagemap", O_RDONLY);
  if (fd < 0) {
    perror("open failed");
    return 1;
  }

  /*
   * TASK 4: CALCULATE FILE OFFSET
   * Input: Virtual Address (vaddr).
   *
   * Derivation Chain:
   * 1. Virtual Page Number (VPN) = vaddr / PAGE_SIZE.
   * 2. File Offset (Bytes) = VPN * Entry_Size (8).
   *
   * DO THIS BY HAND:
   * vaddr = ........................ (from print above)
   * PAGE_SIZE = 4096 (0x1000)
   * VPN = vaddr >> 12.
   * Offset = VPN << 3 (multiply by 8).
   */
  unsigned long index = (unsigned long)vaddr / PAGE_SIZE_AXIOM;
  off_t offset = index * PAGEMAP_ENTRY_BYTES;

  /*
   * STEP 5: READ ENTRY
   */
  uint64_t entry;
  if (pread(fd, &entry, PAGEMAP_ENTRY_BYTES, offset) != PAGEMAP_ENTRY_BYTES) {
    perror("pread failed");
    return 1;
  }
  close(fd);

  printf("4. Raw Entry: 0x%016lx\n", entry);

  /*
   * TASK 5: DECODE PFN
   * Use your masks defined above.
   */
  if (MY_PRESENT_MASK == 0) {
    printf("   STOP: CONSTANTS NOT DEFINED. EDIT CODE.\n");
    return 1;
  }

  uint64_t pfn = entry & MY_PFN_MASK;
  int present = (entry & MY_PRESENT_MASK) ? 1 : 0;

  printf("   Present: %d\n", present);
  printf("   PFN: 0x%lx\n", pfn);

  /*
   * TASK 6: CALCULATE PHYSICAL ADDRESS
   * Phys = PFN * PAGE_SIZE
   * Phys = PFN << 12
   */
  printf("   Phys: 0x%lx\n", pfn << 12);

  /*
   * TASK 7: VERIFY INDICES via KERNEL (AXIOM H)
   * We calculated PGD Index = 0xae (174).
   * Let's ask the Kernel to prove it.
   */
  struct axiom_walk_args {
    int pid;
    unsigned long va;
  } args;

  args.pid = getpid();
  args.va = (unsigned long)target_addr;

  // Use a misc device if available, or piggyback on the one we have?
  // We didn't open the driver yet! The open above was /proc/self/pagemap.
  // We need to open /dev/mm_axiom_hw.

  printf("5. Verifying Indices with Kernel...\n");
  int dev_fd = open("/dev/mm_axiom_hw", O_RDWR);
  if (dev_fd < 0) {
    // Try to make the node if it doesn't exist (it's a misc device)
    // Misc devices usually appear automatically with udev, but let's be safe.
    perror("   [WARN] Cannot open /dev/mm_axiom_hw. Is module loaded?");
  } else {
#define MM_AXIOM_IOC_WALK_VA _IOW('a', 2, struct axiom_walk_args)
    if (ioctl(dev_fd, MM_AXIOM_IOC_WALK_VA, &args) < 0) {
      perror("   ioctl failed");
    } else {
      printf("   Done. Check dmesg for '[KERNEL] PGD Index'.\n");
    }
    close(dev_fd);
  }

  printf("\n[Step Finished] Record PFN 0x%lx in your worksheet.\n", pfn);
  printf("hit enter to exit...\n");
  getchar();

  return 0;
}
