/*
 * DEMO 14: PAGE FLAGS
 * ════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: struct page (Metadata for 1 Physical Frame).
 *    Action: Read `flags` field.
 *    Output: State of that 4KB chunk.
 *
 *    Bits:
 *    - Locked, Error, Referenced, Uptodate, Dirty, LRU, Active, Slab.
 *
 * 2. WHY:
 *    - RAM is just bytes. Kernel needs to trace *Lifecycle*.
 *    - Is this page valid? (Uptodate)
 *    - Has it been written to? (Dirty) -> Must write to disk before evicting.
 *    - Is it being used? (Referenced/Active) -> Keep in RAM.
 *    - Is it unused? (Inactive) -> Reclaim for other apps.
 *
 * 3. WHERE:
 *    - `struct page` array (`mem_map`).
 *    - Not in the page itself.
 *    - 32GB RAM = ~8 million pages = ~512MB metadata array.
 *
 * 4. WHO:
 *    - Page Reclaim Algorithm (LRU Scanner).
 *    - Filesystem (Marks Uptodate/Dirty).
 *
 * 5. WHEN:
 *    - Every time page is touched, allocated, or freed.
 *
 * 6. WITHOUT:
 *    - Kernel wouldn't know which pages are junk and which are precious data.
 *    - Would discard modified data (Data Loss).
 *
 * 7. WHICH:
 *    - Which specific flags? A huge bitmap inside `unsigned long flags`.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Library Book Stickers
 * - Book Content = 4KB Data.
 * - Cover = struct page.
 *
 * Stickers (Flags):
 * - Red Dot: "Active" (Popular).
 * - Blue Dot: "Dirty" (Scribbled notes, needs copying to archive).
 * - Yellow Dot: "Locked" (Someone is currently reading it).
 *
 * Logic:
 * - Librarian (Kernel) needs space for new books.
 * - Scans shelf.
 * - Finds Book A: Red Dot. Keep it.
 * - Finds Book B: No Dot. Evict?
 * - Check Blue Dot: Yes? Provide copy to Archive first (Writeback).
 * - Check Yellow Dot: Yes? Can't touch it right now.
 */

#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 14: Page Flags");

static int demo_flags_show(struct seq_file* m, void* v) {
    struct page* page;
    unsigned long pfn;
    unsigned long flags;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 14: PAGE FLAGS\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "FLAG BITS (from include/linux/page-flags.h):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  PG_locked = page is locked\n");
    seq_printf(m, "  PG_referenced = recently accessed\n");
    seq_printf(m, "  PG_uptodate = data is valid\n");
    seq_printf(m, "  PG_dirty = needs writeback\n");
    seq_printf(m, "  PG_lru = on LRU list\n");
    seq_printf(m, "  PG_active = on active list\n");
    seq_printf(m, "  PG_slab = slab allocator\n");
    seq_printf(m, "  PG_hugetlb = huge page\n\n");

    /* Sample a few PFNs */
    seq_printf(m, "SAMPLE PAGES:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    unsigned long sample_pfns[] = {0, 1, 256, 1024, 65536, 262144};
    int i;

    for (i = 0; i < 6; i++) {
        pfn = sample_pfns[i];
        if (!pfn_valid(pfn)) {
            seq_printf(m, "PFN %lu: invalid\n", pfn);
            continue;
        }

        page = pfn_to_page(pfn);
        flags = page->flags;

        seq_printf(m, "PFN %6lu: flags=0x%016lx\n", pfn, flags);
        seq_printf(m, "  phys = 0x%lx\n", pfn << 12);
        seq_printf(m, "  locked=%d uptodate=%d dirty=%d\n", PageLocked(page) ? 1 : 0,
                   PageUptodate(page) ? 1 : 0, PageDirty(page) ? 1 : 0);
        seq_printf(m, "  lru=%d slab=%d reserved=%d\n\n", PageLRU(page) ? 1 : 0,
                   PageSlab(page) ? 1 : 0, PageReserved(page) ? 1 : 0);
    }

    return 0;
}

static int demo_flags_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_flags_show, NULL);
}

static const struct proc_ops demo_flags_ops = {
    .proc_open = demo_flags_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_flags_init(void) {
    proc_create("demo_flags", 0444, NULL, &demo_flags_ops);
    pr_info("demo_flags: loaded\n");
    return 0;
}

static void __exit demo_flags_exit(void) { remove_proc_entry("demo_flags", NULL); }

module_init(demo_flags_init);
module_exit(demo_flags_exit);
