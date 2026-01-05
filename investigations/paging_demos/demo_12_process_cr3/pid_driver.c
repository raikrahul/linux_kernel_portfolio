/*
 * DEMO 12: READ TARGET PROCESS CR3
 * ═════════════════════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Process ID (PID).
 *    Action: Find `task_struct` -> `mm_struct` -> `pgd` (Page Global Dir).
 *    Output: Physical Address of PML4 (CR3 value).
 *
 *    Computation:
 *    CR3 = virt_to_phys(task->mm->pgd)
 *
 * 2. WHY:
 *    - Memory Isolation.
 *    - Process A CR3 != Process B CR3.
 *    - When CPU runs A, CR3 points to A's Tables.
 *    - A cannot see B's memory because A's tables don't map it.
 *
 * 3. WHERE:
 *    - `task_struct`: Kernel memory (Slab).
 *    - `pgd`: Allocated page in RAM.
 *
 * 4. WHO:
 *    - Scheduler (`__switch_to`):
 *    - Loads new CR3 when switching processes.
 *    - Flushes non-global TLB entries (Demo 18).
 *
 * 5. WHEN:
 *    - Context Switch (thousands of times per second).
 *
 * 6. WITHOUT:
 *    - All processes share same page tables.
 *    - One crash crashes everything.
 *    - Security nightmare (A reads B's password).
 *
 * 7. WHICH:
 *    - Which CR3? The one belonging to `current` or target PID.
 *    - Kernel Threads (mm=NULL) use `mm_users/active_mm`.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Apartment Complex Wi-Fi
 * - Each Apartment (PID) has a Router (CR3).
 * - Router Config (Page Table) maps "192.168.1.1" to specific dev.
 *
 * User A:
 * - CR3 = Apt 101 Router.
 * - "Connect Printer" -> Maps to Printer 101.
 *
 * User B:
 * - CR3 = Apt 102 Router.
 * - "Connect Printer" -> Maps to Printer 102.
 *
 * Numerical:
 * - Virtual Addr = 192.168.1.5 (Same for both).
 * - Physical Addr A = Device #55.
 * - Physical Addr B = Device #99.
 * - Context Switch = Unplugging Router A, plugging in Router B.
 */

#include <asm/io.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 12: Target Process CR3");

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

static int demo_pid_show(struct seq_file* m, void* v) {
    unsigned long my_cr3, my_pml4;
    struct task_struct* task;

    /* Get current CR3 */
    asm volatile("mov %%cr3, %0" : "=r"(my_cr3));
    my_pml4 = my_cr3 & PTE_ADDR_MASK;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 12: PROCESS CR3 COMPARISON\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "CURRENT PROCESS:\n");
    seq_printf(m, "  PID: %d\n", current->pid);
    seq_printf(m, "  COMM: %s\n", current->comm);
    seq_printf(m, "  CR3: 0x%lx\n", my_cr3);
    seq_printf(m, "  PML4_phys: 0x%lx\n\n", my_pml4);

    seq_printf(m, "OTHER PROCESSES:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");

    int count = 0;
    rcu_read_lock();
    for_each_process(task) {
        if (task->mm && count < 10) {
            unsigned long pgd_phys = virt_to_phys(task->mm->pgd);

            seq_printf(m, "PID %6d %-16s PML4=0x%lx\n", task->pid, task->comm, pgd_phys);
            count++;
        }
    }
    rcu_read_unlock();

    seq_printf(m, "\nOBSERVATION AXIOMS:\n");
    seq_printf(m, "  1. CR3 determines validity of ANY virtual address.\n");
    seq_printf(m, "  2. Two PIDs can use VA 0x1000 for different data.\n");
    seq_printf(m, "  3. This is maintained by hardware pointers (CR3).\n");

    return 0;
}

static int demo_pid_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_pid_show, NULL);
}

static const struct proc_ops demo_pid_ops = {
    .proc_open = demo_pid_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_pid_init(void) {
    proc_create("demo_pid", 0444, NULL, &demo_pid_ops);
    pr_info("demo_pid: loaded\n");
    return 0;
}

static void __exit demo_pid_exit(void) { remove_proc_entry("demo_pid", NULL); }

module_init(demo_pid_init);
module_exit(demo_pid_exit);
