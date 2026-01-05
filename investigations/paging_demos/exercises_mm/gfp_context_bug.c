/*
 * gfp_context_bug.c - EXERCISE: GFP flag misuse in atomic context
 *
 * AXIOMS:
 * A01. GFP_KERNEL = 0xCC0 = __GFP_RECLAIM | __GFP_IO | __GFP_FS = CAN SLEEP
 * A02. GFP_ATOMIC = 0x0 = no flags = CANNOT SLEEP
 * A03. in_interrupt() = true when in hardirq/softirq context
 * A04. preempt_count bits: [0-7]=preempt, [8-15]=softirq, [16-19]=hardirq, [20]=NMI
 * A05. If GFP_KERNEL used in atomic context → "BUG: sleeping function called from invalid context"
 *
 * DEFINITIONS:
 * D01. ATOMIC CONTEXT = interrupts disabled or in interrupt handler = CANNOT SLEEP
 * D02. PROCESS CONTEXT = normal execution, can be preempted = CAN SLEEP
 * D03. GFP_KERNEL tries reclaim, IO, FS operations = SLEEPS if memory low
 * D04. GFP_ATOMIC returns immediately = fails if no memory available
 * D05. preempt_count > 0 means cannot schedule = atomic context
 *
 * EXERCISE: Trigger "sleeping in atomic context" warning, then fix with GFP_ATOMIC
 */

#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/preempt.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("user");
MODULE_DESCRIPTION("Exercise: GFP flag misuse in atomic context");

static DEFINE_SPINLOCK(test_lock);
static struct page* test_page = NULL;

/*
 * WORKSHEET LINE 01: GFP FLAG BITS (BY HAND)
 * __GFP_RECLAIM = 0x400 (bit 10) - can reclaim memory
 * __GFP_IO      = 0x040 (bit 6)  - can start disk IO
 * __GFP_FS      = 0x080 (bit 7)  - can call into filesystem
 * GFP_KERNEL = __GFP_RECLAIM | __GFP_IO | __GFP_FS
 *            = 0x400 | 0x040 | 0x080 = 0x4C0
 * NOTE: Actual value varies by kernel, check include/linux/gfp.h
 */

/*
 * WORKSHEET LINE 02: PREEMPT_COUNT BITS (BY HAND)
 * preempt_count() returns 32-bit value:
 * bits [0-7]   = preempt disable depth (0-255)
 * bits [8-15]  = softirq count
 * bits [16-19] = hardirq count
 * bit  [20]    = NMI flag
 *
 * If any of [8-20] set → in_interrupt() = true → atomic context
 * If [0-7] > 0 → preempt disabled → atomicish context
 *
 * EXAMPLE: preempt_count = 0x00010001
 *   bits [0-7] = 0x01 = preempt disabled once
 *   bits [16-19] = 0x01 = in hardirq
 *   → in_interrupt() = true, in_atomic() = true
 */

/*
 * WORKSHEET LINE 03: SPINLOCK EFFECT ON PREEMPT_COUNT
 * spin_lock(&lock):
 *   1. local_irq_save() or local_irq_disable() (on PREEMPT_RT or certain configs)
 *   2. preempt_disable() → preempt_count += 1
 *   3. Acquire spinlock
 *
 * After spin_lock: preempt_count[0-7] ≥ 1 → in_atomic() = true
 * GFP_KERNEL check: might_sleep() → if in_atomic() → WARNING
 */

