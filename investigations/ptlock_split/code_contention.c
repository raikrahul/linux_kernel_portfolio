/*
 * PTLOCK SPLIT DEMONSTRATION - USERSPACE CODE
 * ============================================
 *
 * AXIOM SOURCE (from this machine, live data):
 *   PAGE_SIZE = 4096 bytes ← getconf PAGE_SIZE
 *   CPU count = 12         ← nproc
 *   PTE size  = 8 bytes    ← sizeof(unsigned long) on x86_64
 *   PTEs per page = 4096 / 8 = 512
 *   Coverage per PTE page = 512 × 4096 = 2097152 = 0x200000 = 2MB
 *
 * PROBLEM THIS CODE DEMONSTRATES:
 *   Thread A accesses virtual address VA_A
 *   Thread B accesses virtual address VA_B
 *   If VA_A and VA_B are in SAME 2MB region → SAME PTE page → SAME lock
 *   If VA_A and VA_B are in DIFFERENT 2MB regions → DIFFERENT PTE pages →
 * DIFFERENT locks
 *
 * YOUR TASK:
 *   Fill TODO blocks to create addresses that land in SAME vs DIFFERENT PTE
 * pages.
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

/*
 * AXIOM: Virtual address structure on x86_64 (48-bit addressing)
 * ───────────────────────────────────────────────────────────────
 * Bits 47-39: PGD index (9 bits) → selects 1 of 512 PGD entries
 * Bits 38-30: PUD index (9 bits) → selects 1 of 512 PUD entries
 * Bits 29-21: PMD index (9 bits) → selects 1 of 512 PMD entries  ← THIS
 * determines PTE page Bits 20-12: PTE index (9 bits) → selects 1 of 512 PTEs
 * within PTE page Bits 11-0:  Page offset (12 bits) → offset within 4KB data
 * page
 *
 * KEY INSIGHT:
 *   If bits 47-21 are SAME → same PGD→PUD→PMD path → SAME PTE page
 *   If bits 29-21 differ  → different PMD entry → DIFFERENT PTE page
 */

#define PAGE_SIZE_CONST 4096
#define PTE_COVERAGE (512 * PAGE_SIZE_CONST) /* = 2097152 = 2MB */

/* Global region - will be mmap'd later */
static char *region = NULL;
static size_t region_size = 0;

/* Timing results per thread */
static long thread_time_ns[12];

/*
 * NUMERICAL CALCULATION BLOCK:
 * ────────────────────────────
 * mmap returns base address, let's say: 0x7f1234000000
 *
 * PTE page coverage = 2MB = 0x200000
 *
 * Address 0x7f1234000000 is in PTE page covering: 0x7f1234000000 -
 * 0x7f12341FFFFF Address 0x7f1234100000 is in PTE page covering: 0x7f1234000000
 * - 0x7f12341FFFFF (SAME!) Address 0x7f1234200000 is in PTE page covering:
 * 0x7f1234200000 - 0x7f12343FFFFF (DIFFERENT!)
 *
 * To be in SAME PTE page:   offset from base < 2MB
 * To be in DIFFERENT PTE page: offset from base >= 2MB
 */

