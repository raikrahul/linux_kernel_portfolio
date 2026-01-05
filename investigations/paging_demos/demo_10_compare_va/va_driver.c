/*
 * DEMO 10: COMPARE __VA MACRO
 * ═══════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Physical Address (PA) e.g., 0x1234
 *    Action: Add Kernel Offset `page_offset_base`
 *    Output: Kernel Virtual Address (KVA)
 *
 *    Computation:
 *    KVA = PA + page_offset_base
 *    page_offset_base (on your machine) = 0xFFFF89DF00000000
 *    e.g., __va(0) = 0xFFFF89DF00000000
 *
 * 2. WHY:
 *    - In 64-bit Long Mode, we cannot access physical RAM directly.
 *    - We must go through the MMU (Paging).
 *    - The Kernel maps ALL physical RAM to a specific virtual region.
 *    - This is the "Direct Map" or "Physmap".
 *
 * 3. WHERE:
 *    - Region starts at `page_offset_base`.
 *    - Covers all RAM (16GB).
 *
 * 4. WHO:
 *    - Kernel Boot Code (Head.S).
 *    - Creates this mapping at PML4 index 273/275/variable (KASLR).
 *
 * 5. WHEN:
 *    - Anytime kernel code needs to read a page table or physical frame.
 *    - `phys_to_virt()` and `virt_to_phys()` use this simple addition.
 *
 * 6. WITHOUT:
 *    - To read physical 0x1234, we would need to manually `ioremap` it.
 *    - Extremely slow.
 *
 * 7. WHICH:
 *    - Which Mapping? The "Direct Mapping".
 *    - Not to be confused with "Highmem" (32-bit concept) or "vmalloc" (discontiguous).
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Parallel Universe
 * - Universe A (Physical) starts at year 0.
 * - Universe B (Virtual) starts at year 2000.
 * - Time flows same in both.
 *
 * Conversion:
 * - Event at Year 5 in Universe A = Year 2005 in Universe B.
 * - Formula: B_Year = A_Year + 2000.
 *
 * Numerical:
 * - A_Year = 100.
 * - B_Year = 2100.
 * - A_Year = 200.
 * - B_Year = 2200.
 *
 * Key: The "Offset" (2000) is constant.
 * KASLR: Sometimes the Offset is 3000, 4000... (Randomized at boot).
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 10: Compare __va Macro");

extern unsigned long page_offset_base;

static int demo_va_show(struct seq_file* m, void* v) {
    unsigned long test_phys[] = {
        0x0UL,         0x1000UL,      0x1337A3000UL,
        0x2CCB6C000UL, 0x3FFFFFFFFUL, /* ~16 GB, max RAM on your machine */
    };
    int i;
    unsigned long manual, macro_result;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 10: __VA MACRO COMPARISON\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "page_offset_base = 0x%lx\n\n", page_offset_base);
    seq_printf(m, "FORMULA: __va(phys) = page_offset_base + phys\n\n");

    for (i = 0; i < 5; i++) {
        unsigned long phys = test_phys[i];

        manual = page_offset_base + phys;
        macro_result = (unsigned long)__va(phys);

        seq_printf(m, "phys = 0x%lx:\n", phys);
        seq_printf(m, "  manual = 0x%lx + 0x%lx = 0x%lx\n", page_offset_base, phys, manual);
        seq_printf(m, "  __va() = 0x%lx\n", macro_result);
        seq_printf(m, "  match = %s\n\n", (manual == macro_result) ? "✓" : "✗");
    }

    /* Demonstrate reading memory via __va */
    seq_printf(m, "PROOF - READ MEMORY VIA __VA:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    unsigned long cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    unsigned long pml4_phys = cr3 & 0x000FFFFFFFFFF000UL;
    unsigned long pml4_virt = (unsigned long)__va(pml4_phys);
    unsigned long first_entry = *(unsigned long*)pml4_virt;

    seq_printf(m, "CR3 = 0x%lx → PML4_phys = 0x%lx\n", cr3, pml4_phys);
    seq_printf(m, "__va(0x%lx) = 0x%lx\n", pml4_phys, pml4_virt);
    seq_printf(m, "*(0x%lx) = 0x%016lx ← PML4[0]\n", pml4_virt, first_entry);

    return 0;
}

static int demo_va_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_va_show, NULL);
}

static const struct proc_ops demo_va_ops = {
    .proc_open = demo_va_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_va_init(void) {
    proc_create("demo_va", 0444, NULL, &demo_va_ops);
    pr_info("demo_va: loaded\n");
    return 0;
}

static void __exit demo_va_exit(void) { remove_proc_entry("demo_va", NULL); }

module_init(demo_va_init);
module_exit(demo_va_exit);
