/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 05: CHECK HUGE PAGE BIT
 * Machine: AMD Ryzen 5 4600H | pdpe1gb flag ✓ (1GB pages supported)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * PS BIT (Page Size) - BIT 7 OF PAGE TABLE ENTRY
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ At L4 (PML4): Reserved, must be 0                                     │
 * │ At L3 (PDPT): If 1, entry maps a 1GB page (not table)                 │
 * │ At L2 (PD):   If 1, entry maps a 2MB page (not table)                 │
 * │ At L1 (PT):   Reserved/PAT bit, not page size indicator               │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * ENTRY = 0x80000002FAE001A1 ANALYSIS:
 * 01. Low byte: 0xA1 = 1010_0001 binary
 * 02. Bit 7 = (0xA1 >> 7) & 1 = (161 >> 7) & 1 = 1 & 1 = 1
 * 03. ∴ PS = 1 → This is a huge page entry (if at L2 or L3)
 *
 * PS EXTRACTION FORMULA:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ ps = (entry >> 7) & 1                                                 │
 * │                                                                        │
 * │ Example: entry = 0x1A1                                                │
 * │ 0x1A1 = 417 decimal = 0b110100001                                     │
 * │                              ↑                                         │
 * │                              bit 7                                     │
 * │ 417 >> 7 = 417 / 128 = 3 (integer division)                           │
 * │ 3 & 1 = 1                                                              │
 * │ ∴ PS = 1 ✓                                                            │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * ADDRESS MASKS BY PAGE SIZE:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ 4KB:  0x000FFFFFFFFFF000  (bits [51:12], 40 bits, offset = 12 bits)   │
 * │ 2MB:  0x000FFFFFFFE00000  (bits [51:21], 31 bits, offset = 21 bits)   │
 * │ 1GB:  0x000FFFFFC0000000  (bits [51:30], 22 bits, offset = 30 bits)   │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * WALK SHORTCUT DIAGRAM:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ 4KB Walk: CR3 → PML4 → PDPT → PD → PT → 4KB Page                     │
 * │           (4 table reads + 1 page access = 5 dereferences)            │
 * │                                                                        │
 * │ 2MB Walk: CR3 → PML4 → PDPT → PD(PS=1) → 2MB Page                    │
 * │           (3 table reads + 1 page access = 4 dereferences)            │
 * │           SAVE: 1 table read (~100ns RAM latency)                     │
 * │                                                                        │
 * │ 1GB Walk: CR3 → PML4 → PDPT(PS=1) → 1GB Page                         │
 * │           (2 table reads + 1 page access = 3 dereferences)            │
 * │           SAVE: 2 table reads (~200ns)                                │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * WORKED EXAMPLE: FIND 2MB HUGE PAGE IN KERNEL DIRECT MAP
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ VA = 0xFFFF89DF00000000 (page_offset_base)                            │
 * │                                                                        │
 * │ Indices:                                                               │
 * │ PML4 = (VA >> 39) & 0x1FF = 275 (kernel space, index > 255)           │
 * │ PDPT = (VA >> 30) & 0x1FF = 380                                       │
 * │ PD   = (VA >> 21) & 0x1FF = 0                                         │
 * │                                                                        │
 * │ Walk:                                                                  │
 * │ 1. PML4[275] → PDPT_phys (P=1, skip)                                  │
 * │ 2. PDPT[380] → check PS: if 1, 1GB page (unlikely for direct map)     │
 * │                if 0, get PD_phys                                       │
 * │ 3. PD[0] → check PS: if 1, 2MB page (EXPECTED for direct map!)        │
 * │            Physical = (entry & 0x000FFFFFFFE00000) | (VA & 0x1FFFFF)  │
 * └────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 05: Check Huge Page Bit");

extern unsigned long page_offset_base;

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL
#define HUGE_2MB_MASK 0x000FFFFFFFE00000UL
#define HUGE_1GB_MASK 0x000FFFFFC0000000UL

