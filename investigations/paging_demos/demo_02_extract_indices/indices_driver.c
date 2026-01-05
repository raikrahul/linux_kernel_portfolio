/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 02: EXTRACT PAGE TABLE INDICES
 * Machine: AMD Ryzen 5 4600H | Phys=44 bits | Virt=48 bits
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * VA LAYOUT (48 bits used):
 * ┌─────────┬─────────┬─────────┬─────────┬─────────────┐
 * │ [47:39] │ [38:30] │ [29:21] │ [20:12] │   [11:0]    │
 * │  PML4   │  PDPT   │   PD    │   PT    │   Offset    │
 * │  9 bits │  9 bits │  9 bits │  9 bits │   12 bits   │
 * └─────────┴─────────┴─────────┴─────────┴─────────────┘
 *
 * MASK DERIVATIONS:
 * 01. 0x1FF = 2^9 - 1 = 512 - 1 = 511 = binary 111111111 (9 ones)
 * 02. 0xFFF = 2^12 - 1 = 4096 - 1 = 4095 = binary 111111111111 (12 ones)
 *
 * EXAMPLE 1: VA = 0x7FFE5E4ED123 (user stack from process_a)
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │ Step 1: PML4 = (0x7FFE5E4ED123 >> 39) & 0x1FF                            │
 * │         0x7FFE5E4ED123 = 140,730,825,814,307 decimal                     │
 * │         >> 39: 140,730,825,814,307 / 549,755,813,888 = 255.99...        │
 * │         integer part = 255 = 0xFF                                        │
 * │         0xFF & 0x1FF = 0xFF = 255 ✓                                     │
 * │                                                                          │
 * │ Step 2: PDPT = (0x7FFE5E4ED123 >> 30) & 0x1FF                           │
 * │         >> 30: 0x7FFE5E4ED123 / 1,073,741,824 = 131,071.99...           │
 * │         131,071 = 0x1FFFF                                                │
 * │         0x1FFFF & 0x1FF = 0x1FF = 511 ✓                                 │
 * │                                                                          │
 * │ Step 3: PD = (0x7FFE5E4ED123 >> 21) & 0x1FF                             │
 * │         >> 21: 0x7FFE5E4ED123 >> 21 = 0x3FFF2F27                        │
 * │         0x3FFF2F27 & 0x1FF = 0x127 = 295 ✓                              │
 * │         Work: 0x27 = 39, + 0x100 = 256+39 = 295                         │
 * │                                                                          │
 * │ Step 4: PT = (0x7FFE5E4ED123 >> 12) & 0x1FF                             │
 * │         >> 12: 0x7FFE5E4ED123 >> 12 = 0x7FFE5E4ED                       │
 * │         0x7FFE5E4ED & 0x1FF = 0xED = 237 ✓                              │
 * │         Work: 0xED = 14×16 + 13 = 224 + 13 = 237                        │
 * │                                                                          │
 * │ Step 5: OFF = 0x7FFE5E4ED123 & 0xFFF = 0x123 = 291 ✓                    │
 * │         Work: 0x123 = 1×256 + 2×16 + 3 = 256 + 32 + 3 = 291             │
 * └──────────────────────────────────────────────────────────────────────────┘
 *
 * EXAMPLE 2: VA = 0xFFFF89DF00000000 (page_offset_base, kernel)
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │ TRAP: This is a KERNEL address. Bit 47 = 1, canonical sign extension.   │
 * │                                                                          │
 * │ Hex breakdown: F F F F 8 9 D F 0 0 0 0 0 0 0 0                          │
 * │ Position:     15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 (hex digit index)   │
 * │                                                                          │
 * │ Hex digit 11 = 8 = 1000 binary (bits 47-44)                             │
 * │ Hex digit 10 = 9 = 1001 binary (bits 43-40)                             │
 * │ Hex digit 9  = D = 1101 binary (bits 39-36)                             │
 * │                                                                          │
 * │ PML4 bits [47:39]:                                                       │
 * │   bit 47 = 1 (from 8=1000)                                               │
 * │   bit 46 = 0 (from 8=1000)                                               │
 * │   bit 45 = 0 (from 8=1000)                                               │
 * │   bit 44 = 0 (from 8=1000)                                               │
 * │   bit 43 = 1 (from 9=1001)                                               │
 * │   bit 42 = 0 (from 9=1001)                                               │
 * │   bit 41 = 0 (from 9=1001)                                               │
 * │   bit 40 = 1 (from 9=1001)                                               │
 * │   bit 39 = 1 (from D=1101)                                               │
 * │   = 1,0,0,0,1,0,0,1,1 = 0b100010011 = 0x113 = 275                       │
 * │                                                                          │
 * │ PDPT bits [38:30]:                                                       │
 * │   bits 38,37,36 from D=1101: 1,0,1                                      │
 * │   bits 35-32 from F=1111: 1,1,1,1                                       │
 * │   bits 31,30 from next 0: 0,0                                            │
 * │   = 1,0,1,1,1,1,1,0,0 = 0b101111100 = 0x17C = 380                       │
 * │                                                                          │
 * │ PD, PT, OFF: all 0 (trailing zeros in address)                          │
 * └──────────────────────────────────────────────────────────────────────────┘
 *
 * HARDER EXAMPLE: VA = 0x00007F3A2B4C5678
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │ PML4 = (0x7F3A2B4C5678 >> 39) & 0x1FF                                   │
 * │ 0x7F3A2B4C5678 = 139,652,107,593,336 decimal                            │
 * │ >> 39: 139,652,107,593,336 / 549,755,813,888 = 254.04...                │
 * │ = 254 = 0xFE                                                             │
 * │ 0xFE & 0x1FF = 0xFE = 254                                               │
 * │                                                                          │
 * │ PDPT = (0x7F3A2B4C5678 >> 30) & 0x1FF                                   │
 * │ >> 30: 0x7F3A2B4C5678 >> 30 = 0x1FCE8                                   │
 * │ 0x1FCE8 & 0x1FF = 0x0E8 = 232                                           │
 * │                                                                          │
 * │ PD = (0x7F3A2B4C5678 >> 21) & 0x1FF                                     │
 * │ >> 21: 0x7F3A2B4C5678 >> 21 = 0x3F9D15A                                 │
 * │ 0x3F9D15A & 0x1FF = 0x15A = 346                                         │
 * │                                                                          │
 * │ PT = (0x7F3A2B4C5678 >> 12) & 0x1FF                                     │
 * │ >> 12: 0x7F3A2B4C5678 >> 12 = 0x7F3A2B4C5                               │
 * │ 0x7F3A2B4C5 & 0x1FF = 0x0C5 = 197                                       │
 * │                                                                          │
 * │ OFF = 0x7F3A2B4C5678 & 0xFFF = 0x678 = 1656                             │
 * │ Work: 0x678 = 6×256 + 7×16 + 8 = 1536 + 112 + 8 = 1656                  │
 * └──────────────────────────────────────────────────────────────────────────┘
 *
 * EDGE CASE: VA = 0 (null pointer)
 * ┌──────────────────────────────────────────────────────────────────────────┐
 * │ All indices = 0, offset = 0                                              │
 * │ Path: PML4[0] → PDPT[0] → PD[0] → PT[0] → byte 0                        │
 * │ Usually unmapped → page fault on access                                  │
 * └──────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 02: Extract Page Table Indices");