void *thread_touch_same_pte_page(void *arg) {
  /*
   * THREAD FUNCTION: Touch addresses in SAME PTE page as another thread.
   *
   * INPUT arg = thread index (0 to 11)
   *
   * AXIOM: Two addresses in same 2MB region use same PTE page.
   *
   * NUMERICAL EXAMPLE:
   *   base = 0x7f1234000000 (example mmap return)
   *   thread 0 offset = 0 × 4096 = 0x0000 → address 0x7f1234000000
   *   thread 1 offset = 1 × 4096 = 0x1000 → address 0x7f1234001000
   *   ...
   *   thread 11 offset = 11 × 4096 = 0xB000 → address 0x7f123400B000
   *
   *   All offsets < 2MB (0x200000)
   *   ∴ All addresses in SAME PTE page
   *   ∴ All threads compete for SAME lock (in kernel)
   */

  int tid = (int)(long)arg;

  /* Pin to specific CPU for consistent measurement */
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(tid % 12, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 1: Calculate offset for this thread so ALL threads land in SAME PTE
   * page.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * GIVEN:
   *   tid = thread index (0, 1, 2, ... 11)
   *   region = base address of mmap'd memory
   *   PAGE_SIZE_CONST = 4096
   *
   * CONSTRAINT:
   *   offset must be < 2MB (0x200000) so all threads use SAME PTE page
   *
   * NUMERICAL CALCULATION (work this out):
   *   If tid = 0: offset = ?
   *   If tid = 5: offset = ?
   *   If tid = 11: offset = ?
   *
   *   MAX offset = 11 × 4096 = 45056 = 0xB000
   *   0xB000 < 0x200000 ✓ (all fit in same 2MB)
   *
   * WRITE YOUR CALCULATION HERE:
   *   offset = ____________________
   *
   * TRAP: What if you multiply by 4097 instead of 4096?
   *       Would all still be in same PTE page?
   *       11 × 4097 = 45067 < 2097152 ✓ (still same page, but misaligned!)
   */

  size_t offset = /* TODO: YOUR FORMULA HERE */ 0;

  /* Touch this page - triggers page fault if first access */
  char *addr = region + offset;

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 2: Write something to the page to trigger page fault.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * AXIOM: Reading unmapped page → segfault (no write permission to add
   * mapping) Writing unmapped page → page fault → kernel allocates page → adds
   * PTE
   *
   * WHAT TO WRITE: Any value. Just need to trigger the fault.
   *
   * NUMERICAL TRACE:
   *   addr = 0x7f1234000000 + offset
   *   *addr = 'X' means: write byte 0x58 to RAM at virtual address (addr)
   *   CPU → MMU → check PTE[???] → not present → PAGE FAULT → kernel
   *   Kernel → allocate physical page → write PTE → return
   *   CPU → retry → MMU → PTE present → translate → write to physical RAM
   *
   * TRAP: What if you read instead of write? (e.g., char c = *addr;)
   *       If page is MAP_ANONYMOUS with PROT_READ|PROT_WRITE, read also faults.
   *       But write is cleaner for demonstration.
   */

  /* TODO: Write to *addr to trigger page fault */

  clock_gettime(CLOCK_MONOTONIC, &end);
  thread_time_ns[tid] =
      (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

  return NULL;
}

void *thread_touch_different_pte_pages(void *arg) {
  /*
   * THREAD FUNCTION: Touch addresses in DIFFERENT PTE pages.
   *
   * AXIOM: Two addresses in different 2MB regions use different PTE pages.
   *
   * NUMERICAL EXAMPLE:
   *   base = 0x7f1234000000
   *   thread 0 offset = 0 × 2MB = 0x000000 → address 0x7f1234000000 → PTE page
   * 0 thread 1 offset = 1 × 2MB = 0x200000 → address 0x7f1234200000 → PTE page
   * 1 thread 2 offset = 2 × 2MB = 0x400000 → address 0x7f1234400000 → PTE page
   * 2
   *   ...
   *
   *   All offsets are multiples of 2MB
   *   ∴ All addresses in DIFFERENT PTE pages
   *   ∴ Each thread uses DIFFERENT lock (in kernel)
   *   ∴ NO CONTENTION (parallel execution)
   */

  int tid = (int)(long)arg;

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(tid % 12, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 3: Calculate offset so EACH thread lands in DIFFERENT PTE page.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * GIVEN:
   *   tid = thread index (0, 1, 2, ... 11)
   *   PTE_COVERAGE = 2097152 = 2MB = 0x200000
   *
   * CONSTRAINT:
   *   Each thread must be in different 2MB region.
   *   offset_thread_N must be at least N × 2MB from base.
   *
   * NUMERICAL CALCULATION:
   *   thread 0: offset = 0 × 2097152 = 0
   *   thread 1: offset = 1 × 2097152 = 2097152 = 0x200000
   *   thread 5: offset = 5 × 2097152 = 10485760 = 0xA00000
   *   thread 11: offset = 11 × 2097152 = 23068672 = 0x1600000
   *
   * VERIFY each is in different 2MB chunk:
   *   0 / 2097152 = 0 → PTE page 0
   *   2097152 / 2097152 = 1 → PTE page 1
   *   23068672 / 2097152 = 11 → PTE page 11
   *   All different ✓
   *
   * TRAP: What if you use (tid × 2MB + 1)?
   *       Still in different pages, but not page-aligned!
   *       Works but wasteful.
   *
   * WRITE YOUR FORMULA:
   *   offset = ____________________
   */

  size_t offset = /* TODO: YOUR FORMULA HERE */ 0;

  char *addr = region + offset;

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 4: Write to trigger page fault (same as TODO 2).
   * ═══════════════════════════════════════════════════════════════════════
   */

  /* TODO: Write to *addr */

  clock_gettime(CLOCK_MONOTONIC, &end);
  thread_time_ns[tid] =
      (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

  return NULL;
}

int main(int argc, char *argv[]) {
  printf("PTLOCK SPLIT DEMONSTRATION\n");
  printf("==========================\n\n");

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 5: Calculate required mmap size for 12 threads in DIFFERENT PTE pages.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * AXIOM:
   *   12 threads, each in different 2MB region = 12 × 2MB = 24MB minimum
   *
   * NUMERICAL CALCULATION:
   *   PTE_COVERAGE = 2097152 bytes = 2MB
   *   Threads = 12
   *   Minimum size = 12 × 2097152 = ?
   *
   *   CALCULATE BY HAND:
   *     12 × 2097152
   *     = 12 × 2000000 + 12 × 97152
   *     = 24000000 + 1165824
   *     = 25165824 bytes
   *
   *   In hex: 25165824 = 0x1800000
   *
   *   Verify: 0x1800000 / 0x200000 = 12 ✓
   *
   * WRITE YOUR CALCULATION:
   *   region_size = ____________________
   */

  region_size = /* TODO: YOUR CALCULATION HERE */ 0;

  printf("Allocating region of size: %zu bytes = %zu MB\n", region_size,
         region_size / (1024 * 1024));

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 6: Call mmap to allocate the region.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * MAN PAGE (mmap):
   *   void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t
   * offset);
   *
   *   addr   = NULL (let kernel choose)
   *   length = region_size (from TODO 5)
   *   prot   = PROT_READ | PROT_WRITE (need write to trigger fault)
   *   flags  = MAP_PRIVATE | MAP_ANONYMOUS (no file backing)
   *   fd     = -1 (anonymous)
   *   offset = 0
   *
   * RETURN VALUE:
   *   On success: pointer to mapped area (e.g., 0x7f1234000000)
   *   On failure: MAP_FAILED (which is (void*)-1)
   *
   * NUMERICAL TRACE:
   *   mmap(NULL, 25165824, 3, 34, -1, 0)
   *   3 = PROT_READ(1) | PROT_WRITE(2)
   *   34 = MAP_PRIVATE(2) | MAP_ANONYMOUS(32)
   *   Returns: 0x7f???? (some kernel-chosen address)
   *
   * TRAP: If you forget MAP_ANONYMOUS, mmap expects a valid fd!
   * TRAP: If you forget PROT_WRITE, write to region → SEGFAULT!
   */

  region = /* TODO: mmap call here */ NULL;

  if (region == MAP_FAILED) {
    perror("mmap failed");
    return 1;
  }

  printf("Region mapped at: %p\n\n", region);

  /* ═══════════════════════════════════════════════════════════════════════
   * TEST 1: All threads in SAME PTE page (HIGH CONTENTION expected)
   * ═══════════════════════════════════════════════════════════════════════
   */
  printf("TEST 1: 12 threads touching SAME PTE page (same 2MB region)\n");
  printf("─────────────────────────────────────────────────────────────\n");

  /* Reset timing */
  memset(thread_time_ns, 0, sizeof(thread_time_ns));

  pthread_t threads[12];
  for (int i = 0; i < 12; i++) {
    pthread_create(&threads[i], NULL, thread_touch_same_pte_page,
                   (void *)(long)i);
  }
  for (int i = 0; i < 12; i++) {
    pthread_join(threads[i], NULL);
  }

  /* Print results */
  long total_same = 0;
  for (int i = 0; i < 12; i++) {
    printf("  Thread %2d: %8ld ns\n", i, thread_time_ns[i]);
    total_same += thread_time_ns[i];
  }
  printf("  TOTAL: %ld ns\n\n", total_same);

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 7: Unmap and remap to reset page table state.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * WHY: After TEST 1, PTEs exist for touched pages.
   *      TEST 2 won't trigger faults if we reuse same region.
   *      Need fresh region with no PTEs.
   *
   * MAN PAGE (munmap):
   *   int munmap(void *addr, size_t length);
   *   Returns 0 on success, -1 on error.
   */

  /* TODO: munmap the region and mmap again */

  /* ═══════════════════════════════════════════════════════════════════════
   * TEST 2: Each thread in DIFFERENT PTE page (LOW CONTENTION expected)
   * ═══════════════════════════════════════════════════════════════════════
   */
  printf("TEST 2: 12 threads touching DIFFERENT PTE pages (different 2MB "
         "regions)\n");
  printf("─────────────────────────────────────────────────────────────────────"
         "───\n");

  memset(thread_time_ns, 0, sizeof(thread_time_ns));

  for (int i = 0; i < 12; i++) {
    pthread_create(&threads[i], NULL, thread_touch_different_pte_pages,
                   (void *)(long)i);
  }
  for (int i = 0; i < 12; i++) {
    pthread_join(threads[i], NULL);
  }

  long total_diff = 0;
  for (int i = 0; i < 12; i++) {
    printf("  Thread %2d: %8ld ns\n", i, thread_time_ns[i]);
    total_diff += thread_time_ns[i];
  }
  printf("  TOTAL: %ld ns\n\n", total_diff);

  /* ═══════════════════════════════════════════════════════════════════════
   * TODO 8: Calculate and print the speedup.
   * ═══════════════════════════════════════════════════════════════════════
   *
   * AXIOM: If split locks work, TEST 2 should be faster than TEST 1.
   *
   * NUMERICAL EXAMPLE:
   *   total_same = 120000 ns (contention)
   *   total_diff = 20000 ns (no contention)
   *   speedup = 120000 / 20000 = 6.0×
   *
   * EXPECTED: speedup ≥ 1.0 (usually much higher)
   *
   * TRAP: Division by zero if total_diff = 0!
   * TRAP: Integer division loses precision! Use (double).
   */

  double speedup = /* TODO: YOUR CALCULATION HERE */ 0.0;
  printf("SPEEDUP: %.2fx faster with split locks\n", speedup);

  /* Cleanup */
  munmap(region, region_size);

  return 0;
}
