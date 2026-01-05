/* flags_zone_node_hw.c - HOMEWORK: YOU fill TODO blocks
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║ PARAGRAPH: "A flags field which specifies attributes of the page and     ║
 * ║ additionally encodes zone and optionally node information"               ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * WHAT: Kernel module that allocates page, reads flags, extracts zone/node.
 * WHY: Demonstrate struct page.flags encoding of zone (bits 62-63) and node
 * (bits 60-61). WHERE: Kernel space, runs as loadable module, accesses vmemmap
 * array. WHO: alloc_page() → buddy allocator → returns struct page pointer.
 * WHEN: Module load (insmod) triggers init, unload (rmmod) triggers exit.
 * WITHOUT: Without zone encoding, kernel cannot tell which devices can DMA to
 * page. WHICH: GFP_KERNEL=0xCC0 → Normal zone, GFP_DMA32=0x04 → DMA32 zone,
 * GFP_DMA=0x01 → DMA zone.
 */
#include <linux/gfp.h> /* GFP_KERNEL=0xCC0, GFP_DMA=0x01, GFP_DMA32=0x04 → allocation flags */
#include <linux/init.h> /* __init, __exit → section attributes for linker */
#include <linux/kernel.h> /* printk, KERN_INFO → 0 bytes, macro definitions only */
#include <linux/mm.h> /* struct page (64 bytes), page_to_pfn macro, vmemmap_base=0xFFFFEA0000000000 */
#include <linux/mmzone.h> /* ZONE_DMA=0, ZONE_DMA32=1, ZONE_NORMAL=2 → enum zone_type */
#include <linux/module.h> /* MODULE_LICENSE, module_init, module_exit → 0 bytes, just declarations */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Flags field homework - fill TODO blocks");

/* ══════════════════════════════════════════════════════════════════════════
 * MEMORY LAYOUT - YOUR MACHINE (verified via /proc/meminfo, getconf):
 *
 * RAM: 16,154,902,528 bytes = 0x3C3C8F000 bytes
 * ├── Byte 0x000000000 ────────────────────────────────────────────┐
 * │   ...                                                          │ DMA Zone
 * ├── Byte 0x000FFFFFF (16 MB - 1) ────────────────────────────────┤ PFN 0-4095
 * │   ...                                                          │ DMA32 Zone
 * ├── Byte 0x0FFFFFFFF (4 GB - 1) ─────────────────────────────────┤ PFN
 * 4096-1048575 │   ... │ Normal Zone └── Byte 0x3C3C8EFFF (16 GB - 1)
 * ────────────────────────────────┘ PFN 1048576-3944067
 *
 * PAGE_SIZE = 4096 bytes = 0x1000 = 2^12
 * PAGES = 16,154,902,528 / 4096 = 3,944,068 pages
 *
 * ZONE BOUNDARIES (derived from hardware constraints):
 * ┌─────────────────────────────────────────────────────────────────────────┐
 * │ Zone    │ PFN Start │ PFN End   │ Phys Start │ Phys End   │ Why        │
 * ├─────────┼───────────┼───────────┼────────────┼────────────┼────────────┤
 * │ DMA     │ 0         │ 4095      │ 0x0        │ 0xFFFFFF   │ ISA 24-bit │
 * │ DMA32   │ 4096      │ 1048575   │ 0x1000000  │ 0xFFFFFFFF │ PCI 32-bit │
 * │ Normal  │ 1048576   │ 3944067   │ 0x100000000│ 0x3C3C8EFFF│ 64-bit     │
 * └─────────┴───────────┴───────────┴────────────┴────────────┴────────────┘
 *
 * DERIVATION:
 * DMA end:   2^24 = 16,777,216 bytes → 16,777,216 / 4096 = 4096 PFNs
 * DMA32 end: 2^32 = 4,294,967,296 bytes → 4,294,967,296 / 4096 = 1,048,576 PFNs
 *
 * ══════════════════════════════════════════════════════════════════════════ */

