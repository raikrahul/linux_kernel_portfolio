/*
 * DEMO 06: EXTRACT PHYSICAL ADDRESS FROM ENTRY
 * ═════════════════════════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input:  Entry = 0x80000002FAE001A1
 *    Action: Zero out Flags (bits 0-11, 52-63)
 *    Output: 0x2FAE00000 (Pure Physical Address)
 *
 *    Computation (4KB):
 *    Mask = 0x000FFFFFFFFFF000
 *    0x80000002FAE001A1
 *  & 0x000FFFFFFFFFF000
 *    ──────────────────
 *    0x00000002FAE00000 ✓
 *
 * 2. WHY:
 *    - The Entry is a packed struct: [NX|...|Address|...|Flags].
 *    - If we use Entry as a pointer directly:
 *      Pointer = 0x80000002FAE001A1
 *      NX bit (63) is set -> Valid in Kernel (maybe), but high bits wrong.
 *      Flags (0-11) set -> Misaligned address.
 *    - Must isolate the Address field.
 *
 * 3. WHERE:
 *    - Bits [51:12] for 4KB Page Table Pointer.
 *    - Bits [51:21] for 2MB Huge Page.
 *    - Bits [51:30] for 1GB Huge Page.
 *
 * 4. WHO:
 *    - Software (Driver): Must apply mask explicitly.
 *    - Hardware (MMU): Ignores flags wires when driving address bus.
 *
 * 5. WHEN:
 *    - Before calling `__va()` (Virtual Address macro).
 *    - Before dereferencing the next table.
 *
 * 6. WITHOUT:
 *    - `__va(0x80000002FAE001A1)`
 *    - Adds Offset base to GARBAGE.
 *    - Result: Points to nowhere -> Crash.
 *
 * 7. WHICH:
 *    - 4KB Mask: Clear low 12 bits (FFF).
 *    - 2MB Mask: Clear low 21 bits (1FFFFF).
 *    - 1GB Mask: Clear low 30 bits (3FFFFFFF).
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Dirty Money
 * - A Banknote has Serial Number (Value) and Graffiti (Flags).
 * - Note: "888-123456-XXX"
 * - 888 = Reserved Prefix (NX bit).
 * - XXX = Dirt/Stamp (Flags).
 * - 123456 = Value.
 *
 * Action:
 * - To use the serial number, you must ERASE the graffiti.
 * - Mask: "000-111111-000".
 * - Apply Mask -> "000-123456-000" -> 123456.
 *
 * Numerical Analogy:
 * - Value = 123456.
 * - Noise = 900000 (Prefix) + 7 (Suffix).
 * - Dirty = 900123463.
 * - Mask = 000111110.
 * - Clean = 000123460? No, digit masking.
 * - 900123463 % 1000000 = 123463.
 * - 123463 / 10 = 12346.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 06: Extract Physical Address");

#define MASK_4KB 0x000FFFFFFFFFF000UL
#define MASK_2MB 0x000FFFFFFFE00000UL
#define MASK_1GB 0x000FFFFFC0000000UL

static int demo_addr_show(struct seq_file* m, void* v) {
    unsigned long test_entry = 0x80000002FAE001A1UL;
    unsigned long addr_4kb, addr_2mb, addr_1gb;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 06: EXTRACT PHYSICAL ADDRESS\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "TEST ENTRY: 0x%016lx\n\n", test_entry);

    /* 4 KB mask */
    addr_4kb = test_entry & MASK_4KB;
    seq_printf(m, "CASE 1: 4 KB PAGE (table pointer)\n");
    seq_printf(m, "  Assuming PS=0. Mask bits [51:12].\n");
    seq_printf(m, "  0x%lx\n", test_entry);
    seq_printf(m, "& 0x%lx\n", MASK_4KB);
    seq_printf(m, "  ──────────────────\n");
    seq_printf(m, "  0x%lx\n\n", addr_4kb);

    /* 2 MB mask */
    addr_2mb = test_entry & MASK_2MB;
    seq_printf(m, "CASE 2: 2 MB HUGE PAGE\n");
    seq_printf(m, "  Assuming PS=1 at PD. Mask bits [51:21].\n");
    seq_printf(m, "  0x%lx\n", test_entry);
    seq_printf(m, "& 0x%lx\n", MASK_2MB);
    seq_printf(m, "  ──────────────────\n");
    seq_printf(m, "  0x%lx\n\n", addr_2mb);

    /* 1 GB mask */
    addr_1gb = test_entry & MASK_1GB;
    seq_printf(m, "CASE 3: 1 GB HUGE PAGE\n");
    seq_printf(m, "  Assuming PS=1 at PDPT. Mask bits [51:30].\n");
    seq_printf(m, "  0x%lx\n", test_entry);
    seq_printf(m, "& 0x%lx\n", MASK_1GB);
    seq_printf(m, "  ──────────────────\n");
    seq_printf(m, "  0x%lx\n\n", addr_1gb);

    seq_printf(m, "MASK DERIVATION AXIOMS:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "1. Address max width = 52 bits (Architecture Limit).\n");
    seq_printf(m, "2. 4KB alignment = Low 12 bits are 0.\n");
    seq_printf(m, "   ∴ Mask top 12 (Reserved) and bottom 12 (Flags).\n");
    seq_printf(m, "   Inverse Mask: ~0xFFF = ...111000\n");

    return 0;
}

static int demo_addr_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_addr_show, NULL);
}

static const struct proc_ops demo_addr_ops = {
    .proc_open = demo_addr_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_addr_init(void) {
    proc_create("demo_addr", 0444, NULL, &demo_addr_ops);
    pr_info("demo_addr: loaded\n");
    return 0;
}

static void __exit demo_addr_exit(void) { remove_proc_entry("demo_addr", NULL); }

module_init(demo_addr_init);
module_exit(demo_addr_exit);
