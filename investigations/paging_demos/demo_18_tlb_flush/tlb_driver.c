/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 18: TLB FLUSH
 * Machine: AMD Ryzen 5 4600H | 12 logical CPUs
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * TLB (Translation Lookaside Buffer):
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Purpose: Cache virtual-to-physical translations                       │
 * │                                                                        │
 * │ Without TLB:                                                           │
 * │   Every memory access → 4 table reads (PML4→PDPT→PD→PT)               │
 * │   Each read = ~100 ns (RAM latency)                                   │
 * │   Total = 400 ns per translation + 100 ns data = 500 ns               │
 * │                                                                        │
 * │ With TLB hit:                                                          │
 * │   VA → TLB lookup (~1 cycle = ~0.25 ns) → PA                          │
 * │   Total = ~1 ns translation + access                                  │
 * │   Speedup: 500× for translation, ~5× overall with cache               │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * TLB SIZES (typical AMD Zen2):
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ L1 dTLB: 64 entries × 4KB = 256 KB coverage                           │
 * │ L1 dTLB: 32 entries × 2MB = 64 MB coverage                            │
 * │ L1 iTLB: 64 entries × 4KB = 256 KB coverage                           │
 * │ L2 TLB:  1536 entries unified                                         │
 * │                                                                        │
 * │ Calculation:                                                           │
 * │ 4KB: 64 × 4096 = 262,144 = 256 KB                                    │
 * │ 2MB: 32 × 2,097,152 = 67,108,864 = 64 MB                             │
 * │ L2:  1536 × 4096 = 6,291,456 = 6 MB                                  │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * FLUSH TRIGGERS:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ 1. MOV CR3, value  → Full flush (except G=1 global pages)             │
 * │ 2. invlpg [addr]   → Single page flush                                │
 * │ 3. Context switch  → Full flush or PCID switch                        │
 * │ 4. munmap/mprotect → Range flush via invlpg                           │
 * │ 5. MOV CR4, value  → Full flush including globals (if modifying PGE) │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * INVLPG INSTRUCTION:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Syntax: invlpg [memory_address]                                       │
 * │ Opcode: 0F 01 /7                                                      │
 * │ Privilege: Ring 0 only                                                │
 * │ Effect: Invalidate TLB entry for the specified address                │
 * │ Scope: Current CPU ONLY! Other CPUs unaffected.                       │
 * │                                                                        │
 * │ For SMP: Need IPI (Inter-Processor Interrupt) to other CPUs            │
 * │ Kernel: flush_tlb_mm_range() sends IPIs automatically                 │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * IPI OVERHEAD CALCULATION:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ IPI latency: 1-10 μs per CPU                                          │
 * │ 12 CPU shootdown: ~10 μs (parallel send, wait for all ACKs)           │
 * │ Compare to invlpg alone: ~25 ns                                       │
 * │ Overhead: 10,000 ns / 25 ns = 400×                                    │
 * │                                                                        │
 * │ Optimization: Batch shootdowns                                         │
 * │ - Unmap 1000 pages with 1 shootdown (flush range) instead of 1000    │
 * │ - Each invlpg on remote: 100 cycles × 1000 = 100,000 cycles          │
 * │ - Range flush + 1 IPI: much cheaper                                   │
 * └────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 18: TLB Flush");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

/*
 * local_flush_tlb: Flush entire TLB on current CPU by rewriting CR3
 *
 * Machine code:
 *   mov rax, cr3     ; 0F 20 D8 (3 bytes)
 *   mov cr3, rax     ; 0F 22 D8 (3 bytes)
 *
 * Effect:
 * - All non-global TLB entries invalidated
 * - Global entries (G=1) preserved (for kernel mappings)
 * - Must execute on each CPU needing flush
 */
static void local_flush_tlb(void) {
    unsigned long cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %0, %%cr3" : : "r"(cr3) : "memory");
}

