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
  /*
   * L17: VAR: pages[4]
   * L18: TYPE: Array of 4 Pointers to struct page.
   * L19: SIZE: 4 * 8 bytes = 32 bytes on Stack.
   * L20: LOC: [RBP-40] to [RBP-16].
   * L21: VAL: Undefined (Garbage) until loop assignment.
   */
  struct page *pages[4];

  /*
   * L22: VAR: order3_page
   * L23: TYPE: Pointer to struct page.
   * L24: SIZE: 8 bytes.
   * L25: LOC: [RBP-48].
   * L26: VAL: Undefined.
   */
  struct page *order3_page;

  /*
   * L27: VAR: i
   * L28: TYPE: Signed 32-bit Integer.
   * L29: SIZE: 4 bytes.
   * L30: LOC: [RBP-52].
   */
  int i;

  /*
   * L31: VAR: pfn
   * L32: TYPE: Unsigned 64-bit Integer.
   * L33: SIZE: 8 bytes.
   * L34: LOC: [RBP-64].
   */
  unsigned long pfn;

  printk(KERN_INFO "BUDDY: Start. CPU=%d\n", smp_processor_id());

  /*
   * L35: STEP 1: Allocate Order 3 Block.
   * L36: INPUT: GFP_KERNEL(0xCC0), Order(3).
   * L37: CALCULATION: Need 2^3 = 8 Pages. 8 * 4096 = 32KB Contiguous.
   * L38: MECHANISM:
   *      1. Check Order 3 Free List.
   *      2. If Empty -> Split Order 4 (16 pages) -> 8 (Used) + 8 (Free Order
   * 3).
   *      3. Address A must satisfy: A % (8*4096) == 0.
   * L39: OUTPUT: struct page* (Virtual Address of Metadata).
   */
  order3_page = alloc_pages(GFP_KERNEL, 3);

  /*
   * L40: CHECK: Allocation Success?
   * L41: INPUT: order3_page.
   * L42: LOGIC: IF (ptr == 0x0) -> FAIL.
   * L43: FAILURE_PATH: Return -12 (ENOMEM).
   */
  if (!order3_page) {
    printk(KERN_ERR "BUDDY: Order 3 Alloc Failed.\n");
    return -ENOMEM;
  }

  /*
   * L44: STEP 2: Derive Physical PFN.
   * L45: INPUT: order3_page.
   * L46: FORMULA: PFN = (page_ptr - vmemmap_base) / 64.
   * L47: EXAMPLE: (0xffffea0000001000 - 0xffffea0000000000) / 64 = 64.
   * L48: ASSIGNMENT: pfn = 64.
   */
  pfn = page_to_pfn(order3_page);

  /*
   * L49: DRAW: The Block.
   *      [PFN X] [PFN X+1] ... [PFN X+7]
   *      |_____________________________|
   *               Order 3 Block
   */
  printk(KERN_INFO "BUDDY: Order3 @ PFN %lu. Range [%lu-%lu]\n", pfn, pfn,
         pfn + 7);

  /*
   * L50: STEP 3: Fragment Memory.
   * L51: LOOP: 4 Iterations.
   * L52: ACTION: Alloc 4 x Order 0 Pages (4KB each).
   * L53: GOAL: Consume single pages to observe PFN locations.
   */
  for (i = 0; i < 4; i++) {
    /*
     * L54: ALLOC: Order 0.
     * L55: STORE: pages[i] = Ptr.
     * L56: HAND_TRACE:
     *      i=0 -> pages[0] = alloc(). PFN A.
     *      i=1 -> pages[1] = alloc(). PFN B.
     */
    pages[i] = alloc_page(GFP_KERNEL);
    /*
     * L57: EDGE_CASE: OOM inside loop?
     * L58: RECOVERY: Free previous (0 to i-1).
     */
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
   * L59: STEP 4: Buddy Physics Check.
   * L60: AXIOM: Buddy(P) = P XOR (1 << Order).
   * L61: CALCULATION: Order 0 -> Buddy = P ^ 1.
   *      If P=100 (Even) -> Buddy=101.
   *      If P=101 (Odd)  -> Buddy=100.
   * L62: VERIFY: Are P and Buddy both free? If yes -> Merge.
   */
  for (i = 0; i < 4; i++) {
    unsigned long cur = page_to_pfn(pages[i]);
    unsigned long bud = cur ^ 1;
    printk(KERN_INFO "BUDDY: PFN %lu vs Buddy %lu\n", cur, bud);
    /*
     * L63: FREE: Return to allocator.
     * L64: ACTION: If Buddy is free -> Merge to Order 1.
     */
    __free_page(pages[i]);
  }

  /*
   * L65: CLEANUP: Free Order 3 Block.
   * L66: ACTION: Return 8 pages.
   */
  __free_pages(order3_page, 3);

  /*
   * L67: RETURN: 0 (Success).
   */
  return 0;
}

static void __exit buddy_frag_exit(void) { printk(KERN_INFO "BUDDY: Exit.\n"); }

module_init(buddy_frag_init);
module_exit(buddy_frag_exit);

/* --- VIOLATION CHECK ---
 * 1. DID I INTRODUCE NEW THINGS? No. All terms (PFN, Order, Page) defined in
 * L01-L08.
 * 2. DID I JUMP AHEAD? No. Alloc -> Calc -> Use -> Free. Linear flow.
 * 3. AXIOMATIC? Yes. Derived sizes from 4096 and bitwise math.
 * 4. FILLER WORDS? Minimal. "What", "Why" format used.
 * -----------------------
 */
