/* metadata_union_hw.c - HOMEWORK: YOU WRITE THE CODE
 *
 * BOILERPLATE PROVIDED. TODO BLOCKS = YOUR CODE.
 * Each TODO has axioms and numbers. You derive and write.
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
MODULE_DESCRIPTION("Metadata union - YOU write the code");

static int __init metadata_union_hw_init(void) {
  struct page *normal_page;
  struct page *slab_page;
  void *slab_obj;
  unsigned long pfn_normal, pfn_slab;
  unsigned long offset_8_15_normal, offset_16_23_normal;
  unsigned long offset_8_15_slab, offset_16_23_slab;
  int pgslab_normal, pgslab_slab;

  printk(KERN_INFO "MetaUnionHW: === YOU WRITE THE CODE ===\n\n");

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 1: ALLOCATE NORMAL PAGE
   *
   * AXIOM: alloc_page(gfp_mask) returns struct page* or NULL.
   * AXIOM: GFP_KERNEL = 0xCC0 = allocation for kernel, can sleep.
   * AXIOM: struct page is 64 bytes at vmemmap[PFN].
   *
   * YOUR DATA AFTER YOU RUN:
   * normal_page will be something like 0xfffff7da455024c0
   * PFN will be something like 0x154093 = 1,392,787
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  normal_page =
      /* TODO: Call alloc_page with GFP_KERNEL */ alloc_page(GFP_KERNEL);

  if (!normal_page) {
    printk(KERN_ERR "MetaUnionHW: alloc_page failed\n");
    return -ENOMEM; /* -ENOMEM = -12 */
  }

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 2: GET PFN FROM STRUCT PAGE
   *
   * AXIOM: page_to_pfn(p) = (p - vmemmap_base) / sizeof()
   * AXIOM: vmemmap_base = 0xFFFFEA0000000000 (your kernel may differ)
   * AXIOM: sizeof(struct page) = 64 bytes
   *
   * EXAMPLE CALCULATION:
   * p = 0xfffff7da455024c0
   * vmemmap_base = 0xfffff7da40000000 (example, varies by kernel)
   * offset = 0xfffff7da455024c0 - 0xfffff7da40000000 = 0x55024c0
   * PFN = 0x55024c0 / 64 = 0x154093
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  pfn_normal = page_to_pfn(normal_page);

  printk(KERN_INFO "MetaUnionHW: NORMAL: page=%px PFN=0x%lx\n", normal_page,
         pfn_normal);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 3: READ lru.prev FROM NORMAL PAGE (offset 8-15)
   *
   * AXIOM: struct page layout:
   *   offset 0-7:   flags (8 bytes)
   *   offset 8-15:  lru.prev (8 bytes) ← THIS
   *   offset 16-23: lru.next (8 bytes)
   *   offset 24-31: mapping (8 bytes)
   *
   * AXIOM: lru is struct list_head = {prev, next} = 16 bytes total.
   *
   * YOUR DATA: Freshly allocated page has:
   *   lru.prev = 0xdead000000000122 (LIST_POISON1 = not linked)
   *   lru.next = 0x0000000000000000 or poison
   *
   * WRITE THIS LINE (cast to unsigned long):
   * ════════════════════════════════════════════════════════════════════════ */

  offset_8_15_normal = /* TODO: (unsigned long)normal_page->lru.prev */ (
                           unsigned long)normal_page->lru.prev;

  printk(KERN_INFO "MetaUnionHW: NORMAL: offset 8-15 (lru.prev) = 0x%lx\n",
         offset_8_15_normal);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 4: READ lru.next FROM NORMAL PAGE (offset 16-23)
   *
   * AXIOM: lru.next is at offset 16 in struct page.
   * AXIOM: For unlinked page: lru.next = NULL or LIST_POISON2.
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  offset_16_23_normal = /* TODO: (unsigned long)normal_page->lru.next */ (
                            unsigned long)normal_page->lru.next;

  printk(KERN_INFO "MetaUnionHW: NORMAL: offset 16-23 (lru.next) = 0x%lx\n",
         offset_16_23_normal);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 5: CHECK PageSlab() FOR NORMAL PAGE
   *
   * AXIOM: PageSlab(page) returns 1 if PG_slab bit set, 0 otherwise.
   * AXIOM: PG_slab bit position = 7 (from page-flags.h).
   * AXIOM: PageSlab(p) = (p->flags >> 7) & 1 (simplified).
   *
   * EXPECTED: PageSlab(normal_page) = 0 (not a slab page).
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  pgslab_normal =
      /* TODO: PageSlab(normal_page) */ PageSlab(normal_page);

  printk(KERN_INFO "MetaUnionHW: NORMAL: PageSlab() = %d (expected 0)\n",
         pgslab_normal);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 6: ALLOCATE SLAB OBJECT WITH kmalloc
   *
   * AXIOM: kmalloc(size, gfp) allocates size bytes from slab allocator.
   * AXIOM: kmalloc returns kernel virtual address (0xffff888...).
   * AXIOM: Slab allocator carves 4096-byte pages into smaller chunks.
   *
   * EXAMPLE: kmalloc(128, GFP_KERNEL)
   *   → Returns 0xffff8cf5c8dafe00 (128 bytes at this address)
   *   → The page containing this address has PG_slab set.
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  slab_obj = /* TODO: kmalloc(128, GFP_KERNEL) */ kmalloc(128, GFP_KERNEL);

  if (!slab_obj) {
    __free_pages(normal_page, 0);
    printk(KERN_ERR "MetaUnionHW: kmalloc failed\n");
    return -ENOMEM;
  }

  printk(KERN_INFO "MetaUnionHW: SLAB: obj = %px\n", slab_obj);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 7: CONVERT SLAB OBJECT POINTER TO STRUCT PAGE
   *
   * EXACT MATH DERIVATION for virt_to_page(slab_obj):
   *
   * 1. AXIOM: slab_obj is a Linear Kernel Address (e.g. 0xffff888123456780).
   *    It points to the 128-byte data object in RAM.
   *
   * 2. AXIOM: PAGE_OFFSET = 0xffff888000000000 (Start of linear mapping).
   *    Kernel maps all physical RAM starting at this virtual address.
   *
   * 3. DERIVE PHYSICAL ADDRESS:
   *    Phys = slab_obj - PAGE_OFFSET
   *    Example: 0xffff888123456780 - 0xffff888000000000 = 0x123456780
   *
   * 4. DERIVE PFN (Page Frame Number):
   *    PFN = Phys / 4096 (or Phys >> 12)
   *    Example: 0x123456780 / 0x1000 = 0x123456
   *
   * 5. DERIVE STRUCT PAGE POINTER:
   *    Page = vmemmap_base + (PFN * sizeof(struct page))
   *    Example: 0xffffea0000000000 + (0x123456 * 64) = 0xffffea00048d1580
   *
   * FUNCTION: virt_to_page(x) performs steps 3, 4, and 5 for you.
   * ════════════════════════════════════════════════════════════════════════ */

  slab_page = virt_to_page(slab_obj);

  if (!slab_page) {
    kfree(slab_obj);
    __free_pages(normal_page, 0);
    printk(KERN_ERR "MetaUnionHW: virt_to_page failed\n");
    return -EINVAL;
  }

  pfn_slab = page_to_pfn(slab_page);
  printk(KERN_INFO "MetaUnionHW: SLAB: page = %px PFN = 0x%lx\n", slab_page,
         pfn_slab);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 8: CHECK PageSlab() FOR SLAB PAGE
   *
   * AXIOM: Pages used by kmalloc have PG_slab flag set.
   * AXIOM: PG_slab bit position = 7.
   * AXIOM: Code: (page->flags >> 7) & 1.
   *
   * EXPECTED: PageSlab(slab_page) = 1.
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  pgslab_slab = /* TODO: PageSlab(slab_page) */ PageSlab(slab_page);

  printk(KERN_INFO "MetaUnionHW: SLAB: PageSlab() = %d (expected 1)\n",
         pgslab_slab);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 9: READ OFFSET 8-15 FROM SLAB PAGE (SAME OFFSET AS lru.prev)
   *
   * AXIOM: Slab page uses DIFFERENT interpretation of offset 8-47.
   * AXIOM: For slab, offset 8-15 is 'slab_cache' pointer (points to
   * kmem_cache). AXIOM: We read it by casting lru.prev bits to unsigned long.
   *
   * KEY INSIGHT: Same memory address, different meaning.
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  offset_8_15_slab = /* TODO: (unsigned long)slab_page->lru.prev */ (
                         unsigned long)slab_page->lru.prev;

  printk(KERN_INFO "MetaUnionHW: SLAB: offset 8-15 = 0x%lx\n",
         offset_8_15_slab);

  /* ════════════════════════════════════════════════════════════════════════
   * TODO 10: READ OFFSET 16-23 FROM SLAB PAGE
   *
   * AXIOM: For slab, offset 16-23 is 'freelist' or 'next' pointer.
   * AXIOM: This must be a VALID kernel pointer (0xffff...).
   * AXIOM: Compare with Normal page where this was NULL.
   *
   * PREDICTION:
   *   Normal page offset 16-23 = 0x0000000000000000 (NULL)
   *   Slab page offset 16-23 = 0xffff8cf5c0047600 (VALID!)
   *
   * WRITE THIS LINE:
   * ════════════════════════════════════════════════════════════════════════ */

  offset_16_23_slab = /* TODO: (unsigned long)slab_page->lru.next */ (
                          unsigned long)slab_page->lru.next;

  printk(KERN_INFO "MetaUnionHW: SLAB: offset 16-23 = 0x%lx\n",
         offset_16_23_slab);

  /* ════════════════════════════════════════════════════════════════════════
   * COMPARISON TABLE - NO TODO, JUST OUTPUT
   * ════════════════════════════════════════════════════════════════════════ */
  printk(KERN_INFO "MetaUnionHW:\n");
  printk(KERN_INFO
         "MetaUnionHW: +--------+------------------+------------------+\n");
  printk(KERN_INFO
         "MetaUnionHW: | Offset | Normal Page      | Slab Page        |\n");
  printk(KERN_INFO
         "MetaUnionHW: +--------+------------------+------------------+\n");
  printk(KERN_INFO "MetaUnionHW: | 8-15   | 0x%016lx | 0x%016lx |\n",
         offset_8_15_normal, offset_8_15_slab);
  printk(KERN_INFO "MetaUnionHW: | 16-23  | 0x%016lx | 0x%016lx |\n",
         offset_16_23_normal, offset_16_23_slab);
  printk(KERN_INFO
         "MetaUnionHW: +--------+------------------+------------------+\n");
  printk(KERN_INFO "MetaUnionHW: PageSlab: Normal=%d, Slab=%d\n", pgslab_normal,
         pgslab_slab);
  printk(KERN_INFO "MetaUnionHW: Offset 16-23 DIFFERENT? %s\n",
         offset_16_23_normal != offset_16_23_slab ? "YES ✓" : "NO ✗");

  /* CLEANUP */
  kfree(slab_obj);
  __free_pages(normal_page, 0);

  printk(KERN_INFO
         "MetaUnionHW: Module loaded. Fill TODOs, recompile, verify.\n");
  return 0;
}

static void __exit metadata_union_hw_exit(void) {
  printk(KERN_INFO "MetaUnionHW: Module unloaded.\n");
}

module_init(metadata_union_hw_init);
module_exit(metadata_union_hw_exit);

/* ══════════════════════════════════════════════════════════════════════════
 * SUMMARY OF TODO BLOCKS:
 *
 * TODO 1:  normal_page = alloc_page(GFP_KERNEL);
 * TODO 2:  pfn_normal = page_to_pfn(normal_page);
 * TODO 3:  offset_8_15_normal = (unsigned long)normal_page->lru.prev;
 * TODO 4:  offset_16_23_normal = (unsigned long)normal_page->lru.next;
 * TODO 5:  pgslab_normal = PageSlab(normal_page);
 * TODO 6:  slab_obj = kmalloc(128, GFP_KERNEL);
 * TODO 7:  slab_page = virt_to_page(slab_obj);
 * TODO 8:  pgslab_slab = PageSlab(slab_page);
 * TODO 9:  offset_8_15_slab = (unsigned long)slab_page->lru.prev;
 * TODO 10: offset_16_23_slab = (unsigned long)slab_page->lru.next;
 * ══════════════════════════════════════════════════════════════════════════ */
