/* L01: AXIOM: Atomic Context = Cannot Sleep.
 * L02: AXIOM: GFP_KERNEL = 0xCC0 (__GFP_RECLAIM | __GFP_IO | __GFP_FS).
 * L03: AXIOM: GFP_ATOMIC = 0x080 (__GFP_HIGH).
 * L04: PUNISHMENT_CALCULATION (Context Masking):
 *      - GFP_KERNEL involves bit 0x10 (___GFP_DIRECT_RECLAIM).
 *      - If in Interrupt Handler (IRQ): Flags = 0.
 *      - If sleep() called: Scheduler saves state, switches task.
 *      - BUT: IRQ has no task_struct context to save properly.
 *      - RESULT: CRASH (Scheduling in Interrupt).
 *      - LOGIC: (CurrentContext & ATOMIC) && (AllocFlags & SLEEP) -> BUG.
 */

#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/spinlock.h> // [L05: Spinlock -> Atomic Context]

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate_Coder");
MODULE_DESCRIPTION("Axiomatic GFP Context Bug Trace");

static DEFINE_SPINLOCK(my_lock);

static int __init gfp_bug_init(void) {
  struct page *page;
  unsigned long flags;

  printk(KERN_INFO "GFP_BUG: Start. CPU=%d\n", smp_processor_id());

  /*
   * STEP 1: spin_lock_irqsave()
   * ---------------------------
   * 1. WHAT: Disable IRQs local + Acquire Lock.
   * 2. WHY: Enter Atomic Context.
   * 3. WHERE: RFLAGS Register (IF bit cleared).
   * 4. WHO: CPU CLI instruction.
   * 5. CALCULATION (Preemption Count):
   *      - preempt_count() increments.
   *      - Val: 0 -> 1 (SoftIRQ/HardIRQ/Lock).
   *      - IF > 0, we are Atomic.
   */
  spin_lock_irqsave(&my_lock, flags);

  /*
   * STEP 2: alloc_page(GFP_ATOMIC)
   * ------------------------------
   * 1. WHAT: Safe allocation.
   * 2. WHY: GFP_ATOMIC does not sleep.
   * 3. CALCULATION (Watermarks):
   *      - Zone Watermark = MIN.
   *      - GFP_ATOMIC allows dipping into Reserves (MIN/2).
   *      - If FreePages < MIN: GFP_KERNEL sleeps.
   *      - If FreePages < MIN: GFP_ATOMIC succeeds (up to limit).
   */
  page = alloc_page(GFP_ATOMIC);
  if (page) {
    printk(KERN_INFO "GFP_BUG: Atomic Alloc Success PFN %lu\n",
           page_to_pfn(page));
    __free_page(page);
  }

  /*
   * STEP 3: The Hypothetical Bug (Commented to avoid crash)
   * -------------------------------------------------------
   * // page = alloc_page(GFP_KERNEL);
   * 1. ANALYSIS: GFP_KERNEL = Allows Sleep.
   * 2. CONTEXT: We hold &my_lock (Atomic).
   * 3. OUTCOME: "might_sleep" warning or Kernel Oops.
   */

  spin_unlock_irqrestore(&my_lock, flags);
  return 0;
}

static void __exit gfp_bug_exit(void) { printk(KERN_INFO "GFP_BUG: Exit.\n"); }

module_init(gfp_bug_init);
module_exit(gfp_bug_exit);

/*
 * === DENSE AXIOMATIC TRACE: gfp_bug_init ===
 * DRAW[Stack:flags]→CALL[spin_lock_irqsave]→WRITE[RFLAGS:IF=0]→INC[PreemptCount]→LOCK[Bus]→DONE→CALL[alloc_page(ATOMIC)]→CHECK[PreemptCount>0]→CHECK[Flags&RECLAIM==0]→✓→ALLOC[ReservePool]→RETURN[Page]→LOG[Success]→CALL[free_page]→CALL[spin_unlock]→DEC[PreemptCount]→RESTORE[RFLAGS]→RETURN[0]
 */