static int __init gfp_context_bug_init(void) {
    unsigned int preempt_val;

    pr_info("=== gfp_context_bug: EXERCISE START ===\n");

    /* Step 1: Show we're in process context (can sleep) */
    preempt_val = preempt_count();
    pr_info("PROCESS CONTEXT: preempt_count=0x%x, in_interrupt=%d, in_atomic=%d\n", preempt_val,
            !!in_interrupt(), !!in_atomic());

    /*
     * CORRECT USAGE: GFP_KERNEL in process context
     * DRAW: preempt_count=0 → in_atomic=false → GFP_KERNEL allowed ✓
     */
    pr_info("Allocating with GFP_KERNEL in process context (CORRECT)...\n");
    test_page = alloc_page(GFP_KERNEL);
    if (test_page) {
        pr_info("SUCCESS: page=%px, pfn=0x%lx\n", test_page, page_to_pfn(test_page));
        __free_page(test_page);
        test_page = NULL;
    }

    /*
     * TODO BLOCK 1: TRIGGER BUG - GFP_KERNEL UNDER SPINLOCK
     * ───────────────────────────────────────────────────────────────────
     * TRACE preempt_count before: 0
     * CALL: spin_lock(&test_lock)
     * TRACE preempt_count after: 1 (preempt disabled)
     * CALL: alloc_page(GFP_KERNEL) ← BUG! might_sleep() will warn
     *
     * TO SEE THE BUG: Uncomment GFP_KERNEL line, comment GFP_ATOMIC line
     * TO FIX THE BUG: Use GFP_ATOMIC instead
     */
    pr_info("\n--- ENTERING ATOMIC CONTEXT (spinlock) ---\n");
    spin_lock(&test_lock);

    preempt_val = preempt_count();
    pr_info("UNDER SPINLOCK: preempt_count=0x%x, in_interrupt=%d, in_atomic=%d\n", preempt_val,
            !!in_interrupt(), !!in_atomic());
    pr_info("preempt_count bits: preempt_depth=%d\n", preempt_val & 0xFF);

    /*
     * BUG LINE (uncomment to trigger warning):
     * test_page = alloc_page(GFP_KERNEL);
     *
     * dmesg will show:
     * "BUG: sleeping function called from invalid context"
     * "in_atomic(): 1, irqs_disabled(): 0"
     */

    /* CORRECT: Use GFP_ATOMIC in atomic context */
    pr_info("Allocating with GFP_ATOMIC under spinlock (CORRECT)...\n");
    test_page = alloc_page(GFP_ATOMIC);
    if (test_page) {
        pr_info("SUCCESS: page=%px, pfn=0x%lx\n", test_page, page_to_pfn(test_page));
    } else {
        pr_info("GFP_ATOMIC failed (no immediately available memory)\n");
    }

    spin_unlock(&test_lock);
    pr_info("--- EXITED ATOMIC CONTEXT ---\n");

    preempt_val = preempt_count();
    pr_info("AFTER UNLOCK: preempt_count=0x%x, in_atomic=%d\n", preempt_val, !!in_atomic());

    /* Cleanup */
    if (test_page) {
        __free_page(test_page);
        test_page = NULL;
    }

    pr_info("=== gfp_context_bug: EXERCISE END ===\n");
    return 0;
}

static void __exit gfp_context_bug_exit(void) {
    if (test_page) {
        __free_page(test_page);
        test_page = NULL;
    }
    pr_info("gfp_context_bug: module unloaded\n");
}

module_init(gfp_context_bug_init);
module_exit(gfp_context_bug_exit);

/*
 * FAILURE PREDICTIONS:
 * F1. User uses GFP_KERNEL under spinlock → WARNING in dmesg
 * F2. User confuses GFP_KERNEL and GFP_ATOMIC values
 * F3. User doesn't check preempt_count → doesn't know why atomic
 * F4. User assumes spin_lock disables interrupts → depends on kernel config
 * F5. User forgets GFP_ATOMIC can fail → must check return value
 * F6. User doesn't know might_sleep() is what triggers the warning
 *
 * DERIVATION EXERCISE:
 * 1. CALCULATE: What preempt_count value after spin_lock_irq?
 *    Answer: preempt_disable (bits 0-7 += 1) + irqs disabled
 * 2. CALCULATE: If preempt_count = 0x00020003, what context?
 *    bits 0-7 = 3 → preempt disabled 3 times
 *    bits 16-19 = 2 → in hardirq (nested)
 *    → in_interrupt() = true, in_atomic() = true
 *
 * VERIFICATION:
 * sudo insmod gfp_context_bug.ko && dmesg | tail -30 && sudo rmmod gfp_context_bug
 */
