/*
 * mapping_user.c
 * TRIGGER PROGRAM: Generates pages in "FILE" and "ANON" states.
 *
 * OBJECTIVE:
 * 1. Create a file-backed mapping.
 *    - Kernel sees a struct address_space pointer.
 *    - Optimization: Lower 2 bits of pointer must be 00.
 *
 * 2. Create an anonymous mapping.
 *    - Kernel sees a struct anon_vma pointer.
 *    - Optimization: Lower 2 bits must encode PAGE_MAPPING_ANON (01).
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096

int main() {
  int fd;
  char *file_ptr;
  char *anon_ptr;

  printf("=== MAPPING FIELD DECODER DEMO ===\n");
  printf("PID: %d\n", getpid());

  // --- STEP 1: CREATE FILE-BACKED PAGE ---
  // AXIOM 1: Page Cache requires a backing file (inode -> address_space).
  // WORK: Open a file called "temp_map_file" for R/W.
  fd = open("temp_map_file", O_RDWR | O_CREAT | O_TRUNC, 0600);
  if (fd < 0) {
    perror("open");
    return 1;
  }

  // AXIOM 2: A page only exists if there is data or backing store is grown.
  // WORK: Write 4096 bytes of dummy data ('F') to create 64-bit file size.
  char buf[PAGE_SIZE];
  memset(buf, 'F', PAGE_SIZE);
  write(fd, buf, PAGE_SIZE);

  /*
   * TODO 1: MMAP THE FILE
   * ---------------------
   * AXIOM: PROT_READ(0x1) | PROT_WRITE(0x2) = 0x3 (R/W Access)
   * AXIOM: MAP_SHARED(0x01) -> updates go to file (Page Cache behavior)
   * CALC: mmap(NULL, 4096, 0x3, 0x01, fd, 0)
   */
  // file_ptr = ...
  file_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                  0); /* FILL THIS */

  if (file_ptr == MAP_FAILED) {
    perror("mmap file");
    return 1;
  }

  /*
   * TODO 2: TRIGGER PAGE FAULT (FILE)
   * ---------------------------------
   * AXIOM: mmap updates VMA (Metadata), but PTE is empty (Demand Paging).
   * AXIOM: CPU Access -> #PF -> Kernel allocs page -> Reads file -> Sets PTE.
   * WORK: Write 'M' to the first byte of file_ptr.
   *
   * CALCULATION TRACE:
   * 1. file_ptr = 0x7f...1000 (Generic Virtual Address)
   * 2. *file_ptr IS 'M' (0x4D in ASCII)
   * 3. CPU executes MOV BYTE PTR [RAX], 0x4D
   * 4. MMU Lookup: VPN = 0x7f...1 -> TLB Miss -> Page Walk
   * 5. PTE P-Bit is 0 -> CPU raises Exception 14 (#PF)
   * 6. Kernel (do_page_fault) -> finds VMA -> sees backed by file
   * 7. Kernel Allocates Page (PFN: 0x12345)
   * 8. Kernel reads disk sector -> RAM PFN 0x12345
   * 9. Kernel updates PTE: 0x7f...1 -> 0x12345 | Present
   * 10. Instruction Restarts -> Write 'M' -> RAM[0x12345000] = 'M'
   */
  file_ptr[0] = 'M';

  printf("\n[1] FILE-BACKED MAPPING:\n");
  printf("    VA: %p\n", file_ptr);
  printf("    Action: mmap('temp_map_file', SHARED) + write\n");

  // --- STEP 2: CREATE ANONYMOUS PAGE ---

  /*
   * TODO 3: MMAP ANONYMOUS MEMORY
   * -----------------------------
   * AXIOM: MAP_ANONYMOUS(0x20) -> No file backing.
   * AXIOM: MAP_PRIVATE(0x02) -> COW (Copy On Write) semantics usually.
   * CALC: Flags = MAP_PRIVATE | MAP_ANONYMOUS = 0x02 | 0x20 = 0x22.
   * WORK: mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1,
   * 0)
   *
   * MEMORY STATE CALCULATION:
   * 1. Inputs:
   *    - ADDR = NULL (Kernel decides)
   *    - LEN = 4096 (1 Page)
   *    - PROT = 0x3 (R+W)
   *    - FLAGS = 0x22 (Anon + Private)
   *    - FD = -1 (Ignored for Anon)
   *    - OFFSET = 0
   * 2. Kernel Action:
   *    - Allocates struct vm_area_struct (VMA)
   *    - Range: [0x7f...2000, 0x7f...3000]
   *    - vm_ops = NULL (Generic Anon)
   *    - PAGE TABLES ARE EMPTY (No physical RAM yet)
   * 3. Output:
   *    - anon_ptr = 0x7f...2000 (Address of new VMA start)
   */
  anon_ptr = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); /* FILL THIS */

  if (anon_ptr == MAP_FAILED) {
    perror("mmap anon");
    return 1;
  }

  /*
   * TODO 4: TRIGGER PAGE FAULT (ANON)
   * ---------------------------------
   * AXIOM: First write to Anon VMA -> Kernel allocs ZERO page or new page.
   * WORK: Write 'A' to first byte of anon_ptr.
   *
   * CALCULATION TRACE (ANON FAULT):
   * 1. anon_ptr = 0x7f...2000
   * 2. Write 'A' (0x41)
   * 3. #PF Exception
   * 4. Kernel sets up struct anon_vma for the Page.
   * 5. Kernel Allocates Physical Page (PFN: 0x67890)
   * 6. Kernel sets: page->mapping = (anon_vma ptr | 0x1)
   *    - anon_vma Ptr = 0xffff8880abcd1230 (Aligned to 8)
   *    - OR 0x1 (PAGE_MAPPING_ANON)
   *    - Result: 0xffff8880abcd1231
   * 7. PTE Updated to point to PFN 0x67890
   */
  anon_ptr[0] = 'A';

  printf("\n[2] ANONYMOUS MAPPING:\n");
  printf("    VA: %p\n", anon_ptr);
  printf("    Action: mmap(ANON, PRIVATE) + write\n");

  /*
   * TODO 5: TRIGGER KSM DEDUPLICATION (STATE 11)
   * --------------------------------------------
   * AXIOM: KSM (Kernel Samepage Merging) scans memory for identical pages.
   * AXIOM: Userspace must opt-in via madvise(MADV_MERGEABLE).
   * AXIOM: If pages merge, kernel updates mapping to (ksm_stable_node | 0x3).
   *
   * WORK:
   * 1. Allocate 2 Anonymous Pages with IDENTICAL CONTENT.
   * 2. Mark them MERGEABLE.
   * 3. Wait for ksmd to calculate the hash and merge them.
   *
   * MEMORY TRACE:
   * 1. ksm_ptr1, ksm_ptr2 allocated.
   * 2. memset(0xCC) -> Both have same hash.
   * 3. madvise() -> Adds range to KSM scan list.
   * 4. sleep() -> Gives ksmd (kernel thread) time to run.
   */
  char *ksm_ptr1, *ksm_ptr2;

  // 1. Alloc
  ksm_ptr1 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ksm_ptr2 = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ksm_ptr1 == MAP_FAILED || ksm_ptr2 == MAP_FAILED) {
    perror("mmap ksm");
    return 1;
  }

  // 2. Fill (Make them candidates for merging)
  // Trick: Use something non-zero so we don't just get the Zero Page.
  memset(ksm_ptr1, 0xCC, PAGE_SIZE);
  memset(ksm_ptr2, 0xCC, PAGE_SIZE);

  // 3. Mark Mergeable
  if (madvise(ksm_ptr1, PAGE_SIZE, MADV_MERGEABLE) < 0)
    perror("madvise 1");
  if (madvise(ksm_ptr2, PAGE_SIZE, MADV_MERGEABLE) < 0)
    perror("madvise 2");

  printf("\n[3] KSM CANDIDATES CREATED:\n");
  printf("    VA1: %p\n", ksm_ptr1);
  printf("    VA2: %p\n", ksm_ptr2);
  printf("    Action: mmap(ANON) + memset + madvise(MERGEABLE)\n");
  printf("    Status: Waiting 5 seconds for KSM daemon...\n");
  sleep(5);

  // --- STEP 3: WAIT FOR INSPECTION ---
  printf("\n=== READY FOR KERNEL INSPECTION (NORMAL) ===\n");
  printf("Runs these commands manually:\n");
  printf("  sudo insmod mapping_hw.ko target_pid=%d target_va=0x%lx (FILE)\n",
         getpid(), (unsigned long)file_ptr);
  printf("  sudo insmod mapping_hw.ko target_pid=%d target_va=0x%lx (ANON)\n",
         getpid(), (unsigned long)anon_ptr);
  // NOTE: Inspecting KSM might fail if merge hasn't happened yet.
  printf("  sudo insmod mapping_hw.ko target_pid=%d target_va=0x%lx (KSM1)\n",
         getpid(), (unsigned long)ksm_ptr1);

  // --- STEP 4: CREATE HUGE PAGE (COMPOUND) ---
  /*
   * TODO 6: TRIGGER THP (COMPOUND PAGE) - MANUAL ALIGNMENT
   * ------------------------------------------------------
   * AXIOM 1: Huge Page Size = 2MB = 2 * 1024 * 1024 = 2097152 bytes = 0x200000.
   * AXIOM 2: `mmap` usually gives 4KB alignment (0x1000). THP needs 2MB
   * alignment.
   *
   * TASK: Allocate memory such that we can find a 2MB aligned address inside
   * it. CALCULATION TRACE:
   * 1. We need 2MB of valid memory STARTING at address X where X % 2MB == 0.
   * 2. If we alloc exactly 2MB, OS might give range [0x1001000, 0x1201000].
   *    -> This is NOT aligned. 0x1001000 % 0x200000 != 0.
   * 3. If we alloc 4MB (2x Size), we guarantee a 2MB boundary exists inside.
   *    -> Range [0x1001000, 0x1401000].
   *    -> Aligned Addr = Nearest 2MB boundary up from Start.
   *
   * FORMULA (ALIGN UP):
   *    Aligned = (Raw + (Alignment - 1)) & ~(Alignment - 1)
   *
   * WORK:
   * 1. mmap 4MB.
   * 2. Calculate `thp_ptr` aligned to 0x200000.
   * 3. madvise(thp_ptr, 2MB, MADV_HUGEPAGE).
   * 4. Write to `thp_ptr[0]` (Head) and `thp_ptr[PAGE_SIZE]` (Tail).
   */
  void *thp_raw;
  char *thp_ptr;

  // 1. ALLOCATE 4MB
  thp_raw = mmap(NULL, 1 << 22, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  // ...

  // 2. CALCULATE ALIGNED ADDRESS (DO THE MATH MANUALLY)
  unsigned long raw_addr = (unsigned long)thp_raw;
  unsigned long aligned_addr = (raw_addr + (1 << 21) - 1) & ~((1 << 21) - 1);
  thp_ptr = (char *)aligned_addr;

  // 3. MARK HUGEPAGE
  if (madvise(thp_ptr, 1 << 21, MADV_HUGEPAGE) < 0)
    perror("madvise thp");

  // 4. TRIGGER FAULT
  thp_ptr[0] = 'A';
  thp_ptr[PAGE_SIZE] = 'B';

  printf("\n[4] HUGE PAGE (COMPOUND) CREATED:\n");
  printf("    Raw mmap:  %p\n", thp_raw);
  printf("    Aligned:   %p (2MB aligned)\n", thp_ptr);
  printf("    HEAD:      %p (offset 0)\n", thp_ptr);
  printf("    TAIL1:     %p (offset 4096)\n", thp_ptr + PAGE_SIZE);
  printf("\n  Inspect with:\n");
  printf("  sudo insmod mapping_hw.ko target_pid=%d target_va=0x%lx (HEAD)\n",
         getpid(), (unsigned long)thp_ptr);
  printf("  sudo insmod mapping_hw.ko target_pid=%d target_va=0x%lx (TAIL)\n",
         getpid(), (unsigned long)(thp_ptr + PAGE_SIZE));

  printf("\nPress ENTER to clean up and exit...");
  getchar();

  // CLEANUP
  munmap(thp_raw, 1 << 22);

  munmap(file_ptr, PAGE_SIZE);
  munmap(anon_ptr, PAGE_SIZE);
  munmap(ksm_ptr1, PAGE_SIZE);
  munmap(ksm_ptr2, PAGE_SIZE);
  close(fd);
  unlink("temp_map_file");
  return 0;
}
