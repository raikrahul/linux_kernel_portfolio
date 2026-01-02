
#include <asm/pgtable.h> // AXIOM: For PTRS_PER_PTE and other pagetable constants
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "mm_axiom_hw"
#define MM_AXIOM_IOC_VERIFY_PFN _IOW('a', 1, unsigned long)

struct axiom_walk_args {
  int pid;
  unsigned long va;
};
#define MM_AXIOM_IOC_WALK_VA _IOW('a', 2, struct axiom_walk_args)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axiomatic Agent");
MODULE_DESCRIPTION("Kernel MM Axiom Verifier");

// AXIOM VERIFIER FUNCTION
static void verify_pfn_axiom(unsigned long pfn) {
  struct page *page;
  unsigned long phys_addr;
  void *virt_addr = NULL;

  pr_info("AXIOM: Verifying PFN 0x%lx\n", pfn);

  // 1. VALIDITY CHECK
  if (!pfn_valid(pfn)) {
    pr_err("AXIOM: PFN 0x%lx is INVALID (Hole?)\n", pfn);
    return;
  }

  // 2. CONVERSION (The macro we are studying)
  page = pfn_to_page(pfn);
  pr_info("AXIOM: pfn_to_page(0x%lx) = %p (Struct Page VA)\n", pfn, page);

  // 3. PHYSICAL ADDRESS
  phys_addr = PFN_PHYS(pfn);
  pr_info("AXIOM: PFN_PHYS(0x%lx) = 0x%lx\n", pfn, phys_addr);

  // 4. KERNEL DIRECT MAPPING (Logical Address)
  // Only valid if within lowmem or x86_64 direct map range
  virt_addr = __va(phys_addr);
  pr_info("AXIOM: __va(0x%lx) = %p\n", phys_addr, virt_addr);

  // 5. INSPECT FIELDS (Raw values for user derivation)
  pr_info("AXIOM INSPECT: Flags = 0x%lx\n", page->flags);
  pr_info("AXIOM INSPECT: Refcount = %d\n", page_ref_count(page));
  pr_info("AXIOM INSPECT: Mapcount = %d\n", folio_mapcount(page_folio(page)));

  // 6. COMPOUND PAGE CHECK (If flags indicate head/tail)
  if (PageCompound(page)) {
    pr_info("AXIOM: Page is Compound.\n");
    if (PageHead(page))
      pr_info("AXIOM: It is a HEAD page. Order: %u\n", compound_order(page));
    else
      pr_info("AXIOM: It is a TAIL page.\n");
  }
}

