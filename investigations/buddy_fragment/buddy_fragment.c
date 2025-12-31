/* L01: AXIOM: Machine=x86_64. Bit=0/1. Byte=8bits. RAM=Array[0..MAX].
 * L02: DEFINITION: Page=4096B(2^12). PFN=Addr>>12.
 * L03: PUZZLE_HARDER(Punishment):
 *      - GIVEN: RAM=4TB. PFN_MAX = 4TB/4KB = 1,073,741,824 (2^30).
 *      - TARGET: Order 11 (2^11=2048 pages).
 *      - ADDRESS_A: PFN 1,000,000,000 (0x3B9ACA00).
 *      - BINARY: 0011_1011_1001_1010_1100_1010_0000_0000.
 *      - CHECK: Is A start of Order 11?
 *      - LOGIC: Order 11 mask = (1<<11)-1 = 0x7FF = 111_1111_1111 (Bin).
 *      - ALIGNMENT: 0x3B9ACA00 & 0x7FF = 0x200 (10_0000_0000) != 0.
 *      - CONCLUSION: ✗ Failed. PFN 10^9 is INSIDE an Order 11 block, at offset
 * 512.
 *      - CORRECT_START: 0x3B9ACA00 & ~0x7FF = 0x3B9AC800 (PFN 999,999,488).
 */

#include <linux/gfp.h>    // [L08: Const: GFP_KERNEL=0xCC0]
#include <linux/init.h>   // [L06: Attr: __init -> Section cleanup]
#include <linux/kernel.h> // [L05: Func: printk -> RingBuffer]
#include <linux/mm.h>     // [L07: Struct: page -> 64B Metadata]
#include <linux/module.h> // [L04: Macro: MODULE_LICENSE -> Modinfo]

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate_Coder");
MODULE_DESCRIPTION("Axiomatic Buddy Allocator Trace");

static int __init buddy_frag_init(void) {
  /* L09: VAR: pages[4]@Stack. SIZE: 32B. VAL: Garbage. */
  struct page *pages[4];
  /* L10: VAR: order3_page@Stack. SIZE: 8B. VAL: Garbage. */
  struct page *order3_page;
  /* L11: VAR: i@Stack. SIZE: 4B. VAL: Garbage. */
  int i;
  /* L12: VAR: pfn@Stack. SIZE: 8B. VAL: Garbage. */
  unsigned long pfn;

  printk(KERN_INFO "BUDDY: Start. CPU=%d\n", smp_processor_id());

  /*
   * STEP 1: alloc_pages(GFP_KERNEL, 3)
   * ----------------------------------
   * 1. WHAT: Alloc 2^3=8 contiguous pages (32KB).
   * 2. WHY: Stress test Order 3 FreeList.
   * 3. WHERE: Zone Normal (Start PFN > 1M).
   * 4. WHO: __alloc_pages_nodemask (Buddy).
   * 5. WHEN: Immediate (via fast path or reclaim).
   * 6. WITHOUT: NULL (-ENOMEM).
   * 7. CALCULATON (Large Scale):
   *      - Order 3 Block Size = 32,768 Bytes.
   *      - If RAM = 16GB = 16 * 1024^3 = 17,179,869,184 Bytes.
   *      - Max Order 3 Blocks = 17,179,869,184 / 32,768 = 524,288 Blocks.
   *      - Bitmap Overhead (1 bit per block) = 524,288 bits = 65,536 Bytes =
   * 64KB.
   *      - ∴ Tracking 16GB needs only 64KB of bitmap (Efficient).
   */
  order3_page = alloc_pages(GFP_KERNEL, 3);

  if (!order3_page) {
    printk(KERN_ERR "BUDDY: Order 3 Alloc Failed.\n");
    return -ENOMEM;
  }

  /*
   * STEP 2: page_to_pfn(order3_page)
   * --------------------------------
   * 1. WHAT: PFN Extraction.
   * 2. WHY: Hardware uses Address, OS uses PFN index.
   * 3. WHERE: vmemmap array.
   * 4. WHO: CPU ALU.
   * 5. WHEN: < 1 cycle.
   * 6. WITHOUT: No Physical Address known.
   * 7. CALCULATON (Edge Case/Fractional):
   *      - If Ptr is at end of memory.
   *      - Ptr = Base + (MaxPFN * 64).
   *      - Calc: ( (Base + MaxPFN*64) - Base ) / 64 = MaxPFN. ✓
   *      - If Strut Size changed to 56B (Fractional fit in CacheLine 64B):
   *      - 1,000,000 Pages * 56B = 56MB Metadata. (vs 64MB).
   *      - Saving = 8MB. (But alignment cost > saving).
   */
  pfn = page_to_pfn(order3_page);

  printk(KERN_INFO "BUDDY: Order3 @ PFN %lu. Range [%lu-%lu]\n", pfn, pfn,
         pfn + 7);

  for (i = 0; i < 4; i++) {
    /*
     * STEP 3: alloc_page(GFP_KERNEL) [Loop]
     * -------------------------------------
     * 1. WHAT: Single Page Alloc (4KB).
     * 2. WHY: Fragment the blocks.
     * 3. WHERE: PCP List -> Zone FreeList.
     * 4. WHO: PCP Allocator.
     * 5. WHEN: Fast Path.
     * 6. WITHOUT: Panic/Fail.
     * 7. CALCULATON (Mid Scale):
     *      - Alloc 4 pages.
     *      - Gap Analysis: If PFNs are 100, 102, 104, 106.
     *      - Gaps are 101, 103, 105.
     *      - Can we merge 102+103? Only if 102 is even aligned (102%2==0). ✓
     *      - Can we merge 101+102? No. 101%2 != 0. ✗
     */
    pages[i] = alloc_page(GFP_KERNEL);
    if (!pages[i]) {
      int k;
      for (k = 0; k < i; k++)
        __free_page(pages[k]);
      __free_pages(order3_page, 3);
      return -ENOMEM;
    }
    printk(KERN_INFO "BUDDY: Page[%d] @ PFN %lu\n", i, page_to_pfn(pages[i]));
  }

  /*
   * STEP 4: XOR Buddy Check
   * -----------------------
   * 1. WHAT: Determine Sibling PFN.
   * 2. WHY: Merging requirement.
   * 3. WHERE: Register.
   * 4. WHO: ALU.
   * 5. WHEN: Pre-Free check.
   * 6. WITHOUT: No compaction.
   * 7. CALCULATON (Bit Pattern Punishment):
   *      - PFN = 0xFFFF_FFFF_FFFF_FFFF (Max 64-bit).
   *      - Order 0 Buddy = PFN ^ 1.
   *      - Result: 0xFFFF_FFFF_FFFF_FFFE.
   *      - Order 63 Buddy (Theoretical) = PFN ^ (1<<63).
   *      - Result: 0x7FFF_FFFF_FFFF_FFFF.
   */
  for (i = 0; i < 4; i++) {
    unsigned long cur = page_to_pfn(pages[i]);
    unsigned long bud = cur ^ 1;
    printk(KERN_INFO "BUDDY: PFN %lu vs Buddy %lu\n", cur, bud);
    __free_page(pages[i]);
  }

  __free_pages(order3_page, 3);
  return 0;
}

