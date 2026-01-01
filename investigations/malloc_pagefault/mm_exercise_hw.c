
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "mm_axiom_hw"
#define MM_AXIOM_IOC_VERIFY_PFN _IOW('a', 1, unsigned long)

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

static long mm_axiom_ioctl(struct file *file, unsigned int cmd,
                           unsigned long arg) {
  unsigned long pfn;

  switch (cmd) {
  case MM_AXIOM_IOC_VERIFY_PFN:
    if (get_user(pfn, (unsigned long __user *)arg))
      return -EFAULT;
    verify_pfn_axiom(pfn);
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
