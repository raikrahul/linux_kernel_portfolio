/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 03: READ PAGE TABLE ENTRY
 * Machine: AMD Ryzen 5 4600H | page_offset_base = 0xFFFF89DF00000000
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ENTRY ADDRESS CALCULATION:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Formula: entry_addr = table_base + (index × 8)                        │
 * │                                                                        │
 * │ Each page table has 512 entries (2^9)                                 │
 * │ Each entry is 8 bytes (64 bits)                                       │
 * │ Table size = 512 × 8 = 4096 bytes = 1 page                           │
 * │ Valid indices: 0-511                                                  │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * WORKED EXAMPLE: PML4 base = 0x102FAB000, index = 255
 * ─────────────────────────────────────────────────────────────────────────
 * 01. byte_offset = 255 × 8 = 2040 = 0x7F8
 * 02. entry_phys = 0x102FAB000 + 0x7F8 = 0x102FAB7F8
 * 03. entry_virt = 0xFFFF89DF00000000 + 0x102FAB7F8 = 0xFFFF89E002FAB7F8
 * 04. entry_value = *(unsigned long *)0xFFFF89E002FAB7F8
 *
 * ADDITION TRACE FOR STEP 3:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │   0xFFFF89DF00000000                                                  │
 * │ + 0x0000000102FAB7F8                                                  │
 * │ ──────────────────────                                                │
 * │                                                                        │
 * │ Column-wise (right to left):                                          │
 * │ 0+8=8, 0+F=F, 0+7=7, 0+B=B, 0+A=A, 0+F=F, 0+2=2, 0+0=0               │
 * │ 0+1=1, F+0=F, D+0=D, 9+0=9, 8+0=8, F+0=F, F+0=F, F+0=F               │
 * │                                                                        │
 * │ Wait, let me redo properly:                                           │
 * │ Low 32: 00000000 + 02FAB7F8 = 02FAB7F8 (no overflow)                 │
 * │ Next 16: 89DF + 0001 = 89E0 (DF + 01 = E0, no carry)                 │
 * │ High 16: FFFF + 0000 = FFFF                                          │
 * │                                                                        │
 * │ Result: 0xFFFF89E002FAB7F8                                            │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * EDGE CASES:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Index 0:   offset = 0×8 = 0       → entry at table base               │
 * │ Index 511: offset = 511×8 = 4088  → entry at table_base + 0xFF8       │
 * │ Index 512: offset = 512×8 = 4096  → INVALID! Outside table!           │
 * │                                                                        │
 * │ Table layout:                                                          │
 * │ ┌────────────┐ base + 0x000                                           │
 * │ │ Entry[0]   │ 8 bytes                                                │
 * │ ├────────────┤ base + 0x008                                           │
 * │ │ Entry[1]   │ 8 bytes                                                │
 * │ ├────────────┤                                                        │
 * │ │ ...        │                                                        │
 * │ ├────────────┤ base + 0xFF8                                           │
 * │ │ Entry[511] │ 8 bytes                                                │
 * │ └────────────┘ base + 0x1000 (next page, not part of table)          │
 * └────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 03: Read Page Table Entry");

extern unsigned long page_offset_base;

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

static int demo_entry_show(struct seq_file* m, void* v) {
    unsigned long cr3, pml4_phys, pml4_virt;
    unsigned long entry, entry_addr_phys, entry_addr_virt;
    int indices[] = {0, 255, 256, 511};
    int i;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    pml4_phys = cr3 & PTE_ADDR_MASK;
    pml4_virt = (unsigned long)__va(pml4_phys);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 03: READ PAGE TABLE ENTRY\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    /*
     * Display calculation steps:
     *
     * CR3 = 0x102FAB000 (example)
     * PML4_phys = CR3 & 0x000FFFFFFFFFF000 = 0x102FAB000
     * PML4_virt = page_offset_base + 0x102FAB000
     *           = 0xFFFF89DF00000000 + 0x102FAB000
     *           = 0xFFFF89E002FAB000
     */
    seq_printf(m, "CR3 = 0x%lx\n", cr3);
    seq_printf(m, "PML4_phys = 0x%lx\n", pml4_phys);
    seq_printf(m, "page_offset_base = 0x%lx\n", page_offset_base);
    seq_printf(m, "PML4_virt = 0x%lx\n\n", pml4_virt);

    seq_printf(m, "ENTRY ADDRESS FORMULA:\n");
    seq_printf(m, "  entry_addr = PML4_virt + (index × 8)\n\n");

    /*
     * Read entries at specific indices and show calculation
     */
    for (i = 0; i < 4; i++) {
        int idx = indices[i];
        unsigned long offset;

        /*
         * Offset calculation:
         * For index 255: offset = 255 × 8 = 2040 = 0x7F8
         * For index 511: offset = 511 × 8 = 4088 = 0xFF8
         */
        offset = idx * 8;
        entry_addr_phys = pml4_phys + offset;
        entry_addr_virt = pml4_virt + offset;
        entry = *(unsigned long*)entry_addr_virt;

        seq_printf(m, "PML4[%3d]:\n", idx);
        seq_printf(m, "  offset = %d × 8 = %lu = 0x%lx\n", idx, offset, offset);
        seq_printf(m, "  entry_phys = 0x%lx + 0x%lx = 0x%lx\n", pml4_phys, offset, entry_addr_phys);
        seq_printf(m, "  entry_virt = 0x%lx\n", entry_addr_virt);
        seq_printf(m, "  *entry = 0x%016lx\n", entry);

        /*
         * Quick flag check:
         * P = entry & 1
         * If P=0, entry not valid (unmapped or swapped)
         */
        if (entry & 1) {
            unsigned long next_phys = entry & PTE_ADDR_MASK;
            seq_printf(m, "  → P=1, next_table_phys = 0x%lx\n\n", next_phys);
        } else {
            seq_printf(m, "  → P=0, not present\n\n");
        }
    }

    return 0;
}

static int demo_entry_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_entry_show, NULL);
}

static const struct proc_ops demo_entry_ops = {
    .proc_open = demo_entry_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_entry_init(void) {
    proc_create("demo_entry", 0444, NULL, &demo_entry_ops);
    pr_info("demo_entry: loaded\n");
    return 0;
}

static void __exit demo_entry_exit(void) { remove_proc_entry("demo_entry", NULL); }

module_init(demo_entry_init);
module_exit(demo_entry_exit);