/*
 * Test addresses from real system:
 * 0x7FFE5E4ED123 = process_a stack (captured via /proc/PID/maps)
 * 0xFFFF89DF00000000 = page_offset_base (kernel direct map start)
 * 0x0000000000001000 = minimal user VA (page 1)
 * 0xFFFFFFFFFFFFFFFF = maximum VA (last byte of address space)
 */
static unsigned long test_vaddrs[] = {
    0x7FFE5E4ED123UL,
    0xFFFF89DF00000000UL,
    0x0000000000001000UL,
    0xFFFFFFFFFFFFFFFFUL,
};
#define NUM_VADDRS (sizeof(test_vaddrs) / sizeof(test_vaddrs[0]))

/*
 * show_extraction: Display index extraction with calculation trace
 *
 * For VA = 0x7FFE5E4ED123:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ REGISTER TRACE (pseudo-assembly):                                     │
 * │                                                                        │
 * │ 1. MOV RAX, 0x7FFE5E4ED123     ; RAX = VA                              │
 * │ 2. SHR RAX, 39                 ; RAX = 0xFF (PML4 raw)                │
 * │ 3. AND RAX, 0x1FF              ; RAX = 0xFF = 255 (PML4 index)        │
 * │                                                                        │
 * │ 4. MOV RAX, 0x7FFE5E4ED123     ; reload                                │
 * │ 5. SHR RAX, 30                 ; RAX = 0x1FFFF (PDPT raw)             │
 * │ 6. AND RAX, 0x1FF              ; RAX = 0x1FF = 511 (PDPT index)       │
 * │                                                                        │
 * │ ...and so on for PD, PT, Offset                                       │
 * └────────────────────────────────────────────────────────────────────────┘
 */
