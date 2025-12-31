/* compound_lab.c - Module 4: Compound pages (head/tail pointer math)
 * COMPILE: make
 * LOAD: sudo insmod compound_lab.ko
 * VERIFY: sudo dmesg | tail -40
 * UNLOAD: sudo rmmod compound_lab
 */
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Struct Page Compound Pages Lab");

static int __init compound_lab_init(void) {
  struct page *head, *tail1, *tail2;
  unsigned long head_pfn, tail1_pfn;
  int order = 2; /* 2^2 = 4 pages */

  printk(KERN_INFO "CompoundLab: === MODULE 4: COMPOUND PAGES ===\n");

  /* D01. DEFINE: Compound page = 2^order contiguous pages allocated together
   * with __GFP_COMP */
  /* D02. DEFINE: Head page = first struct page, has PG_head flag set */
  /* D03. DEFINE: Tail page = remaining struct pages, has compound_head pointing
   * to head|1 */
  /* D04. AXIOM: For order=2, we get 4 pages: 1 head + 3 tails */
  /* D05. FORMULA: tail->compound_head = head_address | 1 (low bit set as
   * marker) */

  /* TODO BLOCK 1: Allocate compound page order=2 */
  /* ACTION: alloc_pages(GFP_KERNEL | __GFP_COMP, order) → Returns head page */
  /* DRAW: buddy_order2→unlink→4 pages→prep_compound_page→set head/tail→return
   * head */
  head = alloc_pages(GFP_KERNEL | __GFP_COMP, order);
  if (!head) {
    printk(KERN_ERR "CompoundLab: alloc_pages failed\n");
    return -ENOMEM;
  }

  head_pfn = page_to_pfn(head);
  printk(KERN_INFO "CompoundLab: Head page=%px PFN=0x%lx\n", head, head_pfn);

  /* TODO BLOCK 2: Access tail pages via pointer arithmetic */
  /* FORMULA: tail[N] = head + N (pointer arithmetic on struct page*) */
  /* OFFSET: Each struct page is 64 bytes → head+1 is 64 bytes after head */
  tail1 = head + 1; /* First tail page */
  tail2 = head + 2; /* Second tail page */
  tail1_pfn = page_to_pfn(tail1);
  printk(KERN_INFO "CompoundLab: Tail1 page=%px PFN=0x%lx (head_pfn+1=0x%lx)\n",
         tail1, tail1_pfn, head_pfn + 1);

  /* TODO BLOCK 3: Verify head/tail flags */
  /* FORMULA: PageHead(head) checks PG_head flag → should be true */
  /* FORMULA: PageTail(tail1) checks compound_head low bit → should be true */
  printk(KERN_INFO "CompoundLab: PageHead(head)=%d PageTail(tail1)=%d "
                   "PageCompound(head)=%d\n",
         PageHead(head) ? 1 : 0, PageTail(tail1) ? 1 : 0,
         PageCompound(head) ? 1 : 0);
  /* EXPECT: 1, 1, 1 */

  /* TODO BLOCK 4: Trace compound_head pointer in tail */
  /* FORMULA: tail->compound_head = head | 1 */
  /* CALCULATION: If head=0xFFFFEA0000100000, then
   * tail->compound_head=0xFFFFEA0000100001 */
  printk(KERN_INFO
         "CompoundLab: tail1->compound_head=0x%lx (expect head|1=0x%lx)\n",
         tail1->compound_head, (unsigned long)head | 1);

  /* TODO BLOCK 5: Get head from tail using compound_head() */
  /* FORMULA: compound_head(tail) = (struct page*)(tail->compound_head & ~1) */
  {
    struct page *derived_head = compound_head(tail1);
    printk(KERN_INFO
           "CompoundLab: compound_head(tail1)=%px (expect head=%px)\n",
           derived_head, head);
    /* VERIFY: derived_head == head */
  }

  /* TODO BLOCK 6: Check compound order stored in first tail */
  /* FORMULA: compound_order(head) reads head[1].compound_order */
  /* OFFSET: head[1] = first tail, compound_order at offset 8+1=9? Actually it's
   * in a union */
  printk(KERN_INFO "CompoundLab: compound_order(head)=%d (expect %d)\n",
         compound_order(head), order);

  /* CLEANUP: Free entire compound page via head with correct order */
  __free_pages(head, order);
  printk(KERN_INFO "CompoundLab: Compound page freed. Module loaded.\n");

  return 0;
}

static void __exit compound_lab_exit(void) {
  printk(KERN_INFO "CompoundLab: Module unloaded.\n");
}

module_init(compound_lab_init);
module_exit(compound_lab_exit);
