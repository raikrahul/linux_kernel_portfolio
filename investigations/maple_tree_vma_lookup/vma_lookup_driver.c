/*
 * vma_lookup_driver.c
 * PURPOSE: Walk the Maple Tree of a target process and list all VMAs.
 *          Verify that a specific VA is present in the tree.
 *
 * USAGE:
 *   sudo insmod vma_lookup_driver.ko target_pid=12345
 * target_addr=0x7f1234567000 sudo dmesg | tail -50 sudo rmmod vma_lookup_driver
 *
 * AXIOM: Each process has mm_struct with mm_mt (Maple Tree).
 * AXIOM: Maple Tree stores VMAs as (pivot=vm_end-1, slot=vma_pointer).
 * AXIOM: VMA iteration: for_each_vma(vmi, vma) walks all VMAs.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antigravity");
MODULE_DESCRIPTION("Walk Maple Tree of target process, list all VMAs");

static int target_pid = 0;
module_param(target_pid, int, 0644);
MODULE_PARM_DESC(target_pid, "PID of target process");

static unsigned long target_addr = 0;
module_param(target_addr, ulong, 0644);
MODULE_PARM_DESC(target_addr, "VA to search for in Maple Tree");

static int __init vma_lookup_init(void) {
  struct task_struct *task;
  struct mm_struct *mm;
  struct vm_area_struct *vma;
  struct vma_iterator vmi;
  int vma_count = 0;
  int found = 0;

  pr_info("=== VMA LOOKUP DRIVER LOADED ===\n");
  pr_info("TARGET_PID:  %d\n", target_pid);
  pr_info("TARGET_ADDR: 0x%lx\n", target_addr);

  if (target_pid == 0) {
    pr_err("ERROR: target_pid not specified\n");
    return -EINVAL;
  }

  /*
   * AXIOM: find_get_pid(nr) returns struct pid* for given PID number.
   * AXIOM: pid_task(pid, PIDTYPE_PID) returns task_struct* from pid.
   * AXIOM: task->mm is the mm_struct (NULL for kernel threads).
   */
  rcu_read_lock();
  task = pid_task(find_get_pid(target_pid), PIDTYPE_PID);
  if (!task) {
    pr_err("ERROR: PID %d not found\n", target_pid);
    rcu_read_unlock();
    return -ESRCH;
  }

  mm = task->mm;
  if (!mm) {
    pr_err("ERROR: PID %d has no mm_struct (kernel thread?)\n", target_pid);
    rcu_read_unlock();
    return -EINVAL;
  }

  /*
   * AXIOM: mmget(mm) increments mm->mm_count to prevent freeing.
   * AXIOM: Must call mmap_read_lock(mm) before walking VMAs.
   */
  mmget(mm);
  rcu_read_unlock();

  mmap_read_lock(mm);

  pr_info("=== MAPLE TREE VMA LIST (PID %d) ===\n", target_pid);
  pr_info("| IDX | vm_start           | vm_end             | SIZE     | FLAGS  "
          "  |\n");
  pr_info("|-----|--------------------|--------------------|----------|--------"
          "--|\n");

  /*
   * AXIOM: vma_iter_init(&vmi, mm, 0) initializes iterator at address 0.
   * AXIOM: for_each_vma(vmi, vma) iterates all VMAs in sorted order.
   * DERIVATION: This proves Maple Tree stores VMAs sorted by vm_start.
   */
  vma_iter_init(&vmi, mm, 0);
  for_each_vma(vmi, vma) {
    unsigned long size = vma->vm_end - vma->vm_start;
    int match = 0;

    /* Check if target_addr is within this VMA */
    if (target_addr >= vma->vm_start && target_addr < vma->vm_end) {
      match = 1;
      found = 1;
    }

    pr_info("| %3d | 0x%016lx | 0x%016lx | %8lu | 0x%06lx | %s\n", vma_count,
            vma->vm_start, vma->vm_end, size, vma->vm_flags,
            match ? "<-- MATCH" : "");

    vma_count++;
  }

  mmap_read_unlock(mm);
  mmput(mm);

  pr_info("=== TOTAL VMAs: %d ===\n", vma_count);

  if (target_addr != 0) {
    if (found) {
      pr_info("RESULT: 0x%lx FOUND in Maple Tree ✓\n", target_addr);
    } else {
      pr_info("RESULT: 0x%lx NOT FOUND in Maple Tree ✗\n", target_addr);
    }
  }

  pr_info("=== VMA LOOKUP COMPLETE ===\n");

  return 0;
}

static void __exit vma_lookup_exit(void) {
  pr_info("VMA LOOKUP DRIVER UNLOADED\n");
}

module_init(vma_lookup_init);
module_exit(vma_lookup_exit);
