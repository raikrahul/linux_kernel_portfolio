/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 07: FULL 4KB PAGE WALK
 * Machine: AMD Ryzen 5 4600H | page_offset_base=0xFFFF89DF00000000
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * PAGE WALK ALGORITHM:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ 1. CR3 → PML4 physical base                                           │
 * │ 2. PML4[(VA>>39)&0x1FF] → PDPT physical base (if P=1)                 │
 * │ 3. PDPT[(VA>>30)&0x1FF] → PD physical base (if P=1, PS=0)             │
 * │    OR final physical (if P=1, PS=1 → 1GB huge page)                   │
 * │ 4. PD[(VA>>21)&0x1FF] → PT physical base (if P=1, PS=0)               │
 * │    OR final physical (if P=1, PS=1 → 2MB huge page)                   │
 * │ 5. PT[(VA>>12)&0x1FF] → Page physical base (if P=1)                   │
 * │ 6. Final = Page_base | (VA & 0xFFF)                                   │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * WORKED EXAMPLE: VA = 0xFFFFFFFF81000000 (kernel text)
 * ─────────────────────────────────────────────────────────────────────────
 * Index extraction:
 * ┌──────────────────────────────────────────────────────────────────────┐
 * │ VA = 0xFFFFFFFF81000000                                              │
 * │ Binary breakdown:                                                    │
 * │ FFFF = 1111111111111111 (bits 63-48)                                │
 * │ FFFF = 1111111111111111 (bits 47-32)                                │
 * │ 8100 = 1000000100000000 (bits 31-16)                                │
 * │ 0000 = 0000000000000000 (bits 15-0)                                 │
 * │                                                                      │
 * │ PML4 [47:39]: bits 47-44=1111, 43-40=1111, 39=1                     │
 * │ = 111111111 = 0x1FF = 511                                           │
 * │                                                                      │
 * │ PDPT [38:30]: bits 38-36=000 (from 8=1000, low 3), 35-32=0001 (1),  │
 * │              31-30=00                                                │
 * │ = 000000100 = 0x004 = 4                                             │
 * │                                                                      │
 * │ PD [29:21]: 0x81000000 >> 21 = 0x408                                │
 * │ 0x408 & 0x1FF = 0x008 = 8                                           │
 * │                                                                      │
 * │ PT [20:12]: 0x81000000 >> 12 = 0x81000                              │
 * │ 0x81000 & 0x1FF = 0x000 = 0                                         │
 * │                                                                      │
 * │ OFF [11:0]: 0x81000000 & 0xFFF = 0x000 = 0                          │
 * └──────────────────────────────────────────────────────────────────────┘
 *
 * WALK TRACE (sample data):
 * ┌──────────────────────────────────────────────────────────────────────┐
 * │ CR3 = 0x0000000102FAB000 (from hardware)                            │
 * │ PML4_phys = 0x102FAB000                                             │
 * │ PML4_virt = 0xFFFF89E002FAB000                                      │
 * │                                                                      │
 * │ L4: PML4[511] at 0xFFFF89E002FAB000 + 511×8 = 0xFFFF89E002FABFF8   │
 * │     Entry = 0x80000001030AB067 (example)                            │
 * │     P = 1 ✓, Extract next = 0x1030AB000                            │
 * │                                                                      │
 * │ L3: PDPT @ 0xFFFF89E0030AB000, [4] at +32 = 0xFFFF89E0030AB020     │
 * │     Entry = 0x80000001040CD067 (example)                            │
 * │     P = 1 ✓, PS = 0 → continue                                     │
 * │     Next = 0x1040CD000                                              │
 * │                                                                      │
 * │ L2: PD @ 0xFFFF89E0040CD000, [8] at +64 = 0xFFFF89E0040CD040       │
 * │     Entry = 0x80000001050EF067 (example)                            │
 * │     P = 1 ✓, PS = 0 → continue                                     │
 * │     Next = 0x1050EF000                                              │
 * │                                                                      │
 * │ L1: PT @ 0xFFFF89E0050EF000, [0] at +0 = 0xFFFF89E0050EF000        │
 * │     Entry = 0x8000000106123163 (example)                            │
 * │     P = 1 ✓                                                         │
 * │     Page_phys = 0x106123000                                         │
 * │                                                                      │
 * │ FINAL: 0x106123000 | 0x000 = 0x106123000                           │
 * │ Verify: __va(0x106123000) should contain kernel instructions       │
 * └──────────────────────────────────────────────────────────────────────┘
 *
 * MEMORY READ COUNT:
 * 01. Read PML4[511]: 8 bytes from RAM
 * 02. Read PDPT[4]: 8 bytes from RAM
 * 03. Read PD[8]: 8 bytes from RAM
 * 04. Read PT[0]: 8 bytes from RAM
 * TOTAL: 4 reads × ~100ns = 400ns per translation without TLB
 * WITH TLB HIT: ~1ns (cache lookup)
 * SPEEDUP: 400×
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 07: Full 4KB Page Walk");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

