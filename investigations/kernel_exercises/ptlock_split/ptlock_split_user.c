#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * 01. USERSPACE TRIGGER
 * ---------------------
 * This program just logs the start and end.
 * The real work is in the kernel, but we might want to interact later.
 * For now, simple boilerplate.
 */

int main() {
  printf("Ptlock Split User Program\n");
  printf("1. Load the kernel module: sudo insmod ptlock_split_hw.ko\n");
  printf("2. Check dmesg for the lock addresses: dmesg | tail\n");
  printf("3. Unload: sudo rmmod ptlock_split_hw\n");
  return 0;
}