/* ══════════════════════════════════════════════════════════════════════════
 * VMEMMAP MEMORY LAYOUT:
 *
 * AXIOM: Kernel needs 64-byte struct page for EACH physical page.
 * CALCULATE: 3,944,068 pages × 64 bytes = 252,420,352 bytes = 241 MB
 *
 * KERNEL ACTION AT BOOT:
 * 1. Reserve physical RAM[0x10000000..0x1F0ED400] for struct page array.
 * 2. Pick virtual address vmemmap_base = 0xFFFFEA0000000000.
 * 3. Create page table: 0xFFFFEA0000000000+OFFSET → 0x10000000+OFFSET.
 *
 * RESULT:
 * ┌───────────────────────────────────────────────────────────────────────┐
 * │ VIRTUAL ADDRESS              │ PHYSICAL ADDRESS  │ Contains          │
 * ├──────────────────────────────┼───────────────────┼───────────────────┤
 * │ 0xFFFFEA0000000000           │ 0x10000000        │ struct page[0]    │
 * │ 0xFFFFEA0000000040 (+64)     │ 0x10000040        │ struct page[1]    │
 * │ 0xFFFFEA0000000080 (+128)    │ 0x10000080        │ struct page[2]    │
 * │ ...                          │ ...               │ ...               │
 * │ 0xFFFFEA0008F7A300           │ 0x18F7A300        │ struct page[PFN]  │
 * └───────────────────────────────────────────────────────────────────────┘
 *
 * page_to_pfn(p) DERIVATION:
 * p = 0xFFFFEA0008F7A300 (virtual pointer to struct page)
 * vmemmap_base = 0xFFFFEA0000000000
 * offset = p - vmemmap_base = 0xFFFFEA0008F7A300 - 0xFFFFEA0000000000 =
 * 0x8F7A300 pfn = offset / sizeof(struct page) = 0x8F7A300 / 64 = 0x23DE8C =
 * 2,350,732
 *
 * THIS IS NOT MMU TRANSLATION. This is POINTER ARITHMETIC on KNOWN array
 * layout.
 * ══════════════════════════════════════════════════════════════════════════ */

