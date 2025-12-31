/* flags_zone_node.c - Kernel module demonstrating flags field with zone and
 * node encoding PARAGRAPH: "A flags field which specifies attributes of the
 * page and additionally encodes zone and optionally node information"
 * FUNCTIONS: set_page_zone(), set_page_node(), page_zonenum(), page_to_nid()
 * COMPILE: make
 * LOAD: sudo insmod flags_zone_node.ko
 * VERIFY: sudo dmesg | tail -40
 * UNLOAD: sudo rmmod flags_zone_node
 */
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Flags field with zone and node encoding demo");

/* D01 DEFINE FLAGS: 64-bit unsigned long at offset 0 in struct page → Holds
 * attributes + zone + node */
/* D02 ZONE ENCODING: Zone stored in bits 62-63 of flags → ZONES_SHIFT=62 →
 * ZONES_MASK=0x3 */
/* D03 NODE ENCODING: Node stored in bits 60-61 of flags → NODES_SHIFT=60 → For
 * NUMA systems */
/* D04 YOUR ZONES: ZONE_DMA=0 (0-16MB), ZONE_DMA32=1 (16MB-4GB), ZONE_NORMAL=2
 * (4GB+) → From mmzone.h:759,762,769 */

static int __init flags_zone_node_init(void) {
  struct page *p0, *p1, *p2;
  unsigned long pfn0, pfn1, pfn2;
  unsigned long flags0, flags1, flags2;
  int zone0, zone1, zone2;
  int node0, node1, node2;

  printk(KERN_INFO "FlagsZoneNode: === FLAGS FIELD + ZONE + NODE DEMO ===\n\n");

  /* === ALLOCATION 1: GFP_KERNEL → Typically from Normal zone === */
  /* D05 GFP_KERNEL=0xCC0 → Allow sleep → For process context → Prefers high
   * memory (Normal zone) */
  p0 = alloc_page(GFP_KERNEL);
  if (!p0) {
    printk(KERN_ERR "alloc1 failed\n");
    return -ENOMEM;
  }

  pfn0 = page_to_pfn(p0);
  flags0 = p0->flags;
  zone0 = page_zonenum(p0); /* D06 page_zonenum(p) = (p->flags >> ZONES_SHIFT) &
                               ZONES_MASK = (flags >> 62) & 0x3 */
  node0 = page_to_nid(p0);  /* D07 page_to_nid(p) = (p->flags >> NODES_SHIFT) &
                               NODES_MASK = (flags >> 60) & 0x? */

  printk(KERN_INFO "FlagsZoneNode: ALLOC1: p=%px pfn=0x%lx flags=0x%016lx\n",
         p0, pfn0, flags0);
  printk(KERN_INFO
         "FlagsZoneNode: ALLOC1: page_zonenum()=%d page_to_nid()=%d\n",
         zone0, node0);
  /* D08 DERIVE ZONE: 0=DMA, 1=DMA32, 2=Normal → If zone0=2 → Page from Normal
   * zone ✓ */
  /* D09 DERIVE NODE: 0=Node0 → Your machine has single NUMA node → node0=0
   * expected */

  /* === EXTRACTION BY HAND === */
  /* D10 Manual zone extraction: (flags0 >> 62) & 0x3 */
  printk(KERN_INFO
         "FlagsZoneNode: ALLOC1: Manual zone = (0x%lx >> 62) & 0x3 = %lu\n",
         flags0, (flags0 >> 62) & 0x3);
  /* D11 Verify: manual == page_zonenum()? */
  printk(KERN_INFO "FlagsZoneNode: ALLOC1: Manual=%lu, API=%d → %s\n",
         (flags0 >> 62) & 0x3, zone0,
         ((flags0 >> 62) & 0x3) == zone0 ? "MATCH ✓" : "MISMATCH ✗");

  /* === ALLOCATION 2: GFP_DMA32 → Must come from DMA32 zone === */
  /* D12 GFP_DMA32=0x04 → Allocate from DMA32 zone → For 32-bit DMA devices */
  p1 = alloc_page(GFP_DMA32);
  if (!p1) {
    __free_pages(p0, 0);
    printk(KERN_ERR "alloc2 failed\n");
    return -ENOMEM;
  }

  pfn1 = page_to_pfn(p1);
  flags1 = p1->flags;
  zone1 = page_zonenum(p1);
  node1 = page_to_nid(p1);

  printk(KERN_INFO "FlagsZoneNode: ALLOC2: p=%px pfn=0x%lx flags=0x%016lx\n",
         p1, pfn1, flags1);
  printk(KERN_INFO
         "FlagsZoneNode: ALLOC2: page_zonenum()=%d page_to_nid()=%d\n",
         zone1, node1);
  /* D13 VERIFY: zone1 should be 1 (DMA32) or 0 (DMA) → PFN should be < 1048576
   */
  printk(KERN_INFO
         "FlagsZoneNode: ALLOC2: PFN=%lu < 1048576? → %s (Zone=DMA32 or DMA)\n",
         pfn1, pfn1 < 1048576 ? "YES ✓" : "NO (surprising)");

  /* === ALLOCATION 3: GFP_DMA → Must come from DMA zone === */
  /* D14 GFP_DMA=0x01 → Allocate from DMA zone → For old ISA devices → PFN <
   * 4096 */
  p2 = alloc_page(GFP_DMA);
  if (!p2) {
    __free_pages(p0, 0);
    __free_pages(p1, 0);
    printk(KERN_ERR "alloc3 failed\n");
    return -ENOMEM;
  }

  pfn2 = page_to_pfn(p2);
  flags2 = p2->flags;
  zone2 = page_zonenum(p2);
  node2 = page_to_nid(p2);

  printk(KERN_INFO "FlagsZoneNode: ALLOC3: p=%px pfn=0x%lx flags=0x%016lx\n",
         p2, pfn2, flags2);
  printk(KERN_INFO
         "FlagsZoneNode: ALLOC3: page_zonenum()=%d page_to_nid()=%d\n",
         zone2, node2);
  /* D15 VERIFY: zone2 should be 0 (DMA) → PFN should be < 4096 */
  printk(KERN_INFO "FlagsZoneNode: ALLOC3: PFN=%lu < 4096? → %s (Zone=DMA)\n",
         pfn2, pfn2 < 4096 ? "YES ✓" : "NO ✗ (BUG or DMA zone exhausted)");

  /* === SUMMARY TABLE === */
  printk(KERN_INFO "FlagsZoneNode:\n");
  printk(KERN_INFO
         "FlagsZoneNode: +----------+-----------+----------+------+------+\n");
  printk(KERN_INFO
         "FlagsZoneNode: | GFP_FLAG | PFN       | flags    | zone | node |\n");
  printk(KERN_INFO
         "FlagsZoneNode: +----------+-----------+----------+------+------+\n");
  printk(KERN_INFO
         "FlagsZoneNode: | KERNEL   | 0x%06lx  | ...%04lx | %d    | %d    |\n",
         pfn0, flags0 & 0xFFFF, zone0, node0);
  printk(KERN_INFO
         "FlagsZoneNode: | DMA32    | 0x%06lx  | ...%04lx | %d    | %d    |\n",
         pfn1, flags1 & 0xFFFF, zone1, node1);
  printk(KERN_INFO
         "FlagsZoneNode: | DMA      | 0x%06lx  | ...%04lx | %d    | %d    |\n",
         pfn2, flags2 & 0xFFFF, zone2, node2);
  printk(KERN_INFO
         "FlagsZoneNode: +----------+-----------+----------+------+------+\n");
  printk(KERN_INFO "FlagsZoneNode: Zone names: 0=DMA, 1=DMA32, 2=Normal\n");

  /* === ZONE BOUNDARY VERIFICATION === */
  /* D16 DMA boundary: 16MB / 4096 = 4096 PFNs → PFN < 4096 = DMA zone */
  /* D17 DMA32 boundary: 4GB / 4096 = 1048576 PFNs → 4096 <= PFN < 1048576 =
   * DMA32 zone */
  /* D18 Normal: PFN >= 1048576 = Normal zone */
  printk(KERN_INFO "FlagsZoneNode: Zone boundaries: DMA<4096, DMA32<1048576, "
                   "Normal>=1048576\n");
  printk(KERN_INFO "FlagsZoneNode: ALLOC1 PFN=%lu: %s\n", pfn0,
         pfn0 >= 1048576 ? "Normal ✓" : (pfn0 >= 4096 ? "DMA32" : "DMA"));
  printk(KERN_INFO "FlagsZoneNode: ALLOC2 PFN=%lu: %s\n", pfn1,
         pfn1 >= 1048576 ? "Normal" : (pfn1 >= 4096 ? "DMA32 ✓" : "DMA ✓"));
  printk(KERN_INFO "FlagsZoneNode: ALLOC3 PFN=%lu: %s\n", pfn2,
         pfn2 >= 1048576 ? "Normal" : (pfn2 >= 4096 ? "DMA32" : "DMA ✓"));

  /* CLEANUP */
  __free_pages(p0, 0);
  __free_pages(p1, 0);
  __free_pages(p2, 0);

  printk(KERN_INFO "FlagsZoneNode: Module loaded. Check output above.\n");
  return 0;
}

static void __exit flags_zone_node_exit(void) {
  printk(KERN_INFO "FlagsZoneNode: Module unloaded.\n");
}

module_init(flags_zone_node_init);
module_exit(flags_zone_node_exit);
