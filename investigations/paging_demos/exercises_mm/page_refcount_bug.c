/*
 * page_refcount_bug.c - EXERCISE: Find and fix refcount underflow bug
 *
 * AXIOMS FROM THIS MACHINE:
 * A01. PAGE_SIZE=4096=2^12=0x1000 (getconf PAGE_SIZE)
 * A02. MemTotal=15776276 kB=15406 MB (cat /proc/meminfo)
 * A03. nr_free_pages=324735 (cat /proc/vmstat)
 * A04. vmemmap=0xFFFFEA0000000000 (arch/x86/include/asm/pgtable_64_types.h)
 * A05. PAGE_OFFSET=0xFFFF888000000000 (arch/x86/include/asm/page_64.h)
 * A06. sizeof(struct page)=64 bytes (pahole struct page)
 * A07. offsetof(struct page, _refcount)=52 bytes
 * A08. offsetof(struct page, flags)=0 bytes
 * A09. DMA zone: PFN 0 to 4096 (0 to 16MB)
 * A10. DMA32 zone: PFN 4096 to 1048576 (16MB to 4GB)
 * A11. Normal zone: PFN 1048576+ (4GB+)
 * A12. buddyinfo DMA32: order0=24301, order1=13358, order2=5390...
 * A13. buddyinfo Normal: order0=14762, order1=16940, order2=2774...
 *
 * DEFINITIONS (BEFORE USE):
 * D01. RAM=bytes on motherboard, address 0,1,2,...
 * D02. PAGE=4096 contiguous bytes, WHY: managing billions of bytes one-by-one slow
 * D03. PFN=page frame number=physical_address/4096, WHY: kernel allocates pages not bytes
 * D04. struct page=64 bytes metadata per page, WHY: kernel needs to track usage
 * D05. _refcount=atomic counter, WHY: prevent freeing page while in use
 * D06. alloc_page()→_refcount=1, get_page()→_refcount++, put_page()→_refcount--, if 0 then free
 * D07. BUG: put_page() when _refcount=0 → underflow → _refcount=-1 → kernel BUG_ON
 *
 * EXERCISE STRUCTURE:
 * - BOILERPLATE: all includes, module macros, printk skeleton PROVIDED
 * - TODO BLOCKS: user fills allocation calls, get_page/put_page sequence
 * - BUG INJECTION: extra put_page causes underflow
 * - USER TASK: trace _refcount by hand, predict BUG_ON, then remove extra put_page
 */

#include <linux/atomic.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("user");
MODULE_DESCRIPTION("Exercise: page refcount underflow bug");

static struct page* test_page = NULL;

/*
 * WORKSHEET LINE 01: alloc_page(GFP_KERNEL) returns page→CALCULATE page address from vmemmap
 * page_addr = vmemmap + PFN × 64 = 0xFFFFEA0000000000 + PFN × 64
 * If PFN=0x1000=4096, page_addr = 0xFFFFEA0000000000 + 4096 × 64 = 0xFFFFEA0000000000 + 262144
 * = 0xFFFFEA0000000000 + 0x40000 = 0xFFFFEA0000040000
 * phys = PFN × 4096 = 4096 × 4096 = 16777216 = 0x1000000 = 16 MB
 * zone = DMA32 (4096 ≥ 4096 ✓, 4096 < 1048576 ✓)
 */

/*
 * WORKSHEET LINE 02: after alloc_page, _refcount=___? FILL: 1
 * atomic_read(&page->_refcount) = 1
 * offset in struct page = 52 bytes from base
 * raw address = page + 52 = 0xFFFFEA0000040000 + 52 = 0xFFFFEA0000040034
 */

/*
 * WORKSHEET LINE 03: get_page(page) increments _refcount
 * _refcount: 1 → 1+1 = 2
 * atomic_inc(&page->_refcount) = adds 1 atomically
 */

/*
 * WORKSHEET LINE 04: put_page(page) decrements _refcount
 * _refcount: 2 → 2-1 = 1
 * atomic_dec_and_test(&page->_refcount) → dec to 1, test if 0? 1≠0 ✗ → do not free
 */

/*
 * WORKSHEET LINE 05: put_page(page) again
 * _refcount: 1 → 1-1 = 0
 * atomic_dec_and_test → dec to 0, test if 0? 0=0 ✓ → FREE PAGE
 */

/*
 * WORKSHEET LINE 06: put_page(page) THIRD TIME (BUG!)
 * _refcount: 0 → 0-1 = -1 = 0xFFFFFFFF (underflow on unsigned)
 * atomic_dec_and_test → dec to -1, test if 0? -1≠0 ✗
 * BUT page already freed → BUG_ON in kernel → dmesg shows:
 * "BUG: Bad page state" or "page_ref_sub_and_test: refcount error"
 */

