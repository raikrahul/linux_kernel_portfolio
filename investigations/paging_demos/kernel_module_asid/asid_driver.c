/*
 * LEGACY DRIVER: ASID / PCID
 * ══════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: CR3 Low Bits (0-11).
 *    Action: Read Process Context ID (PCID).
 *    Output: Tag for TLB entries.
 *
 * 2. WHY:
 *    - To allow TLB entries from multiple processes to coexist.
 *    - Process A (PCID 1) and Process B (PCID 2) both use VA 0x4000.
 *    - CPU checks PCID tag to match.
 *
 * 3. WHERE:
 *    - CR3 Register [11:0].
 *    - TLB Entry (Tag field).
 *
 * 4. WHO:
 *    - OS assigns them (usually limit 4096).
 *
 * 5. WHEN:
 *    - Context Switch.
 *
 * 6. WITHOUT:
 *    - Must flush ENTIRE TLB on every switch.
 *    - Performance kills (TLB Repopulation).
 *
 * 7. WHICH:
 *    - 12 bits = 4096 IDs.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: VIP Nightclub
 * - Club (TLB) has guests.
 * - Everyone has "Member ID" on forehead.
 *
 * Rule:
 * - "John" (Virtual Addr) is allowed. Wait, which John?
 * - John from Group A? Or John from Group B?
 *
 * Solution:
 * - Badge says "John | Group 5".
 * - Bouncer checks: "Are we hosting Group 5 tonight?" (CR3 says 5).
 * - Yes -> Enter.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Legacy ASID Driver");

static int demo_asid_show(struct seq_file* m, void* v) {
    unsigned long cr3;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "LEGACY ASID / PCID CHECK\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n");

    seq_printf(m, "CR3: 0x%lx\n", cr3);
    seq_printf(m, "PCID (Bits 0-11): 0x%lx\n", cr3 & 0xFFF);

    return 0;
}

static int demo_asid_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_asid_show, NULL);
}

static const struct proc_ops demo_asid_ops = {
    .proc_open = demo_asid_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_asid_init(void) {
    proc_create("legacy_asid", 0444, NULL, &demo_asid_ops);
    return 0;
}

static void __exit demo_asid_exit(void) { remove_proc_entry("legacy_asid", NULL); }

module_init(demo_asid_init);
module_exit(demo_asid_exit);
