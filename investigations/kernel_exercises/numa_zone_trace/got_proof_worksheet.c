/*
 * got_proof_worksheet.c
 *
 * COMPILE: gcc -o got_proof_worksheet got_proof_worksheet.c -ldl
 * RUN: ./got_proof_worksheet
 *
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 * REAL DATA FROM YOUR MACHINE (fetched 2025-12-28):
 *
 * nm -D /usr/lib/x86_64-linux-gnu/libc.so.6 | grep -w printf:
 *   0000000000060100 T printf@@GLIBC_2.2.5
 *
 * nm -D /usr/lib/x86_64-linux-gnu/libc.so.6 | grep -w scanf:
 *   00000000000662a0 T scanf@@GLIBC_2.2.5
 *
 * /proc/self/maps | grep "libc.*r-xp":
 *   760cee228000-760cee3b0000 r-xp 00028000 103:05 5160837 libc.so.6
 *
 * PARSED VALUES:
 *   libc_start (r-xp VMA)  = 0x760cee228000
 *   libc_offset (from maps) = 0x28000 = 163840 bytes
 *   vm_pgoff = 163840 / 4096 = 40 pages
 *   printf_file_offset = 0x60100 = 393472 bytes
 *   scanf_file_offset = 0x662a0 = 418464 bytes
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 *
 * TEST CASES (expected output):
 *
 * T01: printf vaddr = libc_start + (printf_offset - vm_pgoff_bytes)
 *      = 0x760cee228000 + (0x60100 - 0x28000)
 *      = 0x760cee228000 + 0x38100
 *      = 0x760cee260100
 *      dlsym("printf") should return 0x760cee260100 → MATCH ✓
 *
 * T02: scanf vaddr = libc_start + (scanf_offset - vm_pgoff_bytes)
 *      = 0x760cee228000 + (0x662a0 - 0x28000)
 *      = 0x760cee228000 + 0x3E2A0
 *      = 0x760cee2662a0
 *      dlsym("scanf") should return 0x760cee2662a0 → MATCH ✓
 *
 * FAILURE PREDICTIONS:
 * F1: User forgets to subtract vm_pgoff_bytes → vaddr too high by 0x28000
 * F2: User uses wrong libc VMA (r--p instead of r-xp) → wrong libc_start
 * F3: User divides instead of subtracts → completely wrong result
 * F4: User uses decimal instead of hex → parsing error or wrong calculation
 *
 * RARE BUG TO WATCH:
 * // BUG-PRONE: sscanf with %lx returns unsigned, but store in signed long →
 * // addresses > 0x7FFFFFFFFFFFFFFF become negative → comparisons fail
 * // FIX: Always use unsigned long for addresses
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 */

#define _GNU_SOURCE
#include <dlfcn.h>  /* dlsym, RTLD_NEXT, RTLD_DEFAULT, dladdr, Dl_info */
#include <link.h>   /* ELF link structures */
#include <stdio.h>  /* printf, fopen, fgets, sscanf, fclose */
#include <stdlib.h> /* NULL */
#include <string.h> /* strstr */

/*
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 * MEMORY LAYOUT CONSTANTS (from your machine):
 *
 * PAGE_SIZE = 4096 = 0x1000 = 2^12
 * WHY 4096: PTE uses bits 11-0 for page offset → 12 bits → 2^12 = 4096
 *
 * PRINTF_OFFSET = 0x60100 = 393472
 * CALCULATION: 0x60100 = 6×16^4 + 0×16^3 + 1×16^2 + 0×16 + 0
 *            = 6×65536 + 0 + 256 + 0 + 0
 *            = 393216 + 256
 *            = 393472 ✓
 *
 * SCANF_OFFSET = 0x662a0 = 418464
 * CALCULATION: 0x662a0 = 6×16^4 + 6×16^3 + 2×16^2 + 10×16 + 0
 *            = 393216 + 24576 + 512 + 160 + 0
 *            = 418464 ✓
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 */
#define PAGE_SIZE 4096
#define PRINTF_OFFSET 0x60100 /* 393472 bytes from file start */
#define SCANF_OFFSET 0x662a0  /* 418464 bytes from file start */