static int __init flags_zone_node_hw_init(void) {
  /* ════════════════════════════════════════════════════════════════════
   * LOCAL VARIABLES - STACK LAYOUT AT 0x7FFFFFFFF000 (example):
   *
   * ┌─────────────────────────────────────────────────────────────────────┐
   * │ Offset │ Variable │ Size  │ Initial Value │ After alloc_page       │
   * ├────────┼──────────┼───────┼───────────────┼─────────────────────────┤
   * │ -8     │ p        │ 8B    │ undefined     │ 0xFFFFEA0008F7A300      │
   * │ -16    │ pfn      │ 8B    │ undefined     │ 0x23DE8C = 2,350,732    │
   * │ -24    │ flags    │ 8B    │ undefined     │ 0x0017FFFFC0000000      │
   * │ -28    │ zone     │ 4B    │ undefined     │ 2 (Normal)              │
   * │ -32    │ node     │ 4B    │ undefined     │ 0 (single NUMA node)    │
   * └─────────────────────────────────────────────────────────────────────┘
   * ════════════════════════════════════════════════════════════════════ */
  struct page
      *p; /* 8 bytes → pointer to vmemmap entry → will hold 0xFFFFEA... */
  unsigned long
      pfn; /* 8 bytes → 0 to 3,944,067 → derived from (p - vmemmap) / 64 */
  unsigned long
      flags;      /* 8 bytes → raw bits from p->flags → zone in bits 62-63 */
  int zone, node; /* 4+4 bytes → 0,1,2 for zone → 0 for node (your machine) */

  printk(KERN_INFO "FlagsHW: === HOMEWORK: FILL TODO BLOCKS ===\n\n");

  /* ════════════════════════════════════════════════════════════════════
   * alloc_page(GFP_KERNEL) CALL TRACE:
   *
   * GFP_KERNEL = 0xCC0 = 0b110011000000
   *   Bit 6 (0x40): __GFP_RECLAIM → can reclaim memory
   *   Bit 7 (0x80): __GFP_IO → can start I/O
   *   Bit 8 (0x100): __GFP_FS → can call filesystem
   *   Bit 10 (0x400): __GFP_HARDWALL → obey cpuset allocation
   *   Bit 11 (0x800): __GFP_THISNODE → allocate from this node
   *
   * CALL CHAIN:
   * alloc_page(0xCC0) → alloc_pages(0xCC0, 0) → __alloc_pages(0xCC0, 0,
   * preferred_nid, NULL) → get_page_from_freelist() → scan zones in order:
   * Normal → DMA32 → DMA → rmqueue() → remove page from buddy freelist →
   * prep_new_page() → set refcount=1, clear flags → RETURN struct page pointer
   *
   * EXAMPLE OUTPUT FROM YOUR MACHINE:
   * p = 0xfffff7da45906d80 (virtual address in vmemmap)
   * Calculation: 0xfffff7da45906d80 is kernel pointer to struct page
   * ════════════════════════════════════════════════════════════════════ */
  p = alloc_page(GFP_KERNEL); /* Returns struct page* or NULL if OOM */
  if (!p) {
    printk(KERN_ERR "FlagsHW: allocation failed\n");
    return -ENOMEM; /* -ENOMEM = -12 → "Cannot allocate memory" */
  }

  /* ════════════════════════════════════════════════════════════════════
   * page_to_pfn(p) CALCULATION:
   *
   * GIVEN: p = 0xfffff7da45906d80 (from dmesg output)
   * STEP 1: offset = p - vmemmap_base
   *         vmemmap_base = 0xFFFFEA0000000000 (architecture constant)
   *         WAIT: 0xFFFFF7DA... ≠ 0xFFFFEA00...
   *         Different kernel config uses different vmemmap!
   *         Let's use actual dmesg values:
   *
   * ACTUAL CALCULATION (from your dmesg):
   * p = 0xfffff7da45906d80
   * pfn reported = 0x1641b6 = 1,458,614
   *
   * VERIFY: Does pfn match zone?
   * pfn = 1,458,614
   * 1,458,614 < 4,096? → NO (not DMA)
   * 1,458,614 < 1,048,576? → NO (not DMA32)
   * 1,458,614 >= 1,048,576? → YES → Zone = Normal = 2 ✓
   *
   * PHYSICAL ADDRESS:
   * phys = pfn × PAGE_SIZE = 1,458,614 × 4,096 = 5,974,478,848 bytes
   * phys = 0x1641b6 × 0x1000 = 0x1641b6000 = 5.56 GB into RAM ✓
   * ════════════════════════════════════════════════════════════════════ */
  pfn = page_to_pfn(p); /* Macro: (p - vmemmap) >> 6 where >>6 = /64 */
  printk(KERN_INFO "FlagsHW: page=%px, PFN=0x%lx (%lu decimal)\n", p, pfn, pfn);

  /* ════════════════════════════════════════════════════════════════════
   * p->flags MEMORY READ:
   *
   * struct page LAYOUT (64 bytes):
   * ┌────────┬──────────────────────────────────────────────────────────┐
   * │ Offset │ Field                                                    │
   * ├────────┼──────────────────────────────────────────────────────────┤
   * │ 0      │ unsigned long flags (8 bytes) ← WE READ THIS             │
   * │ 8      │ metadata union (40 bytes) - lru/mapping/index/private    │
   * │ 48     │ _mapcount OR page_type (4 bytes)                         │
   * │ 52     │ _refcount (4 bytes)                                      │
   * │ 56     │ memcg_data (8 bytes)                                     │
   * └────────┴──────────────────────────────────────────────────────────┘
   * Total = 8 + 40 + 4 + 4 + 8 = 64 bytes ✓
   *
   * FLAGS BITS (from page-flags.h):
   * ┌───────────────────────────────────────────────────────────────────┐
   * │ Bits   │ Field              │ Your Value (example)               │
   * ├────────┼────────────────────┼────────────────────────────────────┤
   * │ 0      │ PG_locked          │ 0 (not locked)                     │
   * │ 1      │ PG_writeback       │ 0 (not under writeback)            │
   * │ 2      │ PG_referenced      │ 0 (not recently accessed)          │
   * │ 3      │ PG_uptodate        │ 0 (content not valid yet)          │
   * │ 4      │ PG_dirty           │ 0 (not modified)                   │
   * │ 5      │ PG_lru             │ 0 (not on LRU list)                │
   * │ 6      │ PG_head            │ 0 (not compound head)              │
   * │ ...    │ ...                │ ...                                │
   * │ 60-61  │ NODE bits          │ 00 (node 0)                        │
   * │ 62-63  │ ZONE bits          │ 10 (zone 2 = Normal)               │
   * └───────────────────────────────────────────────────────────────────┘
   *
   * YOUR flags = 0x0017FFFFC0000000
   * Binary: 0000 0000 0001 0111 1111 1111 1111 1111 1100 0000 0000 0000 0000
   * 0000 0000 0000
   * ════════════════════════════════════════════════════════════════════ */
  flags = p->flags; /* Direct memory read from vmemmap: RAM[vmemmap_phys +
                       pfn×64 + 0] */
  printk(KERN_INFO "FlagsHW: flags=0x%016lx\n", flags);

  /* ════════════════════════════════════════════════════════════════════
   * page_zonenum(p) EXTRACTION:
   *
   * PROBLEM: Zone bits NOT at fixed position 62-63!
   * Kernel config determines ZONES_SHIFT.
   *
   * page_zonenum() DOES: (p->flags >> ZONES_SHIFT) & ZONES_MASK
   * Where ZONES_SHIFT and ZONES_MASK are kernel config dependent.
   *
   * YOUR OUTPUT: page_zonenum() = 2 → ZONE_NORMAL
   *
   * ZONE_DMA = 0 → PFN 0 to 4095 → Phys 0 to 16MB
   * ZONE_DMA32 = 1 → PFN 4096 to 1048575 → Phys 16MB to 4GB
   * ZONE_NORMAL = 2 → PFN 1048576+ → Phys 4GB+
   * ════════════════════════════════════════════════════════════════════ */
  zone = page_zonenum(p); /* Kernel macro - handles config-dependent shift */
  printk(KERN_INFO "FlagsHW: page_zonenum() = %d\n", zone);

  /* ════════════════════════════════════════════════════════════════════
   * page_to_nid(p) EXTRACTION:
   *
   * YOUR MACHINE: Single NUMA node → node always = 0.
   *
   * NUMA EXAMPLE (2-node system):
   * Node 0: PFN 0 to 2,000,000 → RAM slots 0-8GB
   * Node 1: PFN 2,000,001 to 4,000,000 → RAM slots 8GB-16GB
   *
   * page_to_nid(p) returns which node owns this physical page.
   * Your machine: node = 0 always (only one node).
   * ════════════════════════════════════════════════════════════════════ */
  node = page_to_nid(p); /* Kernel macro - extracts node ID from flags */
  printk(KERN_INFO "FlagsHW: page_to_nid() = %d\n", node);

  /* ════════════════════════════════════════════════════════════════════
   * TODO BLOCK 1: VERIFY ZONE FROM PFN
   *
   * YOUR DATA (from dmesg):
   * PFN = 0x1641b6 = 1,458,614
   *
   * CALCULATION:
   * 1,458,614 < 4,096? → 1,458,614 > 4,096 → NO → Not DMA
   * 1,458,614 < 1,048,576? → 1,458,614 > 1,048,576 → NO → Not DMA32
   * 1,458,614 >= 1,048,576? → YES → Zone = Normal = 2
   *
   * ┌────────────────────────────────────────────────────────────────────┐
   * │ PFN Range          │ Zone   │ Your PFN=1,458,614 in range?        │
   * ├────────────────────┼────────┼─────────────────────────────────────┤
   * │ 0 to 4,095         │ DMA    │ NO (1,458,614 > 4,095)              │
   * │ 4,096 to 1,048,575 │ DMA32  │ NO (1,458,614 > 1,048,575)          │
   * │ 1,048,576+         │ Normal │ YES ✓ (1,458,614 >= 1,048,576)      │
   * └────────────────────────────────────────────────────────────────────┘
   * ════════════════════════════════════════════════════════════════════ */
  {
    int calculated_zone;

    if (pfn < 4096) {
      calculated_zone =
          0; /* DMA: ISA 24-bit = 2^24 = 16MB = 16MB/4096 = 4096 PFNs */
    } else if (pfn < 1048576) {
      calculated_zone =
          1; /* DMA32: PCI 32-bit = 2^32 = 4GB = 4GB/4096 = 1,048,576 PFNs */
    } else {
      calculated_zone = 2; /* Normal: Modern 64-bit devices, no address limit */
    }

    printk(KERN_INFO "FlagsHW: calculated_zone=%d, API_zone=%d → %s\n",
           calculated_zone, zone,
           calculated_zone == zone ? "MATCH ✓" : "MISMATCH ✗");
  }

  /* ════════════════════════════════════════════════════════════════════
   * TODO BLOCK 2: CALCULATE PHYSICAL ADDRESS FROM PFN
   *
   * FORMULA: PhysAddr = PFN × PAGE_SIZE
   *
   * YOUR DATA:
   * PFN = 0x1641b6 = 1,458,614
   * PAGE_SIZE = 4,096 = 0x1000 = 2^12
   *
   * CALCULATION METHOD 1 (multiplication):
   * PhysAddr = 1,458,614 × 4,096 = 5,974,478,848 bytes
   *
   * CALCULATION METHOD 2 (bit shift):
   * PhysAddr = PFN << 12 = 0x1641b6 << 12 = 0x1641b6000
   *
   * VERIFY: 0x1641b6000 = 5,974,478,848 ✓
   *
   * LOCATION IN RAM:
   * 5,974,478,848 bytes = 5.56 GB into your 16 GB RAM
   * ════════════════════════════════════════════════════════════════════ */
  {
    unsigned long phys_addr;

    phys_addr = pfn << 12; /* <<12 = ×4096 = ×2^12 = ×PAGE_SIZE */
    /*
     * WHY <<12 instead of ×4096?
     * <<12 is always exact for unsigned long (no overflow on 64-bit)
     * ×4096 could overflow on 32-bit if pfn > 1,048,576
     * Example: pfn=2,000,000 → 2,000,000 × 4,096 = 8,192,000,000 > 2^32 =
     * 4,294,967,296 → OVERFLOW!
     */

    printk(KERN_INFO "FlagsHW: phys_addr=0x%lx (expected 0x%lx000)\n",
           phys_addr, pfn);
  }

  /* ════════════════════════════════════════════════════════════════════
   * TODO BLOCK 3: EXTRACT INDIVIDUAL FLAG BITS
   *
   * YOUR DATA: flags = 0x0017FFFFC0000000
   *
   * EXTRACTION FORMULA: bit_value = (flags >> bit_position) & 1
   *
   * PG_locked (bit 0):
   *   (0x0017FFFFC0000000 >> 0) & 1
   *   = 0x0017FFFFC0000000 & 1
   *   = 0 (last bit is 0)
   *
   * PG_dirty (bit 4):
   *   (0x0017FFFFC0000000 >> 4) & 1
   *   = 0x0017FFFFC000000 & 1
   *   = 0 (last bit of shifted value is 0)
   *
   * PG_lru (bit 5):
   *   (0x0017FFFFC0000000 >> 5) & 1
   *   = 0x000BFFFFE00000 & 1
   *   = 0 (last bit of shifted value is 0)
   *
   * EXPECTED FOR FRESH PAGE: All attribute bits = 0 (page just allocated, not
   * used yet)
   * ════════════════════════════════════════════════════════════════════ */
  {
    int pg_locked, pg_dirty, pg_lru;

    pg_locked = (flags >> 0) & 1; /* Bit 0: Is page locked? 0=no, 1=yes */
    pg_dirty = (flags >> 4) & 1;  /* Bit 4: Is page dirty? 0=no, 1=yes */
    pg_lru = (flags >> 5) & 1;    /* Bit 5: Is page on LRU? 0=no, 1=yes */

    printk(KERN_INFO "FlagsHW: PG_locked=%d PG_dirty=%d PG_lru=%d\n", pg_locked,
           pg_dirty, pg_lru);
  }

  /* ════════════════════════════════════════════════════════════════════
   * TODO BLOCK 4: VERIFY ZONE BOUNDARY CALCULATION
   *
   * DMA ZONE DERIVATION:
   * PROBLEM: Old ISA DMA controller has 24-bit address bus.
   * CALCULATE: Max address = 2^24 = 16,777,216 bytes = 16 MB
   * CALCULATE: Max PFN = 16,777,216 / 4,096 = 4,096 PFNs
   * ∴ DMA zone = PFN 0 to 4,095
   *
   * DMA32 ZONE DERIVATION:
   * PROBLEM: 32-bit PCI devices have 32-bit address bus.
   * CALCULATE: Max address = 2^32 = 4,294,967,296 bytes = 4 GB
   * CALCULATE: Max PFN = 4,294,967,296 / 4,096 = 1,048,576 PFNs
   * ∴ DMA32 zone = PFN 4,096 to 1,048,575
   *
   * NORMAL ZONE:
   * PFN >= 1,048,576 → No hardware address limit → 64-bit devices
   * ════════════════════════════════════════════════════════════════════ */
  {
    unsigned long dma_end_pfn, dma32_end_pfn;

    dma_end_pfn = 4096;      /* 2^24 / 2^12 = 2^12 = 4096 */
    dma32_end_pfn = 1048576; /* 2^32 / 2^12 = 2^20 = 1,048,576 */

    printk(KERN_INFO "FlagsHW: DMA_end_PFN=%lu DMA32_end_PFN=%lu\n",
           dma_end_pfn, dma32_end_pfn);
  }

  /* CLEANUP: Return page to buddy allocator */
  __free_pages(p, 0); /* 0 = order → frees 2^0 = 1 page */

  printk(KERN_INFO "FlagsHW: Module loaded. Check dmesg for results.\n");
  return 0;
}

