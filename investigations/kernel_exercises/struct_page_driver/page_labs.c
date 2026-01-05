/*
 * page_labs.c - Struct Page Driver Exercise
 *
 * COMPILE: make
 * LOAD:    sudo insmod page_labs.ko
 * UNLOAD:  sudo rmmod page_labs
 * TRAIL:   dmesg | tail -20
 */

#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Struct Page Axiomatic Labs");

/* ═══════════════════════════════════════════════════════════════════════════
 * BOILERPLATE: INIT/EXIT SKELETON
 * ═══════════════════════════════════════════════════════════════════════════
 */

static int __init page_labs_init(void) {
  struct page *p;
  unsigned long pfn;
  void *vaddr;

  printk(KERN_INFO "PageLabs: Starting axiomatic trace...\n");

  /* ════════════════════════════════════════════════════════════════════════
   * LAB 1: PAGE ALLOCATION & ADDRESS DERIVATION
   *
   * AXIOM: alloc_pages(gfp, order) returns struct page* or NULL.
   * AXIOM: PAGE_SIZE = 4096 (0x1000).
   * ════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 1: Allocate a single page (order 0) using GFP_KERNEL */
  /* TODO BLOCK 1: Allocate a single page (order 0) using GFP_KERNEL */
  /* ACTION (Worksheet Line 15): alloc_page finds free block in Zone */
  p = alloc_page(GFP_KERNEL);

  if (!p) {
    printk(KERN_ERR "PageLabs: Allocation failed! (Did you fill TODO 1?)\n");
    return -ENOMEM;
  }

  /* ════════════════════════════════════════════════════════════════════════
   * LAB 2: PFN DERIVATION
   *
   * AXIOM: PFN = (page_addr - vmemmap_base) / sizeof(struct page)
   * KERNEL MACRO: page_to_pfn(p)
   * ════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 2: Calculate PFN using kernel macro */
  /* TODO BLOCK 2: Calculate PFN using kernel macro */
  /* FORMULA (Worksheet Line 14): (Addr - Base) / 64 */
  pfn = page_to_pfn(p);

  printk(KERN_INFO "PageLabs: Page=%p, PFN=%lu (0x%lx)\n", p, pfn, pfn);

  /* ════════════════════════════════════════════════════════════════════════
   * LAB 3: PHYSICAL ADDRESS CALCULATION
   *
   * FORMULA: Phys = PFN * PAGE_SIZE
   * ════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 3: Print physical address (derive from PFN) */
  /* TODO BLOCK 3: Print physical address (derive from PFN) */
  /* FORMULA (Worksheet Line 18): Phys = PFN * PAGE_SIZE */
  {
    phys_addr_t phys = (phys_addr_t)pfn * PAGE_SIZE;
    printk(KERN_INFO "PageLabs: Phys Addr = 0x%llx\n",
           (unsigned long long)phys);
  }

  /* ════════════════════════════════════════════════════════════════════════
   * LAB 4: REFCOUNT MANIPULATION
   *
   * TRAIT: _refcount starts at 1.
   * ACTION: Increment it (simulate a mapper).
   * ════════════════════════════════════════════════════════════════════════
   */

  /* TODO BLOCK 4: Increment refcount */
  /* TODO BLOCK 4: Increment refcount */
  /* TRACE (Worksheet Line 22): get_page adds 1 -> Refcount becomes 2 */
  get_page(p);
  printk(KERN_INFO "PageLabs: Refcount after get = %d\n", page_ref_count(p));

  /* CLEANUP */
  if (p) {
    /* TODO BLOCK 5: Free the page correctly */
    /* TODO BLOCK 5: Free the page correctly */
    /* TRACE (Worksheet Line 23): put_page drops ref to 1. __free_pages drops
     * ref to 0? */
    /* Correct ownership: We hold 1 (alloc) + 1 (get). */
    /* Put drops to 1. Free drops to 0. */
    put_page(p);
    __free_pages(p, 0);
    printk(KERN_INFO "PageLabs: Page freed.\n");
  }

  return 0;
}

static void __exit page_labs_exit(void) {
  printk(KERN_INFO "PageLabs: Module unloaded.\n");
}

module_init(page_labs_init);
module_exit(page_labs_exit);
