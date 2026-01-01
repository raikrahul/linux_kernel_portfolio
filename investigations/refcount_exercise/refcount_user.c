/*D01:AXIOM:Userspace_cannot_directly_access_struct_page→D02:AXIOM:Userspace_can_observe_effects_via_/proc/meminfo→D03:AXIOM:MemFree_changes_when_kernel_allocates/frees_pages→D04:PROBLEM:How_to_verify_kernel_module_actually_allocates_a_page?→D05:SOLUTION:Read_/proc/meminfo_before_and_after_insmod*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/*TODO01:DERIVE:PAGE_SIZE=4096→1_page=4096_bytes=4KB→CALCULATE:MemFree_drop_after_alloc_page=4KB*/
/*TODO02:DERIVE:1024_pages=1024*4096=4194304_bytes=4MB→CALCULATE:MemFree_drop_after_alloc_pages(10)=1024*4KB=4MB*/
long read_memfree_kb(void) {
  FILE *f = fopen("/proc/meminfo", "r");
  if (!f) {
    perror("fopen");
    return -1;
  }
  char line[256];
  long memfree_kb = -1;
  /*TODO03:PARSE:/proc/meminfo→line_format="MemFree:_____12345_kB"→extract_number→FILL:sscanf_format*/
  while (fgets(line, sizeof(line), f)) {
    if (strncmp(line, "MemFree:", 8) == 0) {
      sscanf(line, "MemFree: %ld kB", &memfree_kb);
      break;
    }
  }
  fclose(f);
  return memfree_kb;
}
int main(void) {
  long before_kb, after_insmod_kb, after_rmmod_kb;
  printf("REFCOUNT_USER:start\n");
  /*TODO04:READ:MemFree_before_insmod→STORE:before_kb*/
  before_kb = read_memfree_kb();
  printf("REFCOUNT_USER:MemFree_before=%ld_kB\n", before_kb);
  printf("REFCOUNT_USER:RUN_IN_ANOTHER_TERMINAL:sudo_insmod_refcount_hw.ko\n");
  printf("REFCOUNT_USER:Press_Enter_after_insmod...\n");
  getchar();
  /*TODO05:READ:MemFree_after_insmod→STORE:after_insmod_kb→CALCULATE:delta=before-after*/
  after_insmod_kb = read_memfree_kb();
  printf("REFCOUNT_USER:MemFree_after_insmod=%ld_kB\n", after_insmod_kb);
  /*TODO06:CALCULATE:delta=before_kb-after_insmod_kb→EXPECTED:delta>=4(one_page=4KB)→VERIFY:delta>=4?*/
  long delta_kb = before_kb - after_insmod_kb;
  printf("REFCOUNT_USER:delta=%ld_kB\n", delta_kb);
  /*TODO07:ANALYZE:If_delta<4→module_freed_page_before_exit→refcount_reached_0→__free_page_called*/
  /*TODO08:ANALYZE:If_delta>=4→module_did_NOT_free_page→BUG:refcount_leak→memory_leak*/
  printf("REFCOUNT_USER:CHECK:delta>=4?→%s\n",
         delta_kb >= 4 ? "PAGE_LEAKED_BUG" : "PAGE_FREED_OK");
  printf("REFCOUNT_USER:RUN:sudo_rmmod_refcount_hw\n");
  printf("REFCOUNT_USER:Press_Enter_after_rmmod...\n");
  getchar();
  /*TODO09:READ:MemFree_after_rmmod→VERIFY:should_return_to_before_kb*/
  after_rmmod_kb = read_memfree_kb();
  printf("REFCOUNT_USER:MemFree_after_rmmod=%ld_kB\n", after_rmmod_kb);
  printf("REFCOUNT_USER:FINAL:before=%ld,after_insmod=%ld,after_rmmod=%ld\n",
         before_kb, after_insmod_kb, after_rmmod_kb);
  return 0;
}
/*VIOLATION_CHECK:NEW_THINGS_INTRODUCED_WITHOUT_DERIVATION:*/
/*V1:/proc/meminfo→derived_in_D02*/
/*V2:MemFree→derived_in_D03*/
/*V3:sscanf→standard_C_library→assumed_known*/
/*USER_MUST_VERIFY:cat_/proc/meminfo|grep_MemFree*/