static void __exit flags_zone_node_hw_exit(void) {
  printk(KERN_INFO "FlagsHW: Module unloaded.\n");
}

module_init(flags_zone_node_hw_init);
module_exit(flags_zone_node_hw_exit);

/* ══════════════════════════════════════════════════════════════════════════
 * FAILURE PREDICTIONS - BUGS YOU WILL MAKE:
 *
 * F1: phys_addr = pfn * 4096 without proper type
 *     ERROR: On 32-bit, pfn=2,000,000 → 2,000,000 × 4,096 = 8,192,000,000 >
 * 2^32 OVERFLOW → Wrong answer! FIX: Use pfn << 12 or (unsigned long)pfn * 4096
 *
 * F2: (flags >> 62) & 0x3 for zone → WRONG on Kernel 6.14
 *     ERROR: Zone bits NOT at position 62-63 in your kernel config
 *     PROOF: Manual extraction gave 0, but page_zonenum() gave 2
 *     FIX: Always use page_zonenum() API
 *
 * F3: Confuse PFN with physical address
 *     ERROR: PFN=0x1000 ≠ PhysAddr=0x1000
 *     PFN=0x1000 → PhysAddr = 0x1000 × 0x1000 = 0x1000000 = 16MB
 *     RULE: PhysAddr = PFN × PAGE_SIZE always
 *
 * F4: Boundary off-by-one
 *     ERROR: DMA32 ends at PFN 1,048,575, Normal starts at PFN 1,048,576
 *     If PFN == 1,048,576 → Zone = Normal (NOT DMA32)
 *     Use >= for lower bound, < for upper bound
 *
 * F5: Wrong powers of 2
 *     ERROR: 2^24 = 16,000,000 (WRONG!)
 *     CORRECT: 2^24 = 16,777,216
 *     2^10 = 1,024
 *     2^20 = 1,048,576
 *     2^24 = 16,777,216
 *     2^32 = 4,294,967,296
 * ══════════════════════════════════════════════════════════════════════════ */
