/*
 * DEMO 11: DUMP PML4 TABLE
 * ════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: CR3 Register value.
 *    Action: Iterate 512 entries of the Top Level Table.
 *    Output: Map of the Virtual Address Space.
 *
 *    Computation:
 *    Table Size = 512 entries * 8 bytes = 4096 bytes (1 Page).
 *    Loop i = 0 to 511.
 *    If Entry[i] & 1 (Present): Print it.
 *
 * 2. WHY:
 *    - To visualize valid address ranges.
 *    - Valid ranges map to:
 *      - User Code/Data (Indices 0-255).
 *      - Kernel Code/Data/Modules (Indices 256-511).
 *
 * 3. WHERE:
 *    - Physical Address stored in CR3.
 *    - Virtual Address: Canonical Space (48-bit).
 *    - 0x0000... to 0x0000_7FFF_FFFF_FFFF (User).
 *    - 0xFFFF_8000_0000_0000 to 0xFFFF_FFFF_FFFF_FFFF (Kernel).
 *
 * 4. WHO:
 *    - OS (Linux) sets this up at boot (`pagetable_init`).
 *    - Process fork() copies Kernel part, creates new User part.
 *
 * 5. WHEN:
 *    - Debugging memory issues.
 *    - Analyzing KASLR shifts.
 *
 * 6. WITHOUT:
 *    - A debugging tool like this, you are blind to the address space layout.
 *    - You cannot verify if map corresponds to `/proc/PID/maps`.
 *
 * 7. WHICH:
 *    - Entries 0-255: User Space (Low half).
 *    - Entries 256-511: Kernel Space (High half).
 *    - Note: This split is hardware enforced (Canonical Address).
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Dictionary Volumes
 * - Encyclopedia has 26 Volumes (A-Z).
 * - User has Volume A-M (0-12).
 * - Admin has Volume N-Z (13-25).
 *
 * Input:
 * - Shelf has 26 slots.
 * - Empty slot = Missing Volume.
 * - Filled slot = Volume Present.
 *
 * Numerical:
 * - Slot 0 (A): Present (User App).
 * - Slot 1 (B): Empty.
 * ...
 * - Slot 25 (Z): Present (Kernel Trap Handler).
 *
 * Meaning:
 * - You can look up words starting with A.
 * - You can look up words starting with Z (if you are Admin).
 * - You cannot look up words starting with B.
 */

#include <asm/pgtable.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 11: Dump PML4");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

static void decode_flags(struct seq_file* m, unsigned long entry) {
    seq_printf(m, "[%c%c%c%c%c%c%c] ", (entry & _PAGE_NX) ? 'N' : '-',
               (entry & _PAGE_GLOBAL) ? 'G' : '-', (entry & _PAGE_PSE) ? 'H' : '-', /* Huge */
               (entry & _PAGE_ACCESSED) ? 'A' : '-', (entry & _PAGE_USER) ? 'U' : 'K',
               (entry & _PAGE_RW) ? 'W' : 'R', (entry & _PAGE_PRESENT) ? 'P' : '-');
}

static int demo_pml4_show(struct seq_file* m, void* v) {
    unsigned long cr3, pml4_phys;
    unsigned long* pml4_virt;
    int i;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    pml4_phys = cr3 & PTE_ADDR_MASK;
    pml4_virt = (unsigned long*)__va(pml4_phys);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 11: PML4 TABLE DUMP\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "CR3: 0x%lx\n", cr3);
    seq_printf(m, "Virt: 0x%lx\n\n", (unsigned long)pml4_virt);

    seq_printf(m, "IDX   ENTRY                FLAGS        MAPPED REGION (Start)\n");
    seq_printf(m, "─────────────────────────────────────────────────────────────\n");

    for (i = 0; i < 512; i++) {
        if (pml4_virt[i] & _PAGE_PRESENT) {
            /*
             * Calculate Virtual Address Start for this Index
             * VA = Index << 39
             * Must sign-extend if Index >= 256
             */
            unsigned long va_start = (unsigned long)i << 39;
            if (i >= 256) {
                va_start |= 0xFFFF000000000000UL;
            }

            seq_printf(m, "%3d:  0x%016lx   ", i, pml4_virt[i]);
            decode_flags(m, pml4_virt[i]);
            seq_printf(m, "0x%016lx\n", va_start);
        }
    }

    return 0;
}

static int demo_pml4_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_pml4_show, NULL);
}

static const struct proc_ops demo_pml4_ops = {
    .proc_open = demo_pml4_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_pml4_init(void) {
    proc_create("demo_pml4", 0444, NULL, &demo_pml4_ops);
    pr_info("demo_pml4: loaded\n");
    return 0;
}

static void __exit demo_pml4_exit(void) { remove_proc_entry("demo_pml4", NULL); }

module_init(demo_pml4_init);
module_exit(demo_pml4_exit);