static void __exit buddy_frag_exit(void) { printk(KERN_INFO "BUDDY: Exit.\n"); }

module_init(buddy_frag_init);
module_exit(buddy_frag_exit);

/*
 * === DENSE AXIOMATIC TRACE: buddy_frag_init ===
 * DRAW[Memory:Stack@0xFFFF8000(RBP)→pages[4]@-40|order3@-48|i@-52|pfn@-64]→CHECK[RSP=0xFFFF7FBC]→ARGS[void]→CALL[alloc_pages(GFP_KERNEL=0xCC0,Order=3)]→DRAW[ZoneNormal:FreeArea[3]:Head→Page@PFN64(0xffffea0000001000)→Next→NULL]→LOGIC[Pop_Head→List_Empty]→RETURN[0xffffea0000001000]→STORE[order3_page=0xffffea0000001000]→CHECK[!=NULL]→✓→CALL[page_to_pfn(0xffffea0000001000)]→LOGIC[(Ptr-VmemmapBase)/64]→RETURN[64]→STORE[pfn=64]→CALL[printk("Order3@PFN%lu",64)]→DRAW[Loop:i=0]→CALL[alloc_page(0xCC0)]→DRAW[ZoneNormal:FreeArea[0]:Head→Page@PFN128→Next→Page@PFN129]→LOGIC[Pop_Head→List=Page@PFN129]→RETURN[0xffffea0000002000]→STORE[pages[0]=0xffffea0000002000]→DRAW[Loop:i=1]→CALL[alloc_page(0xCC0)]→DRAW[ZoneNormal:FreeArea[0]:Head→Page@PFN129→Next→Page@PFN320]→LOGIC[Pop_Head→List=Page@PFN320]→RETURN[0xffffea0000002001]→STORE[pages[1]=0xffffea0000002001]→DRAW[Loop:i=2]→CALL[alloc_page(0xCC0)]→LOGIC[Pop_Page@PFN320]→STORE[pages[2]]→DRAW[Loop:i=3]→CALL[alloc_page(0xCC0)]→LOGIC[Pop_Page@PFN321]→STORE[pages[3]]→CALL[page_to_pfn(pages[0])]→RETURN[128]→CALC[Buddy=128^1=129]→CALL[__free_page(PFN128)]→DRAW[FreeList[0]:Add(PFN128)→Check(PFN129)]→LOGIC[PFN129_is_Allocated→NoMerge]→CALL[page_to_pfn(pages[1])]→RETURN[129]→CALC[Buddy=129^1=128]→CALL[__free_page(PFN129)]→DRAW[FreeList[0]:Add(PFN129)→Check(PFN128)]→LOGIC[PFN128_is_Free→MERGE(128,129)→Order1_Block(PFN128)]→CALL[__free_pages(PFN64,3)]→DRAW[FreeList[3]:Add(PFN64)]→RETURN[0]
 */