// AXIOM: PAGE TABLE WALKER
// Why: To prove 0xae (PGD Index) is correct.
// How: Mimic CPU behavior in software.
// AXIOM: PAGE TABLE WALKER
// Why: To prove 0xae (PGD Index) is correct.
// How: Mimic CPU behavior in software.
static void trace_page_table_walk(int pid, unsigned long va) {
  struct task_struct *task;
  struct mm_struct *mm;
  pgd_t *pgd;
  p4d_t *p4d;
  pud_t *pud;
  pmd_t *pmd;
  pte_t *pte;

  // Step 1: Find the process
  /* AXIOM PROOF STEP 2: The Handler Accesses 'current' */
  if (pid == current->pid) {
    task = current;
    get_task_struct(task); // AXIOM: Increment refcount so we can put it later
  } else {
    task = get_pid_task(find_vpid(pid), PIDTYPE_PID);
  }

  if (!task) {
    pr_err("AXIOM ERROR: Task not found for PID %d\n", pid);
    return;
  }

  pr_info("================ [ AXIOM STEP 2: GET MM ] ================\n");
  pr_info("AXIOM: CPU executing this code is running Task @ 0x%px\n", task);
  pr_info("AXIOM: Task Name: %s | PID: %d\n", task->comm, task->pid);

  /* AXIOM: The task has an mm_struct */
  mm = task->mm;
  if (!mm) {
    pr_err("AXIOM ERROR: Task has no mm (Kernel thread?)\n");
    put_task_struct(task); // AXIOM: Release task ref
    return;
  }

  mmget(mm); // AXIOM: Pin mm to prevent destruction
  pr_info("AXIOM: current->mm (Memory Descriptor) = 0x%px\n", mm);
  pr_info("AXIOM: CR3 (PGD Physical Base) should be in mm->pgd\n");

  mmap_read_lock(mm); // Lock against changes

  pr_info("================ [ AXIOM WALK: 0x%lx ] ================\n", va);

  // AXIOM: Use correct macros for indices
  // Note: pte_index() is not always exported. We derive it using symbols.
  unsigned long pgd_idx = pgd_index(va);
  unsigned long pud_idx = (va >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
  unsigned long pmd_idx = (va >> PMD_SHIFT) & (PTRS_PER_PMD - 1);
  unsigned long pte_idx = (va >> PAGE_SHIFT) & (PTRS_PER_PTE - 1);

  pr_info("MATH: PGD=%lu PUD=%lu PMD=%lu PTE=%lu\n", pgd_idx, pud_idx, pmd_idx,
          pte_idx);

  // LEVEL 1: PGD (Top)
  pgd = pgd_offset(mm, va);
  pr_info("L1 PGD: Index = %lu | Val = 0x%lx\n", pgd_idx, pgd_val(*pgd));

  if (pgd_none(*pgd) || pgd_bad(*pgd)) {
    pr_info("L1 PGD: Terminated (None or Bad)\n");
    goto out;
  }

  // LEVEL 2: PUD
  p4d = p4d_offset(pgd, va); // On 4-level x86, p4d folds to pgd
  pud = pud_offset(p4d, va);
  pr_info("L2 PUD: Index = %lu | Val = 0x%lx\n", pud_idx, pud_val(*pud));

  if (pud_none(*pud) || pud_bad(*pud)) {
    pr_info("L2 PUD: Terminated (None or Bad)\n");
    goto out;
  }

  if (pud_leaf(*pud)) {
    pr_info("L2 PUD: 1GB Huge Page Detected. Stopping Walk.\n");
    goto out;
  }

  // LEVEL 3: PMD
  pmd = pmd_offset(pud, va);
  pr_info("L3 PMD: Index = %lu | Val = 0x%lx\n", pmd_idx, pmd_val(*pmd));

  if (pmd_none(*pmd) || pmd_bad(*pmd)) {
    pr_info("L3 PMD: Terminated (None or Bad)\n");
    goto out;
  }

  if (pmd_leaf(*pmd)) {
    pr_info("L3 PMD: 2MB Huge Page Detected. Stopping Walk.\n");
    // Could decode 2MB PFN here if needed
    goto out;
  }

  // LEVEL 4: PTE
  pte = pte_offset_kernel(pmd, va); // Use _kernel since we are in kernel
  if (!pte) {
    pr_info("L4 PTE: Failed to map\n");
    goto out;
  }

  // AXIOM: Use macros for PFN extraction
  unsigned long pte_val_raw = pte_val(*pte);
  unsigned long pfn = pte_pfn(*pte);
  unsigned long va_offset = va & ~PAGE_MASK;

  // AXIOM: SAFETY CHECK - Only calculate Phys if PFN is valid
  // Note: pfn_valid just checks memory map existance, not if it is RAM vs MMIO
  if (!pfn_valid(pfn)) {
    pr_info("L4 PTE: Index = %lu | Val = 0x%lx | PFN = 0x%lx (INVALID/MMIO)\n",
            pte_idx, pte_val_raw, pfn);
    goto out;
  }

  unsigned long phys_addr = (pfn << PAGE_SHIFT) | va_offset;

  pr_info("L4 PTE: Index = %lu | Val = 0x%lx\n", pte_idx, pte_val_raw);
  pr_info("L4 PTE: PFN = 0x%lx | Offset = 0x%lx | Phys = 0x%lx\n", pfn,
          va_offset, phys_addr);

  // READ ACTUAL RAM CONTENT (first 16 bytes)
  // AXIOM: Only read if we assume it is linear RAM.
  // Safety: Check if it's within direct map range implicitly via __va check or
  // similar? We will assume standard RAM for this exercise but add a comment.
  {
    void *kva = __va(phys_addr);

    // Safety check: Is this address readable?
    // In kernel, we trust ourselves, but bad PFN could crash.
    // We already checked pfn_valid.

    if (virt_addr_valid(kva)) {
      char buf[17];
      memcpy(buf, kva, 16);
      buf[16] = '\0';
      pr_info("RAM CONTENT (first 16 bytes): \"%s\"\n", buf);
      pr_info("RAM HEX: %02x %02x %02x %02x\n", ((unsigned char *)kva)[0],
              ((unsigned char *)kva)[1], ((unsigned char *)kva)[2],
              ((unsigned char *)kva)[3]);
    } else {
      pr_warn(
          "AXIOM WARNING: Address %p is not valid linear map. Skipping read.\n",
          kva);
    }
  }

out:
  pr_info("=========================================================\n");
  mmap_read_unlock(mm);
  mmput(mm);
  put_task_struct(task);
}

static long mm_axiom_ioctl(struct file *file, unsigned int cmd,
                           unsigned long arg) {
  unsigned long pfn;
  struct axiom_walk_args args;

  switch (cmd) {
  case MM_AXIOM_IOC_VERIFY_PFN:
    if (get_user(pfn, (unsigned long __user *)arg))
      return -EFAULT;
    verify_pfn_axiom(pfn);
    break;

  case MM_AXIOM_IOC_WALK_VA:
    if (copy_from_user(&args, (void __user *)arg, sizeof(args)))
      return -EFAULT;
    trace_page_table_walk(args.pid, args.va);
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static const struct file_operations mm_axiom_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = mm_axiom_ioctl,
};

static struct miscdevice mm_axiom_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DRIVER_NAME,
    .fops = &mm_axiom_fops,
};

static int __init mm_axiom_init(void) {
  int ret;
  pr_info("AXIOM: Module Loading...\n");
  ret = misc_register(&mm_axiom_misc);
  if (ret)
    pr_err("AXIOM: Misc Register Failed\n");
  return ret;
}

static void __exit mm_axiom_exit(void) {
  misc_deregister(&mm_axiom_misc);
  pr_info("AXIOM: Module Unloaded.\n");
}

module_init(mm_axiom_init);
module_exit(mm_axiom_exit);
