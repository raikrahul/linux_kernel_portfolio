/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 01: READ CR3 REGISTER
 * Machine: AMD Ryzen 5 4600H | Phys=44 bits | Virt=48 bits | RAM=15406 MB
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ CR3 Register Layout (64 bits)                                          │
 * ├────────────┬────────────────────────────────────────┬──────────────────┤
 * │ [63:52]    │              [51:12]                   │     [11:0]       │
 * │ Reserved   │     PML4 Physical Address (40 bits)    │ PCID (12 bits)   │
 * │ =0         │     0x00000XXXXXXXXX000                │ 0x000-0xFFF      │
 * └────────────┴────────────────────────────────────────┴──────────────────┘
 *
 * MASK DERIVATION:
 * 01. Need bits [51:12] → 40 bits of address
 * 02. Bit 12 value = 2^12 = 4096 = 0x1000
 * 03. Bit 51 value = 2^51 = 2251799813685248 = 0x8000000000000
 * 04. Sum [51:12] = 2^52 - 2^12 = 4503599627370496 - 4096 = 4503599627366400
 * 05. 4503599627366400 = 0x000FFFFFFFFFF000 ✓
 *
 * REAL DATA TRACE (sample CR3 = 0x0000000102FAB000):
 * 06. PCID = 0x102FAB000 & 0xFFF = 0x000 = 0
 * 07. PHYS = 0x102FAB000 & 0x000FFFFFFFFFF000 = 0x102FAB000
 * 08. __va(0x102FAB000) = 0xFFFF89DF00000000 + 0x102FAB000 = 0xFFFF89E002FAB000
 * 09. fls64(0x102FAB000) = 33 (bit 32 is highest, 1-indexed returns 33)
 *
 * HARDER: CR3 = 0x000000039AB7EFFF (PCID=4095, unusual pattern)
 * 10. PCID = 0xFFF = 4095
 * 11. PHYS = 0x39AB7EFFF & 0x000FFFFFFFFFF000 = 0x39AB7E000
 * 12. __va(0x39AB7E000) = 0xFFFF89DF00000000 + 0x39AB7E000
 *     = 0xFFFF89E29AB7E000 (work: DF+03=E2, no carry, 89 stays)
 * 13. fls64(0x39AB7E000) = 34 (2^33 < 0x39AB7E000=15459704832 < 2^34)
 *
 * TRAP: Using CR3 directly as pointer → page fault (phys vs virt confusion)
 * TRAP: Forgetting PCID mask → misaligned address (ends in FFF not 000)
 * TRAP: fls64 is 1-indexed not 0-indexed
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 01: Read CR3");

/*
 * page_offset_base = 0xFFFF89DF00000000 (KASLR, varies per boot)
 * Used by __va() macro: __va(phys) = phys + page_offset_base
 */
extern unsigned long page_offset_base;

/*
 * CR3_ADDR_MASK = 0x000FFFFFFFFFF000
 *
 * Derivation by hand:
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │ Hex:  0   0   0   F   F   F   F   F   F   F   F   F   F   0   0   0 │
 * │ Bits: 63-60 59-56 55-52 51-48 47-44 43-40 39-36 35-32 ...          │
 * │       0000  0000  0000  1111  1111  1111  1111  1111  ...          │
 * │       ↑zeros       ↑ones from bit 51 down to bit 12  ↑zeros        │
 * └─────────────────────────────────────────────────────────────────────┘
 *
 * Why these specific bits?
 * - Bits [63:52] = reserved by Intel, must be 0 (12 bits zeroed)
 * - Bits [51:12] = 40-bit physical address space (2^40 = 1TB theoretical)
 * - Bits [11:0] = PCID or must-be-zero (12 bits zeroed)
 *
 * On this machine (44-bit physical), only bits [43:12] actually used
 * But mask is wider for forward compatibility with 52-bit systems
 */
#define CR3_ADDR_MASK 0x000FFFFFFFFFF000UL

