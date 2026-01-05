/*
 * DEMO 15: VMA WALK
 * ═════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: `mm_struct` of a Process.
 *    Action: Walk the Linked List (or RB-Tree) of `vm_area_struct`s.
 *    Output: List of Valid Virtual address ranges + Permissions.
 *
 *    Computation:
 *    [Start, End) intervals.
 *    0x400000 - 0x401000: r-x (Code)
 *    0x401000 - 0x402000: r-- (RotData)
 *    0x402000 - 0x403000: rw- (Data)
 *
 * 2. WHY:
 *    - Page Tables show "Mapping to RAM".
 *    - VMAs show "Intent for usage".
 *    - VMA says "This gap is valid stack space".
 *    - Page Table says "It's not mapped yet (Page Fault needed)".
 *
 * 3. WHERE:
 *    - Kernel Memory (likely Slab allocated `vm_area_struct`).
 *    - Pointed to by `mm->mmap`.
 *
 * 4. WHO:
 *    - `mmap()` syscall creates them.
 *    - Page Fault Handler (`do_page_fault`) checks them.
 *      - If Addr in VMA range? Valid fault (Allocate RAM).
 *      - If Addr NOT in VMA range? Invalid fault (Segfault).
 *
 * 5. WHEN:
 *    - Process startup (Execve load segments).
 *    - `malloc` (brk/mmap).
 *    - Dynamic Lib loading (.so).
 *
 * 6. WITHOUT:
 *    - Kernel wouldn't know permissions (RX vs RW).
 *    - Wouldn't know backing file (disk offset).
 *    - Segfaults would be impossible to distinguish from valid demand paging.
 *
 * 7. WHICH:
 *    - Whichever range contains the faulting address.
 *    - Checked via `find_vma()`.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Land Surveyor Claims
 * - Desert (Virtual Space) is infinite.
 * - Claims (VMAs) mark specific rectangles.
 *
 * Claim 1: "Plot A" (0 to 100 meters).
 * - Usage: Cactus Farm (Read-Only).
 *
 * Claim 2: "Plot B" (200 to 300 meters).
 * - Usage: Mine (Read-Write).
 *
 * Event:
 * - Digging at meter 50? -> Police check Claim 1. "Farm only!" -> Arrest (Segfault).
 * - Digging at meter 150? -> No Claim found. -> Arrest (Segfault).
 * - Digging at meter 250? -> Claim 2 found. "Mine". -> Allowed (Resolve Fault).
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 15: VMA Walk");

static int demo_vma_show(struct seq_file* m, void* v) {
    struct mm_struct* mm;
    struct vm_area_struct* vma;
    int count = 0;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 15: VMA WALK (current process)\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    mm = current->mm;
    if (!mm) {
        seq_printf(m, "No mm_struct (kernel thread)\n");
        return 0;
    }

    seq_printf(m, "PID: %d  COMM: %s\n", current->pid, current->comm);
    seq_printf(m, "total_vm: %lu pages = %lu KB\n", mm->total_vm, mm->total_vm * 4);
    seq_printf(m, "stack_vm: %lu pages\n", mm->stack_vm);
    seq_printf(m, "data_vm: %lu pages\n\n", mm->data_vm);

    seq_printf(m, "VMAs (first 20):\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "%-18s %-18s %6s %s\n", "START", "END", "SIZE", "PERM");

    mmap_read_lock(mm);

    VMA_ITERATOR(vmi, mm, 0);
    for_each_vma(vmi, vma) {
        if (count >= 20) break;

        unsigned long size = vma->vm_end - vma->vm_start;
        char perm[5] = "----";

        if (vma->vm_flags & VM_READ) perm[0] = 'r';
        if (vma->vm_flags & VM_WRITE) perm[1] = 'w';
        if (vma->vm_flags & VM_EXEC) perm[2] = 'x';
        if (vma->vm_flags & VM_SHARED)
            perm[3] = 's';
        else
            perm[3] = 'p';

        seq_printf(m, "0x%016lx 0x%016lx %6lu KB %s\n", vma->vm_start, vma->vm_end, size / 1024,
                   perm);
        count++;
    }

    mmap_read_unlock(mm);

    seq_printf(m, "\nTotal VMAs displayed: %d\n", count);

    return 0;
}

static int demo_vma_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_vma_show, NULL);
}

static const struct proc_ops demo_vma_ops = {
    .proc_open = demo_vma_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_vma_init(void) {
    proc_create("demo_vma", 0444, NULL, &demo_vma_ops);
    pr_info("demo_vma: loaded\n");
    return 0;
}

static void __exit demo_vma_exit(void) { remove_proc_entry("demo_vma", NULL); }

module_init(demo_vma_init);
module_exit(demo_vma_exit);
