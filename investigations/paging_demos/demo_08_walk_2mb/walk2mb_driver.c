/*
 * DEMO 08: 2MB HUGE PAGE WALK
 * ═══════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Virtual Address (VA) mapped with 2MB huge page.
 *    Process: 3-Level Descent (PML4 -> PDPT -> PD).
 *    Output: Physical Address (PA).
 *
 *    Trace:
 *    1. CR3 -> PML4
 *    2. PML4[i4] -> PDPT
 *    3. PDPT[i3] -> PD
 *    4. PD[i2] Check: Bit 7 (PS) == 1?
 *    5. Yes -> Stop.
 *    6. PA = (PD_Entry & Mask_2MB) | (VA & Offset_2MB)
 *
 * 2. WHY:
 *    - To reduce TLB pressure.
 *    - 1 TLB entry covers 2MB (512 x 4KB pages).
 *    - 512x fewer TLB misses for large contiguous arrays.
 *    - Used by Kernel Direct Map (your machine).
 *
 * 3. WHERE:
 *    - Valid at Level 2 (PD).
 *    - Physical Address is 2MB aligned (lowest 21 bits 0).
 *
 * 4. WHO:
 *    - OS: Sets up huge pages at boot or via `mmap(HUGETLB)`.
 *    - MMU: Detects PS=1, short-circuits the walk.
 *
 * 5. WHEN:
 *    - Direct Map access (all kernel memory).
 *    - Large database buffers (Oracle, Postgres, etc).
 *    - virtualization (Guest memory).
 *
 * 6. WITHOUT:
 *    - Kernel Direct Map (64TB max) would need 32GB of Page Tables (4KB).
 *    - With 2MB pages -> needs only ~64MB Page Tables.
 *    - Massive RAM saving.
 *
 * 7. WHICH:
 *    - Offset becomes 21 bits (Bits 20-0).
 *    - 2^21 = 2,097,152 bytes = 2MB.
 *    - Index 1 (PT) is skipped/swallowed by offset.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Express Train vs Local Train
 * - Route: City -> District -> Town -> Street.
 * - Local Train: Stops at CityHub -> DistrictHub -> TownHub -> Walks to Street.
 * - Express Train: Stops at CityHub -> DistrictHub -> Takes Taxi to ANYWHERE in District.
 *
 * Logic:
 * - "DistrictHub" Ticket has a special "Express" stamp (PS=1).
 * - Passenger ignores TownHub.
 * - Passenger uses "District-Relative Coordinates" (Large Offset) to find House.
 *
 * Numerical:
 * - Address: 5-2-9999.
 * - 5 = City.
 * - 2 = District.
 * - 9999 = House ID (Range 0-10000).
 * - Standard Address would be 5-2-4-80 (Town 4, House 80).
 * - Here, Town ID is merged into House ID.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 08: 2MB Huge Page Walk");

extern unsigned long page_offset_base;

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL
#define HUGE_2MB_MASK 0x000FFFFFFFE00000UL

static int demo_walk2mb_show(struct seq_file* m, void* v) {
    unsigned long va, cr3, table_phys, entry;
    unsigned long pml4_idx, pdpt_idx, pd_idx;

    /* Use page_offset_base as test VA (direct map uses 2MB pages) */
    va = page_offset_base;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    table_phys = cr3 & PTE_ADDR_MASK;

    pml4_idx = (va >> 39) & 0x1FF;
    pdpt_idx = (va >> 30) & 0x1FF;
    pd_idx = (va >> 21) & 0x1FF;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 08: 2MB HUGE PAGE WALK\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "VA = 0x%lx (page_offset_base)\n", va);
    seq_printf(m, "PML4=%lu PDPT=%lu PD=%lu\n\n", pml4_idx, pdpt_idx, pd_idx);

    /* L4: PML4 */
    entry = ((unsigned long*)__va(table_phys))[pml4_idx];
    seq_printf(m, "L4 PML4[%lu] = 0x%016lx\n", pml4_idx, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  NOT PRESENT\n");
        return 0;
    }
    table_phys = entry & PTE_ADDR_MASK;
    seq_printf(m, "  → PDPT @ 0x%lx\n\n", table_phys);

    /* L3: PDPT */
    entry = ((unsigned long*)__va(table_phys))[pdpt_idx];
    seq_printf(m, "L3 PDPT[%lu] = 0x%016lx\n", pdpt_idx, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  NOT PRESENT\n");
        return 0;
    }
    if ((entry >> 7) & 1) {
        seq_printf(m, "  PS=1 → 1GB HUGE (unexpected)\n");
        return 0;
    }
    table_phys = entry & PTE_ADDR_MASK;
    seq_printf(m, "  PS=0 → PD @ 0x%lx\n\n", table_phys);

    /* L2: PD - expect huge page here */
    entry = ((unsigned long*)__va(table_phys))[pd_idx];
    seq_printf(m, "L2 PD[%lu] = 0x%016lx\n", pd_idx, entry);
    if (!(entry & 1)) {
        seq_printf(m, "  NOT PRESENT\n");
        return 0;
    }

    unsigned long ps_bit = (entry >> 7) & 1;
    seq_printf(m, "  bit 7 = (0x%lx >> 7) & 1 = %lu\n", entry, ps_bit);

    if (ps_bit) {
        unsigned long page_base = entry & HUGE_2MB_MASK;
        unsigned long offset_21 = va & 0x1FFFFF;
        unsigned long final_phys = page_base | offset_21;

        seq_printf(m, "  PS=1 → 2MB HUGE PAGE ✓\n\n");
        seq_printf(m, "CALCULATION:\n");
        seq_printf(m, "  page_base = entry & 0x%lx = 0x%lx\n", HUGE_2MB_MASK, page_base);
        seq_printf(m, "  offset = VA & 0x1FFFFF = 0x%lx\n", offset_21);
        seq_printf(m, "  final_phys = 0x%lx | 0x%lx = 0x%lx\n", page_base, offset_21, final_phys);
        seq_printf(m, "\nLEVELS TRAVERSED: 3 (PML4 → PDPT → PD)\n");
        seq_printf(m, "RAM READS: 3 × 8 = 24 bytes\n");
    } else {
        seq_printf(m, "  PS=0 → Not a 2MB huge page, would continue to PT\n");
    }

    return 0;
}

static int demo_walk2mb_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_walk2mb_show, NULL);
}

static const struct proc_ops demo_walk2mb_ops = {
    .proc_open = demo_walk2mb_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_walk2mb_init(void) {
    proc_create("demo_walk2mb", 0444, NULL, &demo_walk2mb_ops);
    pr_info("demo_walk2mb: loaded\n");
    return 0;
}

static void __exit demo_walk2mb_exit(void) { remove_proc_entry("demo_walk2mb", NULL); }

module_init(demo_walk2mb_init);
module_exit(demo_walk2mb_exit);
