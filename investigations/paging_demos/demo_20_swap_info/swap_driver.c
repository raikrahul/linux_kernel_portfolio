/*
 * DEMO 20: SWAP INFO
 * ══════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Page in RAM. RAM is full.
 *    Action: Evict page to Disk (SSD/HDD).
 *    Output: PTE marked "Prohibited" (P=0). Address points to Disk Sector.
 *
 *    Computation:
 *    P=0.
 *    Type (5 bits) = /dev/sda2.
 *    Offset (50 bits) = Sector 1,000,000.
 *
 * 2. WHY:
 *    - To extend RAM limit.
 *    - 16GB RAM + 32GB Swap = 48GB available memory.
 *    - Unused data (background tabs) shouldn't occupy precious RAM.
 *
 * 3. WHERE:
 *    - Swap Partition or Swap File.
 *    - PTE stores the location cookie.
 *
 * 4. WHO:
 *    - kswapd: Kernel thread wakes up when free memory low.
 *    - Page Fault Handler: Wakes up when user touches P=0 page.
 *
 * 5. WHEN:
 *    - Memory Pressure.
 *    - Hibernation (Suspend to Disk).
 *
 * 6. WITHOUT:
 *    - Out Of Memory (OOM) Killer triggers immediately on full RAM.
 *    - App Crashes.
 *
 * 7. WHICH:
 *    - Anonymous pages (Heap/Stack).
 *    - File pages are just dropped (re-read from file later), not swapped.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Self-Storage
 * - House (RAM) has 5 Rooms.
 * - You have 6 Sets of Furniture.
 *
 * Logic:
 * - Room 1: Bed.
 * - Room 2: Sofa.
 * ...
 * - Room 5: Desk.
 * - Set 6 (Old Piano): Doesn't fit.
 *
 * Action:
 * - Move Old Piano to Warehouse (Disk).
 * - Keep "Receipt" (PTE Swap Entry) in Room 5's drawer.
 * - Room 5 is now Empty.
 * - Put New Treadmill in Room 5.
 *
 * Access:
 * - Time to play Piano?
 * - Check Receipt. Go to Warehouse.
 * - Move Treadmill to Warehouse.
 * - Bring Piano back.
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 20: Swap Info");

static int demo_swap_show(struct seq_file* m, void* v) {
    struct sysinfo si;

    si_meminfo(&si);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 20: SWAP SUBSYSTEM\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "MEMORY STATISTICS:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Total RAM:  %lu pages = %lu MB\n", si.totalram, (si.totalram * 4) / 1024);
    seq_printf(m, "  Free RAM:   %lu pages = %lu MB\n", si.freeram, (si.freeram * 4) / 1024);
    seq_printf(m, "  Total swap: %lu pages = %lu MB\n", si.totalswap, (si.totalswap * 4) / 1024);
    seq_printf(m, "  Free swap:  %lu pages = %lu MB\n\n", si.freeswap, (si.freeswap * 4) / 1024);

    seq_printf(m, "SWAP PTE FORMAT (when P=0):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  bit 0:       0 (not present)\n");
    seq_printf(m, "  bits [1:5]:  swap type\n");
    seq_printf(m, "  bits [6:63]: swap offset\n\n");

    seq_printf(m, "SWAP-OUT:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  1. kswapd wakes when RAM low\n");
    seq_printf(m, "  2. Scan LRU for victims\n");
    seq_printf(m, "  3. Write to swap device\n");
    seq_printf(m, "  4. Update PTE: P=0, store swap loc\n");
    seq_printf(m, "  5. Free physical page\n\n");

    seq_printf(m, "SWAP-IN:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  1. Access swapped page → fault\n");
    seq_printf(m, "  2. Read swap loc from PTE\n");
    seq_printf(m, "  3. Alloc new page\n");
    seq_printf(m, "  4. Read from swap\n");
    seq_printf(m, "  5. Update PTE: P=1\n\n");

    seq_printf(m, "SEE: cat /proc/swaps, vmstat 1\n");

    return 0;
}

static int demo_swap_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_swap_show, NULL);
}

static const struct proc_ops demo_swap_ops = {
    .proc_open = demo_swap_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_swap_init(void) {
    proc_create("demo_swap", 0444, NULL, &demo_swap_ops);
    pr_info("demo_swap: loaded\n");
    return 0;
}

static void __exit demo_swap_exit(void) { remove_proc_entry("demo_swap", NULL); }

module_init(demo_swap_init);
module_exit(demo_swap_exit);
