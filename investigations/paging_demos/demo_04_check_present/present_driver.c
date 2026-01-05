/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 04: CHECK PRESENT BIT AND FLAGS
 * Machine: AMD Ryzen 5 4600H | Kernel 6.14.0-37-generic
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * PAGE TABLE ENTRY FLAG LAYOUT (64 bits):
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Bit  │ Name │ Meaning                                                 │
 * ├──────┼──────┼─────────────────────────────────────────────────────────┤
 * │ 0    │ P    │ Present (1=mapped, 0=not present/swapped)               │
 * │ 1    │ R/W  │ Read/Write (0=read-only, 1=writable)                    │
 * │ 2    │ U/S  │ User/Supervisor (0=kernel only, 1=user accessible)      │
 * │ 3    │ PWT  │ Page Write-Through (cache policy)                       │
 * │ 4    │ PCD  │ Page Cache Disable                                      │
 * │ 5    │ A    │ Accessed (set by CPU on read)                           │
 * │ 6    │ D    │ Dirty (set by CPU on write)                             │
 * │ 7    │ PS   │ Page Size (0=table pointer, 1=huge page)                │
 * │ 63   │ NX   │ No Execute (1=data only, 0=can execute)                 │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * WORKED EXAMPLE: Entry = 0x80000002FAE00067
 * ─────────────────────────────────────────────────────────────────────────
 * Low byte extraction: 0x80000002FAE00067 & 0xFF = 0x67
 * 0x67 = 103 decimal = 0110_0111 binary
 *
 * Bit-by-bit decode:
 * 01. P   = 0x67 & 0x01 = 0x01 = 1 → Present ✓
 * 02. R/W = (0x67 >> 1) & 1 = (51) & 1 = 1 → Writable ✓
 *     Work: 0x67 = 103, 103 >> 1 = 51, 51 & 1 = 1
 * 03. U/S = (0x67 >> 2) & 1 = (25) & 1 = 1 → User accessible ✓
 *     Work: 103 >> 2 = 25, 25 & 1 = 1
 * 04. PWT = (0x67 >> 3) & 1 = (12) & 1 = 0 → Write-back
 * 05. PCD = (0x67 >> 4) & 1 = (6) & 1 = 0 → Cached
 * 06. A   = (0x67 >> 5) & 1 = (3) & 1 = 1 → Accessed ✓
 * 07. D   = (0x67 >> 6) & 1 = (1) & 1 = 1 → Dirty ✓
 * 08. PS  = (0x67 >> 7) & 1 = (0) & 1 = 0 → Not huge page
 * 09. NX  = (Entry >> 63) & 1 = 1 → No Execute ✓
 *     Work: 0x8... has bit 63 set (0x8 = 1000 binary)
 *
 * Physical address extraction:
 * 10. phys = Entry & 0x000FFFFFFFFFF000 = 0x00000002FAE00000
 * ─────────────────────────────────────────────────────────────────────────
 *
 * WHEN P=0 (NOT PRESENT):
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Entry = 0 means: Never mapped, completely empty                       │
 * │ Entry ≠ 0 but P=0: Likely a swap entry                                │
 * │                                                                        │
 * │ Swap entry format (Linux x86_64):                                     │
 * │ Bit 0: Always 0 (not present)                                         │
 * │ Bits [1:5]: Swap type (0-31, index into swap devices)                 │
 * │ Bits [6:63]: Swap offset                                              │
 * │                                                                        │
 * │ TRAP: Checking R/W, A, D when P=0 is MEANINGLESS!                     │
 * │       These bits contain swap metadata, not flags.                    │
 * └────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 04: Check Present Bit and Flags");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

/*
 * decode_entry: Extract and display all flags from a PTE
 *
 * For entry = 0x80000002FAE00067:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Register state during decoding:                                       │
 * │ RAX = 0x80000002FAE00067 (entry)                                      │
 * │ After AND 0xFF: RAX = 0x67 (flags byte)                               │
 * │ After SHR 1: RAX = 0x33 = 51                                          │
 * │ After AND 1: RAX = 1 (R/W bit)                                        │
 * └────────────────────────────────────────────────────────────────────────┘
 */
