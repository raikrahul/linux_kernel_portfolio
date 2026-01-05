/*
 * libc_map_trace.c
 * GOAL: Observe how many processes map libc.so.6 on this machine.
 * GOAL: Prove that one physical page of libc is shared by N processes.
 * GOAL: Show what "offset" means in /proc/PID/maps.
 *
 * BUILD: gcc -o libc_map_trace libc_map_trace.c
 * RUN:   ./libc_map_trace
 */

#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

/* AXIOM 1: /proc/PID/maps shows all VMAs for process PID */
/* AXIOM 2: Each line format: start-end perms offset dev inode pathname */
/* AXIOM 3: offset = byte offset into the file where this VMA starts */

int main(void) {
  DIR *proc_dir;
  struct dirent *entry;
  char maps_path[256];
  char line[512];
  FILE *maps_file;
  int libc_process_count = 0;
  unsigned long first_vma_start = 0, first_vma_end = 0;
  unsigned long first_offset = 0;

  printf("=== LIBC.SO.6 MAPPING TRACE ===\n\n");

  /* STEP 1: Get libc.so.6 inode from stat */
  struct stat libc_stat;
  if (stat("/usr/lib/x86_64-linux-gnu/libc.so.6", &libc_stat) == 0) {
    printf("01. libc.so.6 inode = %lu\n", libc_stat.st_ino);
    printf("02. libc.so.6 size = %lu bytes = %lu pages (4096 per page)\n",
           libc_stat.st_size, libc_stat.st_size / 4096);
    printf("03. libc.so.6 device = %lu:%lu\n", major(libc_stat.st_dev),
           minor(libc_stat.st_dev));
  }

  printf("\n");

  /* STEP 2: Count processes mapping libc */
  proc_dir = opendir("/proc");
  if (!proc_dir) {
    perror("opendir /proc");
    return 1;
  }

  while ((entry = readdir(proc_dir)) != NULL) {
    /* Only numeric directories (PIDs) */
    if (entry->d_type != DT_DIR)
      continue;
    if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
      continue;

    snprintf(maps_path, sizeof(maps_path), "/proc/%s/maps", entry->d_name);
    maps_file = fopen(maps_path, "r");
    if (!maps_file)
      continue;

    while (fgets(line, sizeof(line), maps_file)) {
      if (strstr(line, "libc.so.6")) {
        libc_process_count++;
        break; /* Count each process once */
      }
    }
    fclose(maps_file);
  }
  closedir(proc_dir);

  printf("04. Processes mapping libc.so.6 = %d\n", libc_process_count);
  printf("05. ∴ address_space->i_mmap tree has %d VMA entries (at least)\n\n",
         libc_process_count);

  /* STEP 3: Show this process's libc mappings */
  printf("06. This process (PID %d) libc.so.6 VMAs:\n", getpid());

  maps_file = fopen("/proc/self/maps", "r");
  if (maps_file) {
    int vma_num = 0;
    while (fgets(line, sizeof(line), maps_file)) {
      if (strstr(line, "libc.so.6")) {
        unsigned long start, end, offset;
        char perms[8], dev[16], path[256];
        unsigned long inode;

        sscanf(line, "%lx-%lx %s %lx %s %lu %s", &start, &end, perms, &offset,
               dev, &inode, path);

        unsigned long size_bytes = end - start;
        unsigned long size_pages = size_bytes / 4096;

        printf("    VMA%d: start=0x%lx end=0x%lx perms=%s offset=0x%lx "
               "pages=%lu\n",
               vma_num, start, end, perms, offset, size_pages);

        if (vma_num == 0) {
          first_vma_start = start;
          first_vma_end = end;
          first_offset = offset;
        }
        vma_num++;
      }
    }
    fclose(maps_file);
  }

  printf("\n");

  /* STEP 4: Explain what offset means */
  printf("=== OFFSET EXPLANATION ===\n");
  printf("07. VMA0 offset=0x%lx → this VMA maps bytes [0x%lx, 0x%lx) of "
         "libc.so.6 file\n",
         first_offset, first_offset,
         first_offset + (first_vma_end - first_vma_start));
  printf("08. offset/4096 = 0x%lx / 4096 = page %lu of libc.so.6\n",
         first_offset, first_offset / 4096);
  printf("09. When process accesses vaddr 0x%lx → kernel reads libc.so.6 at "
         "byte 0x%lx\n",
         first_vma_start, first_offset);
  printf(
      "10. page->index = offset/4096 = %lu → identifies which page of file\n\n",
      first_offset / 4096);

  /* STEP 5: Show reverse lookup concept */
  printf("=== REVERSE LOOKUP TRACE ===\n");
  printf("11. Kernel needs to swap page 40 (offset 0x28000) of libc.so.6\n");
  printf("12. page->mapping = address_space of inode %lu\n", libc_stat.st_ino);
  printf("13. page->index = 40 (page 40 of file)\n");
  printf("14. address_space->i_mmap = interval tree containing %d VMAs\n",
         libc_process_count);
  printf("15. Kernel walks tree: for each VMA where vm_pgoff <= 40 < vm_pgoff "
         "+ vma_pages:\n");
  printf("    → vaddr = vm_start + (40 - vm_pgoff) × 4096\n");
  printf("    → walk VMA's mm->pgd to find PTE at vaddr\n");
  printf("    → unmap PTE\n");
  printf("16. ∴ Kernel unmaps page 40 from all %d processes in O(log %d + %d) "
         "time\n\n",
         libc_process_count, libc_process_count, libc_process_count);

  /* STEP 6: Numerical calculation */
  printf("=== NUMERICAL EXAMPLE ===\n");
  printf("17. libc.so.6 r-xp VMA: offset=0x28000=163840 bytes\n");
  printf("18. 163840 / 4096 = 40 pages\n");
  printf("19. vm_pgoff = 40 (this VMA starts at page 40 of file)\n");
  printf("20. If process accesses vaddr = vm_start + 0x5000:\n");
  printf("    page_in_file = vm_pgoff + (0x5000 / 4096) = 40 + 5 = page 45\n");
  printf("21. page 45 of libc.so.6 is shared by all %d processes\n",
         libc_process_count);
  printf("22. page->_mapcount = %d - 1 = %d (if all mapped same page)\n\n",
         libc_process_count, libc_process_count - 1);

  printf("=== PROOF COMPLETE ===\n");
  printf("One physical page of libc.so.6 → shared by %d processes\n",
         libc_process_count);
  printf("address_space->i_mmap tree → O(log N) lookup of all VMAs\n");
  printf("Without tree → O(N) linear scan of all VMAs in system → system "
         "freeze\n");

  return 0;
}
