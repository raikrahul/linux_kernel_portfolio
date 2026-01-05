/*
 * DEMO 13: ZONE INFO
 * ══════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Physical RAM range.
 *    Action: Partition RAM into "Zones".
 *    Output: DMA, DMA32, Normal.
 *
 *    Data (x86_64):
 *    - DMA:    0 - 16 MB.
 *    - DMA32:  16 MB - 4 GB.
 *    - Normal: 4 GB - End of RAM.
 *
 * 2. WHY:
 *    - Hardware Limitations.
 *    - Legacy ISA devices can only address 24 bits (16MB).
 *    - 32-bit PCI devices can only address 32 bits (4GB).
 *    - Kernel prefers allocating from Normal to save lower zones for devices.
 *
 * 3. WHERE:
 *    - Physical RAM addresses.
 *
 * 4. WHO:
 *    - Boot Allocator.
 *    - Page Allocator (`alloc_pages` with `GFP_DMA` flags).
 *
 * 5. WHEN:
 *    - Driver requests memory for device transfers.
 *
 * 6. WITHOUT:
 *    - Sound card (Old) needs buffer.
 *    - Allocator gives pointer at 10GB.
 *    - Sound card writes to (10GB & 0xFFFFFF) = Chaos.
 *
 * 7. WHICH:
 *    - Which zone implies "Can my device reach this?".
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Parking Garage Height
 * - Ground Floor: 3 meters high (Trucks ok).
 * - Level 1: 2 meters high (Cars only).
 * - Level 2+: 2 meters high.
 *
 * Request:
 * - "I have a Truck" (Needs > 2.5m).
 * - Valet MUST park it on Ground Floor.
 * - If Ground Floor full -> Cannot Park.
 *
 * Request:
 * - "I have a Mini Cooper".
 * - Valet SHOULD park it on Level 2+ (Save Ground for Trucks).
 *
 * Numerical:
 * - Truck Height = 2.8m.
 * - Zone DMA Cap = 3.0m.
 * - Zone Normal Cap = 2.0m.
 * - 2.8 <= 3.0 (True) -> DMA.
 * - 2.8 <= 2.0 (False) -> Not Normal.
 */

#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sysinfo.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 13: Zone Info");

static int demo_zone_show(struct seq_file* m, void* v) {
    struct sysinfo si;
    unsigned long total_mb;

    si_meminfo(&si);
    total_mb = (si.totalram * si.mem_unit) / (1024 * 1024);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 13: MEMORY ZONES\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "SYSTEM MEMORY:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Total RAM: %lu MB\n", total_mb);
    seq_printf(m, "  Free RAM:  %lu MB\n\n", (si.freeram * si.mem_unit) / (1024 * 1024));

    seq_printf(m, "ZONE DEFINITIONS (x86_64 Standard):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    seq_printf(m, "1. ZONE_DMA (0 - 16 MB)\n");
    seq_printf(m, "   Size: 16 MB\n");
    seq_printf(m, "   Use: Legacy ISA devices (24-bit address limitation)\n\n");

    seq_printf(m, "2. ZONE_DMA32 (16 MB - 4 GB)\n");
    seq_printf(m, "   Size: ~4080 MB\n");
    seq_printf(m, "   Use: 32-bit PCI devices (32-bit address limitation)\n\n");

    seq_printf(m, "3. ZONE_NORMAL (4 GB - End of RAM)\n");
    if (total_mb > 4096) {
        seq_printf(m, "   Size: %lu MB\n", total_mb - 4096);
    } else {
        seq_printf(m, "   Size: 0 MB (System has < 4GB RAM)\n");
    }
    seq_printf(m, "   Use: All regular kernel/user usage\n\n");

    seq_printf(m, "4. ZONE_MOVABLE\n");
    seq_printf(m, "   Use: Hot-pluggable memory, defragmentation\n\n");

    seq_printf(m, "ALLOCATION STRATEGY:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Request(Normal): Try Normal -> DMA32 -> DMA\n");
    seq_printf(m, "  Request(DMA32):  Try DMA32 -> DMA\n");
    seq_printf(m, "  Request(DMA):    Try DMA\n");

    return 0;
}

static int demo_zone_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_zone_show, NULL);
}

static const struct proc_ops demo_zone_ops = {
    .proc_open = demo_zone_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_zone_init(void) {
    proc_create("demo_zone", 0444, NULL, &demo_zone_ops);
    pr_info("demo_zone: loaded\n");
    return 0;
}

static void __exit demo_zone_exit(void) { remove_proc_entry("demo_zone", NULL); }

module_init(demo_zone_init);
module_exit(demo_zone_exit);
