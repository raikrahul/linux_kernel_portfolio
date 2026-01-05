/*
 * DEMO 17: BUDDY ALLOCATOR
 * ═════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Order `n` (Power of 2).
 *    Action: Allocate contiguous block of 2^n pages.
 *    Output: Pointer to first page.
 *
 *    Computation:
 *    Order 0 = 2^0 = 1 Page  = 4 KB.
 *    Order 1 = 2^1 = 2 Pages = 8 KB.
 *    Order 9 = 2^9 = 512 Pages = 2 MB.
 *
 * 2. WHY:
 *    - External Fragmentation Solution.
 *    - If we just allocated 1 page at random spots, we could never find
 *      2 physically contiguous pages for a DMA buffer.
 *    - Buddy system keeps free memory in defined power-of-2 blocks.
 *
 * 3. WHERE:
 *    - Manages the entire `free_area` array in `struct zone`.
 *
 * 4. WHO:
 *    - `alloc_pages()`, `get_free_pages()`.
 *    - Underlying engine for Slab/Slub (which request Order-0 or Order-1 blocks).
 *
 * 5. WHEN:
 *    - Process fork (allocate stack/heap).
 *    - Device Driver initialization (Buffers).
 *
 * 6. WITHOUT:
 *    - System RAM sets would become "Swiss Cheese".
 *    - Total Free RAM = 1GB, but Max Contiguous Block = 4KB.
 *    - Failed Allocations.
 *
 * 7. WHICH:
 *    - Order 0 to MAX_ORDER (usually 11).
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Pizza Shop (The Buddy Logic)
 * - Inventory: 1 Giant Pizza (Order 3).
 *
 * Customer A: Wants Order 0 (1 Slice).
 * 1. Logic:
 *    - Have Order 3. Split -> Two Order 2s.
 *    - Have Order 2. Split -> Two Order 1s.
 *    - Have Order 1. Split -> Two Order 0s.
 *    - Serve one Order 0. Keep one Order 0 (Buddy).
 *
 * Customer B: Wants Order 0.
 * - Serve the kept Buddy.
 *
 * Return:
 * - A returns Slice. B returns Slice.
 * - Logic:
 *    - Are A and B buddies? Yes.
 *    - Merge -> Order 1.
 *    - Is Order 1 buddy free? Yes -> Merge -> Order 2...
 *    - Result: Back to Giant Pizza.
 */

#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 17: Buddy Allocator");

static int demo_buddy_show(struct seq_file* m, void* v) {
    struct page* pages[5];
    int orders[] = {0, 1, 2, 3, 4};
    int i;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 17: BUDDY ALLOCATOR\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "ORDER TABLE:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  order 0 = 2^0 = 1 page = 4 KB\n");
    seq_printf(m, "  order 1 = 2^1 = 2 pages = 8 KB\n");
    seq_printf(m, "  order 2 = 2^2 = 4 pages = 16 KB\n");
    seq_printf(m, "  order 3 = 2^3 = 8 pages = 32 KB\n");
    seq_printf(m, "  order 4 = 2^4 = 16 pages = 64 KB\n");
    seq_printf(m, "  order 10 = 2^10 = 1024 pages = 4 MB\n\n");

    seq_printf(m, "ALLOCATIONS:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    for (i = 0; i < 5; i++) {
        pages[i] = alloc_pages(GFP_KERNEL, orders[i]);
        if (pages[i]) {
            unsigned long pfn = page_to_pfn(pages[i]);
            unsigned long phys = pfn << PAGE_SHIFT;
            unsigned long size = (1 << orders[i]) * PAGE_SIZE;

            seq_printf(m, "order %d: %d pages = %lu bytes\n", orders[i], 1 << orders[i], size);
            seq_printf(m, "  PFN = %lu, phys = 0x%lx\n\n", pfn, phys);
        } else {
            seq_printf(m, "order %d: FAILED\n\n", orders[i]);
        }
    }

    /* Free all */
    for (i = 0; i < 5; i++) {
        if (pages[i]) __free_pages(pages[i], orders[i]);
    }

    seq_printf(m, "All pages freed (merged back into buddy system)\n\n");

    seq_printf(m, "SEE: cat /proc/buddyinfo\n");

    return 0;
}

static int demo_buddy_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_buddy_show, NULL);
}

static const struct proc_ops demo_buddy_ops = {
    .proc_open = demo_buddy_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_buddy_init(void) {
    proc_create("demo_buddy", 0444, NULL, &demo_buddy_ops);
    pr_info("demo_buddy: loaded\n");
    return 0;
}

static void __exit demo_buddy_exit(void) { remove_proc_entry("demo_buddy", NULL); }

module_init(demo_buddy_init);
module_exit(demo_buddy_exit);
