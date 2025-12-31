/* refcount_lab.c - Module 3: _refcount manipulation and bug injection
 * COMPILE: make
 * LOAD: sudo insmod refcount_lab.ko
 * VERIFY: sudo dmesg | tail -40
 * UNLOAD: sudo rmmod refcount_lab
 * BUG: This module contains INTENTIONAL bug for user to find and fix
 */
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Struct Page Refcount Lab with Bug");

static int __init refcount_lab_init(void) {
  struct page *p;
  int ref_before, ref_after_get, ref_after_put1, ref_after_put2;

  printk(KERN_INFO "RefcountLab: === MODULE 3: _REFCOUNT MANIPULATION ===\n");

  /* D01. DEFINE: _refcount is atomic_t (4 bytes) at offset 52 (0x34) in struct
   * page */
  /* D02. DEFINE: alloc_page sets _refcount = 1 (kernel owns the page) */
  /* D03. DEFINE: get_page(p) does atomic_inc(&p->_refcount) → refcount += 1 */
  /* D04. DEFINE: put_page(p) does atomic_dec_and_test(&p->_refcount) → if
   * result=0, free page */
  /* D05. DEFINITION FREE CONDITION: Page freed when refcount transitions from 1
   * to 0 */

  /* TODO BLOCK 1: Allocate page */
  /* ACTION: alloc_page(GFP_KERNEL) → returns page with _refcount=1 */
  /* DRAW: buddy→unlink→page→set_page_count(p,1)→return */
  p = alloc_page(GFP_KERNEL);
  if (!p) {
    printk(KERN_ERR "RefcountLab: alloc_page failed\n");
    return -ENOMEM;
  }

  /* TODO BLOCK 2: Read initial refcount */
  /* FORMULA: page_ref_count(p) = atomic_read(&p->_refcount) */
  /* OFFSET: p + 52 bytes = _refcount location */
  ref_before = page_ref_count(p);
  printk(KERN_INFO "RefcountLab: After alloc: refcount=%d (Expected: 1)\n",
         ref_before);

  /* TODO BLOCK 3: Increment refcount */
  /* ACTION: get_page(p) → atomic_inc → refcount: 1 → 2 */
  get_page(p);
  ref_after_get = page_ref_count(p);
  printk(KERN_INFO "RefcountLab: After get_page: refcount=%d (Expected: 2)\n",
         ref_after_get);

  /* TODO BLOCK 4: First put_page */
  /* ACTION: put_page(p) → atomic_dec_and_test → refcount: 2 → 1 → returns false
   * (not zero) */
  put_page(p);
  ref_after_put1 = page_ref_count(p);
  printk(KERN_INFO
         "RefcountLab: After first put_page: refcount=%d (Expected: 1)\n",
         ref_after_put1);

  /* TODO BLOCK 5: Second put_page (THIS IS THE BUG INJECTION) */
  /* ACTION: put_page(p) → atomic_dec_and_test → refcount: 1 → 0 → returns true
   * → PAGE FREED */
  /* BUG: After this, page is freed. Any further access is Use-After-Free */
  put_page(p);
  /* WARNING: p is now freed. Reading refcount here is DANGEROUS */
  /* The following line may show -1 or garbage or crash */
  ref_after_put2 = page_ref_count(p); /* DANGER: Reading freed page! */
  printk(KERN_INFO "RefcountLab: After second put_page: refcount=%d (UAF if "
                   "not 0 or crash)\n",
         ref_after_put2);

  /* TODO BLOCK 6: BUGGY FREE ATTEMPT */
  /* BUG: Calling __free_pages on already freed page → DOUBLE FREE → Kernel BUG
   * or corruption */
  /* USER TASK: Comment out this line to fix the bug */
  /* __free_pages(p, 0); */ /* ← UNCOMMENT THIS LINE TO TRIGGER BUG_ON */

  printk(KERN_INFO "RefcountLab: Module loaded. Check for bugs above.\n");
  printk(KERN_INFO "RefcountLab: TRACE: alloc(1)→get(2)→put(1)→put(0=freed)\n");

  return 0;
}

static void __exit refcount_lab_exit(void) {
  printk(KERN_INFO "RefcountLab: Module unloaded.\n");
}

module_init(refcount_lab_init);
module_exit(refcount_lab_exit);