static int demo_huge_show(struct seq_file* m, void* v) {
    unsigned long cr3, pml4_phys, pdpt_phys, pd_phys;
    unsigned long *pml4, *pdpt, *pd;
    unsigned long entry;
    int i, j, k, huge_count = 0;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    pml4_phys = cr3 & PTE_ADDR_MASK;
    pml4 = (unsigned long*)__va(pml4_phys);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 05: HUGE PAGE DETECTION\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "FORMULA: is_huge = (entry >> 7) & 1\n");
    seq_printf(m, "page_offset_base = 0x%lx\n\n", page_offset_base);

    /*
     * Scan kernel space (PML4 indices 256-511)
     * Looking for PS=1 entries at PDPT and PD levels
     *
     * Memory access pattern:
     * For each present PML4[i]:
     *   Read PDPT table (512 × 8 = 4096 bytes)
     *   For each present PDPT[j]:
     *     If PS=1: found 1GB huge page
     *     Else: Read PD table
     *       For each present PD[k]:
     *         If PS=1: found 2MB huge page
     */
    seq_printf(m, "SCANNING KERNEL SPACE FOR HUGE PAGES:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    for (i = 256; i < 512 && huge_count < 10; i++) {
        if (!(pml4[i] & 1)) continue; /* P=0, skip */

        pdpt_phys = pml4[i] & PTE_ADDR_MASK;
        pdpt = (unsigned long*)__va(pdpt_phys);

        for (j = 0; j < 512 && huge_count < 10; j++) {
            entry = pdpt[j];
            if (!(entry & 1)) continue; /* P=0, skip */

            /*
             * Check 1GB huge page at PDPT level
             * PS = (entry >> 7) & 1
             *
             * If entry = 0x80000002FAE001A1:
             * 0xA1 >> 7 = 1, so PS = 1
             */
            if ((entry >> 7) & 1) {
                seq_printf(m, "1 GB HUGE @ PML4[%d] PDPT[%d]\n", i, j);
                seq_printf(m, "  entry = 0x%016lx\n", entry);
                seq_printf(m, "  PS = (0x%lx >> 7) & 1 = 1\n", entry & 0xFF);
                seq_printf(m, "  phys_base = entry & 0x%lx = 0x%lx\n", HUGE_1GB_MASK,
                           entry & HUGE_1GB_MASK);
                huge_count++;
                continue;
            }

            /* PS=0, descend to PD */
            pd_phys = entry & PTE_ADDR_MASK;
            pd = (unsigned long*)__va(pd_phys);

            for (k = 0; k < 512 && huge_count < 10; k++) {
                entry = pd[k];
                if (!(entry & 1)) continue;

                /*
                 * Check 2MB huge page at PD level
                 */
                if ((entry >> 7) & 1) {
                    seq_printf(m, "2 MB HUGE @ PML4[%d] PDPT[%d] PD[%d]\n", i, j, k);
                    seq_printf(m, "  entry = 0x%016lx\n", entry);
                    seq_printf(m, "  PS = (0x%lx >> 7) & 1 = 1\n", entry & 0xFF);
                    seq_printf(m, "  phys_base = entry & 0x%lx = 0x%lx\n", HUGE_2MB_MASK,
                               entry & HUGE_2MB_MASK);
                    huge_count++;
                    if (huge_count >= 10) break;
                }
            }
        }
    }

    seq_printf(m, "\nTotal huge pages found (limit 10): %d\n", huge_count);

    return 0;
}

static int demo_huge_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_huge_show, NULL);
}

static const struct proc_ops demo_huge_ops = {
    .proc_open = demo_huge_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_huge_init(void) {
    proc_create("demo_huge", 0444, NULL, &demo_huge_ops);
    pr_info("demo_huge: loaded\n");
    return 0;
}

static void __exit demo_huge_exit(void) { remove_proc_entry("demo_huge", NULL); }

module_init(demo_huge_init);
module_exit(demo_huge_exit);