static void decode_entry(struct seq_file* m, unsigned long entry) {
    unsigned long flags = entry & 0xFFF;
    unsigned long phys = entry & PTE_ADDR_MASK;

    seq_printf(m, "  Entry = 0x%016lx\n", entry);
    seq_printf(m, "  Flags = 0x%03lx (low 12 bits)\n", flags);

    /*
     * Present bit check - MUST BE FIRST
     * If P=0, all other bits are undefined/swap metadata
     */
    if (!(entry & 1)) {
        seq_printf(m, "  P = 0 → NOT PRESENT\n");
        if (entry == 0) {
            seq_printf(m, "    → Never mapped (all zeros)\n");
        } else {
            seq_printf(m, "    → Swap entry: type=%lu, offset=0x%lx\n", (entry >> 1) & 0x1F,
                       entry >> 6);
        }
        return;
    }

    /*
     * Decode flags when P=1
     */
    seq_printf(m, "  P   = 1 (Present)\n");
    seq_printf(m, "  R/W = %lu (%s)\n", (entry >> 1) & 1, (entry & 2) ? "Writable" : "Read-only");
    seq_printf(m, "  U/S = %lu (%s)\n", (entry >> 2) & 1, (entry & 4) ? "User" : "Kernel");
    seq_printf(m, "  A   = %lu (%s)\n", (entry >> 5) & 1,
               (entry & 0x20) ? "Accessed" : "Not accessed");
    seq_printf(m, "  D   = %lu (%s)\n", (entry >> 6) & 1, (entry & 0x40) ? "Dirty" : "Clean");
    seq_printf(m, "  PS  = %lu (%s)\n", (entry >> 7) & 1,
               (entry & 0x80) ? "Huge page" : "Table ptr");
    seq_printf(m, "  NX  = %lu (%s)\n", (entry >> 63) & 1,
               (entry >> 63) ? "No execute" : "Executable");
    seq_printf(m, "  Phys = 0x%lx\n", phys);
}

static int demo_present_show(struct seq_file* m, void* v) {
    unsigned long cr3, pml4_phys;
    unsigned long* pml4;
    int indices[] = {0, 255, 256, 511};
    int i;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    pml4_phys = cr3 & PTE_ADDR_MASK;
    pml4 = (unsigned long*)__va(pml4_phys);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 04: PRESENT BIT AND FLAGS\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "FLAG EXTRACTION FORMULA:\n");
    seq_printf(m, "  P   = (entry >> 0) & 1    ; bit 0\n");
    seq_printf(m, "  R/W = (entry >> 1) & 1    ; bit 1\n");
    seq_printf(m, "  U/S = (entry >> 2) & 1    ; bit 2\n");
    seq_printf(m, "  A   = (entry >> 5) & 1    ; bit 5\n");
    seq_printf(m, "  D   = (entry >> 6) & 1    ; bit 6\n");
    seq_printf(m, "  PS  = (entry >> 7) & 1    ; bit 7\n");
    seq_printf(m, "  NX  = (entry >> 63) & 1   ; bit 63\n\n");

    for (i = 0; i < 4; i++) {
        unsigned long entry = pml4[indices[i]];
        seq_printf(m, "PML4[%d]:\n", indices[i]);
        decode_entry(m, entry);
        seq_printf(m, "\n");
    }

    return 0;
}

static int demo_present_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_present_show, NULL);
}

static const struct proc_ops demo_present_ops = {
    .proc_open = demo_present_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_present_init(void) {
    proc_create("demo_present", 0444, NULL, &demo_present_ops);
    pr_info("demo_present: loaded\n");
    return 0;
}

static void __exit demo_present_exit(void) { remove_proc_entry("demo_present", NULL); }

module_init(demo_present_init);
module_exit(demo_present_exit);
