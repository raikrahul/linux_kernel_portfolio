/* metadata_union.c - Demonstrate 40-byte metadata union in struct page
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║ PARAGRAPH: "A 40-byte metadata union, a union of a series of data       ║
 * ║ structures which varies depending on what the page is being used for."  ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * WHAT: Kernel module comparing metadata union fields for normal page vs slab
 * page. WHY: Same 40 bytes at offset 8-47 interpreted differently based on page
 * type. WHERE: Kernel space, accesses struct page fields directly. WHO:
 * alloc_page() → buddy allocator. kmalloc() → slab allocator. WHEN: Module load
 * triggers comparison. WITHOUT: Without union, struct page would be 200+ bytes
 * instead of 64 bytes. WHICH: Normal page uses lru/mapping/index. Slab page
 * uses slab_cache/freelist/slab_list.
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
MODULE_DESCRIPTION("Metadata union demo - normal page vs slab page");

/* ══════════════════════════════════════════════════════════════════════════
 * STRUCT PAGE MEMORY LAYOUT (64 bytes):
 *
 * ┌─────────┬────────────────────────────────────────────────────────────────┐
 * │ Offset  │ Field                                                          │
 * ├─────────┼────────────────────────────────────────────────────────────────┤
 * │ 0-7     │ unsigned long flags (8B)                                       │
 * ├─────────┼────────────────────────────────────────────────────────────────┤
 * │ 8-47    │ METADATA UNION (40B) ← THIS IS WHAT WE EXAMINE                 │
 * │         │ ┌─────────────────────────────────────────────────────────────┐│
 * │         │ │ CASE 1: Anonymous/Page Cache                               ││
 * │         │ │   8-23:  struct list_head lru (16B) {prev*, next*}         ││
 * │         │ │   24-31: struct address_space *mapping (8B)                ││
 * │         │ │   32-39: pgoff_t index (8B)                                ││
 * │         │ │   40-47: unsigned long private (8B)                        ││
 * │         │ ├─────────────────────────────────────────────────────────────┤│
 * │         │ │ CASE 2: Slab Page (cast to struct slab)                    ││
 * │         │ │   8-15:  struct kmem_cache *slab_cache (8B)                ││
 * │         │ │   16-23: void *freelist (8B)                               ││
 * │         │ │   24-31: struct slab *next (8B)                            ││
 * │         │ │   32-35: int objects (4B)                                  ││
 * │         │ │   36-39: int inuse (4B)                                    ││
 * │         │ │   40-47: padding/other (8B)                                ││
 * │         │ └─────────────────────────────────────────────────────────────┘│
 * ├─────────┼────────────────────────────────────────────────────────────────┤
 * │ 48-51   │ atomic_t _mapcount OR unsigned int page_type (4B)             │
 * ├─────────┼────────────────────────────────────────────────────────────────┤
 * │ 52-55   │ atomic_t _refcount (4B)                                       │
 * ├─────────┼────────────────────────────────────────────────────────────────┤
 * │ 56-63   │ unsigned long memcg_data (8B)                                 │
 * └─────────┴────────────────────────────────────────────────────────────────┘
 * Total = 8 + 40 + 4 + 4 + 8 = 64 bytes ✓
 * ══════════════════════════════════════════════════════════════════════════ */

