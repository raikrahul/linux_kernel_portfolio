/*
 * buddy_fragment.c - EXERCISE: Understand buddy allocator fragmentation
 *
 * AXIOMS FROM /proc/buddyinfo (THIS MACHINE):
 * A01. DMA32 order0=24301 → 24301×1×4096 = 99545088 bytes = 94.9 MB free in 4KB blocks
 * A02. DMA32 order1=13358 → 13358×2×4096 = 109445120 bytes = 104.3 MB free in 8KB blocks
 * A03. DMA32 order2=5390 → 5390×4×4096 = 88309760 bytes = 84.2 MB free in 16KB blocks
 * A04. Normal order0=14762 → 14762×1×4096 = 60448768 bytes = 57.6 MB
 * A05. Normal order1=16940 → 16940×2×4096 = 138690560 bytes = 132.3 MB
 * A06. MAX_ORDER=11, max block = 2^10 pages = 1024 pages = 4MB
 *
 * DEFINITIONS:
 * D01. ORDER = power of 2, order-N means 2^N contiguous pages
 * D02. order0 = 1 page = 4KB, order1 = 2 pages = 8KB, ..., order10 = 1024 pages = 4MB
 * D03. FRAGMENTATION = many small blocks, few large blocks → large allocation fails
 * D04. SPLITTING = no order-N available → take order-(N+1), split into two order-N
 * D05. COALESCING = after free, if buddy free → merge into order-(N+1)
 * D06. BUDDY ADDRESS = block_addr XOR (block_size_bytes)
 *
 * EXERCISE: ALLOCATE AND FREE TO CAUSE/OBSERVE FRAGMENTATION
 */

#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("user");
MODULE_DESCRIPTION("Exercise: buddy allocator fragmentation");

#define NUM_PAGES 16

static struct page* pages[NUM_PAGES];

/*
 * WORKSHEET LINE 01: ORDER TO BYTES CALCULATION (BY HAND)
 * order=0 → 2^0=1 page → 1×4096=4096 bytes=4KB ✓
 * order=1 → 2^1=2 pages → 2×4096=8192 bytes=8KB ✓
 * order=2 → 2^2=4 pages → 4×4096=16384 bytes=16KB ✓
 * order=3 → 2^3=8 pages → 8×4096=32768 bytes=32KB ✓
 * order=4 → 2^4=16 pages → 16×4096=65536 bytes=64KB ✓
 * order=5 → 2^5=32 pages → 32×4096=131072 bytes=128KB ✓
 * order=10 → 2^10=1024 pages → 1024×4096=4194304 bytes=4MB ✓
 */

/*
 * WORKSHEET LINE 02: BUDDYINFO INTERPRETATION (BY HAND)
 * Column N in buddyinfo = count of free blocks at order N
 * Total pages at order N = count × 2^N
 * DMA32 order0=24301 → 24301×1=24301 pages
 * DMA32 order1=13358 → 13358×2=26716 pages
 * DMA32 order2=5390 → 5390×4=21560 pages
 * Total DMA32 free = 24301+26716+21560+...= CALCULATE BY SUMMING ALL
 */

/*
 * WORKSHEET LINE 03: FRAGMENTATION SCENARIO
 * BEFORE: order2=5390 (5390 blocks of 16KB each)
 * ALLOCATE: 100 order-2 blocks → order2 count decreases by 100
 * AFTER: order2=5290
 * FREE ODD ONLY: free pages[1,3,5,...] → they can't coalesce (buddies still allocated)
 * RESULT: order2 increases by 50, but order3 stays same (no coalescing)
 */

