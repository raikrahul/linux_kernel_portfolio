/* flags_lab.c - Module 1: struct page flags field exploration
 * COMPILE: make
 * LOAD: sudo insmod flags_lab.ko
 * VERIFY: sudo dmesg | tail -30
 * UNLOAD: sudo rmmod flags_lab
 */
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/page-flags.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Struct Page Flags Field Lab");

static int __init flags_lab_init(void) {
  struct page *p;
  unsigned long flags_val;
  unsigned long pfn;
  int zone_num;

  printk(KERN_INFO "FlagsLab: === MODULE 1: FLAGS FIELD ===\n");

  /* D01. DEFINE: flags is unsigned long (8 bytes) at offset 0 in struct page */
  /* D02. DEFINE: flags encodes (1) page attributes (bits 0-21) (2) zone (bits
   * 62-63) (3) node (bits 60-61 if NUMA) */
  /* D03. AXIOM: Your machine has 3 zones: DMA, DMA32, Normal → needs 2 bits for
   * zone encoding */

  /* TODO BLOCK 1: Allocate single page */
  /* ACTION: alloc_page(GFP_KERNEL) → returns struct page* → kernel picks from
   * Normal zone (PFN > 1048576) */
  /* DRAW: zone→free_area[0]→page→unlink→return */
  p = alloc_page(GFP_KERNEL);
  if (!p) {
    printk(KERN_ERR "FlagsLab: alloc_page failed\n");
    return -ENOMEM;
  }

  /* TODO BLOCK 2: Read flags field */
  /* FORMULA: flags_val = p->flags (direct access) OR READ_ONCE(p->flags) for
   * atomic read */
  /* OFFSET: p + 0 = flags (first 8 bytes of struct page) */
  flags_val = p->flags;
  pfn = page_to_pfn(p);

  /* TODO BLOCK 3: Extract zone from flags */
  /* FORMULA: zone_num = page_zonenum(p) = (flags >> ZONES_SHIFT) & ZONES_MASK
   */
  /* AXIOM: ZONES_SHIFT = 62 on x86_64 (top 2 bits), ZONES_MASK = 0x3 */
  zone_num = page_zonenum(p);

  printk(KERN_INFO "FlagsLab: p=%px pfn=0x%lx flags=0x%lx zone=%d\n", p, pfn,
         flags_val, zone_num);
  /* VERIFY: zone=2 means Normal (DMA=0, DMA32=1, Normal=2) */

  /* TODO BLOCK 4: Check specific flag bits */
  /* AXIOM: PG_lru=5, PG_active=8, PG_dirty=4, PG_locked=0 */
  /* FORMULA: PageLRU(p) = test_bit(PG_lru, &p->flags) = (flags >> 5) & 1 */
  printk(KERN_INFO "FlagsLab: PageLRU=%d PageDirty=%d PageLocked=%d\n",
         PageLRU(p) ? 1 : 0, PageDirty(p) ? 1 : 0, PageLocked(p) ? 1 : 0);
  /* EXPECT: All 0 for freshly allocated page */

  /* TODO BLOCK 5: Set a flag and verify */
  /* ACTION: SetPageDirty(p) → set_bit(PG_dirty, &p->flags) → flags |= (1 << 4)
   * = flags |= 0x10 */
  SetPageDirty(p);
  printk(KERN_INFO "FlagsLab: After SetPageDirty: flags=0x%lx PageDirty=%d\n",
         p->flags, PageDirty(p) ? 1 : 0);
  /* VERIFY: flags changed, PageDirty=1 */

  /* TODO BLOCK 6: Clear flag */
  /* ACTION: ClearPageDirty(p) → clear_bit(PG_dirty, &p->flags) → flags &= ~(1
   * << 4) = flags &= ~0x10 */
  ClearPageDirty(p);
  printk(KERN_INFO "FlagsLab: After ClearPageDirty: flags=0x%lx PageDirty=%d\n",
         p->flags, PageDirty(p) ? 1 : 0);

  /* CLEANUP */
  __free_pages(p, 0);
  printk(KERN_INFO "FlagsLab: Page freed. Module loaded.\n");

  return 0;
}

static void __exit flags_lab_exit(void) {
  printk(KERN_INFO "FlagsLab: Module unloaded.\n");
}

module_init(flags_lab_init);
module_exit(flags_lab_exit);
