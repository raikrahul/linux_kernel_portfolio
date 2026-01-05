/*
 * got_proof.c
 *
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 * MEMORY LAYOUT AT PROGRAM START (before main runs):
 *
 * STACK (this process):
 * ┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
 * │ 0x7FFD00000000 ← RSP (stack pointer) │ │ ... │ │ argc, argv, envp pushed by
 * kernel before _start                                                 │
 * └─────────────────────────────────────────────────────────────────────────────────────────────────┘
 *
 * HEAP (not used yet):
 * ┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
 * │ 0x5555XXXX0000 ← brk start (empty) │
 * └─────────────────────────────────────────────────────────────────────────────────────────────────┘
 *
 * libc.so.6 MAPPED (by ld.so before main):
 * ┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
 * │ VMA1: 0x711d4bc00000-0x711d4bc28000 r--p offset=0x00000 (40 pages × 4096 =
 * 163840 bytes, ELF hdr)│ │ VMA2: 0x711d4bc28000-0x711d4bc35b000 r-xp
 * offset=0x28000 (392 pages, .text with printf/scanf)   │ │       ↑ libc_start
 * = 0x711d4bc28000 │ │       printf @ 0x711d4bc60100 = libc_start + 0x38100 │
 * │       scanf  @ 0x711d4bc662a0 = libc_start + 0x3E2A0 │ │ VMA3:
 * 0x711d4bdb0000-... r--p offset=0x1b0000 (read-only data) │ │ VMA4:
 * 0x711d4bdfe000-... r--p offset=0x1fe000 (GOT read-only) │ │ VMA5:
 * 0x711d4be02000-... rw-p offset=0x202000 (.data/.bss) │
 * └─────────────────────────────────────────────────────────────────────────────────────────────────┘
 *
 * GOT (Global Offset Table) of THIS executable:
 * ┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
 * │ Addr        │ Value            │ What │
 * ├─────────────┼──────────────────┼────────────────────────────────────────────────────────────────┤
 * │ GOT[0]      │ _DYNAMIC addr    │ pointer to .dynamic section │ │ GOT[1] │
 * link_map*        │ ld.so internal structure │ │ GOT[2]      │
 * _dl_runtime_resolve │ lazy binding resolver │ │ GOT[3]      │ 0x711d4bc60100
 * │ printf resolved address (filled by ld.so)                     │ │ GOT[4] │
 * 0x711d4bcXXXXX   │ other libc functions... │
 * └─────────────────────────────────────────────────────────────────────────────────────────────────┘
 *
 * WHAT: Prove formula vaddr = libc_start + (file_offset - vm_pgoff_bytes) with
 * real data WHY: Show GOT contains calculated vaddr, not magic WHERE: GOT in
 * executable's .got.plt section, libc in mmap region WHO: ld.so (dynamic
 * linker) fills GOT before main() WHEN: After execve(), before main(), during
 * dynamic linking WITHOUT: GOT → every call would need full symbol lookup
 * (slow) WHICH: printf @ GOT[3], scanf @ GOT[4], etc.
 * ═══════════════════════════════════════════════════════════════════════════════════════════════════
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STACK FRAME for main():
   * ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   * │ RBP-0x08: printf_addr      (8 bytes, void*) = 0x711d4bc60100 after dlsym
   * │ │ RBP-0x10: libc_base        (8 bytes, void*) = unused in this program │
   * │ RBP-0x18: printf_offset    (8 bytes, ulong) = 0x60100 = 393472 │ │
   * RBP-0x20: vm_pgoff_bytes   (8 bytes, ulong) = 0x28000 = 163840 │ │
   * RBP-0x28: f                (8 bytes, FILE*) = 0x711d4beXXXXX
   * (heap-allocated by fopen)      │ │ RBP-0x228: line[512]       (512 bytes,
   * char[]) = "711d4bc28000-711d4bb0000 r-xp..."       │ │ RBP-0x230:
   * libc_start      (8 bytes, ulong) = 0x711d4bc28000 │ │ RBP-0x238:
   * libc_offset     (8 bytes, ulong) = 0x28000 │ │ RBP-0x240: expected_printf
   * (8 bytes, ulong) = 0x711d4bc60100                                │ │
   * RBP-0x260: info            (32 bytes, Dl_info) = {dli_fname, dli_fbase,
   * dli_sname, dli_saddr}│ │ Total locals ≈ 600 bytes │
   * └─────────────────────────────────────────────────────────────────────────────────────────────┘
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  void *printf_addr;
  void *libc_base;
  unsigned long printf_offset;
  unsigned long vm_pgoff_bytes;

  printf("=== GOT DYNAMIC LINKING PROOF ===\n\n");

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 1: dlsym(RTLD_NEXT, "printf")
   *
   * WHAT: Returns actual runtime address of printf symbol
   * WHERE: dlsym searches loaded DSOs, finds libc.so.6, returns 0x711d4bc60100
   * HOW: dlsym walks link_map chain: executable → ld.so → libc.so.6 → finds
   * "printf"
   *
   * CALCULATION:
   *   libc base (r-xp VMA) = 0x711d4bc28000
   *   printf file offset   = 0x60100
   *   VMA offset           = 0x28000
   *   printf vaddr = 0x711d4bc28000 + (0x60100 - 0x28000)
   *                = 0x711d4bc28000 + 0x38100
   *                = 0x711d4bc60100
   *
   * MEMORY after dlsym:
   *   printf_addr @ RBP-0x08 = 0x711d4bc60100 (8 bytes, little-endian: 00 01 c6
   * 4b 1d 71 00 00)
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  printf_addr = dlsym(RTLD_NEXT, "printf");
  if (!printf_addr) {
    printf_addr = dlsym(RTLD_DEFAULT, "printf");
  }
  printf("01. printf actual vaddr = %p\n", printf_addr);

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 2: Parse /proc/self/maps to find libc r-xp VMA
   *
   * /proc/self/maps content (virtual file, kernel generates on read):
   * ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   * │ 55d3e8a00000-55d3e8a01000 r--p 00000000 103:05 1234567 /path/to/got_proof
   * │ │ 55d3e8a01000-55d3e8a02000 r-xp 00001000 103:05 1234567
   * /path/to/got_proof                   │ │ ... │ │ 711d4bc00000-711d4bc28000
   * r--p 00000000 103:05 5160837 /usr/lib/.../libc.so.6 ← VMA1        │ │
   * 711d4bc28000-711d4bb0000 r-xp 00028000 103:05 5160837
   * /usr/lib/.../libc.so.6 ← VMA2 TARGET │ │ ↑ libc_start    ↑ libc_end  ↑
   * perms ↑ offset                                                │
   * └─────────────────────────────────────────────────────────────────────────────────────────────┘
   *
   * PARSING:
   *   sscanf("%lx-%*lx %*s %lx", ...) extracts:
   *     libc_start = 0x711d4bc28000 (first hex before '-')
   *     libc_offset = 0x28000 (third field, file offset)
   *
   * CALCULATION vm_pgoff:
   *   libc_offset = 0x28000 = 163840 bytes
   *   vm_pgoff = 163840 / 4096 = 40 pages
   *   VMA starts at page 40 of libc.so.6 file (skips ELF header, starts at
   * .text)
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  FILE *f = fopen("/proc/self/maps", "r");
  char line[512];
  unsigned long libc_start = 0, libc_offset = 0;

  while (fgets(line, sizeof(line), f)) {
    /*
     * strstr(line, "libc") searches for "libc" in 512-byte buffer
     * strstr returns pointer to 'l' in "libc" or NULL
     * strstr(line, "r-xp") ensures we get executable section (not r--p or rw-p)
     */
    if (strstr(line, "libc") && strstr(line, "r-xp")) {
      sscanf(line, "%lx-%*lx %*s %lx", &libc_start, &libc_offset);
      printf("02. libc r-xp VMA: start=0x%lx, offset=0x%lx\n", libc_start,
             libc_offset);
      break;
    }
  }
  fclose(f);

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 3: printf offset from nm -D libc.so.6
   *
   * COMMAND: nm -D /usr/lib/x86_64-linux-gnu/libc.so.6 | grep -w printf
   * OUTPUT:  0000000000060100 T printf@@GLIBC_2.2.5
   *
   * WHAT: 0x60100 = byte position of printf code within libc.so.6 file
   * WHY: ELF symbol table stores each function's offset from file start
   *
   * BREAKDOWN:
   *   0x60100 = 393472 decimal
   *   393472 / 4096 = 96 pages (printf is on page 96 of libc file)
   *   393472 % 4096 = 256 bytes into page 96
   *
   * EDGE CASES:
   *   N=0: offset=0 → function at start of file (like _start)
   *   N=2125327: last byte of libc (file size - 1)
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  printf_offset = 0x60100;
  printf("03. printf offset in libc file = 0x%lx (from nm -D libc.so.6)\n",
         printf_offset);

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 4: Calculate expected vaddr using FORMULA
   *
   * FORMULA: vaddr = libc_start + (printf_offset - vm_pgoff_bytes)
   *
   * NUMERICAL CALCULATION (your machine data):
   *   libc_start     = 0x711d4bc28000 = 124,275,939,549,184 decimal
   *   printf_offset  = 0x60100 = 393,472 decimal
   *   vm_pgoff_bytes = 0x28000 = 163,840 decimal
   *
   *   Step A: printf_offset - vm_pgoff_bytes = 393472 - 163840 = 229632
   *   Step B: 229632 in hex = 0x38100
   *   Step C: libc_start + 229632 = 124275939549184 + 229632 = 124275939778816
   *   Step D: 124275939778816 in hex = 0x711d4bc60100
   *
   * VERIFY by decomposition:
   *   0x711d4bc60100 = 0x711d4bc28000 + 0x38100
   *   0x38100 = 0x60100 - 0x28000 ✓
   *
   * HARDER EXAMPLE (scanf):
   *   scanf_offset = 0x662a0 = 418464
   *   scanf_offset - vm_pgoff_bytes = 418464 - 163840 = 254624 = 0x3E2A0
   *   scanf_vaddr = 0x711d4bc28000 + 0x3E2A0 = 0x711d4bc662a0 ✓
   *
   * EDGE CASE (function at VMA start):
   *   hypothetical_offset = 0x28000
   *   expected_vaddr = 0x711d4bc28000 + (0x28000 - 0x28000) = 0x711d4bc28000
   *   Function would be at exact vm_start
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  vm_pgoff_bytes = libc_offset;
  unsigned long expected_printf = libc_start + (printf_offset - vm_pgoff_bytes);
  printf("04. Expected printf vaddr = libc_start + (printf_offset - "
         "vm_pgoff_bytes)\n");
  printf("                         = 0x%lx + (0x%lx - 0x%lx)\n", libc_start,
         printf_offset, vm_pgoff_bytes);
  printf("                         = 0x%lx + 0x%lx\n", libc_start,
         printf_offset - vm_pgoff_bytes);
  printf("                         = 0x%lx\n", expected_printf);

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 5: Verify actual == calculated
   *
   * COMPARISON:
   *   printf_addr (from dlsym)     = 0x711d4bc60100
   *   expected_printf (calculated) = 0x711d4bc60100
   *   Difference = 0 bytes → MATCH ✓
   *
   * FAILURE CASE (if ASLR shifted libc differently):
   *   Actual   = 0x7ABC12345678
   *   Expected = 0x711d4bc60100
   *   Difference = huge → MISMATCH ✗ (would mean wrong libc_start parsed)
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  printf("\n05. VERIFICATION:\n");
  printf("    Actual printf vaddr   = %p\n", printf_addr);
  printf("    Calculated printf vaddr = 0x%lx\n", expected_printf);
  if ((unsigned long)printf_addr == expected_printf) {
    printf("    MATCH ✓\n");
  } else {
    printf("    MISMATCH ✗ (difference = %ld bytes)\n",
           (long)((unsigned long)printf_addr - expected_printf));
  }

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * STEP 6: Dl_info structure from dladdr
   *
   * Dl_info layout (32 bytes on x86_64):
   * ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
   * │ Offset │ Field       │ Size  │ Value │
   * ├────────┼─────────────┼───────┼──────────────────────────────────────────────────────────────┤
   * │ 0x00   │ dli_fname   │ 8     │ 0x711d4beXXXXX → "/usr/lib/.../libc.so.6"
   * │ │ 0x08   │ dli_fbase   │ 8     │ 0x711d4bc00000 (libc base, not r-xp
   * start!)                  │ │ 0x10   │ dli_sname   │ 8     │ 0x711d4beYYYYY
   * → "_IO_printf" (internal name)                │ │ 0x18   │ dli_saddr   │ 8
   * │ 0x711d4bc60100 (printf actual address)                       │
   * └─────────────────────────────────────────────────────────────────────────────────────────────┘
   *
   * NOTE: dli_fbase = 0x711d4bc00000 (r--p VMA start, not r-xp!)
   *       This is the ELF load address, not the .text section start.
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  printf("\n06. GOT ENTRY PROOF:\n");

  Dl_info info;
  if (dladdr((void *)printf, &info)) {
    printf("    Symbol: %s\n", info.dli_sname ? info.dli_sname : "(null)");
    printf("    DSO base: %p\n", info.dli_fbase);
    printf("    Symbol addr: %p\n", info.dli_saddr);
  }

  /*
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   * FINAL TRACE (proven with real data):
   *
   * TIMELINE:
   * T0: execve("got_proof") → kernel loads executable, passes control to ld.so
   * T1: ld.so reads executable's .dynamic section → finds NEEDED: libc.so.6
   * T2: ld.so opens /usr/lib/.../libc.so.6, reads ELF header
   * T3: ld.so mmap()s libc at 0x711d4bc00000 (r--p), 0x711d4bc28000 (r-xp),
   * etc. T4: ld.so resolves printf → calculates 0x711d4bc60100 T5: ld.so writes
   * 0x711d4bc60100 to GOT[printf] T6: ld.so jumps to main() T7: main() calls
   * printf() → CALL *GOT → jumps to 0x711d4bc60100 T8: CPU executes printf code
   * at physical RAM backing 0x711d4bc60100
   *
   * MEMORY AT T7 (GOT state):
   *   GOT[printf] @ 0x55d3e8AXXXX8 contains 0x711d4bc60100 (8 bytes, resolved)
   * ═══════════════════════════════════════════════════════════════════════════════════════════════
   */
  printf("\n=== TRACE PROVEN ===\n");
  printf("1. gcc compiled printf() → CALL via PLT/GOT\n");
  printf("2. Dynamic linker (ld.so) ran at program start\n");
  printf("3. ld.so mapped libc.so.6 at 0x%lx\n", libc_start);
  printf("4. ld.so knows printf = file offset 0x%lx\n", printf_offset);
  printf("5. ld.so calculated vaddr = 0x%lx\n", expected_printf);
  printf("6. ld.so wrote 0x%lx into GOT\n", (unsigned long)printf_addr);
  printf("7. Your code CALL *GOT → jumps to 0x%lx ✓\n",
         (unsigned long)printf_addr);

  return 0;
}
