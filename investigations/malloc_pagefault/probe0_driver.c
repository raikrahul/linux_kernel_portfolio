
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

/*
 * TASK: PROBE 1 - THE API ENTRY POINT
 * -----------------------------------
 * Target: lock_vma_under_rcu
 * Filtering: (PID == target_pid) && (Address == target_addr)
 *
 * AXIOM 0: SYSTEM V AMD64 ABI REGISTER MAPPING
 * Arg 1 (vma)     -> %rdi
 * Arg 2 (address) -> %rsi
 * Arg 3 (flags)   -> %rdx
 * Arg 4 (regs)    -> %rcx
 *
 * AXIOM 1: TARGET DATA
 * Expected Address: [Determined from userspace VA+0x100]
 * Expected Flags:   0x1255 (WRITE | USER | KILLABLE | VMA_LOCK)
 */

static int target_pid = 0;
module_param(target_pid, int, 0644);

static unsigned long target_addr = 0;
module_param(target_addr, ulong, 0644);

static char symbol_name[64] = "lock_vma_under_rcu";
module_param_string(symbol, symbol_name, sizeof(symbol_name), 0644);

// KPROBE STRUCTURE
static struct kprobe kp = {
    .symbol_name = symbol_name,
};

/*
 * PRE-HANDLER: Triggered BEFORE 'lock_vma_under_rcu' starts.
 * Signature: struct vm_area_struct *lock_vma_under_rcu(struct mm_struct *mm,
 * unsigned long address) Regs: mm (rdi), address (rsi)
 */
static int pre_handler(struct kprobe *p, struct pt_regs *regs) {
  unsigned long address = regs->si;

  // Filter: Only trace our target pid
  if (target_pid != 0 && current->pid != target_pid)
    return 0;

  pr_info("AXIOM_TRACE: START lock_vma_under_rcu\n");
  pr_info("   COMM: %s\n", current->comm);
  pr_info("   TGID: %d | PID: %d\n", current->tgid, current->pid);
  pr_info("   ADDR: 0x%lx\n", address);

  // Check if within target page
  if (target_addr != 0) {
    if ((address & PAGE_MASK) == (target_addr & PAGE_MASK)) {
      pr_info("   MATCH: Target address found!\n");
    }
  }

  return 0;
}

static int __init kprobe_init(void) {
  kp.pre_handler = pre_handler;
  int ret = register_kprobe(&kp);
  if (ret < 0)
    return ret;
  pr_info("Probe 1 planted: %s\n", symbol_name);
  return 0;
}

static void __exit kprobe_exit(void) { unregister_kprobe(&kp); }

module_init(kprobe_init);
module_exit(kprobe_exit);
MODULE_LICENSE("GPL");