/*
 * read_cr3_value: Read CR3 register using inline assembly
 *
 * Assembly: mov %%cr3, %0
 * ┌─────────────────────────────────────────────────────────────────────┐
 * │ mov = copy instruction                                              │
 * │ %%cr3 = Control Register 3 (source) - % doubled for GCC escaping   │
 * │ %0 = first output operand (destination) = cr3 variable             │
 * │ "=r" = output constraint: = means write-only, r means any register │
 * │ volatile = do not optimize away, has side effects                   │
 * └─────────────────────────────────────────────────────────────────────┘
 *
 * Privilege: Ring 0 only. User-mode read → #GP (General Protection Fault)
 * Encoding: 0F 20 C0 (mov rax, cr3) - 3 bytes machine code
 */
static unsigned long read_cr3_value(void) {
    unsigned long cr3;
    /*
     * Machine state before: cr3 holds (example) 0x0000000102FAB000
     * Machine state after: RAX = 0x0000000102FAB000, cr3 variable = same
     */
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

/*
 * demo_cr3_show: Display CR3 components with calculation trace
 *
 * Memory layout during execution:
 * ┌───────────────────────────────────────────────────────────────────────┐
 * │ Stack Frame (approx):                                                 │
 * │ RSP+0x00: cr3_raw   = 0x0000000102FAB000 (8 bytes)                   │
 * │ RSP+0x08: cr3_addr  = 0x0000000102FAB000 (8 bytes)                   │
 * │ RSP+0x10: cr3_pcid  = 0x0000000000000000 (8 bytes)                   │
 * │ RSP+0x18: bits_used = 0x0000000000000021 = 33 (8 bytes)              │
 * │ RSP+0x20: pml4_virt = 0xFFFF89E002FAB000 (8 bytes)                   │
 * └───────────────────────────────────────────────────────────────────────┘
 */
static int demo_cr3_show(struct seq_file* m, void* v) {
    unsigned long cr3_raw, cr3_addr, cr3_pcid;
    unsigned long pml4_virt;
    int bits_used;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 01: READ CR3 REGISTER\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    /*
     * Step 1: Read Raw CR3
     * Example execution:
     * Before: CR3 register = 0x0000000102FAB000 (hardware state)
     * After:  cr3_raw = 0x0000000102FAB000 (software copy)
     */
    cr3_raw = read_cr3_value();
    seq_printf(m, "1. RAW CR3 VALUE:\n");
    seq_printf(m, "   0x%016lx\n\n", cr3_raw);

    /*
     * Step 2: Extract Physical Address
     *
     * Calculation trace for cr3_raw = 0x0000000102FAB000:
     * ┌──────────────────────────────────────────────────────────────────┐
     * │ cr3_raw         = 0x0000000102FAB000                             │
     * │ CR3_ADDR_MASK   = 0x000FFFFFFFFFF000                             │
     * │                   ──────────────────                             │
     * │ AND operation:                                                    │
     * │ 0x0000000102FAB000                                               │
     * │ 0x000FFFFFFFFFF000                                               │
     * │ ═══════════════════                                              │
     * │ 0x0000000102FAB000 (bits 63-52 already 0, bits 11-0 already 0)  │
     * └──────────────────────────────────────────────────────────────────┘
     */
    cr3_addr = cr3_raw & CR3_ADDR_MASK;

    /*
     * Step 3: Extract PCID
     *
     * Calculation trace for cr3_raw = 0x0000000102FAB000:
     * ┌──────────────────────────────────────────────────────────────────┐
     * │ cr3_raw = 0x...000  (last 3 hex digits)                          │
     * │ 0xFFF   = 0x...FFF                                               │
     * │ AND     = 0x...000 = 0                                           │
     * │                                                                   │
     * │ If cr3_raw = 0x...005, then PCID = 5                             │
     * │ If cr3_raw = 0x...FFF, then PCID = 4095 (max)                    │
     * └──────────────────────────────────────────────────────────────────┘
     */
    cr3_pcid = cr3_raw & 0xFFF;

    seq_printf(m, "2. DECODED COMPONENTS:\n");
    seq_printf(m, "   Physical Base (PML4) = 0x%016lx\n", cr3_addr);
    seq_printf(m, "   PCID / Flags         = 0x%03lx\n\n", cr3_pcid);

    /*
     * Step 4: Find Highest Set Bit
     *
     * fls64(0x102FAB000) calculation:
     * ┌──────────────────────────────────────────────────────────────────┐
     * │ 0x102FAB000 = 4,344,627,200 decimal                             │
     * │ 2^32 = 4,294,967,296                                             │
     * │ 2^33 = 8,589,934,592                                             │
     * │ 4,294,967,296 < 4,344,627,200 < 8,589,934,592                   │
     * │ ∴ highest set bit = 32 (0-indexed)                               │
     * │ fls64 returns 1-indexed → 32 + 1 = 33                            │
     * └──────────────────────────────────────────────────────────────────┘
     */
    bits_used = fls64(cr3_addr);

    seq_printf(m, "3. AXIOMATIC CHECKS:\n");
    seq_printf(m, "   Max Physical Address Bit: %d\n", bits_used);
    seq_printf(m, "   Expected range: 12..44 (this machine: 44-bit physical)\n");
    seq_printf(m, "   Is Aligned (Low 12 bits 0)? %s\n\n", (cr3_addr & 0xFFF) == 0 ? "YES" : "NO");

    /*
     * Step 5: Convert to Virtual Address
     *
     * __va() expansion for phys = 0x102FAB000:
     * ┌──────────────────────────────────────────────────────────────────┐
     * │ page_offset_base = 0xFFFF89DF00000000 (KASLR value)              │
     * │ phys             = 0x0000000102FAB000                            │
     * │                    ──────────────────                            │
     * │ Addition column-by-column (right to left):                       │
     * │                                                                   │
     * │   0xFFFF89DF00000000                                             │
     * │ + 0x0000000102FAB000                                             │
     * │   ══════════════════                                             │
     * │   0xFFFF89E002FAB000                                             │
     * │                                                                   │
     * │ Work:                                                             │
     * │ 00000000 + 02FAB000 = 02FAB000 (no overflow)                     │
     * │ 89DF0000 + 00010000 = 89E00000                                   │
     * │   DF + 01 = E0 (223 + 1 = 224 = 0xE0)                            │
     * │   89 + carry? DF+01=E0, no carry (E0 < 0x100) → 89 stays        │
     * │   Wait: 0x89DF + 0x0001 = 0x89E0 ✓                               │
     * │ FFFF + 0000 = FFFF                                               │
     * │                                                                   │
     * │ Result: 0xFFFF89E002FAB000                                       │
     * └──────────────────────────────────────────────────────────────────┘
     */
    pml4_virt = (unsigned long)__va(cr3_addr);

    seq_printf(m, "4. DERIVED VIRTUAL ADDRESS:\n");
    seq_printf(m, "   phys_to_virt(0x%lx)\n", cr3_addr);
    seq_printf(m, "   = page_offset_base + phys\n");
    seq_printf(m, "   = 0x%lx + 0x%lx\n", page_offset_base, cr3_addr);
    seq_printf(m, "   = 0x%016lx\n\n", pml4_virt);

    /*
     * Step 6: Verify by reading first PML4 entry
     *
     * Memory access trace:
     * ┌──────────────────────────────────────────────────────────────────┐
     * │ Address 0xFFFF89E002FAB000 → contains PML4[0]                    │
     * │ Address 0xFFFF89E002FAB008 → contains PML4[1]                    │
     * │ ...                                                               │
     * │ Address 0xFFFF89E002FABFF8 → contains PML4[511]                  │
     * │ Table size = 512 × 8 = 4096 bytes = 1 page ✓                    │
     * └──────────────────────────────────────────────────────────────────┘
     */
    seq_printf(m, "5. PML4[0] VALUE (verification read):\n");
    seq_printf(m, "   *(0x%lx) = 0x%016lx\n", pml4_virt, *(unsigned long*)pml4_virt);

    return 0;
}

static int demo_cr3_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_cr3_show, NULL);
}

static const struct proc_ops demo_cr3_ops = {
    .proc_open = demo_cr3_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_cr3_init(void) {
    proc_create("demo_cr3", 0444, NULL, &demo_cr3_ops);
    pr_info("demo_cr3: loaded\n");
    return 0;
}

static void __exit demo_cr3_exit(void) {
    remove_proc_entry("demo_cr3", NULL);
    pr_info("demo_cr3: unloaded\n");
}

module_init(demo_cr3_init);
module_exit(demo_cr3_exit);