/*
 * local_invlpg: Invalidate single TLB entry for given address
 *
 * Machine code: invlpg [rax]  ; 0F 01 38 (3 bytes with mod/rm)
 *
 * Effect:
 * - Only the TLB entry for this specific page invalidated
 * - Cheaper than full flush for single page modifications
 */
static void local_invlpg(unsigned long addr) {
    asm volatile("invlpg (%0)" : : "r"(addr) : "memory");
}

static int demo_tlb_show(struct seq_file* m, void* v) {
    unsigned long cr3;
    unsigned long test_addr = (unsigned long)m; /* Some kernel address */

    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 18: TLB FLUSH\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "TLB COST ANALYSIS:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "Without TLB (cold miss):\n");
    seq_printf(m, "  PML4 read: 100 ns\n");
    seq_printf(m, "  PDPT read: 100 ns\n");
    seq_printf(m, "  PD   read: 100 ns\n");
    seq_printf(m, "  PT   read: 100 ns\n");
    seq_printf(m, "  ─────────────────\n");
    seq_printf(m, "  Translation: 400 ns\n");
    seq_printf(m, "  + Data read: 100 ns\n");
    seq_printf(m, "  Total: 500 ns\n\n");

    seq_printf(m, "With TLB hit:\n");
    seq_printf(m, "  TLB lookup: ~1 ns\n");
    seq_printf(m, "  + Data read: ~1-100 ns (L1=1, RAM=100)\n");
    seq_printf(m, "  Total: 2-101 ns\n\n");

    seq_printf(m, "Speedup: 500 / 2 = 250× (with L1 cache)\n");
    seq_printf(m, "         500 / 101 = 5× (RAM access)\n\n");

    seq_printf(m, "TLB COVERAGE:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "L1 dTLB 4KB: 64 entries × 4096 = 262,144 = 256 KB\n");
    seq_printf(m, "L1 dTLB 2MB: 32 entries × 2 MB = 64 MB\n");
    seq_printf(m, "L2 TLB:      1536 entries × 4 KB = 6 MB\n\n");

    seq_printf(m, "FLUSH DEMONSTRATION:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "1. Full TLB flush (MOV CR3, CR3):\n");
    seq_printf(m, "   Before: CR3 = 0x%lx\n", cr3);

    local_flush_tlb();

    seq_printf(m, "   After:  CR3 = 0x%lx (same, but TLB flushed)\n\n", cr3);

    seq_printf(m, "2. Single page flush (INVLPG):\n");
    seq_printf(m, "   Address: 0x%lx\n", test_addr);

    local_invlpg(test_addr);

    seq_printf(m, "   Executed: invlpg (0x%lx)\n", test_addr);
    seq_printf(m, "   Effect: TLB entry for this page invalidated\n\n");

    seq_printf(m, "IPI SHOOTDOWN (SMP):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "Problem: invlpg only affects CURRENT CPU\n");
    seq_printf(m, "Solution: Send IPI to all CPUs running this mm\n");
    seq_printf(m, "Cost: ~10 μs for 12-CPU shootdown\n");
    seq_printf(m, "vs invlpg alone: ~25 ns (400× cheaper)\n\n");

    seq_printf(m, "PCID OPTIMIZATION:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "Without PCID: Context switch → full TLB flush\n");
    seq_printf(m, "With PCID: Context switch → no flush, entries tagged\n");
    seq_printf(m, "           Returning process finds TLB warm\n");
    seq_printf(m, "CR3[11:0] = PCID (12 bits = 4096 contexts)\n");

    return 0;
}

static int demo_tlb_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_tlb_show, NULL);
}

static const struct proc_ops demo_tlb_ops = {
    .proc_open = demo_tlb_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_tlb_init(void) {
    proc_create("demo_tlb", 0444, NULL, &demo_tlb_ops);
    pr_info("demo_tlb: loaded\n");
    return 0;
}

static void __exit demo_tlb_exit(void) { remove_proc_entry("demo_tlb", NULL); }

module_init(demo_tlb_init);
module_exit(demo_tlb_exit);