static int __init metadata_union_init(void) {
  struct page *normal_page;
  struct page *slab_page;
  void *slab_obj;
  unsigned long pfn_normal, pfn_slab;

  printk(KERN_INFO "MetaUnion: === METADATA UNION DEMO ===\n\n");

  /* ════════════════════════════════════════════════════════════════════
   * PART 1: ALLOCATE NORMAL PAGE (Anonymous/Page Cache type)
   *
   * alloc_page(GFP_KERNEL) returns fresh page from buddy allocator.
   * This page uses CASE 1 layout: lru, mapping, index, private.
   * Fresh page: lru not linked, mapping=NULL, index=0.
   * ════════════════════════════════════════════════════════════════════ */
  normal_page = alloc_page(GFP_KERNEL);
  if (!normal_page) {
    printk(KERN_ERR "MetaUnion: alloc_page failed\n");
    return -ENOMEM;
  }

  pfn_normal = page_to_pfn(normal_page);
  printk(KERN_INFO "MetaUnion: NORMAL PAGE: page=%px PFN=0x%lx\n", normal_page,
         pfn_normal);

  /* ════════════════════════════════════════════════════════════════════
   * READ CASE 1 FIELDS (Anonymous/Page Cache):
   *
   * Offset 0-7:   flags
   * Offset 8-15:  lru.prev
   * Offset 16-23: lru.next
   * Offset 24-31: mapping
   * Offset 32-39: index
   * Offset 40-47: private
   *
   * For freshly allocated page:
   * - lru is not linked (prev/next = poison or NULL)
   * - mapping = NULL (not assigned to any address_space)
   * - index = undefined
   * ════════════════════════════════════════════════════════════════════ */
  printk(KERN_INFO "MetaUnion: NORMAL: flags=0x%016lx\n", normal_page->flags);
  printk(KERN_INFO
         "MetaUnion: NORMAL: lru.prev=%px lru.next=%px (offset 8-23)\n",
         normal_page->lru.prev, normal_page->lru.next);
  printk(KERN_INFO "MetaUnion: NORMAL: mapping=%px (offset 24-31)\n",
         normal_page->mapping);
  printk(KERN_INFO "MetaUnion: NORMAL: index=%lu (offset 32-39)\n",
         normal_page->index);
  printk(KERN_INFO "MetaUnion: NORMAL: private=%lu (offset 40-47)\n",
         normal_page->private);
  printk(KERN_INFO "MetaUnion: NORMAL: PageSlab()=%d (expected 0)\n",
         PageSlab(normal_page));

  /* ════════════════════════════════════════════════════════════════════
   * PART 2: ALLOCATE SLAB OBJECT
   *
   * kmalloc(128, GFP_KERNEL) allocates from slab allocator.
   * Slab allocator carves pages into small chunks.
   * The underlying page has PG_slab flag set.
   * The same 40 bytes at offset 8-47 are interpreted as slab fields.
   * ════════════════════════════════════════════════════════════════════ */
  slab_obj = kmalloc(128, GFP_KERNEL);
  if (!slab_obj) {
    __free_pages(normal_page, 0);
    printk(KERN_ERR "MetaUnion: kmalloc failed\n");
    return -ENOMEM;
  }

  /* ════════════════════════════════════════════════════════════════════
   * FIND THE STRUCT PAGE FOR SLAB OBJECT:
   *
   * slab_obj = 0xFFFF888012345680 (example kernel virtual address)
   * virt_to_page(slab_obj) converts to struct page pointer.
   *
   * CALCULATION:
   * phys = __pa(slab_obj) = slab_obj - PAGE_OFFSET
   * where PAGE_OFFSET = 0xFFFF888000000000 (direct map base)
   * pfn = phys >> 12
   * page = vmemmap + pfn
   *
   * BUT: virt_to_page() does this for us.
   * ════════════════════════════════════════════════════════════════════ */
  slab_page = virt_to_page(slab_obj);
  pfn_slab = page_to_pfn(slab_page);

  printk(KERN_INFO "MetaUnion: SLAB OBJECT: obj=%px → page=%px PFN=0x%lx\n",
         slab_obj, slab_page, pfn_slab);

  /* ════════════════════════════════════════════════════════════════════
   * CHECK PG_slab FLAG:
   *
   * PageSlab(page) checks if PG_slab bit is set in flags.
   * If true, the metadata union contains slab-specific fields,
   * NOT lru/mapping/index.
   * ════════════════════════════════════════════════════════════════════ */
  printk(KERN_INFO "MetaUnion: SLAB: flags=0x%016lx\n", slab_page->flags);
  printk(KERN_INFO "MetaUnion: SLAB: PageSlab()=%d (expected 1)\n",
         PageSlab(slab_page));

  /* ════════════════════════════════════════════════════════════════════
   * WARNING: DO NOT ACCESS lru/mapping/index ON SLAB PAGE!
   *
   * The bytes at offset 8-47 are NOT lru/mapping/index.
   * They are slab_cache/freelist/etc.
   * Accessing page->lru on slab page gives GARBAGE or CRASH.
   *
   * DEMONSTRATION (dangerous but educational):
   * We read the raw bytes and show they are different.
   * ════════════════════════════════════════════════════════════════════ */
  printk(KERN_INFO "MetaUnion: SLAB: [offset 8-15]=%px (interpreted as "
                   "lru.prev, actually slab_cache or other)\n",
         slab_page->lru.prev);
  printk(KERN_INFO "MetaUnion: SLAB: [offset 16-23]=%px (interpreted as "
                   "lru.next, actually freelist or other)\n",
         slab_page->lru.next);

  /* ════════════════════════════════════════════════════════════════════
   * COMPARE RAW BYTES AT SAME OFFSET:
   *
   * NORMAL PAGE offset 8-15: lru.prev = 0x___ (poison or NULL)
   * SLAB PAGE offset 8-15:   slab_cache ptr = 0x___ (valid kmem_cache*)
   *
   * Same memory location, different meanings!
   * ════════════════════════════════════════════════════════════════════ */
  printk(KERN_INFO "MetaUnion:\n");
  printk(
      KERN_INFO
      "MetaUnion: +--------+----------------------+----------------------+\n");
  printk(
      KERN_INFO
      "MetaUnion: | Offset | Normal Page          | Slab Page            |\n");
  printk(
      KERN_INFO
      "MetaUnion: +--------+----------------------+----------------------+\n");
  printk(KERN_INFO "MetaUnion: | 8-15   | lru.prev=%px | slab_data=%px |\n",
         normal_page->lru.prev, slab_page->lru.prev);
  printk(KERN_INFO "MetaUnion: | 16-23  | lru.next=%px | slab_data=%px |\n",
         normal_page->lru.next, slab_page->lru.next);
  printk(
      KERN_INFO
      "MetaUnion: +--------+----------------------+----------------------+\n");

  /* CLEANUP */
  kfree(slab_obj);
  __free_pages(normal_page, 0);

  printk(KERN_INFO
         "MetaUnion: Module loaded. Check dmesg for union comparison.\n");
  return 0;
}

static void __exit metadata_union_exit(void) {
  printk(KERN_INFO "MetaUnion: Module unloaded.\n");
}

module_init(metadata_union_init);
module_exit(metadata_union_exit);

/* ══════════════════════════════════════════════════════════════════════════
 * FAILURE PREDICTIONS:
 *
 * F1: Access page->mapping on slab page
 *     ERROR: Same offset (24-31) contains slab->next, not mapping
 *     Result: Garbage pointer, potential crash if dereferenced
 *
 * F2: Assume PageSlab() returns 0 for all pages
 *     ERROR: kmalloc uses slab pages, PageSlab() = 1
 *     Check before accessing lru/mapping/index
 *
 * F3: Think union means multiple fields valid simultaneously
 *     ERROR: Union means ONLY ONE interpretation valid at a time
 *     Same 40 bytes, different meanings based on page type
 *
 * F4: Forget compound pages have different metadata too
 *     ERROR: Tail pages have compound_head, not lru
 *     Check PageTail() before accessing lru
 * ══════════════════════════════════════════════════════════════════════════ */
