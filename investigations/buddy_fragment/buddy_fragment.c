/* L01: AXIOM: Machine = x86_64. CPU = 64-bit.
 * L02: AXIOM: Bit = 0 or 1. Byte = 8 bits.
 * L03: AXIOM: Physical RAM = Contiguous Array of Bytes.
 * L04: AXIOM: Page = 4096 bytes (2^12) block of RAM.
 * L05: AXIOM: PFN (Page Frame Number) = PhysicalAddress >> 12.
 * L06: DERIVATION: Order N -> 2^N contiguous pages.
 * L07: CALCULATION: Order 0 = 1 page = 4096 bytes.
 * L08: CALCULATION: Order 3 = 2^3 pages = 8 pages; Size = 8 * 4096 = 32768
 * bytes.
 */

#include <linux/gfp.h>    // [L13: Const: GFP_KERNEL = 0xCC0 -> Wait Allowed]
#include <linux/init.h>   // [L11: Attr: __init -> .init.text Section]
#include <linux/kernel.h> // [L10: Func: printk -> Ring Buffer Writer]
#include <linux/mm.h>     // [L12: Struct: page -> 64 bytes Metadata]
#include <linux/module.h> // [L09: Macro: MODULE_LICENSE -> Modinfo Metadata]

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate_Coder");
MODULE_DESCRIPTION("Axiomatic Buddy Allocator Trace");

/*
 * FUNCTION: buddy_frag_init
 * L14: WHAT: Entry point. Called by kernel thread (insmod).
 * L15: CONTEXT: Process Context (Can Sleep).
 * L16: STACK_FRAME_DRAWING:
 *      [High Addr]
 *      | Return Address (8B) [RBP+8]
 *      | Saved RBP      (8B) [RBP]
 *      | pages[3]       (8B) [RBP-16]
 *      | pages[2]       (8B) [RBP-24]
 *      | pages[1]       (8B) [RBP-32]
 *      | pages[0]       (8B) [RBP-40]
 *      | order3_page    (8B) [RBP-48]
 *      | i              (4B) [RBP-52]
 *      | pfn            (8B) [RBP-64]
 *      [Low Addr]
 */
static int __init buddy_frag_init(void) {
  struct page *pages[4];
  struct page *order3_page;
  int i;
  unsigned long pfn;

  printk(KERN_INFO "BUDDY: Start. CPU=%d\n", smp_processor_id());

  /*
   * STEP 1: alloc_pages(GFP_KERNEL, 3)
   * ----------------------------------
   * 1. WHAT: Request for 8 contiguous physical pages.
   *    - Size = 8 * 4096 = 32,768 bytes.
   * 2. WHY: To create a high-order contiguous block.
   *    - Fragmentation checks if 32KB chunks exist.
   * 3. WHERE: Zone Normal (Start PFN > 1,048,576).
   *    - Specific Range Example: PFN 0x100000 to PFN 0x100007.
   * 4. WHO: Buddy Allocator (mm/page_alloc.c).
   * 5. WHEN: Immediate (if FreeList[3] not empty) OR After Reclaim (ms
   * latency).
   * 6. WITHOUT: If Order 3 list empty AND Order > 3 lists empty -> Returns
   * NULL.
   *    - Result: -ENOMEM (-12).
   * 7. WHICH: Order 3 Free List.
   *    - Index 3 in zone->free_area[].
   */
  order3_page = alloc_pages(GFP_KERNEL, 3);

  if (!order3_page) {
    printk(KERN_ERR "BUDDY: Order 3 Alloc Failed.\n");
    return -ENOMEM;
  }

  /*
   * STEP 2: page_to_pfn(order3_page)
   * --------------------------------
   * 1. WHAT: Pointer arithmetic to derive index.
   *    - Input: 0xffffea0000001000 (vmemmap address).
   *    - Base:  0xffffea0000000000 (vmemmap base).
   *    - Struct Size: 64 bytes.
   * 2. WHY: Kernel identifies pages by integer index (PFN), not pointer.
   * 3. WHERE: Array 'vmemmap'.
   * 4. WHO: Compiler/CPU ALU.
   * 5. WHEN: < 1 ns (Register calculation).
   * 6. WITHOUT: We cannot know the Physical Address.
   *    - PhysAddr = PFN * 4096.
   * 7. WHICH: The index corresponding to the physical frame.
   *    - Calc: (Ptr - Base) >> 6 (div 64).
   */
  pfn = page_to_pfn(order3_page);

  printk(KERN_INFO "BUDDY: Order3 @ PFN %lu. Range [%lu-%lu]\n", pfn, pfn,
         pfn + 7);

  for (i = 0; i < 4; i++) {
    /*
     * STEP 3: alloc_page(GFP_KERNEL) [In Loop]
     * ----------------------------------------
     * 1. WHAT: Request for 1 physical page (Order 0).
     *    - Size = 4096 bytes.
     * 2. WHY: To fragment memory by taking small chunks.
     * 3. WHERE: Any free PFN in Zone Normal.
     *    - e.g., PFN 200, then PFN 500, then PFN 300.
     * 4. WHO: Per-CPU Pageset (PCP) Allocator.
     *    - Hot/Cold optimization layer before Buddy.
     * 5. WHEN: < 100 cycles (Fast path).
     * 6. WITHOUT: Loop fails, goto cleanup.
     * 7. WHICH: Order 0 Free List (or PCP list).
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
   * STEP 4: Buddy Verification Loop
   * -------------------------------
   * 1. WHAT: XOR Calculation.
   *    - Formula: Buddy = PFN ^ (1 << 0).
   * 2. WHY: To find the hypothetical merge candidate.
   *    - If PFN=100, Buddy=101. If PFN=101, Buddy=100.
   * 3. WHERE: CPU Registers.
   * 4. WHO: CPU ALU (XOR instruction).
   * 5. WHEN: Immediate.
   * 6. WITHOUT: The allocator cannot merge pages.
   * 7. WHICH: The specific PFN that completes the pair.
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
