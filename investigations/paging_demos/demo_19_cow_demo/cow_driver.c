/*
 * ═══════════════════════════════════════════════════════════════════════════
 * DEMO 19: COPY-ON-WRITE MECHANICS
 * Machine: AMD Ryzen 5 4600H | RAM = 15406 MB
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * COW ALGORITHM OVERVIEW:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ 1. fork() called                                                      │
 * │ 2. For each writable page in parent:                                  │
 * │    a. Mark PTE R/W = 0 (read-only) in BOTH parent and child          │
 * │    b. Increment page reference count (mapcount)                       │
 * │    c. Child PTE points to SAME physical page                         │
 * │ 3. On write attempt:                                                  │
 * │    a. Page fault (write to read-only)                                 │
 * │    b. Check: Is VMA writable? If yes: COW situation                  │
 * │    c. Allocate new physical page                                      │
 * │    d. Copy 4096 bytes                                                 │
 * │    e. Update faulting process PTE: new phys, R/W=1                   │
 * │    f. Decrement old page mapcount                                     │
 * │    g. Flush TLB                                                       │
 * │    h. Retry faulting instruction                                      │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * NUMERICAL EXAMPLE: 512 MB PROCESS FORKS
 * ─────────────────────────────────────────────────────────────────────────
 * Without COW:
 * 01. Pages to copy = 512 MB / 4 KB = 131,072 pages
 * 02. Copy time @ 10 GB/s = 512 MB / 10,000 MB/s = 51.2 ms
 * 03. RAM used = 512 + 512 = 1024 MB
 *
 * With COW:
 * 04. Pages to copy = 0 (data pages)
 * 05. PTEs to update ≈ 131,072 × 8 bytes = 1 MB of table entries
 * 06. Fork time ≈ 1 MB / 10,000 MB/s + overhead ≈ 0.1 ms
 * 07. RAM used = 512 MB (shared)
 * 08. SPEEDUP = 51.2 / 0.1 = 512×
 * ─────────────────────────────────────────────────────────────────────────
 *
 * PTE BIT CHANGES:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ Before fork (parent only):                                            │
 * │   PTE = 0x00000003FAE00067                                            │
 * │         ──────────────────                                            │
 * │         0x67 = 0110_0111 = P R/W U A D - - -                          │
 * │                   ↑                                                    │
 * │                   R/W = 1 (writable)                                  │
 * │                                                                        │
 * │ After fork (both parent and child):                                   │
 * │   PTE = 0x00000003FAE00065                                            │
 * │         ──────────────────                                            │
 * │         0x65 = 0110_0101 = P - U A D - - -                            │
 * │                   ↑                                                    │
 * │                   R/W = 0 (read-only, but VMA says writable = COW)    │
 * │                                                                        │
 * │ After child writes (child only):                                      │
 * │   Child PTE = 0x0000000456780067                                      │
 * │               ──────────────────                                      │
 * │               New phys = 0x456780000, R/W = 1                         │
 * │   Parent PTE = 0x00000003FAE00067 (if only user left, restore R/W=1) │
 * └────────────────────────────────────────────────────────────────────────┘
 *
 * PAGE FAULT FLOW:
 * ┌────────────────────────────────────────────────────────────────────────┐
 * │ CPU executes: MOV [0x7FFE5E4ED000], RAX (write instruction)           │
 * │                                                                        │
 * │ 1. TLB lookup: 0x7FFE5E4ED000 → TLB miss or hit                       │
 * │ 2. Page walk: PTE found, P=1, R/W=0                                   │
 * │ 3. Permission check: Write to R/W=0 → FAIL                            │
 * │ 4. Exception #14 (Page Fault):                                        │
 * │    CR2 = 0x7FFE5E4ED000 (faulting address)                            │
 * │    Error code = 0x7:                                                  │
 * │      bit 0 = 1 (P: page was present)                                  │
 * │      bit 1 = 1 (W: write access)                                      │
 * │      bit 2 = 1 (U: user mode)                                         │
 * │                                                                        │
 * │ 5. Kernel handler:                                                     │
 * │    if (vma->vm_flags & VM_WRITE && is_cow_mapping(pte))               │
 * │        do_wp_page(pte, vma);  // COW path                             │
 * │    else                                                               │
 * │        send_sigsegv();  // Real permission violation                  │
 * └────────────────────────────────────────────────────────────────────────┘
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 19: Copy-on-Write");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

static int demo_cow_show(struct seq_file* m, void* v) {
    unsigned long cr3;
    unsigned long* pml4;
    int i, ro_count = 0, rw_count = 0;

    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    pml4 = (unsigned long*)__va(cr3 & PTE_ADDR_MASK);

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 19: COPY-ON-WRITE (COW)\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    /*
     * COW SAVINGS CALCULATION:
     *
     * Given: Parent = 512 MB, fork() called
     *
     * Without COW:
     *   Copy 512 MB data = 131072 pages × 4096 bytes = 536,870,912 bytes
     *   At 10 GB/s: 536,870,912 / 10,737,418,240 = 0.05 sec = 50 ms
     *   RAM: 512 + 512 = 1024 MB
     *
     * With COW:
     *   Copy ~0 bytes (only page table structure)
     *   Update 131072 PTEs × 8 bytes = 1 MB
     *   At 10 GB/s: 1 MB / 10 GB/s = 0.0001 sec = 0.1 ms
     *   RAM: 512 MB (shared)
     *
     * Speedup: 50 / 0.1 = 500×
     * RAM savings: 512 MB
     */
    seq_printf(m, "FORK WITHOUT COW:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Parent: 512 MB data\n");
    seq_printf(m, "  fork() → copy 131072 pages × 4096 bytes\n");
    seq_printf(m, "         = 536,870,912 bytes\n");
    seq_printf(m, "  Time @ 10GB/s: 50 ms\n");
    seq_printf(m, "  Total RAM: 1024 MB\n\n");

    seq_printf(m, "FORK WITH COW:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Parent: 512 MB data\n");
    seq_printf(m, "  fork() → share pages, mark R/W=0\n");
    seq_printf(m, "  Time: ~0.1 ms (update PTEs only)\n");
    seq_printf(m, "  Total RAM: 512 MB (shared)\n");
    seq_printf(m, "  Speedup: 500×\n\n");

    seq_printf(m, "ON WRITE:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  1. Write to shared page → Page fault\n");
    seq_printf(m, "  2. Error code = 0x7 (present, write, user)\n");
    seq_printf(m, "  3. Kernel: check VMA.vm_flags & VM_WRITE\n");
    seq_printf(m, "  4. If true: allocate new page, copy 4096 bytes\n");
    seq_printf(m, "  5. Update PTE: new phys, R/W=1\n");
    seq_printf(m, "  6. Flush TLB, retry write\n\n");

    /*
     * Scan user-space PML4 entries (0-255) for R/W bit distribution
     * This shows how many entries are currently writable vs read-only
     */
    seq_printf(m, "CURRENT PROCESS PML4 R/W ANALYSIS:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    for (i = 0; i < 256; i++) {
        if (pml4[i] & 1) { /* P = 1 */
            if (pml4[i] & 2) {
                rw_count++;
            } else {
                ro_count++;
                /*
                 * R/W=0 entries might be:
                 * 1. COW marked pages
                 * 2. Genuine read-only pages (.text)
                 * 3. Read-only shared library mappings
                 */
            }
        }
    }

    seq_printf(m, "  User PML4[0-255]: %d present\n", ro_count + rw_count);
    seq_printf(m, "  R/W=1 (writable): %d\n", rw_count);
    seq_printf(m, "  R/W=0 (read-only or COW): %d\n\n", ro_count);

    seq_printf(m, "PTE FLAG CHANGE (bit 1 = R/W):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Before fork: 0x67 = 0110_0111 (R/W=1 at bit 1)\n");
    seq_printf(m, "  After fork:  0x65 = 0110_0101 (R/W=0 at bit 1)\n");
    seq_printf(m, "  After write: 0x67 = 0110_0111 (R/W=1, new page)\n");

    return 0;
}

static int demo_cow_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_cow_show, NULL);
}

static const struct proc_ops demo_cow_ops = {
    .proc_open = demo_cow_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_cow_init(void) {
    proc_create("demo_cow", 0444, NULL, &demo_cow_ops);
    pr_info("demo_cow: loaded\n");
    return 0;
}

static void __exit demo_cow_exit(void) { remove_proc_entry("demo_cow", NULL); }

module_init(demo_cow_init);
module_exit(demo_cow_exit);