static void show_extraction(struct seq_file* m, unsigned long va) {
    unsigned long pml4, pdpt, pd, pt, off;
    unsigned long shifted;

    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "VA = 0x%016lx\n", va);
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    /*
     * PML4 extraction: bits [47:39]
     *
     * For VA = 0x7FFE5E4ED123:
     * shifted = 0x7FFE5E4ED123 >> 39
     *         = 0x7FFE5E4ED123 / 549755813888
     *         = 255 (integer division)
     * pml4 = 255 & 0x1FF = 255
     */
    shifted = va >> 39;
    pml4 = shifted & 0x1FF;
    seq_printf(m, "PML4: 0x%lx >> 39 = 0x%lx → & 0x1FF = %lu\n", va, shifted, pml4);

    /*
     * PDPT extraction: bits [38:30]
     */
    shifted = va >> 30;
    pdpt = shifted & 0x1FF;
    seq_printf(m, "PDPT: 0x%lx >> 30 = 0x%lx → & 0x1FF = %lu\n", va, shifted, pdpt);

    /*
     * PD extraction: bits [29:21]
     */
    shifted = va >> 21;
    pd = shifted & 0x1FF;
    seq_printf(m, "PD:   0x%lx >> 21 = 0x%lx → & 0x1FF = %lu\n", va, shifted, pd);

    /*
     * PT extraction: bits [20:12]
     */
    shifted = va >> 12;
    pt = shifted & 0x1FF;
    seq_printf(m, "PT:   0x%lx >> 12 = 0x%lx → & 0x1FF = %lu\n", va, shifted, pt);

    /*
     * Offset extraction: bits [11:0]
     */
    off = va & 0xFFF;
    seq_printf(m, "OFF:  0x%lx & 0xFFF = %lu (0x%lx)\n\n", va, off, off);
}

static int indices_show(struct seq_file* m, void* v) {
    int i;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 02: PAGE TABLE INDEX EXTRACTION\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    /*
     * FORMULA REFERENCE:
     * ┌────────────────────────────────────────────────────────────────────┐
     * │ Level  │ Shift │ Mask  │ Bit Range │ Coverage                     │
     * ├────────┼───────┼───────┼───────────┼──────────────────────────────│
     * │ PML4   │ >> 39 │ 0x1FF │ [47:39]   │ 512 GB per entry             │
     * │ PDPT   │ >> 30 │ 0x1FF │ [38:30]   │ 1 GB per entry               │
     * │ PD     │ >> 21 │ 0x1FF │ [29:21]   │ 2 MB per entry               │
     * │ PT     │ >> 12 │ 0x1FF │ [20:12]   │ 4 KB per entry               │
     * │ Offset │ & FFF │ 0xFFF │ [11:0]    │ byte within page             │
     * └────────────────────────────────────────────────────────────────────┘
     */
    seq_printf(m, "FORMULA:\n");
    seq_printf(m, "  PML4 = (VA >> 39) & 0x1FF   ; 9 bits, range 0-511\n");
    seq_printf(m, "  PDPT = (VA >> 30) & 0x1FF   ; 9 bits, range 0-511\n");
    seq_printf(m, "  PD   = (VA >> 21) & 0x1FF   ; 9 bits, range 0-511\n");
    seq_printf(m, "  PT   = (VA >> 12) & 0x1FF   ; 9 bits, range 0-511\n");
    seq_printf(m, "  OFF  = VA & 0xFFF           ; 12 bits, range 0-4095\n\n");

    for (i = 0; i < NUM_VADDRS; i++) {
        show_extraction(m, test_vaddrs[i]);
    }

    return 0;
}

static int indices_open(struct inode* inode, struct file* file) {
    return single_open(file, indices_show, NULL);
}

static const struct proc_ops indices_ops = {
    .proc_open = indices_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init indices_init(void) {
    proc_create("demo_indices", 0444, NULL, &indices_ops);
    pr_info("demo_indices: loaded\n");
    return 0;
}

static void __exit indices_exit(void) {
    remove_proc_entry("demo_indices", NULL);
    pr_info("demo_indices: unloaded\n");
}

module_init(indices_init);
module_exit(indices_exit);