static int __init refcount_bug_init(void) {
    unsigned long pfn;
    int refcount;

    pr_info("=== page_refcount_bug: EXERCISE START ===\n");

    /* DRAW BEFORE alloc_page: nr_free_pages=324735, order0_DMA32=24301, order0_Normal=14762 */

    /*
     * TODO BLOCK 1: ALLOCATE PAGE
     * ───────────────────────────────────────────────────────────────────
     * CALCULATE: GFP_KERNEL = __GFP_RECLAIM | __GFP_IO | __GFP_FS = 0xCC0
     * PREDICT: page from Normal zone (first choice), or DMA32 (fallback)
     * FILL IN: test_page = alloc_page(GFP_KERNEL);
     * VERIFY: test_page != NULL, otherwise allocation failed
     */
    test_page = alloc_page(GFP_KERNEL);
    if (!test_page) {
        pr_err("alloc_page failed\n");
        return -ENOMEM;
    }

    /* DRAW AFTER alloc_page: page@addr, _refcount=1, flags=0x... */
    pfn = page_to_pfn(test_page);
    refcount = atomic_read(&test_page->_refcount);

    /*
     * CALCULATE BY HAND:
     * pfn = (page - vmemmap) / sizeof(struct page) = (page - 0xFFFFEA0000000000) / 64
     * phys = pfn × 4096
     * zone: if pfn < 4096 → DMA, else if pfn < 1048576 → DMA32, else Normal
     */
    pr_info("page=%px, pfn=0x%lx=%lu, phys=0x%lx\n", test_page, pfn, pfn, pfn * 4096);
    pr_info("_refcount=%d (EXPECTED: 1, ACTUAL: %d, MATCH: %s)\n", refcount, refcount,
            refcount == 1 ? "✓" : "✗");

    /* ZONE DERIVATION: pfn=N, N < 4096? → DMA. N < 1048576? → DMA32. else Normal */
    if (pfn < 4096)
        pr_info("zone=DMA (pfn=%lu < 4096 ✓)\n", pfn);
    else if (pfn < 1048576)
        pr_info("zone=DMA32 (pfn=%lu ≥ 4096 ✓, < 1048576 ✓)\n", pfn);
    else
        pr_info("zone=Normal (pfn=%lu ≥ 1048576 ✓)\n", pfn);

    /*
     * TODO BLOCK 2: GET_PAGE (increment refcount)
     * ───────────────────────────────────────────────────────────────────
     * TRACE: _refcount before: ___ (FILL: 1)
     * CALL: get_page(test_page);
     * TRACE: _refcount after: ___ (FILL: 2)
     * ARITHMETIC: 1 + 1 = 2 ✓
     */
    pr_info("BEFORE get_page: _refcount=%d\n", atomic_read(&test_page->_refcount));
    get_page(test_page);
    refcount = atomic_read(&test_page->_refcount);
    pr_info("AFTER get_page: _refcount=%d (EXPECTED: 2, ACTUAL: %d, MATCH: %s)\n", refcount,
            refcount, refcount == 2 ? "✓" : "✗");

    /*
     * TODO BLOCK 3: FIRST PUT_PAGE
     * ───────────────────────────────────────────────────────────────────
     * TRACE: _refcount before: 2
     * CALL: put_page(test_page);
     * TRACE: _refcount after: ___ (FILL: 1)
     * ARITHMETIC: 2 - 1 = 1 ✓
     * FREE? atomic_dec_and_test returns (1 == 0) = false → NOT freed
     */
    pr_info("BEFORE first put_page: _refcount=%d\n", atomic_read(&test_page->_refcount));
    put_page(test_page);
    refcount = atomic_read(&test_page->_refcount);
    pr_info("AFTER first put_page: _refcount=%d (EXPECTED: 1, ACTUAL: %d, MATCH: %s)\n", refcount,
            refcount, refcount == 1 ? "✓" : "✗");

    /*
     * TODO BLOCK 4: SECOND PUT_PAGE (releases page)
     * ───────────────────────────────────────────────────────────────────
     * TRACE: _refcount before: 1
     * CALL: put_page(test_page);
     * TRACE: _refcount after: 0 → PAGE FREED
     * ARITHMETIC: 1 - 1 = 0 ✓
     * FREE? atomic_dec_and_test returns (0 == 0) = true → FREED!
     * WARNING: test_page pointer now DANGLING
     */
    pr_info("BEFORE second put_page: _refcount=%d\n", atomic_read(&test_page->_refcount));
    put_page(test_page);
    pr_info("AFTER second put_page: page FREED, pointer DANGLING\n");

    /*
     * BUG BLOCK: THIRD PUT_PAGE (CAUSES UNDERFLOW)
     * ───────────────────────────────────────────────────────────────────
     * TRACE: _refcount before: 0 (or undefined, page freed)
     * CALL: put_page(test_page); ← THIS IS THE BUG
     * EXPECTED: BUG_ON or kernel panic
     *
     * COMMENT OUT THE LINE BELOW TO FIX THE BUG:
     */
    /* put_page(test_page); */ /* ← UNCOMMENT TO TRIGGER BUG, RECOMMENT TO FIX */

    test_page = NULL; /* Mark as freed to prevent double-free in exit */

    pr_info("=== page_refcount_bug: EXERCISE END (no BUG triggered) ===\n");
    return 0;
}

static void __exit refcount_bug_exit(void) {
    /*
     * If test_page is not NULL, we have a leak (forgot to free)
     * If test_page is NULL, either freed correctly or never allocated
     */
    if (test_page) {
        pr_warn("LEAK: test_page not freed, freeing now\n");
        put_page(test_page);
    }
    pr_info("page_refcount_bug: module unloaded\n");
}

module_init(refcount_bug_init);
module_exit(refcount_bug_exit);

/*
 * FAILURE PREDICTIONS:
 * F1. User forgets _refcount starts at 1 after alloc_page → miscounts
 * F2. User confuses get_page (add ref) with alloc_page (create ref)
 * F3. User forgets put_page frees when _refcount reaches 0
 * F4. User uses page after put_page freed it → use-after-free
 * F5. User uncomments bug line → kernel BUG_ON, must reboot
 * F6. User doesn't trace _refcount by hand → doesn't understand bug
 * F7. User assumes _refcount is signed → it's atomic_t (effectively signed, but kernel expects ≥0)
 *
 * VERIFICATION COMMANDS:
 * sudo insmod page_refcount_bug.ko && dmesg | tail -30 && sudo rmmod page_refcount_bug
 * cat /proc/vmstat | grep nr_free_pages (before and after)
 * cat /proc/buddyinfo (before and after)
 */
