/*
 * handle_pte_fault_probe.c
 *
 * AXIOM: handle_pte_fault receives a pointer to struct vm_fault.
 * AXIOM: struct vm_fault is defined in include/linux/mm.h:560-604.
 * AXIOM: vmf->address = faulting virtual address (PAGE_MASK applied).
 * AXIOM: vmf->pmd = pointer to PMD entry for this address.
 *
 * CALL PATH: do_user_addr_fault → handle_mm_fault → __handle_mm_fault →
 * handle_pte_fault
 *
 * REGISTER CONVENTION (x86_64):
 *   RDI = 1st argument = vmf (pointer to struct vm_fault)
 *   struct vm_fault lives on the STACK of __handle_mm_fault.
 *
 * YOUR TASK: Extract fields from vmf and print them.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("user");
MODULE_DESCRIPTION("Probe handle_pte_fault to trace PTE-level fault handling");

static int target_pid = 0;
module_param(target_pid, int, 0644);

static unsigned long target_addr = 0;
module_param(target_addr, ulong, 0644);

/*
 * STRUCT VM_FAULT OFFSET MAP (FROM include/linux/mm.h:560-604):
 *
 * OFFSET  | FIELD          | SIZE | TYPE
 * --------|----------------|------|------------------
 * 0       | vma            | 8    | struct vm_area_struct *
 * 8       | gfp_mask       | 4    | gfp_t (unsigned int)
 * 16      | pgoff          | 8    | pgoff_t (aligned)
 * 24      | address        | 8    | unsigned long
 * 32      | real_address   | 8    | unsigned long
 * 40      | flags          | 4    | enum fault_flag
 * 48      | pmd            | 8    | pmd_t * (aligned)
 * 56      | pud            | 8    | pud_t *
 * 64      | orig_pte       | 8    | pte_t (union)
 * 72      | cow_page       | 8    | struct page *
 * 80      | page           | 8    | struct page *
 * 88      | pte            | 8    | pte_t *
 * 96      | ptl            | 8    | spinlock_t *
 * 104     | prealloc_pte   | 8    | pgtable_t
 *
 * DERIVATION:
 *   address is at offset 24.
 *   vmf_ptr + 24 = address of vmf->address field.
 *   *(unsigned long *)(vmf_ptr + 24) = value of vmf->address.
 */

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
  /*
   * AXIOM: regs->di = RDI = first argument = vmf pointer.
   */
  struct vm_fault *vmf = (struct vm_fault *)regs->di;

  int pid = current->pid;

  /* TODO 1: FILTER BY PID
   *
   * TASK: If pid != target_pid, return 0 immediately.
   *
   * WHY: We only want to trace our specific userspace program.
   *      Other processes also trigger page faults (e.g., bash, systemd).
   *      Without this filter, dmesg will be flooded.
   *
   * FILL IN:
   *   if (______ != ______) {
   *       return 0;
   *   }
   */
  if (pid != target_pid) {
    return 0;
  }

  /* TODO 2: EXTRACT vmf->address
   *
   * AXIOM: vmf->address = the faulting virtual address with PAGE_MASK
   * applied. AXIOM: Offset of address in struct vm_fault = 24 bytes.
   *
   * TASK: Read vmf->address and store it in a local variable.
   *
   * FILL IN:
   *   unsigned long fault_addr = vmf->______;
   */
  unsigned long fault_addr = vmf->address; /* REPLACE THIS LINE */

  /* TODO 3: FILTER BY ADDRESS PAGE
   *
   * AXIOM: PAGE_MASK = 0xFFFFFFFFFFFFF000 (for 4K pages).
   * AXIOM: Two addresses are on the same page if (addr1 & PAGE_MASK) ==
   * (addr2 & PAGE_MASK).
   *
   * TASK: If fault_addr is not on the same page as target_addr, return 0.
   *
   * FILL IN:
   *   if ((fault_addr & ______) != (target_addr & ______)) {
   *       return 0;
   *   }
   */
  if (target_addr != 0 &&
      (fault_addr & PAGE_MASK) != (target_addr & PAGE_MASK)) {
    return 0;
  }

  /* TODO 4: EXTRACT vmf->pmd
   *
   * AXIOM: vmf->pmd = pointer to the PMD entry for this address.
   * AXIOM: PMD entry is 8 bytes on x86_64.
   * AXIOM: If pmd_none(*pmd), then no PTE table exists yet.
   *
   * TASK: Read vmf->pmd and dereference it to get the PMD value.
   *
   * FILL IN:
   *   pmd_t *pmd_ptr = vmf->______;
   *   pmd_t pmd_val = *______;
   */

  /* TODO 5: PRINT TRACE
   *
   * TASK: Print the extracted values using pr_info.
   *
   * FILL IN: Complete the pr_info statements below.
   */
  pmd_t *pmd_ptr = vmf->pmd;
  pmd_t pmd_entry = *pmd_ptr;

  pr_info("AXIOM_TRACE: handle_pte_fault HIT\n");
  pr_info("   COMM: %s | PID: %d\n", current->comm, pid);
  pr_info("   VMF_PTR: 0x%px\n", vmf);
  pr_info("   VMF->address: 0x%lx\n", vmf->address);
  pr_info("   VMF->pmd: 0x%px\n", vmf->pmd);
  pr_info("   PMD_VAL: 0x%lx\n", pmd_val(pmd_entry));
  pr_info("   VMF->real_address: 0x%lx\n", vmf->real_address);

  return 0;
}

static struct kprobe kp = {
    .symbol_name = "handle_pte_fault",
    .pre_handler = handler_pre,
};

static int __init probe_init(void) {
  int ret;
  ret = register_kprobe(&kp);
  if (ret < 0) {
    pr_err("register_kprobe failed: %d\n", ret);
    return ret;
  }
  pr_info("Probe planted: handle_pte_fault at 0x%px\n", kp.addr);
  return 0;
}

static void __exit probe_exit(void) {
  unregister_kprobe(&kp);
  pr_info("Probe removed: handle_pte_fault\n");
}

module_init(probe_init);
module_exit(probe_exit);