/*
 * walk_va: Perform and trace a full page table walk
 *
 * Memory access pattern for VA = 0xFFFFFFFF81000000:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ ACCESS 1: *(PML4_virt + 511×8) → reads entry from RAM                 │
 * │           If entry = 0x80000001030AB067:                               │
 * │           P = 0x67 & 1 = 1 ✓                                          │
 * │           PDPT_phys = 0x80000001030AB067 & 0x000FFFFFFFFFF000          │
 * │                     = 0x00000001030AB000                               │
 * │                                                                        │
 * │ ACCESS 2: *(PDPT_virt + 4×8) = *(PDPT_virt + 32)                      │
 * │           Check PS bit = (entry >> 7) & 1                             │
 * │           PS = 0 → ordinary table, continue                           │
 * │           PS = 1 → 1GB huge page, stop (calculate differently)        │
 * │                                                                        │
 * │ (similar for ACCESS 3 and 4)                                          │
 * └────────────────────────────────────────────────────────────────────────┘
 */
static void walk_va(struct seq_file* m, unsigned long va) {
    unsigned long cr3, table_phys, entry;
    unsigned long pml4_idx, pdpt_idx, pd_idx, pt_idx, offset;

    /*
     * Step 0: Read CR3
     * Machine code: 0F 20 D8 (mov rax, cr3) - 3 bytes
     * Privilege: Ring 0 only
     */
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    table_phys = cr3 & PTE_ADDR_MASK;

    /*
     * Step 1: Extract all indices
     *
     * For VA = 0xFFFFFFFF81000000:
     * pml4_idx = (0xFFFFFFFF81000000 >> 39) & 0x1FF = 511
     * pdpt_idx = (0xFFFFFFFF81000000 >> 30) & 0x1FF = 4
     * pd_idx   = (0xFFFFFFFF81000000 >> 21) & 0x1FF = 8
     * pt_idx   = (0xFFFFFFFF81000000 >> 12) & 0x1FF = 0
     * offset   = 0xFFFFFFFF81000000 & 0xFFF = 0
     */
    pml4_idx = (va >> 39) & 0x1FF;
    pdpt_idx = (va >> 30) & 0x1FF;
    pd_idx = (va >> 21) & 0x1FF;
    pt_idx = (va >> 12) & 0x1FF;
    offset = va & 0xFFF;

    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "VA = 0x%lx\n", va);
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "Indices: PML4=%lu PDPT=%lu PD=%lu PT=%lu OFF=%lu\n\n", pml4_idx, pdpt_idx,
               pd_idx, pt_idx, offset);

    /*
     * Level 4: PML4
     *
     * For pml4_idx = 511:
     * offset_bytes = 511 × 8 = 4088 = 0xFF8
     * entry_addr = table_phys + 0xFF8
     * Read 8 bytes from __va(entry_addr)
     */
    entry = ((unsigned long*)__va(table_phys))[pml4_idx];
    seq_printf(m, "L4 PML4[%lu]: table=0x%lx entry=0x%016lx\n", pml4_idx, table_phys, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  → NOT PRESENT\n");
        return;
    }
    table_phys = entry & PTE_ADDR_MASK;
    seq_printf(m, "  → P=1, next=0x%lx\n", table_phys);

    /*
     * Level 3: PDPT
     *
     * Check PS bit (bit 7) for 1GB huge page
     * If PS=1: final_phys = (entry & 0x000FFFFFC0000000) | (va & 0x3FFFFFFF)
     * 0x3FFFFFFF = 2^30 - 1 = 1GB offset mask
     */
    entry = ((unsigned long*)__va(table_phys))[pdpt_idx];
    seq_printf(m, "L3 PDPT[%lu]: table=0x%lx entry=0x%016lx\n", pdpt_idx, table_phys, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  → NOT PRESENT\n");
        return;
    }
    if ((entry >> 7) & 1) {
        unsigned long phys = (entry & 0x000FFFFFC0000000UL) | (va & 0x3FFFFFFF);
        seq_printf(m, "  → 1GB HUGE PAGE, P=1 PS=1, phys=0x%lx\n", phys);
        return;
    }
    table_phys = entry & PTE_ADDR_MASK;
    seq_printf(m, "  → P=1 PS=0, next=0x%lx\n", table_phys);

    /*
     * Level 2: PD
     *
     * Check PS bit (bit 7) for 2MB huge page
     * If PS=1: final_phys = (entry & 0x000FFFFFFFE00000) | (va & 0x1FFFFF)
     * 0x1FFFFF = 2^21 - 1 = 2MB offset mask
     */
    entry = ((unsigned long*)__va(table_phys))[pd_idx];
    seq_printf(m, "L2 PD[%lu]: table=0x%lx entry=0x%016lx\n", pd_idx, table_phys, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  → NOT PRESENT\n");
        return;
    }
    if ((entry >> 7) & 1) {
        unsigned long phys = (entry & 0x000FFFFFFFE00000UL) | (va & 0x1FFFFF);
        seq_printf(m, "  → 2MB HUGE PAGE, P=1 PS=1, phys=0x%lx\n", phys);
        return;
    }
    table_phys = entry & PTE_ADDR_MASK;
    seq_printf(m, "  → P=1 PS=0, next=0x%lx\n", table_phys);

    /*
     * Level 1: PT
     *
     * PS bit is reserved at this level, ignore it
     */
    entry = ((unsigned long*)__va(table_phys))[pt_idx];
    seq_printf(m, "L1 PT[%lu]: table=0x%lx entry=0x%016lx\n", pt_idx, table_phys, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  → NOT PRESENT\n");
        return;
    }

    /*
     * Final calculation
     *
     * page_phys = entry & 0x000FFFFFFFFFF000
     * final_phys = page_phys | offset
     *
     * For pt_idx=0, offset=0:
     * If entry = 0x8000000106123163:
     * page_phys = 0x106123000
     * final = 0x106123000 | 0 = 0x106123000
     */
    unsigned long page_phys = entry & PTE_ADDR_MASK;
    unsigned long final_phys = page_phys | offset;
    seq_printf(m, "  → 4KB PAGE, base=0x%lx\n", page_phys);
    seq_printf(m, "  → FINAL: 0x%lx | 0x%lx = 0x%lx\n\n", page_phys, offset, final_phys);
}

static int demo_walk4kb_show(struct seq_file* m, void* v) {
    /*
     * Test addresses:
     * 0xFFFFFFFF81000000 = kernel text (usually 4KB mapped)
     * (unsigned long)m = seq_file pointer (kernel heap, likely 2MB mapped)
     */
    unsigned long test_vas[] = {
        0xFFFFFFFF81000000UL,
        (unsigned long)m,
    };
    int i;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 07: 4KB PAGE WALK\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    for (i = 0; i < 2; i++) {
        walk_va(m, test_vas[i]);
    }

    return 0;
}

static int demo_walk4kb_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_walk4kb_show, NULL);
}

static const struct proc_ops demo_walk4kb_ops = {
    .proc_open = demo_walk4kb_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_walk4kb_init(void) {
    proc_create("demo_walk4kb", 0444, NULL, &demo_walk4kb_ops);
    pr_info("demo_walk4kb: loaded\n");
    return 0;
}

static void __exit demo_walk4kb_exit(void) { remove_proc_entry("demo_walk4kb", NULL); }

module_init(demo_walk4kb_init);
module_exit(demo_walk4kb_exit);
