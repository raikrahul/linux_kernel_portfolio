/*01.AXIOM:mmap=syscall_that_maps_file_or_anonymous_memory_into_process_address_space*/
/*02.AXIOM:MAP_SHARED=flag_that_makes_mapping_visible_to_other_processes*/
/*03.AXIOM:fork()=syscall_that_creates_child_process_with_copy_of_parent's_memory*/
/*04.AXIOM:after_fork_MAP_SHARED_regions_are_shared_between_parent_and_child→same_physical_page→refcount=2*/
/*05.AXIOM:munmap=syscall_that_unmaps_memory→triggers_put_page()_in_kernel*/
/*06.PROBLEM:if_parent_and_child_call_munmap_at_same_time→race_on_refcount*/
/*07.GOAL:demonstrate_that_kernel_uses_atomic_ops_so_no_race_occurs*/
#include <fcntl.h> /*provides:open,O_RDWR*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> /*provides:mmap,munmap,PROT_READ,PROT_WRITE,MAP_SHARED,MAP_ANONYMOUS*/
#include <sys/wait.h> /*provides:wait()*/
#include <unistd.h>
#define PAGE_SIZE 4096
int main(void) {
  void *shared;
  pid_t pid;
  printf("RACE_TEST:start:pid=%d\n", getpid());
  /*TODO01:CALL_mmap_to_create_anonymous_shared_mapping:
      - addr=NULL (kernel chooses address)
      - length=PAGE_SIZE (4096 bytes = 1 page)
      - prot=PROT_READ|PROT_WRITE (readable and writable)
      - flags=MAP_SHARED|MAP_ANONYMOUS (shared, no file backing)
      - fd=-1 (no file)
      - offset=0
      - FILL_BELOW:*/
  shared = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (shared == MAP_FAILED) {
    perror("mmap");
    return 1;
  }
  printf("RACE_TEST:mmap_success:addr=%p\n", shared);
  /*TODO02:WRITE_data_to_shared_memory_to_prove_it_works:*/
  /* TODO: WRITE memset or strcpy HERE */
  strcpy(shared, "HELLO");
  printf("RACE_TEST:data_written\n");
  /*TODO03:CALL_fork_to_create_child_process:
      - after_fork:parent_gets_child_pid,child_gets_0
      - both_processes_now_share_the_same_physical_page
      - refcount_of_that_page_is_now_2_(parent+child) // how can you say that
  that is cow
      - FILL_BELOW:* what about the pte and pml4 and other level directories
    */
  pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    /*CHILD_PROCESS:*/
    printf("CHILD:pid=%d:reading_shared=%s\n", getpid(), (char *)shared);
    /*TODO04:CHILD_CALLS_munmap:this_triggers_put_page()_in_kernel→refcount:2→1*/
    /* TODO: CALL munmap(shared, PAGE_SIZE) HERE */
    sleep(30); munmap(shared, PAGE_SIZE);
    printf("CHILD:munmap_done:exiting\n");
    exit(0);
  } else {
    /*PARENT_PROCESS:*/
    printf("PARENT:pid=%d:child_pid=%d\n", getpid(), pid);
    /*TODO05:PARENT_ALSO_CALLS_munmap_almost_at_same_time:this_triggers_put_page()→refcount:1→0→page_freed*/
    /* TODO: CALL munmap(shared, PAGE_SIZE) HERE */

    sleep(30); munmap(shared, PAGE_SIZE);
    printf("PARENT:munmap_done\n");
    /*TODO06:WAIT_for_child_to_finish:*/
    /* TODO: CALL wait(NULL) HERE */
    wait(NULL);
    printf("PARENT:child_exited:done\n");
  }
  return 0;
}
/*EXPECTED_OUTPUT:
  RACE_TEST:start:pid=12345
  RACE_TEST:mmap_success:addr=0x7f1234567000
  RACE_TEST:data_written
  PARENT:pid=12345:child_pid=12346
  CHILD:pid=12346:reading_shared=HELLO
  PARENT:munmap_done
  CHILD:munmap_done:exiting
  PARENT:child_exited:done
*/
/*OBSERVATION:no_crash→kernel_atomic_ops_work_correctly→refcount_race_avoided*/
/*FAILURE_PREDICTION:F1:if_you_forget_MAP_SHARED→child_gets_COPY_not_SHARED→refcount_stays_1_each*/
/*FAILURE_PREDICTION:F2:if_you_forget_MAP_ANONYMOUS→mmap_fails_because_fd=-1_is_invalid_for_file_mapping*/