int main(void) {
  /*
   * STACK FRAME (approx 700 bytes):
   * ┌────────────────────────────────────────────────────────────────────────┐
   * │ RBP-0x08: printf_addr (8 bytes) = result of dlsym                      │
   * │ RBP-0x10: scanf_addr (8 bytes) = result of dlsym                       │
   * │ RBP-0x18: libc_start (8 bytes) = 0x760cee228000                        │
   * │ RBP-0x20: libc_offset (8 bytes) = 0x28000                              │
   * │ RBP-0x28: f (8 bytes, FILE*)                                           │
   * │ RBP-0x228: line[512] (512 bytes)                                       │
   * │ RBP-0x230: expected_printf (8 bytes)                                    │
   * │ RBP-0x238: expected_scanf (8 bytes)                                     │
   * └────────────────────────────────────────────────────────────────────────┘
   */
  void *printf_addr;
  void *scanf_addr;
  unsigned long libc_start = 0;
  unsigned long libc_offset = 0;
  FILE *f;
  char line[512];
  unsigned long expected_printf;
  unsigned long expected_scanf;

  printf("=== GOT DYNAMIC LINKING WORKSHEET ===\n\n");

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 1: Get actual printf address from dynamic linker
   *
   * dlsym(RTLD_NEXT, "printf"):
   *   RTLD_NEXT = search DSOs loaded AFTER current one
   *   Returns: 0x760cee260100 (verified on your machine)
   *
   * WHAT HAPPENS INTERNALLY:
   *   dl_runtime_resolve → hash lookup in libc.so.6 .dynsym → find "printf" →
   *   base + symbol_offset → 0x760cee200000 + 0x60100 = 0x760cee260100
   *   (wait, that's wrong! base is 0x200000 not 0x228000, see note below)
   *
   * TRAP: dli_fbase (ELF base) ≠ r-xp VMA start
   *   dli_fbase = 0x760cee200000 (where libc ELF loads)
   *   r-xp VMA = 0x760cee228000 (where .text section starts)
   *   Difference = 0x28000 = vm_pgoff_bytes
   *
   * BOILERPLATE: */
  printf_addr = dlsym(RTLD_NEXT, "printf");
  if (!printf_addr) {
    printf_addr = dlsym(RTLD_DEFAULT, "printf");
  }
  printf("01. printf actual vaddr = %p\n", printf_addr);

  scanf_addr = dlsym(RTLD_NEXT, "scanf");
  if (!scanf_addr) {
    scanf_addr = dlsym(RTLD_DEFAULT, "scanf");
  }
  printf("02. scanf actual vaddr = %p\n", scanf_addr);

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 2: Parse /proc/self/maps to find libc r-xp VMA
   *
   * WHY r-xp not r--p?
   *   r-xp = read + execute → .text section → contains printf/scanf code
   *   r--p = read only → .rodata, ELF header → no executable code
   *
   * LINE FORMAT: "start-end perms offset dev inode pathname"
   *   760cee228000-760cee3b0000 r-xp 00028000 103:05 5160837 libc.so.6
   *   ↑ libc_start              ↑ perms ↑ offset
   *
   * BOILERPLATE: */
  f = fopen("/proc/self/maps", "r");
  if (!f) {
    perror("fopen /proc/self/maps");
    return 1;
  }

  while (fgets(line, sizeof(line), f)) {
    /* TODO BLOCK 01: Check if line contains libc AND r-xp section
     *
     * HINTS:
     *   strstr(haystack, needle) returns pointer if found, NULL if not
     *   strstr(line, "libc") → finds libc mapping
     *   strstr(line, "r-xp") → ensures executable section
     *
     * WHY BOTH? libc has 5 VMAs, we want the one with code
     *   r--p offset 0x00000 → ELF header, .rodata (not code)
     *   r-xp offset 0x28000 → .text section (printf, scanf here) ← THIS ONE
     *   r--p offset 0x1b0000 → more .rodata
     *   r--p offset 0x1fe000 → GOT
     *   rw-p offset 0x202000 → .data, .bss
     */
    if (strstr(line, "libc") && strstr(line, "r-xp")) {

      /* TODO BLOCK 02: Parse libc_start and libc_offset from line
       *
       * LINE: "760cee228000-760cee3b0000 r-xp 00028000 103:05 5160837 libc"
       *
       * sscanf format: "%lx-%*lx %*s %lx"
       *   %lx → read hex into libc_start (760cee228000)
       *   -%*lx → skip until space (discard end address)
       *   %*s → skip perms string (discard "r-xp")
       *   %lx → read hex into libc_offset (00028000)
       *
       * CALCULATION after parsing:
       *   libc_start = 0x760cee228000 = 129,854,427,070,464 decimal
       *   libc_offset = 0x28000 = 163,840 decimal
       *   vm_pgoff = 163840 / 4096 = 40 pages
       */
      sscanf(line, "%lx-%*lx %*s %lx", &libc_start, &libc_offset);

      printf("03. libc r-xp VMA: start=0x%lx, offset=0x%lx\n", libc_start,
             libc_offset);
      break;
    }
  }
  fclose(f);

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 3: Calculate expected printf vaddr
   *
   * CRITICAL: mm_struct IS PER PROCESS (proven on your machine 2025-12-28):
   * ┌───────────────────────────────────────────────────────────────────────────┐
   * │ ./mm_per_process_proof output: │ │ PARENT (PID=94519):
   * vaddr=0x777bc6c28000, inode=5160837                   │ │ CHILD
   * (PID=94520): vaddr=0x777bc6c28000, inode=5160837                   │ │ │ │
   * SAME inode=5160837 → SAME libc.so.6 file on disk                          │
   * │ SAME vaddr → ASLR inherited from parent at fork (dup_mm copies mm_struct)
   * │ │ BUT: each process has SEPARATE mm_struct, SEPARATE page table, SEPARATE
   * CR3│ │ │ │ HOW KERNEL KNOWS WHICH VMA TREE ON #PF: │ │   current = kernel
   * global pointing to running task_struct                 │ │   current->mm =
   * this process's mm_struct                                  │ │
   * current->mm->mmap = this process's VMA tree (red-black tree)            │
   * │   #PF handler: mm = current->mm; find_vma(mm, fault_addr); │
   * └───────────────────────────────────────────────────────────────────────────┘
   *
   * VMA STRUCTURE (kernel memory):
   * ┌───────────────────────────────────────────────────────────────────────────┐
   * │ PID 94519 task_struct @ 0xFFFF888011110000 │ │ ├── mm =
   * 0xFFFF888022220000 (PID 94519's mm_struct)                       │ │ │ ├──
   * pgd = 0xFFFF888033330000 → loaded into CR3 when 94519 runs        │ │ │ └──
   * mmap → VMA tree for 94519: libc VMA @ 0x777bc6c28000-0x777bc6db0000│ │ │
   * ├── vm_start = 0x777bc6c28000                               │ │ │ ├──
   * vm_end = 0x777bc6db0000                                 │ │ │ ├── vm_pgoff
   * = 40                                           │ │ │             ├──
   * vm_file → struct file for inode 5160837                 │ │ │ └── vm_mm =
   * 0xFFFF888022220000 (back pointer to mm_struct)  │
   * ├───────────────────────────────────────────────────────────────────────────┤
   * │ PID 94520 task_struct @ 0xFFFF888044440000 (child, forked from 94519) │
   * │ ├── mm = 0xFFFF888055550000 (PID 94520's mm_struct, SEPARATE from parent)
   * │ │ │   ├── pgd = 0xFFFF888066660000 → loaded into CR3 when 94520 runs │ │
   * │   └── mmap → VMA tree for 94520: libc VMA @
   * 0x777bc6c28000-0x777bc6db0000│ │ │             (COPY of parent's VMA, same
   * vaddrs, but SEPARATE structure) │
   * └───────────────────────────────────────────────────────────────────────────┘
   *
   * PAGE FAULT TRACE when CPU accesses vaddr 0x777bc6c60100 (printf):
   * 01. CPU at RIP tries to execute instruction at vaddr 0x760cee260100
   * 02. MMU walks page table: CR3 → PML4 → PDPT → PD → PT → PTE not present
   * 03. MMU triggers #PF exception → CPU saves RIP, jumps to interrupt 14
   * handler 04. Kernel page fault handler reads fault_address = 0x760cee260100
   * 05. Kernel searches mm->mmap (VMA tree): find VMA where vm_start ≤
   * 0x760cee260100 < vm_end 06. Found VMA at 0xFFFF888012340000: 0x760cee228000
   * ≤ 0x760cee260100 < 0x760cee3b0000 ✓ 07. Kernel calculates file_page =
   * vm_pgoff + (fault_addr - vm_start) / 4096 = 40 + (0x760cee260100 -
   * 0x760cee228000) / 4096 = 40 + 0x38100 / 4096 = 40 + 229632 / 4096 = 40
   * + 56.0625 → floor → 40 + 56 = 96 08. Kernel calls filemap_fault() → reads
   * file page 96 from disk (or page cache) 09. File page 96 = bytes 96×4096 to
   * 96×4096+4095 = bytes 393216 to 397311 of libc.so.6
   * 10. printf @ file offset 0x60100 = 393472 → 393472 is in [393216, 397311] ✓
   * 11. Kernel allocates physical page → PFN = 0x5000 (example)
   * 12. Kernel copies libc.so.6 bytes 393216-397311 to RAM at PFN×4096 =
   * 0x5000000
   * 13. Kernel creates PTE: vaddr 0x760cee260000 → PFN 0x5000 → flags =
   * present|user|execute
   * 14. Kernel returns from #PF → CPU retries instruction at 0x760cee260100 →
   * now PTE present
   * 15. MMU translates: 0x760cee260100 → physical 0x5000000 + 0x100 = 0x5000100
   * 16. CPU fetches instruction bytes from RAM at physical 0x5000100 → printf
   * code executes
   *
   * FORMULA: vaddr = libc_start + (file_offset - vm_pgoff_bytes)
   *
   * DERIVATION:
   *   vm_pgoff_bytes = vm_pgoff × 4096 = 40 × 4096 = 163840 = 0x28000
   *   file_offset = 0x60100 = 393472 (printf position in libc file)
   *   offset_within_vma = file_offset - vm_pgoff_bytes = 393472 - 163840 =
   * 229632 = 0x38100 vaddr = vm_start + offset_within_vma = 0x760cee228000 +
   * 0x38100 = 0x760cee260100
   *
   * SCANF EXAMPLE (harder, verify same formula):
   *   file_offset = 0x662a0 = 418464
   *   offset_within_vma = 418464 - 163840 = 254624 = 0x3E2A0
   *   file_page = vm_pgoff + offset_within_vma / 4096 = 40 + 254624 / 4096 = 40
   * + 62.17... = 40 + 62 = 102 vaddr = 0x760cee228000 + 0x3E2A0 =
   * 0x760cee2662a0 scanf @ file page 102, byte offset within page = 254624 %
   * 4096 = 254624 - 62×4096 = 254624 - 253952 = 672
   */

  /* TODO BLOCK 03: Calculate expected_printf
   *
   * FORMULA: expected_printf = libc_start + (PRINTF_OFFSET - libc_offset)
   *
   * HAND CALCULATION:
   *   PRINTF_OFFSET = 0x60100 = 393472
   *   libc_offset = 0x28000 = 163840
   *   PRINTF_OFFSET - libc_offset = ???
   *   libc_start + ??? = ???
   */
  expected_printf = libc_start + PRINTF_OFFSET - libc_offset;

  /* TODO BLOCK 04: Calculate expected_scanf
   *
   * FORMULA: expected_scanf = libc_start + (SCANF_OFFSET - libc_offset)
   *
   * HAND CALCULATION:
   *   SCANF_OFFSET = 0x662a0 = 418464
   *   libc_offset = 0x28000 = 163840
   *   SCANF_OFFSET - libc_offset = ???
   *   libc_start + ??? = ???
   */
  expected_scanf = libc_start + SCANF_OFFSET - libc_offset;

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 4: Verify calculated == actual
   * BOILERPLATE: */
  printf("\n04. VERIFICATION:\n");
  printf("    printf: actual=%p, calculated=0x%lx → %s\n", printf_addr,
         expected_printf,
         ((unsigned long)printf_addr == expected_printf) ? "MATCH ✓"
                                                         : "MISMATCH ✗");
  printf("    scanf:  actual=%p, calculated=0x%lx → %s\n", scanf_addr,
         expected_scanf,
         ((unsigned long)scanf_addr == expected_scanf) ? "MATCH ✓"
                                                       : "MISMATCH ✗");

  /* ═══════════════════════════════════════════════════════════════════════════
   * STEP 5: Show the formula proof
   * BOILERPLATE: */
  printf("\n05. FORMULA PROOF:\n");
  printf("    printf_vaddr = libc_start + (PRINTF_OFFSET - libc_offset)\n");
  printf("                 = 0x%lx + (0x%x - 0x%lx)\n", libc_start,
         PRINTF_OFFSET, libc_offset);
  printf("                 = 0x%lx + 0x%lx\n", libc_start,
         (unsigned long)(PRINTF_OFFSET - libc_offset));
  printf("                 = 0x%lx ✓\n", expected_printf);

  return 0;
}

/*
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 * ANSWERS (do not look until you've completed all TODO blocks):
 *
 * TODO 01: strstr(line, "libc") && strstr(line, "r-xp")
 * TODO 02: "%lx-%*lx %*s %lx"
 * TODO 03: libc_start + (PRINTF_OFFSET - libc_offset)
 * TODO 04: libc_start + (SCANF_OFFSET - libc_offset)
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 */