static int __init buddy_fragment_init(void) {
    int i;
    unsigned long pfn;

    pr_info("=== buddy_fragment: EXERCISE START ===\n");
    pr_info("STEP 1: Check /proc/buddyinfo BEFORE allocations\n");

    /*
     * TODO BLOCK 1: ALLOCATE 16 ORDER-0 PAGES
     * ───────────────────────────────────────────────────────────────────
     * PREDICT: buddyinfo order0 count will decrease by 16
     * CALCULATE: if order0=24301 before, after = 24301 - 16 = 24285
     */
    for (i = 0; i < NUM_PAGES; i++) {
        pages[i] = alloc_page(GFP_KERNEL);
        if (!pages[i]) {
            pr_err("alloc_page[%d] failed\n", i);
            goto fail;
        }
        pfn = page_to_pfn(pages[i]);
        /* DRAW: page[i]@pfn=N → phys=N×4096=0x... → zone=DMA32/Normal */
        pr_info("pages[%d]: pfn=0x%lx, phys=0x%lx\n", i, pfn, pfn * 4096);
    }

    pr_info("STEP 2: Check /proc/buddyinfo AFTER allocations (order0 decreased by 16)\n");

    /*
     * TODO BLOCK 2: FREE ODD-INDEXED PAGES ONLY (CAUSES FRAGMENTATION)
     * ───────────────────────────────────────────────────────────────────
     * TRACE: free pages[1], pages[3], pages[5], ..., pages[15]
     * PREDICT: These cannot coalesce because their buddies (pages[0,2,4,...]) are still allocated
     * RESULT: order0 increases by 8, but order1 stays same
     */
    for (i = 1; i < NUM_PAGES; i += 2) {
        pfn = page_to_pfn(pages[i]);
        /*
         * BUDDY CALCULATION:
         * If page[i] at pfn=N, buddy at pfn = N XOR 1 (for order 0)
         * page[1] at pfn=M, buddy at pfn = M XOR 1
         * If M=0x1001, buddy = 0x1001 XOR 0x1 = 0x1000
         * Is buddy free? NO, pages[0] still allocated → no coalescing
         */
        pr_info("FREE pages[%d]: pfn=0x%lx, buddy_pfn=0x%lx (still allocated=✗ coalesce)\n", i, pfn,
                pfn ^ 1);
        __free_page(pages[i]);
        pages[i] = NULL;
    }

    pr_info("STEP 3: Check /proc/buddyinfo (order0 +8, order1 unchanged = FRAGMENTED)\n");

    /*
     * TODO BLOCK 3: NOW FREE EVEN-INDEXED PAGES
     * ───────────────────────────────────────────────────────────────────
     * TRACE: free pages[0], pages[2], pages[4], ..., pages[14]
     * PREDICT: Now buddies are free → COALESCING should happen
     * Each pair (0,1), (2,3), etc. can coalesce to order-1
     * But we already freed odd pages, so we're freeing the buddy now
     * RESULT: order1 increases (coalescing!)
     */
    for (i = 0; i < NUM_PAGES; i += 2) {
        if (pages[i]) {
            pfn = page_to_pfn(pages[i]);
            pr_info("FREE pages[%d]: pfn=0x%lx, buddy_pfn=0x%lx (already free=✓ coalesce)\n", i,
                    pfn, pfn ^ 1);
            __free_page(pages[i]);
            pages[i] = NULL;
        }
    }

    pr_info("STEP 4: Check /proc/buddyinfo (order1 increased = COALESCED)\n");
    pr_info("=== buddy_fragment: EXERCISE END ===\n");
    return 0;

fail:
    for (i = 0; i < NUM_PAGES; i++) {
        if (pages[i]) {
            __free_page(pages[i]);
            pages[i] = NULL;
        }
    }
    return -ENOMEM;
}

static void __exit buddy_fragment_exit(void) {
    int i;
    for (i = 0; i < NUM_PAGES; i++) {
        if (pages[i]) {
            __free_page(pages[i]);
            pages[i] = NULL;
        }
    }
    pr_info("buddy_fragment: module unloaded\n");
}

module_init(buddy_fragment_init);
module_exit(buddy_fragment_exit);

/*
 * FAILURE PREDICTIONS:
 * F1. User forgets order0 buddy = pfn XOR 1, order1 buddy = pfn XOR 2
 * F2. User expects coalescing when buddy is NOT free → no coalescing
 * F3. User confuses count with pages (count×2^order=pages)
 * F4. User doesn't check buddyinfo before/after → can't see fragmentation
 * F5. User forgets to calculate total bytes = count × 2^order × 4096
 * F6. User assumes all pages from same zone → GFP_KERNEL can use multiple zones
 *
 * VERIFICATION:
 * cat /proc/buddyinfo # before
 * sudo insmod buddy_fragment.ko && dmesg | tail -40 && sudo rmmod buddy_fragment
 * cat /proc/buddyinfo # after (should be same as before if all freed)
 */
