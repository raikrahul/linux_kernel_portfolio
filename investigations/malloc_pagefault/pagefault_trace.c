/* pagefault_trace.c - Kernel module to trace page faults via kprobe
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║ WHAT: Hook page fault handler to see anonymous page allocation           ║
 * ║ WHY: Demonstrate kernel side of malloc → page fault → alloc_page         ║
 * ║ WHERE: Kernel space, hooks handle_mm_fault or do_anonymous_page          ║
 * ║ WHO: Userspace write → CPU fault → kernel handler → this kprobe          ║
 * ║ WHEN: Every page fault triggers this hook                                ║
 * ║ WITHOUT: Without this, page faults invisible from userspace              ║
 * ║ WHICH: Only shows anonymous faults where page->mapping = anon_vma        ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * COMPILE: make
 * LOAD: sudo insmod pagefault_trace.ko target_pid=<PID>
 * VERIFY: sudo dmesg -w
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Page fault tracing via kprobe");

static int target_pid = 0;
module_param(target_pid, int, 0644);
MODULE_PARM_DESC(target_pid, "PID to trace (0 = all)");

static unsigned long fault_count = 0;

/* ══════════════════════════════════════════════════════════════════════════
 * KPROBE HANDLER - Runs before handle_mm_fault()
 *
 * handle_mm_fault signature:
 * vm_fault_t handle_mm_fault(struct vm_area_struct *vma,
 *                            unsigned long address,
 *                            unsigned int flags,
 *                            struct pt_regs *regs)
 *
 * We extract:
 * - address (faulting virtual address)
 * - vma->vm_flags (permissions, type)
 * - current->pid (which process faulted)
 * ══════════════════════════════════════════════════════════════════════════ */
static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
  struct vm_area_struct *vma;
  unsigned long address;
  unsigned int fault_flags;
  pid_t pid;

  /* x86_64 calling convention: rdi, rsi, rdx, rcx, r8, r9 */
  vma = (struct vm_area_struct *)regs->di; /* arg1 */
  address = regs->si;                      /* arg2 */
  fault_flags = (unsigned int)regs->dx;    /* arg3 */

  pid = current->pid;

  /* Filter by PID if specified */
  if (target_pid != 0 && pid != target_pid) {
    return 0;
  }

  fault_count++;

  /* ════════════════════════════════════════════════════════════════════
   * DECODE FAULT TYPE:
   *
   * FAULT_FLAG_WRITE (0x1) → Write access caused fault
   * FAULT_FLAG_USER (0x40) → Fault from userspace
   *
   * VMA FLAGS:
   * VM_READ (0x1), VM_WRITE (0x2), VM_EXEC (0x4)
   * VM_SHARED (0x8) → Shared mapping
   *
   * If vma->vm_file == NULL → Anonymous VMA (heap, stack)
   * If vma->vm_file != NULL → File-backed VMA (mmap file)
   * ════════════════════════════════════════════════════════════════════ */

  if (vma) {
    const char *vma_type;
    unsigned long vm_flags = vma->vm_flags;

    if (vma->vm_file) {
      vma_type = "FILE-BACKED";
    } else {
      vma_type = "ANONYMOUS"; /* This is what malloc uses! */
    }

    printk(KERN_INFO
           "PgFault[%lu]: pid=%d addr=0x%lx flags=0x%x vma=%s vm_flags=0x%lx\n",
           fault_count, pid, address, fault_flags, vma_type, vm_flags);

    /* For anonymous pages (malloc), the kernel will:
     * 1. Call alloc_page(GFP_HIGHUSER_MOVABLE)
     * 2. Set page->mapping = anon_vma | PAGE_MAPPING_ANON
     * 3. Add page to anon_vma list for reverse mapping
     * 4. Set up PTE: vaddr → PFN with user+write bits
     */
    if (!vma->vm_file && (fault_flags & 0x1)) {
      printk(KERN_INFO
             "PgFault[%lu]: → Anonymous WRITE fault → alloc_page() called\n",
             fault_count);
    }
  }

  return 0;
}

static struct kprobe kp = {
    .symbol_name = "handle_mm_fault",
    .pre_handler = handler_pre,
};

static int __init pagefault_trace_init(void) {
  int ret;

  ret = register_kprobe(&kp);
  if (ret < 0) {
    printk(KERN_ERR "PgFaultTrace: register_kprobe failed: %d\n", ret);
    printk(KERN_ERR
           "PgFaultTrace: Try: sudo sysctl -w kernel.kptr_restrict=0\n");
    return ret;
  }

  printk(KERN_INFO "PgFaultTrace: Kprobe at %px (%s)\n", kp.addr,
         kp.symbol_name);
  printk(KERN_INFO "PgFaultTrace: Tracing PID=%d (0=all)\n", target_pid);
  printk(KERN_INFO
         "PgFaultTrace: Run userspace program, watch dmesg for faults.\n");

  return 0;
}

static void __exit pagefault_trace_exit(void) {
  unregister_kprobe(&kp);
  printk(KERN_INFO "PgFaultTrace: Unregistered. Total faults traced: %lu\n",
         fault_count);
}

module_init(pagefault_trace_init);
module_exit(pagefault_trace_exit);
